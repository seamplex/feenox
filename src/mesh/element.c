/*------------ -------------- -------- --- ----- ---   --       -            -
 *  FeenoX's mesh-related element routines
 *
 *  Copyright (C) 2015--2021 jeremy theler
 *
 *  This file is part of FeenoX.
 *
 *  FeenoX is free software: you can redistribute it and/or modify
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
 *  along with FeenoX.  If not, see <http://www.gnu.org/licenses/>.
 *------------------- ------------  ----    --------  --     -       -         -
 */
#include "../feenox.h"
#include "element.h"

/*

Line:                   Line3:           Line4:    
                                                
0----------1 --> u      0-----2----1     0----2----3----1

Triangle:               Triangle6:          Triangle9/10:          Triangle12/15:

v                                                              
^                                                                   2 
|                                                                   | \ 
2                       2                    2                      9   8
|`\                     |`\                  | \                    |     \ 
|  `\                   |  `\                7   6                 10 (14)  7
|    `\                 5    `4              |     \                |         \ 
|      `\               |      `\            8  (9)  5             11 (12) (13) 6
|        `\             |        `\          |         \            |             \
0----------1 --> u      0-----3----1         0---3---4---1          0---3---4---5---1

Quadrangle:            Quadrangle8:            Quadrangle9:

      v
      ^
      |
3-----------2          3-----6-----2           3-----6-----2 
|     |     |          |           |           |           | 
|     |     |          |           |           |           | 
|     +---- | --> u    7           5           7     8     5 
|           |          |           |           |           | 
|           |          |           |           |           | 
0-----------1          0-----4-----1           0-----4-----1 

Tetrahedron:                          Tetrahedron10:

                   v
                 .
               ,/
              /
           2                                     2                              
         ,/|`\                                 ,/|`\                          
       ,/  |  `\                             ,/  |  `\       
     ,/    '.   `\                         ,6    '.   `5     
   ,/       |     `\                     ,/       8     `\   
 ,/         |       `\                 ,/         |       `\ 
0-----------'.--------1 --> u         0--------4--'.--------1
 `\.         |      ,/                 `\.         |      ,/ 
    `\.      |    ,/                      `\.      |    ,9   
       `\.   '. ,/                           `7.   '. ,/     
          `\. |/                                `\. |/       
             `3                                    `3        
                `\.
                   ` w

Hexahedron:             Hexahedron20:          Hexahedron27:

       v
3----------2            3----13----2           3----13----2     
|\     ^   |\           |\         |\          |\         |\    
| \    |   | \          | 15       | 14        |15    24  | 14  
|  \   |   |  \         9  \       11 \        9  \ 20    11 \  
|   7------+---6        |   7----19+---6       |   7----19+---6 
|   |  +-- |-- | -> u   |   |      |   |       |22 |  26  | 23| 
0---+---\--1   |        0---+-8----1   |       0---+-8----1   | 
 \  |    \  \  |         \  17      \  18       \ 17    25 \  18
  \ |     \  \ |         10 |        12|        10 |  21    12| 
   \|      w  \|           \|         \|          \|         \| 
    4----------5            4----16----5           4----16----5 

Prism:                      Prism15:               Prism18:

           w
           ^
           |
           3                       3                      3        
         ,/|`\                   ,/|`\                  ,/|`\      
       ,/  |  `\               12  |  13              12  |  13    
     ,/    |    `\           ,/    |    `\          ,/    |    `\  
    4------+------5         4------14-----5        4------14-----5 
    |      |      |         |      8      |        |      8      | 
    |    ,/|`\    |         |      |      |        |    ,/|`\    | 
    |  ,/  |  `\  |         |      |      |        |  15  |  16  | 
    |,/    |    `\|         |      |      |        |,/    |    `\| 
   ,|      |      |\        10     |      11       10-----17-----11
 ,/ |      0      | `\      |      0      |        |      0      | 
u   |    ,/ `\    |    v    |    ,/ `\    |        |    ,/ `\    | 
    |  ,/     `\  |         |  ,6     `7  |        |  ,6     `7  | 
    |,/         `\|         |,/         `\|        |,/         `\| 
    1-------------2         1------9------2        1------9------2 

Pyramid:                     Pyramid13:                   Pyramid14:

               4                            4                            4
             ,/|\                         ,/|\                         ,/|\
           ,/ .'|\                      ,/ .'|\                      ,/ .'|\
         ,/   | | \                   ,/   | | \                   ,/   | | \
       ,/    .' | `.                ,/    .' | `.                ,/    .' | `.
     ,/      |  '.  \             ,7      |  12  \             ,7      |  12  \
   ,/       .' w |   \          ,/       .'   |   \          ,/       .'   |   \
 ,/         |  ^ |    \       ,/         9    |    11      ,/         9    |    11
0----------.'--|-3    `.     0--------6-.'----3    `.     0--------6-.'----3    `.
 `\        |   |  `\    \      `\        |      `\    \     `\        |      `\    \
   `\     .'   +----`\ - \ -> v  `5     .'        10   \      `5     .' 13     10   \
     `\   |    `\     `\  \        `\   |           `\  \       `\   |           `\  \ 
       `\.'      `\     `\`          `\.'             `\`         `\.'             `\` 
          1----------------2            1--------8-------2           1--------8-------2
                    `\
                       u
*/

