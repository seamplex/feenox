/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for neutron transport FEM: boundary conditions
 *
 *  Copyright (C) 2023 jeremy theler
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
#include "neutron_sn.h"

int feenox_problem_bc_parse_neutron_sn(bc_data_t *bc_data, const char *lhs, char *rhs) {

  // TODO: should this be the default BC?
  if (strcmp(lhs, "vacuum") == 0 || strcmp(lhs, "null") == 0) {
    // "null" is supported for compatibility with diffusion
    bc_data->type_math = bc_type_math_dirichlet;
    bc_data->set_essential = feenox_problem_bc_set_neutron_sn_vacuum;
    bc_data->dof = -1;

  } else if (strcmp(lhs, "mirror") == 0 || strcmp(lhs, "symmetry") == 0) {
    bc_data->type_math = bc_type_math_dirichlet;
    bc_data->dof = -1;
    bc_data->set_essential = feenox_problem_bc_set_neutron_sn_mirror;

  // TODO: albedo
  // TODO: white current
  // TODO: individual scalar fluxes as a function of theta & phi  
  } else {
    feenox_push_error_message("unknown neutron_sn boundary condition '%s'", lhs);
    return FEENOX_ERROR;
  }
  
  bc_data->space_dependent = feenox_expression_depends_on_space(bc_data->expr.variables);
  bc_data->nonlinear = feenox_expression_depends_on_function(bc_data->expr.functions, feenox.pde.solution[0]);

  if (bc_data->nonlinear && bc_data->type_math == bc_type_math_dirichlet) {
    feenox_push_error_message("essential boundary condition '%s' cannot depend on phi", rhs);
    return FEENOX_ERROR;
  }

  feenox_call(feenox_expression_parse(&bc_data->expr, rhs));

  // TODO: check consistency, non-linearities, etc.
  
  return FEENOX_OK;
}


// these virtual method fill in the dirichlet indexes and values with bc_data
int feenox_problem_bc_set_neutron_sn_vacuum(bc_data_t *this, element_t *e, size_t j_global) {

#ifdef HAVE_PETSC
  double outward_normal[3];
  feenox_call(feenox_mesh_compute_outward_normal(e, outward_normal));
  for (unsigned n = 0; n < neutron_sn.directions; n++) {
    if (feenox_mesh_dot(neutron_sn.Omega[n], outward_normal) < 0) {
      // if the direction is inward set it to zero
      for (unsigned int g = 0; g < neutron_sn.groups; g++) {
        feenox_call(feenox_problem_dirichlet_add(feenox.pde.mesh->node[j_global].index_dof[dof_index(n,g)], 0));
      }
    }
  }
#endif
  
  return FEENOX_OK;
}


int feenox_problem_bc_set_neutron_sn_mirror(bc_data_t *this, element_t *e, size_t j_global) {
  
#ifdef HAVE_PETSC
  double outward_normal[3];
  double reflected[3];
  double Omega_dot_outward = 0;
  double eps = feenox_var_value(feenox.mesh.vars.eps);
  
  // TODO: mark the BC as dependent on the normal and compute it in the caller
  feenox_call(feenox_mesh_compute_outward_normal(e, outward_normal));
//  printf("element %lu node %lu outward normal %.3f %.3f %.3f\n", e->tag, j_global, outward_normal[0], outward_normal[1], outward_normal[2]);
  for (unsigned n = 0; n < neutron_sn.directions; n++) {
    if ((Omega_dot_outward = feenox_mesh_dot(neutron_sn.Omega[n], outward_normal)) < 0) {
      // if the direction is inward then we have to reflect it
      // if Omega is the incident direction with respect to the outward normal then
      // reflected = Omega - 2*(Omega dot outward_normal) * outward_normal
      
      for (int m = 0; m < 3; m++) {
        reflected[m] = neutron_sn.Omega[n][m] - 2*Omega_dot_outward * outward_normal[m];
      }

      unsigned int n_prime = 0;
      for (n_prime = 0; n_prime < neutron_sn.directions; n_prime++) {
        if (fabs(reflected[0] - neutron_sn.Omega[n_prime][0]) < eps &&
            fabs(reflected[1] - neutron_sn.Omega[n_prime][1]) < eps &&
            fabs(reflected[2] - neutron_sn.Omega[n_prime][2]) < eps) {
          break;
        }
      }
      
      if (n_prime == neutron_sn.directions) {
        feenox_push_error_message("cannot find a reflected direction for n=%d (out of %d in S%d) for node %d", n, neutron_sn.directions, neutron_sn.N, feenox.pde.mesh->node[j_global].tag);
        return FEENOX_ERROR;
      }
      
      double *coefficients = NULL;
      feenox_check_alloc(coefficients = calloc(feenox.pde.dofs, sizeof(double)));
      
      for (unsigned int g = 0; g < neutron_sn.groups; g++) {
        coefficients[dof_index(n,g)] = -1;
        coefficients[dof_index(n_prime,g)] = +1;
      }
      feenox_call(feenox_problem_multifreedom_add(j_global, coefficients));
      feenox_free(coefficients);
    }
  }
      
#endif
  
  return FEENOX_OK;
}
