/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's mesh-related hexahedron element routines
 *
 *  Copyright (C) 2014--2017 C.P. Camusso.
 *  Copyright (C) 2017--2023 Jeremy Theler
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
// twenty-node hexahedron
// --------------------------------------------------------------
int feenox_mesh_hexa20_init(void) {
  
  double xi[3];
  unsigned int j, q;

  element_type_t *element_type = &feenox.mesh.element_types[ELEMENT_TYPE_HEXAHEDRON20];
  element_type->name = "hexa20";
  element_type->id = ELEMENT_TYPE_HEXAHEDRON20;
  element_type->dim = 3;
  element_type->order = 2;
  element_type->nodes = 20;
  element_type->faces = 6;
  element_type->nodes_per_face = 8;
  element_type->h = feenox_mesh_hexa20_h;
  element_type->dhdxi = feenox_mesh_hexa20_dhdr;
  element_type->point_inside = feenox_mesh_point_in_hexahedron;
  element_type->volume = feenox_mesh_hex_volume;
  element_type->size = feenox_mesh_hex_size;

  // from Gmsh’ doc
/*
  Hexahedron20:     

  3----13----2      
  |\         |\
  | 15       | 14   
  9  \       11 \
  |   7----19+---6  
  |   |      |   |  
  0---+-8----1   |  
   \  17      \  18 
   10 |        12|  
     \|         \|  
      4----16----5  
*/     
  element_type->node_coords = calloc(element_type->nodes, sizeof(double *));
  element_type->node_parents = calloc(element_type->nodes, sizeof(node_relative_t *));  
  for (j = 0; j < element_type->nodes; j++) {
    element_type->node_coords[j] = calloc(element_type->dim, sizeof(double));  
  }

  
  element_type->vertices++;  
  element_type->node_coords[0][0] = -1;
  element_type->node_coords[0][1] = -1;
  element_type->node_coords[0][2] = -1;

  element_type->vertices++;  
  element_type->node_coords[1][0] = +1;  
  element_type->node_coords[1][1] = -1;
  element_type->node_coords[1][2] = -1;
  
  element_type->vertices++;
  element_type->node_coords[2][0] = +1;  
  element_type->node_coords[2][1] = +1;
  element_type->node_coords[2][2] = -1;

  element_type->vertices++;
  element_type->node_coords[3][0] = -1;
  element_type->node_coords[3][1] = +1;
  element_type->node_coords[3][2] = -1;
 
  element_type->vertices++;
  element_type->node_coords[4][0] = -1;
  element_type->node_coords[4][1] = -1;
  element_type->node_coords[4][2] = +1;
  
  element_type->vertices++;
  element_type->node_coords[5][0] = +1;  
  element_type->node_coords[5][1] = -1;
  element_type->node_coords[5][2] = +1;
  
  element_type->vertices++;
  element_type->node_coords[6][0] = +1;  
  element_type->node_coords[6][1] = +1;
  element_type->node_coords[6][2] = +1;

  element_type->vertices++;
  element_type->node_coords[7][0] = -1;
  element_type->node_coords[7][1] = +1;
  element_type->node_coords[7][2] = +1;
  
  feenox_mesh_add_node_parent(&element_type->node_parents[8], 0);
  feenox_mesh_add_node_parent(&element_type->node_parents[8], 1);
  feenox_mesh_compute_coords_from_parent(element_type, 8);
  
  feenox_mesh_add_node_parent(&element_type->node_parents[9], 0);
  feenox_mesh_add_node_parent(&element_type->node_parents[9], 3);
  feenox_mesh_compute_coords_from_parent(element_type, 9); 
 
  feenox_mesh_add_node_parent(&element_type->node_parents[10], 0);
  feenox_mesh_add_node_parent(&element_type->node_parents[10], 4);
  feenox_mesh_compute_coords_from_parent(element_type, 10);   
 
  feenox_mesh_add_node_parent(&element_type->node_parents[11], 1);
  feenox_mesh_add_node_parent(&element_type->node_parents[11], 2);
  feenox_mesh_compute_coords_from_parent(element_type, 11);

  feenox_mesh_add_node_parent(&element_type->node_parents[12], 1);
  feenox_mesh_add_node_parent(&element_type->node_parents[12], 5);
  feenox_mesh_compute_coords_from_parent(element_type, 12);

  feenox_mesh_add_node_parent(&element_type->node_parents[13], 2);
  feenox_mesh_add_node_parent(&element_type->node_parents[13], 3);
  feenox_mesh_compute_coords_from_parent(element_type, 13);  

  feenox_mesh_add_node_parent(&element_type->node_parents[14], 2);
  feenox_mesh_add_node_parent(&element_type->node_parents[14], 6);
  feenox_mesh_compute_coords_from_parent(element_type, 14);   
 
  feenox_mesh_add_node_parent(&element_type->node_parents[15], 3);
  feenox_mesh_add_node_parent(&element_type->node_parents[15], 7);
  feenox_mesh_compute_coords_from_parent(element_type, 15);

  feenox_mesh_add_node_parent(&element_type->node_parents[16], 4);
  feenox_mesh_add_node_parent(&element_type->node_parents[16], 5);
  feenox_mesh_compute_coords_from_parent(element_type, 16);

  feenox_mesh_add_node_parent(&element_type->node_parents[17], 4);
  feenox_mesh_add_node_parent(&element_type->node_parents[17], 7);
  feenox_mesh_compute_coords_from_parent(element_type, 17);    

  feenox_mesh_add_node_parent(&element_type->node_parents[18], 5);
  feenox_mesh_add_node_parent(&element_type->node_parents[18], 6);
  feenox_mesh_compute_coords_from_parent(element_type, 18);

  feenox_mesh_add_node_parent(&element_type->node_parents[19], 6);
  feenox_mesh_add_node_parent(&element_type->node_parents[19], 7);
  feenox_mesh_compute_coords_from_parent(element_type, 19);    

  // full integration: 3x3x3
  feenox_mesh_gauss_init_hexa27(element_type, &element_type->gauss[integration_full]);
  element_type->gauss[integration_full].extrap = gsl_matrix_calloc(element_type->nodes, 27);

  for (j = 0; j < element_type->nodes; j++) {
    xi[0] = M_SQRT5/M_SQRT3 * element_type->node_coords[j][0];
    xi[1] = M_SQRT5/M_SQRT3 * element_type->node_coords[j][1];
    xi[2] = M_SQRT5/M_SQRT3 * element_type->node_coords[j][2];
    
    for (q = 0; q < 27; q++) {
      gsl_matrix_set(element_type->gauss[integration_full].extrap, j, q, feenox_mesh_hexa27_h(q, xi));
    }
  }
  
  // reduced integration: 2x2x2
  feenox_mesh_gauss_init_hexa8(element_type, &element_type->gauss[integration_reduced]);
  element_type->gauss[integration_reduced].extrap = gsl_matrix_calloc(element_type->nodes, 8);
  
  // the two extrapolation matrices
  for (j = 0; j < element_type->nodes; j++) {
    xi[0] = M_SQRT3 * element_type->node_coords[j][0];
    xi[1] = M_SQRT3 * element_type->node_coords[j][1];
    xi[2] = M_SQRT3 * element_type->node_coords[j][2];

    for (q = 0; q < 8; q++) {
      gsl_matrix_set(element_type->gauss[integration_reduced].extrap, j, q, feenox_mesh_hexa8_h(q, xi));
    }
  }

  return FEENOX_OK;
}
/*
Reference https://www.code-aster.org/V2/doc/v11/en/man_r/r3/r3.01.01.pdf

                                              Coordinates
Node here(gmsh)       Node reference        r        s        t
 0                       1                 -1       -1       -1
 1                       2                  1       -1       -1
 2                       3                  1        1       -1
 3                       4                 -1        1       -1
 4                       5                 -1       -1        1
 5                       6                  1       -1        1
 6                       7                  1        1        1
 7                       8                 -1        1        1
 8                       9                  0       -1       -1
11                      10                  1        0       -1
13                      11                  0        1       -1
 9                      12                 -1        0       -1
10                      13                 -1       -1        0
12                      14                  1       -1        0
14                      15                  1        1        0
15                      16                 -1        1        0
16                      17                  0       -1        1
18                      18                  1        0        1
19                      19                  0        1        1
17                      20                 -1        0        1
*/

