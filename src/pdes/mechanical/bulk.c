/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox routines to build elemental mechanical objects
 *
 *  Copyright (C) 2021--2025 Jeremy Theler
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

int feenox_problem_build_allocate_aux_mechanical(unsigned int n_nodes) {
  
  mechanical.n_nodes = n_nodes;
  
  // matrix with the shape functions
  if (mechanical.B_shape != NULL) {
    gsl_matrix_free(mechanical.B_shape);
  }
  feenox_check_alloc(mechanical.B_shape = gsl_matrix_calloc(mechanical.stress_strain_size, feenox.pde.dofs * mechanical.n_nodes));
  
  // this is a temporary holder to compute things like BtCB
  if (mechanical.CB != NULL) {
    gsl_matrix_free(mechanical.CB);
  }
  feenox_check_alloc(mechanical.CB = gsl_matrix_calloc(mechanical.stress_strain_size, feenox.pde.dofs * mechanical.n_nodes));

  // --- non-linear stuff ------------------
  if (feenox.pde.math_type == math_type_nonlinear) {
    
    // matrix with shape derivatives for jacobian
    if (mechanical.G != NULL) {
      gsl_matrix_free(mechanical.G);
    }
    feenox_check_alloc(mechanical.G = gsl_matrix_calloc(feenox.pde.dofs*feenox.pde.dofs, feenox.pde.dofs * mechanical.n_nodes));

    // 9x9 expansion of stresses for jacobian
    if (mechanical.Sigma != NULL) {
      gsl_matrix_free(mechanical.Sigma);
    }
    feenox_check_alloc(mechanical.Sigma = gsl_matrix_calloc(feenox.pde.dofs*feenox.pde.dofs, feenox.pde.dofs*feenox.pde.dofs));

    // temporary holder to compute GtSigmaG
    if (mechanical.SigmaG != NULL) {
      gsl_matrix_free(mechanical.SigmaG);
    }
    feenox_check_alloc(mechanical.SigmaG = gsl_matrix_calloc(feenox.pde.dofs*feenox.pde.dofs, feenox.pde.dofs * mechanical.n_nodes));
    
    
    if (mechanical.grad_u == NULL) {
      // 3x3 identity (the symbol I is already taken by complex.h)
      feenox_check_alloc(mechanical.eye = gsl_matrix_alloc(feenox.pde.dofs, feenox.pde.dofs));
      gsl_matrix_set_identity(mechanical.eye);

      // displacement gradient
      feenox_check_alloc(mechanical.grad_u = gsl_matrix_calloc(feenox.pde.dofs, feenox.pde.dofs));
      
      // deformation gradient
      feenox_check_alloc(mechanical.F = gsl_matrix_calloc(feenox.pde.dofs, feenox.pde.dofs));
      feenox_check_alloc(mechanical.F_inv = gsl_matrix_calloc(feenox.pde.dofs, feenox.pde.dofs));

      // strains
      feenox_check_alloc(mechanical.eps = gsl_matrix_calloc(feenox.pde.dofs, feenox.pde.dofs));
      feenox_check_alloc(mechanical.B = gsl_matrix_calloc(feenox.pde.dofs, feenox.pde.dofs));
      feenox_check_alloc(mechanical.C = gsl_matrix_calloc(feenox.pde.dofs, feenox.pde.dofs));
      feenox_check_alloc(mechanical.C_inv = gsl_matrix_calloc(feenox.pde.dofs, feenox.pde.dofs));
    
      // stresses
//      feenox_check_alloc(mechanical.PK1 = gsl_matrix_calloc(feenox.pde.dofs, feenox.pde.dofs));
      feenox_check_alloc(mechanical.PK2 = gsl_matrix_calloc(feenox.pde.dofs, feenox.pde.dofs));
      feenox_check_alloc(mechanical.PK2_voigt = gsl_vector_calloc(mechanical.stress_strain_size));
      feenox_check_alloc(mechanical.S = gsl_matrix_calloc(feenox.pde.dofs, feenox.pde.dofs));
    }

  }
  
  
  return FEENOX_OK;
}

