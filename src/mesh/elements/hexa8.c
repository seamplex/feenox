/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's mesh-related hexahedron element routines
 *
 *  Copyright (C) 2014--2023 Jeremy Theler
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
// eight-node hexahedron
// --------------------------------------------------------------

int feenox_mesh_hexa8_init(void) {
  
  double xi[3];
  unsigned int j, q;

  element_type_t * element_type = &feenox.mesh.element_types[ELEMENT_TYPE_HEXAHEDRON8];
  element_type->name = "hexa8";
  element_type->id = ELEMENT_TYPE_HEXAHEDRON8;
  element_type->dim = 3;
  element_type->order = 1;
  element_type->nodes = 8;
  element_type->faces = 6;
  element_type->nodes_per_face = 4;
  element_type->h = feenox_mesh_hexa8_h;
  element_type->dhdxi = feenox_mesh_hexa8_dhdr;
  element_type->point_inside = feenox_mesh_point_in_hexahedron;
  element_type->volume = feenox_mesh_hex_volume;
  element_type->size = feenox_mesh_hex_size;

  // from Gmsh’ doc
/*
Hexahedron:           

       v
3----------2          
|\     ^   |\
| \    |   | \
|  \   |   |  \
|   7------+---6      
|   |  +-- |-- | -> u 
0---+---\--1   |      
 \  |    \  \  |      
  \ |     \  \ |      
   \|      w  \|      
    4----------5      

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
  
  // ------------
  // gauss points and extrapolation matrices
  
  // full integration: 2x2x2
  feenox_mesh_gauss_init_hexa8(element_type, &element_type->gauss[integration_full]);
  element_type->gauss[integration_full].extrap = gsl_matrix_calloc(element_type->nodes, 8);

  for (j = 0; j < element_type->nodes; j++) {
    xi[0] = M_SQRT3 * element_type->node_coords[j][0];
    xi[1] = M_SQRT3 * element_type->node_coords[j][1];
    xi[2] = M_SQRT3 * element_type->node_coords[j][2];
    
    for (q = 0; q < 8; q++) {
      gsl_matrix_set(element_type->gauss[integration_full].extrap, j, q, feenox_mesh_hexa8_h(q, xi));
    }
  }
  
  // reduced integration: 1
  feenox_mesh_gauss_init_hexa1(element_type, &element_type->gauss[integration_reduced]);
  element_type->gauss[integration_reduced].extrap = gsl_matrix_calloc(element_type->nodes, 1);

  for (j = 0; j < element_type->nodes; j++) {
    gsl_matrix_set(element_type->gauss[integration_reduced].extrap, j, 0, 1.0);
  }

  return FEENOX_OK;
}


int feenox_mesh_gauss_init_hexa1(element_type_t *element_type, gauss_t *gauss) {

  // ---- one Gauss point  ----  
  feenox_call(feenox_mesh_alloc_gauss(gauss, element_type, 1));
  
  gauss->w[0] = 8 * 1.0;
  gauss->xi[0][0] = 0.0;
  gauss->xi[0][1] = 0.0;
  gauss->xi[0][2] = 0.0;
  
  feenox_call(feenox_mesh_init_shape_at_gauss(gauss, element_type));  
    
  return FEENOX_OK;
}


int feenox_mesh_gauss_init_hexa8(element_type_t *element_type, gauss_t *gauss) {

  // ---- eight Gauss points  ----  
  feenox_call(feenox_mesh_alloc_gauss(gauss, element_type, 8));

  gauss->w[0] = 8 * 1.0/8.0;
  gauss->xi[0][0] = -1.0/M_SQRT3;
  gauss->xi[0][1] = -1.0/M_SQRT3;
  gauss->xi[0][2] = -1.0/M_SQRT3;

  gauss->w[1] = 8 * 1.0/8.0;
  gauss->xi[1][0] = +1.0/M_SQRT3;
  gauss->xi[1][1] = -1.0/M_SQRT3;
  gauss->xi[1][2] = -1.0/M_SQRT3;

  gauss->w[2] = 8 * 1.0/8.0;
  gauss->xi[2][0] = +1.0/M_SQRT3;
  gauss->xi[2][1] = +1.0/M_SQRT3;
  gauss->xi[2][2] = -1.0/M_SQRT3;

  gauss->w[3] = 8 * 1.0/8.0;
  gauss->xi[3][0] = -1.0/M_SQRT3;
  gauss->xi[3][1] = +1.0/M_SQRT3;
  gauss->xi[3][2] = -1.0/M_SQRT3;

  gauss->w[4] = 8 * 1.0/8.0;
  gauss->xi[4][0] = -1.0/M_SQRT3;
  gauss->xi[4][1] = -1.0/M_SQRT3;
  gauss->xi[4][2] = +1.0/M_SQRT3;

  gauss->w[5] = 8 * 1.0/8.0;
  gauss->xi[5][0] = +1.0/M_SQRT3;
  gauss->xi[5][1] = -1.0/M_SQRT3;
  gauss->xi[5][2] = +1.0/M_SQRT3;

  gauss->w[6] = 8 * 1.0/8.0;
  gauss->xi[6][0] = +1.0/M_SQRT3;
  gauss->xi[6][1] = +1.0/M_SQRT3;
  gauss->xi[6][2] = +1.0/M_SQRT3;

  gauss->w[7] = 8 * 1.0/8.0;
  gauss->xi[7][0] = -1.0/M_SQRT3;
  gauss->xi[7][1] = +1.0/M_SQRT3;
  gauss->xi[7][2] = +1.0/M_SQRT3;
  
  feenox_call(feenox_mesh_init_shape_at_gauss(gauss, element_type));
  
  return FEENOX_OK;
}



int feenox_mesh_gauss_init_hexa27(element_type_t *element_type, gauss_t *gauss) {

  double const w1 = 5.0/9.0;
  double const w2 = 8.0/9.0;
  
  // ---- one Gauss point  ----  
  feenox_call(feenox_mesh_alloc_gauss(gauss, element_type, 27));

  
  //Reference https://www.code-aster.org/V2/doc/v11/en/man_r/r3/r3.01.01.pdf
  //          https://www.code-aster.org/V2/doc/default/en/man_r/r3/r3.01.01.pdf

  gauss->w[0] = w1 * w1 * w1;
  gauss->xi[0][0] = -M_SQRT3/M_SQRT5;
  gauss->xi[0][1] = -M_SQRT3/M_SQRT5;
  gauss->xi[0][2] = -M_SQRT3/M_SQRT5;

  gauss->w[1] = w1 * w1 * w1;
  gauss->xi[1][0] = +M_SQRT3/M_SQRT5;
  gauss->xi[1][1] = -M_SQRT3/M_SQRT5;
  gauss->xi[1][2] = -M_SQRT3/M_SQRT5;

  gauss->w[2] = w1 * w1 * w1;
  gauss->xi[2][0] = +M_SQRT3/M_SQRT5;
  gauss->xi[2][1] = +M_SQRT3/M_SQRT5;
  gauss->xi[2][2] = -M_SQRT3/M_SQRT5;

  gauss->w[3] = w1 * w1 * w1;
  gauss->xi[3][0] = -M_SQRT3/M_SQRT5;
  gauss->xi[3][1] = +M_SQRT3/M_SQRT5;
  gauss->xi[3][2] = -M_SQRT3/M_SQRT5;
  
  gauss->w[4] = w1 * w1 * w1;
  gauss->xi[4][0] = -M_SQRT3/M_SQRT5;
  gauss->xi[4][1] = -M_SQRT3/M_SQRT5;
  gauss->xi[4][2] = +M_SQRT3/M_SQRT5;

  gauss->w[5] = w1 * w1 * w1;
  gauss->xi[5][0] = +M_SQRT3/M_SQRT5;
  gauss->xi[5][1] = -M_SQRT3/M_SQRT5;
  gauss->xi[5][2] = +M_SQRT3/M_SQRT5;

  gauss->w[6] = w1 * w1 * w1;
  gauss->xi[6][0] = +M_SQRT3/M_SQRT5;
  gauss->xi[6][1] = +M_SQRT3/M_SQRT5;
  gauss->xi[6][2] = +M_SQRT3/M_SQRT5;
  
  gauss->w[7] = w1 * w1 * w1;
  gauss->xi[7][0] = -M_SQRT3/M_SQRT5;
  gauss->xi[7][1] = +M_SQRT3/M_SQRT5;
  gauss->xi[7][2] = +M_SQRT3/M_SQRT5;

  gauss->w[8] = w1 * w1 * w2;
  gauss->xi[8][0] =  0.0;
  gauss->xi[8][1] = -M_SQRT3/M_SQRT5;
  gauss->xi[8][2] = -M_SQRT3/M_SQRT5;
  
  gauss->w[9] = w1 * w1 * w2;
  gauss->xi[9][0] = -M_SQRT3/M_SQRT5;
  gauss->xi[9][1] =  0.0;
  gauss->xi[9][2] = -M_SQRT3/M_SQRT5;

  gauss->w[10] = w1 * w1 * w2;
  gauss->xi[10][0] = -M_SQRT3/M_SQRT5;
  gauss->xi[10][1] = -M_SQRT3/M_SQRT5;
  gauss->xi[10][2] =  0.0;

  gauss->w[11] = w1 * w1 * w2;
  gauss->xi[11][0] = +M_SQRT3/M_SQRT5;
  gauss->xi[11][1] =  0.0;
  gauss->xi[11][2] = -M_SQRT3/M_SQRT5;

  gauss->w[12] = w1 * w1 * w2;
  gauss->xi[12][0] = +M_SQRT3/M_SQRT5;
  gauss->xi[12][1] = -M_SQRT3/M_SQRT5;
  gauss->xi[12][2] =  0.0;
  
  gauss->w[13] = w1 * w1 * w2;
  gauss->xi[13][0] =  0.0;
  gauss->xi[13][1] = +M_SQRT3/M_SQRT5;
  gauss->xi[13][2] = -M_SQRT3/M_SQRT5;
  
  gauss->w[14] = w1 * w1 * w2;
  gauss->xi[14][0] = +M_SQRT3/M_SQRT5;
  gauss->xi[14][1] = +M_SQRT3/M_SQRT5;
  gauss->xi[14][2] =  0.0;
  
  gauss->w[15] = w1 * w1 * w2;
  gauss->xi[15][0] = -M_SQRT3/M_SQRT5;
  gauss->xi[15][1] = +M_SQRT3/M_SQRT5;
  gauss->xi[15][2] =  0.0;

  gauss->w[16] = w1 * w1 * w2;
  gauss->xi[16][0] =  0.0;
  gauss->xi[16][1] = -M_SQRT3/M_SQRT5;
  gauss->xi[16][2] = +M_SQRT3/M_SQRT5;

  gauss->w[17] = w1 * w1 * w2;
  gauss->xi[17][0] = -M_SQRT3/M_SQRT5;
  gauss->xi[17][1] =  0.0;
  gauss->xi[17][2] = +M_SQRT3/M_SQRT5;

  gauss->w[18] = w1 * w1 * w2;
  gauss->xi[18][0] = +M_SQRT3/M_SQRT5;
  gauss->xi[18][1] =  0.0;
  gauss->xi[18][2] = +M_SQRT3/M_SQRT5;
  
  gauss->w[19] = w1 * w1 * w2;
  gauss->xi[19][0] =  0.0;
  gauss->xi[19][1] = +M_SQRT3/M_SQRT5;
  gauss->xi[19][2] = +M_SQRT3/M_SQRT5;

  gauss->w[20] = w1 * w2 * w2;
  gauss->xi[20][0] =  0.0;
  gauss->xi[20][1] =  0.0;
  gauss->xi[20][2] = -M_SQRT3/M_SQRT5;

  gauss->w[21] = w1 * w2 * w2;
  gauss->xi[21][0] =  0.0;
  gauss->xi[21][1] = -M_SQRT3/M_SQRT5;
  gauss->xi[21][2] =  0.0;

  gauss->w[22] = w1 * w2 * w2;
  gauss->xi[22][0] = -M_SQRT3/M_SQRT5;
  gauss->xi[22][1] = 0.0;
  gauss->xi[22][2] = 0.0;
  
  gauss->w[23] = w1 * w2 * w2;
  gauss->xi[23][0] = +M_SQRT3/M_SQRT5;
  gauss->xi[23][1] =  0.0;
  gauss->xi[23][2] =  0.0;
  
  gauss->w[24] = w1 * w2 * w2;
  gauss->xi[24][0] =  0.0;
  gauss->xi[24][1] = +M_SQRT3/M_SQRT5;
  gauss->xi[24][2] =  0.0;

  gauss->w[25] = w1 * w2 * w2;
  gauss->xi[25][0] =  0.0;
  gauss->xi[25][1] =  0.0;
  gauss->xi[25][2] = +M_SQRT3/M_SQRT5;

  gauss->w[26] = w2 * w2 * w2;
  gauss->xi[26][0] =  0.0;
  gauss->xi[26][1] =  0.0;
  gauss->xi[26][2] =  0.0;


  feenox_call(feenox_mesh_init_shape_at_gauss(gauss, element_type));

  return FEENOX_OK;
}  
  


double feenox_mesh_hexa8_h(int j, double *vec_xi) {
  double xi = vec_xi[0];
  double eta = vec_xi[1];
  double zeta = vec_xi[2];

  switch (j) {
    case 0:
      return 1.0/8.0*(1-xi)*(1-eta)*(1-zeta);
      break;
    case 1:
      return 1.0/8.0*(1+xi)*(1-eta)*(1-zeta);
      break;
    case 2:
      return 1.0/8.0*(1+xi)*(1+eta)*(1-zeta);
      break;
    case 3:
      return 1.0/8.0*(1-xi)*(1+eta)*(1-zeta);
      break;
    case 4:
      return 1.0/8.0*(1-xi)*(1-eta)*(1+zeta);
      break;
    case 5:
      return 1.0/8.0*(1+xi)*(1-eta)*(1+zeta);
      break;
    case 6:
      return 1.0/8.0*(1+xi)*(1+eta)*(1+zeta);
      break;
    case 7:
      return 1.0/8.0*(1-xi)*(1+eta)*(1+zeta);
      break;
  }

  return 0;

}

double feenox_mesh_hexa8_dhdr(int j, int d, double *vec_xi) {
  double xi = vec_xi[0];
  double eta = vec_xi[1];
  double zeta = vec_xi[2];

  switch (j) {
    case 0:
      switch(d) {
        case 0:
          return -1.0/8.0*(1-eta)*(1-zeta);
        break;
        case 1:
          return -1.0/8.0*(1-xi)*(1-zeta);
        break;
        case 2:
          return -1.0/8.0*(1-xi)*(1-eta);
        break;
      }
    break;
    case 1:
      switch(d) {
        case 0:
          return +1.0/8.0*(1-eta)*(1-zeta);
        break;
        case 1:
          return -1.0/8.0*(1+xi)*(1-zeta);
        break;
        case 2:
          return -1.0/8.0*(1+xi)*(1-eta);
        break;
      }
    break;
    case 2:
      switch(d) {
        case 0:
          return +1.0/8.0*(1+eta)*(1-zeta);
        break;
        case 1:
          return +1.0/8.0*(1+xi)*(1-zeta);
        break;
        case 2:
          return -1.0/8.0*(1+xi)*(1+eta);
        break;
      }
    break;
    case 3:
      switch(d) {
        case 0:
          return -1.0/8.0*(1+eta)*(1-zeta);
        break;
        case 1:
          return +1.0/8.0*(1-xi)*(1-zeta);
        break;
        case 2:
          return -1.0/8.0*(1-xi)*(1+eta);
        break;
      }
    break;
    case 4:
      switch(d) {
        case 0:
          return -1.0/8.0*(1-eta)*(1+zeta);
        break;
        case 1:
          return -1.0/8.0*(1-xi)*(1+zeta);
        break;
        case 2:
          return +1.0/8.0*(1-xi)*(1-eta);
        break;
      }
    break;
    case 5:
      switch(d) {
        case 0:
          return +1.0/8.0*(1-eta)*(1+zeta);
        break;
        case 1:
          return -1.0/8.0*(1+xi)*(1+zeta);
        break;
        case 2:
          return +1.0/8.0*(1+xi)*(1-eta);
        break;
      }
    break;
    case 6:
      switch(d) {
        case 0:
          return +1.0/8.0*(1+eta)*(1+zeta);
        break;
        case 1:
          return +1.0/8.0*(1+xi)*(1+zeta);
        break;
        case 2:
          return +1.0/8.0*(1+xi)*(1+eta);
        break;
      }
    break;
    case 7:
      switch(d) {
        case 0:
          return -1.0/8.0*(1+eta)*(1+zeta);
        break;
        case 1:
          return +1.0/8.0*(1-xi)*(1+zeta);
        break;
        case 2:
          return +1.0/8.0*(1-xi)*(1+eta);
        break;
      }
    break;
  }

  return 0;


}


int feenox_mesh_point_in_hexahedron(element_t *element, const double *x) {

  // divide the hex8 into six tet4 and check if the point is inside any of them
  element_t tet;
  tet.type = &feenox.mesh.element_types[ELEMENT_TYPE_TETRAHEDRON4];
  feenox_check_alloc(tet.node = calloc(tet.type->nodes, sizeof(node_t *)));
  
  // first tet: 0 1 3 4
  tet.node[0] = element->node[0];
  tet.node[1] = element->node[1];
  tet.node[2] = element->node[3];
  tet.node[3] = element->node[4];
  if (tet.type->point_inside(&tet, x)) {
    feenox_free(tet.node);
    return 1;
  }

  // second tet: 5 4 7 1
  tet.node[0] = element->node[5];
  tet.node[1] = element->node[4];
  tet.node[2] = element->node[7];
  tet.node[3] = element->node[1];
  if (tet.type->point_inside(&tet, x)) {
    feenox_free(tet.node);
    return 1;
  }
  
  // third tet: 3 4 7 1
  tet.node[0] = element->node[3];
  tet.node[1] = element->node[4];
  tet.node[2] = element->node[7];
  tet.node[3] = element->node[1];
  if (tet.type->point_inside(&tet, x)) {
    feenox_free(tet.node);
    return 1;
  }

  // forth tet: 3 1 2 7
  tet.node[0] = element->node[3];
  tet.node[1] = element->node[1];
  tet.node[2] = element->node[2];
  tet.node[3] = element->node[7];
  if (tet.type->point_inside(&tet, x)) {
    feenox_free(tet.node);
    return 1;
  }

  // fifth tet: 5 7 6 1
  tet.node[0] = element->node[5];
  tet.node[1] = element->node[7];
  tet.node[2] = element->node[6];
  tet.node[3] = element->node[1];
  if (tet.type->point_inside(&tet, x)) {
    feenox_free(tet.node);
    return 1;
  }
  
  // sixth tet: 2 7 6 1
  tet.node[0] = element->node[2];
  tet.node[1] = element->node[7];
  tet.node[2] = element->node[6];
  tet.node[3] = element->node[1];
  if (tet.type->point_inside(&tet, x)) {
    feenox_free(tet.node);
    return 1;
  }
  
  feenox_free(tet.node);
  return 0;
  
  
}

double feenox_mesh_hex_volume(element_t *element) {


 if (element->volume == 0) {
 
   double a[3], b[3], c[3];
   double v1, v2, v3, v4, v5, v6;
   
   feenox_mesh_subtract(element->node[0]->x, element->node[1]->x, a);
   feenox_mesh_subtract(element->node[0]->x, element->node[3]->x, b);
   feenox_mesh_subtract(element->node[0]->x, element->node[4]->x, c);
   v1 = fabs(feenox_mesh_cross_dot(a, b, c));
  
   feenox_mesh_subtract(element->node[5]->x, element->node[4]->x, a);
   feenox_mesh_subtract(element->node[5]->x, element->node[7]->x, b);
   feenox_mesh_subtract(element->node[5]->x, element->node[1]->x, c);
   v2 = fabs(feenox_mesh_cross_dot(a, b, c));
  
   feenox_mesh_subtract(element->node[3]->x, element->node[4]->x, a);
   feenox_mesh_subtract(element->node[3]->x, element->node[7]->x, b);
   feenox_mesh_subtract(element->node[3]->x, element->node[1]->x, c);
   v3 = fabs(feenox_mesh_cross_dot(a, b, c));
  
   feenox_mesh_subtract(element->node[3]->x, element->node[1]->x, a);
   feenox_mesh_subtract(element->node[3]->x, element->node[2]->x, b);
   feenox_mesh_subtract(element->node[3]->x, element->node[7]->x, c);
   v4 = fabs(feenox_mesh_cross_dot(a, b, c));
  
   feenox_mesh_subtract(element->node[5]->x, element->node[7]->x, a);
   feenox_mesh_subtract(element->node[5]->x, element->node[6]->x, b);
   feenox_mesh_subtract(element->node[5]->x, element->node[1]->x, c);
   v5 = fabs(feenox_mesh_cross_dot(a, b, c));
  
   feenox_mesh_subtract(element->node[2]->x, element->node[7]->x, a);
   feenox_mesh_subtract(element->node[2]->x, element->node[6]->x, b);
   feenox_mesh_subtract(element->node[2]->x, element->node[1]->x, c);
   v6 = fabs(feenox_mesh_cross_dot(a, b, c));
  
   element->volume = 1.0/(1.0*2.0*3.0) * (v1+v2+v3+v4+v5+v6);
 }  
 
 return element->volume;

}

double feenox_mesh_hex_size(element_t *this) {

  if (this->size == 0) {
    this->size = 1.0/4.0 * (feenox_mesh_subtract_module(this->node[0]->x, this->node[5]->x) +
                            feenox_mesh_subtract_module(this->node[1]->x, this->node[6]->x) +
                            feenox_mesh_subtract_module(this->node[2]->x, this->node[7]->x) +
                            feenox_mesh_subtract_module(this->node[3]->x, this->node[8]->x));
  }  
  
  return this->size;
}