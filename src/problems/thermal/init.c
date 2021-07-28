#include "feenox.h"
#include "thermal.h"
extern feenox_t feenox;
thermal_t thermal;

int feenox_problem_parse_thermal(const char *token) {

  // no need to parse anything;
  if (token != NULL) {
    feenox_push_error_message("undefined keyword '%s'", token);
    return FEENOX_ERROR;
  }
 
  return FEENOX_OK;
}


int feenox_problem_init_parser_thermal(void) {

#ifdef HAVE_PETSC
  feenox.pde.problem_init_runtime_particular = feenox_problem_init_runtime_thermal;
  feenox.pde.bc_parse = feenox_problem_bc_parse_thermal;
  feenox.pde.setup_ksp = feenox_problem_setup_ksp_thermal;
  feenox.pde.setup_pc = feenox_problem_setup_pc_thermal;
  feenox.pde.bc_set_dirichlet = feenox_problem_bc_set_thermal_temperature;
  feenox.pde.build_element_volumetric_gauss_point = feenox_problem_build_volumetric_gauss_point_thermal;
  feenox.pde.solve_post = feenox_problem_solve_post_thermal;
  
  // thermal is a scalar problem
  feenox.pde.dofs = 1;
  feenox_check_alloc(feenox.pde.unknown_name = calloc(feenox.pde.dofs, sizeof(char *)));
  
  // we are FEM not FVM
  feenox.pde.mesh->data_type = data_type_node;
  
  // TODO: choose uknown name
  feenox_check_alloc(feenox.pde.unknown_name[0] = strdup("T"));
  feenox.mesh.default_field_location = field_location_nodes;
  
  feenox_call(feenox_problem_define_solutions());
  
// TODO: document special variables
///va+T_max+name T_max
///va+T_max+detail The maximum temperature\ $T_\text{max}$ of the thermal problem.
//  feenox.pde.vars.T_max = feenox_define_variable_get_ptr("T_max");

///va+T_min+name T_min
///va+T_min+detail The minimum temperature\ $T_\text{min}$ of the thermal problem.
//  feenox.pde.vars.T_min = feenox_define_variable_get_ptr("T_min");
  
  // TODO: heat fluxes as seconday fields
#endif
  return FEENOX_OK;
}

