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

int feenox_problem_mechanical_compute_tangent_matrix_C_neohookean1(const double *x, material_t *material);
int feenox_problem_mechanical_compute_tangent_matrix_C_neohookean2(const double *x, material_t *material);
int feenox_problem_mechanical_compute_tangent_matrix_C_neohookean3(const double *x, material_t *material);


int feenox_mechanical_material_init_neohookean(material_t *material, int i) {
  // TO-DO: mu and K
  return (mechanical.E.defined_per_group[i] && mechanical.nu.defined_per_group[i]) ? material_model_hyperelastic_neohookean : material_model_unknown;
}

int feenox_problem_build_mechanical_stress_measure_neohookean(const gsl_matrix *grad_u, const double *x, material_t *material) {

  // TODO: ask epsilon through arg, it it's null point it to mechanical.epsilon
  mechanical.epsilon = mechanical.epsilon_cauchy_green;
  // same
  mechanical.S = mechanical.cauchy;
  
  feenox_call(feenox_problem_mechanical_compute_deformation_gradient(grad_u));
  // RCG = Ft * F   right cauchy-green tensor
  feenox_blas_BtB(mechanical.F, 1.0, mechanical.epsilon_cauchy_green);
//  printf("E = Green Lagrange\n");
//  feenox_debug_print_gsl_matrix(mechanical.epsilon_cauchy_green, stdout);
    
  // second piola kirchoff
  feenox_call(feenox_problem_mechanical_compute_stress_PK2_neohookean(x, material));

  // cauchy stress
  feenox_call(feenox_problem_mechanical_compute_stress_cauchy_from_PK2(mechanical.F, mechanical.PK2));  
  
  // tangent matrix
  feenox_call(feenox_problem_mechanical_compute_tangent_matrix_C_neohookean3(x, material));
//  feenox_call(feenox_problem_mechanical_compute_tangent_matrix_C_linear_elastic(x, material));

  return FEENOX_OK;
}

// TODO:
// int feenox_problem_mechanical_compute_C_hyperelastic_neohookean(const double *x, material_t *material);

int feenox_problem_mechanical_compute_stress_cauchy_from_PK2(const gsl_matrix *F, const gsl_matrix *PK2) {
  // volume change    
  double J = feenox_fem_determinant(mechanical.F);
  feenox_blas_BCBt(mechanical.F, mechanical.PK2, NULL, 1/J, mechanical.cauchy);
    
  return FEENOX_OK;
}

int feenox_problem_mechanical_compute_stress_PK2_neohookean(const double *x, material_t *material) {
  // second piola-kirchoff
  double J = feenox_fem_determinant(mechanical.F);
  // TODO: check J > 0
  
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
  
/*  
    // Compute PK2 stress in Voigt form
    double PK2[3][3];
    double J_minus_2_3 = pow(J, -2.0/3.0);
    
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            // Deviatoric part
            double dev_part = mu * J_minus_2_3 * 
                             ((i == j ? 1.0 : 0.0) - (1.0/3.0) * trC * Cinv[i][j]);
            // Volumetric part  
            double vol_part = K * (J - 1.0) * J * Cinv[i][j];
            
            PK2[i][j] = dev_part + vol_part;
        }
    }  
*/

  return FEENOX_OK;
}



void voigt_outer_product(const gsl_matrix *A, const gsl_matrix *B, gsl_matrix *C_voigt) {
//    int voigt_map[6][2] = {{0,0}, {1,1}, {2,2}, {0,1}, {0,2}, {1,2}};
    int voigt_map[6][2] = {{0,0}, {1,1}, {2,2}, {0,1}, {1,2}, {0,2}};  

    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 6; j++) {
            int p = voigt_map[i][0], q = voigt_map[i][1];
            int r = voigt_map[j][0], s = voigt_map[j][1];
            double val = gsl_matrix_get(A, p, q) * gsl_matrix_get(B, r, s);

            // Account for Voigt shear factors (optional)
//            if (i >= 3) val *= 0.25;
//            if (j >= 3) val *= 0.25;

            gsl_matrix_set(C_voigt, i, j, val);
        }
    }
}


