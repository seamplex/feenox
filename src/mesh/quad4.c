/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's mesh-related quadrangle element routines
 *
 *  Copyright (C) 2014--2020 jeremy theler
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

// --------------------
// four-node quadrangle
// --------------------
int feenox_mesh_quad4_init(void) {
  
  double r[2];
  element_type_t *element_type;
  int j, v;
  
  element_type = &feenox.mesh.element_types[ELEMENT_TYPE_QUADRANGLE4];
  feenox_check_alloc(element_type->name = strdup("quad4"));
  element_type->id = ELEMENT_TYPE_QUADRANGLE4;
  element_type->dim = 2;
  element_type->order = 1;
  element_type->nodes = 4;
  element_type->faces = 4;
  element_type->nodes_per_face = 2;
  element_type->h = feenox_mesh_quad4_h;
  element_type->dhdr = feenox_mesh_quad4_dhdr;
  element_type->point_in_element = feenox_mesh_point_in_quadrangle;
  element_type->element_volume = feenox_mesh_quad_vol;

  // from Gmsh’ doc
/*
      v
      ^
      |
3-----------2       
|     |     |       
|     |     |       
|     +---- | --> u 
|           |       
|           |       
0-----------1       
*/     
  element_type->node_coords = calloc(element_type->nodes, sizeof(double *));
  element_type->node_parents = calloc(element_type->nodes, sizeof(node_relative_t *));  
  for (j = 0; j < element_type->nodes; j++) {
    element_type->node_coords[j] = calloc(element_type->dim, sizeof(double));  
  }
  
  element_type->vertices++;
  element_type->node_coords[0][0] = -1;
  element_type->node_coords[0][1] = -1;
  
  element_type->vertices++;
  element_type->node_coords[1][0] = +1;  
  element_type->node_coords[1][1] = -1;
  
  element_type->vertices++;
  element_type->node_coords[2][0] = +1;  
  element_type->node_coords[2][1] = +1;

  element_type->vertices++;
  element_type->node_coords[3][0] = -1;
  element_type->node_coords[3][1] = +1;

  // ------------
  // gauss points and extrapolation matrices
  
  // full integration: 2x2
  feenox_mesh_gauss_init_quad4(element_type, &element_type->gauss[integration_full]);
  element_type->gauss[integration_full].extrap = gsl_matrix_calloc(element_type->nodes, 4);

  for (j = 0; j < element_type->nodes; j++) {
    r[0] = M_SQRT3 * element_type->node_coords[j][0];
    r[1] = M_SQRT3 * element_type->node_coords[j][1];
    
    for (v = 0; v < 4; v++) {
      gsl_matrix_set(element_type->gauss[integration_full].extrap, j, v, feenox_mesh_quad4_h(v, r));
    }
  }
  
  
  // reduced integration: 1x1
  feenox_mesh_gauss_init_quad1(element_type, &element_type->gauss[integration_reduced]);
  element_type->gauss[integration_reduced].extrap = gsl_matrix_calloc(element_type->nodes, 1);
  
  for (j = 0; j < element_type->nodes; j++) {
    gsl_matrix_set(element_type->gauss[integration_reduced].extrap, j, 0, 1.0);
  }

  return FEENOX_OK;    
}

void feenox_mesh_gauss_init_quad1(element_type_t *element_type, gauss_t *gauss) {

  // ---- one Gauss point  ----  
  feenox_mesh_alloc_gauss(gauss, element_type, 1);
  
  gauss->w[0] = 4 * 1.0;
  gauss->r[0][0] = 0.0;
  gauss->r[0][1] = 0.0;

  feenox_mesh_init_shape_at_gauss(gauss, element_type);  
  
  return;
}

void feenox_mesh_gauss_init_quad4(element_type_t *element_type, gauss_t *gauss) {

  // ---- four Gauss points ----  
  feenox_mesh_alloc_gauss(gauss, element_type, 4);

  gauss->w[0] = 4 * 0.25;
  gauss->r[0][0] = -1.0/M_SQRT3;
  gauss->r[0][1] = -1.0/M_SQRT3;

  gauss->w[1] = 4 * 0.25;
  gauss->r[1][0] = +1.0/M_SQRT3;
  gauss->r[1][1] = -1.0/M_SQRT3;
 
  gauss->w[2] = 4 * 0.25;
  gauss->r[2][0] = +1.0/M_SQRT3;
  gauss->r[2][1] = +1.0/M_SQRT3;

  gauss->w[3] = 4 * 0.25;
  gauss->r[3][0] = -1.0/M_SQRT3;
  gauss->r[3][1] = +1.0/M_SQRT3;

  feenox_mesh_init_shape_at_gauss(gauss, element_type);
  
  return;
  
}

