/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for Laplace's equation: initialization
 *
 *  Copyright (C) 2021--2022 jeremy theler
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
#include "laplace.h"
laplace_t laplace;

///pb_laplace+NONE+description Laplace's equation does not need any extra keyword to `PROBLEM`.
int feenox_problem_parse_problem_laplace(const char *token) {

///kw_pde+PROBLEM+detail  * `laplace` solves the Laplace (or Poisson) equation.
  
  // no need to parse anything;
  if (token != NULL) {
    feenox_push_error_message("undefined keyword '%s'", token);
    return FEENOX_ERROR;
  }
 
  return FEENOX_OK;
}


int feenox_problem_init_parser_laplace(void) {

#ifdef HAVE_PETSC
  // we are FEM
  feenox.mesh.default_field_location = field_location_nodes;

  feenox.pde.init_runtime_particular = feenox_problem_init_runtime_laplace;
  feenox.pde.bc_parse = feenox_problem_bc_parse_laplace;
  feenox.pde.setup_ksp = feenox_problem_setup_ksp_laplace;
  feenox.pde.setup_pc = feenox_problem_setup_pc_laplace;
  feenox.pde.build_element_volumetric_gauss_point = feenox_problem_build_volumetric_gauss_point_laplace;
  
  // laplace is a scalar problem
  feenox.pde.dofs = 1;
///re_laplace+phi+description The scalar field\ $\phi(\vec{x})$ whose Laplacian is equal to zero or to\ $f(\vec{x})$.
  feenox_check_alloc(feenox.pde.unknown_name = calloc(feenox.pde.dofs, sizeof(char *)));    
  feenox_check_alloc(feenox.pde.unknown_name[0] = strdup("phi"));
  feenox_call(feenox_problem_define_solutions());

#endif
  return FEENOX_OK;
}

int feenox_problem_init_runtime_laplace(void) {
  
#ifdef HAVE_PETSC

  // we are FEM not FVM
  feenox.pde.mesh->data_type = data_type_node;
  
  feenox.pde.spatial_unknowns = feenox.pde.mesh->n_nodes;
  feenox.pde.size_global = feenox.pde.spatial_unknowns * feenox.pde.dofs;
  
  // check if we were given an initial guess
  if ((feenox.pde.initial_guess = feenox_get_function_ptr("phi_guess")) != NULL) {
    if (feenox.pde.initial_guess->n_arguments != feenox.pde.dim) {
      feenox_push_error_message("initial guess function phi_guess ought to have %d arguments instead of %d", feenox.pde.dim, feenox.pde.initial_condition->n_arguments);
      return FEENOX_ERROR;
    }
  }

  // check if we were given an initial solution
  if ((feenox.pde.initial_condition = feenox_get_function_ptr("phi_0")) != NULL) {
    if (feenox.pde.initial_condition->n_arguments != feenox.pde.dim) {
      feenox_push_error_message("initial condition function phi_0 ought to have %d arguments instead of %d", feenox.pde.dim, feenox.pde.initial_condition->n_arguments);
      return FEENOX_ERROR;
    }
  }

  // initialize distributions
///pr_laplace+f+description The right hand side of the equation\ $\nabla^2 \phi=f(\vec{x})$.
///pr_laplace+f+description If not given, default is zero (i.e. Laplace).
  feenox_call(feenox_distribution_init(&laplace.f, "f"));
  laplace.f.uniform = feenox_expression_depends_on_space(laplace.f.dependency_variables);
  laplace.f.non_linear = feenox_expression_depends_on_function(laplace.f.dependency_functions, feenox.pde.solution[0]);  
  
///pr_laplace+alpha+description The coefficient of the temporal derivative for the transient
///pr_laplace+alpha+description equation \ $\alpha \frac{\partial \phi}{\partial t} + \nabla^2 \phi=f(\vec{x})$.
///pr_laplace+alpha+description If not given, default is one.
  feenox.pde.has_mass = (feenox_var_value(feenox_special_var(end_time)) > 0);
  if (feenox.pde.has_mass) {
    feenox_call(feenox_distribution_init(&laplace.alpha, "alpha"));
    if (laplace.alpha.defined == 0) {
      // TODO: define something identically equal to one
      feenox_push_error_message("'alpha' is needed for transient");
      return FEENOX_ERROR;
    }  
    if (laplace.alpha.full == 0) {
      feenox_push_error_message("'alpha' is not defined over all volumes");
      return FEENOX_ERROR;
    }
  }

  laplace.alpha.uniform = feenox_expression_depends_on_space(laplace.alpha.dependency_variables);
  laplace.alpha.non_linear      = feenox_expression_depends_on_function(laplace.alpha.dependency_functions,  feenox.pde.solution[0]);
  
  laplace.space_dependent_source = laplace.f.uniform;
  laplace.space_dependent_mass = laplace.alpha.uniform;
  
  laplace.phi_dependent_mass      = feenox_expression_depends_on_function(laplace.alpha.dependency_functions, feenox.pde.solution[0]);
  
  if (feenox.pde.math_type == math_type_automatic) {
    feenox.pde.math_type = (laplace.phi_dependent_mass   == 0 &&
                            laplace.phi_dependent_source == 0 &&
                            laplace.phi_dependent_bc     == 0) ? math_type_linear : math_type_nonlinear;
  }
  
  feenox.pde.solve = (feenox_special_var_value(end_time) > 0) ? feenox_problem_solve_petsc_transient :
                         ((feenox.pde.math_type == math_type_linear) ? feenox_problem_solve_petsc_linear :
                                                                       feenox_problem_solve_petsc_nonlinear);
  
  feenox.pde.has_stiffness = 1;
  feenox.pde.has_rhs = 1;
  // has_mass is above
  
  feenox.pde.has_jacobian_K = 0;
  feenox.pde.has_jacobian_M = laplace.phi_dependent_mass;
  feenox.pde.has_jacobian_b = (laplace.phi_dependent_source || laplace.phi_dependent_bc);
  feenox.pde.has_jacobian = feenox.pde.has_jacobian_K || feenox.pde.has_jacobian_M || feenox.pde.has_jacobian_b;
  
  feenox.pde.symmetric_K = 1;
  feenox.pde.symmetric_M = 1;
  
#endif  
  return FEENOX_OK;
}


#ifdef HAVE_PETSC
int feenox_problem_setup_pc_laplace(PC pc) {

  PCType pc_type = NULL;
  petsc_call(PCGetType(pc, &pc_type));
  if (pc_type == NULL) {
    petsc_call(PCSetType(pc, PCGAMG));
  }
  
  return FEENOX_OK;
}

int feenox_problem_setup_ksp_laplace(KSP ksp ) {

  KSPType ksp_type = NULL;
  petsc_call(KSPGetType(ksp, &ksp_type));
  if (ksp_type == NULL) {
    petsc_call(KSPSetType(ksp, KSPCG));
  }  

  return FEENOX_OK;
}
#endif
