/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for modal analysis: boundary conditions
 *
 *  Copyright (C) 2021-2023 Jeremy Theler
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
#include "modal.h"

int feenox_problem_bc_parse_modal(bc_data_t *bc_data, const char *lhs, char *rhs) {

  // TODO: document BCs with triple comments
  if (strcmp(lhs, "fixed") == 0) {
    bc_data->type_phys = BC_TYPE_MECHANICAL_DISPLACEMENT;
    bc_data->type_math = bc_type_math_dirichlet;
    bc_data->set_essential = feenox_problem_bc_set_modal_displacement;
    bc_data->dof = -1;
    modal.has_dirichlet_bcs = 1;
    
  } else if (strcmp(lhs, "u") == 0 || strcmp(lhs, "u_x") == 0) {
    bc_data->type_phys = BC_TYPE_MECHANICAL_DISPLACEMENT;
    bc_data->type_math = bc_type_math_dirichlet;
    bc_data->set_essential = feenox_problem_bc_set_modal_displacement;
    bc_data->dof = 0;
    modal.has_dirichlet_bcs = 1;
    
  } else if (strcmp(lhs, "v") == 0 || strcmp(lhs, "u_y") == 0) {
    bc_data->type_phys = BC_TYPE_MECHANICAL_DISPLACEMENT;
    bc_data->type_math = bc_type_math_dirichlet;
    bc_data->set_essential = feenox_problem_bc_set_modal_displacement;
    bc_data->dof = 1;
    modal.has_dirichlet_bcs = 1;
    if (feenox.pde.dofs < 1) {
      feenox_push_error_message("cannot set u (displacement in y) with DOFs < 2");
      return FEENOX_ERROR;
    }

  } else if (strcmp(lhs, "w") == 0 || strcmp(lhs, "u_z") == 0) {
    bc_data->type_phys = BC_TYPE_MECHANICAL_DISPLACEMENT;
    bc_data->type_math = bc_type_math_dirichlet;
    bc_data->set_essential = feenox_problem_bc_set_modal_displacement;
    bc_data->dof = 2;
    modal.has_dirichlet_bcs = 1;
    if (feenox.pde.dofs < 2) {
      feenox_push_error_message("cannot set w (displacement in z) with DOFs < 3");
      return FEENOX_ERROR;
    }
    
  } else if (strcmp(lhs, "symmetry") == 0 || strcmp(lhs, "tangential") == 0) {
    bc_data->type_phys = BC_TYPE_MECHANICAL_TANGENTIAL_SYMMETRY;
    bc_data->type_math = bc_type_math_multifreedom;
    bc_data->set_essential = feenox_problem_bc_set_modal_symmetry;
    
  } else if (strcmp(lhs, "radial") == 0) {
    bc_data->type_phys = BC_TYPE_MECHANICAL_RADIAL_SYMMETRY;
    bc_data->type_math = bc_type_math_multifreedom;
    bc_data->set_essential = feenox_problem_bc_set_modal_radial;
    // TODO: x0, y0 and z0
    
  } else if (strcmp(lhs, "0") == 0) {
    bc_data->type_phys = BC_TYPE_MECHANICAL_MULTIDOF_EXPRESSION;
    bc_data->type_math = bc_type_math_multifreedom;
    bc_data->set_essential = feenox_problem_bc_set_modal_multifreedom;
    
    // trick: the idea is that the user might write an expression of space
    // x,y,z but also maybe of u,v y w. However, u,v,w are functinos and not variables!
    // what we do is to define variables named U,V,W and string-replace  u,v,w -> U,V,W
    // in the entered expression
    
    // TODO: there should be a separator (i.e. operator) before and after
    char *s = rhs;
    while (*s != '\0') {
      if (*s == 'u') {
        *s = 'U';
      } else if (*s == 'v') {
        *s = 'V';
      } else if (*s == 'w') {
        *s = 'W';
      }
      s++;
    }
    
  } else {
    feenox_push_error_message("unknown modal boundary condition '%s'", lhs);
    return FEENOX_ERROR;
  }

  if (rhs != NULL && strcmp(rhs, "0") != 0) {
    feenox_push_error_message("boundary conditions in modal have to be homogeneous not '%s'", rhs);
    return FEENOX_ERROR;
  }
  
  return FEENOX_OK;
}


// this virtual method fills in the dirichlet indexes and values with bc_data
int feenox_problem_bc_set_modal_displacement(bc_data_t *this, element_t *e, size_t j_global) {

#ifdef HAVE_PETSC
  
  if (this->dof != -1) {
    
    // only one dof
    feenox_call(feenox_problem_dirichlet_add(feenox.pde.mesh->node[j_global].index_dof[this->dof], feenox_expression_eval(&this->expr)));
    
  } else {
    
    // -1 means all dofs (and the only possibility is to have all them equal to zero)
    feenox_call(feenox_problem_dirichlet_add(feenox.pde.mesh->node[j_global].index_dof[0], 0));
    if (feenox.pde.dofs > 1) {
      feenox_call(feenox_problem_dirichlet_add(feenox.pde.mesh->node[j_global].index_dof[1], 0));
      if (feenox.pde.dofs > 2) {
        feenox_call(feenox_problem_dirichlet_add(feenox.pde.mesh->node[j_global].index_dof[2], 0));
      }
    }
    
  }  
  
#endif
  
  return FEENOX_OK;
}

