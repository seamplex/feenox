#include "feenox.h"
#include "mechanical.h"
extern feenox_t feenox;
extern mechanical_t mechanical;

int feenox_problem_bc_parse_mechanical(bc_data_t *bc_data, const char *lhs, const char *rhs) {
  return FEENOX_OK;
}

int feenox_problem_bc_set_dirichlet_mechanical(bc_data_t *bc_data, size_t j, size_t *k) {
  return FEENOX_OK;
}
