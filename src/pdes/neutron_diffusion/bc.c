/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for neutron diffusion FEM: boundary conditions
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
#include "neutron_diffusion.h"

int feenox_problem_bc_parse_neutron_diffusion(bc_data_t *bc_data, const char *lhs, char *rhs) {

  if (strcmp(lhs, "null") == 0) {
    bc_data->type_math = bc_type_math_dirichlet;
    bc_data->dof = -1;
    bc_data->set_essential = feenox_problem_bc_set_neutron_diffusion_null;

  } else if (strcmp(lhs, "symmetry") == 0 || strcmp(lhs, "mirror") == 0) {
    bc_data->type_math = bc_type_math_neumann;
    bc_data->dof = -1;

  } else if (strcmp(lhs, "vacuum") == 0) {
    bc_data->type_math = bc_type_math_robin;
    
    bc_data->set_natural = feenox_problem_bc_set_neutron_diffusion_vacuum;
    bc_data->fills_matrix = 1;
    bc_data->dof = -1;

  // TODO: fixed current = pure neumann
    
  } else {
    feenox_push_error_message("unknown neutron_diffusion boundary condition '%s'", lhs);
    return FEENOX_ERROR;
  }

  feenox_call(feenox_expression_parse(&bc_data->expr, rhs));

  // TODO: check consistency, non-linearities, etc.
  
  return FEENOX_OK;
}


// this virtual method fills in the dirichlet indexes and values with bc_data
int feenox_problem_bc_set_neutron_diffusion_null(bc_data_t *this, element_t *e, size_t j_global) {

#ifdef HAVE_PETSC
  unsigned int g = 0;
  for (g = 0; g < feenox.pde.dofs; g++) {
    feenox_call(feenox_problem_dirichlet_add(feenox.pde.mesh->node[j_global].index_dof[g], 0));
  }
#endif
  
  return FEENOX_OK;
}


// this virtual method builds the surface elemental matrix
int feenox_problem_bc_set_neutron_diffusion_vacuum(bc_data_t *this, element_t *e, unsigned int q) {
  
#ifdef HAVE_PETSC

  feenox_fem_compute_x_at_gauss_if_needed_and_update_var(e, q, feenox.pde.mesh->integration, this->space_dependent);
  double coeff = (this->expr.items != NULL) ? feenox_expression_eval(&this->expr) : 0.5;
  
  // TODO: convenience call like  feenox_problem_rhs_set()?
  double wdet = feenox_fem_compute_w_det_at_gauss(e, q, feenox.pde.mesh->integration);
  gsl_matrix *H = feenox_fem_compute_H_Gc_at_gauss(e, q, feenox.pde.mesh->integration);
  feenox_call(feenox_blas_BtB(H, wdet*coeff, feenox.fem.Ki));
  
#endif
  
  return FEENOX_OK;
}