int feenox_problem_build_volumetric_forces(element_t *e, unsigned int q, double wdet, double *x) {
  // volumetric force densities in the reference configuration
  if (mechanical.f_x.defined || mechanical.f_y.defined || mechanical.f_z.defined) {
    if (x == NULL) {
      x = feenox_fem_compute_x_at_gauss(e, q, feenox.pde.mesh->integration);           
    }
    material_t *material = feenox_fem_get_material(e);
      
    gsl_matrix *H = feenox_fem_compute_H_c_at_gauss(e, q, feenox.pde.mesh->integration);
    double f_x = mechanical.f_x.eval(&mechanical.f_x, x, material);
    double f_y = mechanical.f_y.eval(&mechanical.f_y, x, material);
    double f_z = mechanical.f_z.eval(&mechanical.f_z, x, material);

    for (int j = 0; j < e->type->nodes; j++) {
      int offset = feenox.pde.dofs*j;
      // TODO: matrix-vector product
      double wh = wdet * gsl_matrix_get(H, 0, j);
      if (mechanical.f_x.defined) {
        gsl_vector_add_to_element(feenox.fem.bi, offset+0, wh * f_x);
      }  
      if (mechanical.f_y.defined) {
        gsl_vector_add_to_element(feenox.fem.bi, offset+1, wh * f_y);
      }
      if (mechanical.f_z.defined) {
        gsl_vector_add_to_element(feenox.fem.bi, offset+2, wh * f_z);
      }  
    }
  }
  
  return FEENOX_OK;
}


// linear small deformation
int feenox_problem_build_volumetric_gauss_point_mechanical(element_t *e, unsigned int q) {
  
#ifdef HAVE_PETSC
  
  double *x = NULL;
  if (mechanical.uniform_C == 0) {
    // material stress-strain relationship
    x = feenox_fem_compute_x_at_gauss(e, q, feenox.pde.mesh->integration);
    feenox_call(mechanical.compute_material_tangent(x, feenox_fem_get_material(e)));
  }
  
  gsl_matrix *dhdx = feenox_fem_compute_B_at_gauss_integration(e, q, feenox.pde.mesh->integration);
  for (unsigned int j = 0; j < mechanical.n_nodes; j++) {
    // TODO: virtual methods? they cannot be inlined...
    if (mechanical.variant == variant_full) {
      
      gsl_matrix_set(mechanical.B_shape, 0, 3*j+0, gsl_matrix_get(dhdx, 0, j));
      gsl_matrix_set(mechanical.B_shape, 1, 3*j+1, gsl_matrix_get(dhdx, 1, j));
      gsl_matrix_set(mechanical.B_shape, 2, 3*j+2, gsl_matrix_get(dhdx, 2, j));
    
      gsl_matrix_set(mechanical.B_shape, 3, 3*j+0, gsl_matrix_get(dhdx, 1, j));
      gsl_matrix_set(mechanical.B_shape, 3, 3*j+1, gsl_matrix_get(dhdx, 0, j));

      gsl_matrix_set(mechanical.B_shape, 4, 3*j+1, gsl_matrix_get(dhdx, 2, j));
      gsl_matrix_set(mechanical.B_shape, 4, 3*j+2, gsl_matrix_get(dhdx, 1, j));

      gsl_matrix_set(mechanical.B_shape, 5, 3*j+0, gsl_matrix_get(dhdx, 2, j));
      gsl_matrix_set(mechanical.B_shape, 5, 3*j+2, gsl_matrix_get(dhdx, 0, j));

    } else if (mechanical.variant == variant_axisymmetric) {
      
      feenox_push_error_message("axisymmetric still not implemented");
      return FEENOX_ERROR;
      
    } else if (mechanical.variant == variant_plane_stress || mechanical.variant == variant_plane_strain) {
      
      // plane stress and plane strain are the same
      // see equation 14.18 IFEM CH.14 sec 14.4.1 pag 14-11
      gsl_matrix_set(mechanical.B_shape, 0, 2*j+0, gsl_matrix_get(dhdx, 0, j));
      gsl_matrix_set(mechanical.B_shape, 1, 2*j+1, gsl_matrix_get(dhdx, 1, j));
    
      gsl_matrix_set(mechanical.B_shape, 2, 2*j+0, gsl_matrix_get(dhdx, 1, j));
      gsl_matrix_set(mechanical.B_shape, 2, 2*j+1, gsl_matrix_get(dhdx, 0, j));

    } else {
      return FEENOX_ERROR;
    }
  }

  // wdet
  double wdet = feenox_fem_compute_w_det_at_gauss_integration(e, q, feenox.pde.mesh->integration);
  
  // volumetric forces
  feenox_call(feenox_problem_build_volumetric_forces(e, q, wdet, x));

  // elemental stiffness B'*C*B
  feenox_call(feenox_blas_BtCB_accum(mechanical.B_shape, mechanical.C_tangent, mechanical.CB, wdet, feenox.fem.Ki));

  // thermal expansion strain vector
  if (mechanical.thermal_expansion_model != thermal_expansion_model_none) {
    if (x == NULL) {
      x = feenox_fem_compute_x_at_gauss(e, q, feenox.pde.mesh->integration);    
    }
    mechanical.compute_thermal_strain(x, feenox_fem_get_material(e));
    
    feenox_call(feenox_blas_Atb(mechanical.C_tangent, mechanical.et, 1.0, mechanical.Cet));
    feenox_call(feenox_blas_Atb_accum(mechanical.B_shape, mechanical.Cet, wdet, feenox.fem.bi));
  }
  
#endif
  
  return FEENOX_OK;
}

