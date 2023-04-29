/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's mesh-related tetrahedron element routines
 *
 *  Copyright (C) 2015--2023 jeremy theler
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
#include "../feenox.h"
#include "element.h"

// -------------------------------------
// four-node tetrahedron
// -------------------------------------

int feenox_mesh_tet4_init(void) {
  
  element_type_t *element_type;
  double r[3];
  double a, b, c, d;
  int j, v;
  
  element_type = &feenox.mesh.element_types[ELEMENT_TYPE_TETRAHEDRON4];
  feenox_check_alloc(element_type->name = strdup("tet4"));
  element_type->id = ELEMENT_TYPE_TETRAHEDRON4;
  element_type->dim = 3;
  element_type->order = 1;
  element_type->nodes = 4;
  element_type->faces = 4;
  element_type->nodes_per_face = 3;
  element_type->h = feenox_mesh_tet4_h;
  element_type->dhdr = feenox_mesh_tet4_dhdr;
  element_type->point_inside = feenox_mesh_point_in_tetrahedron;
  element_type->volume = feenox_mesh_tet_volume;
  element_type->area = feenox_mesh_tet_area;


  // from Gmsh’ doc
/*
Tetrahedron:                     

                   v
                 .
               ,/
              /
           2                     
         ,/|`\
       ,/  |  `\
     ,/    '.   `\
   ,/       |     `\
 ,/         |       `\
0-----------'.--------1 --> u    
 `\.         |      ,/           
    `\.      |    ,/             
       `\.   '. ,/               
          `\. |/                 
             `3                  
                `\.
                   ` w

*/     
  feenox_check_alloc(element_type->node_coords = calloc(element_type->nodes, sizeof(double *)));
  feenox_check_alloc(element_type->node_parents = calloc(element_type->nodes, sizeof(node_relative_t *)));  
  for (j = 0; j < element_type->nodes; j++) {
    feenox_check_alloc(element_type->node_coords[j] = calloc(element_type->dim, sizeof(double)));
  }
  
  element_type->vertices++;
  element_type->node_coords[0][0] = 0;
  element_type->node_coords[0][1] = 0;
  element_type->node_coords[0][2] = 0;
  
  element_type->vertices++;
  element_type->node_coords[1][0] = 1;  
  element_type->node_coords[1][1] = 0;
  element_type->node_coords[1][2] = 0;
  
  element_type->vertices++;
  element_type->node_coords[2][0] = 0;  
  element_type->node_coords[2][1] = 1;
  element_type->node_coords[2][2] = 0;

  element_type->vertices++;
  element_type->node_coords[3][0] = 0;  
  element_type->node_coords[3][1] = 0;
  element_type->node_coords[3][2] = 1;
  
  // ------------
  // gauss points and extrapolation matrices
  
  // full integration: 4 points
  feenox_mesh_gauss_init_tet4(element_type, &element_type->gauss[integration_full]);
  feenox_check_alloc(element_type->gauss[integration_full].extrap = gsl_matrix_calloc(element_type->nodes, 4));

  // reduced integration: 1 point
  feenox_mesh_gauss_init_tet1(element_type, &element_type->gauss[integration_reduced]);
  feenox_check_alloc(element_type->gauss[integration_reduced].extrap = gsl_matrix_calloc(element_type->nodes, 1));
  
  // the two extrapolation matrices
  a = (5.0-M_SQRT5)/20.0;
  b = (5.0+3.0*M_SQRT5)/20.0;
  c = -a/(b-a);
  d = 1+(1-b)/(b-a);
    
  r[0] = c;
  r[1] = c;
  r[2] = c;
  for (v = 0; v < 4; v++) {
    gsl_matrix_set(element_type->gauss[integration_full].extrap, 0, v, feenox_mesh_tet4_h(v, r));
  }

  r[0] = d;
  r[1] = c;
  r[2] = c;
  for (v = 0; v < 4; v++) {
    gsl_matrix_set(element_type->gauss[integration_full].extrap, 1, v, feenox_mesh_tet4_h(v, r));
  }

  r[0] = c;
  r[1] = d;
  r[2] = c;
  for (v = 0; v < 4; v++) {
    gsl_matrix_set(element_type->gauss[integration_full].extrap, 2, v, feenox_mesh_tet4_h(v, r));
  }

  r[0] = c;
  r[1] = c;
  r[2] = d;
  for (v = 0; v < 4; v++) {
    gsl_matrix_set(element_type->gauss[integration_full].extrap, 3, v, feenox_mesh_tet4_h(v, r));
  }
  
  // reduced
  for (j = 0; j < element_type->nodes; j++) {
    gsl_matrix_set(element_type->gauss[integration_reduced].extrap, j, 0, 1.0);
  }  

  return FEENOX_OK;
}


