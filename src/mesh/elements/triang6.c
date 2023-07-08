/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's mesh-related second-order triangle element routines
 *
 *  Copyright (C) 2017--2023 jeremy theler
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

// -------------------------------------
// six-node triangle
// -------------------------------------

int feenox_mesh_triang6_init(void) {

  double r[2];
  element_type_t *element_type;
  int j;
  
  element_type = &feenox.mesh.element_types[ELEMENT_TYPE_TRIANGLE6];
  feenox_check_alloc(element_type->name = strdup("triang6"));
  element_type->id = ELEMENT_TYPE_TRIANGLE6;
  element_type->dim = 2;
  element_type->order = 2;
  element_type->nodes = 6;
  element_type->faces = 3;
  element_type->nodes_per_face = 3;
  element_type->h = feenox_mesh_triang6_h;
  element_type->dhdxi = feenox_mesh_triang6_dhdr;
  element_type->point_inside = feenox_mesh_point_in_triangle;
  element_type->volume = feenox_mesh_triang_volume;
  element_type->area = feenox_mesh_triang_area;
  element_type->size = feenox_mesh_triang_size;

  // from Gmsh’ doc
/*
Triangle6:    
    
2             
|`\
|  `\
5    `4       
|      `\
|        `\
0-----3----1  
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

  feenox_mesh_add_node_parent(&element_type->node_parents[3], 0);
  feenox_mesh_add_node_parent(&element_type->node_parents[3], 1);
  feenox_mesh_compute_coords_from_parent(element_type, 3);
 
  feenox_mesh_add_node_parent(&element_type->node_parents[4], 1);
  feenox_mesh_add_node_parent(&element_type->node_parents[4], 2);
  feenox_mesh_compute_coords_from_parent(element_type, 4);
  
  feenox_mesh_add_node_parent(&element_type->node_parents[5], 2);
  feenox_mesh_add_node_parent(&element_type->node_parents[5], 0);
  feenox_mesh_compute_coords_from_parent(element_type, 5); 
  
  
  // for doc
  element_type->name = "triang6";
  element_type->desc = "Six-node triangle";
  element_type->ascii_art = "\
η                    \n\
2                    \n\
|`\\                 \n\
|  `\\               \n\
5    `4              \n\
|      `\\           \n\
|        `\\         \n\
0-----3----1-> ξ";

  element_type->h_latex = feenox_mesh_triang6_h_latex;
  element_type->doc_n_edges = 6;
  feenox_check_alloc(element_type->doc_edges = calloc(element_type->doc_n_edges, sizeof(int[2])));
  element_type->doc_edges = malloc(element_type->doc_n_edges * sizeof(int[2]));
  element_type->doc_edges[0][0] = 0;
  element_type->doc_edges[0][1] = 3;
  
  element_type->doc_edges[1][0] = 3;
  element_type->doc_edges[1][1] = 1;
  
  element_type->doc_edges[2][0] = 1;
  element_type->doc_edges[2][1] = 4;
  
  element_type->doc_edges[3][0] = 4;
  element_type->doc_edges[3][1] = 2;
  
  element_type->doc_edges[4][0] = 2;
  element_type->doc_edges[4][1] = 5;
  
  element_type->doc_edges[5][0] = 5;
  element_type->doc_edges[5][1] = 0;
  

  element_type->doc_n_faces = 1;
  feenox_check_alloc(element_type->doc_faces = calloc(element_type->doc_n_faces, sizeof(int[8])));
  element_type->doc_faces[0][0] = 1;
  element_type->doc_faces[0][1] = 2;
  element_type->doc_faces[0][2] = 3;
  element_type->doc_faces[0][3] = 4;
  element_type->doc_faces[0][4] = 5;
  element_type->doc_faces[0][5] = 6;

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

  r[0] = +2.0/3.0;
  r[1] = -1.0/3.0;
  for (j = 0; j < 3; j++) {
    gsl_matrix_set(element_type->gauss[integration_full].extrap, 3, j, feenox_mesh_triang3_h(j, r));
  }  

  r[0] = +2.0/3.0;
  r[1] = +2.0/3.0;
  for (j = 0; j < 3; j++) {
    gsl_matrix_set(element_type->gauss[integration_full].extrap, 4, j, feenox_mesh_triang3_h(j, r));
  }  

  r[0] = -1.0/3.0;
  r[1] = +2.0/3.0;
  for (j = 0; j < 3; j++) {
    gsl_matrix_set(element_type->gauss[integration_full].extrap, 5, j, feenox_mesh_triang3_h(j, r));
  }  
  
  // the reduced one is a vector of ones
  for (j = 0; j < element_type->nodes; j++) {
    // reduced
    gsl_matrix_set(element_type->gauss[integration_reduced].extrap, j, 0, 1.0);
  }
  
  return FEENOX_OK;    
}

double feenox_mesh_triang6_h(int j, double *vec_xi) {
  double xi = vec_xi[0];
  double eta = vec_xi[1];

  switch (j) {
    case 0:
      return (1-xi-eta)*(2*(1-xi-eta)-1);
      break;
    case 1:
      return xi*(2*xi-1);
      break;
    case 2:
      return eta*(2*eta-1);
      break;
      
    case 3:
      return 4*(1-xi-eta)*xi;
      break;
    case 4:
      return 4*xi*eta;
      break;
    case 5:
      return 4*eta*(1-xi-eta);
      break;
  }

  return 0;
}

char *feenox_mesh_triang6_h_latex(int j) {
  switch (j) {
    case 0:
      return "(1-\\xi-\\eta)\\cdot \\left[ 2 \\cdot (1-\\xi-\\eta) - 1 \\right]";
      break;
    case 1:
      return "\\xi\\cdot(2\\xi-1)";
      break;
    case 2:
      return "\\eta\\cdot(2\\eta-1)";
      break;
      
    case 3:
      return "4\\cdot(1-\\xi-\\eta)\\cdot\\xi";
      break;
    case 4:
      return "4\\cdot\\xi\\cdot\\eta";
      break;
    case 5:
      return "4\\cdot\\eta\\cdot(1-\\xi-\\eta)";
      break;
  }

  return 0;

}

double feenox_mesh_triang6_dhdr(int j, int m, double *vec_r) {
  double r = vec_r[0];
  double s = vec_r[1];

  switch(j) {
    case 0:
      if (m == 0) {
        return 1-4*(1-r-s);
      } else {
        return 1-4*(1-r-s);
      }
      break;
    case 1:
      if (m == 0) {
        return 4*r-1;
      } else {
        return 0;
      }
      break;
    case 2:
      if (m == 0) {
        return 0;
      } else {
        return 4*s-1;
      }
      break;
      
    case 3:
      if (m == 0) {
        return 4*(1-r-s)-4*r;
      } else {
        return -4*r;
      }
      break;
    case 4:
      if (m == 0) {
        return 4*s;
      } else {
        return 4*r;
      }
      break;
    case 5:
      if (m == 0) {
        return -4*s;
      } else {
        return 4*(1-r-s)-4*s;
      }
      break;

  }

  return 0;


}
