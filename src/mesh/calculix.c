/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's mesh-related routines to read frd files from calculix
 *
 *  Copyright (C) 2018--2025 Jeremy Theler
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

// main reference
//   https://web.mit.edu/calculix_v2.7/CalculiX/cgx_2.7/doc/cgx/node168.html

/*
https://github.com/calculix/ccx2paraview
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

char *ccx_names[] = {
  "",
  "C3D8",
  "C3D6",
  "C3D4",
  "C3D20",
  "C3D15",
  "C3D10",
  "CPS3",
  "CPS6",
  "CPS4",
  "CPS8",
  "T3D2",
  "T3D3",
  "MASS"  
};

int ccx2gmsh_types[] = {
  ELEMENT_TYPE_UNDEFINED,     // 0
  ELEMENT_TYPE_HEXAHEDRON8,   // 1
  ELEMENT_TYPE_PRISM6,        // 2
  ELEMENT_TYPE_TETRAHEDRON4,  // 3
  ELEMENT_TYPE_HEXAHEDRON20,  // 4
  ELEMENT_TYPE_PRISM15,       // 6
  ELEMENT_TYPE_TETRAHEDRON10, // 6
  ELEMENT_TYPE_TRIANGLE3,     // 7
  ELEMENT_TYPE_TRIANGLE6,     // 8
  ELEMENT_TYPE_QUADRANGLE4,   // 9
  ELEMENT_TYPE_QUADRANGLE8,   // 10
  ELEMENT_TYPE_LINE2,         // 11
  ELEMENT_TYPE_LINE3,         // 12
  ELEMENT_TYPE_POINT1         // 13
};


int gmsh2ccx_types[] = {
   0,      // ELEMENT_TYPE_UNDEFINED      0
  11,      // ELEMENT_TYPE_LINE2          1
   7,      // ELEMENT_TYPE_TRIANGLE3      2
   9,      // ELEMENT_TYPE_QUADRANGLE4    3
   3,      // ELEMENT_TYPE_TETRAHEDRON4   4
   1,      // ELEMENT_TYPE_HEXAHEDRON8    5
   2,      // ELEMENT_TYPE_PRISM6         6
   0,      // ELEMENT_TYPE_PYRAMID5       7
  12,      // ELEMENT_TYPE_LINE3          8
   8,      // ELEMENT_TYPE_TRIANGLE6      9
   0,      // ELEMENT_TYPE_QUADRANGLE9    10
   6,      // ELEMENT_TYPE_TETRAHEDRON10  11
   0,      // ELEMENT_TYPE_HEXAHEDRON27   12
   0,      // ELEMENT_TYPE_PRISM18        13
   0,      // ELEMENT_TYPE_PYRAMID14      14
  13,      // ELEMENT_TYPE_POINT1         15
  10,      // ELEMENT_TYPE_QUADRANGLE8    16
   4,      // ELEMENT_TYPE_HEXAHEDRON20   17
   5       // ELEMENT_TYPE_PRISM15        18
};

int gmsh2ccx_hex20[] = {0,1,2,3,4,5,6,7, 8,11,13,9,16,18,19,17,10,12,14,15};
int ccx2gmsh_hex20[] = {0,1,2,3,4,5,6,7, 8,11,16,9,17,10,18,19,12,15,13,14};

#define BUFFER_SIZE 512
int feenox_mesh_read_frd(mesh_t *this) {

  char buffer[BUFFER_SIZE];
  char tmp[BUFFER_SIZE];
  int *tag2index = NULL;

  if (this->file->pointer == NULL) {
    feenox_call(feenox_instruction_file_open(this->file));
  }

  int sparse = 0;

  // start assuming all these are zero
  int bulk_dimensions = 0;
  int spatial_dimensions = 0;
  int order = 0;

  while (fgets(buffer, BUFFER_SIZE-1, this->file->pointer) != NULL) {

    if (strncmp("\n", buffer, 1) == 0) {
      ;

    // ------------------------------------------------------  
    } else if (strncmp("    1C", buffer, 6) == 0) {
      // Format:(1X,'   1','C',A6)  
      // Values: KEY, CODE, NAME
      // >    1Ctest             !''1C'' defines a new calc of name ''test''
      //
      // case name at buffer+6
      // 
      ;

    // ------------------------------------------------------  
    } else if (strncmp("    1U", buffer, 6) == 0) {
      // Format:(1X,'   1','U',A66)  
      // Values: KEY, CODE, STRING
      // >    1UDATE  26.01.2000 !''1U'' stores user job-informations      
      //
      // user-provided field at buffer+6 and user-provided data afterwards
      ;

    // ------------------------------------------------------  
    } else if (strncmp("    2C", buffer, 6) == 0) {
      // Format:(1X,'   2','C',18X,I12,37X,I1)
      // Values: KEY, CODE,NUMNOD, FORMAT
      // Where: KEY    = 2
      //        CODE   = C
      //        NUMNOD = Number of nodes in this block
      //        FORMAT = Format indicator
      //                 0  short format
      //                 1  long format 
      //                 2  binary format       
 
      // number of nodes and "format"
      int format;
      if (sscanf(buffer, "%s %ld %d", tmp, &this->n_nodes, &format) != 3) {
        feenox_push_error_message("error parsing number of nodes '%s'", buffer);
        return FEENOX_ERROR;
      }
      if (this->n_nodes == 0) {
        feenox_push_error_message("no nodes found in mesh '%s'", this->file->path);
        return FEENOX_ERROR;
      }
      if (format > 1) {
        feenox_push_error_message("node format %d not supported'", format);
        return FEENOX_ERROR;
      }

      feenox_check_alloc(this->node = calloc(this->n_nodes, sizeof(node_t)));

      int tag_max = this->n_nodes;
      for (size_t j = 0; j < this->n_nodes; j++) {
        int minusone;
        if (fscanf(this->file->pointer, "%d", &minusone) != 1) {
          feenox_push_error_message("error parsing nodes", buffer);
          return FEENOX_ERROR;
        }
        if (minusone != -1) {
          feenox_push_error_message("expected minus one as line starter", buffer);
          return FEENOX_ERROR;
        }

        int tag = 0;
        if (fscanf(this->file->pointer, "%d", &tag) == 0) {
          return FEENOX_ERROR;
        }

        if (j+1 != tag) {
          sparse = 1;
        }

        if ((this->node[j].tag = tag) > tag_max) {
          tag_max = this->node[j].tag;
        }

        this->node[j].index_mesh = j;

        for (unsigned int d = 0; d < 3; d++) {
          if (fscanf(this->file->pointer, "%lf", &this->node[j].x[d]) == 0) {
            return FEENOX_ERROR;
          }
        }

        if (spatial_dimensions < 1 && fabs(this->node[j].x[0]) > 1e-6) {
          spatial_dimensions = 1;
        }
        if (spatial_dimensions < 2 && fabs(this->node[j].x[1]) > 1e-6) {
          spatial_dimensions = 2;
        }
        if (spatial_dimensions < 3 && fabs(this->node[j].x[2]) > 1e-6) {
          spatial_dimensions = 3;
        }
      }

      // -3
      int minusthree;
      if (fscanf(this->file->pointer, "%d", &minusthree) != 1) {
        feenox_push_error_message("error parsing nodes", buffer);
        return FEENOX_ERROR;
      }
      if (minusthree != -3) {
        feenox_push_error_message("expected minus three as line starter", buffer);
        return FEENOX_ERROR;
      }

      // finished reading the nodes, handle sparse node tags
      if (sparse) {
        // TODO: use the array/map from vtk
        feenox_check_alloc(tag2index = malloc((tag_max+1) * sizeof(int)));
        for (size_t k = 0; k <= tag_max; k++) {
          tag2index[k] = -1;
        }
        for (size_t j = 0; j < this->n_nodes; j++) {
          tag2index[this->node[j].tag] = j;
        }
      }


    // ------------------------------------------------------      
    } else if (strncmp("    3C", buffer, 6) == 0) {
      // Format:(1X,'   3','C',18X,I12,37X,I1)
      // Values: KEY, CODE,NUMELEM, FORMAT
      // Where: KEY    = 3
      //        CODE   = C
      //        NUMELEM= Number of elements in this block
      //        FORMAT = Format indicator
      //                 0  short format
      //                 1  long format 
      //                 2  binary format       

      // TODO: mind the fact that tere might be different "element blocks" for different materials
      // number of elements and "format"
      int format;
      if (sscanf(buffer, "%s %ld %d", tmp, &this->n_elements, &format) != 3) {
        feenox_push_error_message("error parsing number of elements '%s'", buffer);
        return FEENOX_ERROR;
      }
      if (this->n_elements == 0) {
        feenox_push_error_message("no elements found in mesh '%s'", this->file->path);
        return FEENOX_ERROR;
      }
      if (format > 1) {
        feenox_push_error_message("element format %d not supported'", format);
        return FEENOX_ERROR;
      }

      feenox_check_alloc(this->element = calloc(this->n_elements, sizeof(element_t)));
      for (size_t i = 0; i < this->n_elements; i++) {

        // The following block of records must be repeated for each element:
        //  The first record initializes an element definition:
        //  Short Format:(1X,'-1',I5,3I5)
        //  Long Format:(1X,'-1',I10,3I5)
        //  Values: KEY, ELEMENT, TYPE, GROUP, MATERIAL
        // Where: KEY    = -1
        //       NODE   = element number
        //       TYPE   = element type, see section ''Element Types''
        //       GROUP  = element group number, see command ''grps''
        //       MATERIAL= element material number, see command ''mats''.        

        int minusone;
        if (fscanf(this->file->pointer, "%d", &minusone) != 1) {
          feenox_push_error_message("error parsing nodes", buffer);
          return FEENOX_ERROR;
        }
        if (minusone != -1) {
          feenox_push_error_message("expected minus one as line starter", buffer);
          return FEENOX_ERROR;
        }

        int tag = 0;
        if (fscanf(this->file->pointer, "%d", &tag) == 0) {
          return FEENOX_ERROR;
        }
        this->element[i].index = i;
        this->element[i].tag = tag;

        int ccx_element_type;
        if (fscanf(this->file->pointer, "%d", &ccx_element_type) == 0) {
          return FEENOX_ERROR;
        }
        if (ccx_element_type > 13) {
          feenox_push_error_message("element type '%d' should be less than 14", ccx_element_type);
          return FEENOX_ERROR;
        }
        this->element[i].type = &(feenox.mesh.element_types[ccx2gmsh_types[ccx_element_type]]);
        if (this->element[i].type->nodes == 0) {
          feenox_push_error_message("elements of type '%s' are not supported in this version :-(", this->element[i].type->name);
          return FEENOX_ERROR;
        }

        // rec[0] = element group
        // rec[1] = material
        int rec[2];
        if (fscanf(this->file->pointer, "%d %d", &rec[0], &rec[1]) < 2) {
          return FEENOX_ERROR;
        }

        if (this->element[i].type->dim > bulk_dimensions) {
          bulk_dimensions = this->element[i].type->dim;
        }

        if (this->element[i].type->order > order) {
          order = this->element[i].type->order;
        }

        // highest node count
        if (this->element[i].type->nodes > this->max_nodes_per_element) {
          this->max_nodes_per_element = this->element[i].type->nodes;
        }

        // highest neighbors count
        if (this->element[i].type->faces > this->max_faces_per_element) {
          this->max_faces_per_element = this->element[i].type->faces;
        }

        // -2
        int minustwo;
        if (fscanf(this->file->pointer, "%d", &minustwo) != 1) {
          feenox_push_error_message("error parsing elements", buffer);
          return FEENOX_ERROR;
        }
        if (minustwo != -2) {
          feenox_push_error_message("expected minus two as line starter", buffer);
          return FEENOX_ERROR;
        }

        feenox_check_alloc(this->element[i].node = calloc(this->element[i].type->nodes, sizeof(node_t *)));
        for (size_t j = 0; j < this->element[i].type->nodes; j++) {
          int node = 0;
          do {
            if (fscanf(this->file->pointer, "%d", &node) == 0) {
              return FEENOX_ERROR;
            }
            // printf("node %d\n", node);
            // printf("max nodes %d\n", this->n_nodes);
            if (sparse == 0 && node != -2 && node > this->n_nodes) {
              feenox_push_error_message("node %d in element %d does not exist", node, tag);
              return FEENOX_ERROR;
            }
          } while (node == -2);

          // TODO: wedge 2nd order
          int j_gmsh = j;
          if (this->element[i].type->id == ELEMENT_TYPE_TETRAHEDRON10) {
            if (j == 8) {
              j_gmsh = 9;
            } else if (j == 9) {
              j_gmsh = 8;
            } else {
              j_gmsh = j;
            }
          } else if (this->element[i].type->id == ELEMENT_TYPE_TETRAHEDRON10) {
            j_gmsh = ccx2gmsh_hex20[j];
          }

          int node_index = (sparse==0) ? node-1 : tag2index[node];
          if (node_index < 0) {
            feenox_push_error_message("node %d in element %d does not exist", node, tag);
            return FEENOX_ERROR;
          }
          this->element[i].node[j_gmsh] = &this->node[node_index];
          feenox_mesh_add_element_to_list(&this->element[i].node[j_gmsh]->element_list, &this->element[i]);
        }
      }

      // -3
      int minusthree;
      if (fscanf(this->file->pointer, "%d", &minusthree) != 1) {
        feenox_push_error_message("error parsing nodes", buffer);
        return FEENOX_ERROR;
      }
      if (minusthree != -3) {
        feenox_push_error_message("expected minus three as line starter", buffer);
        return FEENOX_ERROR;
      }
     
    // ------------------------------------------------------      
    } else if (strncmp("  100C", buffer, 6) == 0) {
      
      // Format:(1X,' 100','C',6A1,E12.5,I12,20A1,I2,I5,10A1,I2)
      // Values: KEY,CODE,SETNAME,VALUE,NUMNOD,TEXT,ICTYPE,NUMSTP,ANALYS,
      //         FORMAT
      // Where: KEY    = 100
      //        CODE   = C
      //        SETNAME= Name (not used)
      //        VALUE  = Could be frequency, time or any numerical value
      //        NUMNOD = Number of nodes in this nodal results block
      //        TEXT   = Any text
      //        ICTYPE = Analysis type
      //                 0  static
      //                 1  time step
      //                 2  frequency
      //                 3  load step
      //                 4  user named
      //        NUMSTP = Step number
      //        ANALYS = Type of analysis (description)
      //        FORMAT = Format indicator
      //                 0  short format
      //                 1  long format 
      //                 2  binary format       
      
      node_data_t *node_data;
      function_t **function;
      
      int number_of_nodes;
      if (sscanf(buffer+25, "%d", &number_of_nodes) != 1) {
        feenox_push_error_message("error parsing 100CL '%s'", buffer);
        return FEENOX_ERROR;
      }

      int ictype;
      int numstp;
      if (sscanf(buffer+38, "%d %d", &ictype, &numstp) != 2) {
        feenox_push_error_message("error parsing 100CL '%s'", buffer);
        return FEENOX_ERROR;
      }
      
      int format;
      if (sscanf(buffer+74, "%d", &format) != 1) {
        feenox_push_error_message("error parsing 100CL '%s'", buffer);
        return FEENOX_ERROR;
      }
      
      
      if (number_of_nodes != this->n_nodes || ictype != 0 || numstp != 1 || format > 1) {
        continue;
      }
      
      if (fgets(buffer, BUFFER_SIZE-1, this->file->pointer) == NULL) {
        feenox_push_error_message("error parsing -4");
        return FEENOX_ERROR;
      }
      
      // Format:(1X,I2,2X,8A1,2I5)
      // Values: KEY, NAME, NCOMPS, IRTYPE
      // Where: KEY    = -4
      //        NAME   = Dataset name to be used in the menu
      //        NCOMPS = Number of entities
      //        IRTYPE = 1  Nodal data, material independent
      //                 2  Nodal data, material dependant
      //                 3  Element data at nodes (not used)       
      int minusfour;
      int ncomps;
      int irtype;
      if (sscanf(buffer, "%d %s %d %d", &minusfour, tmp, &ncomps, &irtype) != 4) {
        feenox_push_error_message("error parsing -4 '%s'", buffer);
        return FEENOX_ERROR;
      }
      
      if (minusfour != -4) {
        feenox_push_error_message("error parsing -4 '%s'", buffer);
        return FEENOX_ERROR;
      }
      
      if (irtype > 2) {
        continue;
      }
      
      // esto es un array de apuntadores, arrancamos en null y si
      // el nombre en el frd coincide con alguna de las funciones
      // que nos pidieron, entonces lo hacemos apuntar ahi
      feenox_check_alloc(function = calloc(ncomps, sizeof(function_t *)));
      
      for (unsigned int g = 0; g < ncomps; g++) {
        
        // Format:(1X,I2,2X,8A1,5I5,8A1) 
        // Values: KEY, NAME, MENU, ICTYPE, ICIND1, ICIND2, IEXIST, ICNAME
        // Where: KEY    = -5
        //        NAME   = Entity name to be used in the menu for this comp.
        //        MENU   = 1
        //        ICTYPE = Type of entity
        //                 1  scalar
        //                 2  vector with 3 components
        //                 4  matrix
        //                12  vector with 3 amplitudes and 3 phase-angles in
        //                    degree
        //        ICIND1 = sub-component index or row number
        //        ICIND2 = column number for ICTYPE=4
        //        IEXIST = 0  data are provided
        //                 1  data are to be calculated by predefined
        //                    functions (not used)
        //                 2  as 0 but earmarked
        //        ICNAME = Name of the predefined calculation (not used)
        //                 ALL  calculate the total displacement if ICTYPE=2        
        
        if (fgets(buffer, BUFFER_SIZE-1, this->file->pointer) == NULL) {
          feenox_push_error_message("error parsing -5");
          return FEENOX_ERROR;
        }
        
        int minusfive;
        int menu;
        if (sscanf(buffer, "%d %s %d", &minusfive, tmp, &menu) != 3) {
          feenox_push_error_message("error parsing -5 '%s'", buffer);
          return FEENOX_ERROR;
        }
        
        if (minusfive != -5) {
          feenox_push_error_message("error parsing -5 '%s'", buffer);
          return FEENOX_ERROR;
        }
        if (menu != 1) {
          feenox_push_error_message("menu should be 1 '%s'", buffer);
          return FEENOX_ERROR;
        }

        LL_FOREACH(this->node_datas, node_data) {
          if (strcmp(tmp, node_data->name_in_mesh) == 0) {
            function[g] = node_data->function;

            function[g]->type = function_type_pointwise_mesh_node;
            function[g]->data_size = this->n_nodes;
            function[g]->mesh = this;
            function[g]->vector_value->size = this->n_nodes;
            feenox_call(feenox_vector_init(function[g]->vector_value, 1));
            node_data->found = 1;
          }
        }
        
        
        // esto es algo inventado por el viejo de calculix, donde son indices que aparecen
        // a veces si y a veces no, el cuento es que si la funcion se llama ALL no hay
        // datos sino que viene de calcular el modulo del vector desplazamiento
        if (strcmp(tmp, "ALL") == 0) {
          ncomps--;  // esto funciona solo si ALL es la ultima funcion
        }
        
      }
        
      for (size_t j = 0; j < number_of_nodes; j++) {
        int minusone;
        int node;
        if (fscanf(this->file->pointer, "%d %d", &minusone, &node) != 2) {
          feenox_push_error_message("error reading file");
          return FEENOX_ERROR;
        }
        if (minusone != -1) {
          feenox_push_error_message("expected -1 '%s'", buffer);
          return FEENOX_ERROR;
        }
        if (sparse == 0 && (node < 1 || node > this->n_nodes)) {
          feenox_push_error_message("invalid node number '%d'", node);
          return FEENOX_ERROR;
        }
          
        for (unsigned int g = 0; g < ncomps; g++) {
          // TODO: mind that if there are more than 6 we'd need to read a -2
          double scalar;
          if (fscanf(this->file->pointer, "%lf", &scalar) == 0) {
            feenox_push_error_message("error reading file");
            return FEENOX_ERROR;
          }
          if (function[g] != NULL) {
            int node_index = (sparse==0) ? node-1 : tag2index[node];
            if (node_index < 0) {
              feenox_push_error_message("node %d does not exist", node);
              return FEENOX_ERROR;
            }
            feenox_vector_set(function[g]->vector_value, node_index, scalar);
          }
        }
      }
      
      // -3
      int minusthree;
      if (fscanf(this->file->pointer, "%d", &minusthree) != 1) {
        feenox_push_error_message("error parsing -3");
        return FEENOX_ERROR;
      }
      if (minusthree != -3) {
        feenox_push_error_message("expected minus three as line starter", buffer);
        return FEENOX_ERROR;
      }
      
    // ------------------------------------------------------      
    }
  }

  fclose(this->file->pointer);
  this->file->pointer = NULL;

  if (tag2index != NULL) {
    feenox_free(tag2index);
  }  

  
  // verificamos que la malla tenga la dimension esperada
  if (this->dim_topo == 0) {
    this->dim = bulk_dimensions;
  } else if (this->dim != bulk_dimensions) {
    feenox_push_error_message("mesh '%s' is expected to have %d dimensions but it has %d", this->file->path, this->dim, bulk_dimensions);
    return FEENOX_ERROR;
  }
  this->dim = spatial_dimensions;
  this->order = order;  

  return FEENOX_OK;
}
