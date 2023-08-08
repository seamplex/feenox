/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox routines to build elemental mechanical objects
 *
 *  Copyright (C) 2021--2023 jeremy theler
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
  
  if (mechanical.CB != NULL) {
    gsl_matrix_free(mechanical.CB);
  }
  feenox_check_alloc(mechanical.CB = gsl_matrix_calloc(mechanical.stress_strain_size, feenox.pde.dofs * mechanical.n_nodes));

  return FEENOX_OK;
}



int feenox_problem_build_volumetric_gauss_point_mechanical(element_t *e, unsigned int q) {
  
#ifdef HAVE_PETSC
  
  if (mechanical.n_nodes != e->type->nodes) {
    feenox_call(feenox_problem_build_allocate_aux_mechanical(e->type->nodes));
  }
  
  double *x = NULL;
  if (mechanical.uniform_C == 0) {
    // material stress-strain relationship
    double *x = feenox_fem_compute_x_at_gauss(e, q, feenox.pde.mesh->integration);
    mechanical.compute_C(x, feenox_fem_get_material(e));
  }
  
  gsl_matrix *dhdx = feenox_fem_compute_B_at_gauss(e, q, feenox.pde.mesh->integration);
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
  double wdet = feenox_fem_compute_w_det_at_gauss(e, q, feenox.pde.mesh->integration);
  
  // volumetric force densities
  if (mechanical.f_x.defined || mechanical.f_y.defined || mechanical.f_z.defined) {
    if (x == NULL) {
      x = feenox_fem_compute_x_at_gauss(e, q, feenox.pde.mesh->integration);           
    }
    material_t *material = feenox_fem_get_material(e);
      
    gsl_matrix *H = e->type->gauss[feenox.pde.mesh->integration].H_c[q]; 
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
  feenox_call(gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, mechanical.C, mechanical.B, 0, mechanical.CB));
  feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, wdet, mechanical.B, mechanical.CB, 1.0, feenox.fem.Ki));

  // thermal expansion strain vector
  if (mechanical.thermal_expansion_model != thermal_expansion_model_none) {
    if (x == NULL) {
      x = feenox_fem_compute_x_at_gauss(e, q, feenox.pde.mesh->integration);    
    }
    mechanical.compute_thermal_strain(x, feenox_fem_get_material(e));
    
    feenox_call(gsl_blas_dgemv(CblasTrans, 1.0, mechanical.C, mechanical.et, 0, mechanical.Cet));
    feenox_call(gsl_blas_dgemv(CblasTrans, wdet, mechanical.B, mechanical.Cet, 1.0, feenox.fem.bi));
  }
  
#endif
  
  return FEENOX_OK;
}
