/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's problem-solving routines
 *
 *  Copyright (C) 2021--2022 jeremy theler
 *
 *  This file is part of FeenoX <https://www.seamplex.com/feenox>.
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

int feenox_instruction_solve_problem(void *arg) {
  
#ifdef HAVE_PETSC

  // initialize only if we did not initialize before
  if (feenox.pde.spatial_unknowns == 0) {
    // if there is no explicit mesh, use the main one
    if (feenox.pde.mesh == NULL && (feenox.pde.mesh = feenox.mesh.mesh_main) == NULL) {
      feenox_push_error_message("unknown mesh (no READ_MESH keyword)");
      return FEENOX_ERROR;
    }

    feenox_call(feenox.pde.init_runtime_particular());
    if (feenox.pde.solve == NULL) {
      feenox_push_error_message("internal mismatch, undefined solver");
      return FEENOX_ERROR;
    }
    feenox_call(feenox_problem_init_runtime_general());
  }

  // mongocho
/*  
  if (feenox_var_value(feenox_special_var(in_static)) && feenox.pde.initial_guess != NULL) {
    // TODO: what about SLEPc?
    feenox_call(feenox_function_to_phi(feenox.pde.initial_guess, feenox.pde.phi));
  } 
 */
  
  // solve the problem with this per-mathematics virtual method
  // (which in turn calls a per-physics matrix & vector builds)
  feenox_call(feenox.pde.solve());
  
  // TODO: how to do this in parallel?
  feenox_call(feenox_problem_phi_to_solution((feenox.pde.nev == 0) ? feenox.pde.phi : feenox.pde.eigenvector[0]));
  
  if (feenox.pde.solve_post != NULL) {
    feenox_call(feenox.pde.solve_post());
  }
  
#endif  
  return FEENOX_OK;
}

#ifdef HAVE_PETSC
int feenox_function_to_phi(function_t *function, Vec phi) {
  // fill the petsc vector with the data from the initial condition function of space
  size_t j = 0;
  for (j = feenox.pde.first_node; j < feenox.pde.last_node; j++) {
    petsc_call(VecSetValue(phi, feenox.pde.mesh->node[j].index_dof[0], feenox_function_eval(function, feenox.pde.mesh->node[j].x), INSERT_VALUES));
  }
  
  // TODO: add prefixes to allow -vec_view
  petsc_call(VecAssemblyBegin(phi));
  petsc_call(VecAssemblyEnd(phi));
  
  return FEENOX_OK;
}


int feenox_problem_phi_to_solution(Vec phi) {

  VecScatter         vscat;
  Vec                phi_full;
  PetscScalar        *phi_full_array;
  PetscInt           nlocal;

  // TODO: if nprocs == 1 then we already have the full vector
  petsc_call(VecScatterCreateToAll(phi, &vscat, &phi_full));
  petsc_call(VecScatterBegin(vscat, phi, phi_full, INSERT_VALUES, SCATTER_FORWARD));
  petsc_call(VecScatterEnd(vscat, phi, phi_full, INSERT_VALUES, SCATTER_FORWARD));

  petsc_call(VecGetLocalSize(phi_full, &nlocal));
  if (nlocal != feenox.pde.size_global) {
    feenox_push_error_message("internal check of problem size with scatter failed, %d != %d\n", nlocal, feenox.pde.size_global);
    return FEENOX_OK;
  }
  petsc_call(VecGetArray(phi_full, &phi_full_array));
  
  // make up G functions with the solution
  size_t j = 0;
  for (j = 0; j < feenox.pde.spatial_unknowns; j++) {
    
    if (feenox.pde.mesh->node[j].phi == NULL) {
      feenox_check_alloc(feenox.pde.mesh->node[j].phi = calloc(feenox.pde.dofs, sizeof(double)));
    }
    
    for (int g = 0; g < feenox.pde.dofs; g++) {
      feenox.pde.mesh->node[j].phi[g] = phi_full_array[feenox.pde.mesh->node[j].index_dof[g]];

      // if we are not in rough mode we fill the solution here
      // because it is easier, in rough mode we need to
      // iterate over the elements instead of over the nodes
      if (feenox.pde.rough == 0) {
        feenox.pde.solution[g]->data_value[j] = feenox.pde.mesh->node[j].phi[g];
      }
        
      if (feenox.pde.nev > 1) {
        PetscScalar xi = 0;
        for (int i = 0; i < feenox.pde.nev; i++) {
          // the values already have the excitation factor
          PetscInt index = feenox.pde.mesh->node[j].index_dof[g];
          petsc_call(VecGetValues(feenox.pde.eigenvector[i], 1, &index, &xi));
          feenox.pde.mode[g][i]->data_value[j] = xi;
          feenox_vector_set(feenox.pde.vectors.phi[i], j, xi);
        }
      }
    }
  }

  petsc_call(VecRestoreArray(phi_full, &phi_full_array));
  petsc_call(VecDestroy(&phi_full));
  petsc_call(VecScatterDestroy(&vscat));
    
    
  if (feenox.pde.rough) {
    node_t *node;  
    // in rough mode we need to iterate over the elements first and then over the nodes
    // TODO: see if the order of the loops is the optimal one
    unsigned int g = 0;
    size_t i = 0;
    size_t j = 0;
    for (g = 0; g < feenox.pde.dofs; g++) {
      for (i = 0; i < feenox.pde.mesh_rough->n_elements; i++) {
        for (j = 0; j < feenox.pde.mesh_rough->element[i].type->nodes; j++) {
          node = feenox.pde.mesh_rough->element[i].node[j];  
          feenox.pde.solution[g]->data_value[node->index_mesh] = node->phi[g];  
        }
      }
    }
  }

  if (feenox.pde.compute_gradients) {
    feenox_call(feenox_problem_gradient_compute());
  }  
  
  return FEENOX_OK;
}
#endif

int feenox_problem_init_default(void) {
  return FEENOX_OK;
}