int feenox_mesh_element_types_init(void) {

  if (feenox.mesh.element_types != NULL) {
    return FEENOX_OK;
  }
  
  feenox_check_alloc(feenox.mesh.element_types = calloc(NUMBER_ELEMENT_TYPE, sizeof(element_type_t)));
  element_type_t *element_type = NULL;

  // undefined  ----------------------------------------------------------------
  element_type = &feenox.mesh.element_types[ELEMENT_TYPE_UNDEFINED];
  feenox_check_alloc((element_type->name = strdup("undefined")));
  element_type->id = ELEMENT_TYPE_UNDEFINED;
  element_type->dim = 0;
  element_type->nodes = 0;
  element_type->faces = 0;
  element_type->nodes_per_face = 0;
  element_type->h = NULL;
  element_type->dhdr = NULL;
  element_type->point_inside = NULL;
  element_type->volume = NULL;
  
  // line ----------------------------------------------------------------------
  feenox_call(feenox_mesh_line2_init());
  feenox_call(feenox_mesh_line3_init());

  // triangles -----------------------------------------------------------------
  feenox_call(feenox_mesh_triang3_init());
  feenox_call(feenox_mesh_triang6_init());
  
  // quadrangles ----------------------------------------------------------------
  feenox_call(feenox_mesh_quad4_init());
  feenox_call(feenox_mesh_quad8_init());
  feenox_call(feenox_mesh_quad9_init());
  
  // tetrahedra  ---------------------------------------------------------------
  feenox_call(feenox_mesh_tet4_init());
  feenox_call(feenox_mesh_tet10_init());
  
  // hexahedra ---------------------------------------------------------------- 
  feenox_call(feenox_mesh_hexa8_init());
  feenox_call(feenox_mesh_hexa20_init());
  feenox_call(feenox_mesh_hexa27_init());
    
  // prism ---------------------------------------------------------------------
  feenox_call(feenox_mesh_prism6_init());
  feenox_call(feenox_mesh_prism15_init());

  // not supported  
  element_type = &feenox.mesh.element_types[ELEMENT_TYPE_PYRAMID5];
  element_type->dim = 3;
  feenox_check_alloc(element_type->name = strdup("pyramid5"));
  element_type->id = ELEMENT_TYPE_PYRAMID5;
  element_type->nodes = 0;

  feenox_check_alloc(feenox.mesh.element_types[13].name = strdup("prism18"));
  feenox_check_alloc(feenox.mesh.element_types[14].name = strdup("pyramid14"));

  
  // point
  feenox_call(feenox_mesh_one_node_point_init());
  
  // compute the barycenter of each element type in the r-space
  unsigned i, j, d;
  for (i = 0; i < NUMBER_ELEMENT_TYPE; i++) {
    if (feenox.mesh.element_types[i].node_coords != NULL) {
      feenox_check_alloc(feenox.mesh.element_types[i].barycenter_coords = calloc(feenox.mesh.element_types[i].dim, sizeof(double)));
      for (j = 0; j < feenox.mesh.element_types[i].nodes; j++) {
        for (d = 0; d < feenox.mesh.element_types[i].dim; d++) {
          feenox.mesh.element_types[i].barycenter_coords[d] += feenox.mesh.element_types[i].node_coords[j][d];
        }
      }
      for (d = 0; d < feenox.mesh.element_types[i].dim; d++) {
        feenox.mesh.element_types[i].barycenter_coords[d] /= feenox.mesh.element_types[i].nodes;
      }
    }
  }
  
  return FEENOX_OK;
  
};

int feenox_mesh_alloc_gauss(gauss_t *gauss, element_type_t *element_type, int V) {

  int v;
  int dim = (element_type->dim != 0) ? element_type->dim : 1;

  gauss->V = V;

  feenox_check_alloc(gauss->w = calloc(gauss->V, sizeof(double)));
  feenox_check_alloc(gauss->r = calloc(gauss->V, sizeof(double *)));
  feenox_check_alloc(gauss->h = calloc(gauss->V, sizeof(double *)));
  feenox_check_alloc(gauss->dhdr = calloc(gauss->V, sizeof(double **)));
  for (v = 0; v < gauss->V; v++) {
    feenox_check_alloc(gauss->r[v] = calloc(dim, sizeof(double)));
    
    feenox_check_alloc(gauss->h[v] = calloc(element_type->nodes, sizeof(double)));
    feenox_check_alloc(gauss->dhdr[v] = gsl_matrix_calloc(element_type->nodes, dim));
  }
  
  return FEENOX_OK;
  
}

