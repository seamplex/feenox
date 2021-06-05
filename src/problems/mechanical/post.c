#include "feenox.h"
#include "mechanical.h"
extern feenox_t feenox;
extern mechanical_t mechanical;

int feenox_problem_solve_post_mechanical(void) {

#ifdef HAVE_SLEPC
#endif
  return FEENOX_OK;
}