/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's mesh-related prism15 element routines
 *
 *  Copyright (C) 2015 Ezequiel Manavela Chiapero
 *  Copyright (C) 2015,2017--2023 Jeremy Theler
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
// fifteen-node prism
// --------------------------------------------------------------

int feenox_mesh_prism15_init(void) {

  int q;

  element_type_t *element_type = &feenox.mesh.element_types[ELEMENT_TYPE_PRISM15];
  feenox_check_alloc(element_type->name = strdup("prism15"));
  element_type->id = ELEMENT_TYPE_PRISM15;
  element_type->dim = 3;
  element_type->order = 2;
  element_type->nodes = 15;
  element_type->faces = 5;
  element_type->nodes_per_face = 8;   // Ojo aca que en nodos por cara pusimos el maximo valor (8) ya que depende de la cara
  element_type->h = feenox_mesh_prism15_h;
  element_type->dhdxi = feenox_mesh_prism15_dhdr;
  element_type->point_inside = feenox_mesh_point_in_prism;
  element_type->volume = feenox_mesh_prism_volume;

    // coordenadas de los nodos
/*  
Prism:                    

           w
           ^
           |
           3                       3          
         ,/|`\                   ,/|`\        
       ,/  |  `\               12  |  13      
     ,/    |    `\           ,/    |    `\    
    4------+------5         4------14-----5   
    |      |      |         |      8      |   
    |    ,/|`\    |         |      |      |   
    |  ,/  |  `\  |         |      |      |   
    |,/    |    `\|         |      |      |   
   ,|      |      |\        10     |      11  
 ,/ |      0      | `\      |      0      |   
u   |    ,/ `\    |    v    |    ,/ `\    |   
    |  ,/     `\  |         |  ,6     `7  |   
    |,/         `\|         |,/         `\|   
    1-------------2         1------9------2   
 */
  
  element_type->node_coords = calloc(element_type->nodes, sizeof(double *));
  element_type->node_parents = calloc(element_type->nodes, sizeof(node_relative_t *));
  for (q = 0; q < element_type->nodes; q++) {
    element_type->node_coords[q] = calloc(element_type->dim, sizeof(double));  
  }
  
  element_type->vertices++;  
  element_type->node_coords[0][0] = 0;
  element_type->node_coords[0][1] = 0;
  element_type->node_coords[0][2] = -1;
  
  element_type->vertices++;  
  element_type->node_coords[1][0] = 1;
  element_type->node_coords[1][1] = 0;
  element_type->node_coords[1][2] = -1;
  
  element_type->vertices++;  
  element_type->node_coords[2][0] = 0;
  element_type->node_coords[2][1] = 1;
  element_type->node_coords[2][2] = -1;
  
  element_type->vertices++;  
  element_type->node_coords[3][0] = 0;
  element_type->node_coords[3][1] = 0;
  element_type->node_coords[3][2] = 1;
  
  element_type->vertices++;  
  element_type->node_coords[4][0] = 1;
  element_type->node_coords[4][1] = 0;
  element_type->node_coords[4][2] = 1;
  
  element_type->vertices++;  
  element_type->node_coords[5][0] = 0;
  element_type->node_coords[5][1] = 1;
  element_type->node_coords[5][2] = 1;

  feenox_mesh_prism_gauss6_init(element_type);

  return FEENOX_OK;
}



double feenox_mesh_prism15_h(int j, double *vec_xi) {
  double xi = vec_xi[0];
  double eta = vec_xi[1];
  double zeta = vec_xi[2];

  switch (j) {
    case 0:
      return ((zeta-1)*(1-eta-xi)*(zeta+2*eta+2*xi))/2;
      break;
    case 1:
      return (xi*(1-zeta)*(2*xi-2-zeta))/2;
      break;
    case 2:
      return (eta*(1-zeta)*(2*eta-2-zeta))/2;
      break;
    case 3:
      return (((-zeta)-1)*(1-eta-xi)*((-zeta)+2*eta+2*xi))/2;
      break;
    case 4:
      return (xi*(1+zeta)*(2*xi-2+zeta))/2;
      break;
    case 5:
      return (eta*(1+zeta)*(2*eta-2+zeta))/2;
      break;
    case 6:
      return 2*xi*(1-eta-xi)*(1-zeta);
      break;
    case 7:
      return 2*eta*(1-eta-xi)*(1-zeta);
      break;
    case 8:
      return (1-eta-xi)*(1-zeta*zeta);
      break;
    case 9:
      return 2*eta*xi*(1-zeta);
      break;
    case 10:
      return xi*(1-zeta*zeta);
      break;
    case 11:
      return eta*(1-zeta*zeta);
      break;
    case 12:
      return 2*xi*(1-eta-xi)*(1+zeta);
      break;
    case 13:
      return 2*eta*(1-eta-xi)*(1+zeta);
      break;
    case 14:
      return 2*eta*xi*(1+zeta);
      break;
  }

  return 0;

}

