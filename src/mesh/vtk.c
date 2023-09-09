/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's mesh-related vtk routines
 *
 *  Copyright (C) 2014--2023 jeremy theler
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
#include "element.h"
#include <ctype.h>


#define BUFFER_SIZE 256

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


int feenox_mesh_write_mesh_vtk(mesh_t *this, FILE *file, int dummy) {
    
  fprintf(file, "DATASET UNSTRUCTURED_GRID\n");
  fprintf(file, "POINTS %ld double\n", this->n_nodes);
  for (size_t j = 0; j < this->n_nodes; j++) { 
    if (this->node[j].tag != j+1) {
      feenox_push_error_message("VTK output needs sorted nodes");
      return FEENOX_ERROR;
    }
    fprintf(file, "%g %g %g\n", this->node[j].x[0], this->node[j].x[1], this->node[j].x[2]);
  }
  fprintf(file, "\n");

  size_t size = 0;
  size_t volumelements = 0;
  for (size_t i = 0; i < this->n_elements; i++) {
    if (this->element[i].type->dim == this->dim_topo) {
      size += 1 + this->element[i].type->nodes;
      volumelements++;
    }
  }

  fprintf(file, "CELLS %ld %ld\n", volumelements, size);
  for (size_t i = 0; i < this->n_elements; i++) {
    if (this->element[i].type->dim == this->dim_topo) {
      switch(this->element[i].type->id)
        {
        case ELEMENT_TYPE_HEXAHEDRON27: 
          fprintf(file, "%d ", 27);
          for (int j = 0; j < 27 ; ++j) {
            fprintf(file, " %ld", (this->element[i].node[hexa27fromgmsh[j]]->tag)-1);
           }
          fprintf(file, "\n");
        break;
        case ELEMENT_TYPE_HEXAHEDRON20:
          fprintf(file, "%d ", 20);
          for (int j = 0; j < 20 ; ++j) {
            fprintf(file, " %ld", (this->element[i].node[hexa20fromgmsh[j]]->tag)-1);
          }
          fprintf(file, "\n");
        break;
        default:
          fprintf(file, "%d ", this->element[i].type->nodes);
          // ojo! capaz que no funcione si no estan ordenados los indices
          for (unsigned int j = 0; j < this->element[i].type->nodes; j++) {
            // el tet10 es diferente!
            if (vtkfromgmsh_types[this->element[i].type->id] == 24 && (j == 8 || j == 9)) {
              if (j == 8) {
                fprintf(file, " %ld", (this->element[i].node[9]->tag)-1);
              } else if (j == 9) {
                fprintf(file, " %ld", (this->element[i].node[8]->tag)-1);
              }
            } else {
              fprintf(file, " %ld", (this->element[i].node[j]->tag)-1);
            }
          }
          fprintf(file, "\n");
        break;
        }
      }
    }
  fprintf(file, "\n");
  
  fprintf(file, "CELL_TYPES %ld\n", volumelements);
  for (size_t i = 0; i < this->n_elements; i++) {
    if (this->element[i].type->dim == this->dim_topo) {
      fprintf(file, "%d\n", vtkfromgmsh_types[this->element[i].type->id]);
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
  if (dist->size == 1) {  
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

  // TODO: virtual methods
  if (dist->field_location == field_location_cells) {

    for (size_t i = 0; i < this->mesh->n_cells; i++) {
      for (int g = 0; g < dist->size; g++) {
        // TODO: remove the conditional from the loop
        double value = (dist->field[g]->type == function_type_pointwise_mesh_cell && dist->field[g]->mesh == this->mesh) ?
                  feenox_vector_get(dist->field[g]->vector_value, i) :
                  feenox_function_eval(dist->field[g], this->mesh->cell[i].x);
        fprintf(this->file->pointer, format, value);
      }
      fprintf(this->file->pointer, "\n");
    }
    
  } else {
    
    for (size_t j = 0; j < this->mesh->n_nodes; j++) {
      for (int g = 0; g < dist->size; g++) {
        // TODO: remove the conditional from the loop
        double value = (dist->field[g]->type == function_type_pointwise_mesh_node && dist->field[g]->mesh == this->mesh) ?
                  feenox_vector_get(dist->field[g]->vector_value, j) :
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

  if (this->file->pointer == NULL) {
    feenox_call(feenox_instruction_file_open(this->file));
  }
  FILE *fp = this->file->pointer;
  
  // header
  int version_maj = 0;
  int version_min = 0;
  if (fscanf(fp, "# vtk DataFile Version %d.%d", &version_maj, &version_min) != 2) {
    feenox_push_error_message("wrong VTK header");
    return FEENOX_ERROR;
  }

  // the \n
  char buffer[BUFFER_SIZE];
  feenox_check_alloc(fgets(buffer, BUFFER_SIZE-1, fp));

  // the case name, we can discard it
  feenox_check_alloc(fgets(buffer, BUFFER_SIZE-1, fp));

  // format has to be ASCII
  feenox_check_alloc(fgets(buffer, BUFFER_SIZE-1, fp));
  if (strncmp(buffer, "ASCII", 5) != 0) {
    buffer[strlen(buffer)-1] = '\0';
    feenox_push_error_message("only ASCII VTK files are supported, not '%s'", buffer);
    return FEENOX_ERROR;
  }
 
  // dataset unstructured grid
  do {
    if (!feof(fp)) {
      feenox_check_alloc(fgets(buffer, BUFFER_SIZE-1, fp));
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
    if (!feof(fp)) {
      feenox_check_alloc(fgets(buffer, BUFFER_SIZE-1, fp));
    } else {
      feenox_push_error_message("expecting POINTS");
      return FEENOX_ERROR;
    }  
  } while (strncmp(buffer, "POINTS", 6) != 0);
  
  char tmp[BUFFER_SIZE];
  if (sscanf(buffer, "POINTS %ld %s", &this->n_nodes, tmp) != 2) {
    feenox_push_error_message("expected POINTS");
    return FEENOX_ERROR;
  }
  
  if (strncmp(tmp, "double", 6) != 0 && strncmp(tmp, "float", 5) != 0) {
    feenox_push_error_message("either float or double data expected instead of '%s'", tmp);
    return FEENOX_ERROR;
  }
  
  feenox_check_alloc(this->node = calloc(this->n_nodes, sizeof(node_t)));
  for (size_t j = 0; j < this->n_nodes; j++) {
    if (fscanf(fp, "%lf %lf %lf", &this->node[j].x[0], &this->node[j].x[1], &this->node[j].x[2]) == 0) {
      feenox_push_error_message("error reading file");
      return FEENOX_ERROR;
    }
    this->node[j].index_mesh = j;
    this->node[j].tag = j+1;
  }

  
  // CELLS n_cells numdata
  do {
    if (!feof(fp)) {
      feenox_check_alloc(fgets(buffer, BUFFER_SIZE-1, fp));
    } else {
      feenox_push_error_message("expecting CELLS");
      return FEENOX_ERROR;
    }  
  } while (strncmp(buffer, "CELLS", 5) != 0);
  

  size_t num_cells = 0;
  size_t num_data = 0;
  size_t num_indexes = 0;
  if (sscanf(buffer, "CELLS %ld %ld", &num_cells, &num_data) != 2) {
    feenox_push_error_message("expected CELLS");
    return FEENOX_ERROR;
  }
  
  // in vtk version 5.1 the cells are stored as offset + connectivy
  size_t *celldata = NULL;
  size_t *connectivity = NULL;
  int offset_connectivity = 0;
  int c = fgetc(fp);
  if (isdigit(c)) {
    // old-fashion 4.x format
    offset_connectivity = 0;
    num_indexes = num_data;
    if (ungetc(c, fp) == EOF) {
      return FEENOX_ERROR;
    }
    
  } else if (c == 'O') {
    // new 5.1 format OFFSET
    offset_connectivity = 1;
    num_indexes = num_cells;
    num_cells--;
    if (fscanf(fp, "FFSETS %s", tmp) != 1) {
      feenox_push_error_message("expected OFFSETS");
      return FEENOX_ERROR;
    }
  }
  
  
  feenox_check_alloc(celldata = calloc(num_data, sizeof(size_t)));
  for (size_t i = 0; i < num_indexes; i++) {
    if (fscanf(fp, "%ld", &celldata[i]) != 1) {
      feenox_push_error_message("ran out of CELLS data");
      return FEENOX_ERROR;
    }
  }
  
  if (offset_connectivity) {
    // CONNECTIVITY vtktypeint64
    do {
      if (!feof(fp)) {
        feenox_check_alloc(fgets(buffer, BUFFER_SIZE-1, fp));
      } else {
        feenox_push_error_message("expecting CONNECTIVITY");
        return FEENOX_ERROR;
      }  
    } while (strncmp(buffer, "CONNECTIVITY", 12) != 0);    
  
    if (sscanf(buffer, "CONNECTIVITY %s", tmp) != 1) {
      feenox_push_error_message("expected CONNECTIVITY");
      return FEENOX_ERROR;
    }

    feenox_check_alloc(connectivity = calloc(num_data, sizeof(size_t)));
    for (size_t i = 0; i < num_data; i++) {
      if (fscanf(fp, "%ld", &connectivity[i]) != 1) {
        feenox_push_error_message("ran out of CONNECTIVITY data");
        return FEENOX_ERROR;
      }
    }
  }
  

  // CELL_TYPES n_cells
  do {
    if (!feof(fp)) {
      feenox_check_alloc(fgets(buffer, BUFFER_SIZE-1, fp));
    } else {
      feenox_push_error_message("expecting CELL_TYPES");
      return FEENOX_ERROR;
    }  
  } while (strncmp(buffer, "CELL_TYPES", 10) != 0);
  
  if (sscanf(buffer, "CELL_TYPES %ld", &this->n_elements) != 1) {
    feenox_push_error_message("expected CELL_TYPES");
    return FEENOX_ERROR;
  }
  
  size_t l = 0;
  feenox_check_alloc(this->element = calloc(this->n_elements, sizeof(element_t)));
  for (size_t i = 0; i < this->n_elements; i++) {
    int celltype = 0;
    if (fscanf(fp, "%d", &celltype) != 1) {
      feenox_push_error_message("ran out of CELL_TYPES data");
      return FEENOX_ERROR;
    }

    // TODO: make a map
    for (int k = 0; k < NUMBER_ELEMENT_TYPE && this->element[i].type == NULL; k++) {
      if (vtkfromgmsh_types[k] == celltype) {
        this->element[i].type = &(feenox.mesh.element_types[k]);
      }
    }
    
    size_t n_nodes = (offset_connectivity == 0) ? celldata[l++] : celldata[i+1] - celldata[i];
    
    if (this->element[i].type == NULL) {
      // puede ser alguno de los high-order
      if (celltype == 60 || celltype == 68) {
        // line
        if (n_nodes == 2) {
          this->element[i].type = &feenox.mesh.element_types[ELEMENT_TYPE_LINE2];
        } else if (n_nodes == 3) {
          this->element[i].type = &feenox.mesh.element_types[ELEMENT_TYPE_LINE3];
        } else {
          feenox_push_error_message("high-order lines are supported up to order two");
          return FEENOX_ERROR;
        }
      } else if (celltype == 61 || celltype == 69) {
        // triangle
        if (n_nodes == 3) {
          this->element[i].type = &feenox.mesh.element_types[ELEMENT_TYPE_TRIANGLE3];
        } else if (n_nodes == 6) {
          this->element[i].type = &feenox.mesh.element_types[ELEMENT_TYPE_TRIANGLE6];
        } else {
          feenox_push_error_message("high-order triangles are supported up to order two");
          return FEENOX_ERROR;
        }
      } else if (celltype == 62 || celltype == 70) {
        // quad
        if (n_nodes == 4) {
          this->element[i].type = &feenox.mesh.element_types[ELEMENT_TYPE_QUADRANGLE4];
        } else if (n_nodes == 8) {
          this->element[i].type = &feenox.mesh.element_types[ELEMENT_TYPE_QUADRANGLE8];
        } else if (n_nodes == 9) {
          this->element[i].type = &feenox.mesh.element_types[ELEMENT_TYPE_QUADRANGLE9];
        } else {
          feenox_push_error_message("high-order quadrangles are supported up to order two");
          return FEENOX_ERROR;
        }
      } else if (celltype == 64 || celltype == 71) {
        // tetrahedron
        if (n_nodes == 4) {
          this->element[i].type = &feenox.mesh.element_types[ELEMENT_TYPE_TETRAHEDRON4];
        } else if (n_nodes == 10) {
          this->element[i].type = &feenox.mesh.element_types[ELEMENT_TYPE_TETRAHEDRON10];
        } else {
          feenox_push_error_message("high-order tetrahedra are supported up to order two");
          return FEENOX_ERROR;
        }
      } else if (celltype == 67 || celltype == 72) {
        // hexahedron
        if (n_nodes == 8) {
          this->element[i].type = &feenox.mesh.element_types[ELEMENT_TYPE_HEXAHEDRON8];
        } else if (n_nodes == 20) {
          this->element[i].type = &feenox.mesh.element_types[ELEMENT_TYPE_HEXAHEDRON20];
        } else if (n_nodes == 27) {
          this->element[i].type = &feenox.mesh.element_types[ELEMENT_TYPE_HEXAHEDRON27];
        } else {
          feenox_push_error_message("high-order hexahedra are supported up to order two");
          return FEENOX_ERROR;
        }
      } else if (celltype == 65 || celltype == 73) {
        // prism/wedge
        if (n_nodes == 6) {
          this->element[i].type = &feenox.mesh.element_types[ELEMENT_TYPE_PRISM6];
        } else {
          feenox_push_error_message("high-order wedges are supported up to order one");
          return FEENOX_ERROR;
        }
      } else if (celltype == 66 || celltype == 74) {
        // prism/wedge
        if (n_nodes == 5) {
          this->element[i].type = &feenox.mesh.element_types[ELEMENT_TYPE_PYRAMID5];
        } else {
          feenox_push_error_message("high-order pyramids are supported up to order one");
          return FEENOX_ERROR;
        }
      }
    }
    
    // element type
    if (this->element[i].type == NULL) {
      feenox_push_error_message("vtk elements of type '%d' are not supported in this version :-(", celltype);
      return FEENOX_ERROR;
    }
    if (this->element[i].type->nodes == 0) {
      feenox_push_error_message("elements of type '%s' are not supported in this version :-(", this->element[i].type->name);
      return FEENOX_ERROR;
    }
    
    // nodes, we have the data in celldata
    if (this->element[i].type->nodes != n_nodes) {
      feenox_push_error_message("CELL %d gives %d nodes but type '%s' has %d nodes", i, n_nodes, this->element[i].type->name, this->element[i].type->nodes);
      return FEENOX_ERROR;
    }
    
    this->element[i].index = i;
    this->element[i].tag = i+1;
    
    feenox_check_alloc(this->element[i].node = calloc(this->element[i].type->nodes, sizeof(node_t *)));
    for (size_t j = 0; j < this->element[i].type->nodes; j++) {
    
      // mind the order!
      // tet10 has a single flip
      // hex20 and hex27 have other flips as well
      int j_gmsh = 0;
      if (this->element[i].type->id == ELEMENT_TYPE_TETRAHEDRON10) {
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
      
      size_t node_index = (offset_connectivity == 0) ? celldata[l++] : connectivity[celldata[i]+j];
      this->element[i].node[j_gmsh] = &this->node[node_index];
      feenox_mesh_add_element_to_list(&this->element[i].node[j_gmsh]->element_list, &this->element[i]);
    }
  }


  // we have the mesh already, now we peek what else the file has
  // TODO: CELL_DATA
  int n_fields = 0;
  int field_data = 0;
  int point_data = 1;
  while (fgets(buffer, BUFFER_SIZE-1, fp) != NULL) {
    
    if (strncmp("POINT_DATA", buffer, 10) == 0) {
      point_data = 1;

      size_t check = 0;
      if (sscanf(buffer, "POINT_DATA %ld", &check) != 1) {
        feenox_push_error_message("expecting POINT_DATA");
        return FEENOX_ERROR;
      }
      
      if (this->n_nodes != check) {
        feenox_push_error_message("expecting %ld POINT_DATA instead of %ld", this->n_nodes, check);
        return FEENOX_ERROR;
      }

    } else if (strncmp("CELL_DATA", buffer, 9) == 0) {
      // TODO: cell data
      point_data = 0;
      
      
    } else if (strncmp("METADATA", buffer, 8) == 0) {
      
      int size = 0;
      feenox_check_alloc(fgets(buffer, BUFFER_SIZE-1, fp));
      if (sscanf(buffer, "INFORMATION %d", &size) != 1) {
        feenox_push_error_message("expecting INFORMATION");
        return FEENOX_ERROR;
      }
      
      for (int i = 0; i < size; i++) {
        feenox_check_alloc(fgets(buffer, BUFFER_SIZE-1, fp));
      }
      
    } else if (strncmp("FIELD", buffer, 5) == 0) {
      
      if (sscanf(buffer, "FIELD %s %d", tmp, &n_fields) != 2) {
        feenox_push_error_message("expecting FIELD");
        return FEENOX_ERROR;
      }
      field_data = 1;
      
    } else if (strncmp(buffer, "SCALARS", 7) == 0 && point_data != 0) {
      
      char name[BUFFER_SIZE];
      if (sscanf(buffer, "SCALARS %s %s", name, tmp) != 2) {
        feenox_push_error_message("expected SCALARS");
        return FEENOX_ERROR;
      }
  
      if (strncmp(tmp, "double", 6) != 0 &&
          strncmp(tmp, "float", 5) != 0 &&
          strncmp(tmp, "int", 3) != 0) {
        feenox_push_error_message("either int, float or double data expected instead of '%s'", tmp);
        return FEENOX_ERROR;
      }
      
      feenox_call(feenox_mesh_read_vtk_field_node(this, fp, name, 1));

    } else if (strncmp(buffer, "VECTORS", 7) == 0 && point_data != 0) {
      
      char name[BUFFER_SIZE];
      if (sscanf(buffer, "VECTORS %s %s", name, tmp) != 2) {
        feenox_push_error_message("expected VECTORS");
        return FEENOX_ERROR;
      }
  
      if (strncmp(tmp, "double", 6) != 0 &&
          strncmp(tmp, "float", 5) != 0 &&
          strncmp(tmp, "int", 3) != 0) {
        feenox_push_error_message("either int, float or double data expected instead of '%s'", tmp);
        return FEENOX_ERROR;
      }
      
      feenox_call(feenox_mesh_read_vtk_field_node(this, fp, name, 3));
      
    } else if (field_data != 0 && point_data != 0) {
      
      char name[BUFFER_SIZE];
      int size = 0;
      size_t check = 0;
      if ((sscanf(buffer, "%s %d %ld %s", name, &size, &check, tmp) == 4) && (strcmp(tmp, "double") == 0 || strcmp(tmp, "float") == 0)) {
        
        if (check != this->n_nodes) {
          feenox_push_error_message("expected %ld data values but there are %ld", this->n_nodes, check);
          return FEENOX_ERROR;
        }
        
        feenox_call(feenox_mesh_read_vtk_field_node(this, fp, name, size));
        
      }

    }
  }
  
  // close the mesh file
  fclose(fp);
  this->file->pointer = NULL;
  
  return FEENOX_OK;
}



int feenox_mesh_read_vtk_field_node(mesh_t *this, FILE *fp, const char *name, unsigned int size) {
 
  char buffer[BUFFER_SIZE];
  
  // check if we have to read this scalar or vector field
  function_t **functions = NULL;
  feenox_check_alloc(functions = calloc(size, sizeof(function_t *)));
  
  int found_something = 0;
  node_data_t *node_data = NULL;
  LL_FOREACH(this->node_datas, node_data) {
    
    if (size == 1) {
      if (strcmp(name, node_data->name_in_mesh) == 0) {
        node_data->found = 1;
        found_something = 1;
        functions[0] = node_data->function;
      }
    } else {
      for (unsigned int i = 0; i < size; i++) {
        // try name1 name2 name3
        char *tried_name = NULL;
        feenox_check_minusone(asprintf(&tried_name, "%s%d", name, i+1));
        if (strcmp(tried_name, node_data->name_in_mesh) == 0) {
          node_data->found = 1;
          found_something = 1;
          functions[i] = node_data->function;
        }
        feenox_free(tried_name);

        if (functions[i] == NULL && i < 3) {
          // try namex namey namez
          feenox_check_minusone(asprintf(&tried_name, "%s%c", name, 'x'+i));
          if (strcmp(tried_name, node_data->name_in_mesh) == 0) {
            node_data->found = 1;
            found_something = 1;
            functions[i] = node_data->function;
          }
          feenox_free(tried_name);
        }
      }
    }
  }

  // if we don't have to read anything, keep on
  if (found_something == 0) {
    return FEENOX_OK;
  }

  // dummy lookup_table
  int c = fgetc(fp);
  if (ungetc(c, fp) == EOF) {
    return FEENOX_ERROR;
  }
  if (c == 'L') {
    feenox_check_alloc(fgets(buffer, BUFFER_SIZE-1, fp));
  }

  for (unsigned int i = 0; i < size; i++) {
    if (functions[i] != NULL) {
      functions[i]->type = function_type_pointwise_mesh_node;
      functions[i]->mesh = this;
      functions[i]->data_size = this->n_nodes;
      functions[i]->vector_value->size = this->n_nodes;
      feenox_call(feenox_vector_init(functions[i]->vector_value, 1));
    }
  }

  for (size_t j = 0; j < this->n_nodes; j++) {
    double xi = 0;
    for (unsigned int i = 0; i < size; i++) {
      if (fscanf(fp, "%lf", &xi) != 1) {
        feenox_push_error_message("ran out of field data");
        return FEENOX_ERROR;
      }
      
      if (functions[i] != NULL) {
        feenox_vector_set(functions[i]->vector_value, j, xi);
      }
    }  
  }
  
  feenox_free(functions);
  
  return FEENOX_OK;
}