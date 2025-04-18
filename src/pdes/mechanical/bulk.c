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
  
  if (mechanical.B != NULL) {
    gsl_matrix_free(mechanical.B);
  }
  feenox_check_alloc(mechanical.B = gsl_matrix_calloc(mechanical.stress_strain_size, feenox.pde.dofs * mechanical.n_nodes));
  
  // this is a temporary holder to compute things like BtCB
  if (mechanical.CB != NULL) {
    gsl_matrix_free(mechanical.CB);
  }
  feenox_check_alloc(mechanical.CB = gsl_matrix_calloc(mechanical.stress_strain_size, feenox.pde.dofs * mechanical.n_nodes));

  // --- ldef stuff ------------------
  
  // displacement gradient
  if (mechanical.grad_u == NULL) {
    feenox_check_alloc(mechanical.grad_u = gsl_matrix_calloc(feenox.pde.dofs, feenox.pde.dofs));
  }
  
  // 3x3 identity (the symbol I is already taken by complex.h)
  if (mechanical.eye == NULL) {
    feenox_check_alloc(mechanical.eye = gsl_matrix_calloc(feenox.pde.dofs, feenox.pde.dofs));
    gsl_matrix_set_identity(mechanical.eye);
  }
  
  // deformation gradient
  if (mechanical.F == NULL) {
    feenox_check_alloc(mechanical.F = gsl_matrix_calloc(feenox.pde.dofs, feenox.pde.dofs));
  }
  
  // green-lagrange strain tensor
  if (mechanical.EGL == NULL) {
    feenox_check_alloc(mechanical.EGL = gsl_matrix_calloc(feenox.pde.dofs, feenox.pde.dofs));
  }

  // second piola-kirchoff stress tensor
  if (mechanical.S == NULL) {
    feenox_check_alloc(mechanical.S = gsl_matrix_calloc(feenox.pde.dofs, feenox.pde.dofs));
  }
  
  if (mechanical.S_voigt == NULL) {
    feenox_check_alloc(mechanical.S_voigt = gsl_vector_calloc(mechanical.stress_strain_size));
  }
  
  if (mechanical.Sigma == NULL) {
    feenox_check_alloc(mechanical.Sigma = gsl_matrix_calloc(feenox.pde.dofs*feenox.pde.dofs, feenox.pde.dofs*feenox.pde.dofs));
  }

  // matrix with shape derivativesp
  if (mechanical.G != NULL) {
    gsl_matrix_free(mechanical.G);
  }
  feenox_check_alloc(mechanical.G = gsl_matrix_calloc(feenox.pde.dofs*feenox.pde.dofs, feenox.pde.dofs * mechanical.n_nodes));
  
  // this is a temporary holder to compute GtSigmaG
  if (mechanical.SigmaG != NULL) {
    gsl_matrix_free(mechanical.SigmaG);
  }
  feenox_check_alloc(mechanical.SigmaG = gsl_matrix_calloc(feenox.pde.dofs*feenox.pde.dofs, feenox.pde.dofs * mechanical.n_nodes));
  
  
  
  return FEENOX_OK;
}

