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
  
  if (mechanical.variant != variant_full) {
    feenox_push_error_message("hyperelastic neohookean materials cannot be used in plane stress/strain");
    return FEENOX_ERROR;
  }
  
  // TO-DO: lambda & my or mu & K
  return (mechanical.E.defined_per_group[i] && mechanical.nu.defined_per_group[i]) ? material_model_hyperelastic_neohookean : material_model_unknown;
}

int feenox_mechanical_material_setup_neohookean(void) {
  mechanical.compute_PK2 = feenox_problem_build_mechanical_stress_measure_neohookean;
  // TODO: compute cauchy from PK2?
  mechanical.compute_stress_from_strain = feenox_stress_from_strain_linear;
  mechanical.nonlinear_material = 1;
  return FEENOX_OK;
}


gsl_matrix *feenox_problem_build_mechanical_stress_measure_neohookean(const double *x, material_t *material) {

  // second piola kirchoff
  feenox_problem_mechanical_compute_stress_PK2_neohookean(x, material);

  // tangent matrix
  feenox_problem_mechanical_compute_tangent_matrix_C_neohookean(x, material);

  return mechanical.PK2;
}

gsl_matrix *feenox_problem_mechanical_compute_stress_PK2_neohookean(const double *x, material_t *material) {
  // second piola-kirchoff
  mechanical.J = feenox_fem_determinant(mechanical.F);
  // TODO: check J > 0
  mechanical.J23 = pow(mechanical.J, -2.0/3.0); // isochoric scaling
  mechanical.trC = gsl_matrix_get(mechanical.C, 0, 0) + gsl_matrix_get(mechanical.C, 1, 1) + gsl_matrix_get(mechanical.C, 2, 2);
  mechanical.C_inv = feenox_fem_matrix_invert(mechanical.C, mechanical.C_inv);
    
  double lambda, mu;
  feenox_problem_mechanical_compute_lambda_mu(x, material, &lambda, &mu);
    
  double K = lambda + 2.0/3.0 * mu;
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      double dev_part = mu * mechanical.J23 * ((i == j ? 1.0 : 0.0) - (1.0/3.0) * mechanical.trC * gsl_matrix_get(mechanical.C_inv, i, j));
      double vol_part = K * (mechanical.J - 1.0) * mechanical.J * gsl_matrix_get(mechanical.C_inv, i, j);
            
      gsl_matrix_set(mechanical.PK2, i, j, dev_part + vol_part);
    }
  }  

  return mechanical.PK2;
}


void feenox_tensor_outer_product_voigt(const gsl_matrix *A, const gsl_matrix *B, gsl_matrix *C_voigt) {
  static const int voigt_map[6][2] = {{0,0}, {1,1}, {2,2}, {0,1}, {1,2}, {0,2}};  

  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 6; j++) {
      int p = voigt_map[i][0];
      int q = voigt_map[i][1];
      int r = voigt_map[j][0];
      int s = voigt_map[j][1];
      gsl_matrix_set(C_voigt, i, j, gsl_matrix_get(A, p, q) * gsl_matrix_get(B, r, s));
    }
  }
  
  return;
}

void feenox_tensor_outer_product_ikjl(const gsl_vector *A_voigt, const gsl_vector *B_voigt, gsl_matrix *C_voigt) {
  // Lookup tables mapping from (row, col) in C_voigt to indices in A_voigt and B_voigt
  // These tables encode the specific tensor contraction pattern IKJL
  static const int A_ikjl[6][6] = {
    {0, 3, 5, 0, 3, 0},  // row 0
    {3, 1, 4, 3, 1, 3},  // row 1
    {5, 4, 2, 5, 4, 5},  // row 2
    {0, 3, 5, 0, 3, 0},  // row 3
    {3, 1, 4, 3, 1, 3},  // row 4
    {0, 3, 5, 0, 3, 0}   // row 5
  };
    
  static const int B_ikjl[6][6] = {
    {0, 3, 5, 3, 5, 5},  // row 0
    {3, 1, 4, 1, 4, 4},  // row 1
    {5, 4, 2, 4, 2, 2},  // row 2
    {3, 1, 4, 1, 4, 4},  // row 3
    {5, 4, 2, 4, 2, 2},  // row 4
    {5, 4, 2, 4, 2, 2}   // row 5
  };
    
  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 6; j++) {
      double a_val = gsl_vector_get(A_voigt, A_ikjl[i][j]);
      double b_val = gsl_vector_get(B_voigt, B_ikjl[i][j]);
      gsl_matrix_set(C_voigt, i, j, a_val * b_val);
    }
  }
  
  return;
}

