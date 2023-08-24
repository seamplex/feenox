/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for the heat equation: initialization
 *
 *  Copyright (C) 2021-2023 jeremy theler
 *
 *  This file is part of FeenoX <https://www.seamplex.com/feenox>.
 *
 *  feenox is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  FeenoX is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with FeenoX.  If not, see <http://www.gnu.org/licenses/>.
 *------------------- ------------  ----    --------  --     -       -         -
 */
#include "feenox.h"
#include "thermal.h"
thermal_t thermal;

int feenox_problem_parse_time_init_thermal(void) {

#ifdef HAVE_PETSC
  // virtual methods
  feenox.pde.parse_bc = feenox_problem_bc_parse_thermal;
  feenox.pde.parse_write_results = feenox_problem_parse_write_post_thermal;
  
  feenox.pde.init_before_run = feenox_problem_init_runtime_thermal;
  
  feenox.pde.setup_ksp = feenox_problem_setup_ksp_thermal;
  feenox.pde.setup_pc = feenox_problem_setup_pc_thermal;
  
  feenox.pde.element_build_volumetric_at_gauss = feenox_problem_build_volumetric_gauss_point_thermal;
  
  feenox.pde.solve_post = feenox_problem_solve_post_thermal;
  feenox.pde.gradient_fill = feenox_problem_gradient_fill_thermal;
  feenox.pde.gradient_nodal_properties = feenox_problem_gradient_properties_at_element_nodes_thermal;
  feenox.pde.gradient_alloc_nodal_fluxes = feenox_problem_gradient_fluxes_at_node_alloc_thermal;
  feenox.pde.gradient_add_elemental_contribution_to_node = feenox_problem_gradient_add_elemental_contribution_to_node_thermal;
  feenox.pde.gradient_fill_fluxes = feenox_problem_gradient_fill_fluxes_thermal;

  // we are FEM
  feenox.mesh.default_field_location = field_location_nodes;
  
  // thermal is a scalar problem
  feenox.pde.dofs = 1;
///re_thermal+T+description The temperature field\ $T(\vec{x})$. This is the primary unknown of the problem.  
  feenox_check_alloc(feenox.pde.unknown_name = calloc(feenox.pde.dofs, sizeof(char *)));
  feenox_check_alloc(feenox.pde.unknown_name[0] = strdup("T"));
  feenox_call(feenox_problem_define_solutions());
    
  // heat fluxes
///re_thermal+qx+description The heat flux field\ $q_x(\vec{x}) = -k(\vec{x}) \cdot \frac{\partial T}{\partial x}$ in the\ $x$ direction. This is a secondary unknown of the problem.  
///re_thermal+qy+description The heat flux field\ $q_y(\vec{x}) = -k(\vec{x}) \cdot \frac{\partial T}{\partial y}$ in the\ $x$ direction. This is a secondary unknown of the problem.
///re_thermal+qy+description Only available for two and three-dimensional problems.  
///re_thermal+qz+description The heat flux field\ $q_z(\vec{x}) = -k(\vec{x}) \cdot \frac{\partial T}{\partial z}$ in the\ $x$ direction. This is a secondary unknown of the problem.  
///re_thermal+qz+description Only available for three-dimensional problems.
  feenox_call(feenox_problem_define_solution_function("qx", &thermal.qx, FEENOX_SOLUTION_GRADIENT));
  if (feenox.pde.dim > 1) {
    feenox_call(feenox_problem_define_solution_function("qy", &thermal.qy, FEENOX_SOLUTION_GRADIENT));
    if (feenox.pde.dim > 2) {
      feenox_call(feenox_problem_define_solution_function("qz", &thermal.qz, FEENOX_SOLUTION_GRADIENT));
    }
  }

///pr_thermal+T_0+description The initial condition for the temperature in transient problems.
///pr_thermal+T_0+description If not given, a steady-steady computation at $t=0$ is performed.  

///pr_thermal+T_0+description The initial guess for the temperature in steady-state problems.
///pr_thermal+T_0+description If not given, a uniform distribution equal to the the average
///pr_thermal+T_0+description of all the temperature appearing in boundary conditions is used.
  
///va_thermal+T_max+detail The maximum temperature\ $T_\text{max}$.
  feenox_check_null(thermal.T_max = feenox_define_variable_get_ptr("T_max"));

///va_thermal+T_min+detail The minimum temperature\ $T_\text{min}$.
  feenox_check_null(thermal.T_min = feenox_define_variable_get_ptr("T_min"));
  
#endif
  return FEENOX_OK;
}

