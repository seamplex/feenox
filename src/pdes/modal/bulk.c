/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for modal analysis: bulk elements
 *
 *  Copyright (C) 2021 jeremy theler
 *
 *  This file is part of FeenoX <https://www.seamplex.com/feenox>.
 *
 *  feenox is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  FeenoX is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with FeenoX.  If not, see <http://www.gnu.org/licenses/>.
 *------------------- ------------  ----    --------  --     -       -         -
 */
#include "feenox.h"
#include "modal.h"

int feenox_problem_build_allocate_aux_modal(unsigned int n_nodes) {
  
  modal.n_nodes = n_nodes;
  
  if (modal.B != NULL) {
    gsl_matrix_free(modal.B);
  }
  feenox_check_alloc(modal.B = gsl_matrix_calloc(modal.stress_strain_size, feenox.pde.dofs * modal.n_nodes));
  
  if (modal.CB != NULL) {
    gsl_matrix_free(modal.CB);
  }
  feenox_check_alloc(modal.CB = gsl_matrix_calloc(modal.stress_strain_size, feenox.pde.dofs * modal.n_nodes));

  return FEENOX_OK;
}

int feenox_problem_build_volumetric_gauss_point_modal(element_t *e, unsigned int q) {

#ifdef HAVE_PETSC
  
//  feenox_call(feenox_mesh_compute_wHB_at_gauss(e, q));
  material_t *material = feenox_fem_get_material(e);
  
  if (modal.n_nodes != e->type->nodes) {
    feenox_call(feenox_problem_build_allocate_aux_modal(e->type->nodes));
  }
  
  if (modal.uniform_C == 0) {
    // material stress-strain relationship
    feenox_fem_compute_x_at_gauss(e, q, feenox.pde.mesh->integration);
    modal.compute_C(e->x[q], material);
  }
  
  // TODO: unify with mechanical!
  gsl_matrix *dhdx = e->B[q];
  for (int j = 0; j < modal.n_nodes; j++) {
    // TODO: virtual methods? they cannot be inlined...
    if (modal.variant == variant_full) {
      
      gsl_matrix_set(modal.B, 0, 3*j+0, gsl_matrix_get(dhdx, 0, j));
      gsl_matrix_set(modal.B, 1, 3*j+1, gsl_matrix_get(dhdx, 1, j));
      gsl_matrix_set(modal.B, 2, 3*j+2, gsl_matrix_get(dhdx, 2, j));
    
      gsl_matrix_set(modal.B, 3, 3*j+0, gsl_matrix_get(dhdx, 1, j));
      gsl_matrix_set(modal.B, 3, 3*j+1, gsl_matrix_get(dhdx, 0, j));

      gsl_matrix_set(modal.B, 4, 3*j+1, gsl_matrix_get(dhdx, 2, j));
      gsl_matrix_set(modal.B, 4, 3*j+2, gsl_matrix_get(dhdx, 1, j));

      gsl_matrix_set(modal.B, 5, 3*j+0, gsl_matrix_get(dhdx, 2, j));
      gsl_matrix_set(modal.B, 5, 3*j+2, gsl_matrix_get(dhdx, 0, j));
      
    } else if (modal.variant == variant_axisymmetric) {
      
      feenox_push_error_message("axisymmetric still not implemented");
      return FEENOX_ERROR;
      
    } else if (modal.variant == variant_plane_stress || modal.variant == variant_plane_strain) {
      
      // plane stress and plane strain are the same
      // see equation 14.18 IFEM CH.14 sec 14.4.1 pag 14-11
      gsl_matrix_set(modal.B, 0, 2*j+0, gsl_matrix_get(dhdx, 0, j));
      gsl_matrix_set(modal.B, 1, 2*j+1, gsl_matrix_get(dhdx, 1, j));
    
      gsl_matrix_set(modal.B, 2, 2*j+0, gsl_matrix_get(dhdx, 1, j));
      gsl_matrix_set(modal.B, 2, 2*j+1, gsl_matrix_get(dhdx, 0, j));

    } else {
      return FEENOX_ERROR;
    }
  }


  
  // elemental stiffness B'*C*B
  feenox_call(gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, modal.C, modal.B, 0, modal.CB));
  feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, e->w[q], modal.B, modal.CB, 1.0, feenox.pde.Ki));
  
  // elemental mass H'*rho*H
  if (modal.rho.uniform == 0) {
    double *x = feenox_fem_compute_x_at_gauss_if_needed(e, q, modal.space_rho);
    modal.rho.eval(&modal.rho, x, material);
  }
  feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, e->w[q] * modal.rho.value, e->type->H_Gc[q], e->type->H_Gc[q], 1.0, feenox.pde.Mi));
  
#endif
  
  return FEENOX_OK;
  
}

