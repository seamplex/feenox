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
    
  } else if (strcmp(lhs, "u") == 0 || strcmp(lhs, "u_x") == 0) {
    bc_data->type_phys = BC_TYPE_MECHANICAL_DISPLACEMENT;
    bc_data->type_math = bc_type_math_dirichlet;
    bc_data->dof = 0;
    
  } else if (strcmp(lhs, "v") == 0 || strcmp(lhs, "u_y") == 0) {
    bc_data->type_phys = BC_TYPE_MECHANICAL_DISPLACEMENT;
    bc_data->type_math = bc_type_math_dirichlet;
    bc_data->dof = 1;
    if (feenox.pde.dofs < 1) {
      feenox_push_error_message("cannot set u (displacement in y) with DOFs < 2");
      return FEENOX_ERROR;
    }

  } else if (strcmp(lhs, "w") == 0 || strcmp(lhs, "u_z") == 0) {
    bc_data->type_phys = BC_TYPE_MECHANICAL_DISPLACEMENT;
    bc_data->type_math = bc_type_math_dirichlet;
    bc_data->dof = 2;
    if (feenox.pde.dofs < 2) {
      feenox_push_error_message("cannot set w (displacement in z) with DOFs < 3");
      return FEENOX_ERROR;
    }

  } else if (strcmp(lhs, "p") == 0 || strcmp(lhs, "pressure") == 0) {
    bc_data->type_phys = BC_TYPE_MECHANICAL_PRESSURE_COMPRESSION;
    bc_data->type_math = bc_type_math_neumann;
    bc_data->set = feenox_problem_bc_set_mechanical_pressure;
    
    
  } else if (strcmp(lhs, "normal_tension") == 0) {
    bc_data->type_phys = BC_TYPE_MECHANICAL_PRESSURE_TRACTION;
    bc_data->type_math = bc_type_math_neumann;


  } else {
    feenox_push_error_message("unknown mechanical boundary condition '%s'", lhs);
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

int feenox_problem_bc_set_mechanical_displacement(bc_data_t *bc_data, size_t node_index) {
  
#ifdef HAVE_PETSC
  
  if (bc_data->dof != -1) {
    
    // only one dof
    feenox_call(feenox_dirichlet_add(feenox.pde.mesh->node[node_index].index_dof[bc_data->dof], feenox_expression_eval(&bc_data->expr)));
    
  } else {
    
    // fixed means homogeneous dirichlet bc
    feenox_call(feenox_dirichlet_add(feenox.pde.mesh->node[node_index].index_dof[0], 0));
    if (feenox.pde.dofs > 1) {
      feenox_call(feenox_dirichlet_add(feenox.pde.mesh->node[node_index].index_dof[1], 0));
      if (feenox.pde.dofs > 2) {
        feenox_call(feenox_dirichlet_add(feenox.pde.mesh->node[node_index].index_dof[2], 0));
      }
    }
    
  }  
  
#endif
  
  
  return FEENOX_OK;
}

// this virtual method builds the surface elemental matrix
// here "pressure" is taken positive as compression and negative as tension
int feenox_problem_bc_set_mechanical_pressure(element_t *element, bc_data_t *bc_data, unsigned int v) {

  // maybe this check can be made on the dimension of the physical entity at parse time
  if ((feenox.pde.dim - element->type->dim) != 1) {
    feenox_push_error_message("pressure BCs can only be applied to surfaces");
    return FEENOX_ERROR;
  }
  
#ifdef HAVE_PETSC


  // outward normal
  double n[3];
  feenox_call(feenox_mesh_compute_outward_normal(element, n));

  // TODO: cache if not space dependent
  if (bc_data->space_dependent) {
    feenox_mesh_compute_x_at_gauss(element, v, feenox.pde.mesh->integration);
    feenox_mesh_update_coord_vars(element->x[v]);
  }
  double p = feenox_expression_eval(&bc_data->expr);
  
  // remember that here p > 0 means compression
  double t[3];
  for (unsigned int g = 0; g < feenox.pde.dim; g++) {
    t[g] = -p * n[g];
  }
  
  feenox_call(feenox_problem_bc_natural_set(element, v, t));
  
#endif
  
  return FEENOX_OK;
}