int feenox_mesh_init_shape_at_gauss(gauss_t *gauss, element_type_t *element_type) {

  for (unsigned int v = 0; v < gauss->V; v++) {
    for (unsigned int j = 0; j < element_type->nodes; j++) {
      gauss->h[v][j] = element_type->h(j, gauss->r[v]);
      for (unsigned int m = 0; m < element_type->dim; m++) {
        gsl_matrix_set(gauss->dhdr[v], j, m, element_type->dhdr(j, m, gauss->r[v]));
      }
      
    }
  }
  
  return FEENOX_OK;
}


// esta no rellena los nodos!
int feenox_mesh_create_element(element_t *e, size_t index, size_t tag, unsigned int type, physical_group_t *physical_group) {
 
  e->index = index;
  e->tag = tag;
  e->type = &(feenox.mesh.element_types[type]);
  feenox_check_alloc(e->node  = calloc(e->type->nodes, sizeof(node_t *)));
  e->physical_group = physical_group;
  
  return FEENOX_OK;
}

int feenox_mesh_add_element_to_list(element_ll_t **list, element_t *e) {
  
  element_ll_t *item = NULL;
  feenox_check_alloc(item = calloc(1, sizeof(element_ll_t)));
  item->element = e;
  LL_APPEND(*list, item);
  
  return FEENOX_OK;
  
}

int feenox_mesh_add_material_to_list(material_ll_t **list, material_t *material) {
  
  // solo agregamos el material si es que no esta en la lista
  material_ll_t *item = NULL;
  LL_FOREACH(*list, item) {
    if (item->material == material) {
      return FEENOX_OK;
    }
  }
  
  feenox_check_alloc(item = calloc(1, sizeof(material_ll_t)));
  item->material = material;
  LL_APPEND(*list, item);
  
  return FEENOX_OK;
  
}

int feenox_mesh_compute_element_barycenter(element_t *this, double barycenter[]) {
  
  barycenter[0] = barycenter[1] = barycenter[2] = 0;
  for (unsigned int j = 0; j < this->type->nodes; j++) {
    barycenter[0] += this->node[j]->x[0];
    barycenter[1] += this->node[j]->x[1];
    barycenter[2] += this->node[j]->x[2];
  }
  barycenter[0] /= this->type->nodes; 
  barycenter[1] /= this->type->nodes; 
  barycenter[2] /= this->type->nodes; 
  
  return FEENOX_OK;
}


int feenox_mesh_init_nodal_indexes(mesh_t *this, int dofs) {
  
  this->degrees_of_freedom = dofs;

  for (size_t j = 0; j < this->n_nodes; j++) {
    feenox_check_alloc(this->node[j].index_dof = malloc(this->degrees_of_freedom*sizeof(size_t)));
    for (unsigned int g = 0; g < this->degrees_of_freedom; g++) {
      this->node[j].index_dof[g] = this->degrees_of_freedom*j + g;
    }
  }  
  
  return FEENOX_OK;
}

int feenox_mesh_compute_normal_2d(element_t *e) {
  if (e->normal == NULL) {
    feenox_check_alloc(e->normal = calloc(3, sizeof(double)));
    double a[3], b[3];
    feenox_mesh_subtract(e->node[0]->x, e->node[1]->x, a);
    feenox_mesh_subtract(e->node[0]->x, e->node[2]->x, b);
    feenox_mesh_normalized_cross(a, b, e->normal);
  }
  
  return FEENOX_OK;
}


unsigned int feenox_mesh_compute_local_node_index(element_t *element, size_t global_index) {
  unsigned int local_index = -1;
  
  for (unsigned int j = 0; j < element->type->nodes; j++) {
    if (element->node[j]->tag == global_index+1) {
      local_index = j;
      break;
    }
  }
  
  return local_index;
}


void feenox_mesh_add_node_parent(node_relative_t **parents, int index) {
  node_relative_t *parent = calloc(1, sizeof(node_relative_t));
  parent->index = index;
  LL_APPEND(*parents, parent);
  return;
}

void feenox_mesh_compute_coords_from_parent(element_type_t *element_type, int j) {
  node_relative_t *parent;
  int m;
  int den = 0;
  
  LL_FOREACH(element_type->node_parents[j], parent) {
    den++;
    for (m = 0; m < element_type->dim; m++) {
      element_type->node_coords[j][m] += element_type->node_coords[parent->index][m];  
    }
  }
  
  for (m = 0; m < element_type->dim; m++) {
    if (den == 0) {
      fprintf(stderr, "le tomaron la leche al gato\n");
    }
    element_type->node_coords[j][m] /= den;
  }
  return;
}