int feenox_mesh_gauss_init_tet1(element_type_t *element_type, gauss_t *gauss) {
 
  // ---- one Gauss point ----  
  feenox_call(feenox_mesh_alloc_gauss(gauss, element_type, 4));
    
  gauss->w[0] = 1.0/6.0 * 1.0;
  gauss->r[0][0] = 1.0/4.0;
  gauss->r[0][1] = 1.0/4.0;
  gauss->r[0][2] = 1.0/4.0;

  feenox_call(feenox_mesh_init_shape_at_gauss(gauss, element_type));  
    
  return FEENOX_OK;
}

int feenox_mesh_gauss_init_tet4(element_type_t *element_type, gauss_t *gauss) {

  double a = (5.0-M_SQRT5)/20.0;
  double b = (5.0+3.0*M_SQRT5)/20.0;

  // ---- two Gauss points ----  
  feenox_call(feenox_mesh_alloc_gauss(gauss, element_type, 4));
    
  gauss->w[0] = 1.0/6.0 * 1.0/4.0;
  gauss->r[0][0] = a;
  gauss->r[0][1] = a;
  gauss->r[0][2] = a;
  
  gauss->w[1] = 1.0/6.0 * 1.0/4.0;
  gauss->r[1][0] = b;
  gauss->r[1][1] = a;
  gauss->r[1][2] = a;
 
  gauss->w[2] = 1.0/6.0 * 1.0/4.0;
  gauss->r[2][0] = a;
  gauss->r[2][1] = b;
  gauss->r[2][2] = a;
    
  gauss->w[3] = 1.0/6.0 * 1.0/4.0;
  gauss->r[3][0] = a;
  gauss->r[3][1] = a;
  gauss->r[3][2] = b;

  feenox_call(feenox_mesh_init_shape_at_gauss(gauss, element_type));  
  
  return FEENOX_OK;
}



double feenox_mesh_tet4_h(int j, double *vec_r) {
  double r = vec_r[0];
  double s = vec_r[1];
  double t = vec_r[2];

  switch (j) {
    case 0:
      return 1-r-s-t;
      break;
    case 1:
      return r;
      break;
    case 2:
      return s;
      break;
    case 3:
      return t;
      break;
  }

  return 0;

}


double feenox_mesh_tet4_dhdr(int j, int m, double *vec_r) {
  
  switch (j) {
    case 0:
      switch(m) {
        case 0:
          return -1;
        break;
        case 1:
          return -1;
        break;
        case 2:
          return -1;
        break;
      }
    break;
    case 1:
      switch(m) {
        case 0:
          return +1;
        break;
        case 1:
          return 0;
        break;
        case 2:
          return 0;
        break;
      }
    break;
    case 2:
      switch(m) {
        case 0:
          return 0;
        break;
        case 1:
          return +1;
        break;
        case 2:
          return 0;
        break;
      }
    break;
    case 3:
      switch(m) {
        case 0:
          return 0;
        break;
        case 1:
          return 0;
        break;
        case 2:
          return +1;
        break;
      }
    break;
  }

  return 0;


}

