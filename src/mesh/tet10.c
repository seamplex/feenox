/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's mesh-related second-order tetrahedron element routines
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
extern feenox_t feenox;

#include "element.h"

// -----------------------------------
// isoparametric ten-noded tetrahedron
// -----------------------------------

int feenox_mesh_tet10_init(void) {
  
  element_type_t *element_type;
  double r[3];
  double a, b, c, d;
  int j, v;
  
  element_type = &feenox.mesh.element_types[ELEMENT_TYPE_TETRAHEDRON10];
  feenox_check_alloc(element_type->name = strdup("tet10"));
  element_type->id = ELEMENT_TYPE_TETRAHEDRON10;
  element_type->dim = 3;
  element_type->order = 2;
  element_type->nodes = 10;
  element_type->faces = 4;
  element_type->nodes_per_face = 6;
  element_type->h = feenox_mesh_tet10_h;
  element_type->dhdr = feenox_mesh_tet10_dhdr;
  element_type->point_in_element = feenox_mesh_point_in_tetrahedron;
  element_type->element_volume = feenox_mesh_tet_vol;

/*
   
Tetrahedron10 (from Gmsh’ doc):

           2                  
         ,/|`\                
       ,/  |  `\       
     ,6    '.   `5     
   ,/       8     `\   
 ,/         |       `\ 
0--------4--'.--------1
 `\.         |      ,/ 
    `\.      |    ,9   
       `7.   '. ,/     
          `\. |/       
             `3        
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
  
  feenox_mesh_add_node_parent(&element_type->node_parents[4], 0);
  feenox_mesh_add_node_parent(&element_type->node_parents[4], 1);
  feenox_mesh_compute_coords_from_parent(element_type, 4);
  
  feenox_mesh_add_node_parent(&element_type->node_parents[5], 1);
  feenox_mesh_add_node_parent(&element_type->node_parents[5], 2);
  feenox_mesh_compute_coords_from_parent(element_type, 5); 
 
  feenox_mesh_add_node_parent(&element_type->node_parents[6], 2);
  feenox_mesh_add_node_parent(&element_type->node_parents[6], 0);
  feenox_mesh_compute_coords_from_parent(element_type, 6);   
 
  feenox_mesh_add_node_parent(&element_type->node_parents[7], 3);
  feenox_mesh_add_node_parent(&element_type->node_parents[7], 0);
  feenox_mesh_compute_coords_from_parent(element_type, 7);

  feenox_mesh_add_node_parent(&element_type->node_parents[8], 2);
  feenox_mesh_add_node_parent(&element_type->node_parents[8], 3);
  feenox_mesh_compute_coords_from_parent(element_type, 8);

  feenox_mesh_add_node_parent(&element_type->node_parents[9], 3);
  feenox_mesh_add_node_parent(&element_type->node_parents[9], 1);
  feenox_mesh_compute_coords_from_parent(element_type, 9);  
  
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

  r[0] = 0.5*(c+d);
  r[1] = 0.5*(c+c);
  r[2] = 0.5*(c+c);
  for (v = 0; v < 4; v++) {
    gsl_matrix_set(element_type->gauss[integration_full].extrap, 4, v, feenox_mesh_tet4_h(v, r));
  }

  r[0] = 0.5*(d+c);
  r[1] = 0.5*(c+d);
  r[2] = 0.5*(c+c);
  for (v = 0; v < 4; v++) {
    gsl_matrix_set(element_type->gauss[integration_full].extrap, 5, v, feenox_mesh_tet4_h(v, r));
  }

  r[0] = 0.5*(c+c);
  r[1] = 0.5*(c+d);
  r[2] = 0.5*(c+c);
  for (v = 0; v < 4; v++) {
    gsl_matrix_set(element_type->gauss[integration_full].extrap, 6, v, feenox_mesh_tet4_h(v, r));
  }

  r[0] = 0.5*(c+c);
  r[1] = 0.5*(c+c);
  r[2] = 0.5*(c+d);
  for (v = 0; v < 4; v++) {
    gsl_matrix_set(element_type->gauss[integration_full].extrap, 7, v, feenox_mesh_tet4_h(v, r));
  }

  r[0] = 0.5*(c+c);
  r[1] = 0.5*(d+c);
  r[2] = 0.5*(c+d);
  for (v = 0; v < 4; v++) {
    gsl_matrix_set(element_type->gauss[integration_full].extrap, 8, v, feenox_mesh_tet4_h(v, r));
  }

  r[0] = 0.5*(d+c);
  r[1] = 0.5*(c+c);
  r[2] = 0.5*(c+d);
  for (v = 0; v < 4; v++) {
    gsl_matrix_set(element_type->gauss[integration_full].extrap, 9, v, feenox_mesh_tet4_h(v, r));
  }
  
  // reduced
  for (j = 0; j < element_type->nodes; j++) {
    gsl_matrix_set(element_type->gauss[integration_reduced].extrap, j, 0, 1.0);
  }  

  return FEENOX_OK;
}

double feenox_mesh_tet10_h(int j, double *vec_r) {
  double r = vec_r[0];
  double s = vec_r[1];
  double t = vec_r[2];

  switch (j) {
    case 0:
      return (1-r-s-t)*(2*(1-r-s-t)-1);
      break;
    case 1:
      return r*(2*r-1);
      break;
    case 2:
      return s*(2*s-1);
      break;
    case 3:
      return t*(2*t-1);
      break;
      
    case 4:
      return 4*(1-r-s-t)*r;
      break;
    case 5:
      return 4*r*s;
      break;
    case 6:
      return 4*s*(1-r-s-t);
      break;
    case 7:
      return 4*(1-r-s-t)*t;
      break;
    case 8:
      return 4*s*t;
      break;
    case 9:
      return 4*r*t;
      break;
      
  }

  return 0;

}

double feenox_mesh_tet10_dhdr(int j, int m, double *vec_r) {
  double r = vec_r[0];
  double s = vec_r[1];
  double t = vec_r[2];
  
  switch (j) {
    case 0:
      switch(m) {
        case 0:
          return 1-4*(1-r-s-t);
        break;
        case 1:
          return 1-4*(1-r-s-t);
        break;
        case 2:
          return 1-4*(1-r-s-t);
        break;
      }
    break;
    case 1:
      switch(m) {
        case 0:
          return 4*r-1;
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
          return 4*s-1;
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
          return 4*t-1;
        break;
      }

    case 4:
      switch(m) {
        case 0:
          return -4*r+4*(1-r-s-t);
        break;
        case 1:
          return -4*r;
        break;
        case 2:
          return -4*r;
        break;
      }
    case 5:
      switch(m) {
        case 0:
          return 4*s;
        break;
        case 1:
          return 4*r;
        break;
        case 2:
          return 0;
        break;
      }
    case 6:
      switch(m) {
        case 0:
          return -4*s;
        break;
        case 1:
          return -4*s+4*(1-r-s-t);
        break;
        case 2:
          return -4*s;
        break;
      }
    case 7:
      switch(m) {
        case 0:
          return -4*t;
        break;
        case 1:
          return -4*t;
        break;
        case 2:
          return -4*t+4*(1-r-s-t);
        break;
      }
    case 8:
      switch(m) {
        case 0:
          return 0;
        break;
        case 1:
          return 4*t;
        break;
        case 2:
          return 4*s;
        break;
      }
    case 9:
      switch(m) {
        case 0:
          return 4*t;
        break;
        case 1:
          return 0;
        break;
        case 2:
          return 4*r;
        break;
      }

      break;
  }

  return 0;


}

