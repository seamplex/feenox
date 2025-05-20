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


int feenox_problem_build_mechanical_stress_measure_neohookean(const gsl_matrix *grad_u, const double *x, material_t *material) {
  
  mechanical.epsilon = mechanical.epsilon_cauchy_green;
  mechanical.PK = mechanical.PK2;
  mechanical.S = mechanical.cauchy;
  
  feenox_call(mechanical.compute_C(x, material));
    
  feenox_call(feenox_problem_mechanical_compute_deformation_gradient(grad_u));
  // RCG = Ft * F   right cauchy-green tensor
  feenox_blas_BtB(mechanical.F, 1.0, mechanical.epsilon_cauchy_green);
    
  // cauchy stress
  feenox_call(feenox_problem_mechanical_compute_stress_cauchy_neohookean(x, material));  

  // first piola kirchoff
  feenox_call(feenox_problem_mechanical_compute_stress_second_piola_kirchoff_neohookean(x, material));
  
  return FEENOX_OK;
}

// TODO:
// int feenox_problem_mechanical_compute_C_hyperelastic_neohookean(const double *x, material_t *material);

int feenox_problem_mechanical_compute_stress_cauchy_neohookean(const double *x, material_t *material) {
  // volume change    
  double J = feenox_fem_determinant(mechanical.F);
    
  // invariant
/* 
  double I1 = gsl_matrix_get(mechanical.epsilon_cauchy_green, 0, 0) +
              gsl_matrix_get(mechanical.epsilon_cauchy_green, 1, 1) +
              gsl_matrix_get(mechanical.epsilon_cauchy_green, 2, 2);
*/
    
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

// compute the first Piola-Kirchoff stress tensor
// P = mu * J**(-2/3) * (F - (1/3)*I1*FinvT) + lambda*J*(J-1)*FinvT
// S = mu * J**(-2/3) * (I - (1/3)*trace(C) * inv_C)  + lambda * (J - 1.0) * J * inv_C;
/*
int feenox_problem_mechanical_compute_stress_first_piola_kirchoff_neohookean(const double *x, material_t *material) {
  // first piola-kirchoff
  double J = feenox_fem_determinant(mechanical.F);
  feenox_fem_matrix_invert(mechanical.F, mechanical.invF);
  
  gsl_matrix *Ft = gsl_matrix_alloc(3,3);
  gsl_matrix_transpose_memcpy(Ft, mechanical.F);
  
  gsl_matrix *invFt = gsl_matrix_alloc(3,3);
  feenox_fem_matrix_invert(Ft, invFt);
  
  
  double lambda, mu;
  feenox_problem_mechanical_compute_lambda_mu(x, material, &lambda, &mu);

  gsl_matrix_memcpy(mechanical.PK1, mechanical.F);
  
  gsl_matrix *tmp = gsl_matrix_alloc(3,3);
  gsl_matrix_memcpy(tmp, invFt);
  
  double I1 = gsl_matrix_get(mechanical.epsilon_cauchy_green, 0, 0) +
              gsl_matrix_get(mechanical.epsilon_cauchy_green, 1, 1) +
              gsl_matrix_get(mechanical.epsilon_cauchy_green, 2, 2);

  gsl_matrix_scale(tmp, 1.0/3.0*I1);
  gsl_matrix_sub(mechanical.PK1, tmp);
  gsl_matrix_scale(mechanical.PK1, mu * pow(J, -2.0/3.0));
  
  gsl_matrix_memcpy(mechanical.volumetric, invFt);
  gsl_matrix_scale(mechanical.volumetric, lambda * J * (J-1));
  
  // PK1
  gsl_matrix_add(mechanical.PK1, mechanical.volumetric);  
  
  return FEENOX_OK;
}
*/

int feenox_problem_mechanical_compute_stress_second_piola_kirchoff_neohookean(const double *x, material_t *material) {
  // second piola-kirchoff
  double J = feenox_fem_determinant(mechanical.F);
//  feenox_fem_matrix_invert(mechanical.F, mechanical.invF);
  
  gsl_matrix *C = mechanical.epsilon_cauchy_green;
  
  gsl_matrix *invC = gsl_matrix_alloc(3,3);
  feenox_fem_matrix_invert(C, invC);
  
  double trC = gsl_matrix_get(C, 0, 0) +
               gsl_matrix_get(C, 1, 1) +
               gsl_matrix_get(C, 2, 2);
  
  double lambda, mu;
  feenox_problem_mechanical_compute_lambda_mu(x, material, &lambda, &mu);
  
  // S = μ J^(-2/3) (I - (1/3)tr(C)C⁻¹) + λ (J-1) J C⁻¹
  gsl_matrix_set_identity(mechanical.PK2);
  
  gsl_matrix_memcpy(mechanical.volumetric, invC);
  gsl_matrix_scale(mechanical.volumetric, 1.0/3.0 * trC);
  
  gsl_matrix_sub(mechanical.PK2, mechanical.volumetric);
  gsl_matrix_scale(mechanical.PK2, mu * pow(J, -2.0/3.0));  
   
  gsl_matrix_scale(invC, lambda * (J - 1) * J);
  gsl_matrix_add(mechanical.PK2, invC);
  
  return FEENOX_OK;
}
