/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox thermal isotropic expansion mechanical material
 *
 *  Copyright (C) 2022 Jeremy Theler
 *
 *  This file is part of Feenox <https://www.seamplex.com/feenox>.
 *
 *  feenox is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Feenox is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Feenox.  If not, see <http://www.gnu.org/licenses/>.
 *------------------- ------------  ----    --------  --     -       -         -
 */
#include "feenox.h"
#include "mechanical.h"

int feenox_problem_build_compute_mechanical_strain_isotropic (const double *x, material_t *material) {
  
  double delta_T = mechanical.T.eval(&mechanical.T, x, material) - mechanical.T0;
  double alpha = mechanical.alpha.eval(&mechanical.alpha, x, material);
  double alpha_delta_T = alpha * delta_T;
  
  gsl_vector_set(mechanical.et, 0, alpha_delta_T);
  gsl_vector_set(mechanical.et, 1, alpha_delta_T);
  if (feenox.pde.dim > 2) {
    gsl_vector_set(mechanical.et, 2, alpha_delta_T);
  }
  
  return FEENOX_OK;
  
}

int feenox_problem_build_compute_mechanical_stress_isotropic (const double *x, material_t *material, double *sigmat_x, double *sigmat_y, double *sigmat_z) {

  double delta_T = mechanical.T.eval(&mechanical.T, x, material) - mechanical.T0;
  double alpha = mechanical.alpha.eval(&mechanical.alpha, x, material);
  double alpha_delta_T = alpha * delta_T;
  
  // if the material is isotropic but non uniform, we do not have the proper C matrix
  if (mechanical.uniform_C == 0 && mechanical.material_model == material_model_elastic_isotropic) {
    mechanical.compute_C(x, material);
  }

  *sigmat_x = alpha_delta_T * (gsl_matrix_get(mechanical.C, 0, 0) + gsl_matrix_get(mechanical.C, 0, 1) + gsl_matrix_get(mechanical.C, 0, 2));
  *sigmat_y = alpha_delta_T * (gsl_matrix_get(mechanical.C, 1, 0) + gsl_matrix_get(mechanical.C, 1, 1) + gsl_matrix_get(mechanical.C, 1, 2));
  if (feenox.pde.dofs > 2) {
    *sigmat_z = alpha_delta_T * (gsl_matrix_get(mechanical.C, 2, 0) + gsl_matrix_get(mechanical.C, 2, 1) + gsl_matrix_get(mechanical.C, 2, 2));
  }  
  
  return FEENOX_OK;
  
}
