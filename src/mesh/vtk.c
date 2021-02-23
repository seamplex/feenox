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

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <assert.h>

// conversion de gmsh a vtk
//Sacado de https://github.com/Kitware/VTK/blob/master/Common/DataModel/vtkCellType.h
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
// conversion de gmsh a vtk (by reading files because by following the docs it did not work).
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

int mesh_vtk_write_header(FILE *file) {
  fprintf(file, "# vtk DataFile Version 2.0\n");
  fprintf(file, "feenox vtk output\n");
  fprintf(file, "ASCII\n");

  return FEENOX_OK;
}


int mesh_vtk_write_mesh(mesh_t *mesh, int dummy, FILE *file) {
  
  if (mesh->structured) {
    feenox_call(mesh_vtk_write_structured_mesh(mesh, file));
  } else {
    feenox_call(mesh_vtk_write_unstructured_mesh(mesh, file));
  }
  
  return FEENOX_OK;
}

int mesh_vtk_write_structured_mesh(mesh_t *mesh, FILE *file) {
  
  int i, j, k;
  int effective_y, effective_z;
  
  assert(mesh->structured != 0);
  
  effective_y = (mesh->bulk_dimensions > 1) ? mesh->ncells_y+1 : 1;
  effective_z = (mesh->bulk_dimensions > 2) ? mesh->ncells_z+1 : 1;

  fprintf(file, "DATASET RECTILINEAR_GRID\n");
  fprintf(file, "DIMENSIONS %d %d %d\n", mesh->ncells_x+1, effective_y, effective_z);

  fprintf(file, "X_COORDINATES %d double\n", mesh->ncells_x+1);
  for (i = 0; i < mesh->ncells_x+1; i++) {
    fprintf(file, "%g ", mesh->nodes_x[i]);
  }
  fprintf(file, "\n");

  fprintf(file, "Y_COORDINATES %d double\n", effective_y);
  for (j = 0; j < effective_y; j++) {
    fprintf(file, "%g ", mesh->nodes_y[j]);
  }
  fprintf(file, "\n");

  fprintf(file, "Z_COORDINATES %d double\n", effective_z);
  for (k = 0; k < effective_z; k++) {
    fprintf(file, "%g ", mesh->nodes_z[k]);
  }
  fprintf(file, "\n");
  
  return FEENOX_OK;  
}

