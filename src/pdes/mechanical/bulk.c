#include "feenox.h"
#include "mechanical.h"
extern feenox_t feenox;
extern mechanical_t mechanical;
extern double zero[3];

int feenox_problem_build_volumetric_gauss_point_mechanical(element_t *this, unsigned int v) {
  
#ifdef HAVE_PETSC
  
  feenox_call(feenox_mesh_compute_w_at_gauss(this, v, feenox.pde.mesh->integration));
  feenox_call(feenox_mesh_compute_H_at_gauss(this, v, feenox.pde.dofs, feenox.pde.mesh->integration));
  feenox_call(feenox_mesh_compute_B_at_gauss(this, v, feenox.pde.dofs, feenox.pde.mesh->integration));
  
  double *x = zero;
  // TODO: compute once
  if (mechanical.E.space_dependent || mechanical.nu.space_dependent || mechanical.alpha.space_dependent || mechanical.T.space_dependent) {
    feenox_call(feenox_mesh_compute_x_at_gauss(this, v, feenox.pde.mesh->integration));
    x = this->x[v];
  }
  
  // TODO: axisymmetric
//  r_for_axisymmetric = feenox_compute_r_for_axisymmetric(this, v);
  double r_for_axisymmetric = 1;
  double w = this->w[v] * r_for_axisymmetric;
  
  material_t *material = NULL;
  if (this->physical_group != NULL && this->physical_group->material != NULL) {
    material = this->physical_group->material;
  }

  // TODO: static allocation, realloc only if J changes
  unsigned int J = this->type->nodes;
  static size_t stress_strain_size = 0;
  if (mechanical.variant == variant_full) {
    stress_strain_size = 6;
  } else if (mechanical.variant == variant_axisymmetric) {
    stress_strain_size = 4;
  } else if (mechanical.variant == variant_plane_stress || mechanical.variant == variant_plane_strain) {
    stress_strain_size = 3;
  } else {
    return FEENOX_ERROR;
  }
  
  gsl_matrix *B = gsl_matrix_calloc(stress_strain_size, feenox.pde.dofs * J);
  gsl_matrix *CB = gsl_matrix_calloc(stress_strain_size, feenox.pde.dofs * this->type->nodes);
  gsl_matrix *C = gsl_matrix_calloc(stress_strain_size, stress_strain_size);
  
  // TODO: cache constant properties
  double E = mechanical.E.eval(&mechanical.E, x, material);
  double nu = mechanical.nu.eval(&mechanical.nu, x, material);
  
  double lambda = E*nu/((1+nu)*(1-2*nu));
  double mu = 0.5*E/(1+nu);
  double lambda2mu = lambda + 2*mu;
  
  if (mechanical.variant == variant_full) {
    gsl_matrix_set(C, 0, 0, lambda2mu);
    gsl_matrix_set(C, 0, 1, lambda);
    gsl_matrix_set(C, 0, 2, lambda);

    gsl_matrix_set(C, 1, 0, lambda);
    gsl_matrix_set(C, 1, 1, lambda2mu);
    gsl_matrix_set(C, 1, 2, lambda);

    gsl_matrix_set(C, 2, 0, lambda);
    gsl_matrix_set(C, 2, 1, lambda);
    gsl_matrix_set(C, 2, 2, lambda2mu);
  
    gsl_matrix_set(C, 3, 3, mu);
    gsl_matrix_set(C, 4, 4, mu);
    gsl_matrix_set(C, 5, 5, mu);
  } else if (mechanical.variant == variant_plane_stress) {
    
    double c1 = E/(1-nu*nu);
    double c2 = nu * c1;
    gsl_matrix_set(C, 0, 0, c1);
    gsl_matrix_set(C, 0, 1, c2);
    
    gsl_matrix_set(C, 1, 0, c2);
    gsl_matrix_set(C, 1, 1, c1);

    gsl_matrix_set(C, 2, 2, c1*0.5*(1-nu));
    
  } else if (mechanical.variant == variant_plane_strain) {
    
    gsl_matrix_set(C, 0, 0, lambda2mu);
    gsl_matrix_set(C, 0, 1, lambda);
    
    gsl_matrix_set(C, 1, 0, lambda);
    gsl_matrix_set(C, 1, 1, lambda2mu);

    gsl_matrix_set(C, 2, 2, mu);
  } else {
    return FEENOX_ERROR;
  }
  

  unsigned int j = 0;
  gsl_matrix *dhdx = this->dhdx[v];
  for (j = 0; j < J; j++) {
    // TODO: virtual methods
    if (mechanical.variant == variant_full) {
      gsl_matrix_set(B, 0, 3*j+0, gsl_matrix_get(dhdx, j, 0));
      
      gsl_matrix_set(B, 1, 3*j+1, gsl_matrix_get(dhdx, j, 1));
      
      gsl_matrix_set(B, 2, 3*j+2, gsl_matrix_get(dhdx, j, 2));
    
      gsl_matrix_set(B, 3, 3*j+0, gsl_matrix_get(dhdx, j, 1));
      gsl_matrix_set(B, 3, 3*j+1, gsl_matrix_get(dhdx, j, 0));

      gsl_matrix_set(B, 4, 3*j+1, gsl_matrix_get(dhdx, j, 2));
      gsl_matrix_set(B, 4, 3*j+2, gsl_matrix_get(dhdx, j, 1));

      gsl_matrix_set(B, 5, 3*j+0, gsl_matrix_get(dhdx, j, 2));
      gsl_matrix_set(B, 5, 3*j+2, gsl_matrix_get(dhdx, j, 0));
    } else if (mechanical.variant == variant_axisymmetric) {
      ;
    } else if (mechanical.variant == variant_plane_stress || mechanical.variant == variant_plane_strain) {
      // plane stress and plane strain are the same
      // see equation 14.18 IFEM CH.14 sec 14.4.1 pag 14-11
      gsl_matrix_set(B, 0, 2*j+0, gsl_matrix_get(dhdx, j, 0));
      
      gsl_matrix_set(B, 1, 2*j+1, gsl_matrix_get(dhdx, j, 1));
    
      gsl_matrix_set(B, 2, 2*j+0, gsl_matrix_get(dhdx, j, 1));
      gsl_matrix_set(B, 2, 2*j+1, gsl_matrix_get(dhdx, j, 0));
    } else {
      return FEENOX_ERROR;
    }
  }

  // elemental stiffness B'*C*B
  feenox_call(gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, C, B, 0, CB));
  feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, w, B, CB, 1.0, feenox.pde.Ki));

  // thermal expansion vector
  if (mechanical.alpha.defined) {
    // TODO: static allocation
    gsl_vector *et = NULL;
    feenox_check_alloc(et = gsl_vector_calloc(stress_strain_size));
    
    gsl_vector *Cet = NULL;
    feenox_check_alloc(Cet = gsl_vector_calloc(stress_strain_size));
    
    double alpha = mechanical.alpha.eval(&mechanical.alpha, x, material);
    if (alpha != 0) {
      
      double alpha_delta_T = alpha * (mechanical.T.eval(&mechanical.T, x, material) - mechanical.T0);
      gsl_vector_set(et, 0, alpha_delta_T);
      if (feenox.pde.dim > 1) {
        gsl_vector_set(et, 1, alpha_delta_T);
        if (feenox.pde.dim > 2) {
          gsl_vector_set(et, 2, alpha_delta_T);
        }  
      }  
      // feenox_call()
      gsl_blas_dgemv(CblasTrans, 1.0, C, et, 0, Cet);
      gsl_blas_dgemv(CblasTrans, w, B, Cet, 1.0, feenox.pde.bi);
    }  
    
    gsl_vector_free(et);
    gsl_vector_free(Cet);
  }  
  
  // TODO: rhs with volumetric sources
  
  gsl_matrix_free(B);
  gsl_matrix_free(CB);
  gsl_matrix_free(C);
  
#endif
  
  return FEENOX_OK;
}
