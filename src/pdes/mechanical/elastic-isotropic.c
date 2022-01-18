/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox elastic isotropic mechanical material
 *
 *  Copyright (C) 2021-2022 jeremy theler
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
