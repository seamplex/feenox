/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox neohookean mechanical material
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

int feenox_mechanical_material_init_neohookean(material_t *material, int i) {
  // TO-DO: mu and K
  return (mechanical.E.defined_per_group[i] && mechanical.nu.defined_per_group[i]) ? material_model_hyperelastic_neohookean : material_model_unknown;
}


// TODO: ask for grad_u as parameter
/*
int feenox_problem_build_mechanical_stress_measure_neohookean(const double *x, material_t *material) {
  
  return FEENOX_OK;
}
*/
// TODO:
// int feenox_problem_mechanical_compute_C_hyperelastic_neohookean(const double *x, material_t *material);

int feenox_problem_mechanical_compute_stress_cauchy_neohookean(const double *x, material_t *material) {
  // volume change    
  double J = feenox_fem_determinant(mechanical.F);
    
  // invariant
//  double I1 = gsl_matrix_get(mechanical.LCG, 0, 0) + gsl_matrix_get(mechanical.LCG, 1, 1) + gsl_matrix_get(mechanical.LCG, 1, 1);
    
  // neo-hookean cauchy stress
  // sigma = (mu/J) * (b - I) + lambda/J * log(J) * I
    
  double lambda, mu;
  feenox_problem_mechanical_compute_lambda_mu(x, material, &lambda, &mu);
    
  // deviatoric
  gsl_matrix_memcpy(mechanical.cauchy, mechanical.epsilon);
  gsl_matrix_sub(mechanical.cauchy, mechanical.eye);
  gsl_matrix_scale(mechanical.cauchy, mu/J);
    
  // volumetric
  gsl_matrix_memcpy(mechanical.volumetric, mechanical.eye);
  gsl_matrix_scale(mechanical.volumetric, lambda/J * log(J));
    
  // cauchy
  gsl_matrix_add(mechanical.cauchy, mechanical.volumetric);
    
  return FEENOX_OK;
}
