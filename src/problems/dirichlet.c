#include "feenox.h"
extern feenox_t feenox;

// evaluates the dirichlet BCs and stores them in the internal representation
int feenox_dirichlet_eval(void) {

#ifdef HAVE_PETSC

  size_t n_bcs = 0;
  if (feenox.pde.n_dirichlet_rows == 0) {
    // on the first iteration, assume that all the nodes need a dirichlet BC
    // then we trim the extra space to save memory
    // TODO: allow the user to provide a factor at runtime
    n_bcs = feenox.pde.dofs * (feenox.pde.last_node - feenox.pde.first_node);    

    feenox_check_alloc(feenox.pde.dirichlet_indexes = calloc(n_bcs, sizeof(PetscInt)));
    feenox_check_alloc(feenox.pde.dirichlet_values = calloc(n_bcs, sizeof(PetscScalar)));
  } else {
    // if we are here then we know more or less the number of BCs we need
    n_bcs = feenox.pde.n_dirichlet_rows;
  }  
  size_t current_size = n_bcs;

  
  bc_t *bc = NULL;
  bc_data_t *bc_data = NULL;
  element_t *element = NULL;
  element_ll_t *element_list = NULL;
  size_t j = 0;
  size_t k = 0;
  for (j = feenox.pde.first_node; j < feenox.pde.last_node; j++) {

    // TODO: high-order nodes end up with a different penalty method!
    // TODO: optimize the way this loop is done to avoid checking
    //       several times for the same group, wasting time and money
    LL_FOREACH(feenox.pde.mesh->node[j].associated_elements, element_list) {
      element = element_list->element;
      if (element != NULL && element->type->dim < feenox.pde.dim && element->physical_group != NULL) {
        LL_FOREACH(element->physical_group->bcs, bc) {
          DL_FOREACH(bc->bc_datums, bc_data) {
            if (bc_data->type_math == bc_type_math_dirichlet) {

//              printf("BC t = %g\n", feenox_special_var_value(t));
              
              // if there is a condition we evaluate it now
              if (bc_data->condition.items == NULL || fabs(feenox_expression_eval(&bc_data->condition)) > 1e-3) {

                if (k >= (current_size-16)) {            
                  current_size += n_bcs;
                  feenox_check_alloc(feenox.pde.dirichlet_indexes = realloc(feenox.pde.dirichlet_indexes, current_size * sizeof(PetscInt)));
                  feenox_check_alloc(feenox.pde.dirichlet_values  = realloc(feenox.pde.dirichlet_values,  current_size * sizeof(PetscScalar)));
                }
                
                // TODO: per-problem virtual methods
                if (bc_data->space_dependent) {
                  feenox_var_value(feenox.mesh.vars.x) = feenox.pde.mesh->node[j].x[0];
                  feenox_var_value(feenox.mesh.vars.y) = feenox.pde.mesh->node[j].x[1];
                  feenox_var_value(feenox.mesh.vars.z) = feenox.pde.mesh->node[j].x[2];
                }  
                feenox_call(feenox_problem_bc_set_thermal_dirichlet(bc_data, j, k));
                k++;
                
              }  
            }  
          }
        }
      }
    }
  }

  // now we know how many rows we need to change
  if (feenox.pde.n_dirichlet_rows != k) {
    feenox.pde.n_dirichlet_rows = k;
    
    // if k == 0 this like freeing
    feenox_check_alloc(feenox.pde.dirichlet_indexes = realloc(feenox.pde.dirichlet_indexes, feenox.pde.n_dirichlet_rows * sizeof(PetscInt)));
    feenox_check_alloc(feenox.pde.dirichlet_values = realloc(feenox.pde.dirichlet_values, feenox.pde.n_dirichlet_rows * sizeof(PetscScalar)));
  }

#endif
  return FEENOX_OK;
}