void compute_tensor_product_IKJL(const gsl_vector *A_voigt, const gsl_vector *B_voigt, gsl_matrix *C_voigt) {

    gsl_matrix_set(C_voigt, 0, 0, gsl_vector_get(A_voigt, 0) * gsl_vector_get(B_voigt, 0));
    gsl_matrix_set(C_voigt, 0, 1, gsl_vector_get(A_voigt, 3) * gsl_vector_get(B_voigt, 3));
    gsl_matrix_set(C_voigt, 0, 2, gsl_vector_get(A_voigt, 5) * gsl_vector_get(B_voigt, 5));
    gsl_matrix_set(C_voigt, 0, 3, gsl_vector_get(A_voigt, 0) * gsl_vector_get(B_voigt, 3));
    gsl_matrix_set(C_voigt, 0, 4, gsl_vector_get(A_voigt, 3) * gsl_vector_get(B_voigt, 5));
    gsl_matrix_set(C_voigt, 0, 5, gsl_vector_get(A_voigt, 0) * gsl_vector_get(B_voigt, 5));
    
    gsl_matrix_set(C_voigt, 1, 0, gsl_vector_get(A_voigt, 3) * gsl_vector_get(B_voigt, 3));
    gsl_matrix_set(C_voigt, 1, 1, gsl_vector_get(A_voigt, 1) * gsl_vector_get(B_voigt, 1));
    gsl_matrix_set(C_voigt, 1, 2, gsl_vector_get(A_voigt, 4) * gsl_vector_get(B_voigt, 4));
    gsl_matrix_set(C_voigt, 1, 3, gsl_vector_get(A_voigt, 3) * gsl_vector_get(B_voigt, 1));
    gsl_matrix_set(C_voigt, 1, 4, gsl_vector_get(A_voigt, 1) * gsl_vector_get(B_voigt, 4));
    gsl_matrix_set(C_voigt, 1, 5, gsl_vector_get(A_voigt, 3) * gsl_vector_get(B_voigt, 4));
    
    gsl_matrix_set(C_voigt, 2, 0, gsl_vector_get(A_voigt, 5) * gsl_vector_get(B_voigt, 5));
    gsl_matrix_set(C_voigt, 2, 1, gsl_vector_get(A_voigt, 4) * gsl_vector_get(B_voigt, 4));
    gsl_matrix_set(C_voigt, 2, 2, gsl_vector_get(A_voigt, 2) * gsl_vector_get(B_voigt, 2));
    gsl_matrix_set(C_voigt, 2, 3, gsl_vector_get(A_voigt, 5) * gsl_vector_get(B_voigt, 4));
    gsl_matrix_set(C_voigt, 2, 4, gsl_vector_get(A_voigt, 4) * gsl_vector_get(B_voigt, 2));
    gsl_matrix_set(C_voigt, 2, 5, gsl_vector_get(A_voigt, 5) * gsl_vector_get(B_voigt, 2));
    
    gsl_matrix_set(C_voigt, 3, 0, gsl_vector_get(A_voigt, 0) * gsl_vector_get(B_voigt, 3));
    gsl_matrix_set(C_voigt, 3, 1, gsl_vector_get(A_voigt, 3) * gsl_vector_get(B_voigt, 1));
    gsl_matrix_set(C_voigt, 3, 2, gsl_vector_get(A_voigt, 5) * gsl_vector_get(B_voigt, 4));
    gsl_matrix_set(C_voigt, 3, 3, gsl_vector_get(A_voigt, 0) * gsl_vector_get(B_voigt, 1));
    gsl_matrix_set(C_voigt, 3, 4, gsl_vector_get(A_voigt, 3) * gsl_vector_get(B_voigt, 4));
    gsl_matrix_set(C_voigt, 3, 5, gsl_vector_get(A_voigt, 0) * gsl_vector_get(B_voigt, 4));
    
    gsl_matrix_set(C_voigt, 4, 0, gsl_vector_get(A_voigt, 3) * gsl_vector_get(B_voigt, 5));
    gsl_matrix_set(C_voigt, 4, 1, gsl_vector_get(A_voigt, 1) * gsl_vector_get(B_voigt, 4));
    gsl_matrix_set(C_voigt, 4, 2, gsl_vector_get(A_voigt, 4) * gsl_vector_get(B_voigt, 2));
    gsl_matrix_set(C_voigt, 4, 3, gsl_vector_get(A_voigt, 3) * gsl_vector_get(B_voigt, 4));
    gsl_matrix_set(C_voigt, 4, 4, gsl_vector_get(A_voigt, 1) * gsl_vector_get(B_voigt, 2));
    gsl_matrix_set(C_voigt, 4, 5, gsl_vector_get(A_voigt, 3) * gsl_vector_get(B_voigt, 2));
    
    gsl_matrix_set(C_voigt, 5, 0, gsl_vector_get(A_voigt, 0) * gsl_vector_get(B_voigt, 5));
    gsl_matrix_set(C_voigt, 5, 1, gsl_vector_get(A_voigt, 3) * gsl_vector_get(B_voigt, 4));
    gsl_matrix_set(C_voigt, 5, 2, gsl_vector_get(A_voigt, 5) * gsl_vector_get(B_voigt, 2));
    gsl_matrix_set(C_voigt, 5, 3, gsl_vector_get(A_voigt, 0) * gsl_vector_get(B_voigt, 4));
    gsl_matrix_set(C_voigt, 5, 4, gsl_vector_get(A_voigt, 3) * gsl_vector_get(B_voigt, 2));
    gsl_matrix_set(C_voigt, 5, 5, gsl_vector_get(A_voigt, 0) * gsl_vector_get(B_voigt, 2));

    return;
}

