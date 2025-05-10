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

#define FLUX_SIGMAX  0
#define FLUX_SIGMAY  1
#define FLUX_SIGMAZ  3
#define FLUX_TAUXY   2
#define FLUX_TAUYZ   4
#define FLUX_TAUZX   5
#define FLUX_EXX     6
#define FLUX_EYY     7
#define FLUX_EZZ     9
#define FLUX_EXY     8
#define FLUX_EYZ    10
#define FLUX_EZX    11
#define FLUX_SIZE   12

int feenox_problem_gradient_fill_mechanical(void) {
  
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
    unsigned int m = 0;
    for (m = 0; m < flux_size; m++) {
      node->flux[m] = 0;
    }
  }
  
  return FEENOX_OK;
}

int feenox_problem_mechanical_compute_strain_green_lagrange(const gsl_matrix *grad_u) {
  // deformation gradient
  // F = I + grad_u
  gsl_matrix_memcpy(mechanical.F, mechanical.eye);
  gsl_matrix_add(mechanical.F, grad_u);

  // EGL = 1/2 * (FtF - I)   green-lagrange strain tensor  
  feenox_blas_BtB(mechanical.F, 1.0, mechanical.EGL);
  gsl_matrix_sub(mechanical.EGL, mechanical.eye);
  gsl_matrix_scale(mechanical.EGL, 0.5);
  
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
  // TODO: use an integer flag
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

    // green-lagrange strain  
    feenox_call(feenox_problem_mechanical_compute_strain_green_lagrange(element->dphidx_node[j]));  

    exx = gsl_matrix_get(mechanical.EGL, 0, 0);
    eyy = gsl_matrix_get(mechanical.EGL, 1, 1);
    ezz = gsl_matrix_get(mechanical.EGL, 2, 2);
    exy = gsl_matrix_get(mechanical.EGL, 0, 1);
    eyz = gsl_matrix_get(mechanical.EGL, 1, 2);
    ezx = gsl_matrix_get(mechanical.EGL, 2, 0);

    // second piola kirchoff    
    feenox_call(feenox_problem_mechanical_compute_stress_second_piola_kirchoff_elastic_isotropic(node->x, element->physical_group->material));

    // cauchy stress  
    double J = feenox_fem_determinant(mechanical.F);
    feenox_call(feenox_blas_BtCB(mechanical.F, mechanical.S, mechanical.SF, 1/J, mechanical.cauchy));
  
    sigmax = gsl_matrix_get(mechanical.cauchy, 0, 0);
    sigmay = gsl_matrix_get(mechanical.cauchy, 1, 1);
    sigmaz = gsl_matrix_get(mechanical.cauchy, 2, 2);
    tauxy = gsl_matrix_get(mechanical.cauchy, 0, 1);
    tauyz = gsl_matrix_get(mechanical.cauchy, 1, 2);
    tauzx = gsl_matrix_get(mechanical.cauchy, 2, 0);
  }
  

  
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
