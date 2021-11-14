#include "feenox.h"
#include "mechanical.h"
extern feenox_t feenox;
extern mechanical_t mechanical;

int feenox_problem_build_compute_mechanical_C_elastic_plane_stress(const double *x, material_t *material) {
  
  double E = mechanical.E.eval(&mechanical.E, x, material);
  double nu = mechanical.nu.eval(&mechanical.nu, x, material);
  
  double c1 = E/(1-nu*nu);
  double c2 = nu * c1;
  
  gsl_matrix_set(mechanical.C, 0, 0, c1);
  gsl_matrix_set(mechanical.C, 0, 1, c2);
    
  gsl_matrix_set(mechanical.C, 1, 0, c2);
  gsl_matrix_set(mechanical.C, 1, 1, c1);

  gsl_matrix_set(mechanical.C, 2, 2, c1*0.5*(1-nu));
    
  return FEENOX_OK;
}