void feenox_tensor_outer_product_iljk(const gsl_vector *A_voigt, const gsl_vector *B_voigt, gsl_matrix *C_voigt) {
  // Lookup tables mapping from (row, col) in C_voigt to indices in A_voigt and B_voigt
  // These tables encode the specific tensor contraction pattern ILJK: C[IJ,LK] = A[IL] × B[JK]
  static const int A_iljk[6][6] = {
    {0, 3, 5, 3, 5, 5},  // row 0: I=0, J=0 -> A[IL] for L=0,1,2,1,2,2
    {3, 1, 4, 1, 4, 4},  // row 1: I=1, J=1 -> A[IL] for L=0,1,2,1,2,2  
    {5, 4, 2, 4, 2, 2},  // row 2: I=2, J=2 -> A[IL] for L=0,1,2,1,2,2
    {0, 3, 5, 3, 5, 5},  // row 3: I=0, J=1 -> A[IL] for L=0,1,2,1,2,2
    {3, 1, 4, 1, 4, 4},  // row 4: I=1, J=2 -> A[IL] for L=0,1,2,1,2,2
    {0, 3, 5, 3, 5, 5}   // row 5: I=0, J=2 -> A[IL] for L=0,1,2,1,2,2
  };
    
  static const int B_iljk[6][6] = {
    {0, 3, 5, 0, 3, 0},  // row 0: I=0, J=0 -> B[JK] for K=0,1,2,1,2,2
    {3, 1, 4, 3, 1, 3},  // row 1: I=1, J=1 -> B[JK] for K=0,1,2,1,2,2
    {5, 4, 2, 5, 4, 5},  // row 2: I=2, J=2 -> B[JK] for K=0,1,2,1,2,2
    {3, 1, 4, 3, 1, 3},  // row 3: I=0, J=1 -> B[JK] for K=0,1,2,1,2,2
    {5, 4, 2, 5, 4, 5},  // row 4: I=1, J=2 -> B[JK] for K=0,1,2,1,2,2
    {5, 4, 2, 5, 4, 5}   // row 5: I=0, J=2 -> B[JK] for K=0,1,2,1,2,2
  };
    
  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 6; j++) {
      double a_val = gsl_vector_get(A_voigt, A_iljk[i][j]);
      double b_val = gsl_vector_get(B_voigt, B_iljk[i][j]);
      gsl_matrix_set(C_voigt, i, j, a_val * b_val);
    }
  }
  
  return;
}
/*    
        (2.0 / 3.0) * std::pow(J, -2.0 / 3.0) * mu *
            (-IxinvC - invCxI + (1.0 / 3.0) * trC * invCxinvC +
             (1.0 / 2.0) * trC * (invCxinvC_ikjl + invCxinvC_iljk)) +
        K * J *
            ((2.0 * J - 1.0) * invCxinvC - (J - 1.0) * (invCxinvC_ikjl + invCxinvC_iljk));    
*/
int feenox_problem_mechanical_compute_tangent_matrix_C_neohookean(const double *x, material_t *material) {
    // Bulk modulus
    // TODO: we already have these! think where to store them...
    double lambda, mu;
    feenox_problem_mechanical_compute_lambda_mu(x, material, &lambda, &mu);
    double K = lambda + (2.0/3.0) * mu;

    if (mechanical.invCxI == NULL) {
      mechanical.invCxI = gsl_matrix_calloc(6, 6);
      mechanical.IxinvC = gsl_matrix_calloc(6, 6);
      mechanical.invCxinvC = gsl_matrix_calloc(6, 6);
      mechanical.invC_voigt = gsl_vector_calloc(6);
      mechanical.invCxinvC_ikjl = gsl_matrix_calloc(6, 6);
      mechanical.invCxinvC_iljk = gsl_matrix_calloc(6, 6);
      mechanical.tmp1 = gsl_matrix_calloc(6, 6);
      mechanical.tmp2 = gsl_matrix_calloc(6, 6);
      mechanical.tmp3 = gsl_matrix_calloc(6, 6);
    }
    feenox_tensor_outer_product_voigt(mechanical.C_inv, mechanical.eye, mechanical.invCxI);
    feenox_tensor_outer_product_voigt(mechanical.eye, mechanical.C_inv, mechanical.IxinvC);
    feenox_tensor_outer_product_voigt(mechanical.C_inv, mechanical.C_inv, mechanical.invCxinvC);
    
    gsl_vector_set(mechanical.invC_voigt, 0, gsl_matrix_get(mechanical.C_inv, 0, 0));
    gsl_vector_set(mechanical.invC_voigt, 1, gsl_matrix_get(mechanical.C_inv, 1, 1));
    gsl_vector_set(mechanical.invC_voigt, 2, gsl_matrix_get(mechanical.C_inv, 2, 2));
    gsl_vector_set(mechanical.invC_voigt, 3, gsl_matrix_get(mechanical.C_inv, 0, 1));
    gsl_vector_set(mechanical.invC_voigt, 4, gsl_matrix_get(mechanical.C_inv, 1, 2));
    gsl_vector_set(mechanical.invC_voigt, 5, gsl_matrix_get(mechanical.C_inv, 2, 0));
    
    feenox_tensor_outer_product_ikjl(mechanical.invC_voigt, mechanical.invC_voigt, mechanical.invCxinvC_ikjl);    
    feenox_tensor_outer_product_iljk(mechanical.invC_voigt, mechanical.invC_voigt, mechanical.invCxinvC_iljk);
    
    double trC = gsl_matrix_get(mechanical.C, 0, 0) + gsl_matrix_get(mechanical.C, 1, 1) + gsl_matrix_get(mechanical.C, 2, 2);
    
    // Precompute common terms
    double J = sqrt(feenox_fem_determinant(mechanical.C));
    double J23 = pow(J, -2.0/3.0);
    
    double term1_scale = (2.0/3.0) * J23 * mu;
    double term2_scale = K * J;
        
    // First part: (2/3)*J^(-2/3)*mu * [...]
    // -IxinvC - invCxI
    gsl_matrix_memcpy(mechanical.tmp1, mechanical.IxinvC);
    gsl_matrix_scale(mechanical.tmp1, -1.0);
    gsl_matrix_sub(mechanical.tmp1, mechanical.invCxI);
    
    // + (1/3)*trC*invCxinvC
    gsl_matrix_memcpy(mechanical.tmp2, mechanical.invCxinvC);
    gsl_matrix_scale(mechanical.tmp2, (1.0/3.0) * trC);
    gsl_matrix_add(mechanical.tmp1, mechanical.tmp2);
    
    // + (1/2)*trC*(invCxinvC_ikjl + invCxinvC_iljk)
    gsl_matrix_memcpy(mechanical.tmp2, mechanical.invCxinvC_ikjl);
    gsl_matrix_add(mechanical.tmp2, mechanical.invCxinvC_iljk);
    gsl_matrix_scale(mechanical.tmp2, (1.0/2.0) * trC);
    gsl_matrix_add(mechanical.tmp1, mechanical.tmp2);
    
    // Scale by term1_scale
    gsl_matrix_scale(mechanical.tmp1, term1_scale);
    
    // Second part: K*J * [...]
    // (2*J-1)*invCxinvC
    gsl_matrix_memcpy(mechanical.tmp2, mechanical.invCxinvC);
    gsl_matrix_scale(mechanical.tmp2, (2.0*J - 1.0));
    
    // - (J-1)*(invCxinvC_ikjl + invCxinvC_iljk)
    gsl_matrix_memcpy(mechanical.tmp3, mechanical.invCxinvC_ikjl);
    gsl_matrix_add(mechanical.tmp3, mechanical.invCxinvC_iljk);
    gsl_matrix_scale(mechanical.tmp3, -(J - 1.0));
    gsl_matrix_add(mechanical.tmp2, mechanical.tmp3);
    
    // Scale by term2_scale
    gsl_matrix_scale(mechanical.tmp2, term2_scale);
    
    // Combine both parts
    gsl_matrix_memcpy(mechanical.C_tangent, mechanical.tmp1);
    gsl_matrix_add(mechanical.C_tangent, mechanical.tmp2);    
    
    
    return FEENOX_OK;
}

