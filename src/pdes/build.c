/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox routines to build elemental objects
 *
 *  Copyright (C) 2015-2022 jeremy theler
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

int feenox_problem_build(void) {
  
#ifdef HAVE_PETSC  
  size_t step = ceil((double)feenox.pde.mesh->n_elements/100.0);
  if (step < 1) {
    step = 1;
  }
  
  // TODO: may we don't need to always empty stuff, i.e. if we decide to re-use the matrices
  // empty global objects
  if (feenox.pde.has_stiffness) {
    petsc_call(MatZeroEntries(feenox.pde.K));
  }  
  if (feenox.pde.has_mass) {
    petsc_call(MatZeroEntries(feenox.pde.M));
  }
  if (feenox.pde.has_jacobian_K) {
    petsc_call(MatZeroEntries(feenox.pde.JK));
  }
  if (feenox.pde.has_jacobian_b) {
    petsc_call(MatZeroEntries(feenox.pde.Jb));
  }
  if (feenox.pde.has_rhs) {
    petsc_call(VecZeroEntries(feenox.pde.b));
  }  
  
  size_t volumetric_elements = 0;
  int ascii_progress_chars = 0;  
  for (size_t i = feenox.pde.first_element; i < feenox.pde.last_element; i++) {

// ------ progress bar ------------------------------------------    
    if (feenox.pde.progress_ascii == PETSC_TRUE && (i % step) == 0) {
      printf(CHAR_PROGRESS_BUILD);  
      fflush(stdout);
      ascii_progress_chars++;
    }
// --------------------------------------------------------------    
    
    // TODO: make a list of bulk elements and another with BC elements
    if (feenox.pde.mesh->element[i].type->dim == feenox.pde.dim) {
      
      // volumetric elements need volumetric builds
      // TODO: process only elements that are local, i.e. the partition number
      //       matches the local rank (not necessarily one to one)
      // TODO: check if we can skip re-building in linear transient and/or
      //       nonlinear BCs only
      feenox_call(feenox_problem_build_element_volumetric(&feenox.pde.mesh->element[i]));
      volumetric_elements++;
      
    } else if (feenox.pde.mesh->element[i].physical_group != NULL) {
      
      // lower-dimensional elements might have BCs (or not)
      bc_t *bc = NULL;
      LL_FOREACH(feenox.pde.mesh->element[i].physical_group->bcs, bc) {
        bc_data_t *bc_data = NULL;
        DL_FOREACH(bc->bc_datums, bc_data) {
          // we only handle natural BCs here, essential BCs are handled in feenox_dirichlet_*()
          if (bc_data->set != NULL && bc_data->type_math != bc_type_math_dirichlet && bc_data->disabled == 0) {
            // and only apply them if the condition holds true (or if there's no condition at all)
            if (bc_data->condition.items == NULL || fabs(feenox_expression_eval(&bc_data->condition)) > 1e-3) {
              feenox_call(feenox_problem_build_element_natural_bc(&feenox.pde.mesh->element[i], bc_data));
            }
          }
        }
      }
    }
  }
  
  if (volumetric_elements == 0) {
    feenox_push_error_message("no volumetric elements found in '%s'", feenox.pde.mesh->file->name);
    return FEENOX_ERROR;
  }

  feenox_call(feenox_problem_build_assemble());

  // TODO: put 100 as a define or as a variable
  if (feenox.pde.progress_ascii == PETSC_TRUE) {  
    if (feenox.n_procs == 1) {
      while (ascii_progress_chars++ < 100) {
        printf(CHAR_PROGRESS_BUILD);
      }
    }
    if (feenox.rank == 0) {
      printf("\n");  
      fflush(stdout);
    }  
  }  
#endif
  
  return FEENOX_OK;
}



int feenox_problem_build_elemental_objects_allocate(element_t *this) {

#ifdef HAVE_PETSC
  
  feenox_call(feenox_problem_build_elemental_objects_free());
      
  feenox.pde.n_local_nodes = this->type->nodes;
  feenox.pde.elemental_size = this->type->nodes * feenox.pde.dofs;
  
  if (feenox.pde.has_stiffness) {
    feenox_check_alloc(feenox.pde.Ki = gsl_matrix_calloc(feenox.pde.elemental_size, feenox.pde.elemental_size));
  }  
  if (feenox.pde.has_mass) {
    feenox_check_alloc(feenox.pde.Mi = gsl_matrix_calloc(feenox.pde.elemental_size, feenox.pde.elemental_size));
  }
  if (feenox.pde.has_jacobian_K) {
    feenox_check_alloc(feenox.pde.JKi = gsl_matrix_calloc(feenox.pde.elemental_size, feenox.pde.elemental_size));
  }
  if (feenox.pde.has_jacobian_b) {
    feenox_check_alloc(feenox.pde.Jbi = gsl_matrix_calloc(feenox.pde.elemental_size, feenox.pde.elemental_size));
  }
  if (feenox.pde.has_rhs) {
    feenox_check_alloc(feenox.pde.bi = gsl_vector_calloc(feenox.pde.elemental_size));
  }
  
#endif  
  
  return FEENOX_OK;

}


