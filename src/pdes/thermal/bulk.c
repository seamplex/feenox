/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for the heat equation: bulk elements
 *
 *  Copyright (C) 2021--2023 jeremy theler
 *
 *  This file is part of FeenoX <https://www.seamplex.com/feenox>.
 *
 *  feenox is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  FeenoX is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with FeenoX.  If not, see <http://www.gnu.org/licenses/>.
 *------------------- ------------  ----    --------  --     -       -         -
 */
#include "feenox.h"
#include "thermal.h"

// TODO: put the gauss loop inside this call so we can cache number of nodes, etc
int feenox_problem_build_volumetric_gauss_point_thermal(element_t *e, unsigned int q) {

#ifdef HAVE_PETSC
  
  double wdet = feenox_fem_compute_w_det_at_gauss(e, q, feenox.pde.mesh->integration);
  gsl_matrix *B = feenox_fem_compute_B_at_gauss(e, q, feenox.pde.mesh->integration);

  double *x = feenox_fem_compute_x_at_gauss_if_needed(e, q, thermal.space_dependent_stiffness || thermal.space_dependent_source || thermal.space_dependent_mass);
  material_t *material = feenox_fem_get_material(e);  
  double k = thermal.k.eval(&thermal.k, x, material);
  
  feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, wdet*k, B, B, 1.0, feenox.fem.Ki));
 
  // volumetric heat source term Ht*q
  // TODO: total source Q
  if (thermal.q.defined) {
    double power = thermal.q.eval(&thermal.q, x, material);
    feenox_call(feenox_problem_rhs_set(e, q, &power));
  }
    
  if (feenox.pde.has_jacobian) {
    gsl_matrix *local_vec_T = NULL;
    unsigned int nodes = e->type->nodes;
    feenox_check_alloc(local_vec_T = gsl_matrix_calloc(nodes, 1));
    double T = 0;
    double Tj = 0;
    gsl_matrix *H = feenox_fem_compute_H_Gc_at_gauss(e->type, q, feenox.pde.mesh->integration);
    for (unsigned int j = 0; j < nodes; j++) {
      Tj = feenox_vector_get(feenox.pde.solution[0]->vector_value, e->node[j]->index_dof[0]);
      gsl_matrix_set(local_vec_T, j, 0, Tj);
      T += gsl_matrix_get(H, 0, j) * Tj; 
    }  

    if (thermal.temperature_dependent_stiffness) {
      double dkdT = feenox_expression_derivative_wrt_function(thermal.k.expr, feenox.pde.solution[0], T);
      gsl_matrix *TH = gsl_matrix_calloc(nodes, nodes);
      gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1, local_vec_T, H, 1, TH);

      // add a convenience function, mind the allocation
      gsl_matrix *BtB = gsl_matrix_calloc(nodes, nodes);
      gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1, B, B, 1, BtB);

      gsl_matrix *BtBTH = gsl_matrix_calloc(nodes, nodes);
      feenox_call(gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1, BtB, TH, 1, BtBTH));
      feenox_call(gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, wdet*dkdT, BtB, TH, 1, feenox.fem.JKi));
    }

    if (thermal.temperature_dependent_source) {
      double dqdT = feenox_expression_derivative_wrt_function(thermal.q.expr, feenox.pde.solution[0], T);
      // mind the positive sign!
      feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, wdet*dqdT, H, H, 1.0, feenox.fem.Jbi));
    } 
    gsl_matrix_free(local_vec_T);
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
    gsl_matrix *H = feenox_fem_compute_H_Gc_at_gauss(e->type, q, feenox.pde.mesh->integration);
//    gsl_matrix *H = e->type->H_Gc[q];
    feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, wdet * rhocp, H, H, 1.0, feenox.fem.Mi));
  }
  

#endif
  
  return FEENOX_OK;
  
}