// nonlinear large deformation
int feenox_problem_build_volumetric_gauss_point_mechanical_nonlinear(element_t *e, unsigned int q) {
  
#ifdef HAVE_PETSC
  double *x = feenox_fem_compute_x_at_gauss(e, q, feenox.pde.mesh->integration);
  material_t *material = feenox_fem_get_material(e);
  
  gsl_matrix *dhdx = feenox_fem_compute_B_at_gauss_integration(e, q, feenox.pde.mesh->integration);
  mechanical.grad_u = feenox_problem_mechanical_compute_gradient_displacement(dhdx, feenox.fem.phii);
  mechanical.F = feenox_problem_mechanical_compute_gradient_deformation(mechanical.grad_u);
  mechanical.C = feenox_problem_mechanical_compute_strain_cauchy_green_left(mechanical.F);
  mechanical.eps = feenox_problem_mechanical_compute_strain_green_lagrange(mechanical.C);  
  mechanical.PK2 = mechanical.compute_PK2(x, material);
 
  // residual = B^T*PK_voigt
  // material stiffness = B^T*C*B
  // geometric stiffness = G^T*S*G
  
  double Sxx = gsl_matrix_get(mechanical.PK2, 0, 0);
  double Syy = gsl_matrix_get(mechanical.PK2, 1, 1);
  double Szz = gsl_matrix_get(mechanical.PK2, 2, 2);
  double Sxy = gsl_matrix_get(mechanical.PK2, 0, 1);
  double Syz = gsl_matrix_get(mechanical.PK2, 1, 2);
  double Szx = gsl_matrix_get(mechanical.PK2, 2, 0);

  // here voigt is not Landau's lexicographical but the cyclic ordering
  gsl_vector_set(mechanical.PK2_voigt, 0, Sxx);
  gsl_vector_set(mechanical.PK2_voigt, 1, Syy);
  gsl_vector_set(mechanical.PK2_voigt, 2, Szz);
  gsl_vector_set(mechanical.PK2_voigt, 3, Sxy);
  gsl_vector_set(mechanical.PK2_voigt, 4, Syz);
  gsl_vector_set(mechanical.PK2_voigt, 5, Szx);
  
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
    
  
  for (unsigned int j = 0; j < mechanical.n_nodes; j++) {
    gsl_matrix_set(mechanical.B_shape, 0, 3*j+0, gsl_matrix_get(dhdx, 0, j) * gsl_matrix_get(mechanical.F, 0, 0));
    gsl_matrix_set(mechanical.B_shape, 0, 3*j+1, gsl_matrix_get(dhdx, 0, j) * gsl_matrix_get(mechanical.F, 1, 0));
    gsl_matrix_set(mechanical.B_shape, 0, 3*j+2, gsl_matrix_get(dhdx, 0, j) * gsl_matrix_get(mechanical.F, 2, 0));
    
    gsl_matrix_set(mechanical.B_shape, 1, 3*j+0, gsl_matrix_get(dhdx, 1, j) * gsl_matrix_get(mechanical.F, 0, 1));
    gsl_matrix_set(mechanical.B_shape, 1, 3*j+1, gsl_matrix_get(dhdx, 1, j) * gsl_matrix_get(mechanical.F, 1, 1));
    gsl_matrix_set(mechanical.B_shape, 1, 3*j+2, gsl_matrix_get(dhdx, 1, j) * gsl_matrix_get(mechanical.F, 2, 1));

    gsl_matrix_set(mechanical.B_shape, 2, 3*j+0, gsl_matrix_get(dhdx, 2, j) * gsl_matrix_get(mechanical.F, 0, 2));
    gsl_matrix_set(mechanical.B_shape, 2, 3*j+1, gsl_matrix_get(dhdx, 2, j) * gsl_matrix_get(mechanical.F, 1, 2));
    gsl_matrix_set(mechanical.B_shape, 2, 3*j+2, gsl_matrix_get(dhdx, 2, j) * gsl_matrix_get(mechanical.F, 2, 2));
    
    gsl_matrix_set(mechanical.B_shape, 3, 3*j+0, gsl_matrix_get(dhdx, 1, j) * gsl_matrix_get(mechanical.F, 0, 0) + gsl_matrix_get(dhdx, 0, j) * gsl_matrix_get(mechanical.F, 0, 1));
    gsl_matrix_set(mechanical.B_shape, 3, 3*j+1, gsl_matrix_get(dhdx, 1, j) * gsl_matrix_get(mechanical.F, 1, 0) + gsl_matrix_get(dhdx, 0, j) * gsl_matrix_get(mechanical.F, 1, 1));
    gsl_matrix_set(mechanical.B_shape, 3, 3*j+2, gsl_matrix_get(dhdx, 1, j) * gsl_matrix_get(mechanical.F, 2, 0) + gsl_matrix_get(dhdx, 0, j) * gsl_matrix_get(mechanical.F, 2, 1));

    gsl_matrix_set(mechanical.B_shape, 4, 3*j+0, gsl_matrix_get(dhdx, 2, j) * gsl_matrix_get(mechanical.F, 0, 1) + gsl_matrix_get(dhdx, 1, j) * gsl_matrix_get(mechanical.F, 0, 2));
    gsl_matrix_set(mechanical.B_shape, 4, 3*j+1, gsl_matrix_get(dhdx, 2, j) * gsl_matrix_get(mechanical.F, 1, 1) + gsl_matrix_get(dhdx, 1, j) * gsl_matrix_get(mechanical.F, 1, 2));
    gsl_matrix_set(mechanical.B_shape, 4, 3*j+2, gsl_matrix_get(dhdx, 2, j) * gsl_matrix_get(mechanical.F, 2, 1) + gsl_matrix_get(dhdx, 1, j) * gsl_matrix_get(mechanical.F, 2, 2));

    gsl_matrix_set(mechanical.B_shape, 5, 3*j+0, gsl_matrix_get(dhdx, 2, j) * gsl_matrix_get(mechanical.F, 0, 0) + gsl_matrix_get(dhdx, 0, j) * gsl_matrix_get(mechanical.F, 0, 2));
    gsl_matrix_set(mechanical.B_shape, 5, 3*j+1, gsl_matrix_get(dhdx, 2, j) * gsl_matrix_get(mechanical.F, 1, 0) + gsl_matrix_get(dhdx, 0, j) * gsl_matrix_get(mechanical.F, 1, 2));
    gsl_matrix_set(mechanical.B_shape, 5, 3*j+2, gsl_matrix_get(dhdx, 2, j) * gsl_matrix_get(mechanical.F, 2, 0) + gsl_matrix_get(dhdx, 0, j) * gsl_matrix_get(mechanical.F, 2, 2));
    
    // the matrix of shape function derivatives G
    gsl_matrix_set(mechanical.G, 0, 3*j+0, gsl_matrix_get(dhdx, 0, j));
    gsl_matrix_set(mechanical.G, 1, 3*j+1, gsl_matrix_get(dhdx, 0, j));
    gsl_matrix_set(mechanical.G, 2, 3*j+2, gsl_matrix_get(dhdx, 0, j));
    
    gsl_matrix_set(mechanical.G, 3, 3*j+0, gsl_matrix_get(dhdx, 1, j));
    gsl_matrix_set(mechanical.G, 4, 3*j+1, gsl_matrix_get(dhdx, 1, j));
    gsl_matrix_set(mechanical.G, 5, 3*j+2, gsl_matrix_get(dhdx, 1, j));

    gsl_matrix_set(mechanical.G, 6, 3*j+0, gsl_matrix_get(dhdx, 2, j));
    gsl_matrix_set(mechanical.G, 7, 3*j+1, gsl_matrix_get(dhdx, 2, j));
    gsl_matrix_set(mechanical.G, 8, 3*j+2, gsl_matrix_get(dhdx, 2, j));
  }
  
  // wdet
  double wdet = feenox_fem_compute_w_det_at_gauss_integration(e, q, feenox.pde.mesh->integration);
  
  // external volumetric forces
  feenox_call(feenox_problem_build_volumetric_forces(e, q, wdet, x));
  
  // internal non-linear force
  feenox_call(feenox_blas_Atb_accum(mechanical.B_shape, mechanical.PK2_voigt, wdet, feenox.fem.fi));
  
  // elemental stiffness B'*C*B
  feenox_call(feenox_blas_BtCB_accum(mechanical.B_shape, mechanical.C_tangent, mechanical.CB, wdet, feenox.fem.Ki));
  
  // elemental geometric stiffness G'*S*G
  feenox_call(feenox_blas_BtCB_accum(mechanical.G, mechanical.Sigma, mechanical.SigmaG, wdet, feenox.fem.JKi));


//#define VERBOSE  
#ifdef VERBOSE  
  printf("B\n");
  feenox_debug_print_gsl_matrix(mechanical.B_shape, stdout);

  printf("C\n");
  feenox_debug_print_gsl_matrix(mechanical.C_tangent, stdout);
  
  printf("K\n");
  feenox_debug_print_gsl_matrix(feenox.fem.Ki, stdout);

  printf("G\n");
  feenox_debug_print_gsl_matrix(mechanical.G, stdout);

  printf("Sigma\n");
  feenox_debug_print_gsl_matrix(mechanical.Sigma, stdout);
  
  printf("JK\n");
  feenox_debug_print_gsl_matrix(feenox.fem.JKi, stdout);
#endif  
  
  
#endif
  
  return FEENOX_OK;
}
