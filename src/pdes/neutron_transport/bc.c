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
#include "neutron_transport.h"

// TODO: unify with diffusion?
int feenox_problem_bc_parse_neutron_transport(bc_data_t *bc_data, const char *lhs, char *rhs) {

  if (strcmp(lhs, "null") == 0) {
    bc_data->type_math = bc_type_math_dirichlet;
    bc_data->dof = -1;

  } else if (strcmp(lhs, "symmetry") == 0 || strcmp(lhs, "mirror") == 0) {
    bc_data->type_math = bc_type_math_neumann;
    bc_data->dof = -1;

  } else if (strcmp(lhs, "vacuum") == 0) {
    bc_data->type_math = bc_type_math_robin;
    
    bc_data->set = feenox_problem_bc_set_neutron_transport_vacuum;
    bc_data->fills_matrix = 1;
    bc_data->dof = -1;
    
  } else {
    feenox_push_error_message("unknown neutron_transport boundary condition '%s'", lhs);
    return FEENOX_ERROR;
  }

  feenox_call(feenox_expression_parse(&bc_data->expr, rhs));

  // TODO: check consistency, non-linearities, etc.
  
  return FEENOX_OK;
}


// this virtual method fills in the dirichlet indexes and values with bc_data
int feenox_problem_bc_set_neutron_transport_null(element_t *element, bc_data_t *bc_data, size_t node_global_index) {

#ifdef HAVE_PETSC
  unsigned int g = 0;
  for (g = 0; g < feenox.pde.dofs; g++) {
    feenox_call(feenox_problem_dirichlet_add(feenox.pde.mesh->node[node_global_index].index_dof[g], 0));
  }
#endif
  
  return FEENOX_OK;
}


// these "virtual" methods build the surface elemental matrix
int feenox_problem_bc_set_neutron_transport_vacuum(element_t *element, bc_data_t *bc_data, unsigned int v) {
  
#ifdef HAVE_PETSC
  
/*  
  for (n = 0; n < milonga.directions; n++) {
    if (mesh_dot(Omega[n], outward_normal) < 0) {
      // si el producto interno de Omega con la normal es negativo entonces hacemos cero el flujo
      if (k_vacuum > current_threshold_vacuum) {
        current_size_vacuum += milonga.problem_size/BC_FACTOR;
        current_threshold_vacuum = current_size_vacuum - 2*milonga.groups;
        indexes_vacuum = realloc(indexes_vacuum, current_size_vacuum * sizeof(int));
      }

      petsc_call(PetscMemcpy(indexes_vacuum+k_vacuum, node->index_dof+dof_index(n,0), milonga.groups * sizeof(int)));
      k_vacuum += milonga.groups;
    }
  }
*/
 
#endif
  
  return FEENOX_OK;
}



int feenox_problem_bc_set_neutron_transport_mirror(element_t *element, bc_data_t *bc_data, unsigned int v) {
  
#ifdef HAVE_PETSC
/*
  double reflected[3] = {0, 0, 0};
  double Omega_dot_outward;
  int n, n_refl;
  
  PetscFunctionBegin;
  for (n = 0; n < milonga.directions; n++) {
    if ((Omega_dot_outward = mesh_dot(Omega[n], outward_normal)) < 0) {

      // si el producto interno de Omega con la normal es positivo entonces tenemos que reflejar
      // si Omega es la direccion de incidencia, la direccion reflejada con respecto a la normal outward_normal es 
      // reflected = Omega - 2*(Omega dot outward_normal) * outward_normal
      reflected[0] = Omega[n][0] - 2*Omega_dot_outward * outward_normal[0];
      reflected[1] = Omega[n][1] - 2*Omega_dot_outward * outward_normal[1];
      reflected[2] = Omega[n][2] - 2*Omega_dot_outward * outward_normal[2];
      for (n_refl = 0; n_refl < milonga.directions; n_refl++) {
        if (fabs(reflected[0]-Omega[n_refl][0]) < wasora_var(wasora_mesh.vars.eps) &&
            fabs(reflected[1]-Omega[n_refl][1]) < wasora_var(wasora_mesh.vars.eps) &&
            fabs(reflected[2]-Omega[n_refl][2]) < wasora_var(wasora_mesh.vars.eps)) {
          break;
        }
      }
      if (n_refl == milonga.directions) {
        wasora_push_error_message("cannot find a reflected direction for n=%d in node %d", n, node->tag);
        PetscFunctionReturn(WASORA_RUNTIME_ERROR);
      }

      if (k_mirror > current_threshold_mirror) {
        current_size_mirror += milonga.problem_size/BC_FACTOR;
        current_threshold_mirror = current_size_mirror - 2*milonga.groups;
        indexes_mirror = realloc(indexes_mirror, current_size_mirror * sizeof(int));
        indexes_mirror_reflected = realloc(indexes_mirror_reflected, current_size_mirror * sizeof(int));
      }
      petsc_call(PetscMemcpy(indexes_mirror          +k_mirror, node->index_dof+dof_index(n,0),      milonga.groups * sizeof(int)));
      petsc_call(PetscMemcpy(indexes_mirror_reflected+k_mirror, node->index_dof+dof_index(n_refl,0), milonga.groups * sizeof(int)));
      k_mirror += milonga.groups;
    }
  }
*/
 
#endif
  
  return FEENOX_OK;
}
