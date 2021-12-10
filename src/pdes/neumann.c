#include "feenox.h"
extern feenox_t feenox;
extern double zero[3];

int feenox_problem_bc_natural_set(element_t *element, unsigned int v, double *value) {

#ifdef HAVE_PETSC
  for (unsigned int g = 0; g < feenox.pde.dofs; g++) {
    gsl_vector_set(feenox.pde.Nb, g, value[g]);
  }  
  double w = feenox_problem_bc_natural_weight(element, v);
  feenox_call(gsl_blas_dgemv(CblasTrans, w, element->H[v], feenox.pde.Nb, 1.0, feenox.pde.bi));
#endif
  
  return FEENOX_OK;
}

double feenox_problem_bc_natural_weight(element_t *element, unsigned int v) {
  feenox_mesh_compute_w_at_gauss(element, v, feenox.pde.mesh->integration);
  feenox_mesh_compute_H_at_gauss(element, v, feenox.pde.dofs, feenox.pde.mesh->integration);

  // TODO: axisymmetric
//  r_for_axisymmetric = feenox_compute_r_for_axisymmetric(this, v);
  double r_for_axisymmetric = 1;
  return element->w[v] * r_for_axisymmetric;
}

double *feenox_problem_bc_natural_x(element_t *element, bc_data_t *bc_data, unsigned int v) {
  double *x = zero;
  if (bc_data->space_dependent) {
    feenox_mesh_compute_x_at_gauss(element, v, feenox.pde.mesh->integration);
    x = element->x[v];
    feenox_mesh_update_coord_vars(x);
  }
  
  return x;
}

