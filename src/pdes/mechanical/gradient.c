/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox routines to compute gradient-realated fields
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

#define FLUX_SIGMAX  0
#define FLUX_SIGMAY  1
#define FLUX_SIGMAZ  2
#define FLUX_TAUXY   3
#define FLUX_TAUYZ   4
#define FLUX_TAUZX   5
#define FLUX_EXX     6
#define FLUX_EYY     7
#define FLUX_EZZ     8
#define FLUX_EXY     9
#define FLUX_EYZ    10
#define FLUX_EZX    11
#define FLUX_SIZE   12

int feenox_problem_gradient_fill_mechanical(void) {

  feenox_problem_fill_aux_solution(mechanical.exx);
  feenox_problem_fill_aux_solution(mechanical.eyy);
  feenox_problem_fill_aux_solution(mechanical.exy);
  if (feenox.pde.dofs == 3) {
    feenox_problem_fill_aux_solution(mechanical.ezz);
    feenox_problem_fill_aux_solution(mechanical.eyz);
    feenox_problem_fill_aux_solution(mechanical.ezx);
  }
  
  feenox_problem_fill_aux_solution(mechanical.sigmax);
  feenox_problem_fill_aux_solution(mechanical.sigmay);
  feenox_problem_fill_aux_solution(mechanical.tauxy);
  if (feenox.pde.dofs == 3) {
    feenox_problem_fill_aux_solution(mechanical.sigmaz);
    feenox_problem_fill_aux_solution(mechanical.tauyz);
    feenox_problem_fill_aux_solution(mechanical.tauzx);
  }
  
  // if one is used, all of them are
  if (mechanical.sigma1->used || mechanical.sigma2->used || mechanical.sigma3->used) {
    feenox_problem_fill_aux_solution(mechanical.sigma1);
    feenox_problem_fill_aux_solution(mechanical.sigma2);
    feenox_problem_fill_aux_solution(mechanical.sigma3);
  }
  if (mechanical.sigma->used) {
    feenox_problem_fill_aux_solution(mechanical.sigma);
  }
  if (mechanical.tresca->used) {
    feenox_problem_fill_aux_solution(mechanical.tresca);
  }
  
  return FEENOX_OK;
  
}

// used only in rough
int feenox_problem_gradient_properties_at_element_nodes_mechanical(element_t *element, mesh_t *mesh) {

  
  
  
  return FEENOX_OK;
}

int feenox_problem_gradient_fluxes_at_node_alloc_mechanical(node_t *node) {
  
  size_t flux_size = 12;
  if (node->flux == NULL) {
    feenox_check_alloc(node->flux = calloc(FLUX_SIZE, sizeof(double)));
  } else {
    for (unsigned int m = 0; m < flux_size; m++) {
      node->flux[m] = 0;
    }
  }
  
  return FEENOX_OK;
}

