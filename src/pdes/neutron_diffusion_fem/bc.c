/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for neutron diffusion FEM: boundary conditions
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
#include "neutron_diffusion_fem.h"
extern feenox_t feenox;
extern neutron_diffusion_fem_t neutron_diffusion_fem;

int feenox_problem_bc_parse_neutron_diffusion_fem(bc_data_t *bc_data, const char *lhs, const char *rhs) {

  if (strcmp(lhs, "null") == 0) {
    bc_data->type_math = bc_type_math_dirichlet;
    bc_data->dof = -1;
    
  } else {
    feenox_push_error_message("unknown neutron_diffusion_fem boundary condition '%s'", lhs);
    return FEENOX_ERROR;
  }

  return FEENOX_OK;
}


// this virtual method fills in the dirichlet indexes and values with bc_data
int feenox_problem_bc_set_neutron_diffusion_fem_null(bc_data_t *bc_data, size_t node_index) {

#ifdef HAVE_PETSC
  unsigned int g = 0;
  for (g = 0; g < feenox.pde.dofs; g++) {
    feenox_call(feenox_problem_dirichlet_add(feenox.pde.mesh->node[node_index].index_dof[g], 0));
  }
#endif
  
  return FEENOX_OK;
}
