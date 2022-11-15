/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's mesh-related line3 element routines
 *
 *  Copyright (C) 2017--2020 jeremy theler
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
#include "element.h"

// --------------------------------------------------------------
// three-node line
// --------------------------------------------------------------


int feenox_mesh_line3_init(void) {

  double r[1];
  element_type_t *element_type;
  int j, v;
  
  element_type = &feenox.mesh.element_types[ELEMENT_TYPE_LINE3];
  feenox_check_alloc(element_type->name = strdup("line3"));
  element_type->id = ELEMENT_TYPE_LINE3;
  element_type->dim = 1;
  element_type->order = 2;
  element_type->nodes = 3;
  element_type->faces = 2;
  element_type->nodes_per_face = 1;
  element_type->h = feenox_mesh_line3_h;
  element_type->dhdr = feenox_mesh_line3_dhdr;
  element_type->point_in_element = feenox_mesh_point_in_line;
  element_type->element_volume = feenox_mesh_line_vol;

  // from Gmsh’ doc
/*
Line3:      
            
0-----2----1   
*/  
  element_type->node_coords = calloc(element_type->nodes, sizeof(double *));
  element_type->node_parents = calloc(element_type->nodes, sizeof(node_relative_t *));  
  for (j = 0; j < element_type->nodes; j++) {
    element_type->node_coords[j] = calloc(element_type->dim, sizeof(double));  
  }
  
  element_type->vertices++;
  element_type->node_coords[0][0] = -1;

  element_type->vertices++;
  element_type->node_coords[1][0] = 1;
  
  feenox_mesh_add_node_parent(&element_type->node_parents[2], 0);
  feenox_mesh_add_node_parent(&element_type->node_parents[2], 1);
  feenox_mesh_compute_coords_from_parent(element_type, 2);

  // ------------
  // gauss points and extrapolation matrices
  
  // full integration: three points
  feenox_mesh_gauss_init_line3(element_type, &element_type->gauss[integration_full]);
  feenox_check_alloc(element_type->gauss[integration_full].extrap = feenox_lowlevel_matrix_calloc(element_type->nodes, 3));
  
  for (j = 0; j < element_type->nodes; j++) {
    r[0] = M_SQRT5/M_SQRT3 * element_type->node_coords[j][0];
    
    for (v = 0; v < 3; v++) {
      feenox_lowlevel_matrix_set(element_type->gauss[integration_full].extrap, j, v, feenox_mesh_line3_h(v, r));
    }
  }
  
  // reduced integration: two points
  feenox_mesh_gauss_init_line2(element_type, &element_type->gauss[integration_reduced]);
  feenox_check_alloc(element_type->gauss[integration_reduced].extrap = feenox_lowlevel_matrix_calloc(element_type->nodes, 2));
  
  for (j = 0; j < element_type->nodes; j++) {
    r[0] = M_SQRT3 * element_type->node_coords[j][0];
    
    for (v = 0; v < 2; v++) {
      feenox_lowlevel_matrix_set(element_type->gauss[integration_reduced].extrap, j, v, feenox_mesh_line2_h(v, r));
    }
  }
  
  
  return FEENOX_OK;
}

double feenox_mesh_line3_h(int k, double *vec_r) {
  double r = vec_r[0];

  // Gmsh ordering
  switch (k) {
    case 0:
      return 0.5*r*(r-1);
      break;
    case 1:
      return 0.5*r*(r+1);
      break;
    case 2:
      return (1+r)*(1-r);
      break;
  }

  return 0;

}

double feenox_mesh_line3_dhdr(int k, int m, double *vec_r) {
  double r = vec_r[0];

  switch(k) {
    case 0:
      if (m == 0) {
        return r-0.5;
      }
      break;
    case 1:
      if (m == 0) {
        return r+0.5;
      }
      break;
    case 2:
      if (m == 0) {
        return -2*r;
      }
      break;
  }

  return 0;
}
