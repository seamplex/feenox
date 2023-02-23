/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's mesh-related routines to read frd files from calculix
 *
 *  Copyright (C) 2018--2020 jeremy theler
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

/*
     _________________________________________________________________
    |                               |                                 |
    | №№      CalculiX type         |  №№         VTK type            |
    |_______________________________|_________________________________|
    |    |          |               |      |                          |
    |  1 | C3D8     |  8 node brick | = 12 | VTK_HEXAHEDRON           |
    |    | F3D8     |               |      |                          |
    |    | C3D8R    |               |      |                          |
    |    | C3D8I    |               |      |                          |
    |____|__________|_______________|______|__________________________|
    |    |          |               |      |                          |
    |  2 | C3D6     |  6 node wedge | = 13 | VTK_WEDGE                |
    |    | F3D6     |               |      |                          |
    |____|__________|_______________|______|__________________________|
    |    |          |               |      |                          |
    |  3 | C3D4     |  4 node tet   | = 10 | VTK_TETRA                |
    |    | F3D4     |               |      |                          |
    |____|__________|_______________|______|__________________________|
    |    |          |               |      |                          |
    |  4 | C3D20    | 20 node brick | = 25 | VTK_QUADRATIC_HEXAHEDRON |
    |    | C3D20R   |               |      |                          |
    |____|__________|_______________|______|__________________________|
    |    |          |               |      |                          |
    |  5 | C3D15    | 15 node wedge | ~ 13 | VTK_WEDGE                |
    |____|__________|_______________|______|__________________________|
    |    |          |               |      |                          |
    |  6 | C3D10    | 10 node tet   | = 24 | VTK_QUADRATIC_TETRA      |
    |    | C3D10T   |               |      |                          |
    |____|__________|_______________|______|__________________________|
    |    |          |               |      |                          |
    |  7 | S3       |  3 node shell | =  5 | VTK_TRIANGLE             |
    |    | M3D3     |               |      |                          |
    |    | CPS3     |               |      |                          |
    |    | CPE3     |               |      |                          |
    |    | CAX3     |               |      |                          |
    |____|__________|_______________|______|__________________________|
    |    |          |               |      |                          |
    |  8 | S6       |  6 node shell | = 22 | VTK_QUADRATIC_TRIANGLE   |
    |    | M3D6     |               |      |                          |
    |    | CPS6     |               |      |                          |
    |    | CPE6     |               |      |                          |
    |    | CAX6     |               |      |                          |
    |____|__________|_______________|______|__________________________|
    |    |          |               |      |                          |
    |  9 | S4       |  4 node shell | =  9 | VTK_QUAD                 |
    |    | S4R      |               |      |                          |
    |    | M3D4     |               |      |                          |
    |    | M3D4R    |               |      |                          |
    |    | CPS4     |               |      |                          |
    |    | CPS4R    |               |      |                          |
    |    | CPE4     |               |      |                          |
    |    | CPE4R    |               |      |                          |
    |    | CAX4     |               |      |                          |
    |    | CAX4R    |               |      |                          |
    |____|__________|_______________|______|__________________________|
    |    |          |               |      |                          |
    | 10 | S8       |  8 node shell | = 23 | VTK_QUADRATIC_QUAD       |
    |    | S8R      |               |      |                          |
    |    | M3D8     |               |      |                          |
    |    | M3D8R    |               |      |                          |
    |    | CPS8     |               |      |                          |
    |    | CPS8R    |               |      |                          |
    |    | CPE8     |               |      |                          |
    |    | CPE8R    |               |      |                          |
    |    | CAX8     |               |      |                          |
    |    | CAX8R    |               |      |                          |
    |____|__________|_______________|______|__________________________|
    |    |          |               |      |                          |
    | 11 | B21      |  2 node beam  | =  3 | VTK_LINE                 |
    |    | B31      |               |      |                          |
    |    | B31R     |               |      |                          |
    |    | T2D2     |               |      |                          |
    |    | T3D2     |               |      |                          |
    |    | GAPUNI   |               |      |                          |
    |    | DASHPOTA |               |      |                          |
    |    | SPRING2  |               |      |                          |
    |    | SPRINGA  |               |      |                          |
    |____|__________|_______________|______|__________________________|
    |    |          |               |      |                          |
    | 12 | B32      |  3 node beam  | = 21 | VTK_QUADRATIC_EDGE       |
    |    | B32R     |               |      |                          |
    |    | T3D3     |               |      |                          |
    |    | D        |               |      |                          |
    |____|__________|_______________|______|__________________________|
    |    |          |               |      |                          |
    | ?? | SPRING1  |  1 node       | =  1 | VTK_VERTEX               |
    |    | DCOUP3D  |               |      |                          |
    |    | MASS     |               |      |                          |
    |____|__________|_______________|______|__________________________|
 
    © Ihor Mirzov, August 2019
    Distributed under GNU General Public License v3.0

    Convert Calculix element type to VTK.
    Keep in mind that CalculiX expands shell elements.
    In vtk elements nodes are numbered starting from 0, not from 1.

    For frd see http://www.dhondt.de/cgx_2.15.pdf pages 117-123 (chapter 10)
    For vtk see https://vtk.org/wp-content/uploads/2015/04/file-formats.pdf pages 9-10
   
*/