//void compute_tensor_product_IKJL(const gsl_vector *A_voigt, const gsl_vector *B_voigt, gsl_matrix *C_voigt) {
void compute_tensor_product_ILJK(const gsl_vector *A_voigt, const gsl_vector *B_voigt, gsl_matrix *C_voigt) {

    gsl_matrix_set(C_voigt, 0, 0, gsl_vector_get(A_voigt, 0) * gsl_vector_get(B_voigt, 0));
    gsl_matrix_set(C_voigt, 0, 1, gsl_vector_get(A_voigt, 3) * gsl_vector_get(B_voigt, 3));
    gsl_matrix_set(C_voigt, 0, 2, gsl_vector_get(A_voigt, 5) * gsl_vector_get(B_voigt, 5));
    gsl_matrix_set(C_voigt, 0, 3, gsl_vector_get(A_voigt, 3) * gsl_vector_get(B_voigt, 0));
    gsl_matrix_set(C_voigt, 0, 4, gsl_vector_get(A_voigt, 5) * gsl_vector_get(B_voigt, 3));
    gsl_matrix_set(C_voigt, 0, 5, gsl_vector_get(A_voigt, 5) * gsl_vector_get(B_voigt, 0));

    gsl_matrix_set(C_voigt, 1, 0, gsl_vector_get(A_voigt, 3) * gsl_vector_get(B_voigt, 3));
    gsl_matrix_set(C_voigt, 1, 1, gsl_vector_get(A_voigt, 1) * gsl_vector_get(B_voigt, 1));
    gsl_matrix_set(C_voigt, 1, 2, gsl_vector_get(A_voigt, 4) * gsl_vector_get(B_voigt, 4));
    gsl_matrix_set(C_voigt, 1, 3, gsl_vector_get(A_voigt, 1) * gsl_vector_get(B_voigt, 3));
    gsl_matrix_set(C_voigt, 1, 4, gsl_vector_get(A_voigt, 4) * gsl_vector_get(B_voigt, 1));
    gsl_matrix_set(C_voigt, 1, 5, gsl_vector_get(A_voigt, 4) * gsl_vector_get(B_voigt, 3));

    gsl_matrix_set(C_voigt, 2, 0, gsl_vector_get(A_voigt, 5) * gsl_vector_get(B_voigt, 5));
    gsl_matrix_set(C_voigt, 2, 1, gsl_vector_get(A_voigt, 4) * gsl_vector_get(B_voigt, 4));
    gsl_matrix_set(C_voigt, 2, 2, gsl_vector_get(A_voigt, 2) * gsl_vector_get(B_voigt, 2));
    gsl_matrix_set(C_voigt, 2, 3, gsl_vector_get(A_voigt, 4) * gsl_vector_get(B_voigt, 5));
    gsl_matrix_set(C_voigt, 2, 4, gsl_vector_get(A_voigt, 2) * gsl_vector_get(B_voigt, 4));
    gsl_matrix_set(C_voigt, 2, 5, gsl_vector_get(A_voigt, 2) * gsl_vector_get(B_voigt, 5));

    gsl_matrix_set(C_voigt, 3, 0, gsl_vector_get(A_voigt, 0) * gsl_vector_get(B_voigt, 3));
    gsl_matrix_set(C_voigt, 3, 1, gsl_vector_get(A_voigt, 3) * gsl_vector_get(B_voigt, 1));
    gsl_matrix_set(C_voigt, 3, 2, gsl_vector_get(A_voigt, 5) * gsl_vector_get(B_voigt, 4));
    gsl_matrix_set(C_voigt, 3, 3, gsl_vector_get(A_voigt, 3) * gsl_vector_get(B_voigt, 3));
    gsl_matrix_set(C_voigt, 3, 4, gsl_vector_get(A_voigt, 5) * gsl_vector_get(B_voigt, 1));
    gsl_matrix_set(C_voigt, 3, 5, gsl_vector_get(A_voigt, 5) * gsl_vector_get(B_voigt, 3));

    gsl_matrix_set(C_voigt, 4, 0, gsl_vector_get(A_voigt, 3) * gsl_vector_get(B_voigt, 5));
    gsl_matrix_set(C_voigt, 4, 1, gsl_vector_get(A_voigt, 1) * gsl_vector_get(B_voigt, 4));
    gsl_matrix_set(C_voigt, 4, 2, gsl_vector_get(A_voigt, 4) * gsl_vector_get(B_voigt, 2));
    gsl_matrix_set(C_voigt, 4, 3, gsl_vector_get(A_voigt, 1) * gsl_vector_get(B_voigt, 5));
    gsl_matrix_set(C_voigt, 4, 4, gsl_vector_get(A_voigt, 4) * gsl_vector_get(B_voigt, 4));
    gsl_matrix_set(C_voigt, 4, 5, gsl_vector_get(A_voigt, 4) * gsl_vector_get(B_voigt, 5));

    gsl_matrix_set(C_voigt, 5, 0, gsl_vector_get(A_voigt, 0) * gsl_vector_get(B_voigt, 5));
    gsl_matrix_set(C_voigt, 5, 1, gsl_vector_get(A_voigt, 3) * gsl_vector_get(B_voigt, 4));
    gsl_matrix_set(C_voigt, 5, 2, gsl_vector_get(A_voigt, 5) * gsl_vector_get(B_voigt, 2));
    gsl_matrix_set(C_voigt, 5, 3, gsl_vector_get(A_voigt, 3) * gsl_vector_get(B_voigt, 5));
    gsl_matrix_set(C_voigt, 5, 4, gsl_vector_get(A_voigt, 5) * gsl_vector_get(B_voigt, 4));
    gsl_matrix_set(C_voigt, 5, 5, gsl_vector_get(A_voigt, 5) * gsl_vector_get(B_voigt, 5));

    return;
}


