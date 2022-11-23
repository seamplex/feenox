/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for the heat equation: bulk elements
 *
 *  Copyright (C) 2021 jeremy theler
 *
 *  This file is part of FeenoX <https://www.seamplex.com/feenox>.
 *
 *  feenox is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  feenox is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with feenox.  If not, see <http://www.gnu.org/licenses/>.
 *------------------- ------------  ----    --------  --     -       -         -
 */
#include "feenox.h"
#include "thermal.h"

int feenox_problem_build_volumetric_gauss_point_thermal(element_t *e, unsigned int v) {

#ifdef HAVE_PETSC
  
  feenox_call(feenox_mesh_compute_wHB_at_gauss(e, v));
  
  double *x = NULL;
  if (thermal.space_dependent_stiffness || thermal.space_dependent_source || thermal.space_dependent_mass) {
    feenox_call(feenox_mesh_compute_x_at_gauss(e, v, feenox.pde.mesh->integration));
    x = e->x[v];
  }
  
  // TODO: axisymmetric
//  r_for_axisymmetric = feenox_compute_r_for_axisymmetric(this, v);
  double r_for_axisymmetric = 1;
  double w = e->w[v] * r_for_axisymmetric;
  
  material_t *material = NULL;
  if (e->physical_group != NULL && e->physical_group->material != NULL) {
    material = e->physical_group->material;
  }

  // thermal stiffness matrix Bt*k*B
  double k = thermal.k.eval(&thermal.k, x, material);
  feenox_call(feenox_matTmatmult_accum(w*k, e->B[v], e->B[v], feenox.pde.Ki));
  
  // volumetric heat source term Ht*q
  // TODO: total source Q
  // TODO: there should be a way to use BLAS instead of a for loop
  if (thermal.q.defined) {
    double q = thermal.q.eval(&thermal.q, x, material);
    // TODO: H*q as BLAS
    for (int j = 0; j < e->type->nodes; j++) {
      feenox_lowlevel_vector_accum(feenox.pde.bi, j, w * e->type->gauss[feenox.pde.mesh->integration].h[v][j] * q);
    }
  }
  
  if (feenox.pde.has_jacobian) {
    // TODO: hint the interpolation with the current element
    double T = feenox_function_eval(feenox.pde.solution[0], x);
    
    // TODO: this is not working as expected
    if (thermal.temperature_dependent_stiffness) {
      // TODO: this might not work if the distribution is not given as an
      //       algebraic expression but as a pointwise function of T
      //       (but it works if using a property!)
      double dkdT = feenox_expression_derivative_wrt_function(thermal.k.expr, feenox.pde.solution[0], T);
      feenox_call(feenox_matTmatmult_accum(w*dkdT*T, e->B[v], e->B[v], feenox.pde.JKi));      
    }

    if (thermal.temperature_dependent_source) {
      double dqdT = feenox_expression_derivative_wrt_function(thermal.q.expr, feenox.pde.solution[0], T);
      // mind the positive sign!
      feenox_call(feenox_matTmatmult_accum(w*dqdT, e->H[v], e->H[v], feenox.pde.Jbi));
    }  
  }
    
  // mass matrix Ht*rho*cp*H
  if (feenox.pde.has_mass) {
    double rhocp = 0;
    if (thermal.rhocp.defined) {
      rhocp = thermal.rhocp.eval(&thermal.rhocp, x, material);
    } else if (thermal.kappa.defined) {
      rhocp = k / thermal.kappa.eval(&thermal.kappa, x, material);
    } else if (thermal.rho.defined && thermal.cp.defined) {
      rhocp = thermal.rho.eval(&thermal.rho, x, material) * thermal.cp.eval(&thermal.cp, x, material);
    } else {
      // this should have been already checked
      feenox_push_error_message("no heat capacity found");
      return FEENOX_ERROR;
    }
    feenox_call(feenox_matTmatmult_accum(w * rhocp, e->H[v], e->H[v], feenox.pde.Mi));
  }
  

#endif
  
  return FEENOX_OK;
  
}