int feenox_problem_build_elemental_objects_free(void) {

#ifdef HAVE_PETSC
  
  if (feenox.pde.n_local_nodes != 0 && feenox.pde.elemental_size != 0) {
    gsl_matrix_free(feenox.pde.Ki);
    gsl_matrix_free(feenox.pde.Mi);
    gsl_vector_free(feenox.pde.bi);
  }
  
  feenox.pde.n_local_nodes = 0;
  feenox.pde.elemental_size = 0;
  
#endif  
  return FEENOX_OK;

}


int feenox_problem_build_element_volumetric(element_t *this) {

#ifdef HAVE_PETSC
  
  // TODO: maybe we can relax this requirement
  if (this->physical_group == NULL) {
    feenox_push_error_message("volumetric element %d does not have an associated physical group", this->tag);
    return FEENOX_ERROR;
  }
    
  // if the current element's size is not equal to the previous one, re-allocate
  // TODO: data-oriented approach, these objects in native arrays
  if (feenox.pde.n_local_nodes != this->type->nodes) {
    feenox_call(feenox_problem_build_elemental_objects_allocate(this));
  }
    
  // initialize to zero the elemental objects
  if (feenox.pde.has_stiffness) {
    gsl_matrix_set_zero(feenox.pde.Ki);
  }  
  if (feenox.pde.has_mass) {
    gsl_matrix_set_zero(feenox.pde.Mi);
  }
  if (feenox.pde.has_jacobian_K) {
    gsl_matrix_set_zero(feenox.pde.JKi);
  }
  if (feenox.pde.has_jacobian_b) {
    gsl_matrix_set_zero(feenox.pde.Jbi);
  }
  if (feenox.pde.has_rhs) {
    gsl_vector_set_zero(feenox.pde.bi);
  }
  
  // loop over gauss points to integrate elemental matrices and vectors
  int V = this->type->gauss[feenox.pde.mesh->integration].V;
  for (int v = 0; v < V; v++) {
    // this is a virtual method that depends on the problem type
    feenox_call(feenox.pde.build_element_volumetric_gauss_point(this, v));
  }
 
  // compute the indices of the DOFs to ensamble
  feenox_call(feenox_mesh_compute_dof_indices(this, feenox.pde.mesh));

  if (feenox.pde.has_stiffness)  {
    petsc_call(MatSetValues(feenox.pde.K, feenox.pde.elemental_size, this->l, feenox.pde.elemental_size, this->l, gsl_matrix_ptr(feenox.pde.Ki, 0, 0), ADD_VALUES));
  }  
  if (feenox.pde.has_mass)  {
    petsc_call(MatSetValues(feenox.pde.M, feenox.pde.elemental_size, this->l, feenox.pde.elemental_size, this->l, gsl_matrix_ptr(feenox.pde.Mi, 0, 0), ADD_VALUES));
  }
  if (feenox.pde.has_jacobian_K) {
    petsc_call(MatSetValues(feenox.pde.JK, feenox.pde.elemental_size, this->l, feenox.pde.elemental_size, this->l, gsl_matrix_ptr(feenox.pde.JKi, 0, 0), ADD_VALUES));
  }
  if (feenox.pde.has_jacobian_b) {
    petsc_call(MatSetValues(feenox.pde.Jb, feenox.pde.elemental_size, this->l, feenox.pde.elemental_size, this->l, gsl_matrix_ptr(feenox.pde.Jbi, 0, 0), ADD_VALUES));
  }
  if (feenox.pde.has_rhs) {
    petsc_call(VecSetValues(feenox.pde.b, feenox.pde.elemental_size, this->l, gsl_vector_ptr(feenox.pde.bi, 0), ADD_VALUES));
  }

#endif  
  
  return FEENOX_OK;
}

int feenox_problem_build_element_natural_bc(element_t *this, bc_data_t *bc_data) {

#ifdef HAVE_PETSC
  // total number of gauss points
  unsigned int V = this->type->gauss[feenox.pde.mesh->integration].V;

  if (feenox.pde.n_local_nodes != this->type->nodes) {
    feenox_call(feenox_problem_build_elemental_objects_allocate(this));
  }
  
  if (feenox.pde.Nb == NULL) {
    feenox_check_alloc(feenox.pde.Nb = gsl_vector_calloc(feenox.pde.dofs));
  } else {
    gsl_vector_set_zero(feenox.pde.Nb);
  }

  if (feenox.pde.has_rhs)   {
    gsl_vector_set_zero(feenox.pde.bi);
  }  
  if (bc_data->fills_matrix) {
    gsl_matrix_set_zero(feenox.pde.Ki);
  }
  if (feenox.pde.has_jacobian_b) {
    gsl_matrix_set_zero(feenox.pde.Jbi);
  }

  for (unsigned int v = 0; v < V; v++) {
    feenox_call(bc_data->set(this, bc_data, v));
  }
  
  feenox_call(feenox_mesh_compute_dof_indices(this, feenox.pde.mesh));
  if (feenox.pde.has_rhs == PETSC_TRUE) {
    petsc_call(VecSetValues(feenox.pde.b, feenox.pde.elemental_size, this->l, gsl_vector_ptr(feenox.pde.bi, 0), ADD_VALUES));
  }
  if (bc_data->fills_matrix) {
    petsc_call(MatSetValues(feenox.pde.K, feenox.pde.elemental_size, this->l,
                                          feenox.pde.elemental_size, this->l, gsl_matrix_ptr(feenox.pde.Ki, 0, 0), ADD_VALUES));
  }
/*
  if (feenox.pde.has_jacobian_K) {
    petsc_call(MatSetValues(feenox.pde.JK, feenox.pde.elemental_size, this->l, feenox.pde.elemental_size, this->l, gsl_matrix_ptr(feenox.pde.JKi, 0, 0), ADD_VALUES));
  } 
*/
  if (feenox.pde.has_jacobian_b) {
    petsc_call(MatSetValues(feenox.pde.Jb, feenox.pde.elemental_size, this->l, feenox.pde.elemental_size, this->l, gsl_matrix_ptr(feenox.pde.Jbi, 0, 0), ADD_VALUES));
  }

#endif
  return FEENOX_OK;
  
}