int feenox_problem_mechanical_compute_tangent_matrix_C_neohookean1(const double *x, material_t *material) {


    const gsl_matrix *C = mechanical.epsilon_cauchy_green;       // Input: Right Cauchy-Green tensor (3x3)
    gsl_matrix *C_inv = mechanical.invC;
    feenox_fem_matrix_invert(C, C_inv);

/*  
    printf("C = right cauchy green\n");
    feenox_debug_print_gsl_matrix(C, stdout);

    printf("invC\n");
    feenox_debug_print_gsl_matrix(C_inv, stdout);

    printf("invC\n");
    feenox_debug_print_gsl_matrix(C_inv, stdout);
*/
    
    double J = sqrt(feenox_fem_determinant(C));
    double trC = gsl_matrix_get(C, 0, 0) + gsl_matrix_get(C, 1, 1) + gsl_matrix_get(C, 2, 2);

    double lambda, mu;
    feenox_problem_mechanical_compute_lambda_mu(x, material, &lambda, &mu);
    double K = lambda + (2.0/3.0) * mu;
    
    // Voigt notation mapping: 11->0, 22->1, 33->2, 12->3, 13->4, 23->5
    int voigt_map[6][2] = {{0,0}, {1,1}, {2,2}, {0,1}, {0,2}, {1,2}};
    
    // Compute J^(-2/3) and its derivatives
    double J_minus_2_3 = pow(J, -2.0/3.0);
    double dJ_minus_2_3_dJ = -2.0/3.0 * pow(J, -5.0/3.0);    
    
    // Loop over all Voigt components
    for (int p = 0; p < 6; p++) {
        for (int q = 0; q < 6; q++) {
            int i = voigt_map[p][0];
            int j = voigt_map[p][1];
            int k = voigt_map[q][0];
            int l = voigt_map[q][1];
            
            double C_ijkl = 0.0;
            
            // --- DEVIATORIC CONTRIBUTION ---
            // Term 1: mu * d(J^(-2/3))/dE_kl * (I_ij - 1/3*trC * Cinv_ij)
            double dJ_dE_kl = J * gsl_matrix_get(C_inv, k, l);  // dJ/dE_kl = J * Cinv_kl
            double dev_stress_ij = (i == j ? 1.0 : 0.0) - (1.0/3.0) * trC * gsl_matrix_get(C_inv, i, j);
            C_ijkl += mu * dJ_minus_2_3_dJ * dJ_dE_kl * dev_stress_ij;
            
            // Term 2: mu * J^(-2/3) * d(I_ij - 1/3*trC * Cinv_ij)/dE_kl
            // Sub-term 2a: -mu * J^(-2/3) * 1/3 * d(trC)/dE_kl * Cinv_ij
            double dtrC_dE_kl = 2.0 * (k == l ? 1.0 : 0.0);  // trC = C_mm, so d(trC)/dE_kl = 2*delta_kl
            C_ijkl += -mu * J_minus_2_3 * (1.0/3.0) * dtrC_dE_kl * gsl_matrix_get(C_inv, i, j);
            
            // Sub-term 2b: -mu * J^(-2/3) * 1/3 * trC * d(Cinv_ij)/dE_kl
            // d(Cinv_ij)/dE_kl = -2 * Cinv_ik * Cinv_jl
            double dCinv_ij_dE_kl = -2.0 * gsl_matrix_get(C_inv, i, k) * gsl_matrix_get(C_inv, j, l);
            C_ijkl += -mu * J_minus_2_3 * (1.0/3.0) * trC * dCinv_ij_dE_kl;
            
            // --- VOLUMETRIC CONTRIBUTION ---
            // Term 3: K * d((J-1)*J)/dE_kl * Cinv_ij
            double d_J_minus_1_J_dE_kl = (2.0*J - 1.0) * J * gsl_matrix_get(C_inv, k, l);  // d((J-1)*J)/dE_kl
            C_ijkl += K * d_J_minus_1_J_dE_kl * gsl_matrix_get(C_inv, i, j);
            
            // Term 4: K * (J-1)*J * d(Cinv_ij)/dE_kl
            C_ijkl += K * (J - 1.0) * J * dCinv_ij_dE_kl;
            
            // Apply Voigt factor for shear components
            double voigt_factor = 1.0;
            if (p >= 3) voigt_factor *= 2.0;  // Engineering shear strain factor
            if (q >= 3) voigt_factor *= 2.0;
            
            gsl_matrix_set(mechanical.C, p, q, C_ijkl * voigt_factor);
        }
    }
/*    
    printf("\n");
    feenox_debug_print_gsl_matrix(mechanical.C, stdout);
*/    
}


