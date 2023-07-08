/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's mesh-related line element routines
 *
 *  Copyright (C) 2014--2023 jeremy theler
 *
 *  This file is part of feenox.
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
 *  along with feenox.  If not, see <http://www.gnu.org/licenses/>.
 *------------------- ------------  ----    --------  --     -       -         -
 */
#include "../../feenox.h"
#include "../element.h"

// --------------------------------------------------------------
// two-node line
// --------------------------------------------------------------


int feenox_mesh_line2_init(void) {

  double xi[1];
  unsigned int j, q;
  
  element_type_t *element_type = &feenox.mesh.element_types[ELEMENT_TYPE_LINE2];
  element_type->name = "line2";
  element_type->id = ELEMENT_TYPE_LINE2;
  element_type->dim = 1;
  element_type->order = 1;
  element_type->nodes = 2;
  element_type->faces = 2;
  element_type->nodes_per_face = 1;
  element_type->h = feenox_mesh_line2_h;
  element_type->dhdxi = feenox_mesh_line2_dhdr;
  element_type->point_inside = feenox_mesh_point_in_line;
  element_type->volume = feenox_mesh_line_volume;
  element_type->area = feenox_mesh_line_area;
  element_type->size = feenox_mesh_line_size;

  // from Gmsh’ doc
/*
Line:              
                   
0----------1 --> u   
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
  
  
  // ------------
  // gauss points and extrapolation matrices
  
  // full integration: two points
  feenox_mesh_gauss_init_line2(element_type, &element_type->gauss[integration_full]);
  element_type->gauss[integration_full].extrap = gsl_matrix_calloc(element_type->nodes, 2);
  
  for (j = 0; j < element_type->nodes; j++) {
    xi[0] = M_SQRT3 * element_type->node_coords[j][0];
    
    for (q = 0; q < 2; q++) {
      gsl_matrix_set(element_type->gauss[integration_full].extrap, j, q, feenox_mesh_line2_h(q, xi));
    }
  }
  

  // reduced integration: one point
  feenox_mesh_gauss_init_line1(element_type, &element_type->gauss[integration_reduced]);
  element_type->gauss[integration_reduced].extrap = gsl_matrix_calloc(element_type->nodes, 1);
  
  for (j = 0; j < element_type->nodes; j++) {
    gsl_matrix_set(element_type->gauss[integration_reduced].extrap, j, 0, 1.0);
  }
  
  return FEENOX_OK;
}


int feenox_mesh_gauss_init_line1(element_type_t *element_type, gauss_t *gauss) {
  
  // ---- one Gauss point ----  
  feenox_call(feenox_mesh_alloc_gauss(gauss, element_type, 1));

  gauss->w[0] = 2.0;
  gauss->xi[0][0] = 0.0;

  feenox_call(feenox_mesh_init_shape_at_gauss(gauss, element_type));
  
  return FEENOX_OK;
}  
  
int feenox_mesh_gauss_init_line2(element_type_t *element_type, gauss_t *gauss) {
  
  // ---- two Gauss points ----  
  feenox_call(feenox_mesh_alloc_gauss(gauss, element_type, 2));

  gauss->w[0] = 1.0;
  gauss->xi[0][0] = -1.0/M_SQRT3;

  gauss->w[1] = 1.0;
  gauss->xi[1][0] = +1.0/M_SQRT3;

  feenox_call(feenox_mesh_init_shape_at_gauss(gauss, element_type));
  
  return FEENOX_OK;
}  


int feenox_mesh_gauss_init_line3(element_type_t *element_type, gauss_t *gauss) {
  
  // ---- two Gauss points ----  
  feenox_call(feenox_mesh_alloc_gauss(gauss, element_type, 3));

  gauss->w[0] = 5.0/9.0;
  gauss->xi[0][0] = -M_SQRT3/M_SQRT5;

  gauss->w[1] = 5.0/9.0;
  gauss->xi[1][0] = +M_SQRT3/M_SQRT5;

  gauss->w[2] = 8.0/9.0;
  gauss->xi[2][0] = 0.0;
  
  feenox_call(feenox_mesh_init_shape_at_gauss(gauss, element_type));
  
  return FEENOX_OK;
}  


double feenox_mesh_line2_h(int j, double *vec_xi) {
  double xi = vec_xi[0];

  // numeracion gmsh
  switch (j) {
    case 0:
      return 0.5*(1-xi);
      break;
    case 1:
      return 0.5*(1+xi);
      break;
  }

  return 0;

}

double feenox_mesh_line2_dhdr(int j, int d, double *vec_xi) {

  switch(j) {
    case 0:
      if (d == 0) {
        return -0.5;
      }
      break;
    case 1:
      if (d == 0) {
        return 0.5;
      }
      break;
  }

  return 0;
}



int feenox_mesh_point_in_line(element_t *this, const double *x) {
  return ((x[0] >= this->node[0]->x[0] && x[0] <= this->node[1]->x[0]) ||
          (x[0] >= this->node[1]->x[0] && x[0] <= this->node[0]->x[0]));
}


double feenox_mesh_line_volume(element_t *this) {
  
  if (this->volume == 0) {
    this->volume = fabs(this->node[1]->x[0] - this->node[0]->x[0]);
  }  
  return this->volume;
}

double feenox_mesh_line_area(element_t *this) {
  
  if (this->area == 0) {
    this->area = 1;
  }  
  return this->area;
}

double feenox_mesh_line_size(element_t *this) {
  
  if (this->size == 0) {
    this->size = feenox_mesh_line_volume(this);
  }  
  return this->size;
}