#ifdef HAVE_PETSC
// K - stiffness matrix: needs a one in the diagonal and the value in b and keep symmetry
// b - RHS: needs to be updated when modifying K
//int feenox_dirichlet_set_K(Mat K, Vec b) {
int feenox_dirichlet_set_K(void) {
  
/*  
  // sometimes there are hanging nodes with no associated volumes
  // this can trigger zeros on the diagonal and MatZeroRowsColumns complains
  // TODO: execute this block only if a special flag is set, i.e. PROBLEM HANDLE_HANGING_NODES
  // TODO: change to MatGetDiagonal
  PetscScalar diag = 0;
  PetscInt k = 0;
  for (k = feenox.pde.first_row; k < feenox.pde.last_row; k++) {
    petsc_call(MatGetValues(K, 1, &k, 1, &k, &diag));
    if (diag == 0) {
      petsc_call(MatSetOption(K, MAT_NEW_NONZERO_ALLOCATION_ERR, PETSC_FALSE));
      petsc_call(MatSetValue(K, k, k, 1.0, INSERT_VALUES));
      petsc_call(MatSetOption(K, MAT_NEW_NONZERO_ALLOCATION_ERR, PETSC_TRUE));
      feenox_call(feenox_assembly());
    }  
  }
*/    
  // this vector holds the dirichlet values and is used to re-write
  // the actual rhs vector b in order to keep the symmetry of K
  Vec rhs;
  petsc_call(MatCreateVecs(feenox.pde.K, NULL, &rhs));
  petsc_call(VecSetValues(rhs, feenox.pde.n_dirichlet_rows, feenox.pde.dirichlet_indexes, feenox.pde.dirichlet_values, INSERT_VALUES));
  // TODO: scale up the diagonal!
  // see alpha in https://scicomp.stackexchange.com/questions/3298/appropriate-space-for-weak-solutions-to-an-elliptical-pde-with-mixed-inhomogeneo/3300#3300
  petsc_call(MatZeroRowsColumns(feenox.pde.K, feenox.pde.n_dirichlet_rows, feenox.pde.dirichlet_indexes, 1.0, rhs, feenox.pde.b));
  petsc_call(VecDestroy(&rhs));
  
  return FEENOX_OK;
}
  
// J - Jacobian matrix: same as K but without the RHS vector
int feenox_dirichlet_set_J(Mat J) {

  // the jacobian is exactly one for the dirichlet values and zero otherwise without keeping symmetry
  petsc_call(MatZeroRowsColumns(J, feenox.pde.n_dirichlet_rows, feenox.pde.dirichlet_indexes, 1.0, NULL, NULL));

  return FEENOX_OK;
}

// phi - solution: the BC values are set directly in order to be used as a initial condition or guess
int feenox_dirichlet_set_phi(Vec phi) {
  
  petsc_call(VecSetValues(phi, feenox.pde.n_dirichlet_rows, feenox.pde.dirichlet_indexes, feenox.pde.dirichlet_values, INSERT_VALUES));
  return FEENOX_OK;
  
}

// phi - solution: the values at the BC DOFs are zeroed
int feenox_dirichlet_set_phi_dot(Vec phi_dot) {

  // TODO: put this array somewhere and avoid allocating/freeing each time
  PetscScalar *zero;
  feenox_check_alloc(zero = calloc(feenox.pde.n_dirichlet_rows, sizeof(PetscScalar)));
  petsc_call(VecSetValues(phi_dot, feenox.pde.n_dirichlet_rows, feenox.pde.dirichlet_indexes, zero, INSERT_VALUES));
  feenox_free(zero);

/*  
//  printf("before\n");
//  VecView(phi_dot, 	PETSC_VIEWER_STDOUT_WORLD);
  
  PetscScalar *derivative;
  feenox_check_alloc(derivative = calloc(feenox.pde.n_dirichlet_rows, sizeof(PetscScalar)));
  derivative[0] = 100;
  derivative[1] = 0;  
  petsc_call(VecSetValues(phi_dot, feenox.pde.n_dirichlet_rows, feenox.pde.dirichlet_indexes, derivative, INSERT_VALUES));
  feenox_free(derivative);
  
//  printf("after\n");
//  VecView(phi_dot, 	PETSC_VIEWER_STDOUT_WORLD);
*/

  return FEENOX_OK;
}

// r - residual: the BC indexes are set to the difference between the value and the solution
int feenox_dirichlet_set_r(Vec r, Vec phi) {

  size_t k;
  
  // TODO: put this array somewhere and avoid allocing/freeing each time
  PetscScalar *diff;
  feenox_check_alloc(diff = calloc(feenox.pde.n_dirichlet_rows, sizeof(PetscScalar)));
  petsc_call(VecGetValues(phi, feenox.pde.n_dirichlet_rows, feenox.pde.dirichlet_indexes, diff));
  for (k = 0; k < feenox.pde.n_dirichlet_rows; k++) {
    diff[k] -= feenox.pde.dirichlet_values[k];
  }
  
  petsc_call(VecSetValues(r, feenox.pde.n_dirichlet_rows, feenox.pde.dirichlet_indexes, diff, INSERT_VALUES));
  feenox_free(diff);

  return FEENOX_OK;
}
#endif