int feenox_problem_mechanical_compute_tangent_matrix_C_neohookean2(const double *x, material_t *material) {
  
    const gsl_matrix *C = mechanical.epsilon_cauchy_green;       // Input: Right Cauchy-Green tensor (3x3)
    gsl_matrix *C_inv = mechanical.invC;
    feenox_fem_matrix_invert(C, C_inv);
    
    double J = sqrt(feenox_fem_determinant(C));
    double trC = gsl_matrix_get(C, 0, 0) + gsl_matrix_get(C, 1, 1) + gsl_matrix_get(C, 2, 2);

    double lambda, mu;
    feenox_problem_mechanical_compute_lambda_mu(x, material, &lambda, &mu);
    double K = lambda + (2.0/3.0) * mu;
    
    // --- Step 2: Compute isochoric (deviatoric) part ---
    gsl_matrix *I_minus_13trC_Cinv = gsl_matrix_alloc(3, 3);
    gsl_matrix_set_identity(I_minus_13trC_Cinv);
    gsl_matrix_scale(C_inv, -1.0/3.0 * trC);
    gsl_matrix_add(I_minus_13trC_Cinv, C_inv);
    gsl_matrix_scale(C_inv, -3.0/trC); // Restore C_inv

    // --- Step 3: Compute volumetric part ---
    gsl_matrix *S_vol = gsl_matrix_alloc(3, 3);
    gsl_matrix_memcpy(S_vol, C_inv);
    gsl_matrix_scale(S_vol, K * (J - 1.0) * J);

    // --- Step 4: Assemble 6x6 tangent matrix (Voigt notation) ---
    // Standard Voigt ordering: 11→0, 22→1, 33→2, 12→3, 13→4, 23→5
    double C_tangent_data[6][6] = {0};

    // Deviatoric + Volumetric contributions
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 3; k++) {
                for (int l = 0; l < 3; l++) {
                    // Compute 4th-order tangent modulus (simplified)
                    double C_iso = mu * pow(J, -2.0/3.0) * (
                        (i == k && j == l ? 1.0 : 0.0) - 
                        (1.0/3.0) * trC * gsl_matrix_get(C_inv, i, j) * gsl_matrix_get(C_inv, k, l) +
                        (1.0/3.0) * trC * (gsl_matrix_get(C_inv, i, k) * gsl_matrix_get(C_inv, j, l) + 
                                          gsl_matrix_get(C_inv, i, l) * gsl_matrix_get(C_inv, j, k))
                    );

                    double C_vol = K * J * (
                        (2.0 * J - 1.0) * gsl_matrix_get(C_inv, i, j) * gsl_matrix_get(C_inv, k, l) -
                        2.0 * (J - 1.0) * (gsl_matrix_get(C_inv, i, k) * gsl_matrix_get(C_inv, j, l) + 
                                           gsl_matrix_get(C_inv, i, l) * gsl_matrix_get(C_inv, j, k))
                    );

                    // Map to Voigt notation (standard ordering)
                    int voigt_i, voigt_k;
                    if (i == j) voigt_i = i;     // 11→0, 22→1, 33→2
                    else voigt_i = 3 + i + (j > i ? j-1 : j); // 12→3, 13→4, 23→5
                    if (k == l) voigt_k = k;
                    else voigt_k = 3 + k + (l > k ? l-1 : l);
                    
                    C_tangent_data[voigt_i][voigt_k] += C_iso + C_vol;
                }
            }
        }
    }
    // --- Step 5: Fill the 6x6 output matrix ---
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 6; j++) {
            gsl_matrix_set(mechanical.C, i, j, C_tangent_data[i][j]);
        }
    }