int feenox_problem_gradient_add_elemental_contribution_to_node_mechanical(node_t *node, element_t *element, unsigned int j, double rel_weight) {

  double exx = 0;
  double eyy = 0;
  double ezz = 0;
  double exy = 0;
  double eyz = 0;
  double ezx = 0;

  double sigmax = 0;
  double sigmay = 0;
  double sigmaz = 0;
  double tauxy = 0;
  double tauyz = 0;
  double tauzx = 0;
  
  
  // TODO: virtual methods in the material ctx
  // TODO: use an integer flag for ldef
  if (feenox_var_value(mechanical.ldef) == 0) {
    exx = gsl_matrix_get(element->dphidx_node[j], 0, 0);
    eyy = gsl_matrix_get(element->dphidx_node[j], 1, 1);
    ezz = (feenox.pde.dofs == 3) ? gsl_matrix_get(element->dphidx_node[j], 2, 2) : 0;
  
    double gammaxy = gsl_matrix_get(element->dphidx_node[j], 0, 1) + gsl_matrix_get(element->dphidx_node[j], 1, 0);
    double gammayz = 0;
    double gammazx = 0;
    if (feenox.pde.dofs == 3) {
      gammayz = gsl_matrix_get(element->dphidx_node[j], 1, 2) + gsl_matrix_get(element->dphidx_node[j], 2, 1);
      gammazx = gsl_matrix_get(element->dphidx_node[j], 2, 0) + gsl_matrix_get(element->dphidx_node[j], 0, 2);
    }
    exy = 0.5 * gammaxy;
    eyz = 0.5 * gammayz;
    ezx = 0.5 * gammazx;
    
    feenox_call(mechanical.compute_stress_from_strain(node, element, j,
                                                    exx, eyy, ezz, gammaxy, gammayz, gammazx,
                                                    &sigmax, &sigmay, &sigmaz, &tauxy, &tauyz, &tauzx));
    
    
    
  } else {

    if (mechanical.material_model == material_model_elastic_isotropic) {
      // linear elastic
      // green-lagrange strain  
      feenox_call(feenox_problem_mechanical_compute_strain_green_lagrange(element->dphidx_node[j]));  

      exx = gsl_matrix_get(mechanical.epsilon, 0, 0);
      eyy = gsl_matrix_get(mechanical.epsilon, 1, 1);
      ezz = gsl_matrix_get(mechanical.epsilon, 2, 2);
      exy = gsl_matrix_get(mechanical.epsilon, 0, 1);
      eyz = gsl_matrix_get(mechanical.epsilon, 1, 2);
      ezx = gsl_matrix_get(mechanical.epsilon, 2, 0);

      // second piola kirchoff    
      feenox_call(feenox_problem_mechanical_compute_stress_second_piola_kirchoff_elastic(node->x, element->physical_group->material));

      // cauchy stress  
      double J = feenox_fem_determinant(mechanical.F);
      feenox_call(feenox_blas_BtCB(mechanical.F, mechanical.PK, mechanical.SF, 1/J, mechanical.cauchy));
  
      sigmax = gsl_matrix_get(mechanical.cauchy, 0, 0);
      sigmay = gsl_matrix_get(mechanical.cauchy, 1, 1);
      sigmaz = gsl_matrix_get(mechanical.cauchy, 2, 2);
      tauxy = gsl_matrix_get(mechanical.cauchy, 0, 1);
      tauyz = gsl_matrix_get(mechanical.cauchy, 1, 2);
      tauzx = gsl_matrix_get(mechanical.cauchy, 2, 0);
      
    } else if (mechanical.material_model == material_model_hyperelastic_neohookean) {
      // neo-hookean
      // left cauchy green
      feenox_call(feenox_problem_mechanical_compute_left_cauchy_green(element->dphidx_node[j]));  

      exx = gsl_matrix_get(mechanical.epsilon, 0, 0);
      eyy = gsl_matrix_get(mechanical.epsilon, 1, 1);
      ezz = gsl_matrix_get(mechanical.epsilon, 2, 2);
      exy = gsl_matrix_get(mechanical.epsilon, 0, 1);
      eyz = gsl_matrix_get(mechanical.epsilon, 1, 2);
      ezx = gsl_matrix_get(mechanical.epsilon, 2, 0);

      // second piola kirchoff    
      feenox_call(feenox_problem_mechanical_compute_stress_cauchy_neohookean(node->x, element->physical_group->material));
  
      sigmax = gsl_matrix_get(mechanical.cauchy, 0, 0);
      sigmay = gsl_matrix_get(mechanical.cauchy, 1, 1);
      sigmaz = gsl_matrix_get(mechanical.cauchy, 2, 2);
      tauxy = gsl_matrix_get(mechanical.cauchy, 0, 1);
      tauyz = gsl_matrix_get(mechanical.cauchy, 1, 2);
      tauzx = gsl_matrix_get(mechanical.cauchy, 2, 0);
    }
  }

  
  if (mechanical.thermal_expansion_model != thermal_expansion_model_none) {
    // subtract the thermal contribution to the normal stresses (see IFEM.Ch30)
    double sigmax_thermal = 0;
    double sigmay_thermal = 0;
    double sigmaz_thermal = 0;
    
    feenox_call(mechanical.compute_thermal_stress(node->x, element->physical_group->material, &sigmax_thermal, &sigmay_thermal, &sigmaz_thermal));

    sigmax -= sigmax_thermal;
    sigmay -= sigmay_thermal;
    sigmaz -= sigmaz_thermal;
  }

  node->flux[FLUX_EXX] += rel_weight * (exx - node->flux[FLUX_EXX]);
  node->flux[FLUX_EYY] += rel_weight * (eyy - node->flux[FLUX_EYY]);
  node->flux[FLUX_EXY] += rel_weight * (exy - node->flux[FLUX_EXY]);
  if (feenox.pde.dofs == 3) {
    node->flux[FLUX_EZZ] += rel_weight * (ezz - node->flux[FLUX_EZZ]);
    node->flux[FLUX_EYZ] += rel_weight * (eyz - node->flux[FLUX_EYZ]);
    node->flux[FLUX_EZX] += rel_weight * (ezx - node->flux[FLUX_EZX]);
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

  feenox_vector_set(mechanical.exx->vector_value, j, mesh->node[j].flux[FLUX_EXX]);
  feenox_vector_set(mechanical.eyy->vector_value, j, mesh->node[j].flux[FLUX_EYY]);
  feenox_vector_set(mechanical.exy->vector_value, j, mesh->node[j].flux[FLUX_EXY]);
  if (feenox.pde.dofs == 3) {
    feenox_vector_set(mechanical.ezz->vector_value, j, mesh->node[j].flux[FLUX_EZZ]);
    feenox_vector_set(mechanical.eyz->vector_value, j, mesh->node[j].flux[FLUX_EYZ]);
    feenox_vector_set(mechanical.ezx->vector_value, j, mesh->node[j].flux[FLUX_EZX]);
  }
  
  feenox_vector_set(mechanical.sigmax->vector_value, j, mesh->node[j].flux[FLUX_SIGMAX]);
  feenox_vector_set(mechanical.sigmay->vector_value, j, mesh->node[j].flux[FLUX_SIGMAY]);
  feenox_vector_set(mechanical.tauxy->vector_value, j, mesh->node[j].flux[FLUX_TAUXY]);
  if (feenox.pde.dofs == 3) {
    feenox_vector_set(mechanical.sigmaz->vector_value, j, mesh->node[j].flux[FLUX_SIGMAZ]);
    feenox_vector_set(mechanical.tauyz->vector_value, j, mesh->node[j].flux[FLUX_TAUYZ]);
    feenox_vector_set(mechanical.tauzx->vector_value, j, mesh->node[j].flux[FLUX_TAUZX]);
  }

  if ((mechanical.sigma1 != NULL && mechanical.sigma1->used) ||
      (mechanical.sigma2 != NULL && mechanical.sigma2->used) ||
      (mechanical.sigma3 != NULL && mechanical.sigma3->used) ||
      (mechanical.tresca != NULL && mechanical.tresca->used)) {
    
    double sigma1 = 0;
    double sigma2 = 0;
    double sigma3 = 0;
    feenox_principal_stress_from_cauchy(mesh->node[j].flux[FLUX_SIGMAX],
                                        mesh->node[j].flux[FLUX_SIGMAY],
                                        mesh->node[j].flux[FLUX_SIGMAZ],
                                        mesh->node[j].flux[FLUX_TAUXY],
                                        mesh->node[j].flux[FLUX_TAUYZ],
                                        mesh->node[j].flux[FLUX_TAUZX],
                                        &sigma1,
                                        &sigma2,
                                        &sigma3);
    feenox_vector_set(mechanical.sigma1->vector_value, j, sigma1);
    feenox_vector_set(mechanical.sigma2->vector_value, j, sigma2);
    feenox_vector_set(mechanical.sigma3->vector_value, j, sigma3);
    
    if (mechanical.sigma->used) {
      feenox_vector_set(mechanical.sigma->vector_value, j, feenox_vonmises_from_principal(sigma1, sigma2, sigma3));
    }
    if (mechanical.tresca->used) {
      feenox_vector_set(mechanical.tresca->vector_value, j, sigma1 - sigma3);
    }
  } else if (mechanical.sigma->used) {
    
    feenox_vector_set(mechanical.sigma->vector_value, j, feenox_vonmises_from_stress_tensor(mesh->node[j].flux[FLUX_SIGMAX],
                                                                         mesh->node[j].flux[FLUX_SIGMAY],
                                                                         mesh->node[j].flux[FLUX_SIGMAZ],
                                                                         mesh->node[j].flux[FLUX_TAUXY],
                                                                         mesh->node[j].flux[FLUX_TAUYZ],
                                                                         mesh->node[j].flux[FLUX_TAUZX]));
    
  }
  
  // TODO: uncertainties
  
  return FEENOX_OK;
  
}
