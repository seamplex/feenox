/*------------ -------------- -------- --- ----- ---   --       -            -
 *  FeenoX-to-CalculiX input file converter
 *
 *  Copyright (C) 2025 Jeremy Theler
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
#include "feenox.h"
feenox_t feenox;
#include "pdes/mechanical/mechanical.h"

#include <stdlib.h>
#include <libgen.h>

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
  13,      // ELEMENT_TYPE_POINT1         15
  10,      // ELEMENT_TYPE_QUADRANGLE8    16
   4,      // ELEMENT_TYPE_HEXAHEDRON20   17
   5       // ELEMENT_TYPE_PRISM15        18
};



int main(int argc, char **argv) {

  if (argc == 1) {
    printf("usage: %s feenox-input.fee\n", argv[0]);
    return 0;
  }

  // TODO: check this ir run in serial
  // TODO: particular argc && argv
  // e.g. --version
  //      --do-not-include-original-input
  if (feenox_initialize(argc, argv) != FEENOX_OK) {
    feenox_pop_errors();
    exit(EXIT_FAILURE);
  }

  if (feenox_step(feenox.instructions, NULL) != FEENOX_OK) {
    feenox_pop_errors();
    exit(EXIT_FAILURE);
  }

  mesh_t *mesh = feenox.mesh.mesh_main;
  feenox_instruction_mesh_read(mesh);

  printf("*NODE\n");
  for (size_t j = 0; j < mesh->n_nodes; j++) {
    printf("%ld, %g, %g, %g\n", mesh->node[j].tag, mesh->node[j].x[0], mesh->node[j].x[1], mesh->node[j].x[2]);
  }
  printf("\n");

  int element_type = ELEMENT_TYPE_UNDEFINED;
  char element_types[512]; // this is a string of all the element types we've seen
  for (size_t i = 0; i < mesh->n_elements; i++) {
    element_t *e = &mesh->element[i];  
    if (e->type->dim == feenox.pde.dim) {
      if (e->type->id != element_type) {
        if (element_type == ELEMENT_TYPE_UNDEFINED) {
          strncpy(element_types, e->type->name, 512);
        } else {
          strcat(element_types, e->type->name);  
        }
        strcat(element_types, ", ");
        printf("*ELEMENT, TYPE=%s, ELSET=%s\n", ccx_names[gmsh2ccx_types[e->type->id]], e->type->name);
        element_type = e->type->id;
      }  
      printf("%ld", mesh->element[i].tag);
      for (unsigned int j = 0; j < mesh->element[i].type->nodes; j++) {
        if (e->type->id == ELEMENT_TYPE_TETRAHEDRON10 && j == 8) {
          printf(", %ld", mesh->element[i].node[9]->tag);
        } else if (e->type->id == ELEMENT_TYPE_TETRAHEDRON10 && j == 9) {
          printf(", %ld", mesh->element[i].node[8]->tag);
        } else {
          printf(", %ld", mesh->element[i].node[j]->tag);
        }  
      }
      printf("\n");
    }
  }  
  printf("\n");

  // TODO: if material properties are all from variables and functions use this
  // otherwise go one by one
  printf("*ELSET, ELSET=bulk\n");
  element_types[strlen(element_types)-2] = '\0';
  printf("%s\n", element_types);
  printf("\n");

/*  
  printf("*ELSET, ELSET=bulk\n");
  int first = 1;
  int n = 0;
  for (size_t i = 0; i < mesh->n_elements; i++) {
    if (mesh->element[i].type->dim == 3 && strcmp(mesh->element[i].physical_group->name, "bulk") == 0) {
      if (first == 0) {
        printf(", ");
      }
      printf("%ld", mesh->element[i].tag);
      first = 0;
      if (++n == 16) {
        printf("\n");
        n = 0;
        first = 1;
      }
    }
  }
  printf("\n");
  printf("\n");
*/

  // single material
  double x[] = {0, 0, 0};
  double E = mechanical.E.eval(&mechanical.E, x, NULL);
  double nu = mechanical.nu.eval(&mechanical.nu, x, NULL);
  printf("*MATERIAL, NAME=bulk\n");
  printf("*ELASTIC\n");
  printf("%g, %g\n", E, nu);
  printf("\n");
  printf("*SOLID SECTION, ELSET=bulk, MATERIAL=bulk\n");
  printf("\n");

  printf("*BOUNDARY\n");
  for (size_t k = 0; k < feenox.pde.dirichlet_rows; k++) {
//  <node set or node>, <first DOF>, <last DOF>, <displacement value>
    printf("%ld, %d, %d, %g\n", feenox.pde.dirichlet_nodes[k],
                                feenox.pde.dirichlet_dofs[k]+1,
                                feenox.pde.dirichlet_dofs[k]+1,
                                feenox.pde.dirichlet_values[k]);
  }
  printf("\n");

  printf("*STEP\n");
  printf("*EL FILE     # Commands responsible for saving results\n");
  printf("E, S\n");
  printf("*NODE FILE\n");
  printf("U\n");
  printf("\n");
  
  printf("*CLOAD\n");
  PetscScalar *b = NULL;
  petsc_call(VecGetArray(feenox.pde.b, &b));  
  for (size_t j = 0; j < mesh->n_nodes; j++) {
    for (unsigned int g = 0; g < feenox.pde.dofs; g++) {
      if (b[mesh->node[j].index_dof[g]] != 0) {
        printf("%ld, %d, %g\n", mesh->node[j].tag, g+1, b[mesh->node[j].index_dof[g]]);          
      }
    }
  }
  petsc_call(VecRestoreArray(feenox.pde.b, &b));
  printf("\n");
  
  printf("*STATIC\n");
  printf("*END STEP\n");

  feenox_finalize();

  return 0;
}