// linear small deformation
int feenox_problem_build_volumetric_gauss_point_mechanical(element_t *e, unsigned int q) {
  
#ifdef HAVE_PETSC
  
  double *x = NULL;
  if (mechanical.uniform_C == 0) {
    // material stress-strain relationship
    x = feenox_fem_compute_x_at_gauss(e, q, feenox.pde.mesh->integration);
    mechanical.compute_C(x, feenox_fem_get_material(e));
  }
  
  gsl_matrix *dhdx = feenox_fem_compute_B_at_gauss_integration(e, q, feenox.pde.mesh->integration);
  for (unsigned int j = 0; j < mechanical.n_nodes; j++) {
    // TODO: virtual methods? they cannot be inlined...
    if (mechanical.variant == variant_full) {
      
      gsl_matrix_set(mechanical.B, 0, 3*j+0, gsl_matrix_get(dhdx, 0, j));
      gsl_matrix_set(mechanical.B, 1, 3*j+1, gsl_matrix_get(dhdx, 1, j));
      gsl_matrix_set(mechanical.B, 2, 3*j+2, gsl_matrix_get(dhdx, 2, j));
    
      gsl_matrix_set(mechanical.B, 3, 3*j+0, gsl_matrix_get(dhdx, 1, j));
      gsl_matrix_set(mechanical.B, 3, 3*j+1, gsl_matrix_get(dhdx, 0, j));

      gsl_matrix_set(mechanical.B, 4, 3*j+1, gsl_matrix_get(dhdx, 2, j));
      gsl_matrix_set(mechanical.B, 4, 3*j+2, gsl_matrix_get(dhdx, 1, j));

      gsl_matrix_set(mechanical.B, 5, 3*j+0, gsl_matrix_get(dhdx, 2, j));
      gsl_matrix_set(mechanical.B, 5, 3*j+2, gsl_matrix_get(dhdx, 0, j));
      
    } else if (mechanical.variant == variant_axisymmetric) {
      
      feenox_push_error_message("axisymmetric still not implemented");
      return FEENOX_ERROR;
      
    } else if (mechanical.variant == variant_plane_stress || mechanical.variant == variant_plane_strain) {
      
      // plane stress and plane strain are the same
      // see equation 14.18 IFEM CH.14 sec 14.4.1 pag 14-11
      gsl_matrix_set(mechanical.B, 0, 2*j+0, gsl_matrix_get(dhdx, 0, j));
      gsl_matrix_set(mechanical.B, 1, 2*j+1, gsl_matrix_get(dhdx, 1, j));
    
      gsl_matrix_set(mechanical.B, 2, 2*j+0, gsl_matrix_get(dhdx, 1, j));
      gsl_matrix_set(mechanical.B, 2, 2*j+1, gsl_matrix_get(dhdx, 0, j));

    } else {
      return FEENOX_ERROR;
    }
  }

  // wdet
  double wdet = feenox_fem_compute_w_det_at_gauss_integration(e, q, feenox.pde.mesh->integration);
  
  // volumetric force densities
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

  // elemental stiffness B'*C*B
  feenox_call(feenox_blas_BtCB_accum(mechanical.B, mechanical.C, mechanical.CB, wdet, feenox.fem.Ki));

  // thermal expansion strain vector
  if (mechanical.thermal_expansion_model != thermal_expansion_model_none) {
    if (x == NULL) {
      x = feenox_fem_compute_x_at_gauss(e, q, feenox.pde.mesh->integration);    
    }
    mechanical.compute_thermal_strain(x, feenox_fem_get_material(e));
    
    feenox_call(feenox_blas_Atb(mechanical.C, mechanical.et, 1.0, mechanical.Cet));
    feenox_call(feenox_blas_Atb_accum(mechanical.B, mechanical.Cet, wdet, feenox.fem.bi));
  }
  
#endif
  
  return FEENOX_OK;
}

