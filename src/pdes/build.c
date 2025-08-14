/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox routines to build elemental objects
 *
 *  Copyright (C) 2015-2025 Jeremy Theler
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
  if (feenox.pde.has_internal_fluxes) {
    petsc_call(VecZeroEntries(feenox.pde.f));
  }  
  
  size_t volumetric_elements = 0;
  int ascii_progress_chars = 0;  
  for (size_t i = 0; i < feenox.pde.mesh->n_elements; i++) { 
    element_t *e = &feenox.pde.mesh->element[i];
    if (is_element_local(feenox.pde.mesh, e)) {
    
  // ------ progress bar ------------------------------------------    
      if (feenox.pde.progress_ascii == PETSC_TRUE && (i % step) == 0) {
        printf(CHAR_PROGRESS_BUILD);  
        fflush(stdout);
        ascii_progress_chars++;
      }
  // --------------------------------------------------------------    

      // TODO: make a list of bulk elements and another with BC elements
      if (e->type->dim == feenox.pde.dim) {

        // volumetric elements need volumetric builds
        // TODO: process only elements that are local, i.e. the partition number
        //       matches the local rank (not necessarily one to one)
        // TODO: check if we can skip re-building in linear transient and/or
        //       nonlinear BCs only
        feenox_call(feenox_problem_build_element_volumetric(e));
        volumetric_elements++;

      } else if (e->physical_group != NULL) {

        // lower-dimensional elements might have BCs (or not)
        for (int i = 0; i < e->physical_group->n_bcs; i++) {
          bc_data_t *bc_data = NULL;
          DL_FOREACH(e->physical_group->bc[i]->bc_datums, bc_data) {
            // we only handle natural BCs here, essential BCs are handled in feenox_dirichlet_*()
            if (bc_data->set_natural != NULL && bc_data->disabled == 0) {
              // and only apply them if the condition holds true (or if there's no condition at all)
              if (bc_data->condition.items == NULL || fabs(feenox_expression_eval(&bc_data->condition)) > DEFAULT_CONDITION_THRESHOLD) {
                feenox_call(feenox_problem_build_element_natural_bc(e, bc_data));
              }
            }
          }
        }
      }
    }
  }
  
  int valid_local = (volumetric_elements > 0);
  int valid_global = 0;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-value"
  MPI_Allreduce(&valid_local, &valid_global, 1, MPIU_INT, MPIU_SUM, PETSC_COMM_WORLD);
