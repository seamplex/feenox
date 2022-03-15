/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's mesh-related vtk routines
 *
 *  Copyright (C) 2014--2021 jeremy theler
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


// conversion from gmsh to vtk element types
// source: https://github.com/Kitware/VTK/blob/master/Common/DataModel/vtkCellType.h
int vtkfromgmsh_types[NUMBER_ELEMENT_TYPE] = {
  0,    // ELEMENT_TYPE_UNDEFINED
  3,    // ELEMENT_TYPE_LINE
  5,    // ELEMENT_TYPE_TRIANGLE
  9,    // ELEMENT_TYPE_QUADRANGLE
 10,    // ELEMENT_TYPE_TETRAHEDRON
 12,    // ELEMENT_TYPE_HEXAHEDRON
 13,    // ELEMENT_TYPE_PRISM 
 14,    // ELEMENT_TYPE_PYRAMID 
  0,
 22,    // ELEMENT_TYPE_TRIANGLE6
 28,    // ELEMENT_TYPE_QUADRANGLE9
 24,    // ELEMENT_TYPE_TETRAHEDRON10
 29,    // ELEMENT_TYPE_HEXAHEDRON27
  0,
  0,
  1,    // ELEMENT_TYPE_POINT
 23,    // ELEMENT_TYPE_QUADRANGLE8
 25,    // ELEMENT_TYPE_HEXAHEDRON20
};

// conversion from gmsh to vtk node ordering in hexes
// (by reading files because by following the docs it did not work).
// index  0 1 2 3 4 5 6 7 8  9 10 11 12 13 14 15 16 17 18 19
// vtk    0 1 3 2 4 6 7 5 8 10 11  9 18 17 19 14 12 15 16 13
// gmsh   0 1 3 2 4 6 7 5 8  9 12 10 15 11 16 13 18 14 17 19
int hexa20fromgmsh[20] = { 
  0 , 1  , 2  ,  3 , 4  , 5  , 6  , 7  ,
  8 , 11 , 13 ,  9 , 16 , 18 , 19 , 17 ,
 10 , 12 , 14 , 15 } ;
int hexa27fromgmsh[27] = { 
  0 , 1  , 2  ,  3 , 4  , 5  , 6  , 7  ,
  8 , 11 , 13 ,  9 , 16 , 18 , 19 , 17 ,
 10 , 12 , 14 , 15 , 22 , 23 , 21 , 24 ,
 20 , 25 , 26} ;

int feenox_mesh_write_header_vtk(FILE *file) {
  fprintf(file, "# vtk DataFile Version 2.0\n");
  fprintf(file, "FeenoX VTK output\n");
  fprintf(file, "ASCII\n");

  return FEENOX_OK;
}


