/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox routines to handle dirichlet BCs
 *
 *  Copyright (C) 2015-2023 jeremy theler
 *
 *  This file is part of Feenox <https://www.seamplex.com/feenox>.
 *
 *  feenox is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Feenox is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Feenox.  If not, see <http://www.gnu.org/licenses/>.
 *------------------- ------------  ----    --------  --     -       -         -
 */
#include "feenox.h"

int feenox_problem_dirichlet_add(size_t index, double value) {

#ifdef HAVE_PETSC
  feenox.pde.dirichlet_indexes[feenox.pde.dirichlet_k] = index;
  feenox.pde.dirichlet_values[feenox.pde.dirichlet_k] = value;
  feenox.pde.dirichlet_k++;
#endif
  
  return FEENOX_OK;
}

int feenox_problem_multifreedom_add(size_t j_global, double *coefficients) {

#ifdef HAVE_PETSC
  PetscInt *l = NULL;
  feenox_check_alloc(l = calloc(feenox.pde.dofs, sizeof(PetscInt)));
  
  gsl_matrix *c = NULL;
  feenox_check_alloc(c = gsl_matrix_alloc(1, feenox.pde.dofs));

  for (int g = 0; g < feenox.pde.dofs; g++) {
    l[g] = feenox.pde.mesh->node[j_global].index_dof[g];
    gsl_matrix_set(c, 0, g, coefficients[g]);
  }
  
  feenox.pde.multifreedom_indexes[feenox.pde.multifreedom_k] = l;
  feenox.pde.multifreedom_coefficients[feenox.pde.multifreedom_k] = c;
  feenox.pde.multifreedom_k++;
#endif
  
  return FEENOX_OK;
}

// evaluates the dirichlet BCs and stores them in the internal representation
int feenox_problem_dirichlet_eval(void) {

#ifdef HAVE_PETSC

  size_t n_bcs = 0;
  if (feenox.pde.dirichlet_rows == 0) {
    // on the first iteration, assume that all the nodes need a dirichlet BC
    // then we trim the extra space to save memory
    // caution! in small problems we might end up with more bcs than nodes 
    // because there might nodes counted more than once because we loop over elements

    // TODO: allow the user to provide a factor at runtime
    n_bcs = 1.2*feenox.pde.dofs * (feenox.pde.last_node - feenox.pde.first_node);    

    feenox_check_alloc(feenox.pde.dirichlet_indexes = calloc(n_bcs, sizeof(PetscInt)));
    feenox_check_alloc(feenox.pde.dirichlet_values = calloc(n_bcs, sizeof(PetscScalar)));
    feenox_check_alloc(feenox.pde.dirichlet_derivatives = calloc(n_bcs, sizeof(PetscScalar)));
    
    feenox_check_alloc(feenox.pde.multifreedom_indexes = calloc(n_bcs, sizeof(PetscInt *)));
    feenox_check_alloc(feenox.pde.multifreedom_coefficients = calloc(n_bcs, sizeof(PetscScalar *)));

  } else {
    // if we are here then we know more or less the number of BCs we need
    n_bcs = feenox.pde.dirichlet_rows;
  }  
  
  feenox.pde.dirichlet_k = 0;
  feenox.pde.multifreedom_k = 0;
  for (size_t j_global = feenox.pde.first_node; j_global < feenox.pde.last_node; j_global++) {
    // TODO: optimize these ugly nested loops
    // maybe if we went the other way and looped over the elements first?
    // merge_sort + remove duplicates?
    physical_group_t *last_physical_group = NULL;
    element_ll_t *element_list = NULL;
    LL_FOREACH(feenox.pde.mesh->node[j_global].element_list, element_list) {
      element_t *element = element_list->element;
      if (element != NULL && element->type->dim < feenox.pde.dim && element->physical_group != NULL && element->physical_group != last_physical_group) {
        last_physical_group = element->physical_group;
        bc_t *bc = NULL;
        LL_FOREACH(element->physical_group->bcs, bc) {
          bc_data_t *bc_data = NULL;
          DL_FOREACH(bc->bc_datums, bc_data) {
            // if there is a condition we evaluate it now
            if (bc_data->set_essential != NULL && (bc_data->condition.items == NULL || fabs(feenox_expression_eval(&bc_data->condition)) > DEFAULT_CONDITION_THRESHOLD)) {
              
              // TODO: normal-dependent
              if (bc_data->space_dependent) {
                feenox_fem_update_coord_vars(feenox.pde.mesh->node[j_global].x);
              }
              
              // TODO: for multi-freedom high-order nodes end up with a different penalty weight
              // TODO: pass the node instead of the index
              feenox_call(bc_data->set_essential(bc_data, element, j_global));
            }  
          }
        }
      }
    }
  }

  // now we know how many rows we need to change
  if (feenox.pde.dirichlet_rows != feenox.pde.dirichlet_k) {
    feenox.pde.dirichlet_rows = feenox.pde.dirichlet_k;
    
    // if k == 0 this like freeing
    feenox_check_alloc(feenox.pde.dirichlet_indexes = realloc(feenox.pde.dirichlet_indexes, feenox.pde.dirichlet_rows * sizeof(PetscInt)));
    feenox_check_alloc(feenox.pde.dirichlet_values = realloc(feenox.pde.dirichlet_values, feenox.pde.dirichlet_rows * sizeof(PetscScalar)));
    feenox_check_alloc(feenox.pde.dirichlet_derivatives = realloc(feenox.pde.dirichlet_derivatives, feenox.pde.dirichlet_rows * sizeof(PetscScalar)));
  }
  if (feenox.pde.multifreedom_nodes != feenox.pde.multifreedom_k) {
    feenox.pde.multifreedom_nodes = feenox.pde.multifreedom_k;
    feenox_check_alloc(feenox.pde.multifreedom_indexes = realloc(feenox.pde.multifreedom_indexes, feenox.pde.multifreedom_nodes * sizeof(PetscInt *)));
    feenox_check_alloc(feenox.pde.multifreedom_coefficients = realloc(feenox.pde.multifreedom_coefficients, feenox.pde.multifreedom_nodes * sizeof(gsl_matrix *)));
  }

#endif
  return FEENOX_OK;
}

