/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox routines to compute strains
 *
 *  Copyright (C) 2025 Jeremy Theler
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

gsl_matrix *feenox_problem_mechanical_compute_deformation_gradient(const gsl_matrix *grad_u) {
  // deformation gradient
  // F = I + grad_u
  gsl_matrix_memcpy(mechanical.F, mechanical.eye);
  gsl_matrix_add(mechanical.F, grad_u);

  return mechanical.F;
}
gsl_matrix *feenox_problem_mechanical_compute_strain_cauchy_green_left(const gsl_matrix *F) {
  
  // left cauchy-green tensor
  // C = Ft * F   
  feenox_blas_BtB(F, 1.0, mechanical.C);

  return mechanical.C;
}

gsl_matrix *feenox_problem_mechanical_compute_strain_green_lagrange(const gsl_matrix *C) {
  
  // green-lagrange strain tensor
  // E = 1/2 * (C - I)   
  gsl_matrix_memcpy(mechanical.eps, C);
  gsl_matrix_sub(mechanical.eps, mechanical.eye);
  gsl_matrix_scale(mechanical.eps, 0.5);
  
  return mechanical.eps;
}
