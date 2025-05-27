/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox routines to compute stresses
 *
 *  Copyright (C) 2021-2025 Jeremy Theler
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

int feenox_principal_stress_from_cauchy(double sigmax, double sigmay, double sigmaz, double tauxy, double tauyz, double tauzx, double *sigma1, double *sigma2, double *sigma3) {
  // stress invariants
  // https://en.wikiversity.org/wiki/Principal_stresses
  double I1 = sigmax + sigmay + sigmaz;
  double I2 = sigmax*sigmay + sigmay*sigmaz + sigmaz*sigmax - gsl_pow_2(tauxy) - gsl_pow_2(tauyz) - gsl_pow_2(tauzx);
  double I3 = sigmax*sigmay*sigmaz - sigmax*gsl_pow_2(tauyz) - sigmay*gsl_pow_2(tauzx) - sigmaz*gsl_pow_2(tauxy) + 2*tauxy*tauyz*tauzx;

  // principal stresses
  double c1 = sqrt(fabs(gsl_pow_2(I1) - 3*I2));
  double phi = 1.0/3.0 * acos((2.0*gsl_pow_3(I1) - 9.0*I1*I2 + 27.0*I3)/(2.0*gsl_pow_3(c1)));
  if (isnan(phi)) {
    phi = 0;
  }

  double c2 = I1/3.0;
  double c3 = 2.0/3.0 * c1;
  if (sigma1 != NULL) {
    *sigma1 = c2 + c3 * cos(phi);
  }
  if (sigma2 != NULL) {
    *sigma2 = c2 + c3 * cos(phi - 2.0*M_PI/3.0);
  }
  if (sigma3 != NULL) {
    *sigma3 = c2 + c3 * cos(phi - 4.0*M_PI/3.0);
  }
  
  return FEENOX_OK;
  
}

int feenox_stress_from_strain(node_t *node, element_t *element, unsigned int j,
    double epsilonx, double epsilony, double epsilonz, double gammaxy, double gammayz, double gammazx,
    double *sigmax, double *sigmay, double *sigmaz, double *tauxy, double *tauyz, double *tauzx) {
  
  if (mechanical.uniform_C == 0) {
    mechanical.compute_material_tangent(node->x, (element->physical_group != NULL) ? element->physical_group->material : NULL);
  }  
  
  *sigmax = gsl_matrix_get(mechanical.C_tangent, 0, 0) * epsilonx + gsl_matrix_get(mechanical.C_tangent, 0, 1) * epsilony + gsl_matrix_get(mechanical.C_tangent, 0, 2) * epsilonz;
  *sigmay = gsl_matrix_get(mechanical.C_tangent, 1, 0) * epsilonx + gsl_matrix_get(mechanical.C_tangent, 1, 1) * epsilony + gsl_matrix_get(mechanical.C_tangent, 1, 2) * epsilonz;
  *sigmaz = gsl_matrix_get(mechanical.C_tangent, 2, 0) * epsilonx + gsl_matrix_get(mechanical.C_tangent, 2, 1) * epsilony + gsl_matrix_get(mechanical.C_tangent, 2, 2) * epsilonz;
  *tauxy = gsl_matrix_get(mechanical.C_tangent, 3, 3) * gammaxy;
  *tauyz = gsl_matrix_get(mechanical.C_tangent, 4, 4) * gammayz;
  *tauzx = gsl_matrix_get(mechanical.C_tangent, 5, 5) * gammazx;
  
  return FEENOX_OK;
}


double feenox_vonmises_from_principal(double sigma1, double sigma2, double sigma3) {
  
  return sqrt(0.5*(gsl_pow_2(sigma1-sigma2) + gsl_pow_2(sigma2-sigma3) + gsl_pow_2(sigma3-sigma1)));
  
}

double feenox_vonmises_from_stress_tensor(double sigmax, double sigmay, double sigmaz, double tauxy, double tauyz, double tauzx) {
  
  return sqrt(0.5*(gsl_pow_2(sigmax-sigmay) + gsl_pow_2(sigmay-sigmaz) + gsl_pow_2(sigmaz-sigmax) +
                       6.0 * (gsl_pow_2(tauxy) + gsl_pow_2(tauyz) + gsl_pow_2(tauzx))));
  
}


double feenox_tresca_from_stress_tensor(double sigmax, double sigmay, double sigmaz, double tauxy, double tauyz, double tauzx) {
  
  double sigma1 = 0;
  double sigma3 = 0;
  feenox_call(feenox_principal_stress_from_cauchy(sigmax, sigmay, sigmaz, tauxy, tauyz, tauzx, &sigma1, NULL, &sigma3));
  
  return fabs(sigma1 - sigma3);  
}

int feenox_strain_energy(void) {
#ifdef HAVE_PETSC

  Vec Kphi = NULL;
  petsc_call(VecDuplicate(feenox.pde.phi, &Kphi));
  petsc_call(MatMult(feenox.pde.K, feenox.pde.phi, Kphi));
  PetscScalar e = 0;
  petsc_call(VecDot(feenox.pde.phi, Kphi, &e));
  petsc_call(VecDestroy(&Kphi));
  feenox_var_value(mechanical.strain_energy) = 0.5*e;

/*  
  if (fino.problem_kind == problem_kind_axisymmetric) {
    wasora_var(fino.vars.strain_energy) *= 2*M_PI;
  }
*/
  if (feenox_var_value(feenox_special_var(in_static))) {
    if ((int)(feenox_var_value(feenox_special_var(step_static))) == 1) {
      *mechanical.strain_energy->initial_static = *mechanical.strain_energy->value;
    }
    *mechanical.strain_energy->initial_transient = *mechanical.strain_energy->value;
  }
#endif  
  return FEENOX_OK;
}