int frdfromgmsh_types[] = {
 ELEMENT_TYPE_UNDEFINED,     // 0
 ELEMENT_TYPE_HEXAHEDRON8,   // 1
 ELEMENT_TYPE_PRISM6,        // 2
 ELEMENT_TYPE_TETRAHEDRON4,  // 3
 ELEMENT_TYPE_HEXAHEDRON20,  // 4
 ELEMENT_TYPE_UNDEFINED,     // 5
 ELEMENT_TYPE_TETRAHEDRON10, // 6
 ELEMENT_TYPE_TRIANGLE3,     // 7
 ELEMENT_TYPE_TRIANGLE6,     // 8
 ELEMENT_TYPE_QUADRANGLE4,   // 9
 ELEMENT_TYPE_QUADRANGLE8,   // 10
 ELEMENT_TYPE_LINE2,         // 11
 ELEMENT_TYPE_LINE3,         // 12
 ELEMENT_TYPE_POINT1         // 13
};

int feenox_mesh_read_frd(mesh_t *this) {

/*
  char buffer[BUFFER_SIZE];
  char tmp[BUFFER_SIZE];
  double offset[3];
  
  double scale_factor;
  double scalar;
  
  int i, j, j_gmsh, k;
  int tag, tag_max;
  int type;
  int tags[2];
  int node, node_index;
  int spatial_dimensions;
  int bulk_dimensions;
  int order;
  
  int numnod, ictype, numstp, format, ncomps, irtype, menu;
  int minusone, minustwo, minusthree, minusfour, minusfive;
  
  int sparse = 0; // assume the nodes are not sparse
  int *tag2index = NULL;
  
  if (mesh->file->pointer == NULL) {
    feenox_call(feenox_instruction_open_file(mesh->file));
  }
  scale_factor = feenox_evaluate_expression(mesh->scale_factor);
  offset[0] = feenox_evaluate_expression(mesh->offset_x);
  offset[1] = feenox_evaluate_expression(mesh->offset_y);
  offset[2] = feenox_evaluate_expression(mesh->offset_z);

  // empezamos suponiendo cero dimensiones y vamos viendo cual es el elemento
  // de mayor dimension que aparece -> esa es la de la malla
  bulk_dimensions = 0;
  spatial_dimensions = 0;
  order = 0;
  
  while (fgets(buffer, BUFFER_SIZE-1, mesh->file->pointer) != NULL) {

    if (strncmp("\n", buffer, 1) == 0) {
      ;
    
    // ------------------------------------------------------  
    } else if (strncmp("    1C", buffer, 6) == 0) {
      // case name at buffer+6
      ;
      
    // ------------------------------------------------------  
    } else if (strncmp("    1U", buffer, 6) == 0) {
      // user-provided field at buffer+6 and user-provided data afterwards
      ;
      
    // ------------------------------------------------------  
    } else if (strncmp("    2C", buffer, 6) == 0) {
 
      // la cantidad de nodos y el formato
      if (sscanf(buffer, "%s %d %d", tmp, &mesh->n_nodes, &format) != 3) {
        feenox_push_error_message("error parsing number of nodes '%s'", buffer);
        return WASORA_RUNTIME_ERROR;
      }
      if (mesh->n_nodes == 0) {
        feenox_push_error_message("no nodes found in mesh '%s'", mesh->file->path);
        return WASORA_RUNTIME_ERROR;
      }
      if (format > 1) {
        feenox_push_error_message("node format %d not supported'", format);
        return WASORA_RUNTIME_ERROR;
      }
      
      mesh->node = calloc(mesh->n_nodes, sizeof(node_t));

      for (i = 0; i < mesh->n_nodes; i++) {
        if (fscanf(mesh->file->pointer, "%d", &minusone) != 1) {
          feenox_push_error_message("error parsing nodes", buffer);
          return WASORA_RUNTIME_ERROR;
        }
        if (minusone != -1) {
          feenox_push_error_message("expected minus one as line starter", buffer);
          return WASORA_RUNTIME_ERROR;
        }

        if (fscanf(mesh->file->pointer, "%d", &tag) == 0) {
          return WASORA_RUNTIME_ERROR;
        }
        
        if (i+1 != tag) {
          sparse = 1;
        }
        
        if ((mesh->node[i].tag = tag) > tag_max) {
          tag_max = mesh->node[i].tag;
        }
        
        mesh->node[i].index_mesh = i;

        for (j = 0; j < 3; j++) {
          if (fscanf(mesh->file->pointer, "%lf", &mesh->node[i].x[j]) == 0) {
            return WASORA_RUNTIME_ERROR;
          }
          if (scale_factor != 0 || offset[j] != 0) {
            mesh->node[i].x[j] = scale_factor*mesh->node[i].x[j] - offset[j];
          }
        }
        
        if (spatial_dimensions < 1 && fabs(mesh->node[i].x[0]) > 1e-6) {
          spatial_dimensions = 1;
        }
        if (spatial_dimensions < 2 && fabs(mesh->node[i].x[1]) > 1e-6) {
          spatial_dimensions = 2;
        }
        if (spatial_dimensions < 3 && fabs(mesh->node[i].x[2]) > 1e-6) {
          spatial_dimensions = 3;
        }
      }
  
      // el -3
      if (fscanf(mesh->file->pointer, "%d", &minusthree) != 1) {
        feenox_push_error_message("error parsing nodes", buffer);
        return WASORA_RUNTIME_ERROR;
      }
      if (minusthree != -3) {
        feenox_push_error_message("expected minus three as line starter", buffer);
        return WASORA_RUNTIME_ERROR;
      }
      
      // terminamos de leer los nodos, si los nodos son sparse tenemos que hacer el tag2index
      if (sparse) {
        tag2index = malloc((tag_max+1) * sizeof(int));
        for (k = 0; k <= tag_max; k++) {
          tag2index[k] = -1;
        }
        for (i = 0; i < mesh->n_nodes; i++) {
          tag2index[mesh->node[i].tag] = i;
        }
      }


    // ------------------------------------------------------      
    } else if (strncmp("    3C", buffer, 6) == 0) {

      // la cantidad de elementos
      if (sscanf(buffer, "%s %d %d", tmp, &mesh->n_elements, &format) != 3) {
        feenox_push_error_message("error parsing number of elements '%s'", buffer);
        return WASORA_RUNTIME_ERROR;
      }
      if (mesh->n_elements == 0) {
        feenox_push_error_message("no elements found in mesh '%s'", mesh->file->path);
        return WASORA_RUNTIME_ERROR;
      }
      if (format > 1) {
        feenox_push_error_message("element format %d not supported'", format);
        return WASORA_RUNTIME_ERROR;
      }

      mesh->element = calloc(mesh->n_elements, sizeof(element_t));

      for (i = 0; i < mesh->n_elements; i++) {

        if (fscanf(mesh->file->pointer, "%d", &minusone) != 1) {
          feenox_push_error_message("error parsing nodes", buffer);
          return WASORA_RUNTIME_ERROR;
        }
        if (minusone != -1) {
          feenox_push_error_message("expected minus one as line starter", buffer);
          return WASORA_RUNTIME_ERROR;
        }
        
        if (fscanf(mesh->file->pointer, "%d", &tag) == 0) {
          return WASORA_RUNTIME_ERROR;
        }
        mesh->element[i].index = i;
        mesh->element[i].tag = tag;
    
        if (fscanf(mesh->file->pointer, "%d", &type) == 0) {
          return WASORA_RUNTIME_ERROR;
        }
        if (type > 13) {
          feenox_push_error_message("element type '%d' shold be less than 14", type);
          return WASORA_RUNTIME_ERROR;
        }
        mesh->element[i].type = &(feenox_mesh.element_type[frdfromgmsh_types[type]]);
        if (mesh->element[i].type->nodes == 0) {
          feenox_push_error_message("elements of type '%s' are not supported in this version :-(", mesh->element[i].type->name);
          return WASORA_RUNTIME_ERROR;
        }

        if (fscanf(mesh->file->pointer, "%d %d", &tags[0], &tags[1]) < 2) {
          return WASORA_RUNTIME_ERROR;
        }
*/
        // agregamos uno a la cantidad de elementos asociados a la entidad fisica
/*        
        if (mesh == feenox_mesh.main_mesh) {
          if (mesh->element[i].tag != NULL && mesh->element[i].tag[0] != 0) {
            HASH_FIND(hh_id, feenox_mesh.physical_entities_by_id, &mesh->element[i].tag[0], sizeof(int), physical_entity);
            if ((mesh->element[i].physical_entity = physical_entity) != NULL) {
              physical_entity->n_elements++;
              // ponemos la dimension de la entidad fisica
              if (mesh->element[i].type->dim > physical_entity->dimension) {
                physical_entity->dimension = mesh->element[i].type->dim;
              }
            }
          }
        }
*/      
/*
        // vemos la dimension del elemento -> la mayor es la de la malla
        if (mesh->element[i].type->dim > bulk_dimensions) {
          bulk_dimensions = mesh->element[i].type->dim;
        }
        
        // el orden
        if (mesh->element[i].type->order > order) {
          order = mesh->element[i].type->order;
        }
        
        // nos acordamos del elemento que tenga el mayor numero de nodos
        if (mesh->element[i].type->nodes > mesh->max_nodes_per_element) {
          mesh->max_nodes_per_element = mesh->element[i].type->nodes;
        }

        // y del que tenga mayor cantidad de vecinos
        if (mesh->element[i].type->faces > mesh->max_faces_per_element) {
          mesh->max_faces_per_element = mesh->element[i].type->faces;
        }
    
        // el -2
        if (fscanf(mesh->file->pointer, "%d", &minustwo) != 1) {
          feenox_push_error_message("error parsing elements", buffer);
          return WASORA_RUNTIME_ERROR;
        }
        if (minustwo != -2) {
          feenox_push_error_message("expected minus two as line starter", buffer);
          return WASORA_RUNTIME_ERROR;
        }
        
        mesh->element[i].node = calloc(mesh->element[i].type->nodes, sizeof(node_t *));
        for (j = 0; j < mesh->element[i].type->nodes; j++) {
          do {
            if (fscanf(mesh->file->pointer, "%d", &node) == 0) {
              return WASORA_RUNTIME_ERROR;
            }
            if (sparse == 0 && node > mesh->n_nodes) {
              feenox_push_error_message("node %d in element %d does not exist", node, tag);
              return WASORA_RUNTIME_ERROR;
            }
          } while (node == -2);
          
          // pelar ojo al orden!
          // el tet10 tiene un unico cambio
          // ojo que hexa20y wedge 2nd order tambien tienen cambios
          if (mesh->element[i].type->id == ELEMENT_TYPE_TETRAHEDRON10) {
            if (j == 8) {
              j_gmsh = 9;
            } else if (j == 9) {
              j_gmsh = 8;
            } else {
              j_gmsh = j;
            }
          } else {
            j_gmsh = j;
          }
          
          if ((node_index = (sparse==0)?node-1:tag2index[node]) < 0) {
            feenox_push_error_message("node %d in element %d does not exist", node, tag);
            return WASORA_RUNTIME_ERROR;
          }
          mesh->element[i].node[j_gmsh] = &mesh->node[node_index];
          mesh_add_element_to_list(&mesh->element[i].node[j_gmsh]->associated_elements, &mesh->element[i]);
            
*/          
/*          
          if (mesh->element[id].physical_entity != NULL && mesh->element[id].physical_entity->material != NULL) {
            mesh_add_material_to_list(&mesh->element[id].node[j]->materials_list, mesh->element[id].physical_entity->material);
          }
 */
/*
        }
      }

      // el -3
      if (fscanf(mesh->file->pointer, "%d", &minusthree) != 1) {
        feenox_push_error_message("error parsing nodes", buffer);
        return WASORA_RUNTIME_ERROR;
      }
      if (minusthree != -3) {
        feenox_push_error_message("expected minus three as line starter", buffer);
        return WASORA_RUNTIME_ERROR;
      }
     
    // ------------------------------------------------------      
    } else if (strncmp("  100C", buffer, 6) == 0) {
      
      node_data_t *node_data;
      function_t **function;
      
      // la cantidad de nodos
      if (sscanf(buffer+25, "%d", &numnod) != 1) {
        feenox_push_error_message("error parsing 100CL '%s'", buffer);
        return WASORA_RUNTIME_ERROR;
      }

      if (sscanf(buffer+38, "%d %d", &ictype, &numstp) != 2) {
        feenox_push_error_message("error parsing 100CL '%s'", buffer);
        return WASORA_RUNTIME_ERROR;
      }
      
      if (sscanf(buffer+74, "%d", &format) != 1) {
        feenox_push_error_message("error parsing 100CL '%s'", buffer);
        return WASORA_RUNTIME_ERROR;
      }
      
      
      if (numnod != mesh->n_nodes || ictype != 0 || numstp != 1 || format > 1) {
        continue;
      }
      
      if (fgets(buffer, BUFFER_SIZE-1, mesh->file->pointer) == NULL) {
        feenox_push_error_message("error parsing -4");
        return WASORA_RUNTIME_ERROR;
      }
      
      if (sscanf(buffer, "%d %s %d %d", &minusfour, tmp, &ncomps, &irtype) != 4) {
        feenox_push_error_message("error parsing -4 '%s'", buffer);
        return WASORA_RUNTIME_ERROR;
      }
      
      if (minusfour != -4) {
        feenox_push_error_message("error parsing -4 '%s'", buffer);
        return WASORA_RUNTIME_ERROR;
      }
      
      if (irtype != 1) {
        continue;
      }
      
      // esto es un array de apuntadores, arrancamos en null y si
      // el nombre en el frd coincide con alguna de las funciones
      // que nos pidieron, entonces lo hacemos apuntar ahi
      function = calloc(ncomps, sizeof(function_t *));
      
      for (i = 0; i < ncomps; i++) {
        function[i] = NULL;
        
        if (fgets(buffer, BUFFER_SIZE-1, mesh->file->pointer) == NULL) {
          feenox_push_error_message("error parsing -5");
          return WASORA_RUNTIME_ERROR;
        }
        
        if (sscanf(buffer, "%d %s %d", &minusfive, tmp, &menu) != 3) {
          feenox_push_error_message("error parsing -5 '%s'", buffer);
          return WASORA_RUNTIME_ERROR;
        }
        
        if (minusfive != -5) {
          feenox_push_error_message("error parsing -5 '%s'", buffer);
          return WASORA_RUNTIME_ERROR;
        }
        if (menu != 1) {
          feenox_push_error_message("menu should be 1 '%s'", buffer);
          return WASORA_RUNTIME_ERROR;
        }
        
        LL_FOREACH(mesh->node_datas, node_data) {
          if (strcmp(tmp, node_data->name_in_mesh) == 0) {
            function[i] = node_data->function;
            
            function[i]->type = type_pointwise_mesh_node;
            function[i]->mesh = mesh;
            function[i]->data_argument = mesh->nodes_argument;
            function[i]->data_size = numnod;
            function[i]->data_value = calloc(numnod, sizeof(double));
            
          }
        }
        
        
        // esto es algo inventado por el viejo de calculix, donde son indices que aparecen
        // a veces si y a veces no, el cuento es que si la funcion se llama ALL no hay
        // datos sino que viene de calcular el modulo del vector desplazamiento
        if (strcmp(tmp, "ALL") == 0) {
          ncomps--;  // esto funciona solo si ALL es la ultima funcion
        }
        
      }
        
      for (j = 0; j < numnod; j++) {
        if (fscanf(mesh->file->pointer, "%d %d", &minusone, &node) != 2) {
          feenox_push_error_message("error reading file");
          return WASORA_RUNTIME_ERROR;
        }
        if (minusone != -1) {
          feenox_push_error_message("expected -1 '%s'", buffer);
          return WASORA_RUNTIME_ERROR;
        }
        if (sparse == 0 && (node < 1 || node > mesh->n_nodes)) {
          feenox_push_error_message("invalid node number '%d'", node);
          return WASORA_RUNTIME_ERROR;
        }
          
        for (i = 0; i < ncomps; i++) {
          // no entran mas de 6 por vez, hay que leer un -2 si hay mas
          if (fscanf(mesh->file->pointer, "%lf", &scalar) == 0) {
            feenox_push_error_message("error reading file");
            return WASORA_RUNTIME_ERROR;
          }
          if (function[i] != NULL) {
            if ((node_index = (sparse==0)?node-1:tag2index[node]) < 0) {
              feenox_push_error_message("node %d in element %d does not exist", node, tag);
              return WASORA_RUNTIME_ERROR;
            }
            function[i]->data_value[node_index] = scalar;
          }
        }
      }
      
      // el -3
      if (fscanf(mesh->file->pointer, "%d", &minusthree) != 1) {
        feenox_push_error_message("error parsing -3");
        return WASORA_RUNTIME_ERROR;
      }
      if (minusthree != -3) {
        feenox_push_error_message("expected minus three as line starter", buffer);
        return WASORA_RUNTIME_ERROR;
      }
      
    // ------------------------------------------------------      
    }
  }

  fclose(mesh->file->pointer);
  mesh->file->pointer = NULL;

  if (tag2index != NULL) {
    feenox_free(tag2index);
  }  

  
  // verificamos que la malla tenga la dimension esperada
  if (mesh->bulk_dimensions == 0) {
    mesh->bulk_dimensions = bulk_dimensions;
  } else if (mesh->bulk_dimensions != bulk_dimensions) {
    feenox_push_error_message("mesh '%s' is expected to have %d dimensions but it has %d", mesh->file->path, mesh->bulk_dimensions, bulk_dimensions);
    return WASORA_RUNTIME_ERROR;
  }
  
  mesh->spatial_dimensions = spatial_dimensions;
  mesh->order = order;  
*/
  return FEENOX_OK;
}
