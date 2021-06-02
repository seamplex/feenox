#include "feenox.h"
#include "thermal.h"
extern feenox_t feenox;
extern thermal_t thermal;

int feenox_problem_solve_post_thermal(void) {

#ifdef HAVE_PETSC
#endif

  return FEENOX_OK;
}