double feenox_mesh_hexa20_h(int j, double *vec_xi) {
  double xi = vec_xi[0];
  double eta = vec_xi[1];
  double zeta = vec_xi[2];

  switch (j) {
    case 0:
      return (1-xi)*(1-eta)*(1-zeta)*(-2-xi-eta-zeta)/8.0;
      break;
    case 1:
      return (xi+1)*(1-eta)*(1-zeta)*(-2+xi-eta-zeta)/8.0;
      break;
    case 2:
      return (xi+1)*(eta+1)*(1-zeta)*(-2+xi+eta-zeta)/8.0;
      break;
    case 3:
      return (1-xi)*(eta+1)*(1-zeta)*(-2-xi+eta-zeta)/8.0;
      break;
    case 4:
      return (1-xi)*(1-eta)*(1+zeta)*(-2-xi-eta+zeta)/8.0;
      break;
    case 5:
      return (xi+1)*(1-eta)*(zeta+1)*(-2+xi-eta+zeta)/8.0;
      break;
    case 6:
      return (xi+1)*(1+eta)*(zeta+1)*(-2+xi+eta+zeta)/8.0;
      break;
    case 7:
      return (1-xi)*(eta+1)*(zeta+1)*(-2-xi+eta+zeta)/8.0;
      break;
    case 8:
      return (1-xi*xi)*(1-eta)*(1-zeta)/4.0;
      break;
    case 9:
      return (1-xi)*(1-eta*eta)*(1-zeta)/4.0;
      break;
    case 10:
      return (1-xi)*(1-eta)*(1-zeta*zeta)/4.0;
      break;
    case 11:
      return (xi+1)*(1-eta*eta)*(1-zeta)/4.0;
      break;
    case 12:
      return (xi+1)*(1-eta)*(1-zeta*zeta)/4.0;
      break;
    case 13:
      return (1-xi*xi)*(eta+1)*(1-zeta)/4.0;
      break;
    case 14:
      return (xi+1)*(eta+1)*(1-zeta*zeta)/4.0;
      break;
    case 15:
      return (1-xi)*(eta+1)*(1-zeta*zeta)/4.0;
      break;
    case 16:
      return (1-xi*xi)*(1-eta)*(zeta+1)/4.0;
      break;
    case 17:
      return (1-xi)*(1-eta*eta)*(zeta+1)/4.0;
      break;
    case 18:
      return (xi+1)*(1-eta*eta)*(zeta+1)/4.0;
      break;
    case 19:
      return (1-xi*xi)*(eta+1)*(zeta+1)/4.0;
      break;
  }

  return 0;

}

