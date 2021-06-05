#include "feenox.h"
#include "mechanical.h"
extern feenox_t feenox;
extern mechanical_t mechanical;

int feenox_problem_bc_parse_mechanical(bc_data_t *bc_data, const char *lhs, const char *rhs) {
  return FEENOX_OK;
}
