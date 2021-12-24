/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox function for computing reactions
 *
 *  Copyright (C) 2016--2021 jeremy theler
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
#include "feenox.h"
extern feenox_t feenox;

int feenox_instruction_reaction(void *arg) {

#ifdef HAVE_PETSC
  reaction_t *reaction = (reaction_t *)arg;
  
  if (reaction->vector != NULL && reaction->vector->initialized == 0) {
    feenox_vector_init(reaction->vector, 0);
  }
  
  // order == 1 for moment
  unsigned int g = 0;
  Vec arm[] = {NULL, NULL, NULL};
  double x0[] = {0, 0, 0};
  PetscInt *node_index = NULL;
  if (reaction->order == 1) {
    feenox_check_alloc(node_index = calloc(feenox.pde.size_local, sizeof(PetscInt)));
    for (g = 0; g < feenox.pde.dofs; g++) {
      if (reaction->x0[g].items != NULL) {
        // if an explcit coordinate was given, use it
        x0[g] = feenox_expression_eval(&reaction->x0[g]);
      } else {
        // use the COG
        if (reaction->physical_group->volume == 0) {
          feenox_physical_group_compute_volume(reaction->physical_group, feenox.pde.mesh);
        }
        x0[g] = reaction->physical_group->cog[g];
      }
      petsc_call(MatCreateVecs(feenox.pde.K, PETSC_NULL, &arm[g]));
    }
  }
  
  PetscInt *row[] = {NULL, NULL, NULL};
  for (g = 0; g < feenox.pde.dofs; g++) {
    feenox_check_alloc(row[g] = calloc(feenox.pde.size_local, sizeof(PetscInt)));
  }  
  
  // get which nodes need to be taken into account
  size_t i = 0;
  size_t j = 0;
  size_t k = 0;
  unsigned int j_local = 0;
  PetscBool add = PETSC_FALSE;
  for (j = feenox.pde.first_node; j < feenox.pde.last_node; j++) {
    add = PETSC_FALSE;
    // this could have been done by sweeping the assigned elements and checking out if the nodes belong to the entity
    for (i = 0; add == PETSC_FALSE && i < reaction->physical_group->n_elements; i++) {
      element_t *element = &feenox.pde.mesh->element[reaction->physical_group->element[i]];
      for (j_local = 0; add == PETSC_FALSE && j_local < element->type->nodes; j_local++) {
        if (element->node[j_local]->index_mesh == j) {
          add = PETSC_TRUE;
          for (g = 0; g < feenox.pde.dofs; g++) {
            row[g][k] = feenox.pde.mesh->node[j].index_dof[g];
          }
          if (reaction->order == 1) {
            node_index[k] = j;
          }
          k++;
        }
      }
    }
  }

  // the IS of the columns is the same for all the DOFs
  IS set_cols = NULL;
  petsc_call(ISCreateStride(PETSC_COMM_WORLD, feenox.pde.size_local, feenox.pde.first_row, 1, &set_cols));

  IS set_rows[] = {NULL, NULL, NULL};
  Mat K_row[] = {NULL, NULL, NULL};
  Vec K_row_u[] = {NULL, NULL, NULL};
  double R[] = {0, 0, 0};
  for (g = 0; g < feenox.pde.dofs; g++) {
    // the IS of the rows depends on the DOF
    petsc_call(ISCreateGeneral(PETSC_COMM_WORLD, k, row[g], PETSC_USE_POINTER, &set_rows[g]));
    petsc_call(MatCreateSubMatrix(feenox.pde.K, set_rows[g], set_cols, MAT_INITIAL_MATRIX, &K_row[g]));
    petsc_call(MatCreateVecs(K_row[g], PETSC_NULL, &K_row_u[g]));
    petsc_call(MatMult(K_row[g], feenox.pde.phi, K_row_u[g]));
    
    if (reaction->order == 1) {
      petsc_call(MatCreateVecs(K_row[g], PETSC_NULL, &arm[g]));
      // TODO: do not set non-local values
      for (j = 0; j < k; j++) {
        petsc_call(VecSetValue(arm[g], j, feenox.pde.mesh->node[node_index[j]].x[g] - x0[g], INSERT_VALUES));
      }
    }
    
    // zeroth-order reactions con be resolved right now
    if (reaction->order == 0) {
      petsc_call(VecSum(K_row_u[g], &R[g]));
    }
  }
  
  // for first-order moments we need to do another loop because moments need all the three forces
  if (reaction->order == 1) {
    for (g = 0; g < feenox.pde.dofs; g++) {
      int a;
      int b;
      switch (g) {
        case 0:
          a = 1;
          b = 2;
        break;
        case 1:
          a = 0;
          b = 2;
        break;
        case 2:
          a = 0;
          b = 1;
        break;
      }

      PetscScalar Fa_db = 0;
      petsc_call(VecDot(K_row_u[a], arm[b], &Fa_db));
      PetscScalar Fb_da = 0;
      petsc_call(VecDot(K_row_u[b], arm[a], &Fb_da));
      R[g] = Fa_db + Fb_da;
    }  
  }
  
  for (g = 0; g < feenox.pde.dofs; g++) {
    if (feenox.pde.dofs == 1) {
      feenox_var_value(reaction->scalar) = R[0];
    } else {
      gsl_vector_set(reaction->vector->value, g, R[g]);
    }
    
    petsc_call(VecDestroy(&K_row_u[g]));
    petsc_call(MatDestroy(&K_row[g]));
    petsc_call(ISDestroy(&set_rows[g]));
    if (reaction->order == 1) {
      petsc_call(VecDestroy(&arm[g]));
    }
    
    feenox_free(row[g]);
  }

  petsc_call(ISDestroy(&set_cols));
  if (reaction->order == 1) {
    feenox_free(node_index);
  }
    

  // TODO
/*  
  if (fino.problem_kind == problem_kind_axisymmetric) {
    if (fino.symmetry_axis == symmetry_axis_y) {
      gsl_vector_set(reaction->vector->value, 0, 0);
      gsl_vector_set(reaction->vector->value, 1, 2*M_PI*R[1]);
    } else if (fino.symmetry_axis == symmetry_axis_x) {
      gsl_vector_set(reaction->vector->value, 0, 2*M_PI*R[0]);
      gsl_vector_set(reaction->vector->value, 1, 0);
    }
  }
 */
  
#endif
  return FEENOX_OK;
}
