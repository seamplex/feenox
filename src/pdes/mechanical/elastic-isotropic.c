/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox elastic isotropic mechanical material
 *
 *  Copyright (C) 2015-2022 jeremy theler
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
extern feenox_t feenox;
extern mechanical_t mechanical;

int feenox_problem_build_compute_mechanical_C_elastic_isotropic(const double *x, material_t *material) {
  
  double E = mechanical.E.eval(&mechanical.E, x, material);
  double nu = mechanical.nu.eval(&mechanical.nu, x, material);
  double lambda = E*nu/((1+nu)*(1-2*nu));
  double mu = 0.5*E/(1+nu);
  double lambda2mu = lambda + 2*mu;
  
  gsl_matrix_set(mechanical.C, 0, 0, lambda2mu);
  gsl_matrix_set(mechanical.C, 0, 1, lambda);
  gsl_matrix_set(mechanical.C, 0, 2, lambda);

  gsl_matrix_set(mechanical.C, 1, 0, lambda);
  gsl_matrix_set(mechanical.C, 1, 1, lambda2mu);
  gsl_matrix_set(mechanical.C, 1, 2, lambda);

  gsl_matrix_set(mechanical.C, 2, 0, lambda);
  gsl_matrix_set(mechanical.C, 2, 1, lambda);
  gsl_matrix_set(mechanical.C, 2, 2, lambda2mu);
  
  gsl_matrix_set(mechanical.C, 3, 3, mu);
  gsl_matrix_set(mechanical.C, 4, 4, mu);
  gsl_matrix_set(mechanical.C, 5, 5, mu);
    
  return FEENOX_OK;
}