#ifdef HAVE_PETSC
// K - stiffness matrix: needs one (alpha) in the diagonal, value (alpha*value) in b and keep symmetry
// b - RHS: needs to be updated when modifying K
// this is called only when solving an explicit KSP (or EPS) so 
// it takes K and b and writes K_bc and b_bc
int feenox_problem_dirichlet_set_K(void) {

  if (feenox.pde.dirichlet_scale == 0)
  {
    feenox_problem_dirichlet_compute_scale();
  }
  
  // sometimes there are hanging nodes with no associated volumes
  // this can trigger zeros on the diagonal and MatZeroRowsColumns complains
  if (feenox.pde.hanging_nodes) {
    int found = 0;
    Vec vec_diagonal = NULL;
    petsc_call(VecDuplicate(feenox.pde.phi, &vec_diagonal));
    petsc_call(MatGetDiagonal(feenox.pde.K, vec_diagonal));
    const PetscScalar *array_diagonal;
    petsc_call(VecGetArrayRead(vec_diagonal, &array_diagonal));
    PetscInt size = feenox.pde.last_row - feenox.pde.first_row;

    petsc_call(MatSetOption(feenox.pde.K, MAT_NEW_NONZERO_ALLOCATION_ERR, PETSC_FALSE));
    for (PetscInt j = 0; j < size; j++) {
      if (array_diagonal[j] == 0) {
        found = 1;
        if (feenox.pde.hanging_nodes == hanging_nodes_detect) {
          size_t index = j / feenox.pde.dofs;
          if (j % index == 0) {
            // only report the first dof where the hanging node is found
            feenox_push_error_message("%ld", feenox.pde.mesh->node[index].tag);
          }  
        } else {
          PetscInt row = feenox.pde.first_row + j;
          petsc_call(MatSetValue(feenox.pde.K, row, row, feenox.pde.dirichlet_scale, INSERT_VALUES));
        }  
      }  
    }
    petsc_call(MatSetOption(feenox.pde.K, MAT_NEW_NONZERO_ALLOCATION_ERR, PETSC_TRUE));
  
    petsc_call(VecRestoreArrayRead(vec_diagonal, &array_diagonal));
    petsc_call(VecDestroy(&vec_diagonal));
 
    int found_global = 0;
    MPI_Allreduce(&found, &found_global, 1, MPIU_INT, MPIU_SUM, PETSC_COMM_WORLD);
    if (found_global) {
      if (feenox.pde.hanging_nodes == hanging_nodes_detect) {
        feenox_push_error_message("hanging nodes detected:");
        return FEENOX_ERROR;
      } else {
        petsc_call(MatAssemblyBegin(feenox.pde.K, MAT_FINAL_ASSEMBLY));
        petsc_call(MatAssemblyEnd(feenox.pde.K, MAT_FINAL_ASSEMBLY));
      }
    }  
  }  

  if (feenox.pde.K_bc == NULL) {
    petsc_call(MatDuplicate(feenox.pde.K, MAT_COPY_VALUES, &feenox.pde.K_bc));
    petsc_call(PetscObjectSetName((PetscObject)(feenox.pde.K_bc), "K_bc"));
  } else {
    petsc_call(MatCopy(feenox.pde.K, feenox.pde.K_bc, SAME_NONZERO_PATTERN));
  } 
  

  // add multifreedom constrains using the penalty method (before setting the rows to zero)
  // TODO: multi-freedom constrains with lagrange
  if (feenox.pde.multifreedom_nodes > 0) {
    gsl_matrix *P = NULL;
    feenox_check_alloc(P = gsl_matrix_calloc(feenox.pde.dofs, feenox.pde.dofs));
    for (unsigned int k = 0; k < feenox.pde.multifreedom_nodes; k++) {
      gsl_matrix *c = feenox.pde.multifreedom_coefficients[k];
      PetscInt *l = feenox.pde.multifreedom_indexes[k];

      gsl_matrix_set_zero(P);
      feenox_call(feenox_blas_BtB(c, feenox_var_value(feenox.pde.vars.penalty_weight), P));
      petsc_call(MatSetValues(feenox.pde.K_bc, feenox.pde.dofs, l, feenox.pde.dofs, l, gsl_matrix_ptr(P, 0, 0), ADD_VALUES));
    }
    gsl_matrix_free(P);
  }

  int has_multidof_global = 0;
  int has_multidof_local = (feenox.pde.multifreedom_nodes > 0);
  MPI_Allreduce(&has_multidof_local, &has_multidof_global, 1, MPIU_INT, MPIU_SUM, PETSC_COMM_WORLD);

  if (has_multidof_global) {
    petsc_call(MatAssemblyBegin(feenox.pde.K_bc, MAT_FINAL_ASSEMBLY));
    petsc_call(MatAssemblyEnd(feenox.pde.K_bc, MAT_FINAL_ASSEMBLY));
  }
  
  // this vector holds the dirichlet values and is used to re-write
  // the actual rhs vector b in order to keep the symmetry of K
  Vec rhs = NULL;
  if (feenox.pde.b != NULL) {
    if (feenox.pde.b_bc == NULL) {
      petsc_call(VecDuplicate(feenox.pde.b, &feenox.pde.b_bc));
      petsc_call(PetscObjectSetName((PetscObject)(feenox.pde.b_bc), "b_bc"));
    }
    petsc_call(VecCopy(feenox.pde.b, feenox.pde.b_bc));
    petsc_call(MatCreateVecs(feenox.pde.K, NULL, &rhs));
    petsc_call(VecSetValues(rhs, feenox.pde.dirichlet_rows, feenox.pde.dirichlet_indexes, feenox.pde.dirichlet_values, INSERT_VALUES));
  }  

  if (feenox.pde.symmetric_K) {
    petsc_call(MatZeroRowsColumns(feenox.pde.K_bc, feenox.pde.dirichlet_rows, feenox.pde.dirichlet_indexes, feenox.pde.dirichlet_scale, rhs, feenox.pde.b_bc));
  } else {  
    petsc_call(MatZeroRows(feenox.pde.K_bc, feenox.pde.dirichlet_rows, feenox.pde.dirichlet_indexes, feenox.pde.dirichlet_scale, rhs, feenox.pde.b_bc));
  }
  
  if (rhs != NULL) {
    petsc_call(VecDestroy(&rhs));
  }
  
  return FEENOX_OK;
}