//    printf("\n");
//    feenox_debug_print_gsl_matrix(mechanical.C, stdout);
    
    // Free memory
//    gsl_matrix_free(I_minus_13trC_Cinv);
//    gsl_matrix_free(S_vol);
}



int feenox_problem_mechanical_compute_tangent_matrix_C_neohookean3(const double *x, material_t *material) {
  
    gsl_matrix *C = mechanical.epsilon_cauchy_green;
//    gsl_matrix *C = gsl_matrix_calloc(3, 3);
//    gsl_matrix_set_identity(C);  // Example: No deformation  
    
    gsl_matrix *eye = gsl_matrix_calloc(3, 3);
    gsl_matrix_set_identity(eye);
    
  
    // Compute C inverse
    // TODO: we have already tthis
    gsl_matrix *Cinv = mechanical.invC;
    feenox_fem_matrix_invert(C, Cinv);

    // Bulk modulus
    // we already have this!
    double lambda, mu;
    feenox_problem_mechanical_compute_lambda_mu(x, material, &lambda, &mu);
    double K = lambda + (2.0/3.0) * mu;
    
    gsl_matrix *invCxI = gsl_matrix_calloc(6, 6);
    voigt_outer_product(Cinv, eye, invCxI);

    gsl_matrix *IxinvC = gsl_matrix_calloc(6, 6);
    voigt_outer_product(eye, Cinv, IxinvC);

    gsl_matrix *invCxinvC = gsl_matrix_calloc(6, 6);
    voigt_outer_product(Cinv, Cinv, invCxinvC);
    
    gsl_vector *invC_voigt = gsl_vector_calloc(6);
    gsl_vector_set(invC_voigt, 0, gsl_matrix_get(Cinv, 0, 0));
    gsl_vector_set(invC_voigt, 1, gsl_matrix_get(Cinv, 1, 1));
    gsl_vector_set(invC_voigt, 2, gsl_matrix_get(Cinv, 2, 2));
    gsl_vector_set(invC_voigt, 3, gsl_matrix_get(Cinv, 0, 1));
//    gsl_vector_set(invC_voigt, 4, gsl_matrix_get(Cinv, 0, 2));
//    gsl_vector_set(invC_voigt, 5, gsl_matrix_get(Cinv, 1, 2));
    gsl_vector_set(invC_voigt, 4, gsl_matrix_get(Cinv, 1, 2));
    gsl_vector_set(invC_voigt, 5, gsl_matrix_get(Cinv, 2, 0));
    
    gsl_matrix *invCxinvC_ikjl = gsl_matrix_calloc(6, 6);
    compute_tensor_product_IKJL(invC_voigt, invC_voigt, invCxinvC_ikjl);
    
    
    
    gsl_matrix *invCxinvC_iljk = gsl_matrix_calloc(6, 6);
    compute_tensor_product_ILJK(invC_voigt, invC_voigt, invCxinvC_iljk);
/*
    printf("C\n");
    feenox_debug_print_gsl_matrix(C, stdout);
    printf("Cinv\n");
    feenox_debug_print_gsl_matrix(Cinv, stdout);

    printf("invCxI\n");
    feenox_debug_print_gsl_matrix(invCxI, stdout);
    
    printf("IxinvC\n");
    feenox_debug_print_gsl_matrix(IxinvC, stdout);

    printf("invCxinvC\n");
    feenox_debug_print_gsl_matrix(invCxinvC, stdout);

    printf("invCxinvC_ikjl\n");
    feenox_debug_print_gsl_matrix(invCxinvC_ikjl, stdout);
    
    printf("invCxinvC_iljk\n");
    feenox_debug_print_gsl_matrix(invCxinvC_iljk, stdout);
*/    
    double trC = gsl_matrix_get(C, 0, 0) + gsl_matrix_get(C, 1, 1) + gsl_matrix_get(C, 2, 2);
/*    
        (2.0 / 3.0) * std::pow(J, -2.0 / 3.0) * mu *
            (-IxinvC - invCxI + (1.0 / 3.0) * trC * invCxinvC +
             (1.0 / 2.0) * trC * (invCxinvC_ikjl + invCxinvC_iljk)) +
        K * J *
            ((2.0 * J - 1.0) * invCxinvC - (J - 1.0) * (invCxinvC_ikjl + invCxinvC_iljk));    
*/
    
    // Precompute common terms
    double J = sqrt(feenox_fem_determinant(C));
    double J23 = pow(J, -2.0/3.0);
    double term1_scale = (2.0/3.0) * J23 * mu;
    double term2_scale = K * J;
    
    // Temporary matrix storage
    gsl_matrix *temp1 = gsl_matrix_calloc(6, 6);
    gsl_matrix *temp2 = gsl_matrix_calloc(6, 6);
    
    // First part: (2/3)*J^(-2/3)*mu * [...]
    // -IxinvC - invCxI
    gsl_matrix_memcpy(temp1, IxinvC);
    gsl_matrix_scale(temp1, -1.0);
    gsl_matrix_sub(temp1, invCxI);
    
    // + (1/3)*trC*invCxinvC
    gsl_matrix_memcpy(temp2, invCxinvC);
    gsl_matrix_scale(temp2, (1.0/3.0) * trC);
    gsl_matrix_add(temp1, temp2);
    
    // + (1/2)*trC*(invCxinvC_ikjl + invCxinvC_iljk)
    gsl_matrix_memcpy(temp2, invCxinvC_ikjl);
    gsl_matrix_add(temp2, invCxinvC_iljk);
    gsl_matrix_scale(temp2, (1.0/2.0) * trC);
    gsl_matrix_add(temp1, temp2);
    
    // Scale by term1_scale
    gsl_matrix_scale(temp1, term1_scale);
    
    // Second part: K*J * [...]
    // (2*J-1)*invCxinvC
    gsl_matrix_memcpy(temp2, invCxinvC);
    gsl_matrix_scale(temp2, (2.0*J - 1.0));
    
    // - (J-1)*(invCxinvC_ikjl + invCxinvC_iljk)
    gsl_matrix *temp3 = gsl_matrix_calloc(6, 6);
    gsl_matrix_memcpy(temp3, invCxinvC_ikjl);
    gsl_matrix_add(temp3, invCxinvC_iljk);
    gsl_matrix_scale(temp3, -(J - 1.0));
    gsl_matrix_add(temp2, temp3);
    
    // Scale by term2_scale
    gsl_matrix_scale(temp2, term2_scale);
    
    // Combine both parts
    gsl_matrix_memcpy(mechanical.C, temp1);
    gsl_matrix_add(mechanical.C, temp2);    
    
//    printf("\n");
//    feenox_debug_print_gsl_matrix(mechanical.C, stdout);
    
    return FEENOX_OK;
}