int mesh_vtk_write_unstructured_mesh(mesh_t *mesh, FILE *file) {
  
  int i, j;
  int size, volumelements;
  
  assert(mesh->structured == 0);

  fprintf(file, "DATASET UNSTRUCTURED_GRID\n");
  fprintf(file, "POINTS %d double\n", mesh->n_nodes);
  for (j = 0; j < mesh->n_nodes; j++) { 
    if (mesh->node[j].tag != j+1) {
      
      int on_error = (int)(feenox_value(feenox_special_var(on_gsl_error)));
      if (!(on_error & ON_ERROR_NO_REPORT)) {
        feenox_push_error_message("VTK output needs sorted nodes");
      }
      if (!(on_error & ON_ERROR_NO_QUIT)) {
        return FEENOX_OK;
      } else {
        return WASORA_RUNTIME_ERROR;
      }  
    }
    fprintf(file, "%.8g %.8g %.8g\n", mesh->node[j].x[0], mesh->node[j].x[1], mesh->node[j].x[2]);
  }
  fprintf(file, "\n");

  size = 0;
  volumelements = 0;
  for (i = 0; i < mesh->n_elements; i++) {
    if (mesh->element[i].type->dim == mesh->bulk_dimensions) {
      size += 1 + mesh->element[i].type->nodes;
      volumelements++;
    }
  }

// Here there are the cell types not supported by vtk but are shown as other cell.
//  for (i = 0; i < mesh->n_elements; i++) {
//    if (mesh->element[i].type->dim == mesh->bulk_dimensions) {
//      switch (mesh->element[i].type->id)
//        {
//        case ELEMENT_TYPE_HEXAHEDRON27:
//          size-=7;
//        break;
//        case ELEMENT_TYPE_QUADRANGLE9:
//          size-=1;
//        break;
//        }
//    }
//  }
 
  fprintf(file, "CELLS %d %d\n", volumelements, size);
  for (i = 0; i < mesh->n_elements; i++) {
    if (mesh->element[i].type->dim == mesh->bulk_dimensions) {
      switch(mesh->element[i].type->id)
        {
        case ELEMENT_TYPE_HEXAHEDRON27: 
          fprintf(file, "%d ", 27);
          for(j = 0; j < 27 ; ++j)
            {
            fprintf(file, " %d", mesh->element[i].node[hexa27fromgmsh[j]]->tag-1);
            }
          fprintf(file, "\n");
        break;
        case ELEMENT_TYPE_HEXAHEDRON20:  //It is needed to get a good order.
          fprintf(file, "%d ", 20);
          for(j = 0; j < 20 ; ++j)
            {
            fprintf(file, " %d", mesh->element[i].node[hexa20fromgmsh[j]]->tag-1);
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
                fprintf(file, " %d", mesh->element[i].node[9]->tag-1);
              } else if (j == 9) {
                fprintf(file, " %d", mesh->element[i].node[8]->tag-1);
              }
            } else {
              fprintf(file, " %d", mesh->element[i].node[j]->tag-1);
            }
          }
          fprintf(file, "\n");
        break;
        }
      }
    }
  fprintf(file, "\n");
  
  fprintf(file, "CELL_TYPES %d\n", volumelements);
  for (i = 0; i < mesh->n_elements; i++) {
    if (mesh->element[i].type->dim == mesh->bulk_dimensions) {
//The vtk unsupported cell types go here.
      switch(mesh->element[i].type->id)
        {
//        case ELEMENT_TYPE_HEXAHEDRON27:
//          fprintf(file, "%d\n", 25 );
//        break;
//        case ELEMENT_TYPE_QUADRANGLE9:
//          fprintf(file, "%d\n", 23 );
//        break;
        default:
          fprintf(file, "%d\n", vtkfromgmsh_types[mesh->element[i].type->id]);
        break;
        }
    }
  }
  
  return FEENOX_OK;  
}


int mesh_vtk_write_scalar(mesh_post_t *mesh_post, function_t *function, centering_t centering) {

  int i;
  mesh_t *mesh;
  
  if (mesh_post->mesh != NULL) {
    mesh = mesh_post->mesh;
  } else if (function != NULL) {
    mesh = function->mesh;
  } else {
    return WASORA_RUNTIME_ERROR;
  }
  
  if (centering == centering_cells) {
    if (mesh_post->cell_init == 0) {
      fprintf(mesh_post->file->pointer, "CELL_DATA %d\n", mesh->n_cells);
      mesh_post->cell_init = 1;
    }
      
    fprintf(mesh_post->file->pointer, "SCALARS %s double\n", function->name);
    fprintf(mesh_post->file->pointer, "LOOKUP_TABLE default\n");

    if (function->type == type_pointwise_mesh_cell) {
      feenox_function_init(function);
      for (i = 0; i < function->data_size; i++) {
        fprintf(mesh_post->file->pointer, "%g\n", function->data_value[i]);
      }
    } else {
      for (i = 0; i < mesh->n_cells; i++) {
        fprintf(mesh_post->file->pointer, "%g\n", feenox_evaluate_function(function, mesh->cell[i].x));
      }
    }
  } else  {
    if (mesh_post->point_init == 0) {
      fprintf(mesh_post->file->pointer, "POINT_DATA %d\n", mesh->n_nodes);
      mesh_post->point_init = 1;
    }
      
    fprintf(mesh_post->file->pointer, "SCALARS %s double\n", function->name);
    fprintf(mesh_post->file->pointer, "LOOKUP_TABLE default\n");
  
    if (function->type == type_pointwise_mesh_node && function->data_size == mesh_post->mesh->n_nodes) {
      feenox_function_init(function);
      if (function->data_value != NULL) {
        for (i = 0; i < function->data_size; i++) {
          fprintf(mesh_post->file->pointer, "%g\n", function->data_value[i]);
        } 
      } else {
        for (i = 0; i < function->data_size; i++) {
          fprintf(mesh_post->file->pointer, "%g\n", 0.0);
        } 
      }
    } else {
      for (i = 0; i < mesh->n_nodes; i++) {
        fprintf(mesh_post->file->pointer, "%g\n", feenox_evaluate_function(function, mesh->node[i].x));
      }
    }
  }

  fflush(mesh_post->file->pointer);
  
  return FEENOX_OK;
  
}