// M - mass matrix: needs a zero in the diagonal and the same symmetry scheme that K
// this is called only when solving an EPS so it takes
// M and writes M_bc
int feenox_problem_dirichlet_set_M(void) {

  if (feenox.pde.M_bc == NULL) {
    petsc_call(MatDuplicate(feenox.pde.M, MAT_COPY_VALUES, &feenox.pde.M_bc));
    petsc_call(PetscObjectSetName((PetscObject)(feenox.pde.M_bc), "M_bc"));
  } else {
    petsc_call(MatCopy(feenox.pde.M, feenox.pde.M_bc, SAME_NONZERO_PATTERN));
  }
  
  // the mass matrix is like the stiffness one but with zero instead of one
  if (feenox.pde.symmetric_M) {
    petsc_call(MatZeroRowsColumns(feenox.pde.M_bc, feenox.pde.dirichlet_rows, feenox.pde.dirichlet_indexes, 0.0, NULL, NULL));
  } else {  
    petsc_call(MatZeroRows(feenox.pde.M_bc, feenox.pde.dirichlet_rows, feenox.pde.dirichlet_indexes, 0.0, NULL, NULL));  
  }  

//  petsc_call(MatZeroRows(feenox.pde.M_bc, feenox.pde.dirichlet_rows, feenox.pde.dirichlet_indexes, 1, NULL, NULL));  

  return FEENOX_OK;
}


