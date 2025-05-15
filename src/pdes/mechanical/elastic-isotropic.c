/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox elastic isotropic mechanical material
 *
 *  Copyright (C) 2021-2022 Jeremy Theler
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

int feenox_mechanical_material_init_linear_elastic(material_t *material, int i) {
  // TODO: for lambda & mu 
  return (mechanical.E.defined_per_group[i] && mechanical.nu.defined_per_group[i]) ? material_model_elastic_isotropic : material_model_unknown;
}

void feenox_problem_mechanical_compute_lambda_mu(const double *x, material_t *material, double *lambda, double *mu) {
  double E = mechanical.E.eval(&mechanical.E, x, material);
  double nu = mechanical.nu.eval(&mechanical.nu, x, material);
  *lambda = E*nu/((1+nu)*(1-2*nu));
  *mu = 0.5*E/(1+nu);
  return;
}

int feenox_problem_mechanical_compute_C_elastic_isotropic(const double *x, material_t *material) {
  double lambda, mu;
  feenox_problem_mechanical_compute_lambda_mu(x, material, &lambda, &mu);
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

// compute the seconde Piola-Kirchoff stress tensor
int feenox_problem_mechanical_compute_stress_first_piola_kirchoff(void) {
  // first piola-kirchoff
  double J = feenox_fem_determinant(mechanical.F);
  feenox_fem_matrix_invert(mechanical.F, mechanical.invF);
  feenox_blas_ABt(mechanical.cauchy, mechanical.invF, J, mechanical.PK);
  
  
  return FEENOX_OK;
}

// compute the seconde Piola-Kirchoff stress tensor
int feenox_problem_mechanical_compute_stress_second_piola_kirchoff_elastic_isotropic(const double *x, material_t *material) {
  
  double lambda, mu;
  feenox_problem_mechanical_compute_lambda_mu(x, material, &lambda, &mu);
  
  gsl_matrix_set_identity(mechanical.PK);
  double trE = gsl_matrix_get(mechanical.epsilon, 0, 0) + gsl_matrix_get(mechanical.epsilon, 1, 1) + gsl_matrix_get(mechanical.epsilon, 2, 2);
  gsl_matrix_scale(mechanical.PK, lambda * trE);
  
  gsl_matrix_set_zero(mechanical.twomuE);
  gsl_matrix_memcpy(mechanical.twomuE, mechanical.epsilon);
  gsl_matrix_scale(mechanical.twomuE, 2*mu);
  gsl_matrix_add(mechanical.PK, mechanical.twomuE);
    
  return FEENOX_OK;
}

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
  gsl_matrix_memcpy(mechanical.cauchy, mechanical.LCG);
  gsl_matrix_sub(mechanical.cauchy, mechanical.eye);
  gsl_matrix_scale(mechanical.cauchy, mu/J);
    
  // volumetric
  gsl_matrix *volumetric = gsl_matrix_calloc(3, 3);
  gsl_matrix_memcpy(volumetric, mechanical.eye);
  gsl_matrix_scale(volumetric, lambda/J * log(J));
    
  // cauchy
  gsl_matrix_add(mechanical.cauchy, volumetric);
  gsl_matrix_free(volumetric);
    
  return FEENOX_OK;
}

int feenox_stress_from_strain_elastic_isotropic(node_t *node, element_t *element, unsigned int j,
    double epsilonx, double epsilony, double epsilonz, double gammaxy, double gammayz, double gammazx,
    double *sigmax, double *sigmay, double *sigmaz, double *tauxy, double *tauyz, double *tauzx) {

  double lambda, mu;
  feenox_problem_mechanical_compute_lambda_mu(node->x, element->physical_group->material, &lambda, &mu);
  
  double lambda_trE = lambda*(epsilonx + epsilony + epsilonz);
  double two_mu = 2*mu;

  // TODO: separate
  if (mechanical.variant == variant_full || mechanical.variant == variant_plane_strain) {
    // normal stresses
    *sigmax = lambda_trE + two_mu * epsilonx;
    *sigmay = lambda_trE + two_mu * epsilony;
    *sigmaz = lambda_trE + two_mu * epsilonz;
  
    // shear stresses
    *tauxy = mu * gammaxy;
    if (feenox.pde.dofs == 3) {
      *tauyz = mu * gammayz;
      *tauzx = mu * gammazx;
    }
    
  } else if (mechanical.variant == variant_plane_stress) {
    
      double E = mu*(3*lambda + 2*mu)/(lambda+mu);
      double nu = lambda / (2*(lambda+mu));
    
      double c1 = E/(1-nu*nu);
      double c2 = nu * c1;

      *sigmax = c1 * epsilonx + c2 * epsilony;
      *sigmay = c2 * epsilonx + c1 * epsilony;
      *sigmaz = 0;
      *tauxy = c1*0.5*(1-nu) * gammaxy;
    
  }
  
  return FEENOX_OK;
}



int feenox_problem_build_mechanical_stress_measure_linear_elastic(const double *x, material_t *material) {
  // green-lagrange strain
  feenox_call(feenox_problem_mechanical_compute_strain_green_lagrange(mechanical.grad_u));
  // second piola kirchoff stress tensor
  feenox_call(feenox_problem_mechanical_compute_stress_second_piola_kirchoff_elastic_isotropic(x, material));

  return FEENOX_OK;
}
