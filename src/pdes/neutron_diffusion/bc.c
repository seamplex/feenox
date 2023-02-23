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
#include "neutron_diffusion.h"

int feenox_problem_bc_parse_neutron_diffusion(bc_data_t *bc_data, const char *lhs, char *rhs) {

  if (strcmp(lhs, "null") == 0) {
    bc_data->type_math = bc_type_math_dirichlet;
    bc_data->dof = -1;

  } else if (strcmp(lhs, "symmetry") == 0 || strcmp(lhs, "mirror") == 0) {
    bc_data->type_math = bc_type_math_neumann;
    bc_data->dof = -1;

  } else if (strcmp(lhs, "vacuum") == 0) {
    bc_data->type_math = bc_type_math_robin;
    
    bc_data->set = feenox_problem_bc_set_neutron_diffusion_vacuum;
    bc_data->fills_matrix = 1;
    bc_data->dof = -1;

  // TODO: fixed current
    
  } else {
    feenox_push_error_message("unknown neutron_diffusion boundary condition '%s'", lhs);
    return FEENOX_ERROR;
  }

  feenox_call(feenox_expression_parse(&bc_data->expr, rhs));

  // TODO: check consistency, non-linearities, etc.
  
  return FEENOX_OK;
}


// this virtual method fills in the dirichlet indexes and values with bc_data
int feenox_problem_bc_set_neutron_diffusion_null(element_t *element, bc_data_t *bc_data, size_t node_global_index) {

#ifdef HAVE_PETSC
  unsigned int g = 0;
  for (g = 0; g < feenox.pde.dofs; g++) {
    feenox_call(feenox_problem_dirichlet_add(feenox.pde.mesh->node[node_global_index].index_dof[g], 0));
  }
#endif
  
  return FEENOX_OK;
}


// this virtual method builds the surface elemental matrix
int feenox_problem_bc_set_neutron_diffusion_vacuum(element_t *element, bc_data_t *bc_data, unsigned int v) {
  
#ifdef HAVE_PETSC

  // TODO: remove duplicate, use a macro
  feenox_call(feenox_mesh_compute_wH_at_gauss(element, v));
  if (bc_data->space_dependent) {
    feenox_call(feenox_mesh_compute_x_at_gauss(element, v, feenox.pde.mesh->integration));
    feenox_mesh_update_coord_vars(element->x[v]);
  }
  
  double coeff = (bc_data->expr.items != NULL) ? feenox_expression_eval(&bc_data->expr) : 0.5;
  feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, element->w[v] * coeff, element->H[v], element->H[v], 1.0, feenox.pde.Ki));
  
#endif
  
  return FEENOX_OK;
}