// J - Jacobian matrix: same as K but without the RHS vector
int feenox_problem_dirichlet_set_J(Mat J) {

  if (feenox.pde.dirichlet_scale == 0)
  {
    feenox_problem_dirichlet_compute_scale();
  }
  
  // the jacobian is exactly one (actually alpha) for the dirichlet values and zero otherwise without keeping symmetry
  petsc_call(MatZeroRowsColumns(J, feenox.pde.dirichlet_rows, feenox.pde.dirichlet_indexes, feenox.pde.dirichlet_scale, NULL, NULL));

  return FEENOX_OK;
}

// phi - solution: the BC values are set directly in order to be used as a initial condition or guess
int feenox_problem_dirichlet_set_phi(Vec phi) {
  
  petsc_call(VecSetValues(phi, feenox.pde.dirichlet_rows, feenox.pde.dirichlet_indexes, feenox.pde.dirichlet_values, INSERT_VALUES));
  return FEENOX_OK;
  
}

// phi - solution: the values at the BC DOFs are zeroed
int feenox_problem_dirichlet_set_phi_dot(Vec phi_dot) {

  petsc_call(VecSetValues(phi_dot, feenox.pde.dirichlet_rows, feenox.pde.dirichlet_indexes, feenox.pde.dirichlet_derivatives, INSERT_VALUES));
  return FEENOX_OK;
}

// r - residual: the BC indexes are set to the difference between the value and the solution, scaled by alpha
int feenox_problem_dirichlet_set_r(Vec r, Vec phi) {

  size_t k;
  
  // TODO: put this array somewhere and avoid allocating/freeing each time
  PetscScalar *diff;
  feenox_check_alloc(diff = calloc(feenox.pde.dirichlet_rows, sizeof(PetscScalar)));
  petsc_call(VecGetValues(phi, feenox.pde.dirichlet_rows, feenox.pde.dirichlet_indexes, diff));
  
  if (feenox.pde.dirichlet_scale == 0)
  {
    feenox_problem_dirichlet_compute_scale();
  }
  
  for (k = 0; k < feenox.pde.dirichlet_rows; k++) {
    diff[k] -= feenox.pde.dirichlet_values[k];
    diff[k] *= feenox.pde.dirichlet_scale;
  }
  
  
  petsc_call(VecSetValues(r, feenox.pde.dirichlet_rows, feenox.pde.dirichlet_indexes, diff, INSERT_VALUES));
  feenox_free(diff);

  return FEENOX_OK;
}

// this is alpha in https://scicomp.stackexchange.com/questions/3298/appropriate-space-for-weak-solutions-to-an-elliptical-pde-with-mixed-inhomogeneo/3300#3300
int feenox_problem_dirichlet_compute_scale(void) {
  
  if (feenox.pde.dirichlet_scale_fraction != 0) {
    PetscScalar trace = 0;
    petsc_call(MatGetTrace(feenox.pde.K, &trace));
    feenox.pde.dirichlet_scale = feenox.pde.dirichlet_scale_fraction * trace/feenox.pde.size_global;
  } else if (feenox.pde.dirichlet_scale == 0) {
    feenox.pde.dirichlet_scale = 1.0;
  }  
  
  return FEENOX_OK;
}

#endif
