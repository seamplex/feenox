/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox routines to compute stresses
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

// this works for both 2d and 3d
#define FLUX_SIGMAX  0
#define FLUX_SIGMAY  1
#define FLUX_TAUXY   2
#define FLUX_SIGMAZ  3
#define FLUX_TAUYZ   4
#define FLUX_TAUZX   5

int feenox_problem_gradient_fill_mechanical(void) {
  
  feenox_gradient_fill(mechanical, sigmax);
  feenox_gradient_fill(mechanical, sigmay);
  feenox_gradient_fill(mechanical, tauxy);
  if (feenox.pde.dofs == 3) {
    feenox_gradient_fill(mechanical, sigmaz);
    feenox_gradient_fill(mechanical, tauyz);
    feenox_gradient_fill(mechanical, tauzx);
  }
  
  if (mechanical.sigma1->used) {
    feenox_gradient_fill(mechanical, sigma1);
  }
  if (mechanical.sigma2->used) {
    feenox_gradient_fill(mechanical, sigma2);
  }
  if (mechanical.sigma3->used) {
    feenox_gradient_fill(mechanical, sigma3);
  }
  if (mechanical.sigma->used) {
    feenox_gradient_fill(mechanical, sigma);
  }
  if (mechanical.tresca->used) {
    feenox_gradient_fill(mechanical, tresca);
  }
  
  return FEENOX_OK;
  
}

// used only in rough
int feenox_problem_gradient_properties_at_element_nodes_mechanical(element_t *element, mesh_t *mesh) {

  
  
  
  return FEENOX_OK;
}

int feenox_problem_gradient_fluxes_at_node_alloc_mechanical(node_t *node) {
  
  size_t flux_size = (feenox.pde.dim == 3) ? 6 : 3;
  if (node->flux == NULL) {
    feenox_check_alloc(node->flux = calloc(flux_size, sizeof(double)));
  } else {
    unsigned int m = 0;
    for (m = 0; m < flux_size; m++) {
      node->flux[m] = 0;
    }
  }
  
  return FEENOX_OK;
}

int feenox_problem_gradient_add_elemental_contribution_to_node_mechanical(node_t *node, element_t *element, unsigned int j, double rel_weight) {

  // TODO: cache properties
  // TODO: orthotropic
  // TODO: check what has to be computed and what not
  double E  = mechanical.E.eval(&mechanical.E, node->x, element->physical_group->material);
  double nu = mechanical.nu.eval(&mechanical.nu, node->x, element->physical_group->material);
  double lambda = E*nu/((1+nu)*(1-2*nu));
  double mu = 0.5*E/(1+nu);
  
  double ex = gsl_matrix_get(element->dphidx_node[j], 0, 0);
  double ey = gsl_matrix_get(element->dphidx_node[j], 1, 1);
  double ez = 0;
  // TODO: axisymmetric
  if (feenox.pde.dofs == 3) {
    ez = gsl_matrix_get(element->dphidx_node[j], 2, 2);
  }  
  
  double gammaxy = gsl_matrix_get(element->dphidx_node[j], 0, 1) + gsl_matrix_get(element->dphidx_node[j], 1, 0);
  double gammayz = 0;
  double gammazx = 0;
  if (feenox.pde.dofs == 3) {
    gammayz = gsl_matrix_get(element->dphidx_node[j], 1, 2) + gsl_matrix_get(element->dphidx_node[j], 2, 1);
    gammazx = gsl_matrix_get(element->dphidx_node[j], 2, 0) + gsl_matrix_get(element->dphidx_node[j], 0, 2);
  }  
  
  
  // TODO: virtual
  double lambda_div = lambda*(ex + ey + ez);
  double two_mu = two_mu = 2*mu;
  double sigmax = 0;
  double sigmay = 0;
  double sigmaz = 0;
  double tauxy = 0;
  double tauyz = 0;
  double tauzx = 0;
  
  if (mechanical.variant == variant_full || mechanical.variant == variant_plane_strain) {
    // normal stresses
    sigmax = lambda_div + two_mu * ex;
    sigmay = lambda_div + two_mu * ey;
    sigmaz = lambda_div + two_mu * ez;
  
    // shear stresses
    tauxy = mu * gammaxy;
    if (feenox.pde.dofs == 3) {
      tauyz = mu * gammayz;
      tauzx = mu * gammazx;
    }
  } else if (mechanical.variant == variant_plane_stress) {
    
      double E = mu*(3*lambda + 2*mu)/(lambda+mu);
      double nu = lambda / (2*(lambda+mu));
    
      double c1 = E/(1-nu*nu);
      double c2 = nu * c1;

      sigmax = c1 * ex + c2 * ey;
      sigmay = c2 * ex + c1 * ey;
      tauxy = c1*0.5*(1-nu) * gammaxy;
    
  }

  // subtract the thermal contribution to the normal stresses (see IFEM.Ch30)
  if (mechanical.alpha.defined) {
    double alpha = mechanical.alpha.eval(&mechanical.alpha, node->x, element->physical_group->material);
    if (alpha != 0) {
      double DT = mechanical.T.eval(&mechanical.T, node->x, element->physical_group->material) - mechanical.T0;
      double thermal_stress = E/(1-2*nu) * alpha * DT;
    
      sigmax -= thermal_stress;
      sigmay -= thermal_stress;
      if (feenox.pde.dim == 3) {
        sigmaz -= thermal_stress;
      }  
    }
  }
  
  node->flux[FLUX_SIGMAX] += rel_weight * (sigmax - node->flux[FLUX_SIGMAX]);
  node->flux[FLUX_SIGMAY] += rel_weight * (sigmay - node->flux[FLUX_SIGMAY]);
  node->flux[FLUX_TAUXY] += rel_weight * (tauxy - node->flux[FLUX_TAUXY]);
  if (feenox.pde.dofs == 3) {
    node->flux[FLUX_SIGMAZ] += rel_weight * (sigmaz - node->flux[FLUX_SIGMAZ]);
    node->flux[FLUX_TAUYZ] += rel_weight * (tauyz - node->flux[FLUX_TAUYZ]);
    node->flux[FLUX_TAUZX] += rel_weight * (tauzx - node->flux[FLUX_TAUZX]);
  }  
  
  return FEENOX_OK;
}

