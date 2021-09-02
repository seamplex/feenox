/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for modal analysis: boundary conditions
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
 *  feenox is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with feenox.  If not, see <http://www.gnu.org/licenses/>.
 *------------------- ------------  ----    --------  --     -       -         -
 */
#include "feenox.h"
#include "modal.h"
extern feenox_t feenox;
extern modal_t modal;

int feenox_problem_bc_parse_modal(bc_data_t *bc_data, const char *lhs, const char *rhs) {

  // TODO: document BCs with triple comments
  if (strcmp(lhs, "fixed") == 0) {
    bc_data->type_math = bc_type_math_dirichlet;
    bc_data->dof = -1;
    modal.has_dirichlet_bcs = 1;
    
  } else if (strcmp(lhs, "u") == 0) {
    bc_data->type_math = bc_type_math_dirichlet;
    bc_data->dof = 0;
    modal.has_dirichlet_bcs = 1;
    
  } else if (strcmp(lhs, "v") == 0) {
    bc_data->type_math = bc_type_math_dirichlet;
    bc_data->dof = 1;
    modal.has_dirichlet_bcs = 1;
    if (feenox.pde.dofs < 1) {
      feenox_push_error_message("cannot set u (displacement in y) with DOFs < 2");
      return FEENOX_ERROR;
    }

  } else if (strcmp(lhs, "w") == 0) {
    bc_data->type_math = bc_type_math_dirichlet;
    bc_data->dof = 2;
    modal.has_dirichlet_bcs = 1;
    if (feenox.pde.dofs < 2) {
      feenox_push_error_message("cannot set w (displacement in z) with DOFs < 3");
      return FEENOX_ERROR;
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
int feenox_problem_bc_set_modal_displacement(bc_data_t *bc_data, size_t node_index) {

#ifdef HAVE_PETSC
  feenox_call(feenox_problem_bc_set_mechanical_displacement(bc_data, node_index));  
#endif
  
  return FEENOX_OK;
}