double feenox_mesh_prism15_dhdr(int j, int m, double *vec_r) {
  double r = vec_r[0];
  double s = vec_r[1];
  double t = vec_r[2];

  switch (j) {
    case 0:
      switch(m) {
        case 0:
          return ((-s)-r+1)*(t-1)-((t-1)*(t+2*s+2*r))/2;
        break;
        case 1:
          return ((-s)-r+1)*(t-1)-((t-1)*(t+2*s+2*r))/2;
        break;
        case 2:
          return (((-s)-r+1)*(t+2*s+2*r))/2+(((-s)-r+1)*(t-1))/2;
        break;
      }
    break;
    case 1:
      switch(m) {
        case 0:
          return ((1-t)*((-t)+2*r-2))/2+r*(1-t);
        break;
        case 1:
          return 0;
        break;
        case 2:
          return (-(r*((-t)+2*r-2))/2)-(r*(1-t))/2;
        break;
      }
    break;
    case 2:
      switch(m) {
        case 0:
          return 0;
        break;
        case 1:
          return ((1-t)*((-t)+2*s-2))/2+s*(1-t);
        break;
        case 2:
          return (-(s*((-t)+2*s-2))/2)-(s*(1-t))/2;
        break;
      }
    break;
    case 3:
      switch(m) {
        case 0:
          return ((-s)-r+1)*((-t)-1)-(((-t)-1)*((-t)+2*s+2*r))/2;
        break;
        case 1:
          return ((-s)-r+1)*((-t)-1)-(((-t)-1)*((-t)+2*s+2*r))/2;
        break;
        case 2:
          return (-(((-s)-r+1)*((-t)+2*s+2*r))/2)-(((-s)-r+1)*((-t)-1))/2;
        break;
      }
    break;
    case 4:
      switch(m) {
        case 0:
          return ((t+1)*(t+2*r-2))/2+r*(t+1);
        break;
        case 1:
          return 0;
        break;
        case 2:
          return (r*(t+2*r-2))/2+(r*(t+1))/2;
        break;
      }
    break;
    case 5:
      switch(m) {
        case 0:
          return 0;
        break;
        case 1:
          return ((t+1)*(t+2*s-2))/2+s*(t+1);
        break;
        case 2:
          return (s*(t+2*s-2))/2+(s*(t+1))/2;
        break;
      }
    break;
    case 6:
      switch(m) {
        case 0:
          return 2*((-s)-r+1)*(1-t)-2*r*(1-t);
        break;
        case 1:
          return -2*r*(1-t);
        break;
        case 2:
          return -2*r*((-s)-r+1);
        break;
      }
    break;
    case 7:
      switch(m) {
        case 0:
          return -2*s*(1-t);
        break;
        case 1:
          return 2*((-s)-r+1)*(1-t)-2*s*(1-t);
        break;
        case 2:
          return -2*((-s)-r+1)*s;
        break;
      }
    break;
    case 8:
      switch(m) {
        case 0:
          return t*t-1;
        break;
        case 1:
          return t*t-1;
        break;
        case 2:
          return -2*((-s)-r+1)*t;
        break;
      }
    break;
    case 9:
      switch(m) {
        case 0:
          return 2*s*(1-t);
        break;
        case 1:
          return 2*r*(1-t);
        break;
        case 2:
          return -2*r*s;
        break;
      }
    break;
    case 10:
      switch(m) {
        case 0:
          return 1-t*t;
        break;
        case 1:
          return 0;
        break;
        case 2:
          return -2*r*t;
        break;
      }
    break;
    case 11:
      switch(m) {
        case 0:
          return 0;
        break;
        case 1:
          return 1-t*t;
        break;
        case 2:
          return -2*s*t;
        break;
      }
    break;
    case 12:
      switch(m) {
        case 0:
          return 2*((-s)-r+1)*(t+1)-2*r*(t+1);
        break;
        case 1:
          return -2*r*(t+1);
        break;
        case 2:
          return 2*r*((-s)-r+1);
        break;
      }
    break;
    case 13:
      switch(m) {
        case 0:
          return -2*s*(t+1);
        break;
        case 1:
          return 2*((-s)-r+1)*(t+1)-2*s*(t+1);
        break;
        case 2:
          return 2*((-s)-r+1)*s;
        break;
      }
    break;
    case 14:
      switch(m) {
        case 0:
          return  2*s*(t+1);
        break;
        case 1:
          return  2*r*(t+1);
        break;
        case 2:
          return 2*r*s;
        break;
      }
    break;
  }

  return 0;

}
