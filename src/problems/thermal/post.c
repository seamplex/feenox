#include "feenox.h"
#include "thermal.h"
extern feenox_t feenox;
extern thermal_t thermal;

int feenox_problem_solve_post_thermal(void) {

  double T = 0;
  double T_max = -INFTY;
  double T_min = +INFTY;
  
  size_t j = 0;
  for (j = 0; j < feenox.pde.mesh->n_nodes; j++) {
    T = feenox.pde.solution[0]->data_value[j];
    if (T > T_max) {
      T_max = T;
    }
    if (T < T_min) {
      T_min = T;
    }
  }  
  
  feenox_var_value(thermal.T_max) = T_max;
  feenox_var_value(thermal.T_min) = T_min;
  

  return FEENOX_OK;
}
