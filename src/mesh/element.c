/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's mesh-related element routines
 *
 *  Copyright (C) 2015--2021 jeremy theler
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

  int i, j, d;
  element_type_t *element_type;
  
  feenox.mesh.element_types = calloc(NUMBER_ELEMENT_TYPE, sizeof(element_type_t));

  // undefined  ----------------------------------------------------------------
  element_type = &feenox.mesh.element_types[ELEMENT_TYPE_UNDEFINED];
  element_type->name = strdup("undefined");
  element_type->id = ELEMENT_TYPE_UNDEFINED;
  element_type->dim = 0;
  element_type->nodes = 0;
  element_type->faces = 0;
  element_type->nodes_per_face = 0;
  element_type->h = NULL;
  element_type->dhdr = NULL;
  element_type->point_in_element = NULL;
  element_type->element_volume = NULL;
  
  // line ----------------------------------------------------------------------
  mesh_line2_init();
  mesh_line3_init();

  // triangles -----------------------------------------------------------------
  mesh_triang3_init();
  mesh_triang6_init();
  
  // quadrangles ----------------------------------------------------------------
  mesh_quad4_init();
  mesh_quad8_init();
  mesh_quad9_init();
  
  // tetrahedra  ---------------------------------------------------------------
  mesh_tet4_init();
  mesh_tet10_init();
  
  // hexahedra ---------------------------------------------------------------- 
  mesh_hexa8_init();
  mesh_hexa20_init();
  mesh_hexa27_init();
    
  // prism ---------------------------------------------------------------------
  mesh_prism6_init();
  mesh_prism15_init();

  // not supported  
  element_type = &feenox.mesh.element_types[ELEMENT_TYPE_PYRAMID5];
  element_type->dim = 3;
  element_type->name = strdup("pyramid5");
  element_type->id = ELEMENT_TYPE_PYRAMID5;
  element_type->nodes = 0;

  feenox.mesh.element_types[13].name = strdup("prism18");
  feenox.mesh.element_types[14].name = strdup("pyramid14");

  
  // point
  mesh_one_node_point_init();
  
  // compute the barycenter of each element type in the r-space
  for (i = 0; i < NUMBER_ELEMENT_TYPE; i++) {
    if (feenox.mesh.element_types[i].node_coords != NULL) {
      feenox.mesh.element_types[i].barycenter_coords = calloc(feenox.mesh.element_types[i].dim, sizeof(double));
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

int mesh_alloc_gauss(gauss_t *gauss, element_type_t *element_type, int V) {

  int v;
  int dim = (element_type->dim != 0) ? element_type->dim : 1;

  gauss->V = V;

  gauss->w = calloc(gauss->V, sizeof(double));
  gauss->r = calloc(gauss->V, sizeof(double *));
  gauss->h = calloc(gauss->V, sizeof(double *));
  gauss->dhdr = calloc(gauss->V, sizeof(double **));
  for (v = 0; v < gauss->V; v++) {
    gauss->r[v] = calloc(dim, sizeof(double));
    
    gauss->h[v] = calloc(element_type->nodes, sizeof(double));
    gauss->dhdr[v] = gsl_matrix_calloc(element_type->nodes, dim);
  }
  
  return FEENOX_OK;
  
}

int mesh_init_shape_at_gauss(gauss_t *gauss, element_type_t *element_type) {
  
  int v, j, m;
  
  for (v = 0; v < gauss->V; v++) {
    for (j = 0; j < element_type->nodes; j++) {

      gauss->h[v][j] = element_type->h(j, gauss->r[v]);
      for (m = 0; m < element_type->dim; m++) {
        gsl_matrix_set(gauss->dhdr[v], j, m, element_type->dhdr(j, m, gauss->r[v]));
      }
      
    }
  }
  
  return FEENOX_OK;
}


// esta no rellena los nodos!
int mesh_create_element(element_t *element, int index, int tag, int type, physical_group_t *physical_group) {
 
  element->index = index;
  element->tag = tag;
  element->type = &(feenox.mesh.element_types[type]);
  element->node  = calloc(element->type->nodes, sizeof(node_t *));
  element->physical_group = physical_group;
  
  return FEENOX_OK;
}

int mesh_add_element_to_list(element_list_item_t **list, element_t *element) {
  
  element_list_item_t *item = calloc(1, sizeof(element_list_item_t));
  item->element = element;
  LL_APPEND(*list, item);
  
  return FEENOX_OK;
  
}

int mesh_add_material_to_list(material_list_item_t **list, material_t *material) {
  
  material_list_item_t *item;
  
  // solo agregamos el material si es que no esta en la lista
  LL_FOREACH(*list, item) {
    if (item->material == material) {
      return FEENOX_OK;
    }
  }
  
  item = calloc(1, sizeof(material_list_item_t));
  item->material = material;
  LL_APPEND(*list, item);
  
  return FEENOX_OK;
  
}

int mesh_compute_element_barycenter(element_t *this, double barycenter[]) {
  
  int j;
  
  barycenter[0] = barycenter[1] = barycenter[2] = 0;
  for (j = 0; j < this->type->nodes; j++) {
    barycenter[0] += this->node[j]->x[0];
    barycenter[1] += this->node[j]->x[1];
    barycenter[2] += this->node[j]->x[2];
  }
  barycenter[0] /= this->type->nodes; 
  barycenter[1] /= this->type->nodes; 
  barycenter[2] /= this->type->nodes; 
  
  return FEENOX_OK;
}


int mesh_node_indexes(mesh_t *this, int dofs) {
  int j, g;
  
  this->degrees_of_freedom = dofs;

//  switch (mesh->ordering) {
//    case ordering_node_major:
      for (j = 0; j < this->n_nodes; j++) {
        this->node[j].index_dof = malloc(this->degrees_of_freedom*sizeof(int));
        for (g = 0; g < this->degrees_of_freedom; g++) {
          this->node[j].index_dof[g] = this->degrees_of_freedom*j + g;
        }
      }  
/*  
    break;    
    
    case ordering_dof_major:
      for (j = 0; j < mesh->n_nodes; j++) {
        mesh->node[j].index_dof = malloc(mesh->degrees_of_freedom*sizeof(int));
        for (g = 0; g < mesh->degrees_of_freedom; g++) {
          mesh->node[j].index_dof[g] = mesh->n_nodes*g + j;
        }
      }  
    break;
  }
*/
  
  return FEENOX_OK;
}


int mesh_compute_local_node_index(element_t *element, int global_index) {
  int j;
  int local_index = -1;
  
  for (j = 0; j < element->type->nodes; j++) {
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