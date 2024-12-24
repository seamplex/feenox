/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for Laplace's equation: bulk elements
 *
 *  Copyright (C) 2021--2023 Jeremy Theler
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
  
  double wdet = feenox_fem_compute_w_det_at_gauss(e, q);
  gsl_matrix *B = feenox_fem_compute_B_at_gauss(e, q);
  
  // laplace stiffness matrix Ki += wdet * Bt*B
  feenox_call(feenox_blas_BtB_accum(B, wdet, feenox.fem.Ki));
  
  // the material is needed for either RHS and/or mass
  material_t *material = feenox_fem_get_material(e);
  
  // right-hand side
  double *x = feenox_fem_compute_x_at_gauss_if_needed(e, q, feenox.pde.mesh->integration, laplace.space_dependent_source || laplace.space_dependent_mass);
  if (laplace.f.defined) {
    double f = laplace.f.eval(&laplace.f, x, material);
    feenox_call(feenox_problem_rhs_add(e, q, &f));
  }
  
  if (feenox.pde.has_jacobian) {
    // TODO: jacobian
  }
  
  // mass matrix Ht*rho*cp*H
  if (feenox.pde.has_mass) {
    gsl_matrix *H_Gc = feenox_fem_compute_H_Gc_at_gauss(e, q, feenox.pde.mesh->integration);
    if (laplace.alpha.defined) {
      double alpha = laplace.alpha.eval(&laplace.alpha, x, material);
      feenox_call(feenox_blas_BtB_accum(H_Gc, wdet*alpha, feenox.fem.Mi));
    } else {
      // this should have been already checked
      feenox_push_error_message("no alpha found needed for Laplace's mass matrix");
      return FEENOX_ERROR;
    }
  }

#endif
  
  return FEENOX_OK;
  
}
