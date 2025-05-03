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

void feenox_problem_mechanical_compute_lambda_mu(const double *x, material_t *material, double *lambda, double *mu) {
  double E = mechanical.E.eval(&mechanical.E, x, material);
  double nu = mechanical.nu.eval(&mechanical.nu, x, material);
  *lambda = E*nu/((1+nu)*(1-2*nu));
  *mu = 0.5*E/(1+nu);
  return;
}

int feenox_problem_build_compute_mechanical_C_elastic_isotropic(const double *x, material_t *material) {
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
int feenox_problem_build_compute_mechanical_S_elastic_isotropic(const double *x, material_t *material) {
  
  double lambda, mu;
  feenox_problem_mechanical_compute_lambda_mu(x, material, &lambda, &mu);
  
  gsl_matrix_set_identity(mechanical.S);
  double trE = gsl_matrix_get(mechanical.EGL, 0, 0) + gsl_matrix_get(mechanical.EGL, 1, 1) + gsl_matrix_get(mechanical.EGL, 2, 2);
  gsl_matrix_scale(mechanical.S, lambda * trE);
  
  gsl_matrix_set_zero(mechanical.twomuE);
  gsl_matrix_memcpy(mechanical.twomuE, mechanical.EGL);
  gsl_matrix_scale(mechanical.twomuE, 2*mu);
  gsl_matrix_add(mechanical.S, mechanical.twomuE);

  // S in voigt notation
  double Sxx = gsl_matrix_get(mechanical.S, 0, 0);
  double Syy = gsl_matrix_get(mechanical.S, 1, 1);
  double Szz = gsl_matrix_get(mechanical.S, 2, 2);
  double Sxy = gsl_matrix_get(mechanical.S, 0, 1);
  double Syz = gsl_matrix_get(mechanical.S, 1, 2);
  double Szx = gsl_matrix_get(mechanical.S, 2, 0);

  gsl_vector_set(mechanical.S_voigt, 0, Sxx);
  gsl_vector_set(mechanical.S_voigt, 1, Syy);
  gsl_vector_set(mechanical.S_voigt, 2, Szz);
  gsl_vector_set(mechanical.S_voigt, 3, Sxy);
  gsl_vector_set(mechanical.S_voigt, 4, Syz);
  gsl_vector_set(mechanical.S_voigt, 5, Szx);

  // the 9x9 Sigma matrix
  // row 1  
  gsl_matrix_set(mechanical.Sigma, 0, 0, Sxx);
  gsl_matrix_set(mechanical.Sigma, 1, 1, Sxx);
  gsl_matrix_set(mechanical.Sigma, 2, 2, Sxx);
      
  gsl_matrix_set(mechanical.Sigma, 0, 3, Sxy);
  gsl_matrix_set(mechanical.Sigma, 1, 4, Sxy);
  gsl_matrix_set(mechanical.Sigma, 2, 5, Sxy);
      
  gsl_matrix_set(mechanical.Sigma, 0, 6, Szx);
  gsl_matrix_set(mechanical.Sigma, 1, 7, Szx);
  gsl_matrix_set(mechanical.Sigma, 2, 8, Szx);
      
  // row 2
  gsl_matrix_set(mechanical.Sigma, 3, 0, Sxy);
  gsl_matrix_set(mechanical.Sigma, 4, 1, Sxy);
  gsl_matrix_set(mechanical.Sigma, 5, 2, Sxy);
      
  gsl_matrix_set(mechanical.Sigma, 3, 3, Syy);
  gsl_matrix_set(mechanical.Sigma, 4, 4, Syy);
  gsl_matrix_set(mechanical.Sigma, 5, 5, Syy);
      
  gsl_matrix_set(mechanical.Sigma, 3, 6, Syz);
  gsl_matrix_set(mechanical.Sigma, 4, 7, Syz);
  gsl_matrix_set(mechanical.Sigma, 5, 8, Syz);
      
  // row 3
  gsl_matrix_set(mechanical.Sigma, 6, 0, Szx);
  gsl_matrix_set(mechanical.Sigma, 7, 1, Szx);
  gsl_matrix_set(mechanical.Sigma, 8, 2, Szx);
      
  gsl_matrix_set(mechanical.Sigma, 6, 3, Syz);
  gsl_matrix_set(mechanical.Sigma, 7, 4, Syz);
  gsl_matrix_set(mechanical.Sigma, 8, 5, Syz);
      
  gsl_matrix_set(mechanical.Sigma, 6, 6, Szz);
  gsl_matrix_set(mechanical.Sigma, 7, 7, Szz);
  gsl_matrix_set(mechanical.Sigma, 8, 8, Szz);
  
    
  return FEENOX_OK;
}


int feenox_stress_from_strain_elastic_isotropic(node_t *node, element_t *element, unsigned int j,
    double epsilonx, double epsilony, double epsilonz, double gammaxy, double gammayz, double gammazx,
    double *sigmax, double *sigmay, double *sigmaz, double *tauxy, double *tauyz, double *tauzx) {

  // TODO: cache properties
  // TODO: check what has to be computed and what not
  double E  = mechanical.E.eval(&mechanical.E, node->x, element->physical_group->material);
  double nu = mechanical.nu.eval(&mechanical.nu, node->x, element->physical_group->material);
  double lambda = E*nu/((1+nu)*(1-2*nu));
  double mu = 0.5*E/(1+nu);
  
  double lambda_div = lambda*(epsilonx + epsilony + epsilonz);
  double two_mu = two_mu = 2*mu;

  // TODO: separate
  if (mechanical.variant == variant_full || mechanical.variant == variant_plane_strain) {
    // normal stresses
    *sigmax = lambda_div + two_mu * epsilonx;
    *sigmay = lambda_div + two_mu * epsilony;
    *sigmaz = lambda_div + two_mu * epsilonz;
  
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
