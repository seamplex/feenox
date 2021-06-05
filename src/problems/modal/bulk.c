#include "feenox.h"
#include "modal.h"
extern feenox_t feenox;
extern modal_t modal;
extern double zero[3];

int feenox_problem_build_volumetric_gauss_point_modal(element_t *this, unsigned int v) {

#ifdef HAVE_PETSC
  
  feenox_call(feenox_mesh_compute_w_at_gauss(this, v, feenox.pde.mesh->integration));
  feenox_call(feenox_mesh_compute_H_at_gauss(this, v, feenox.pde.dofs, feenox.pde.mesh->integration));
  feenox_call(feenox_mesh_compute_B_at_gauss(this, v, feenox.pde.dofs, feenox.pde.mesh->integration));
  
  double *x = zero;
  if (modal.space_E || modal.space_nu || modal.space_rho) {
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

  gsl_matrix *C = gsl_matrix_calloc(6, 6);
  double E = modal.E.eval(&modal.E, x, material);
  double nu = modal.nu.eval(&modal.nu, x, material);
  double rho = modal.rho.eval(&modal.rho, x, material);
  
  double lambda = E*nu/((1+nu)*(1-2*nu));
  double mu = 0.5*E/(1+nu);
  double lambda2mu = lambda + 2*mu;
  
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
  
  unsigned int J = this->type->nodes;
  gsl_matrix *B = gsl_matrix_calloc(6, feenox.pde.dofs * J);
  gsl_matrix *CB = gsl_matrix_calloc(6, feenox.pde.dofs * this->type->nodes);
  
  gsl_matrix *dhdx = this->dhdx[v];

  unsigned int j = 0;
  for (j = 0; j < J; j++) {
    gsl_matrix_set(B, 0, 3*j+0, gsl_matrix_get(dhdx, j, 0));
      
    gsl_matrix_set(B, 1, 3*j+1, gsl_matrix_get(dhdx, j, 1));
      
    gsl_matrix_set(B, 2, 3*j+2, gsl_matrix_get(dhdx, j, 2));
    
    gsl_matrix_set(B, 3, 3*j+0, gsl_matrix_get(dhdx, j, 1));
    gsl_matrix_set(B, 3, 3*j+1, gsl_matrix_get(dhdx, j, 0));

    gsl_matrix_set(B, 4, 3*j+1, gsl_matrix_get(dhdx, j, 2));
    gsl_matrix_set(B, 4, 3*j+2, gsl_matrix_get(dhdx, j, 1));

    gsl_matrix_set(B, 5, 3*j+0, gsl_matrix_get(dhdx, j, 2));
    gsl_matrix_set(B, 5, 3*j+2, gsl_matrix_get(dhdx, j, 0));
  }

  // elemental stiffness B'*C*B
  feenox_call(gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, C, B, 0, CB));
  feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, w, B, CB, 1.0, feenox.pde.Ki));

  // elemental mass H'*rho*H
  feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, w * rho, this->H[v], this->H[v], 1.0, feenox.pde.Mi));

  gsl_matrix_free(B);
  gsl_matrix_free(CB);
  gsl_matrix_free(C);
  
#endif
  
  return FEENOX_OK;
  
}



/*
int fino_print_gsl_vector(gsl_vector *b, FILE *file) {

  double xi;
  int i;

  for (i = 0; i < b->size; i++) {
    xi = gsl_vector_get(b, i);
    if (xi != 0) {
      fprintf(file, "% .1e ", xi);
    } else {
      fprintf(file, "    0    ");
    }
    fprintf(file, "\n");
  }
  
  return FEENOX_OK;

}

int fino_print_gsl_matrix(gsl_matrix *A, FILE *file) {

  double xi;
  int i, j;

  for (i = 0; i < A->size1; i++) {
    for (j = 0; j < A->size2; j++) {
      xi = gsl_matrix_get(A, i, j);
      fprintf(file, "% g ", (fabs(xi) > 1e-12)?xi:0.0);
    }
    fprintf(file, "\n");
  }
  
  return FEENOX_OK;

}
*/