int feenox_mesh_write_mesh_vtk(mesh_t *mesh, FILE *file, int dummy) {
    
  fprintf(file, "DATASET UNSTRUCTURED_GRID\n");
  fprintf(file, "POINTS %ld double\n", mesh->n_nodes);
  size_t j = 0;
  for (j = 0; j < mesh->n_nodes; j++) { 
    if (mesh->node[j].tag != j+1) {
      feenox_push_error_message("VTK output needs sorted nodes");
      return FEENOX_ERROR;
    }
    fprintf(file, "%g %g %g\n", mesh->node[j].x[0], mesh->node[j].x[1], mesh->node[j].x[2]);
  }
  fprintf(file, "\n");

  size_t size = 0;
  size_t volumelements = 0;
  size_t i = 0;
  for (i = 0; i < mesh->n_elements; i++) {
    if (mesh->element[i].type->dim == mesh->dim_topo) {
      size += 1 + mesh->element[i].type->nodes;
      volumelements++;
    }
  }

  fprintf(file, "CELLS %ld %ld\n", volumelements, size);
  for (i = 0; i < mesh->n_elements; i++) {
    if (mesh->element[i].type->dim == mesh->dim_topo) {
      switch(mesh->element[i].type->id)
        {
        case ELEMENT_TYPE_HEXAHEDRON27: 
          fprintf(file, "%d ", 27);
          for(j = 0; j < 27 ; ++j) {
            fprintf(file, " %ld", (mesh->element[i].node[hexa27fromgmsh[j]]->tag)-1);
           }
          fprintf(file, "\n");
        break;
        case ELEMENT_TYPE_HEXAHEDRON20:
          fprintf(file, "%d ", 20);
          for(j = 0; j < 20 ; ++j) {
            fprintf(file, " %ld", (mesh->element[i].node[hexa20fromgmsh[j]]->tag)-1);
          }
          fprintf(file, "\n");
        break;
        default:
          fprintf(file, "%d ", mesh->element[i].type->nodes);
          // ojo! capaz que no funcione si no estan ordenados los indices
          for (j = 0; j < mesh->element[i].type->nodes; j++) {
            // el tet10 es diferente!
            if (vtkfromgmsh_types[mesh->element[i].type->id] == 24 && (j == 8 || j == 9)) {
              if (j == 8) {
                fprintf(file, " %ld", (mesh->element[i].node[9]->tag)-1);
              } else if (j == 9) {
                fprintf(file, " %ld", (mesh->element[i].node[8]->tag)-1);
              }
            } else {
              fprintf(file, " %ld", (mesh->element[i].node[j]->tag)-1);
            }
          }
          fprintf(file, "\n");
        break;
        }
      }
    }
  fprintf(file, "\n");
  
  fprintf(file, "CELL_TYPES %ld\n", volumelements);
  for (i = 0; i < mesh->n_elements; i++) {
    if (mesh->element[i].type->dim == mesh->dim_topo) {
      fprintf(file, "%d\n", vtkfromgmsh_types[mesh->element[i].type->id]);
    }
  }
  
  return FEENOX_OK;  
}

int feenox_mesh_write_data_vtk(mesh_write_t *this, mesh_write_dist_t *dist) {

  if (dist->field_location == field_location_cells) {
    if (this->mesh->n_cells == 0) {
      feenox_call(feenox_mesh_element2cell(this->mesh));
    }
    
    if (this->cell_init == 0) {
      fprintf(this->file->pointer, "CELL_DATA %ld\n", this->mesh->n_cells);
      this->cell_init = 1;
    }
  } else {
    if (this->point_init == 0) {
      fprintf(this->file->pointer, "POINT_DATA %ld\n", this->mesh->n_nodes);
      this->point_init = 1;
    }
  }
      
  

  switch (dist->size) {
    case 1:
      fprintf(this->file->pointer, "SCALARS");
      break;
    case 3:
      fprintf(this->file->pointer, "VECTORS");
      break;
    case 6:
      fprintf(this->file->pointer, "TENSORS");
      break;
    default:
      feenox_push_error_message("size %d not supported in VTK output");
      return FEENOX_ERROR;
  }
      
  fprintf(this->file->pointer, " %s double\n", dist->name);
  if (this->lookup_table_init == 0) {
    fprintf(this->file->pointer, "LOOKUP_TABLE default\n");
    this->lookup_table_init = 1;
  }
    
  // custom printf format
  char *format = NULL;
  if (dist->printf_format == NULL) {
    feenox_check_alloc(format = strdup(" %g"));
  } else {
    if (dist->printf_format[0] == '%') {
      feenox_check_minusone(asprintf(&format, " %s", dist->printf_format));
    } else {
      feenox_check_minusone(asprintf(&format, " %%%s", dist->printf_format));
    }  
  }

  if (dist->field_location == field_location_cells) {

    size_t i = 0;
    unsigned int g = 0;
    double value = 0;
    for (i = 0; i < this->mesh->n_cells; i++) {
      for (g = 0; g < dist->size; g++) {
        value = (dist->field[g]->type == function_type_pointwise_mesh_cell && dist->field[g]->mesh == this->mesh) ?
                  dist->field[g]->data_value[i] :
                  feenox_function_eval(dist->field[g], this->mesh->cell[i].x);
        fprintf(this->file->pointer, format, value);
      }
      fprintf(this->file->pointer, "\n");
    }
    
  } else {
    
    size_t j = 0;
    unsigned int g = 0;
    double value = 0;
    for (j = 0; j < this->mesh->n_nodes; j++) {
      for (g = 0; g < dist->size; g++) {
        value = (dist->field[g]->type == function_type_pointwise_mesh_node && dist->field[g]->mesh == this->mesh) ?
                  dist->field[g]->data_value[j] :
                  feenox_function_eval(dist->field[g], this->mesh->node[j].x);
        fprintf(this->file->pointer, format, value);
      }
      fprintf(this->file->pointer, "\n");
    }
  }  
 
  fflush(this->file->pointer);
  feenox_free(format);
  
  return FEENOX_OK;
  
}


