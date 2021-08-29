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
#include "thermal.h"
extern feenox_t feenox;
extern thermal_t thermal;

int feenox_problem_gradient_fill_thermal(void) {
  
  feenox_gradient_fill(thermal, qx);
  if (feenox.pde.dim > 1) {
    feenox_gradient_fill(thermal, qy);
    if (feenox.pde.dim > 2) {
      feenox_gradient_fill(thermal, qz);
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
    unsigned int m = 0;
    for (m = 0; m < feenox.pde.dim; m++) {
      node->flux[m] = 0;
    }
  }
  return FEENOX_OK;
}

int feenox_problem_gradient_add_elemental_contribution_to_node_thermal(node_t *node, element_t *element, unsigned int j, double rel_weight) {
  
  double heat_flux = 0;
  double k = thermal.k.eval(&thermal.k, node->x, element->physical_group->material);
  unsigned int m = 0;
  for (m = 0; m < feenox.pde.dim; m++) {
    heat_flux = -k * gsl_matrix_get(element->dphidx_node[j], 0, m);
    node->flux[m] += rel_weight * (heat_flux - node->flux[m]);
  }
  
  return FEENOX_OK;
}

int feenox_problem_gradient_fill_fluxes_thermal(mesh_t *mesh, size_t j) {
  
  thermal.qx->data_value[j] = mesh->node[j].flux[0];
  if (feenox.pde.dim > 1) {
    thermal.qy->data_value[j] = mesh->node[j].flux[1];
    if (feenox.pde.dim > 2) {
      thermal.qz->data_value[j] = mesh->node[j].flux[2];
    }
  }
  
  // TODO: uncertainties
  
  return FEENOX_OK;
  
}