int feenox_problem_bc_set_modal_symmetry(bc_data_t *this, element_t *e, size_t j_global) {
  
#ifdef HAVE_PETSC
  // outward normal (smoothed over all elements on the physical group of the BC)
  // TODO: choose to smooth or to use the local one
  PetscScalar normal[3] = {0, 0, 0};
  PetscScalar n_element[3] = {0, 0, 0};
  element_ll_t *element_item = NULL;
  LL_FOREACH(feenox.pde.mesh->node[j_global].element_list, element_item) {
    element_t *e_prime = element_item->element;
    if (e_prime != NULL && e_prime->type->dim == (feenox.pde.dim-1) && e_prime->physical_group == e->physical_group) {
      feenox_call(feenox_mesh_compute_outward_normal(element_item->element, n_element));
      normal[0] += n_element[0];
      normal[1] += n_element[1];
      normal[2] += n_element[2];
    }
  }

//    feenox_call(feenox_mesh_compute_outward_normal(element, normal));


  int coordinate_direction = -1;
  double norm = gsl_hypot3(normal[0], normal[1], normal[2]);
  if (feenox_likely(norm != 0)) {
    // if the outward normal coincides with one of the three axes, we can get away with a regular dirichlet BC
    for (int g = 0; g < 3; g++) {
      normal[g] /= norm;
      if (fabs(normal[g]) > (1-1e-4)) {
        coordinate_direction = g;
      }  
    }
  } else {
    feenox_push_error_message("outward normal has zero norm");
    return FEENOX_ERROR;
  }

  // if the condition results in a direction normal to one of the three coordinate planes
  // then we set a traditional dirichlet bc (i.e. u=0 or v=0 or w=0)
  // otherwise we need a generic multifreedom
  if (coordinate_direction != -1) {
    feenox_call(feenox_problem_dirichlet_add(feenox.pde.mesh->node[j_global].index_dof[coordinate_direction], 0));
  } else {
    feenox_call(feenox_problem_multifreedom_add(j_global, normal));
  }  
#endif
    
  return FEENOX_OK;
}

int feenox_problem_bc_set_modal_radial(bc_data_t *this, element_t *e, size_t j_global) {
  
#ifdef HAVE_PETSC
  double x[3] = {0,0,0};
  double eps = 1e-2;

  if (e->physical_group->volume == 0) {
    feenox_call(feenox_physical_group_compute_volume(e->physical_group, feenox.pde.mesh));
  }

  // TODO! read center of the radial condition
  for (int g = 0; g < 3; g++) {
//      x[g] = feenox.pde.mesh->node[node_global_index].x[g] - ((bc_data->expr[g].items == NULL) ? element->physical_entity->cog[d] : feenox_expression_eval(bc_data->expr[g]));
    x[g] = feenox.pde.mesh->node[j_global].x[g] - e->physical_group->cog[g];
  }  

  double coefficients[3] = {0,0,0};

  // x-y
  if (fabs(x[0]) > eps && fabs(x[1]) > eps) {
    coefficients[0] = +x[1];
    coefficients[1] = -x[0];
    coefficients[2] = 0;
    feenox_call(feenox_problem_multifreedom_add(j_global, coefficients));
  }      

  // x-z
  if (fabs(x[0]) > eps && fabs(x[2]) > eps) {
    coefficients[0] = +x[2];
    coefficients[1] = 0;
    coefficients[2] = -x[0];
    feenox_call(feenox_problem_multifreedom_add(j_global, coefficients));
  }      

  // y-z
  if (fabs(x[1]) > eps && fabs(x[2]) > eps) {
    coefficients[0] = 0;
    coefficients[1] = +x[2];
    coefficients[2] = -x[1];
    feenox_call(feenox_problem_multifreedom_add(j_global, coefficients));
  }      
#endif
  return FEENOX_OK;
}


typedef struct {
  expr_t *expr;
  int dof;
} feenox_gsl_function_of_uvw_params_t;

// this virtual method fills in the dirichlet indexes and values with bc_data
int feenox_problem_bc_set_modal_multifreedom(bc_data_t *this, element_t *e, size_t j_global) {

#ifdef HAVE_PETSC
  feenox_gsl_function_of_uvw_params_t params = { &this->expr, -1 };
  gsl_function F = {feenox_modal_gsl_function_of_uvw, &params};

  double coefficients[3] = {0, 0, 0};

  // TODO: choose
  double h = 1e-5;
  double result = 0;
  double abserr = 0;
  for (int g = 0; g < 3; g++) {
    params.dof = g;
    gsl_deriv_central(&F, 0, h, &result, &abserr);
    coefficients[g] = -result;
  }  

  // TODO: non-homogeneous RHS
  feenox_call(feenox_problem_multifreedom_add(j_global, coefficients));
#endif
  
  return FEENOX_OK;
}

// wrapper to compute derivatives with GSL
double feenox_modal_gsl_function_of_uvw(double x, void *params) {
  feenox_gsl_function_of_uvw_params_t *p = (feenox_gsl_function_of_uvw_params_t *)params;

  feenox_var_value(modal.displ_for_bc[0]) = 0;
  feenox_var_value(modal.displ_for_bc[1]) = 0;
  feenox_var_value(modal.displ_for_bc[2]) = 0;
  feenox_var_value(modal.displ_for_bc[p->dof]) = x;
  
  double y = feenox_expression_eval(p->expr);

  if (gsl_isnan(y) || gsl_isinf(y)) {
    feenox_nan_error();
  }

  return y;

}