int feenox_problem_init_runtime_thermal(void) {
  
#ifdef HAVE_PETSC

  feenox.pde.spatial_unknowns = feenox.pde.mesh->n_nodes;
  feenox.pde.global_size = feenox.pde.spatial_unknowns * feenox.pde.dofs;
  
  // check if we were given an initial solution
  if ((feenox.pde.initial_condition = feenox_get_function_ptr("T_0")) != NULL) {
    if (feenox.pde.initial_condition->n_arguments != feenox.pde.dim) {
      feenox_push_error_message("initial condition function T_0 ought to have %d arguments instead of %d", feenox.pde.dim, feenox.pde.initial_condition->n_arguments);
      return FEENOX_ERROR;
    }
  }

  // initialize distributions
  // TODO: document distributions with triple comments
  // here we just initialize everything, during build we know which
  // of them are mandatory and which are optional
  
  feenox_distribution_define_mandatory(thermal, k, "k", "thermal conductivity");
  thermal.k.space_dependent = feenox_expression_depends_on_space(thermal.k.dependency_variables);
  thermal.k.non_linear = feenox_expression_depends_on_function(thermal.k.dependency_functions, feenox.pde.solution[0]);  
  
  feenox_call(feenox_distribution_init(&thermal.q, "q'''"));
  if (thermal.q.defined == 0) {
    feenox_call(feenox_distribution_init(&thermal.q, "q"));
  }
  thermal.q.space_dependent = feenox_expression_depends_on_space(thermal.q.dependency_variables);
  thermal.q.non_linear = feenox_expression_depends_on_function(thermal.q.dependency_functions, feenox.pde.solution[0]);  
  
  feenox.pde.has_mass = (feenox_var_value(feenox_special_var(end_time)) > 0) ? PETSC_TRUE : PETSC_FALSE;
  if (feenox.pde.has_mass) {
    feenox_call(feenox_distribution_init(&thermal.kappa, "kappa"));
    if (thermal.kappa.defined == 0) {
      feenox_call(feenox_distribution_init(&thermal.rhocp, "rhocp"));
      if (thermal.rhocp.defined == 0) {
        feenox_call(feenox_distribution_init(&thermal.rho, "rho"));
        feenox_call(feenox_distribution_init(&thermal.cp, "cp"));

        if (thermal.rho.defined == 0 || thermal.cp.defined == 0) {
          feenox_push_error_message("either 'kappa', 'rhocp' or both 'rho' and 'cp' are needed for transient");
          return FEENOX_ERROR;
        }
        if (thermal.rho.full == 0 || thermal.cp.full == 0) {
          feenox_push_error_message("either 'rho' or 'cp' is not defined over all volumes");
          return FEENOX_ERROR;
        }
      } else if (thermal.rhocp.full == 0) {
        feenox_push_error_message("product 'rhocp' is not defined over all volumes");
        return FEENOX_ERROR;
      }
    } else if (thermal.kappa.full == 0) {
      feenox_push_error_message("thermal diffusivity 'kappa' is not defined over all volumes");
      return FEENOX_ERROR;
    }
  }

  thermal.kappa.space_dependent = feenox_expression_depends_on_space(thermal.kappa.dependency_variables);
  thermal.rho.space_dependent   = feenox_expression_depends_on_space(thermal.rho.dependency_variables);
  thermal.cp.space_dependent    = feenox_expression_depends_on_space(thermal.cp.dependency_variables);
  thermal.rhocp.space_dependent = feenox_expression_depends_on_space(thermal.rhocp.dependency_variables);

  thermal.kappa.non_linear      = feenox_expression_depends_on_function(thermal.kappa.dependency_functions,  feenox.pde.solution[0]);
  thermal.rho.non_linear        = feenox_expression_depends_on_function(thermal.rho.dependency_functions,    feenox.pde.solution[0]);
  thermal.cp.non_linear         = feenox_expression_depends_on_function(thermal.cp.dependency_functions,     feenox.pde.solution[0]);
  thermal.rhocp.non_linear      = feenox_expression_depends_on_function(thermal.rhocp.dependency_functions,  feenox.pde.solution[0]);
  
  thermal.space_k = thermal.k.space_dependent;
  thermal.space_q = thermal.q.space_dependent;
  thermal.space_m = thermal.kappa.space_dependent ||
                    thermal.rho.space_dependent   ||
                    thermal.cp.space_dependent    ||
                    thermal.rhocp.space_dependent;
  
  thermal.temperature_k = feenox_expression_depends_on_function(thermal.k.dependency_functions,     feenox.pde.solution[0]);
  thermal.temperature_m = feenox_expression_depends_on_function(thermal.kappa.dependency_functions, feenox.pde.solution[0]) ||
                          feenox_expression_depends_on_function(thermal.rho.dependency_functions,   feenox.pde.solution[0]) ||
                          feenox_expression_depends_on_function(thermal.cp.dependency_functions,    feenox.pde.solution[0]) ||
                          feenox_expression_depends_on_function(thermal.rhocp.dependency_functions, feenox.pde.solution[0]);
  
  // check BCs are consistent
  bc_t *bc = NULL;
  bc_t *bc_tmp = NULL;
  HASH_ITER(hh, feenox.mesh.bcs, bc, bc_tmp) {
    
    int first_type_math = bc_type_math_undefined;
    bc_data_t *bc_data = NULL;
    bc_data_t *bc_data_tmp = NULL;
    DL_FOREACH_SAFE(bc->bc_datums, bc_data, bc_data_tmp) {

      // this is general, maybe we can put it in a non-virtual method      
      if (first_type_math == bc_type_math_undefined) {
        first_type_math = bc_data->type_math;
      } else if (first_type_math != bc_data->type_math) {
        feenox_push_error_message("BCs of different types cannot be mixed '%s' and '%s'", bc->bc_datums->string, bc_data->string);
        return FEENOX_ERROR;
      }
      
      // convection (i.e. Robin) BCs have two arguments h and Tref,
      // we need to check they both are given (and only these two and nothing more)
      if (bc_data->type_math == bc_type_math_robin) {
        // first, check that there are only two bc_datums
        // utlist's doubly-linked lists work like this, sorry!
        if (bc_data == bc->bc_datums && bc_data->next == NULL) {
          feenox_push_error_message("both 'h' and 'Tref' have to be given in the same BC line");
          return FEENOX_ERROR;
        } else if (!(bc_data->prev == bc_data->next || bc_data->next == NULL)) {
          feenox_push_error_message("only 'h' and 'Tref' have to be given in the same BC line");
          return FEENOX_ERROR;
        }
        
        // check we have one h and one Tref
        if (bc_data == bc->bc_datums) {
          if ((bc_data->type_phys == BC_TYPE_THERMAL_CONVECTION_COEFFICIENT && bc_data->next->type_phys == BC_TYPE_THERMAL_CONVECTION_COEFFICIENT) ||
              (bc_data->type_phys == BC_TYPE_THERMAL_CONVECTION_TEMPERATURE && bc_data->next->type_phys == BC_TYPE_THERMAL_CONVECTION_TEMPERATURE)) {
            feenox_push_error_message("convection BC needs one 'h' and one 'Tref' in the same BC line");
            return FEENOX_ERROR;
          }
        } else if (bc_data->next == NULL) {
          // mark the second as disabled so only the first one is processed
          bc_data->disabled = 1;
        } else {
          feenox_push_error_message("internal mismatch in convection BC");
          return FEENOX_ERROR;
        }
        
      }
      
      bc_data->space_dependent = feenox_expression_depends_on_space(bc_data->expr.variables);
      thermal.space_bc |= bc_data->space_dependent;
      
      bc_data->nonlinear = feenox_expression_depends_on_function(bc_data->expr.functions, feenox.pde.solution[0]);
      thermal.temperature_bc |= bc_data->nonlinear;
      
    }
  }
  
  if (feenox.pde.math_type == math_type_automatic) {
    feenox.pde.math_type = (thermal.temperature_k == 0 &&
                            thermal.temperature_m == 0 &&
                            thermal.temperature_q == 0 &&
                           thermal.temperature_bc == 0) ? math_type_linear : math_type_nonlinear;
  }
  
  // TODO: check for transient_type
  feenox.pde.solve = (feenox_special_var_value(end_time) > 0) ? feenox_problem_solve_petsc_transient :
                         ((feenox.pde.math_type == math_type_linear) ? feenox_problem_solve_petsc_linear :
                                                                       feenox_problem_solve_petsc_nonlinear);
  
  feenox.pde.has_stiffness = PETSC_TRUE;
  feenox.pde.has_rhs = PETSC_TRUE;
  // has_mass is above
  
  feenox.pde.has_jacobian_K = thermal.temperature_k;
  feenox.pde.has_jacobian_M = thermal.temperature_m;
  feenox.pde.has_jacobian_b = (thermal.temperature_q || thermal.temperature_bc);
  feenox.pde.has_jacobian = feenox.pde.has_jacobian_K || feenox.pde.has_jacobian_M || feenox.pde.has_jacobian_b;
  
  feenox.pde.symmetric_K = PETSC_TRUE;
  feenox.pde.symmetric_M = PETSC_TRUE;
  
#endif  
  return FEENOX_OK;
}


#ifdef HAVE_PETSC
int feenox_problem_setup_pc_thermal(PC pc) {

  PCType pc_type = NULL;
  petsc_call(PCGetType(pc, &pc_type));
  if (pc_type == NULL) {
    petsc_call(PCSetType(pc, PCGAMG));
  }
  
  return FEENOX_OK;
}

int feenox_problem_setup_ksp_thermal(KSP ksp ) {

  KSPType ksp_type = NULL;
  petsc_call(KSPGetType(ksp, &ksp_type));
  if (ksp_type == NULL) {
    petsc_call(KSPSetType(ksp, KSPCG));
  }  

  return FEENOX_OK;
}
#endif