double feenox_mesh_hexa20_dhdr(int j, int d, double *vec_xi) {
  double xi = vec_xi[0];
  double eta = vec_xi[1];
  double zeta = vec_xi[2];

  switch (j) {
    case 0:
      switch(d) {
        case 0:
          return -0.125*(1-eta)*(1-zeta)*(-zeta-eta-xi-2)-0.125*(1-xi)*(1-eta)*(1-zeta);
        break;
        case 1:
          return -0.125*(1-xi)*(1-zeta)*(-zeta-eta-xi-2)-0.125*(1-xi)*(1-eta)*(1-zeta);
        break;
        case 2:
          return -0.125*(1-xi)*(1-eta)*(-zeta-eta-xi-2)-0.125*(1-xi)*(1-eta)*(1-zeta);
        break;
      }
    break;
    case 1:
      switch(d) {
        case 0:
          return  0.125*(1-eta)*(1-zeta)*(-zeta-eta+xi-2)+0.125*(xi+1)*(1-eta)*(1-zeta);
        break;
        case 1:
          return -0.125*(xi+1)*(1-zeta)*(-zeta-eta+xi-2)-0.125*(xi+1)*(1-eta)*(1-zeta);
        break;
        case 2:
          return -0.125*(xi+1)*(1-eta)*(-zeta-eta+xi-2)-0.125*(xi+1)*(1-eta)*(1-zeta);
        break;
      }
    break;
    case 2:
      switch(d) {
        case 0:
          return  0.125*(eta+1)*(1-zeta)*(-zeta+eta+xi-2)+0.125*(xi+1)*(eta+1)*(1-zeta);
        break;
        case 1:
          return  0.125*(xi+1)*(1-zeta)*(-zeta+eta+xi-2)+0.125*(xi+1)*(eta+1)*(1-zeta);
        break;
        case 2:
          return -0.125*(xi+1)*(1+eta)*(-zeta+eta+xi-2)-0.125*(xi+1)*(eta+1)*(1-zeta);
        break;
      }
    break;
    case 3:
      switch(d) {
        case 0:
          return -0.125*(eta+1)*(1-zeta)*(-zeta+eta-xi-2)-0.125*(1-xi)*(eta+1)*(1-zeta);
        break;
        case 1:
          return  0.125*(1-xi)*(1-zeta)*(-zeta+eta-xi-2)+0.125*(1-xi)*(eta+1)*(1-zeta);
        break;
        case 2:
          return -0.125*(1-xi)*(eta+1)*(-zeta+eta-xi-2)-0.125*(1-xi)*(eta+1)*(1-zeta);
        break;
      }
    break;
    case 4:
      switch(d) {
        case 0:
          return -0.125*(1-eta)*(zeta+1)*(zeta-eta-xi-2)-0.125*(1-xi)*(1-eta)*(zeta+1);
        break;
        case 1:
          return -0.125*(1-xi)*(zeta+1)*(zeta-eta-xi-2)-0.125*(1-xi)*(1-eta)*(zeta+1);
        break;
        case 2:
          return  0.125*(1-xi)*(1-eta)*(zeta-eta-xi-2)+0.125*(1-xi)*(1-eta)*(zeta+1);
        break;
      }
    break;
    case 5:
      switch(d) {
        case 0:
          return  0.125*(1-eta)*(zeta+1)*(zeta-eta+xi-2)+0.125*(xi+1)*(1-eta)*(zeta+1);
        break;
        case 1:
          return -0.125*(xi+1)*(zeta+1)*(zeta-eta+xi-2)-0.125*(xi+1)*(1-eta)*(zeta+1);
        break;
        case 2:
          return  0.125*(xi+1)*(1-eta)*(zeta-eta+xi-2)+0.125*(xi+1)*(1-eta)*(zeta+1);
        break;
      }
    break;
    case 6:
      switch(d) {
        case 0:
          return 0.125*(eta+1)*(zeta+1)*(zeta+eta+xi-2)+0.125*(xi+1)*(eta+1)*(zeta+1);
        break;
        case 1:
          return 0.125*(xi+1)*(zeta+1)*(zeta+eta+xi-2)+0.125*(xi+1)*(eta+1)*(zeta+1);
        break;
        case 2:
          return 0.125*(xi+1)*(eta+1)*(zeta+eta+xi-2)+0.125*(xi+1)*(eta+1)*(zeta+1);
        break;
      }
    break;
    case 7:
      switch(d) {
        case 0:
          return -0.125*(eta+1)*(zeta+1)*(zeta+eta-xi-2)-0.125*(1-xi)*(eta+1)*(zeta+1);
        break;
        case 1:
          return  0.125*(1-xi)*(zeta+1)*(zeta+eta-xi-2)+0.125*(1-xi)*(eta+1)*(zeta+1);
        break;
        case 2:
          return  0.125*(1-xi)*(eta+1)*(zeta+eta-xi-2)+0.125*(1-xi)*(eta+1)*(zeta+1);
        break;
      }
    break;
    case 8:
      switch(d) {
        case 0:
          return -0.5*xi*(1-eta)*(1-zeta);
        break;
        case 1:
          return -(1-xi*xi)*(1-zeta)/4.0;
        break;
        case 2:
          return -(1-xi*xi)*(1-eta)/4.0;
        break;
      }
    break;
    case 9:
      switch(d) {
        case 0:
          return -(1-eta*eta)*(1-zeta)/4.0;
        break;
        case 1:
          return -(1-xi)*eta*(1-zeta)/2.0;
        break;
        case 2:
          return -(1-xi)*(1-eta*eta)/4.0;
        break;
      }
    break;
    case 10:
      switch(d) {
        case 0:
          return -(1-eta)*(1-zeta*zeta)/4.0;
        break;
        case 1:
          return -(1-xi)*(1-zeta*zeta)/4.0;
        break;
        case 2:
          return -(1-xi)*(1-eta)*zeta/2.0;
        break;
      }
    break;
    case 11:
      switch(d) {
        case 0:
          return (1-eta*eta)*(1-zeta)/4.0;
        break;
        case 1:
          return -(xi+1)*eta*(1-zeta)/2.0;
        break;
        case 2:
          return -(xi+1)*(1-eta*eta)/4.0;
        break;
      }
    break;
    case 12:
      switch(d) {
        case 0:
          return (1-eta)*(1-zeta*zeta)/4.0;
        break;
        case 1:
          return -(xi+1)*(1-zeta*zeta)/4.0;
        break;
        case 2:
          return -(xi+1)*(1-eta)*zeta/2.0;
        break;
      }
    break;
    case 13:
      switch(d) {
        case 0:
          return -xi*(eta+1)*(1-zeta)/2.0;
        break;
        case 1:
          return (1-xi*xi)*(1-zeta)/4.0;
        break;
        case 2:
          return -(1-xi*xi)*(eta+1)/4.0;
        break;
      }
    break;
    case 14:
      switch(d) {
        case 0:
          return (eta+1)*(1-zeta*zeta)/4.0;
        break;
        case 1:
          return (xi+1)*(1-zeta*zeta)/4.0;
        break;
        case 2:
          return -(xi+1)*(eta+1)*zeta/2.0;
        break;
      }
    break;
    case 15:
      switch(d) {
        case 0:
          return -(eta+1)*(1-zeta*zeta)/4.0;
        break;
        case 1:
          return (1-xi)*(1-zeta*zeta)/4.0;
        break;
        case 2:
          return -(1-xi)*(eta+1)*zeta/2.0;
        break;
      }
    break;
    case 16:
      switch(d) {
        case 0:
          return -xi*(1-eta)*(zeta+1)/2.0;
        break;
        case 1:
          return -(1-xi*xi)*(zeta+1)/4.0;
        break;
        case 2:
          return (1-xi*xi)*(1-eta)/4.0;
        break;
      }
    break;
    case 17:
      switch(d) {
        case 0:
          return -(1-eta*eta)*(zeta+1)/4.0;
        break;
        case 1:
          return -(1-xi)*eta*(zeta+1)/2.0;
        break;
        case 2:
          return (1-xi)*(1-eta*eta)/4.0;
        break;
      }
    break;
    case 18:
      switch(d) {
        case 0:
          return (1-eta*eta)*(zeta+1)/4.0;
        break;
        case 1:
          return -(xi+1)*eta*(zeta+1)/2.0;
        break;
        case 2:
          return (xi+1)*(1-eta*eta)/4.0;
        break;
      }
    break;
    case 19:
      switch(d) {
        case 0:
          return -xi*(eta+1)*(zeta+1)/2.0;
        break;
        case 1:
          return (1-xi*xi)*(zeta+1)/4.0;
        break;
        case 2:
          return (1-xi*xi)*(eta+1)/4.0;
        break;
      }
    break;
  }

  return 0;


}

