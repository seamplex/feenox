/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for Laplace's equation: bulk elements
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
#include "laplace.h"

int feenox_problem_build_volumetric_gauss_point_laplace(element_t *e, unsigned int q) {

#ifdef HAVE_PETSC
  
  double wdet = feenox_mesh_compute_w_det_at_gauss(e, q, feenox.pde.mesh->integration);
//  gsl_matrix *B = feenox_mesh_compute_B_G_at_gauss(e, q, feenox.pde.mesh->integration);
  gsl_matrix *B = feenox_mesh_compute_B_at_gauss(e, q, feenox.pde.mesh->integration);
  
  // laplace stiffness matrix Bt*B
  feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, wdet, B, B, 1.0, feenox.pde.Ki));

  material_t *material = feenox_mesh_get_material(e);
  
  // right-hand side
  double *x = feenox_mesh_compute_x_at_gauss_if_needed(e, q, laplace.space_dependent_source || laplace.space_dependent_mass);
  if (laplace.f.defined) {
    double f = laplace.f.eval(&laplace.f, x, material);
    feenox_call(feenox_problem_rhs_set(e, q, &f));
  }
  
  if (feenox.pde.has_jacobian) {
    // TODO: jacobian
  }
    
  // mass matrix Ht*rho*cp*H
  if (feenox.pde.has_mass) {
    gsl_matrix *H = feenox_mesh_compute_H_Gc_at_gauss(e->type, q, feenox.pde.mesh->integration);
    double alpha = 0;
    if (laplace.alpha.defined) {
      alpha = laplace.alpha.eval(&laplace.alpha, x, material);
    } else {
      // this should have been already checked
      feenox_push_error_message("no alpha found");
      return FEENOX_ERROR;
    }
    feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, wdet * alpha, H, H, 1.0, feenox.pde.Mi));
  }
  

#endif
  
  return FEENOX_OK;
  
}
