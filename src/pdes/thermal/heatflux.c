/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for the heat equation: computation of heat fluxes
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
#include "thermal.h"

int feenox_problem_gradient_fill_thermal(void) {
  
  feenox_call(feenox_problem_fill_aux_solution(thermal.qx));
  if (feenox.pde.dim > 1) {
    feenox_call(feenox_problem_fill_aux_solution(thermal.qy));
    if (feenox.pde.dim > 2) {
      feenox_call(feenox_problem_fill_aux_solution(thermal.qz));
    }
  }
  
  return FEENOX_OK;
  
}

// used only in rough
int feenox_problem_gradient_properties_at_element_nodes_thermal(element_t *element, mesh_t *mesh) {

  return FEENOX_OK;
}

int feenox_problem_gradient_fluxes_at_node_alloc_thermal(node_t *node) {
  if (node->flux == NULL) {
    feenox_check_alloc(node->flux = calloc(feenox.pde.dim, sizeof(double)));
  } else {
    for (unsigned int d = 0; d < feenox.pde.dim; d++) {
      node->flux[d] = 0;
    }
  }
  return FEENOX_OK;
}

int feenox_problem_gradient_add_elemental_contribution_to_node_thermal(node_t *node, element_t *element, unsigned int j, double rel_weight) {
  
  double heat_flux = 0;
  double k = thermal.k.eval(&thermal.k, node->x, element->physical_group->material);
  for (unsigned int d = 0; d < feenox.pde.dim; d++) {
    heat_flux = -k * gsl_matrix_get(element->dphidx_node[j], 0, d);
    node->flux[d] += rel_weight * (heat_flux - node->flux[d]);
  }
  
  return FEENOX_OK;
}

int feenox_problem_gradient_fill_fluxes_thermal(mesh_t *mesh, size_t j) {
  
  // TODO: wrappers so pdes don't have to access the vectors
  feenox_vector_set(thermal.qx->vector_value, j, mesh->node[j].flux[0]);
  if (feenox.pde.dim > 1) {
    feenox_vector_set(thermal.qy->vector_value, j, mesh->node[j].flux[1]);
    if (feenox.pde.dim > 2) {
      feenox_vector_set(thermal.qz->vector_value, j, mesh->node[j].flux[2]);
    }
  }
  
  // TODO: uncertainties
  
  return FEENOX_OK;
  
}
