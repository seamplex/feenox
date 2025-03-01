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

#include <stdlib.h>
#include <libgen.h>

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

  // TODO: other elements (think how)
  // TODO: if all the volumes have the same property we can do
  //       *Element, TYPE=C3D4, ELSET=volumes
  printf("*ELEMENT, TYPE=C3D4, ELSET=tet4\n");
  for (size_t i = 0; i < mesh->n_elements; i++) {
    if (mesh->element[i].type == &(feenox.mesh.element_types[ELEMENT_TYPE_TETRAHEDRON4])) {
      printf("%ld", mesh->element[i].tag);
      // the nodes
      for (unsigned int j = 0; j < mesh->element[i].type->nodes; j++) {
        printf(", %ld", mesh->element[i].node[j]->tag);
      }
      printf("\n");
    }
  }  
  printf("\n");

  printf("*ELSET, ELSET=bulk\n");
  printf("tet4\n");
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
  
  printf("*NSET, NSET=left\n");
  for (size_t i = 0; i < mesh->n_elements; i++) {
    element_t *e = &mesh->element[i];
    if (e->physical_group != NULL && strcmp(e->physical_group->name, "left") == 0) {
      for (unsigned int j = 0; j < e->type->nodes; j++) {
        if (j != 0) {
          printf(", ");
        }
        printf("%ld", e->node[j]->tag);
      }
      printf("\n");
    }
  }
  printf("\n");
  
  printf("*MATERIAL, NAME=bulk\n");
  printf("*ELASTIC\n");
  printf("210e9, 0.3\n");
  printf("\n");
  printf("*SOLID SECTION, ELSET=bulk, MATERIAL=bulk");
  printf("\n");
  
  printf("*BOUNDARY\n");
  printf("left, 1, 3, 0\n");
  printf("\n");
      
  printf("*STEP\n");
  printf("\n");
  printf("*EL FILE     # Commands responsible for saving results\n");
  printf("E, S\n");
  printf("*NODE FILE\n");
  printf("U\n");
  printf("\n");
  
  printf("*CLOAD\n");
  
  int dofs = feenox.pde.dofs;
  
  for (size_t i = 0; i < mesh->n_elements; i++) {
    element_t *e = &mesh->element[i];
    // TODO: compare pointers not strings
    if (e->physical_group != NULL && strcmp(e->physical_group->name, "right") == 0) {
      
      // compute the indices of the DOFs to ensamble
      PetscInt *l = feenox_fem_compute_dof_indices(e, dofs);
      PetscInt size = dofs * e->type->nodes;
      PetscScalar values[size];
      petsc_call(VecGetValues(feenox.pde.b, size, l, values));
      
      for (unsigned int j = 0; j < e->type->nodes; j++) {
        for (unsigned int g = 0; g < dofs; g++) {
          if (values[j*dofs + g] != 0) {
            printf("%ld, %d, %g\n", e->node[j]->tag, g+1, values[j*dofs + g]);
          }
        }
      }
    }
  }
  printf("\n");

  printf("*STATIC\n");
  printf("\n");
  printf("*END STEP\n");




  feenox_finalize();

  return 0;
}
