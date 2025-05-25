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
    
  // cauchy stress
  feenox_call(feenox_problem_mechanical_compute_stress_cauchy_neohookean(x, material));  

  // second piola kirchoff
  feenox_call(feenox_problem_mechanical_compute_stress_second_piola_kirchoff_neohookean(x, material));

  // tangent matrix
  feenox_call(feenox_problem_mechanical_compute_tangent_matrix_C_neohookean(x, material));
//  feenox_call(feenox_problem_mechanical_compute_tangent_matrix_C_linear_elastic(x, material));

  return FEENOX_OK;
}

// TODO:
// int feenox_problem_mechanical_compute_C_hyperelastic_neohookean(const double *x, material_t *material);

int feenox_problem_mechanical_compute_stress_cauchy_neohookean(const double *x, material_t *material) {
  // volume change    
  double J = feenox_fem_determinant(mechanical.F);
    
  // neo-hookean cauchy stress
  // sigma = (mu/J) * (b - I) + lambda/J * log(J) * I
    
  double lambda, mu;
  feenox_problem_mechanical_compute_lambda_mu(x, material, &lambda, &mu);
    
  // deviatoric
  gsl_matrix_memcpy(mechanical.cauchy, mechanical.epsilon);
  gsl_matrix_sub(mechanical.cauchy, mechanical.eye);
  gsl_matrix_scale(mechanical.cauchy, mu/J);
    
  // volumetric
  gsl_matrix_memcpy(mechanical.tmp, mechanical.eye);
  gsl_matrix_scale(mechanical.tmp, lambda/J * log(J));
    
  // cauchy
  gsl_matrix_add(mechanical.cauchy, mechanical.tmp);
    
  return FEENOX_OK;
}

int feenox_problem_mechanical_compute_stress_second_piola_kirchoff_neohookean(const double *x, material_t *material) {
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

  return FEENOX_OK;
}



void voigt_outer_product(const gsl_matrix *A, const gsl_matrix *B, gsl_matrix *C_voigt) {
    // Voigt indices: [0=11, 1=22, 2=33, 3=12, 4=23, 5=13]
    int voigt_map[6][2] = {{0,0}, {1,1}, {2,2}, {0,1}, {1,2}, {0,2}};

    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 6; j++) {
            int p = voigt_map[i][0], q = voigt_map[i][1];
            int r = voigt_map[j][0], s = voigt_map[j][1];
            double val = gsl_matrix_get(A, p, q) * gsl_matrix_get(B, r, s);

            // Account for Voigt shear factors (optional)
            if (i >= 3) val *= 0.5;
            if (j >= 3) val *= 0.5;

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


int feenox_problem_mechanical_compute_tangent_matrix_C_neohookean(const double *x, material_t *material) {
  
    gsl_matrix *C = mechanical.epsilon_cauchy_green;
//    gsl_matrix *C = gsl_matrix_calloc(3, 3);
//    gsl_matrix_set_identity(C);  // Example: No deformation  
    
    gsl_matrix *eye = gsl_matrix_calloc(3, 3);
    gsl_matrix_set_identity(eye);  // Example: No deformation  
    
  
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
    gsl_vector_set(invC_voigt, 4, gsl_matrix_get(Cinv, 1, 2));
    gsl_vector_set(invC_voigt, 5, gsl_matrix_get(Cinv, 2, 0));
    
    gsl_matrix *invCxinvC_ikjl = gsl_matrix_calloc(6, 6);
    compute_tensor_product_IKJL(invC_voigt, invC_voigt, invCxinvC_ikjl);
    gsl_matrix *invCxinvC_iljk = gsl_matrix_calloc(6, 6);
    compute_tensor_product_ILJK(invC_voigt, invC_voigt, invCxinvC_ikjl);
    
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

