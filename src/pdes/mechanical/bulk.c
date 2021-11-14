#include "feenox.h"
#include "mechanical.h"
extern feenox_t feenox;
extern mechanical_t mechanical;

int feenox_problem_build_allocate_aux_mechanical(size_t n_nodes) {
  
  mechanical.n_nodes = n_nodes;
  
  feenox_free(mechanical.B);
  feenox_check_alloc(mechanical.B = gsl_matrix_calloc(mechanical.stress_strain_size, feenox.pde.dofs * mechanical.n_nodes));
  
  feenox_free(mechanical.CB);
  feenox_check_alloc(mechanical.CB = gsl_matrix_calloc(mechanical.stress_strain_size, feenox.pde.dofs * mechanical.n_nodes));

  return FEENOX_OK;
}



int feenox_problem_build_volumetric_gauss_point_mechanical(element_t *this, unsigned int v) {
  
#ifdef HAVE_PETSC
  
  feenox_call(feenox_mesh_compute_w_at_gauss(this, v, feenox.pde.mesh->integration));
  feenox_call(feenox_mesh_compute_H_at_gauss(this, v, feenox.pde.dofs, feenox.pde.mesh->integration));
  feenox_call(feenox_mesh_compute_B_at_gauss(this, v, feenox.pde.dofs, feenox.pde.mesh->integration));
  
  double *x = NULL;
  if (mechanical.space_dependent_properties == PETSC_TRUE) {
    feenox_call(feenox_mesh_compute_x_at_gauss(this, v, feenox.pde.mesh->integration));
    x = this->x[v];
  }
  
  // TODO: axisymmetric
//  r_for_axisymmetric = feenox_compute_r_for_axisymmetric(this, v);
//  double r_for_axisymmetric = 1;
//  double w = this->w[v] * r_for_axisymmetric;
  
  material_t *material = NULL;
  if (this->physical_group != NULL && this->physical_group->material != NULL) {
    material = this->physical_group->material;
  }

  if (mechanical.n_nodes != this->type->nodes) {
    feenox_call(feenox_problem_build_allocate_aux_mechanical(this->type->nodes));
  }

  // material stress-strain relationship
  if (mechanical.E.variable == NULL || mechanical.nu.variable == NULL) {
    mechanical.compute_C(x, material);
  }  
  
  

  unsigned int j = 0;
  gsl_matrix *dhdx = this->dhdx[v];
  for (j = 0; j < mechanical.n_nodes; j++) {
    // TODO: virtual methods
    if (mechanical.variant == variant_full) {
      
      gsl_matrix_set(mechanical.B, 0, 3*j+0, gsl_matrix_get(dhdx, j, 0));
      gsl_matrix_set(mechanical.B, 1, 3*j+1, gsl_matrix_get(dhdx, j, 1));
      gsl_matrix_set(mechanical.B, 2, 3*j+2, gsl_matrix_get(dhdx, j, 2));
    
      gsl_matrix_set(mechanical.B, 3, 3*j+0, gsl_matrix_get(dhdx, j, 1));
      gsl_matrix_set(mechanical.B, 3, 3*j+1, gsl_matrix_get(dhdx, j, 0));

      gsl_matrix_set(mechanical.B, 4, 3*j+1, gsl_matrix_get(dhdx, j, 2));
      gsl_matrix_set(mechanical.B, 4, 3*j+2, gsl_matrix_get(dhdx, j, 1));

      gsl_matrix_set(mechanical.B, 5, 3*j+0, gsl_matrix_get(dhdx, j, 2));
      gsl_matrix_set(mechanical.B, 5, 3*j+2, gsl_matrix_get(dhdx, j, 0));
      
    } else if (mechanical.variant == variant_axisymmetric) {
      
      feenox_push_error_message("axisymmetric still not implemented");
      return FEENOX_ERROR;
      
    } else if (mechanical.variant == variant_plane_stress || mechanical.variant == variant_plane_strain) {
      
      // plane stress and plane strain are the same
      // see equation 14.18 IFEM CH.14 sec 14.4.1 pag 14-11
      gsl_matrix_set(mechanical.B, 0, 2*j+0, gsl_matrix_get(dhdx, j, 0));
      gsl_matrix_set(mechanical.B, 1, 2*j+1, gsl_matrix_get(dhdx, j, 1));
    
      gsl_matrix_set(mechanical.B, 2, 2*j+0, gsl_matrix_get(dhdx, j, 1));
      gsl_matrix_set(mechanical.B, 2, 2*j+1, gsl_matrix_get(dhdx, j, 0));

    } else {
      return FEENOX_ERROR;
    }
  }

  // elemental stiffness B'*C*B
  feenox_call(gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, mechanical.C, mechanical.B, 0, mechanical.CB));
  feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, this->w[v], mechanical.B, mechanical.CB, 1.0, feenox.pde.Ki));

  // thermal expansion vector
  if (mechanical.alpha.defined) {
    // TODO: cache with C
    double alpha = mechanical.alpha.eval(&mechanical.alpha, x, material);
    if (alpha != 0) {
      double alpha_delta_T = alpha * (mechanical.T.eval(&mechanical.T, x, material) - mechanical.T0);
      gsl_vector_set(mechanical.et, 0, alpha_delta_T);
      gsl_vector_set(mechanical.et, 1, alpha_delta_T);
      if (feenox.pde.dim > 2) {
        gsl_vector_set(mechanical.et, 2, alpha_delta_T);
      }  
      
      feenox_call(gsl_blas_dgemv(CblasTrans, 1.0, mechanical.C, mechanical.et, 0, mechanical.Cet));
      feenox_call(gsl_blas_dgemv(CblasTrans, this->w[v], mechanical.B, mechanical.Cet, 1.0, feenox.pde.bi));
    }  
  }  
  
  // TODO: rhs with volumetric sources
  // TODO: cleanup aux matrices C, B and CB
  
#endif
  
  return FEENOX_OK;
}
