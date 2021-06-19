#include "feenox.h"
#include "mechanical.h"
extern feenox_t feenox;
extern mechanical_t mechanical;

int feenox_problem_bc_parse_mechanical(bc_data_t *bc_data, const char *lhs, const char *rhs) {
  
  // TODO: document BCs with triple comments
  if (strcmp(lhs, "fixed") == 0) {
    bc_data->type_phys = BC_TYPE_MECHANICAL_DISPLACEMENT;
    bc_data->type_math = bc_type_math_dirichlet;
    bc_data->dof = -1;
    
  } else if (strcmp(lhs, "u") == 0) {
    bc_data->type_phys = BC_TYPE_MECHANICAL_DISPLACEMENT;
    bc_data->type_math = bc_type_math_dirichlet;
    bc_data->dof = 0;
    
  } else if (strcmp(lhs, "v") == 0) {
    bc_data->type_phys = BC_TYPE_MECHANICAL_DISPLACEMENT;
    bc_data->type_math = bc_type_math_dirichlet;
    bc_data->dof = 1;
    if (feenox.pde.dofs < 1) {
      feenox_push_error_message("cannot set u (displacement in y) with DOFs < 2");
      return FEENOX_ERROR;
    }

  } else if (strcmp(lhs, "w") == 0) {
    bc_data->type_phys = BC_TYPE_MECHANICAL_DISPLACEMENT;
    bc_data->type_math = bc_type_math_dirichlet;
    bc_data->dof = 2;
    if (feenox.pde.dofs < 2) {
      feenox_push_error_message("cannot set w (displacement in z) with DOFs < 3");
      return FEENOX_ERROR;
    }
    
  } else {
    feenox_push_error_message("unknown modal boundary condition '%s'", lhs);
    return FEENOX_ERROR;
  }
  
  if (rhs != NULL && strcmp(rhs, "0") != 0) {
    feenox_call(feenox_expression_parse(&bc_data->expr, rhs));
  }
  
  bc_data->space_dependent = feenox_expression_depends_on_space(bc_data->expr.variables);
  bc_data->nonlinear = feenox_expression_depends_on_function(bc_data->expr.functions, feenox.pde.solution[0]);
  if (feenox.pde.dofs > 1) {
    bc_data->nonlinear |= feenox_expression_depends_on_function(bc_data->expr.functions, feenox.pde.solution[1]);
    if (feenox.pde.dofs > 2) {
      bc_data->nonlinear |= feenox_expression_depends_on_function(bc_data->expr.functions, feenox.pde.solution[2]);
    }
  }

  if (bc_data->nonlinear && bc_data->type_phys == BC_TYPE_MECHANICAL_DISPLACEMENT) {
    feenox_push_error_message("strong boundary condition '%s' cannot depend on temperature", rhs);
    return FEENOX_ERROR;
  }
  
  return FEENOX_OK;
}

int feenox_problem_bc_set_dirichlet_mechanical(bc_data_t *bc_data, size_t j, size_t *k) {
  
#ifdef HAVE_PETSC
  
  if (bc_data->dof != -1) {
    
    feenox.pde.dirichlet_indexes[*k] = feenox.pde.mesh->node[j].index_dof[bc_data->dof];
    feenox.pde.dirichlet_values[*k] = feenox_expression_eval(&bc_data->expr);
    (*k) += 1; 
    
  } else {
    
    // fixed means homogeneous dirichlet bc
    feenox.pde.dirichlet_indexes[*k] = feenox.pde.mesh->node[j].index_dof[0];
    (*k) += 1; 
    if (feenox.pde.dofs > 1) {
      feenox.pde.dirichlet_indexes[*k] = feenox.pde.mesh->node[j].index_dof[1];
      (*k) += 1; 
      if (feenox.pde.dofs > 2) {
        feenox.pde.dirichlet_indexes[*k] = feenox.pde.mesh->node[j].index_dof[2];
        (*k) += 1; 
      }
    }
    
  }  
  
#endif
  
  return FEENOX_OK;
}
