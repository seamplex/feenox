#include "feenox.h"
extern feenox_t feenox;
extern double zero[3];

int feenox_problem_bc_natural_set(element_t *e, unsigned int v, double *value) {

#ifdef HAVE_PETSC
  for (unsigned int g = 0; g < feenox.pde.dofs; g++) {
    gsl_vector_set(feenox.pde.Nb, g, value[g]);
  }  
  double w = feenox_problem_bc_natural_weight(e, v);
  feenox_call(gsl_blas_dgemv(CblasTrans, w, e->H[v], feenox.pde.Nb, 1.0, feenox.pde.bi));
#endif
  
  return FEENOX_OK;
}

double feenox_problem_bc_natural_weight(element_t *e, unsigned int v) {
  feenox_mesh_compute_wH_at_gauss(e, v);

  // TODO: axisymmetric
//  r_for_axisymmetric = feenox_compute_r_for_axisymmetric(this, v);
  double r_for_axisymmetric = 1;
  return e->w[v] * r_for_axisymmetric;
}

double *feenox_problem_bc_natural_x(element_t *e, bc_data_t *bc_data, unsigned int v) {
  double *x = zero;
  if (bc_data->space_dependent) {
    feenox_mesh_compute_x_at_gauss(e, v, feenox.pde.mesh->integration);
    x = e->x[v];
    feenox_mesh_update_coord_vars(x);
  }
  
  return x;
}