int feenox_mesh_read_vtk(mesh_t *this) {

/*  
  char buffer[BUFFER_SIZE];
  char tmp[2*BUFFER_SIZE];
  char name[BUFFER_SIZE];
  double f[3];
  int *celldata;
  int version_maj, version_min;
  int numdata, check, celltype;
  int vector_function;
  int i, j, k, l;
  int j_gmsh;
  
  
  if (mesh->file->pointer == NULL) {
    feenox_call(feenox_instruction_open_file(mesh->file));
  }
  
  // header
  if (fscanf(mesh->file->pointer, "# vtk DataFile Version %d.%d", &version_maj, &version_min) != 2) {
    feenox_push_error_message("wrong VTK header");
    return FEENOX_ERROR;
  }

  // el \n
  fgets(buffer, BUFFER_SIZE-1, mesh->file->pointer);

  // el nombre del caso, lo tiramos
  fgets(buffer, BUFFER_SIZE-1, mesh->file->pointer);

  // el formato debe ser ASCII
  fgets(buffer, BUFFER_SIZE-1, mesh->file->pointer);
  if (strncmp(buffer, "ASCII", 5) != 0) {
    feenox_push_error_message("only ASCII VTK files are supported, not '%s'", buffer);
    return FEENOX_ERROR;
  }
 
  // dataset unstructured grid
  do {
    if (!feof(mesh->file->pointer)) {
      fgets(buffer, BUFFER_SIZE-1, mesh->file->pointer);
    } else {
      feenox_push_error_message("expecting DATASET");
      return FEENOX_ERROR;
    }  
  } while (strncmp(buffer, "DATASET", 7) != 0);
  
  if (strncmp(buffer, "DATASET UNSTRUCTURED_GRID", 25) != 0) {
    feenox_push_error_message("only UNSTRUCTURED_GRID data is supported in VTK");
    return FEENOX_ERROR;
  }
  
  // POINTS n_nodes double
  do {
    if (!feof(mesh->file->pointer)) {
      fgets(buffer, BUFFER_SIZE-1, mesh->file->pointer);
    } else {
      feenox_push_error_message("expecting POINTS");
      return FEENOX_ERROR;
    }  
  } while (strncmp(buffer, "POINTS", 6) != 0);
  
  if (sscanf(buffer, "POINTS %d %s", &mesh->n_nodes, tmp) != 2) {
    feenox_push_error_message("expected POINTS");
    return FEENOX_ERROR;
  }
  
  if (strncmp(tmp, "double", 6) != 0 && strncmp(tmp, "float", 5) != 0) {
    feenox_push_error_message("either float or double data expected");
    return FEENOX_ERROR;
  }
  
  mesh->node = calloc(mesh->n_nodes, sizeof(node_t));
  for (j = 0; j < mesh->n_nodes; j++) {
    if (fscanf(mesh->file->pointer, "%lf %lf %lf", &mesh->node[j].x[0], &mesh->node[j].x[1], &mesh->node[j].x[2]) == 0) {
      feenox_push_error_message("error reading file");
      return FEENOX_ERROR;
    }
    mesh->node[j].index_mesh = j;
    mesh->node[j].tag = j+1;
  }

  
  // CELLS n_cells numdata
  do {
    if (!feof(mesh->file->pointer)) {
      fgets(buffer, BUFFER_SIZE-1, mesh->file->pointer);
    } else {
      feenox_push_error_message("expecting CELLS");
      return FEENOX_ERROR;
    }  
  } while (strncmp(buffer, "CELLS", 5) != 0);
  
      
  if (sscanf(buffer, "CELLS %d %d", &mesh->n_elements, &numdata) != 2) {
    feenox_push_error_message("expected CELLS");
    return FEENOX_ERROR;
  }
  
  celldata = malloc(numdata*sizeof(int));
  for (i = 0; i < numdata; i++) {
    if (fscanf(mesh->file->pointer, "%d", &celldata[i]) != 1) {
      feenox_push_error_message("run out of CELLS data");
      return FEENOX_ERROR;
    }  
  }
  

  // CELL_TYPES n_cells
  do {
    if (!feof(mesh->file->pointer)) {
      fgets(buffer, BUFFER_SIZE-1, mesh->file->pointer);
    } else {
      feenox_push_error_message("expecting CELL_TYPES");
      return FEENOX_ERROR;
    }  
  } while (strncmp(buffer, "CELL_TYPES", 10) != 0);
  
      
  if (sscanf(buffer, "CELL_TYPES %d", &check) != 1) {
    feenox_push_error_message("expected CELL_TYPES");
    return FEENOX_ERROR;
  }
  
  if (mesh->n_elements != check) {
    feenox_push_error_message("CELL has %d and CELL_TYPES has %d", mesh->n_elements, check);
    return FEENOX_ERROR;
    
  }
  
  
  l = 0;
  mesh->element = calloc(mesh->n_elements, sizeof(element_t));
  for (i = 0; i < mesh->n_elements; i++) {
    if (fscanf(mesh->file->pointer, "%d", &celltype) != 1) {
      feenox_push_error_message("run out of CELLS data");
      return FEENOX_ERROR;
    }

    for (k = 0; k < NUMBER_ELEMENT_TYPE; k++) {
      if (vtkfromgmsh_types[k] == celltype) {
        mesh->element[i].type = &(feenox_mesh.element_type[k]);
      }
    }
    
    
    
    if (mesh->element[i].type == NULL) {
      // puede ser alguno de los high-order
      if (celltype == 60 || celltype == 68) {
        // line
        if (celldata[l] == 2) {
          mesh->element[i].type = &feenox_mesh.element_type[ELEMENT_TYPE_LINE2];
        } else if (celldata[l] == 3) {
          mesh->element[i].type = &feenox_mesh.element_type[ELEMENT_TYPE_LINE3];
        } else {
          feenox_push_error_message("high-order lines are supported up to order two");
          return FEENOX_ERROR;
        }
      } else if (celltype == 61 || celltype == 69) {
        // triangle
        if (celldata[l] == 3) {
          mesh->element[i].type = &feenox_mesh.element_type[ELEMENT_TYPE_TRIANGLE3];
        } else if (celldata[l] == 6) {
          mesh->element[i].type = &feenox_mesh.element_type[ELEMENT_TYPE_TRIANGLE6];
        } else {
          feenox_push_error_message("high-order triangles are supported up to order two");
          return FEENOX_ERROR;
        }
      } else if (celltype == 62 || celltype == 70) {
        // quad
        if (celldata[l] == 4) {
          mesh->element[i].type = &feenox_mesh.element_type[ELEMENT_TYPE_QUADRANGLE4];
        } else if (celldata[l] == 8) {
          mesh->element[i].type = &feenox_mesh.element_type[ELEMENT_TYPE_QUADRANGLE8];
        } else if (celldata[l] == 9) {
          mesh->element[i].type = &feenox_mesh.element_type[ELEMENT_TYPE_QUADRANGLE9];
        } else {
          feenox_push_error_message("high-order quadrangles are supported up to order two");
          return FEENOX_ERROR;
        }
      } else if (celltype == 64 || celltype == 71) {
        // tetrahedron
        if (celldata[l] == 4) {
          mesh->element[i].type = &feenox_mesh.element_type[ELEMENT_TYPE_TETRAHEDRON4];
        } else if (celldata[l] == 10) {
          mesh->element[i].type = &feenox_mesh.element_type[ELEMENT_TYPE_TETRAHEDRON10];
        } else {
          feenox_push_error_message("high-order tetrahedra are supported up to order two");
          return FEENOX_ERROR;
        }
      } else if (celltype == 67 || celltype == 72) {
        // hexahedron
        if (celldata[l] == 8) {
          mesh->element[i].type = &feenox_mesh.element_type[ELEMENT_TYPE_HEXAHEDRON8];
        } else if (celldata[l] == 20) {
          mesh->element[i].type = &feenox_mesh.element_type[ELEMENT_TYPE_HEXAHEDRON20];
        } else if (celldata[l] == 27) {
          mesh->element[i].type = &feenox_mesh.element_type[ELEMENT_TYPE_HEXAHEDRON27];
        } else {
          feenox_push_error_message("high-order hexahedra are supported up to order two");
          return FEENOX_ERROR;
        }
      } else if (celltype == 65 || celltype == 73) {
        // prism/wedge
        if (celldata[l] == 6) {
          mesh->element[i].type = &feenox_mesh.element_type[ELEMENT_TYPE_PRISM6];
        } else {
          feenox_push_error_message("high-order wedges are supported up to order one");
          return FEENOX_ERROR;
        }
      } else if (celltype == 66 || celltype == 74) {
        // prism/wedge
        if (celldata[l] == 5) {
          mesh->element[i].type = &feenox_mesh.element_type[ELEMENT_TYPE_PYRAMID5];
        } else {
          feenox_push_error_message("high-order pyramids are supported up to order one");
          return FEENOX_ERROR;
        }
      }
    }
    
    
    
    // tipo de elemento
    if (mesh->element[i].type == NULL) {
      feenox_push_error_message("vtk elements of type '%d' are not supported in this version :-(", celltype);
      return FEENOX_ERROR;
    }
    if (mesh->element[i].type->nodes == 0) {
      feenox_push_error_message("elements of type '%s' are not supported in this version :-(", mesh->element[i].type->name);
      return FEENOX_ERROR;
    }
    
    // nodos, tenemos la informacion en celldata
    if (mesh->element[i].type->nodes != celldata[l++]) {
      feenox_push_error_message("CELL %d gives %d nodes but type '%s' has %d nodes", i, celldata[l], mesh->element[i].type->name, mesh->element[i].type->nodes);
      return FEENOX_ERROR;
    }
    
    mesh->element[i].index = i;
    mesh->element[i].tag = i+1;
    
    mesh->element[i].node = calloc(mesh->element[i].type->nodes, sizeof(node_t *));
    for (j = 0; j < mesh->element[i].type->nodes; j++) {
    
      // pelar ojo al orden!
      // el tet10 tiene un unico cambio
      // ojo que los hexa20 y 27 tambien tienen cosas raras
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
      
      mesh->element[i].node[j_gmsh] = &mesh->node[celldata[l++]];
    }
    
    // y miramos aca si hay que renombrar
  }


  // listo el pollo, ya tenemos la malla, ahora vemos que mas hay
  while (fgets(buffer, BUFFER_SIZE-1, mesh->file->pointer) != NULL) {
    
    if (strncmp("POINT_DATA", buffer, 10) == 0) {

      if (sscanf(buffer, "POINT_DATA %d", &check) != 1) {
        feenox_push_error_message("expecting POINT_DATA");
        return FEENOX_ERROR;
      }
      
      if (mesh->n_nodes != check) {
        feenox_push_error_message("expecting %d POINT_DATA instead of %d", mesh->n_nodes, check);
        return FEENOX_ERROR;
      }
      
    } else if (strncmp(buffer, "SCALARS", 7) == 0 || strncmp(buffer, "VECTORS", 7) == 0) {
      
      node_data_t *node_data;
      function_t *scalar = NULL;
      function_t *vector[3] = {NULL, NULL, NULL};
      
      if (strncmp(buffer, "SCALARS", 7) == 0) {
        if (sscanf(buffer, "SCALARS %s %s", name, tmp) != 2) {
          feenox_push_error_message("expected SCALARS");
          return FEENOX_ERROR;
        }
        vector_function = 0;
      } else if (strncmp(buffer, "VECTORS", 7) == 0) {
        if (sscanf(buffer, "VECTORS %s %s", name, tmp) != 2) {
          feenox_push_error_message("expected VECTORS");
          return FEENOX_ERROR;
        }
        vector_function = 1;
      }
  
      if (strncmp(tmp, "double", 6) != 0 && strncmp(tmp, "float", 5) != 0) {
        feenox_push_error_message("either float or double data expected");
        return FEENOX_ERROR;
      }

      // vemos si nos pidieron leer este escalar o vector
      LL_FOREACH(mesh->node_datas, node_data) {
        if (vector_function == 0) {
          if (strcmp(name, node_data->name_in_mesh) == 0) {
            scalar = node_data->function;
          }
        } else {
          for (i = 0; i < 3; i++) {
            // probamos con name1 name2 name3
            // el 2*buffer_size es para que no se queje el compilador con -Wall
            snprintf(tmp, 2*BUFFER_SIZE-1, "%s%d", name, i+1);
            if (strcmp(tmp, node_data->name_in_mesh) == 0) {
              vector[i] = node_data->function;
            }
            // ahora con namex namey namez
            snprintf(tmp, 2*BUFFER_SIZE-1, "%s%c", name, 'x'+i);
            if (strcmp(tmp, node_data->name_in_mesh) == 0) {
              vector[i] = node_data->function;
            }
          }  
        }  
      }
      
      
      // si no tenemos que leer nada seguimos de largo e inogramos todo el bloque
      if (scalar == NULL && vector[0] == NULL && vector[1] == NULL && vector[2] == NULL) {
        continue;
      }
      
      // LOOKUP_TABLE default (solo para escalares)
      if (vector_function == 0) {
        do {
          if (!feof(mesh->file->pointer)) {
            fgets(buffer, BUFFER_SIZE-1, mesh->file->pointer);
          } else {
            feenox_push_error_message("expecting LOOKUP_TABLE");
            return FEENOX_ERROR;
          }  
        } while (strncmp(buffer, "LOOKUP_TABLE", 12) != 0);
      
        // si llegamos hasta aca, tenemos una funcion
        scalar->type = type_pointwise_mesh_node;
        scalar->mesh = mesh;
        scalar->data_argument = mesh->nodes_argument;
        scalar->data_size = mesh->n_nodes;
        scalar->data_value = calloc(mesh->n_nodes, sizeof(double));
      
        for (j = 0; j < mesh->n_nodes; j++) {
          if (fscanf(mesh->file->pointer, "%lf", &scalar->data_value[j]) != 1) {
            feenox_push_error_message("ran out of SCALARS data");
            return FEENOX_ERROR;
          }  
        }
        
      } else {

        // si llegamos hasta aca, tenemos una funcion
        for (i = 0; i < 3; i++) {
          if (vector[i] != NULL) {
            vector[i]->type = type_pointwise_mesh_node;
            vector[i]->mesh = mesh;
            vector[i]->data_argument = mesh->nodes_argument;
            vector[i]->data_size = mesh->n_nodes;
            vector[i]->data_value = calloc(mesh->n_nodes, sizeof(double));
          }  
        }
      
        for (j = 0; j < mesh->n_nodes; j++) {
          if (fscanf(mesh->file->pointer, "%lf %lf %lf", &f[0], &f[1], &f[2]) != 3) {
            feenox_push_error_message("ran out of VECTORS data");
            return FEENOX_ERROR;
          }
            
          for (i = 0; i < 3; i++) {
            if (vector[i] != NULL) {
              vector[i]->data_value[j] = f[i];
            }
          }  
        }
      }
    }
  }
*/  
  
  
  
  
  return FEENOX_OK;
}