void feenox_mesh_gauss_init_quad9(element_type_t *element_type, gauss_t *gauss) {
  
  double w1 = 25.0/81.0;
  double w2 = 40.0/81.0;
  double w3 = 64.0/81.0;
  
  // ---- nine Gauss points
  feenox_mesh_alloc_gauss(gauss, element_type, 9);
  
  gauss->w[0] = w1;
  gauss->r[0][0] = -M_SQRT3/M_SQRT5;
  gauss->r[0][1] = -M_SQRT3/M_SQRT5;

  gauss->w[1] = w1;
  gauss->r[1][0] = +M_SQRT3/M_SQRT5;
  gauss->r[1][1] = -M_SQRT3/M_SQRT5;
 
  gauss->w[2] = w1;
  gauss->r[2][0] = +M_SQRT3/M_SQRT5;
  gauss->r[2][1] = +M_SQRT3/M_SQRT5;

  gauss->w[3] = w1;
  gauss->r[3][0] = -M_SQRT3/M_SQRT5;
  gauss->r[3][1] = +M_SQRT3/M_SQRT5;
    
  gauss->w[4] = w2;
  gauss->r[4][0] = 0.0;
  gauss->r[4][1] = -M_SQRT3/M_SQRT5;

  gauss->w[5] = w2;
  gauss->r[5][0] = +M_SQRT3/M_SQRT5;
  gauss->r[5][1] = 0.0;
 
  gauss->w[6] = w2;
  gauss->r[6][0] = 0.0;
  gauss->r[6][1] = +M_SQRT3/M_SQRT5;

  gauss->w[7] = w2;
  gauss->r[7][0] = -M_SQRT3/M_SQRT5;
  gauss->r[7][1] = 0.0;
   
  gauss->w[8] = w3;
  gauss->r[8][0] = 0.0;
  gauss->r[8][1] = 0.0;

  feenox_mesh_init_shape_at_gauss(gauss, element_type);
    
  return;
}


double feenox_mesh_quad4_h(int j, double *vec_r) {
  double r = vec_r[0];
  double s = vec_r[1];

  switch (j) {
    case 0:
      return 0.25*(1-r)*(1-s);
      break;
    case 1:
      return 0.25*(1+r)*(1-s);
      break;
    case 2:
      return 0.25*(1+r)*(1+s);
      break;
    case 3:
      return 0.25*(1-r)*(1+s);
      break;
  }

  return 0;

}

double feenox_mesh_quad4_dhdr(int j, int m, double *vec_r) {
  double r = vec_r[0];
  double s = vec_r[1];

  switch(j) {
    case 0:
      if (m == 0) {
        return -0.25*(1-s);
      } else {
        return -0.25*(1-r);
      }
      break;
    case 1:
      if (m == 0) {
        return 0.25*(1-s);
      } else {
        return -0.25*(1+r);
      }
      break;
    case 2:
      if (m == 0) {
        return 0.25*(1+s);
      } else {
        return 0.25*(1+r);
      }
      break;
    case 3:
      if (m == 0) {
        return -0.25*(1+s);
      } else {
        return 0.25*(1-r);
      }
      break;

  }

  return 0;


}



int feenox_mesh_point_in_quadrangle(element_t *element, const double *x) {

/*
  double z1, z2, z3;

  z1 = feenox_mesh_subtract_cross_2d(element->node[0]->x, element->node[1]->x, x);
  z2 = feenox_mesh_subtract_cross_2d(element->node[1]->x, element->node[2]->x, x);
  z3 = feenox_mesh_subtract_cross_2d(element->node[2]->x, element->node[0]->x, x);
  
  if ((GSL_SIGN(z1) == GSL_SIGN(z2) && GSL_SIGN(z2) == GSL_SIGN(z3)) ||
      (fabs(z1) < 1e-4 && GSL_SIGN(z2) == GSL_SIGN(z3)) ||
      (fabs(z2) < 1e-4 && GSL_SIGN(z1) == GSL_SIGN(z3)) ||          
      (fabs(z3) < 1e-4 && GSL_SIGN(z1) == GSL_SIGN(z2)) ) {
    return 1;
  }
  
  
  z1 = feenox_mesh_subtract_cross_2d(element->node[0]->x, element->node[2]->x, x);
  z2 = feenox_mesh_subtract_cross_2d(element->node[2]->x, element->node[3]->x, x);
  z3 = feenox_mesh_subtract_cross_2d(element->node[3]->x, element->node[0]->x, x);

  if ((GSL_SIGN(z1) == GSL_SIGN(z2) && GSL_SIGN(z2) == GSL_SIGN(z3)) ||
      (fabs(z1) < 1e-4 && GSL_SIGN(z2) == GSL_SIGN(z3)) ||
      (fabs(z2) < 1e-4 && GSL_SIGN(z1) == GSL_SIGN(z3)) ||          
      (fabs(z3) < 1e-4 && GSL_SIGN(z1) == GSL_SIGN(z2)) ) {
    return 1;
  }
  
  return 0;
*/

  int i, j;

  element_t triang;
  triang.type = &feenox.mesh.element_types[ELEMENT_TYPE_TRIANGLE3];
  triang.node = calloc(triang.type->nodes, sizeof(node_t *));

  for (i = 0; i < element->type->faces; i++) {
    for (j = 0; j < triang.type->faces; j++) {
      triang.node[j] = element->node[(i+j) % element->type->faces];
    }
    if (feenox_mesh_point_in_triangle(&triang, x)) {
      feenox_free(triang.node);
      return 1;
    }
  }

  feenox_free(triang.node);

  return 0;
}

double feenox_mesh_quad_vol(element_t *this) {

  if (this->volume == 0) {
    this->volume = fabs(0.5*
                           ((this->node[2]->x[0]-this->node[0]->x[0])*(this->node[3]->x[1]-this->node[1]->x[1])
                           +(this->node[1]->x[0]-this->node[3]->x[0])*(this->node[2]->x[1]-this->node[0]->x[1]))
                          );
  }  
  
  return this->volume;
}

