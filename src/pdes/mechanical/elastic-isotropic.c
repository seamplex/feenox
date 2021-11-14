#include "feenox.h"
#include "mechanical.h"
extern feenox_t feenox;
extern mechanical_t mechanical;

int feenox_problem_build_compute_mechanical_C_elastic_isotropic(const double *x, material_t *material) {
  
  double E = mechanical.E.eval(&mechanical.E, x, material);
  double nu = mechanical.nu.eval(&mechanical.nu, x, material);
  double lambda = E*nu/((1+nu)*(1-2*nu));
  double mu = 0.5*E/(1+nu);
  double lambda2mu = lambda + 2*mu;
  
  gsl_matrix_set(mechanical.C, 0, 0, lambda2mu);
  gsl_matrix_set(mechanical.C, 0, 1, lambda);
  gsl_matrix_set(mechanical.C, 0, 2, lambda);

  gsl_matrix_set(mechanical.C, 1, 0, lambda);
  gsl_matrix_set(mechanical.C, 1, 1, lambda2mu);
  gsl_matrix_set(mechanical.C, 1, 2, lambda);

  gsl_matrix_set(mechanical.C, 2, 0, lambda);
  gsl_matrix_set(mechanical.C, 2, 1, lambda);
  gsl_matrix_set(mechanical.C, 2, 2, lambda2mu);
  
  gsl_matrix_set(mechanical.C, 3, 3, mu);
  gsl_matrix_set(mechanical.C, 4, 4, mu);
  gsl_matrix_set(mechanical.C, 5, 5, mu);
    
  return FEENOX_OK;
}
