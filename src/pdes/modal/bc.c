/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for modal analysis: boundary conditions
 *
 *  Copyright (C) 2021-2021 jeremy theler
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
    
  } else if (strcmp(lhs, "u") == 0) {
    bc_data->type_phys = BC_TYPE_MECHANICAL_DISPLACEMENT;
    bc_data->type_math = bc_type_math_dirichlet;
    bc_data->set_essential = feenox_problem_bc_set_modal_displacement;
    bc_data->dof = 0;
    modal.has_dirichlet_bcs = 1;
    
  } else if (strcmp(lhs, "v") == 0) {
    bc_data->type_phys = BC_TYPE_MECHANICAL_DISPLACEMENT;
    bc_data->type_math = bc_type_math_dirichlet;
    bc_data->set_essential = feenox_problem_bc_set_modal_displacement;
    bc_data->dof = 1;
    modal.has_dirichlet_bcs = 1;
    if (feenox.pde.dofs < 1) {
      feenox_push_error_message("cannot set u (displacement in y) with DOFs < 2");
      return FEENOX_ERROR;
    }

  } else if (strcmp(lhs, "w") == 0) {
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
    bc_data->set_essential = feenox_problem_bc_set_modal_multifreedom;
    
  } else if (strcmp(lhs, "radial") == 0) {
    bc_data->type_phys = BC_TYPE_MECHANICAL_RADIAL_SYMMETRY;
    bc_data->type_math = bc_type_math_multifreedom;
    bc_data->set_essential = feenox_problem_bc_set_modal_multifreedom;
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
  // TODO: duplicate but independent code vs. reusable code
  feenox_call(feenox_problem_bc_set_mechanical_displacement(this, e, j_global));  
#endif
  
  return FEENOX_OK;
}


// this virtual method fills in the dirichlet indexes and values with bc_data
int feenox_problem_bc_set_modal_multifreedom(bc_data_t *this, element_t *e, size_t j_global) {

#ifdef HAVE_PETSC
  feenox_call(feenox_problem_bc_set_mechanical_multifreedom(this, e, j_global));  
#endif
  
  return FEENOX_OK;
}