int feenox_problem_gradient_fill_fluxes_mechanical(mesh_t *mesh, size_t j) {
  
  mechanical.sigmax->data_value[j] = mesh->node[j].flux[FLUX_SIGMAX];
  mechanical.sigmay->data_value[j] = mesh->node[j].flux[FLUX_SIGMAY];
  mechanical.tauxy->data_value[j] = mesh->node[j].flux[FLUX_TAUXY];
  if (feenox.pde.dofs == 3) {
    mechanical.sigmaz->data_value[j] = mesh->node[j].flux[FLUX_SIGMAZ];
    mechanical.tauyz->data_value[j] = mesh->node[j].flux[FLUX_TAUYZ];
    mechanical.tauzx->data_value[j] = mesh->node[j].flux[FLUX_TAUZX];
  }

  if ((mechanical.sigma1 != NULL && mechanical.sigma1->used) ||
      (mechanical.sigma2 != NULL && mechanical.sigma2->used) ||
      (mechanical.sigma3 != NULL && mechanical.sigma3->used) ||
      (mechanical.tresca != NULL && mechanical.tresca->used)) {
    
    feenox_principal_stress_compute(mesh->node[j].flux[FLUX_SIGMAX],
                                    mesh->node[j].flux[FLUX_SIGMAY],
                                    mesh->node[j].flux[FLUX_SIGMAZ],
                                    mesh->node[j].flux[FLUX_TAUXY],
                                    mesh->node[j].flux[FLUX_TAUYZ],
                                    mesh->node[j].flux[FLUX_TAUZX],
                                    &mechanical.sigma1->data_value[j],
                                    &mechanical.sigma2->data_value[j],
                                    &mechanical.sigma3->data_value[j]);
    
    if (mechanical.sigma->used) {
      mechanical.sigma->data_value[j] = feenox_vonmises_from_principal(mechanical.sigma1->data_value[j],
                                                                       mechanical.sigma2->data_value[j],
                                                                       mechanical.sigma3->data_value[j]);
    }
    if (mechanical.tresca->used) {
      mechanical.tresca->data_value[j] = mechanical.sigma1->data_value[j] - mechanical.sigma3->data_value[j];
    }
  } else if (mechanical.sigma->used) {
    
    mechanical.sigma->data_value[j] = feenox_vonmises_from_stress_tensor(mesh->node[j].flux[FLUX_SIGMAX],
                                                                         mesh->node[j].flux[FLUX_SIGMAY],
                                                                         mesh->node[j].flux[FLUX_SIGMAZ],
                                                                         mesh->node[j].flux[FLUX_TAUXY],
                                                                         mesh->node[j].flux[FLUX_TAUYZ],
                                                                         mesh->node[j].flux[FLUX_TAUZX]);    
    
  }
  
  // TODO: uncertainties
  
  return FEENOX_OK;
  
}



int feenox_principal_stress_compute(double sigmax, double sigmay, double sigmaz, double tauxy, double tauyz, double tauzx, double *sigma1, double *sigma2, double *sigma3) {
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

double feenox_vonmises_from_principal(double sigma1, double sigma2, double sigma3) {
  
  return sqrt(0.5*(gsl_pow_2(sigma1-sigma2) + gsl_pow_2(sigma2-sigma3) + gsl_pow_2(sigma3-sigma1)));
  
}

double feenox_vonmises_from_stress_tensor(double sigmax, double sigmay, double sigmaz, double tauxy, double tauyz, double tauzx) {
  
  return sqrt(0.5*(gsl_pow_2(sigmax-sigmay) + gsl_pow_2(sigmay-sigmaz) + gsl_pow_2(sigmaz-sigmax) +
                       6.0 * (gsl_pow_2(tauxy) + gsl_pow_2(tauyz) + gsl_pow_2(tauzx))));
  
}
