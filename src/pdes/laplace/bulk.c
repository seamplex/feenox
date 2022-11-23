/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for Laplace's equation: bulk elements
 *
 *  Copyright (C) 2021--2022 jeremy theler
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
#include "laplace.h"

int feenox_problem_build_volumetric_gauss_point_laplace(element_t *e, unsigned int v) {

#ifdef HAVE_PETSC
  
  feenox_call(feenox_mesh_compute_wHB_at_gauss(e, v));
  
  double *x = NULL;
  if (laplace.space_dependent_source || laplace.space_dependent_mass) {
    feenox_call(feenox_mesh_compute_x_at_gauss(e, v, feenox.pde.mesh->integration));
    x = e->x[v];
  }
  
  // TODO: axisymmetric
//  r_for_axisymmetric = feenox_compute_r_for_axisymmetric(this, v);
  double r_for_axisymmetric = 1;
  double w = e->w[v] * r_for_axisymmetric;
  
  // laplace stiffness matrix Bt*B
  // note we don't allow any coefficient in the laplacian term
  feenox_call(feenox_matTmatmult_accum(w, e->B[v], e->B[v], feenox.pde.Ki));

  material_t *material = NULL;
  if (e->physical_group != NULL && e->physical_group->material != NULL) {
    material = e->physical_group->material;
  }
  
  // right-hand side
  // TODO: there should be a way to use BLAS instead of a for loop
  if (laplace.f.defined) {
    double f = laplace.f.eval(&laplace.f, x, material);
    unsigned int j = 0;
    for (j = 0; j < e->type->nodes; j++) {
      feenox_lowlevel_vector_accum(feenox.pde.bi, j, w * e->type->gauss[feenox.pde.mesh->integration].h[v][j] * f);
    }
  }
  
  if (feenox.pde.has_jacobian) {
    // TODO: jacobian
  }
    
  // mass matrix Ht*rho*cp*H
  if (feenox.pde.has_mass) {
    double alpha = 0;
    if (laplace.alpha.defined) {
      alpha = laplace.alpha.eval(&laplace.alpha, x, material);
    } else {
      // this should have been already checked
      feenox_push_error_message("no alpha found");
      return FEENOX_ERROR;
    }
    feenox_call(feenox_matTmatmult_accum(w * alpha, e->H[v], e->H[v], feenox.pde.Mi));
  }
  

#endif
  
  return FEENOX_OK;
  
}