int mesh_vtk_write_vector(mesh_post_t *mesh_post, function_t **function, centering_t centering) {

  int i, j;
  mesh_t *mesh;
  
  if (mesh_post->mesh != NULL) {
    mesh = mesh_post->mesh;
  } else if (function[0] != NULL) {
    mesh = function[0]->mesh;
  } else {
    return WASORA_RUNTIME_ERROR;
  }
  
  if (centering == centering_cells) {
    if (mesh_post->cell_init == 0) {
      fprintf(mesh_post->file->pointer, "CELL_DATA %d\n", mesh->n_cells);
      mesh_post->cell_init = 1;
    }
      
    fprintf(mesh_post->file->pointer, "VECTORS %s_%s_%s double\n", function[0]->name, function[1]->name, function[1]->name);
      
    for (i = 0; i < mesh->n_cells; i++) {
      fprintf(mesh_post->file->pointer, "%g %g %g\n", feenox_evaluate_function(function[0], mesh->cell[i].x),
                                                      feenox_evaluate_function(function[1], mesh->cell[i].x),
                                                      feenox_evaluate_function(function[2], mesh->cell[i].x));
    }
  } else {
    if (mesh_post->point_init == 0) {
      fprintf(mesh_post->file->pointer, "POINT_DATA %d\n", mesh->n_nodes);
      mesh_post->point_init = 1;
    }

    feenox_function_init(function[0]);
    feenox_function_init(function[1]);
    feenox_function_init(function[2]);
    
    fprintf(mesh_post->file->pointer, "VECTORS %s_%s_%s double\n", function[0]->name, function[1]->name, function[2]->name);
      
    for (j = 0; j < mesh->n_nodes; j++) {
      if (function[0]->type == type_pointwise_mesh_node && function[0]->data_size == mesh_post->mesh->n_nodes) {
        fprintf(mesh_post->file->pointer, "%g ", (function[0]->data_value != NULL)?function[0]->data_value[j]:0);
      } else {
        fprintf(mesh_post->file->pointer, "%g ", feenox_evaluate_function(function[0], mesh->node[j].x));
      }

      if (function[1]->type == type_pointwise_mesh_node && function[1]->data_size == mesh_post->mesh->n_nodes) {
        fprintf(mesh_post->file->pointer, "%g ", (function[1]->data_value != NULL)?function[1]->data_value[j]:0);
      } else {
        fprintf(mesh_post->file->pointer, "%g ", feenox_evaluate_function(function[1], mesh->node[j].x));
      }

      if (function[2]->type == type_pointwise_mesh_node && function[2]->data_size == mesh_post->mesh->n_nodes) {
        fprintf(mesh_post->file->pointer, "%g\n", (function[2]->data_value != NULL)?function[2]->data_value[j]:0);
      } else {
        fprintf(mesh_post->file->pointer, "%g\n", feenox_evaluate_function(function[2], mesh->node[j].x));
      }
    }
  }
  
     
  fflush(mesh_post->file->pointer);
  
  return FEENOX_OK;
  
}