int feenox_problem_init_runtime_thermal(void) {
  
#ifdef HAVE_PETSC
  // we are FEM not FVM
  feenox.pde.mesh->data_type = data_type_node;
  feenox.pde.spatial_unknowns = feenox.pde.mesh->n_nodes;
  feenox.pde.size_global = feenox.pde.spatial_unknowns * feenox.pde.dofs;
  
  // check if we were given an initial guess
  if ((feenox.pde.initial_guess = feenox_get_function_ptr("T_guess")) != NULL) {
    if (feenox.pde.initial_guess->n_arguments != feenox.pde.dim) {
      feenox_push_error_message("intial guess function T_guess ought to have %d arguments instead of %d", feenox.pde.dim, feenox.pde.initial_condition->n_arguments);
      return FEENOX_ERROR;
    }
  }
  
  // check if we were given an initial solution
  if ((feenox.pde.initial_condition = feenox_get_function_ptr("T_0")) != NULL) {
    if (feenox.pde.initial_condition->n_arguments != feenox.pde.dim) {
      feenox_push_error_message("initial condition function T_0 ought to have %d arguments instead of %d", feenox.pde.dim, feenox.pde.initial_condition->n_arguments);
      return FEENOX_ERROR;
    }
  }

  // initialize distributions
  // here we just initialize everything, during build we know which
  // of them are mandatory and which are optional

///pr_thermal+k+usage k
///pr_thermal+k+description The thermal conductivity in units of power per length per degree of temperature.
///pr_thermal+k+description This property is mandatory.
  feenox_distribution_define_mandatory(thermal, k, "k", "thermal conductivity");
  thermal.k.uniform = feenox_expression_depends_on_space(thermal.k.dependency_variables);
  thermal.k.non_linear = feenox_expression_depends_on_function(thermal.k.dependency_functions, feenox.pde.solution[0]);  
  
  // TODO: orthotropic heat conduction
  
///pr_thermal+q'''+usage q'''
///pr_thermal+q'''+description The volumetric power dissipated in the material in units of power per unit of volume.
///pr_thermal+q'''+description Default is zero (i.e. no power).
  
  feenox_call(feenox_distribution_init(&thermal.q, "q'''"));
  if (thermal.q.defined == 0) {
///pr_thermal+q+usage q
///pr_thermal+q+description Alias for `q'''`
    feenox_call(feenox_distribution_init(&thermal.q, "q"));
  }
  thermal.q.uniform = feenox_expression_depends_on_space(thermal.q.dependency_variables);
  thermal.q.non_linear = feenox_expression_depends_on_function(thermal.q.dependency_functions, feenox.pde.solution[0]);  
  
  feenox.pde.has_mass = (feenox_var_value(feenox_special_var(end_time)) > 0);
  if (feenox.pde.has_mass) {
///pr_thermal+kappa+usage kappa
///pr_thermal+kappa+description Thermal diffusivity in units of area per unit of time. 
///pr_thermal+kappa+description Equal to the thermal conductivity `k` divided by the density `rho` and specific heat capacity `cp`.
///pr_thermal+kappa+description Either `kappa`, `rhocp` or both `rho` and `cp` are needed for transient
    feenox_call(feenox_distribution_init(&thermal.kappa, "kappa"));
    if (thermal.kappa.defined == 0) {
///pr_thermal+rhocp+usage rhocp
///pr_thermal+rhocp+description Product of the density `rho` times the specific heat capacity `cp`,
///pr_thermal+rhocp+description in units of energy per unit of volume per degree of temperature.
///pr_thermal+rhocp+description Either `kappa`, `rhocp` or both `rho` and `cp` are needed for transient
      feenox_call(feenox_distribution_init(&thermal.rhocp, "rhocp"));
      if (thermal.rhocp.defined == 0) {
///pr_thermal+rho+usage rho
///pr_thermal+rho+description Density in units of mass per unit of volume.
///pr_thermal+rho+description Either `kappa`, `rhocp` or both `rho` and `cp` are needed for transient
        feenox_call(feenox_distribution_init(&thermal.rho, "rho"));
///pr_thermal+cp+usage cp
///pr_thermal+cp+description Specific heat in units of energy per unit of mass per degree of temperature.
///pr_thermal+cp+description Either `kappa`, `rhocp` or both `rho` and `cp` are needed for transient
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

  thermal.kappa.uniform = feenox_expression_depends_on_space(thermal.kappa.dependency_variables);
  thermal.rho.uniform   = feenox_expression_depends_on_space(thermal.rho.dependency_variables);
  thermal.cp.uniform    = feenox_expression_depends_on_space(thermal.cp.dependency_variables);
  thermal.rhocp.uniform = feenox_expression_depends_on_space(thermal.rhocp.dependency_variables);

  thermal.kappa.non_linear      = feenox_expression_depends_on_function(thermal.kappa.dependency_functions,  feenox.pde.solution[0]);
  thermal.rho.non_linear        = feenox_expression_depends_on_function(thermal.rho.dependency_functions,    feenox.pde.solution[0]);
  thermal.cp.non_linear         = feenox_expression_depends_on_function(thermal.cp.dependency_functions,     feenox.pde.solution[0]);
  thermal.rhocp.non_linear      = feenox_expression_depends_on_function(thermal.rhocp.dependency_functions,  feenox.pde.solution[0]);
  
  thermal.space_dependent_stiffness = thermal.k.uniform ||
                            thermal.kx.uniform ||
                            thermal.ky.uniform ||
                            thermal.kz.uniform;
  thermal.space_dependent_source = thermal.q.uniform;
  thermal.space_dependent_mass = thermal.kappa.uniform ||
                       thermal.rho.uniform   ||
                       thermal.cp.uniform    ||
                       thermal.rhocp.uniform;
  
  thermal.temperature_dependent_stiffness = feenox_expression_depends_on_function(thermal.k.dependency_functions,     feenox.pde.solution[0]) ||
                                            feenox_expression_depends_on_function(thermal.kx.dependency_functions,    feenox.pde.solution[0]) ||
                                            feenox_expression_depends_on_function(thermal.ky.dependency_functions,    feenox.pde.solution[0]) ||
                                            feenox_expression_depends_on_function(thermal.kz.dependency_functions,    feenox.pde.solution[0]);
  thermal.temperature_dependent_mass      = feenox_expression_depends_on_function(thermal.kappa.dependency_functions, feenox.pde.solution[0]) ||
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
      thermal.space_dependent_bc |= bc_data->space_dependent;
      
      bc_data->nonlinear = feenox_expression_depends_on_function(bc_data->expr.functions, feenox.pde.solution[0]);
      thermal.temperature_dependent_bc |= bc_data->nonlinear;
      
    }
  }
  
  
  // if there is no initial guess, make up one
  if (feenox.pde.initial_guess == NULL && thermal.n_bc_temperatures != 0) {
    // average BC temperatures
    thermal.guessed_initial_guess /= thermal.n_bc_temperatures;
    
    if (feenox.pde.initial_guess == NULL) {
      feenox_check_null(feenox.pde.initial_guess = feenox_define_function_get_ptr("T_guess", feenox.pde.dim));
      feenox_call(feenox_function_set_argument_variable("T_guess", 0, "x"));
      if (feenox.pde.dim > 1) {
        feenox_call(feenox_function_set_argument_variable("T_guess", 1, "y"));
        if (feenox.pde.dim > 2) {
          feenox_call(feenox_function_set_argument_variable("T_guess", 2, "z"));
        }
      }
    }  
    char *evaluated_temp = NULL;
    feenox_check_minusone(asprintf(&evaluated_temp, "%g", thermal.guessed_initial_guess));
    feenox_call(feenox_function_set_expression("T_guess", evaluated_temp));
    feenox_free(evaluated_temp);
  }
  
  
  if (feenox.pde.math_type == math_type_automatic) {
    feenox.pde.math_type = (thermal.temperature_dependent_stiffness == 0 &&
                            thermal.temperature_dependent_mass      == 0 &&
                            thermal.temperature_dependent_source    == 0 &&
                            thermal.temperature_dependent_bc        == 0) ? math_type_linear : math_type_nonlinear;
  }
  
  // TODO: check for transient_type
  feenox.pde.solve = (feenox_special_var_value(end_time) > 0) ? feenox_problem_solve_petsc_transient :
                         ((feenox.pde.math_type == math_type_linear) ? feenox_problem_solve_petsc_linear :
                                                                       feenox_problem_solve_petsc_nonlinear);
  
  feenox.pde.has_stiffness = 1;
  feenox.pde.has_rhs = 1;
  // has_mass is above
  
  feenox.pde.has_jacobian_K = thermal.temperature_dependent_stiffness;
  feenox.pde.has_jacobian_M = thermal.temperature_dependent_mass;
  feenox.pde.has_jacobian_b = (thermal.temperature_dependent_source || thermal.temperature_dependent_bc);
  feenox.pde.has_jacobian = feenox.pde.has_jacobian_K || feenox.pde.has_jacobian_M || feenox.pde.has_jacobian_b;
  
  // if the conductivity depends on temperature the jacobian is not symmetric
  feenox.pde.symmetric_K = !thermal.temperature_dependent_stiffness;
  feenox.pde.symmetric_M = 1;
  
  
  // see if we have to compute gradients
  feenox.pde.compute_gradients |= (thermal.qx != NULL && thermal.qx->used) ||
                                  (thermal.qy != NULL && thermal.qy->used) ||
                                  (thermal.qz != NULL && thermal.qz->used);
  
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
