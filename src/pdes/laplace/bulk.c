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
  
  feenox_call(feenox_mesh_compute_wHB_at_gauss(e, q));
  double *x = feenox_mesh_compute_x_if_needed(e, q, laplace.space_dependent_source || laplace.space_dependent_mass);
  
  // TODO: axisymmetric
//  r_for_axisymmetric = feenox_compute_r_for_axisymmetric(this, v);
//  double r_for_axisymmetric = 1;
  double w = e->w[q];
  
  // laplace stiffness matrix Bt*B
  // note we don't allow any coefficient in the laplacian term
  feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, w, e->B[q], e->B[q], 1.0, feenox.pde.Ki));

  material_t *material = feenox_mesh_get_material(e);
  
  // right-hand side
  if (laplace.f.defined) {
    gsl_vector_set(laplace.vec_f, 0, laplace.f.eval(&laplace.f, x, material));
    feenox_call(gsl_blas_dgemv(CblasTrans, w, e->H[q], laplace.vec_f, 1.0, feenox.pde.bi));
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
    feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, w * alpha, e->H[q], e->H[q], 1.0, feenox.pde.Mi));
  }
  

#endif
  
  return FEENOX_OK;
  
}
