#include "feenox.h"
#include "thermal.h"
extern feenox_t feenox;
extern thermal_t thermal;

int feenox_problem_gradient_fill_thermal(void) {
  
  feenox_gradient_fill(thermal, qx);
  if (feenox.pde.dofs > 1) {
    feenox_gradient_fill(thermal, qy);
    if (feenox.pde.dofs > 2) {
      feenox_gradient_fill(thermal, qz);
    }
  }
  
  return FEENOX_OK;
  
}


int feenox_problem_gradient_compute_at_nodes_thermal(void) {
  
  
  return FEENOX_OK;
}