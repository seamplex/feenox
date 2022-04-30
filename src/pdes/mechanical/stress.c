/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox routines to compute stresses
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
  
  // if one is used, all of them are
  if (mechanical.sigma1->used || mechanical.sigma2->used || mechanical.sigma3->used) {
    feenox_gradient_fill(mechanical, sigma1);
    feenox_gradient_fill(mechanical, sigma2);
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

// TODO: why both node and j?
int feenox_problem_gradient_add_elemental_contribution_to_node_mechanical(node_t *node, element_t *element, unsigned int j, double rel_weight) {

  // read strains from gradient
  double epsilonx = gsl_matrix_get(element->dphidx_node[j], 0, 0);
  double epsilony = gsl_matrix_get(element->dphidx_node[j], 1, 1);
  double epsilonz = (feenox.pde.dofs == 3) ? gsl_matrix_get(element->dphidx_node[j], 2, 2) : 0;
  
  double gammaxy = gsl_matrix_get(element->dphidx_node[j], 0, 1) + gsl_matrix_get(element->dphidx_node[j], 1, 0);
  double gammayz = 0;
  double gammazx = 0;
  if (feenox.pde.dofs == 3) {
    gammayz = gsl_matrix_get(element->dphidx_node[j], 1, 2) + gsl_matrix_get(element->dphidx_node[j], 2, 1);
    gammazx = gsl_matrix_get(element->dphidx_node[j], 2, 0) + gsl_matrix_get(element->dphidx_node[j], 0, 2);
  }  

  double sigmax = 0;
  double sigmay = 0;
  double sigmaz = 0;
  double tauxy = 0;
  double tauyz = 0;
  double tauzx = 0;

  // compute stresses
  feenox_call(mechanical.compute_stress_from_strain(node, element, j,
                                                    epsilonx, epsilony, epsilonz, gammaxy, gammayz, gammazx,
                                                    &sigmax, &sigmay, &sigmaz, &tauxy, &tauyz, &tauzx));
  
  if (mechanical.thermal_expansion_model != thermal_expansion_model_none) {
    // subtract the thermal contribution to the normal stresses (see IFEM.Ch30)
    double sigmat_x = 0;
    double sigmat_y = 0;
    double sigmat_z = 0;
    
    feenox_call(mechanical.compute_thermal_stress(node->x, element->physical_group->material, &sigmat_x, &sigmat_y, &sigmat_z));

    sigmax -= sigmat_x;
    sigmay -= sigmat_y;
    sigmaz -= sigmat_z;
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
    
    feenox_principal_stress_from_cauchy(mesh->node[j].flux[FLUX_SIGMAX],
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
    mechanical.compute_C(node->x, (element->physical_group != NULL) ? element->physical_group->material : NULL);
  }  
  
  *sigmax = gsl_matrix_get(mechanical.C, 0, 0) * epsilonx + gsl_matrix_get(mechanical.C, 0, 1) * epsilony + gsl_matrix_get(mechanical.C, 0, 2) * epsilonz;
  *sigmay = gsl_matrix_get(mechanical.C, 1, 0) * epsilonx + gsl_matrix_get(mechanical.C, 1, 1) * epsilony + gsl_matrix_get(mechanical.C, 1, 2) * epsilonz;
  *sigmaz = gsl_matrix_get(mechanical.C, 2, 0) * epsilonx + gsl_matrix_get(mechanical.C, 2, 1) * epsilony + gsl_matrix_get(mechanical.C, 2, 2) * epsilonz;
  *tauxy = gsl_matrix_get(mechanical.C, 3, 3) * gammaxy;
  *tauyz = gsl_matrix_get(mechanical.C, 4, 4) * gammayz;
  *tauzx = gsl_matrix_get(mechanical.C, 5, 5) * gammazx;
  
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