#pragma GCC diagnostic pop
  
  if (valid_global == 0) {
    feenox_push_error_message("no volumetric elements found in '%s'", feenox.pde.mesh->file->name);
    return FEENOX_ERROR;
  }

  // do we need this?
  feenox_call(feenox_fem_elemental_caches_reset());
  // xxx
  
  
  feenox_call(feenox_problem_build_assemble());

  // TODO: put 100 as a define or as a variable
  if (feenox.pde.progress_ascii == PETSC_TRUE) {  
    if (feenox.mpi_size == 1) {
      while (ascii_progress_chars++ < 100) {
        printf(CHAR_PROGRESS_BUILD);
      }
    }
    if (feenox.mpi_rank == 0) {
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
      
  feenox.fem.current_elemental_type = this->type;
  feenox.fem.current_GJ = feenox.pde.dofs * this->type->nodes;

  feenox_check_alloc(feenox.fem.l = calloc(feenox.fem.current_GJ, sizeof(PetscInt)));
  
  if (feenox.pde.has_stiffness) {
    feenox_check_alloc(feenox.fem.Ki = gsl_matrix_calloc(feenox.fem.current_GJ, feenox.fem.current_GJ));
  }  
  if (feenox.pde.has_mass) {
    feenox_check_alloc(feenox.fem.Mi = gsl_matrix_calloc(feenox.fem.current_GJ, feenox.fem.current_GJ));
  }
  if (feenox.pde.has_jacobian_K) {
    feenox_check_alloc(feenox.fem.JKi = gsl_matrix_calloc(feenox.fem.current_GJ, feenox.fem.current_GJ));
  }
  if (feenox.pde.has_jacobian_b) {
    feenox_check_alloc(feenox.fem.Jbi = gsl_matrix_calloc(feenox.fem.current_GJ, feenox.fem.current_GJ));
  }
  if (feenox.pde.has_rhs) {
    feenox_check_alloc(feenox.fem.bi = gsl_vector_calloc(feenox.fem.current_GJ));
    if (feenox.fem.vec_f == NULL) {
      feenox_check_alloc(feenox.fem.vec_f = gsl_vector_calloc(feenox.pde.dofs));
    }
  }
  if (feenox.pde.has_internal_fluxes) {
    feenox_check_alloc(feenox.fem.fi = gsl_vector_calloc(feenox.fem.current_GJ));
    feenox_check_alloc(feenox.fem.phii = gsl_vector_calloc(feenox.fem.current_GJ));
  }

  if (feenox.pde.element_build_allocate_aux != NULL) {
    feenox.pde.element_build_allocate_aux(this->type->nodes);
  }
  
#endif  
  
  return FEENOX_OK;

}

int feenox_problem_build_elemental_objects_free(void) {

#ifdef HAVE_PETSC
  feenox_free(feenox.fem.l);
  gsl_matrix_free(feenox.fem.Ki);
  feenox.fem.Ki = NULL;
  gsl_matrix_free(feenox.fem.Mi);
  feenox.fem.Mi = NULL;
  gsl_matrix_free(feenox.fem.JKi);
  feenox.fem.JKi = NULL;
  gsl_matrix_free(feenox.fem.Jbi);
  feenox.fem.Jbi = NULL;
  gsl_vector_free(feenox.fem.bi);
  feenox.fem.bi = NULL;
  gsl_vector_free(feenox.fem.vec_f);  
  feenox.fem.vec_f = NULL;
  gsl_vector_free(feenox.fem.fi);
  feenox.fem.fi = NULL;
  gsl_vector_free(feenox.fem.phii);
  feenox.fem.phii = NULL;
  
  
  feenox.fem.current_elemental_type = NULL;
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
  // TODO: data-oriented approach, store these objects in native arrays
  if (feenox.fem.current_elemental_type != this->type) {
    feenox_call(feenox_problem_build_elemental_objects_allocate(this));
  }
    
  // initialize to zero the elemental objects
  if (feenox.pde.has_stiffness) {
    gsl_matrix_set_zero(feenox.fem.Ki);
  }  
  if (feenox.pde.has_mass) {
    gsl_matrix_set_zero(feenox.fem.Mi);
  }
  if (feenox.pde.has_jacobian_K) {
    gsl_matrix_set_zero(feenox.fem.JKi);
  }
  if (feenox.pde.has_jacobian_b) {
    gsl_matrix_set_zero(feenox.fem.Jbi);
  }
  if (feenox.pde.has_rhs) {
    gsl_vector_set_zero(feenox.fem.bi);
  }
  if (feenox.pde.has_internal_fluxes) {
    gsl_vector_set_zero(feenox.fem.fi);
  }

  // compute the indices of the DOFs to ensamble
  PetscInt *l = feenox_fem_compute_dof_indices(this, feenox.pde.dofs);
  
  // if the guy has internal forces, we need the solution
  if (feenox.pde.has_internal_fluxes) {
    petsc_call(VecGetValues(feenox.pde.phi_bc, feenox.fem.current_GJ, l, gsl_vector_ptr(feenox.fem.phii, 0)));
  }
  
  // a single call over the element
  // if the specific pde wants, it can loop over gauss point in the call
  // or it can just do some things that are per-element only and then loop below
  if (feenox.pde.element_build_volumetric) {
    feenox_call(feenox.pde.element_build_volumetric(this));
  }
  
  // or, if there's an entry point for gauss points, then we do the loop here
  if (feenox.pde.element_build_volumetric_at_gauss != NULL) {
    int Q = this->type->gauss[feenox.pde.mesh->integration].Q;
    for (unsigned int q = 0; q < Q; q++) {
      // TODO: hardcode the name of the method to allow inlining with LTO
      feenox_call(feenox.pde.element_build_volumetric_at_gauss(this, q));
    }
  }
  
/*  
  printf("result\n");
  printf("Ki\n");
  feenox_debug_print_gsl_matrix(feenox.fem.Ki, stdout);
  printf("JKi\n");
  feenox_debug_print_gsl_matrix(feenox.fem.JKi, stdout);
  printf("fi\n");
  feenox_debug_print_gsl_vector(feenox.fem.fi, stdout);
  printf("bi\n");
  feenox_debug_print_gsl_vector(feenox.fem.bi, stdout);
*/
 
  if (feenox.pde.has_stiffness)  {
    petsc_call(MatSetValues(feenox.pde.K, feenox.fem.current_GJ, l, feenox.fem.current_GJ, l, gsl_matrix_ptr(feenox.fem.Ki, 0, 0), ADD_VALUES));
  }  
  if (feenox.pde.has_mass)  {
    petsc_call(MatSetValues(feenox.pde.M, feenox.fem.current_GJ, l, feenox.fem.current_GJ, l, gsl_matrix_ptr(feenox.fem.Mi, 0, 0), ADD_VALUES));
  }
  if (feenox.pde.has_jacobian_K) {
    petsc_call(MatSetValues(feenox.pde.JK, feenox.fem.current_GJ, l, feenox.fem.current_GJ, l, gsl_matrix_ptr(feenox.fem.JKi, 0, 0), ADD_VALUES));
  }
  if (feenox.pde.has_jacobian_b) {
    petsc_call(MatSetValues(feenox.pde.Jb, feenox.fem.current_GJ, l, feenox.fem.current_GJ, l, gsl_matrix_ptr(feenox.fem.Jbi, 0, 0), ADD_VALUES));
  }
  if (feenox.pde.has_rhs) {
    petsc_call(VecSetValues(feenox.pde.b, feenox.fem.current_GJ, l, gsl_vector_ptr(feenox.fem.bi, 0), ADD_VALUES));
  }
  if (feenox.pde.has_internal_fluxes) {
    petsc_call(VecSetValues(feenox.pde.f, feenox.fem.current_GJ, l, gsl_vector_ptr(feenox.fem.fi, 0), ADD_VALUES));
  }

#endif  
  
  return FEENOX_OK;
}

int feenox_problem_build_element_natural_bc(element_t *this, bc_data_t *bc_data) {

#ifdef HAVE_PETSC
  // total number of gauss points
  unsigned int Q = this->type->gauss[feenox.pde.mesh->integration].Q;

  if (feenox.fem.current_elemental_type != this->type) {
    feenox_call(feenox_problem_build_elemental_objects_allocate(this));
  }
  
  if (feenox.fem.vec_f == NULL) {
    feenox_check_alloc(feenox.fem.vec_f = gsl_vector_calloc(feenox.pde.dofs));
  } else {
    gsl_vector_set_zero(feenox.fem.vec_f);
  }

  if (feenox.pde.has_rhs)   {
    gsl_vector_set_zero(feenox.fem.bi);
  }  
  if (bc_data->fills_matrix) {
    gsl_matrix_set_zero(feenox.fem.Ki);
  }
  if (feenox.pde.has_jacobian_b) {
    gsl_matrix_set_zero(feenox.fem.Jbi);
  }

  for (unsigned int q = 0; q < Q; q++) {
    feenox_call(bc_data->set_natural(bc_data, this, q));
  }
  
  PetscInt *l = feenox_fem_compute_dof_indices(this, feenox.pde.dofs);
  if (feenox.pde.has_rhs == PETSC_TRUE) {
    petsc_call(VecSetValues(feenox.pde.b, feenox.fem.current_GJ, l, gsl_vector_ptr(feenox.fem.bi, 0), ADD_VALUES));
  }
  if (bc_data->fills_matrix) {
    petsc_call(MatSetValues(feenox.pde.K, feenox.fem.current_GJ, l,
                                          feenox.fem.current_GJ, l, gsl_matrix_ptr(feenox.fem.Ki, 0, 0), ADD_VALUES));
  }
/*
  if (feenox.pde.has_jacobian_K) {
    petsc_call(MatSetValues(feenox.pde.JK, feenox.pde.elemental_size, l, feenox.pde.elemental_size, l, gsl_matrix_ptr(feenox.pde.JKi, 0, 0), ADD_VALUES));
  } 
*/
  if (feenox.pde.has_jacobian_b) {
    petsc_call(MatSetValues(feenox.pde.Jb, feenox.fem.current_GJ, l, feenox.fem.current_GJ, l, gsl_matrix_ptr(feenox.fem.Jbi, 0, 0), ADD_VALUES));
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

  // TODOD: which is better? measure!
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

int feenox_problem_stiffness_add(element_t *e, unsigned int q, gsl_matrix *Kiq) {

#ifdef HAVE_PETSC
  double wdet = feenox_fem_compute_w_det_at_gauss(e, q);
  gsl_matrix_scale(Kiq, wdet);
  gsl_matrix_add(feenox.fem.Ki, Kiq);
#endif
  
  return FEENOX_OK;
}

int feenox_problem_rhs_add(element_t *e, unsigned int q, double *value) {

#ifdef HAVE_PETSC
  for (unsigned int g = 0; g < feenox.pde.dofs; g++) {
    gsl_vector_set(feenox.fem.vec_f, g, value[g]);
  }  
  
  double wdet = feenox_fem_compute_w_det_at_gauss_integration(e, q, feenox.pde.mesh->integration);
  gsl_matrix *H_Gc = feenox_fem_compute_H_Gc_at_gauss(e, q, feenox.pde.mesh->integration);
  feenox_call(feenox_blas_Atb_accum(H_Gc, feenox.fem.vec_f, wdet, feenox.fem.bi));
#endif
  
  return FEENOX_OK;
}

