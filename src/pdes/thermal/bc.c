/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for the heat equation: boundary conditions
 *
 *  Copyright (C) 2021--2023 jeremy theler
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

// this virtual method is called from the parser, it fills in the already-allocated
// structure bc_data according to the two strings lhs=rhs, i.e. q=1 or T=1+x 
// note that h=a and Tref=b are handled by two bc_datas but linked together through next
int feenox_problem_bc_parse_thermal(bc_data_t *bc_data, const char *lhs, char *rhs) {

  // TODO: document BCs with triple comments
  if (strcmp(lhs, "T") == 0) {
    // temperature
    bc_data->type_phys = BC_TYPE_THERMAL_TEMPERATURE;
    bc_data->type_math = bc_type_math_dirichlet;
    bc_data->set_essential = feenox_problem_bc_set_thermal_temperature;
    
  } else if (strcmp(lhs, "q''") == 0 || strcmp(lhs, "q") == 0) {
    // heat flux
    bc_data->type_phys = BC_TYPE_THERMAL_HEATFLUX;
    bc_data->type_math = bc_type_math_neumann;
    bc_data->set_natural = feenox_problem_bc_set_thermal_heatflux;

  } else if (strcmp(lhs, "h") == 0) {
    // heat flux
    bc_data->type_phys = BC_TYPE_THERMAL_CONVECTION_COEFFICIENT;
    bc_data->type_math = bc_type_math_robin;
    bc_data->set_natural = feenox_problem_bc_set_thermal_convection;
    bc_data->fills_matrix = 1;

  } else if (strcmp(lhs, "Tref") == 0 || strcmp(lhs, "T_ref") == 0 ||
             strcmp(lhs, "Tinf") == 0 || strcmp(lhs, "T_inf") == 0) {
    // heat flux
    bc_data->type_phys = BC_TYPE_THERMAL_CONVECTION_TEMPERATURE;
    bc_data->type_math = bc_type_math_robin;
    bc_data->set_natural = feenox_problem_bc_set_thermal_convection;
    bc_data->fills_matrix = 1;
    
  } else {
    feenox_push_error_message("unknown thermal boundary condition '%s'", lhs);
    return FEENOX_ERROR;
  }

  feenox_call(feenox_expression_parse(&bc_data->expr, rhs));
  
  // for non-linear problems it is important to have a good initial guess
  // if the user did not give us one in T_guess we average all the temperatures from the BCs
  if (bc_data->type_phys == BC_TYPE_THERMAL_TEMPERATURE || bc_data->type_phys == BC_TYPE_THERMAL_CONVECTION_TEMPERATURE ) {
    thermal.guessed_initial_guess += feenox_expression_eval(&bc_data->expr);
    thermal.n_bc_temperatures++;
  }
  
  bc_data->space_dependent = feenox_expression_depends_on_space(bc_data->expr.variables);
  bc_data->nonlinear = feenox_expression_depends_on_function(bc_data->expr.functions, feenox.pde.solution[0]);

  
  if (bc_data->nonlinear && bc_data->type_phys == BC_TYPE_THERMAL_TEMPERATURE) {
    feenox_push_error_message("essential boundary condition '%s' cannot depend on temperature", rhs);
    return FEENOX_ERROR;
  }
  // TODO: check that a single BC does not mix T and Tref
  
  return FEENOX_OK;
}


// this virtual method fills in the dirichlet indexes and values with bc_data
int feenox_problem_bc_set_thermal_temperature(bc_data_t *this, element_t *e, size_t j_global) {
  
#ifdef HAVE_PETSC
  
  feenox_call(feenox_problem_dirichlet_add(feenox.pde.mesh->node[j_global].index_dof[0], feenox_expression_eval(&this->expr)));  
  // TODO: only in transient
//  feenox.pde.dirichlet_derivatives[*k] = feenox_expression_derivative_wrt_variable(&bc_data->expr, feenox_special_var(t), feenox_special_var_value(t));
  
#endif
  return FEENOX_OK;
}

// this virtual method builds the surface elemental matrix
// TODO: one method for constant flux, one for temp, one for space
int feenox_problem_bc_set_thermal_heatflux(bc_data_t *this, element_t *e, unsigned int q) {
  
#ifdef HAVE_PETSC
  
  // TODO: cache if neither space nor temperature dependent
  double *x = feenox_mesh_compute_x_at_gauss_if_needed_and_update_var(e, q, this->space_dependent);
  double power = feenox_expression_eval(&this->expr);
  feenox_call(feenox_problem_rhs_set(e, q, &power));
  
  if (this->nonlinear) {
    double T = feenox_function_eval(feenox.pde.solution[0], x);
    double dqdT = feenox_expression_derivative_wrt_function(&this->expr, feenox.pde.solution[0], T);
    // mind the positive sign!
    feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, +e->w[q]*dqdT, e->type->H_Gc[q], e->type->H_Gc[q], 1.0, feenox.pde.Jbi));
  }
  
#endif
  
  return FEENOX_OK;
}

// this virtual method builds the surface elemental matrix
int feenox_problem_bc_set_thermal_convection(bc_data_t *this, element_t *e, unsigned int q) {
  
#ifdef HAVE_PETSC

  // convection needs something in the next bc_data, if there is nothing then we are done
  if (this->next == NULL) {
    return FEENOX_OK;
  }

  feenox_call(feenox_mesh_compute_wH_at_gauss(e, q));
  feenox_mesh_compute_x_at_gauss_if_needed_and_update_var(e, q, this->space_dependent);
  
  double h = 0;
  double Tref = 0;
  
  if (this->type_phys == BC_TYPE_THERMAL_CONVECTION_COEFFICIENT && this->next->type_phys == BC_TYPE_THERMAL_CONVECTION_TEMPERATURE) {
    h = feenox_expression_eval(&this->expr);
    Tref = feenox_expression_eval(&this->next->expr);
  } else if (this->type_phys == BC_TYPE_THERMAL_CONVECTION_TEMPERATURE && this->next->type_phys == BC_TYPE_THERMAL_CONVECTION_COEFFICIENT) {
    Tref = feenox_expression_eval(&this->expr);
    h = feenox_expression_eval(&this->next->expr);
  } else {
    feenox_push_error_message("convection condition needs h and Tref");
    return FEENOX_ERROR;
  }

  // the h*Tref goes to b
  double rhs = h*Tref;
  feenox_call(feenox_problem_rhs_set(e, q, &rhs));
  
  // TODO: the h*T goes directly to the stiffness matrix
  // this is not efficient because if h depends on t or T we might need to re-build the whole K
  feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, e->w[q]*h, e->type->H_Gc[q], e->type->H_Gc[q], 1.0, feenox.pde.Ki));

/*  
  gsl_vector_set(feenox.pde.vec_f, 0, h*Tref);
  feenox_call(gsl_blas_dgemv(CblasTrans, w, e->type->H_Gc[q], feenox.pde.vec_f, 1.0, feenox.pde.bi)); 
*/
  

#endif
  
  return FEENOX_OK;
}

// TODO: we can do something of the like for radiation but care
// has to be taken regarding the units of the temperatures!