// nonlinear large deformation
int feenox_problem_build_volumetric_gauss_point_mechanical_nonlinear(element_t *e, unsigned int q) {
  
#ifdef HAVE_PETSC

  gsl_matrix *dhdx = feenox_fem_compute_B_at_gauss_integration(e, q, feenox.pde.mesh->integration);
  
  // grad_u = displacement gradient
  gsl_matrix_set_zero(mechanical.grad_u);
  for (int row = 0; row < 3; row++) {
    for (int col = 0; col < 3; col++) {
      for (int j = 0; j < mechanical.n_nodes; j++) {
        gsl_matrix_add_to_element(mechanical.grad_u, row, col, gsl_matrix_get(dhdx, col, j) * gsl_vector_get(feenox.fem.phii, 3*j + row));
      }
    }
  }
  
  // F = I + grad_u     deformation gradient  
  gsl_matrix_memcpy(mechanical.F, mechanical.eye);
  gsl_matrix_add(mechanical.F, mechanical.grad_u);

  // EGL = 1/2 * (FtF - I)   green-lagrange strain tensor  
  feenox_blas_BtB(mechanical.F, 1.0, mechanical.EGL);
  gsl_matrix_sub(mechanical.EGL, mechanical.eye);
  gsl_matrix_scale(mechanical.EGL, 0.5);
  
  // second piola kirchoff stress tensor
  // TODO: improve with proper material models
  gsl_matrix_set_identity(mechanical.S);
  double young = mechanical.E.eval(&mechanical.E, NULL, NULL);
  double nu = mechanical.nu.eval(&mechanical.nu, NULL, NULL);
  double lambda = young*nu/((1+nu)*(1-2*nu));
  double mu = 0.5*young/(1+nu);
  double trE = gsl_matrix_get(mechanical.EGL, 0, 0) + gsl_matrix_get(mechanical.EGL, 1, 1) + gsl_matrix_get(mechanical.EGL, 2, 2);
  gsl_matrix_scale(mechanical.S, lambda * trE);
  
  gsl_matrix *twomuE = gsl_matrix_calloc(3, 3);
  gsl_matrix_memcpy(twomuE, mechanical.EGL);
  gsl_matrix_scale(twomuE, 2*mu);
  gsl_matrix_add(mechanical.S, twomuE);
  gsl_matrix_free(twomuE);
  
  gsl_vector_set(mechanical.S_voigt, 0, gsl_matrix_get(mechanical.S, 0, 0));
  gsl_vector_set(mechanical.S_voigt, 1, gsl_matrix_get(mechanical.S, 1, 1));
  gsl_vector_set(mechanical.S_voigt, 2, gsl_matrix_get(mechanical.S, 2, 2));
  gsl_vector_set(mechanical.S_voigt, 3, gsl_matrix_get(mechanical.S, 0, 1));
  gsl_vector_set(mechanical.S_voigt, 4, gsl_matrix_get(mechanical.S, 1, 2));
  gsl_vector_set(mechanical.S_voigt, 5, gsl_matrix_get(mechanical.S, 2, 0));

  for (unsigned int j = 0; j < mechanical.n_nodes; j++) {
    // strain-displacement matrix B
    gsl_matrix_set(mechanical.B, 0, 3*j+0, gsl_matrix_get(dhdx, 0, j) * gsl_matrix_get(mechanical.F, 0, 0));
    gsl_matrix_set(mechanical.B, 0, 3*j+1, gsl_matrix_get(dhdx, 0, j) * gsl_matrix_get(mechanical.F, 1, 0));
    gsl_matrix_set(mechanical.B, 0, 3*j+2, gsl_matrix_get(dhdx, 0, j) * gsl_matrix_get(mechanical.F, 2, 0));
    
    gsl_matrix_set(mechanical.B, 1, 3*j+0, gsl_matrix_get(dhdx, 1, j) * gsl_matrix_get(mechanical.F, 0, 1));
    gsl_matrix_set(mechanical.B, 1, 3*j+1, gsl_matrix_get(dhdx, 1, j) * gsl_matrix_get(mechanical.F, 1, 1));
    gsl_matrix_set(mechanical.B, 1, 3*j+2, gsl_matrix_get(dhdx, 1, j) * gsl_matrix_get(mechanical.F, 2, 1));

    gsl_matrix_set(mechanical.B, 2, 3*j+0, gsl_matrix_get(dhdx, 2, j) * gsl_matrix_get(mechanical.F, 0, 2));
    gsl_matrix_set(mechanical.B, 2, 3*j+1, gsl_matrix_get(dhdx, 2, j) * gsl_matrix_get(mechanical.F, 1, 2));
    gsl_matrix_set(mechanical.B, 2, 3*j+2, gsl_matrix_get(dhdx, 2, j) * gsl_matrix_get(mechanical.F, 2, 2));
    
    gsl_matrix_set(mechanical.B, 3, 3*j+0, gsl_matrix_get(dhdx, 1, j) * gsl_matrix_get(mechanical.F, 0, 0) + gsl_matrix_get(dhdx, 0, j) * gsl_matrix_get(mechanical.F, 0, 1));
    gsl_matrix_set(mechanical.B, 3, 3*j+1, gsl_matrix_get(dhdx, 1, j) * gsl_matrix_get(mechanical.F, 1, 0) + gsl_matrix_get(dhdx, 0, j) * gsl_matrix_get(mechanical.F, 1, 1));
    gsl_matrix_set(mechanical.B, 3, 3*j+2, gsl_matrix_get(dhdx, 1, j) * gsl_matrix_get(mechanical.F, 2, 0) + gsl_matrix_get(dhdx, 0, j) * gsl_matrix_get(mechanical.F, 2, 1));

    gsl_matrix_set(mechanical.B, 4, 3*j+0, gsl_matrix_get(dhdx, 2, j) * gsl_matrix_get(mechanical.F, 0, 1) + gsl_matrix_get(dhdx, 1, j) * gsl_matrix_get(mechanical.F, 0, 2));
    gsl_matrix_set(mechanical.B, 4, 3*j+1, gsl_matrix_get(dhdx, 2, j) * gsl_matrix_get(mechanical.F, 1, 1) + gsl_matrix_get(dhdx, 1, j) * gsl_matrix_get(mechanical.F, 1, 2));
    gsl_matrix_set(mechanical.B, 4, 3*j+2, gsl_matrix_get(dhdx, 2, j) * gsl_matrix_get(mechanical.F, 2, 1) + gsl_matrix_get(dhdx, 1, j) * gsl_matrix_get(mechanical.F, 2, 2));

    gsl_matrix_set(mechanical.B, 5, 3*j+0, gsl_matrix_get(dhdx, 2, j) * gsl_matrix_get(mechanical.F, 0, 0) + gsl_matrix_get(dhdx, 0, j) * gsl_matrix_get(mechanical.F, 0, 2));
    gsl_matrix_set(mechanical.B, 5, 3*j+1, gsl_matrix_get(dhdx, 2, j) * gsl_matrix_get(mechanical.F, 1, 0) + gsl_matrix_get(dhdx, 0, j) * gsl_matrix_get(mechanical.F, 1, 2));
    gsl_matrix_set(mechanical.B, 5, 3*j+2, gsl_matrix_get(dhdx, 2, j) * gsl_matrix_get(mechanical.F, 2, 0) + gsl_matrix_get(dhdx, 0, j) * gsl_matrix_get(mechanical.F, 2, 2));
    
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
  
  // the 9x9 Sigma matrix
  double Sxx = gsl_vector_get(mechanical.S_voigt, 0);
  double Syy = gsl_vector_get(mechanical.S_voigt, 1);
  double Szz = gsl_vector_get(mechanical.S_voigt, 2);
  double Sxy = gsl_vector_get(mechanical.S_voigt, 3);
  double Syz = gsl_vector_get(mechanical.S_voigt, 4);
  double Szx = gsl_vector_get(mechanical.S_voigt, 5);

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
  
  // wdet
  double wdet = feenox_fem_compute_w_det_at_gauss_integration(e, q, feenox.pde.mesh->integration);
  
  // elemental stiffness B'*C*B
  feenox_call(feenox_blas_BtCB_accum(mechanical.B, mechanical.C, mechanical.CB, wdet, feenox.fem.Ki));
  
  // elemental geometric stiffness G'*S*G
  feenox_call(feenox_blas_BtCB_accum(mechanical.G, mechanical.Sigma, mechanical.SigmaG, wdet, feenox.fem.JKi));
  
  // force
  feenox_call(feenox_blas_Atb_accum(mechanical.B, mechanical.S_voigt, wdet, feenox.fem.fi));
  
#endif
  
  return FEENOX_OK;
}