/*
inline double fino_compute_r_for_axisymmetric(element_t *element, int v) {

  double r_for_axisymmetric = 1.0;
  
  if (fino.problem_kind == problem_kind_axisymmetric) {
    if (element->x == NULL || element->x[v] == NULL) {
      mesh_compute_x_at_gauss(element, v, fino.mesh->integration);
    }
  
    if (fino.symmetry_axis == symmetry_axis_y) {
      if ((r_for_axisymmetric = element->x[v][0]) < ZERO) {
        feenox_push_error_message("axisymmetric problems with respect to y cannot have nodes with x <~ 0");
        feenox_runtime_error();
      }
    } else if (fino.symmetry_axis == symmetry_axis_x) {
      if ((r_for_axisymmetric = element->x[v][1]) < ZERO) {
        feenox_push_error_message("axisymmetric problems with respect to x cannot have nodes with y <~ 0");
        feenox_runtime_error();
      }
    }
  }
  
  return r_for_axisymmetric;
}

*/


int feenox_problem_build_assemble(void) {
  
#ifdef HAVE_PETSC

  // TODOD: which is better?
/*  
  petsc_call(MatAssemblyBegin(feenox.K, MAT_FINAL_ASSEMBLY));
  petsc_call(MatAssemblyEnd(feenox.K, MAT_FINAL_ASSEMBLY));

  petsc_call(VecAssemblyBegin(feenox.phi));
  petsc_call(VecAssemblyEnd(feenox.phi));
  
  if (feenox.M != NULL) {
    petsc_call(MatAssemblyBegin(feenox.M, MAT_FINAL_ASSEMBLY));
    petsc_call(MatAssemblyEnd(feenox.M, MAT_FINAL_ASSEMBLY));
  }
  if (feenox.J != NULL) {
    petsc_call(VecAssemblyBegin(feenox.b));
    petsc_call(VecAssemblyEnd(feenox.b));
  }
*/
  if (feenox.pde.phi != NULL) {
    petsc_call(VecAssemblyBegin(feenox.pde.phi));
  }  
  if (feenox.pde.b != NULL) {
    petsc_call(VecAssemblyBegin(feenox.pde.b));
  }
  if (feenox.pde.K != NULL) {
    petsc_call(MatAssemblyBegin(feenox.pde.K, MAT_FINAL_ASSEMBLY));
  }  
  if (feenox.pde.JK != NULL) {
    petsc_call(MatAssemblyBegin(feenox.pde.JK, MAT_FINAL_ASSEMBLY));
  }  
  if (feenox.pde.Jb != NULL) {
    petsc_call(MatAssemblyBegin(feenox.pde.Jb, MAT_FINAL_ASSEMBLY));
  }  
  if (feenox.pde.M != NULL) {
    petsc_call(MatAssemblyBegin(feenox.pde.M, MAT_FINAL_ASSEMBLY));
  }  


  if (feenox.pde.phi != NULL) {
    petsc_call(VecAssemblyEnd(feenox.pde.phi));
  }  
  if (feenox.pde.b != NULL) {
    petsc_call(VecAssemblyEnd(feenox.pde.b));
  }  
  if (feenox.pde.K != NULL) {
    petsc_call(MatAssemblyEnd(feenox.pde.K, MAT_FINAL_ASSEMBLY));
  }  
  if (feenox.pde.JK != NULL) {
    petsc_call(MatAssemblyEnd(feenox.pde.JK, MAT_FINAL_ASSEMBLY));
  }  
  if (feenox.pde.Jb != NULL) {
    petsc_call(MatAssemblyEnd(feenox.pde.Jb, MAT_FINAL_ASSEMBLY));
  }  
  if (feenox.pde.M != NULL) {
    petsc_call(MatAssemblyEnd(feenox.pde.M, MAT_FINAL_ASSEMBLY));
  }

#endif  
  return FEENOX_OK;
}