int mesh_vtk_readmesh(mesh_t *mesh) {

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
    return WASORA_RUNTIME_ERROR;
  }

  // el \n
  fgets(buffer, BUFFER_SIZE-1, mesh->file->pointer);

  // el nombre del caso, lo tiramos
  fgets(buffer, BUFFER_SIZE-1, mesh->file->pointer);

  // el formato debe ser ASCII
  fgets(buffer, BUFFER_SIZE-1, mesh->file->pointer);
  if (strncmp(buffer, "ASCII", 5) != 0) {
    feenox_push_error_message("only ASCII VTK files are supported, not '%s'", buffer);
    return WASORA_RUNTIME_ERROR;
  }
 
  // dataset unstructured grid
  do {
    if (!feof(mesh->file->pointer)) {
      fgets(buffer, BUFFER_SIZE-1, mesh->file->pointer);
    } else {
      feenox_push_error_message("expecting DATASET");
      return WASORA_RUNTIME_ERROR;
    }  
  } while (strncmp(buffer, "DATASET", 7) != 0);
  
  if (strncmp(buffer, "DATASET UNSTRUCTURED_GRID", 25) != 0) {
    feenox_push_error_message("only UNSTRUCTURED_GRID data is supported in VTK");
    return WASORA_RUNTIME_ERROR;
  }
  
  // POINTS n_nodes double
  do {
    if (!feof(mesh->file->pointer)) {
      fgets(buffer, BUFFER_SIZE-1, mesh->file->pointer);
    } else {
      feenox_push_error_message("expecting POINTS");
      return WASORA_RUNTIME_ERROR;
    }  
  } while (strncmp(buffer, "POINTS", 6) != 0);
  
  if (sscanf(buffer, "POINTS %d %s", &mesh->n_nodes, tmp) != 2) {
    feenox_push_error_message("expected POINTS");
    return WASORA_RUNTIME_ERROR;
  }
  
  if (strncmp(tmp, "double", 6) != 0 && strncmp(tmp, "float", 5) != 0) {
    feenox_push_error_message("either float or double data expected");
    return WASORA_RUNTIME_ERROR;
  }
  
  mesh->node = calloc(mesh->n_nodes, sizeof(node_t));
  for (j = 0; j < mesh->n_nodes; j++) {
    if (fscanf(mesh->file->pointer, "%lf %lf %lf", &mesh->node[j].x[0], &mesh->node[j].x[1], &mesh->node[j].x[2]) == 0) {
      feenox_push_error_message("error reading file");
      return WASORA_RUNTIME_ERROR;
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
      return WASORA_RUNTIME_ERROR;
    }  
  } while (strncmp(buffer, "CELLS", 5) != 0);
  
      
  if (sscanf(buffer, "CELLS %d %d", &mesh->n_elements, &numdata) != 2) {
    feenox_push_error_message("expected CELLS");
    return WASORA_RUNTIME_ERROR;
  }
  
  celldata = malloc(numdata*sizeof(int));
  for (i = 0; i < numdata; i++) {
    if (fscanf(mesh->file->pointer, "%d", &celldata[i]) != 1) {
      feenox_push_error_message("run out of CELLS data");
      return WASORA_RUNTIME_ERROR;
    }  
  }
  

  // CELL_TYPES n_cells
  do {
    if (!feof(mesh->file->pointer)) {
      fgets(buffer, BUFFER_SIZE-1, mesh->file->pointer);
    } else {
      feenox_push_error_message("expecting CELL_TYPES");
      return WASORA_RUNTIME_ERROR;
    }  
  } while (strncmp(buffer, "CELL_TYPES", 10) != 0);
  
      
  if (sscanf(buffer, "CELL_TYPES %d", &check) != 1) {
    feenox_push_error_message("expected CELL_TYPES");
    return WASORA_RUNTIME_ERROR;
  }
  
  if (mesh->n_elements != check) {
    feenox_push_error_message("CELL has %d and CELL_TYPES has %d", mesh->n_elements, check);
    return WASORA_RUNTIME_ERROR;
    
  }
  
  
  l = 0;
  mesh->element = calloc(mesh->n_elements, sizeof(element_t));
  for (i = 0; i < mesh->n_elements; i++) {
    if (fscanf(mesh->file->pointer, "%d", &celltype) != 1) {
      feenox_push_error_message("run out of CELLS data");
      return WASORA_RUNTIME_ERROR;
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
          return WASORA_RUNTIME_ERROR;
        }
      } else if (celltype == 61 || celltype == 69) {
        // triangle
        if (celldata[l] == 3) {
          mesh->element[i].type = &feenox_mesh.element_type[ELEMENT_TYPE_TRIANGLE3];
        } else if (celldata[l] == 6) {
          mesh->element[i].type = &feenox_mesh.element_type[ELEMENT_TYPE_TRIANGLE6];
        } else {
          feenox_push_error_message("high-order triangles are supported up to order two");
          return WASORA_RUNTIME_ERROR;
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
          return WASORA_RUNTIME_ERROR;
        }
      } else if (celltype == 64 || celltype == 71) {
        // tetrahedron
        if (celldata[l] == 4) {
          mesh->element[i].type = &feenox_mesh.element_type[ELEMENT_TYPE_TETRAHEDRON4];
        } else if (celldata[l] == 10) {
          mesh->element[i].type = &feenox_mesh.element_type[ELEMENT_TYPE_TETRAHEDRON10];
        } else {
          feenox_push_error_message("high-order tetrahedra are supported up to order two");
          return WASORA_RUNTIME_ERROR;
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
          return WASORA_RUNTIME_ERROR;
        }
      } else if (celltype == 65 || celltype == 73) {
        // prism/wedge
        if (celldata[l] == 6) {
          mesh->element[i].type = &feenox_mesh.element_type[ELEMENT_TYPE_PRISM6];
        } else {
          feenox_push_error_message("high-order wedges are supported up to order one");
          return WASORA_RUNTIME_ERROR;
        }
      } else if (celltype == 66 || celltype == 74) {
        // prism/wedge
        if (celldata[l] == 5) {
          mesh->element[i].type = &feenox_mesh.element_type[ELEMENT_TYPE_PYRAMID5];
        } else {
          feenox_push_error_message("high-order pyramids are supported up to order one");
          return WASORA_RUNTIME_ERROR;
        }
      }
    }
    
    
    
    // tipo de elemento
    if (mesh->element[i].type == NULL) {
      feenox_push_error_message("vtk elements of type '%d' are not supported in this version :-(", celltype);
      return WASORA_RUNTIME_ERROR;
    }
    if (mesh->element[i].type->nodes == 0) {
      feenox_push_error_message("elements of type '%s' are not supported in this version :-(", mesh->element[i].type->name);
      return WASORA_RUNTIME_ERROR;
    }
    
    // nodos, tenemos la informacion en celldata
    if (mesh->element[i].type->nodes != celldata[l++]) {
      feenox_push_error_message("CELL %d gives %d nodes but type '%s' has %d nodes", i, celldata[l], mesh->element[i].type->name, mesh->element[i].type->nodes);
      return WASORA_RUNTIME_ERROR;
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
        return WASORA_RUNTIME_ERROR;
      }
      
      if (mesh->n_nodes != check) {
        feenox_push_error_message("expecting %d POINT_DATA instead of %d", mesh->n_nodes, check);
        return WASORA_RUNTIME_ERROR;
      }
      
    } else if (strncmp(buffer, "SCALARS", 7) == 0 || strncmp(buffer, "VECTORS", 7) == 0) {
      
      node_data_t *node_data;
      function_t *scalar = NULL;
      function_t *vector[3] = {NULL, NULL, NULL};
      
      if (strncmp(buffer, "SCALARS", 7) == 0) {
        if (sscanf(buffer, "SCALARS %s %s", name, tmp) != 2) {
          feenox_push_error_message("expected SCALARS");
          return WASORA_RUNTIME_ERROR;
        }
        vector_function = 0;
      } else if (strncmp(buffer, "VECTORS", 7) == 0) {
        if (sscanf(buffer, "VECTORS %s %s", name, tmp) != 2) {
          feenox_push_error_message("expected VECTORS");
          return WASORA_RUNTIME_ERROR;
        }
        vector_function = 1;
      }
  
      if (strncmp(tmp, "double", 6) != 0 && strncmp(tmp, "float", 5) != 0) {
        feenox_push_error_message("either float or double data expected");
        return WASORA_RUNTIME_ERROR;
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
            return WASORA_RUNTIME_ERROR;
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
            return WASORA_RUNTIME_ERROR;
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
            return WASORA_RUNTIME_ERROR;
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
  
  
  
  
  
  return FEENOX_OK;
}
