/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's mesh-related point element routines
 *
 *  Copyright (C) 2014--2018 jeremy theler
 *
 *  This file is part of feenox.
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
#include "../feenox.h"
extern feenox_t feenox;

#include "element.h"

// --------------------------------------------------------------
// punto de un nodo
// --------------------------------------------------------------
int feenox_mesh_one_node_point_init(void) {
  
  element_type_t *element_type;
  
  element_type = &feenox.mesh.element_types[ELEMENT_TYPE_POINT1];
  element_type->name = strdup("point");
  element_type->id = ELEMENT_TYPE_POINT1;
  element_type->dim = 0;
  element_type->order = 0;
  element_type->nodes = 1;
  element_type->faces = 0;
  element_type->nodes_per_face = 0;
  element_type->h = feenox_mesh_one_node_point_h;
  element_type->dhdr = feenox_mesh_one_node_point_dhdr;
  element_type->element_volume = feenox_mesh_point_vol;
  element_type->point_in_element = NULL;
  
  // ------------
  // gauss points and extrapolation matrices

  // el primero es el default
  feenox_mesh_alloc_gauss(&element_type->gauss[integration_full], element_type, 1);
  element_type->gauss[integration_full].w[0] = 1.0;
  feenox_mesh_init_shape_at_gauss(&element_type->gauss[integration_full], element_type);
  
  feenox_mesh_alloc_gauss(&element_type->gauss[integration_reduced], element_type, 1);
  element_type->gauss[integration_reduced].w[0] = 1.0;
  feenox_mesh_init_shape_at_gauss(&element_type->gauss[integration_reduced], element_type);
  
  return FEENOX_OK;
}
double feenox_mesh_one_node_point_h(int i, double *vec_r) {
  return 1;
}

double feenox_mesh_one_node_point_dhdr(int i, int j, double *vec_r) {
  return 0;
}

double feenox_mesh_point_vol(element_t *this) {
  return 0;
}
