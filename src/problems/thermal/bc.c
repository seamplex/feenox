#include "feenox.h"
#include "thermal.h"
extern feenox_t feenox;

int feenox_problem_bc_parse_thermal(bc_data_t *bc_data, const char *lhs, const char *rhs) {

  if (strcmp(lhs, "T") == 0) {
    bc_data->type_math = bc_type_math_dirichlet;
    // TODO: see how to set the physical type
    // with defines in a thermal.h? we can also use that for material distributions
    // bc_data->type_phys = 1;
    bc_data->dof = 0;
    feenox_check_alloc(bc_data->expr = calloc(1, sizeof(expr_t)));
    feenox_call(feenox_expression_parse(bc_data->expr, rhs));
    
  } else {
    // TODO: for weak bcs, check if the value depends on T so the problem can be marked non-linear automatically
    //       mind the fact that if q=f(x,y,z) then f(x,y,z) can depend on T(x,y,z) itself
    feenox_push_error_message("unknown thermal boundary condition '%s'", lhs);
    return FEENOX_ERROR;
  }
  
  
  return FEENOX_OK;
}


int feenox_problem_bc_set_dirichlet_thermal(bc_data_t *bc_data, size_t j, size_t k) {
#ifdef HAVE_PETSC
  feenox.pde.dirichlet_indexes[k] = feenox.pde.mesh->node[j].index_dof[bc_data->dof];
  feenox.pde.dirichlet_values[k] = feenox_expression_eval(&bc_data->expr[0]);
#endif
  return FEENOX_OK;
}
