/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for Laplace's equation: boundary conditions
 *
 *  Copyright (C) 2021 jeremy theler
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

int feenox_problem_bc_parse_laplace(bc_data_t *bc_data, const char *lhs, char *rhs) {

///bc_laplace+phi+usage phi=<expr>
///bc_laplace+phi+description Dirichlet essential boundary condition in which the value of\ $\phi$ is prescribed.   
  if (strcmp(lhs, "phi") == 0) {
    bc_data->type_math = bc_type_math_dirichlet;


///bc_laplace+phi'+usage phi'=<expr>
///bc_laplace+phi'+description Neumann natural boundary condition in which the value of the normal outward derivative\ $\frac{\partial \phi}{\partial n}$ is prescribed.     
///bc_laplace+dphidn+usage dphidn=<expr>
///bc_laplace+dphidn+description Alias for `phi'`.     
  } else if (strcmp(lhs, "phi'") == 0 || strcmp(lhs, "dphidn") == 0) {
    bc_data->type_math = bc_type_math_neumann;
    bc_data->set = feenox_problem_bc_set_laplace_derivative;

  } else {
    feenox_push_error_message("unknown laplace boundary condition '%s'", lhs);
    return FEENOX_ERROR;
  }

  feenox_call(feenox_expression_parse(&bc_data->expr, rhs));
  
  bc_data->space_dependent = feenox_expression_depends_on_space(bc_data->expr.variables);
  bc_data->nonlinear = feenox_expression_depends_on_function(bc_data->expr.functions, feenox.pde.solution[0]);

  if (bc_data->nonlinear && bc_data->type_math == bc_type_math_dirichlet) {
    feenox_push_error_message("essential boundary condition '%s' cannot depend on phi", rhs);
    return FEENOX_ERROR;
  }
  
  return FEENOX_OK;
}


int feenox_problem_bc_set_laplace_phi(element_t *element, bc_data_t *bc_data, size_t node_global_index) {
  
#ifdef HAVE_PETSC
  
  feenox_call(feenox_problem_dirichlet_add(feenox.pde.mesh->node[node_global_index].index_dof[0], feenox_expression_eval(&bc_data->expr)));
  // TODO: only in transient
//  feenox.pde.dirichlet_derivatives[*k] = feenox_expression_derivative_wrt_variable(&bc_data->expr, feenox_special_var(t), feenox_special_var_value(t));
  
#endif
  return FEENOX_OK;
}

int feenox_problem_bc_set_laplace_derivative(element_t *element, bc_data_t *bc_data, unsigned int v) {
  
#ifdef HAVE_PETSC
  
  // TODO: cache if neither space nor temperature dependent
  double *x = feenox_problem_bc_natural_x(element, bc_data, v);
  double derivative = feenox_expression_eval(&bc_data->expr);
  feenox_call(feenox_problem_bc_natural_set(element, v, &derivative));
  
  if (bc_data->nonlinear) {
    double phi = feenox_function_eval(feenox.pde.solution[0], x);
    double dderivativedphi = feenox_expression_derivative_wrt_function(&bc_data->expr, feenox.pde.solution[0], phi);
    // TODO: axisymmetric
    double w = element->w[v];
    // mind the positive sign!
    feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, +w*dderivativedphi, element->H[v], element->H[v], 1.0, feenox.pde.Jbi));
  }
  
#endif
  
  return FEENOX_OK;
}
