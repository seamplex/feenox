#include "feenox.h"
#include "thermal.h"
extern feenox_t feenox;
extern thermal_t thermal;

// this virtual method is called from the parser, it fills in the already-allocated
// structure bc_data according to the two strings lhs=rhs, i.e. q=1 or T=1+x 
// note that h=a and Tref=b are handled by two bc_datas but linked together through next
int feenox_problem_bc_parse_thermal(bc_data_t *bc_data, const char *lhs, const char *rhs) {

  // TODO: document BCs with triple comments
  if (strcmp(lhs, "T") == 0) {
    // temperature
    bc_data->type_phys = BC_TYPE_THERMAL_TEMPERATURE;
    bc_data->type_math = bc_type_math_dirichlet;
    
  } else if (strcmp(lhs, "q''") == 0 || strcmp(lhs, "q") == 0) {
    // heat flux
    bc_data->type_phys = BC_TYPE_THERMAL_HEATFLUX;
    bc_data->type_math = bc_type_math_neumann;
    bc_data->set = feenox_problem_bc_set_thermal_heatflux;

  } else if (strcmp(lhs, "h") == 0) {
    // heat flux
    bc_data->type_phys = BC_TYPE_THERMAL_CONVECTION_COEFFICIENT;
    bc_data->type_math = bc_type_math_robin;
    bc_data->set = feenox_problem_bc_set_thermal_convection;
    bc_data->fills_matrix = 1;

  } else if (strcmp(lhs, "Tref") == 0 || strcmp(lhs, "Tinf") == 0) {
    // heat flux
    bc_data->type_phys = BC_TYPE_THERMAL_CONVECTION_TEMPERATURE;
    bc_data->type_math = bc_type_math_robin;
    bc_data->set = feenox_problem_bc_set_thermal_convection;
    bc_data->fills_matrix = 1;
    
  } else {
    feenox_push_error_message("unknown thermal boundary condition '%s'", lhs);
    return FEENOX_ERROR;
  }

  feenox_call(feenox_expression_parse(&bc_data->expr, rhs));
  
  bc_data->space_dependent = feenox_expression_depends_on_space(bc_data->expr.variables);
  thermal.bcs_depend_space |= bc_data->space_dependent;
    
  bc_data->nonlinear = feenox_expression_depends_on_function(bc_data->expr.functions, feenox.pde.solution[0]);
  thermal.bcs_depend_temperature |= bc_data->nonlinear;

  
  if (bc_data->nonlinear && bc_data->type_phys == BC_TYPE_THERMAL_TEMPERATURE) {
    feenox_push_error_message("strong boundary condition '%s' cannot depend on temperature", rhs);
    return FEENOX_ERROR;
  }
  // TODO: check that a single BC does not mix T and Tref
  
  return FEENOX_OK;
}


// this virtual method fills in the dirichlet indexes and values with bc_data
int feenox_problem_bc_set_thermal_dirichlet(bc_data_t *bc_data, size_t j, size_t k) {
  
#ifdef HAVE_PETSC
  
  feenox.pde.dirichlet_indexes[k] = feenox.pde.mesh->node[j].index_dof[bc_data->dof];
  feenox.pde.dirichlet_values[k] = feenox_expression_eval(&bc_data->expr);
  
#endif
  return FEENOX_OK;
}

// this virtual method builds the surface elemental matrix
int feenox_problem_bc_set_thermal_heatflux(bc_data_t *this, element_t *element, unsigned int v) {
  
#ifdef HAVE_PETSC

  // TODO: remove duplicate, use a macro
  feenox_call(feenox_mesh_compute_w_at_gauss(element, v, feenox.pde.mesh->integration));
  feenox_call(feenox_mesh_compute_H_at_gauss(element, v, feenox.pde.dofs, feenox.pde.mesh->integration));
  if (this->space_dependent) {
    feenox_call(feenox_mesh_compute_x_at_gauss(element, v, feenox.pde.mesh->integration));
    feenox_var_value(feenox.mesh.vars.x) = element->x[v][0];
    feenox_var_value(feenox.mesh.vars.y) = element->x[v][1];
    feenox_var_value(feenox.mesh.vars.z) = element->x[v][2];
  }
  
  // TODO: axisymmetric
//  r_for_axisymmetric = feenox_compute_r_for_axisymmetric(this, v);
  double r_for_axisymmetric = 1;
  double w = element->w[v] * r_for_axisymmetric;
  double q = feenox_expression_eval(&this->expr);
    
  gsl_vector_set(feenox.pde.Nb, 0, q);
  gsl_blas_dgemv(CblasTrans, w, element->H[v], feenox.pde.Nb, 1.0, feenox.pde.bi); 
  
#endif
  
  return FEENOX_OK;
}

// this virtual method builds the surface elemental matrix
int feenox_problem_bc_set_thermal_convection(bc_data_t *this, element_t *element, unsigned int v) {
  
#ifdef HAVE_PETSC

  // convection needs something in the next bc_data, if there is nothing then we are done
  if (this->next == NULL) {
    return FEENOX_OK;
  }

  // TODO: remove duplicate, use a macro
  feenox_call(feenox_mesh_compute_w_at_gauss(element, v, feenox.pde.mesh->integration));
  feenox_call(feenox_mesh_compute_H_at_gauss(element, v, feenox.pde.dofs, feenox.pde.mesh->integration));
  if (this->space_dependent) {
    feenox_call(feenox_mesh_compute_x_at_gauss(element, v, feenox.pde.mesh->integration));
    feenox_var_value(feenox.mesh.vars.x) = element->x[v][0];
    feenox_var_value(feenox.mesh.vars.y) = element->x[v][1];
    feenox_var_value(feenox.mesh.vars.z) = element->x[v][2];
  }
  
  // TODO: axisymmetric
//  r_for_axisymmetric = feenox_compute_r_for_axisymmetric(this, v);
  double r_for_axisymmetric = 1;
  double w = element->w[v] * r_for_axisymmetric;
  
  double h = 0;
  double Tref = 0;
  
  if (this->type_phys == BC_TYPE_THERMAL_CONVECTION_COEFFICIENT && this->next->type_phys == BC_TYPE_THERMAL_CONVECTION_TEMPERATURE) {
    h = feenox_expression_eval(&this->expr);
    Tref = feenox_expression_eval(&this->next->expr);
  } else if (this->type_phys == BC_TYPE_THERMAL_CONVECTION_COEFFICIENT && this->next->type_phys == BC_TYPE_THERMAL_CONVECTION_TEMPERATURE) {
    Tref = feenox_expression_eval(&this->expr);
    h = feenox_expression_eval(&this->next->expr);
  } else {
    feenox_push_error_message("convection condition needs h and Tref");
    return FEENOX_ERROR;
  }

  gsl_vector_set(feenox.pde.Nb, 0, h*Tref);
  
  // TODO: this is a scalar! no need to have a matrix
  gsl_matrix * Na = gsl_matrix_calloc(feenox.pde.dofs, feenox.pde.dofs);
  gsl_matrix_set(Na, 0, 0, h);

  gsl_matrix *NaH = gsl_matrix_calloc(feenox.pde.dofs, feenox.pde.n_local_nodes);
  gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1, Na, element->H[v], 0, NaH);
  gsl_blas_dgemm(CblasTrans, CblasNoTrans, w, element->H[v], NaH, 1.0, feenox.pde.Ki);
  gsl_blas_dgemv(CblasTrans, w, element->H[v], feenox.pde.Nb, 1.0, feenox.pde.bi); 

  gsl_matrix_free(Na);
  gsl_matrix_free(NaH);
  
#endif
  
  return FEENOX_OK;
}