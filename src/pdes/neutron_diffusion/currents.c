/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for neutron diffusion FEM: computation of neutron currents
 *
 *  Copyright (C) 2023 Jeremy Theler
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

int feenox_problem_gradient_fill_neutron_diffusion(void) {

  for (unsigned int g = 0; g < neutron_diffusion.groups; g++) {
    feenox_call(feenox_problem_fill_aux_solution(neutron_diffusion.Jx[g]));
    if (feenox.pde.dim > 1) {
      feenox_call(feenox_problem_fill_aux_solution(neutron_diffusion.Jy[g]));
      if (feenox.pde.dim > 2) {
        feenox_call(feenox_problem_fill_aux_solution(neutron_diffusion.Jz[g]));
      }
    }
  }
  
  return FEENOX_OK;
  
}

// used only in rough
int feenox_problem_gradient_properties_at_element_nodes_neutron_diffusion(element_t *element, mesh_t *mesh) {

  return FEENOX_OK;
}

int feenox_problem_gradient_fluxes_at_node_alloc_neutron_diffusion(node_t *node) {
  
  unsigned int DG = feenox.pde.dim*neutron_diffusion.groups;
  if (node->flux == NULL) {
    feenox_check_alloc(node->flux = calloc(DG, sizeof(double)));
  } else {
    for (unsigned i = 0; i < DG; i++) {
      node->flux[i] = 0;
    }
  }
  return FEENOX_OK;
}

int feenox_problem_gradient_add_elemental_contribution_to_node_neutron_diffusion(node_t *node, element_t *element, unsigned int j, double rel_weight) {
  
  double current = 0;
  double D = 0;
  
  for (unsigned int g = 0; g < neutron_diffusion.groups; g++) {
    D = neutron_diffusion.D[g].eval(&neutron_diffusion.D[g], node->x, element->physical_group->material);
    for (unsigned int d = 0; d < feenox.pde.dim; d++) {
      current = -D * gsl_matrix_get(element->dphidx_node[j], 0, d);
      node->flux[g*feenox.pde.dim + d] += rel_weight * (current - node->flux[g*feenox.pde.dim + d]);
    }
  }
  
  return FEENOX_OK;
}

int feenox_problem_gradient_fill_fluxes_neutron_diffusion(mesh_t *mesh, size_t j) {
  
  for (unsigned int g = 0; g < neutron_diffusion.groups; g++) {
    feenox_vector_set(neutron_diffusion.Jx[g]->vector_value, j, mesh->node[j].flux[g*feenox.pde.dim + 0]);
    if (feenox.pde.dim > 1) {
      feenox_vector_set(neutron_diffusion.Jy[g]->vector_value, j, mesh->node[j].flux[g*feenox.pde.dim + 1]);
      if (feenox.pde.dim > 2) {
        feenox_vector_set(neutron_diffusion.Jz[g]->vector_value, j, mesh->node[j].flux[g*feenox.pde.dim + 2]);
      }
    }
  }
  
  return FEENOX_OK;
  
}
