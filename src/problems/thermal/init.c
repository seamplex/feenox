#include "feenox.h"
extern feenox_t feenox;

int feenox_problem_init_parser_thermal(void) {
  
  feenox.pde.dofs = 1;
  feenox_check_alloc(feenox.pde.unknown_name = calloc(feenox.pde.dofs, sizeof(char *)));
  feenox_check_alloc(feenox.pde.unknown_name[0] = strdup("T"));
  
  feenox_call(feenox_problem_define_solutions());
  
///va+T_max+name T_max
///va+T_max+detail The maximum temperature\ $T_\text{max}$ of the thermal problem.
  feenox.pde.vars.T_max = feenox_define_variable_get_ptr("T_max");

///va+T_min+name T_min
///va+T_min+detail The minimum temperature\ $T_\text{min}$ of the thermal problem.
  feenox.pde.vars.T_min = feenox_define_variable_get_ptr("T_min");
  
  
  return FEENOX_OK;
}

int feenox_problem_init_runtime_thermal(void) {
  
  // check if we were given an initial solution
  if ((feenox.pde.initial_condition = feenox_get_function_ptr("T_0")) != NULL) {
    if (feenox.pde.initial_condition->n_arguments != feenox.pde.dim) {
      feenox_push_error_message("initial condition function T_0 ought to have %d arguments instead of %d", feenox.pde.dim, feenox.pde.initial_condition->n_arguments);
      return FEENOX_OK;
    }
  }

  // we are FEM not FVM
  feenox.pde.spatial_unknowns = feenox.pde.mesh->n_nodes;
  feenox.pde.mesh->data_type = data_type_node;
  feenox.pde.global_size = feenox.pde.spatial_unknowns * feenox.pde.dofs;
  
  
  // TODO: either use the setting or figure out the dependence of properties & BCs with T
  // TODO: check end_time and quasistatic
  feenox.pde.has_mass = PETSC_FALSE;
  feenox.pde.has_rhs = PETSC_TRUE;
  feenox.pde.solve_petsc = feenox_solve_petsc_linear;

  return FEENOX_OK;
}