int feenox_mesh_point_in_tetrahedron(element_t *element, const double *x) {

// http://en.wikipedia.org/wiki/Barycentric_coordinate_system  
  double T[3][3];
  for (int j = 1; j < 4; j++) {
    for (int g = 0; g < 3; g++) {
      T[g][j-1] = element->node[j]->x[g] - element->node[0]->x[g];
    }
  }

  double xx0[3];
  for (int g = 0; g < 3; g++) {
    xx0[g] = x[g] - element->node[0]->x[g];
  }
  
  double t4  = T[2][0] * T[0][1];
  double t6  = T[2][0] * T[0][2];
  double t8  = T[1][0] * T[0][1];
  double t10 = T[1][0] * T[0][2];
  double t12 = T[0][0] * T[1][1];
  double t14 = T[0][0] * T[1][2];
  double det = t4 * T[1][2] - t6 * T[1][1] - t8 * T[2][2] + t10 * T[2][1] + t12 * T[2][2] - t14 * T[2][1];
  if (fabs(det) < 1e-12) {
    // if the element is degenerate it cannot contain any point
    return 0;
  }
  double t17 = 1.0 / det;

  double inv[3][3];
  inv[0][0] = +(T[1][1] * T[2][2] - T[1][2] * T[2][1]) * t17;
  inv[0][1] = -(T[0][1] * T[2][2] - T[0][2] * T[2][1]) * t17;
  inv[0][2] = +(T[0][1] * T[1][2] - T[0][2] * T[1][1]) * t17;
  inv[1][0] = -(-T[2][0] * T[1][2] + T[1][0] * T[2][2]) * t17;
  inv[1][1] = (-t6 + T[0][0] * T[2][2]) * t17;
  inv[1][2] = -(-t10 + t14) * t17;
  inv[2][0] = (-T[2][0] * T[1][1] + T[1][0] * T[2][1]) * t17;
  inv[2][1] = -(-t4 + T[0][0] * T[2][1]) * t17;
  inv[2][2] = (-t8 + t12) * t17;
    
  double lambda[3];
  for (int i = 0; i < 3; i++) {
    lambda[i] = 0;
    for (int k = 0; k < 3; k++) {
      lambda[i] += inv[i][k] * xx0[k];
    }
  }
 
  double zero = -feenox_var_value(feenox.mesh.vars.eps);
  double one = 1+feenox_var_value(feenox.mesh.vars.eps);
  double lambda4 = 1 - lambda[0] - lambda[1] - lambda[2];
  
  return (lambda[0] > zero && lambda[0] < one &&
          lambda[1] > zero && lambda[1] < one &&
          lambda[2] > zero && lambda[2] < one &&
          lambda4 > zero && lambda4 < one);
}


double feenox_mesh_tet_volume(element_t *this) {

  if (this->volume == 0) {
    double a[3], b[3], c[3];
    
    feenox_mesh_subtract(this->node[0]->x, this->node[1]->x, a);
    feenox_mesh_subtract(this->node[0]->x, this->node[2]->x, b);
    feenox_mesh_subtract(this->node[0]->x, this->node[3]->x, c);
  
    this->volume = 1.0/(1.0*2.0*3.0) * fabs(feenox_mesh_cross_dot(c, a, b));
  }  
  
  return this->volume;

// AFEM.Ch09.pdf
// 6V = J = x 21 (y 23 z 34 − y34 z 23 ) + x32 (y34 z 12 − y12 z34 ) + x 43 (y12 z23 − y23 z 12),
}

double feenox_mesh_tet_area(element_t *this) {

  if (this->area == 0) {
    element_t triangle;
    triangle.type = &feenox.mesh.element_types[ELEMENT_TYPE_TRIANGLE3];
    triangle.node = NULL;
    feenox_check_alloc(triangle.node = calloc(3, sizeof(node_t *)));
    
    // first triangle: 0 1 2
    triangle.node[0] = this->node[0];
    triangle.node[1] = this->node[1];
    triangle.node[2] = this->node[2];
    triangle.volume = 0;
    this->area += triangle.type->volume(&triangle);
    
    // first triangle: 0 1 3
    triangle.node[0] = this->node[0];
    triangle.node[1] = this->node[1];
    triangle.node[2] = this->node[3];
    triangle.volume = 0;
    this->area += triangle.type->volume(&triangle);

    // first triangle: 0 2 3
    triangle.node[0] = this->node[0];
    triangle.node[1] = this->node[2];
    triangle.node[2] = this->node[3];
    triangle.volume = 0;
    this->area += triangle.type->volume(&triangle);

    // first triangle: 1 2 3
    triangle.node[0] = this->node[1];
    triangle.node[1] = this->node[2];
    triangle.node[2] = this->node[3];
    triangle.volume = 0;
    this->area += triangle.type->volume(&triangle);
    
    feenox_free(triangle.node);
  }  
  
  return this->area; 
}

