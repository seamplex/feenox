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
#include "../mechanical.h"

int feenox_mechanical_material_init_neohookean(material_t *material, int i) {
  // TO-DO: mu and K
  return (mechanical.E.defined_per_group[i] && mechanical.nu.defined_per_group[i]) ? material_model_hyperelastic_neohookean : material_model_unknown;
}


int feenox_problem_build_mechanical_stress_measure_neohookean(const gsl_matrix *grad_u, const double *x, material_t *material) {

  mechanical.epsilon = mechanical.epsilon_cauchy_green;
  mechanical.S = mechanical.cauchy;
  
  feenox_call(mechanical.compute_C(x, material));
    
  feenox_call(feenox_problem_mechanical_compute_deformation_gradient(grad_u));
  // RCG = Ft * F   right cauchy-green tensor
  feenox_blas_BtB(mechanical.F, 1.0, mechanical.epsilon_cauchy_green);
    
  // cauchy stress
  feenox_call(feenox_problem_mechanical_compute_stress_cauchy_neohookean(x, material));  

  // second piola kirchoff
  feenox_call(feenox_problem_mechanical_compute_stress_second_piola_kirchoff_neohookean(x, material));
  
  return FEENOX_OK;
}

// TODO:
// int feenox_problem_mechanical_compute_C_hyperelastic_neohookean(const double *x, material_t *material);

int feenox_problem_mechanical_compute_stress_cauchy_neohookean(const double *x, material_t *material) {
  // volume change    
  double J = feenox_fem_determinant(mechanical.F);
    
  // neo-hookean cauchy stress
  // sigma = (mu/J) * (b - I) + lambda/J * log(J) * I
    
  double lambda, mu;
  feenox_problem_mechanical_compute_lambda_mu(x, material, &lambda, &mu);
    
  // deviatoric
  gsl_matrix_memcpy(mechanical.cauchy, mechanical.epsilon);
  gsl_matrix_sub(mechanical.cauchy, mechanical.eye);
  gsl_matrix_scale(mechanical.cauchy, mu/J);
    
  // volumetric
  gsl_matrix_memcpy(mechanical.tmp, mechanical.eye);
  gsl_matrix_scale(mechanical.tmp, lambda/J * log(J));
    
  // cauchy
  gsl_matrix_add(mechanical.cauchy, mechanical.tmp);
    
  return FEENOX_OK;
}

int feenox_problem_mechanical_compute_stress_second_piola_kirchoff_neohookean(const double *x, material_t *material) {
  // second piola-kirchoff
  double J = feenox_fem_determinant(mechanical.F);
  double J23 = pow(J, -2.0/3.0); // Isochoric scaling
  
  gsl_matrix *C = mechanical.epsilon_cauchy_green;
  double trC = gsl_matrix_get(C, 0, 0) + gsl_matrix_get(C, 1, 1) + gsl_matrix_get(C, 2, 2);

  gsl_matrix *C_inv = mechanical.invC;
  feenox_fem_matrix_invert(C, C_inv);
    
  double lambda, mu;
  feenox_problem_mechanical_compute_lambda_mu(x, material, &lambda, &mu);
    
  // Deviatoric term
  gsl_matrix_set_identity(mechanical.PK2);
  
  gsl_matrix_memcpy(mechanical.tmp, C_inv);
  gsl_matrix_scale(mechanical.tmp, trC / 3.0);
  gsl_matrix_sub(mechanical.PK2, mechanical.tmp);
  gsl_matrix_scale(mechanical.PK2, mu * J23);
    
  // Volumetric term
  gsl_matrix_scale(C_inv, (lambda + 2.0/3.0 * mu) * (J - 1) * J);
  gsl_matrix_add(mechanical.PK2, C_inv);  

  return FEENOX_OK;
}
