/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's mesh-related triangle element routines
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


int feenox_mesh_triang3_init(void) {

  double r[2];
  element_type_t *element_type;
  int j;
  
  element_type = &feenox.mesh.element_types[ELEMENT_TYPE_TRIANGLE3];
  feenox_check_alloc(element_type->name = strdup("triang3"));
  element_type->id = ELEMENT_TYPE_TRIANGLE3;
  element_type->dim = 2;
  element_type->order = 1;
  element_type->nodes = 3;
  element_type->faces = 3;
  element_type->nodes_per_face = 2;
  element_type->h = feenox_mesh_triang3_h;
  element_type->dhdr = feenox_mesh_triang3_dhdr;
  element_type->point_in_element = feenox_mesh_point_in_triangle;
  element_type->element_volume = feenox_mesh_triang_vol;

  // coordenadas de los nodos
/*
Triangle:         

v                 
^                 
|                 
2                 
|`\               
|  `\             
|    `\           
|      `\         
|        `\       
0----------1 --> u 
*/   
  element_type->node_coords = calloc(element_type->nodes, sizeof(double *));
  element_type->node_parents = calloc(element_type->nodes, sizeof(node_relative_t *));    
  for (j = 0; j < element_type->nodes; j++) {
    element_type->node_coords[j] = calloc(element_type->dim, sizeof(double));  
  }
  
  element_type->vertices++;
  element_type->node_coords[0][0] = 0;
  element_type->node_coords[0][1] = 0;
  
  element_type->vertices++;
  element_type->node_coords[1][0] = 1;  
  element_type->node_coords[1][1] = 0;
  
  element_type->vertices++;
  element_type->node_coords[2][0] = 0;  
  element_type->node_coords[2][1] = 1;

  // ------------
  // gauss points and extrapolation matrices
  
  // full integration: 3 points
  feenox_mesh_gauss_init_triang3(element_type, &element_type->gauss[integration_full]);
  element_type->gauss[integration_full].extrap = gsl_matrix_calloc(element_type->nodes, 3);

  // reduced integration: 1 point
  feenox_mesh_gauss_init_triang1(element_type, &element_type->gauss[integration_reduced]);
  element_type->gauss[integration_reduced].extrap = gsl_matrix_calloc(element_type->nodes, 1);
  
  // the two extrapolation matrices
  // first the full one
  r[0] = -1.0/3.0;
  r[1] = -1.0/3.0;
  for (j = 0; j < 3; j++) {
    gsl_matrix_set(element_type->gauss[integration_full].extrap, 0, j, feenox_mesh_triang3_h(j, r));
  }  
  
  r[0] = +5.0/3.0;
  r[1] = -1.0/3.0;
  for (j = 0; j < 3; j++) {
    gsl_matrix_set(element_type->gauss[integration_full].extrap, 1, j, feenox_mesh_triang3_h(j, r));
  }  

  r[0] = -1.0/3.0;
  r[1] = +5.0/3.0;
  for (j = 0; j < 3; j++) {
    gsl_matrix_set(element_type->gauss[integration_full].extrap, 2, j, feenox_mesh_triang3_h(j, r));
  }  
  
  // the reduced one is a vector of ones
  for (j = 0; j < element_type->nodes; j++) {
    // reduced
    gsl_matrix_set(element_type->gauss[integration_reduced].extrap, j, 0, 1.0);
  }
  
  return FEENOX_OK;    
}



void feenox_mesh_gauss_init_triang3(element_type_t *element_type, gauss_t *gauss) {

  // ---- three Gauss points
  feenox_mesh_alloc_gauss(gauss, element_type, 3);
  
  gauss->w[0] = 1.0/2.0 * 1.0/3.0;
  gauss->r[0][0] = 1.0/6.0;
  gauss->r[0][1] = 1.0/6.0;
  
  gauss->w[1] = 1.0/2.0 * 1.0/3.0;
  gauss->r[1][0] = 2.0/3.0;
  gauss->r[1][1] = 1.0/6.0;
  
  gauss->w[2] = 1.0/2.0 * 1.0/3.0;
  gauss->r[2][0] = 1.0/6.0;
  gauss->r[2][1] = 2.0/3.0;

  feenox_mesh_init_shape_at_gauss(gauss, element_type);
    

  return;
}

void feenox_mesh_gauss_init_triang1(element_type_t *element_type, gauss_t *gauss) {

  // ---- one Gauss point
  feenox_mesh_alloc_gauss(gauss, element_type, 1);
  
  gauss->w[0] = 0.5 * 1.0;
  gauss->r[0][0] = 1.0/3.0;
  gauss->r[0][1] = 1.0/3.0;

  feenox_mesh_init_shape_at_gauss(gauss, element_type);  

  return;
  
}


double feenox_mesh_triang3_h(int j, double *vec_r) {
  double r = vec_r[0];
  double s = vec_r[1];

  switch (j) {
    case 0:
      return 1-r-s;
      break;
    case 1:
      return r;
      break;
    case 2:
      return s;
      break;
  }

  return 0;

}

double feenox_mesh_triang3_dhdr(int j, int m, double *vec_r) {
  
  switch(j) {
    case 0:
      if (m == 0) {
        return -1;
      } else {
        return -1;
      }
      break;
    case 1:
      if (m == 0) {
        return 1;
      } else {
        return 0;
      }
      break;
    case 2:
      if (m == 0) {
        return 0;
      } else {
        return 1;
      }
      break;
  }

  return 0;

}


int feenox_mesh_point_in_triangle(element_t *element, const double *x) {

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
  return 0;
*/
  
// metodo de coordenadas baricentricas
//  http://en.wikipedia.org/wiki/Barycentric_coordinate_system  
  double lambda1, lambda2, lambda3;
  double x1 = element->node[0]->x[0];
  double x2 = element->node[1]->x[0];
  double x3 = element->node[2]->x[0];
  double y1 = element->node[0]->x[1];
  double y2 = element->node[1]->x[1];
  double y3 = element->node[2]->x[1];
  double zero, one;
  
  lambda1 = ((y2-y3)*(x[0]-x3) + (x3-x2)*(x[1]-y3))/((y2-y3)*(x1-x3) + (x3-x2)*(y1-y3));
  lambda2 = ((y3-y1)*(x[0]-x3) + (x1-x3)*(x[1]-y3))/((y2-y3)*(x1-x3) + (x3-x2)*(y1-y3));
  lambda3 = 1 - lambda1 - lambda2;
  
  zero = -feenox_var_value(feenox.mesh.vars.eps);
  one = 1+feenox_var_value(feenox.mesh.vars.eps);
  
  return (lambda1 > zero && lambda1 < one &&
          lambda2 > zero && lambda2 < one &&
          lambda3 > zero && lambda3 < one);

}


double feenox_mesh_triang_vol(element_t *this) {
  if (this->volume == 0) {
    this->volume = 0.5 * fabs(feenox_mesh_subtract_cross_2d(this->node[0]->x, this->node[1]->x, this->node[2]->x));
  }  
  return this->volume;
}
