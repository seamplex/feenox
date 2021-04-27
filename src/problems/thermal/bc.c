#include "feenox.h"
#include "thermal.h"
extern feenox_t feenox;
extern thermal_t thermal;

int feenox_problem_bc_parse_thermal(bc_data_t *bc_data, const char *lhs, const char *rhs) {

  if (strcmp(lhs, "T") == 0) {
    // temperature
    bc_data->type_phys = BC_TYPE_THERMAL_TEMPERATURE;
    bc_data->type_math = bc_type_math_dirichlet;
    bc_data->dof = 0;
    
    feenox_check_alloc(bc_data->expr = calloc(1, sizeof(expr_t)));
    feenox_call(feenox_expression_parse(bc_data->expr, rhs));

    bc_data->space_dependent = feenox_expression_depends_on_space(bc_data->expr->variables);
    
  } else if (strcmp(lhs, "q''") == 0 || strcmp(lhs, "q") == 0) {
    // heat flux
    bc_data->type_phys = BC_TYPE_THERMAL_HEATFLUX;
    bc_data->type_math = bc_type_math_neumann;
    bc_data->dof = 0;
    
    feenox_check_alloc(bc_data->expr = calloc(1, sizeof(expr_t)));
    feenox_call(feenox_expression_parse(bc_data->expr, rhs));
    
    bc_data->space_dependent = feenox_expression_depends_on_space(bc_data->expr->variables);
    thermal.bcs_depend_space |= bc_data->space_dependent;
    
    bc_data->nonlinear = feenox_expression_depends_on_function(bc_data->expr->functions, feenox.pde.solution[0]);
    thermal.bcs_depend_temperature |= bc_data->nonlinear;

  } else {
    feenox_push_error_message("unknown thermal boundary condition '%s'", lhs);
    return FEENOX_ERROR;
  }
  
  
  return FEENOX_OK;
}


int feenox_problem_bc_set_thermal_dirichlet(bc_data_t *bc_data, size_t j, size_t k) {
#ifdef HAVE_PETSC
  feenox.pde.dirichlet_indexes[k] = feenox.pde.mesh->node[j].index_dof[bc_data->dof];
  feenox.pde.dirichlet_values[k] = feenox_expression_eval(&bc_data->expr[0]);
#endif
  return FEENOX_OK;
}


int feenox_problem_bc_set_thermal_heatflux(element_t *this, bc_data_t *bc_data, unsigned int v) {
  
#ifdef HAVE_PETSC
  
  feenox_call(feenox_mesh_compute_w_at_gauss(this, v, feenox.pde.mesh->integration));
  feenox_call(feenox_mesh_compute_H_at_gauss(this, v, feenox.pde.dofs, feenox.pde.mesh->integration));
  if (thermal.properties_depend_space || thermal.sources_depend_space) {
    feenox_call(feenox_mesh_compute_x_at_gauss(this, v, feenox.pde.mesh->integration));
    feenox_var_value(feenox.mesh.vars.x) = this->x[v][0];
    feenox_var_value(feenox.mesh.vars.y) = this->x[v][1];
    feenox_var_value(feenox.mesh.vars.z) = this->x[v][2];
  }
  
  // TODO: axisymmetric
//  r_for_axisymmetric = feenox_compute_r_for_axisymmetric(this, v);
  double r_for_axisymmetric = 1;
  double w = this->w[v] * r_for_axisymmetric;
  double q = feenox_expression_eval(&bc_data->expr[0]);
    
  gsl_vector_set(feenox.pde.Nb, 0, q);
  gsl_blas_dgemv(CblasTrans, w, this->H[v], feenox.pde.Nb, 1.0, feenox.pde.bi); 
  
#endif
  
  return FEENOX_OK;
}