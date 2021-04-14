#include "feenox.h"
extern feenox_t feenox;

int feenox_build(void) {
  
  size_t step = ceil((double)feenox.pde.mesh->n_elements/100.0);
  if (step < 1) {
    step = 1;
  }
  
  // empty global objects
  if (feenox.pde.K != NULL) {
    petsc_call(MatZeroEntries(feenox.pde.K));
  }
  if (feenox.pde.M != NULL) {
    petsc_call(MatZeroEntries(feenox.pde.M));
  }
  if (feenox.pde.b != NULL) {
    petsc_call(VecZeroEntries(feenox.pde.b));
  }  
  
  unsigned int ascii_progress_chars = 0;  
  size_t i = 0;
  for (i = feenox.pde.first_element; i < feenox.pde.last_element; i++) {

// ------ progress bar ------------------------------------------    
    if (feenox.pde.progress_ascii == PETSC_TRUE && (i % step) == 0) {
      printf(CHAR_PROGRESS_BUILD);  
      fflush(stdout);
      ascii_progress_chars++;
    }
// --------------------------------------------------------------    
    
    if (feenox.pde.mesh->element[i].type->dim == feenox.pde.dim) {
      
      // volumetric elements need volumetric builds
      // TODO: check if we can skip re-building in linear transient
      feenox_call(feenox_build_element_volumetric(&feenox.pde.mesh->element[i]));
      
    } else if (feenox.pde.mesh->element[i].physical_group != NULL) {
      
      // lower-dimensional elements might (or not) have BCs
      bc_t *bc = NULL;
      LL_FOREACH(feenox.pde.mesh->element[i].physical_group->bcs, bc) {
        bc_data_t *bc_data;
        LL_FOREACH(bc->bc_datums, bc_data) {
          // we only handle weak BCs here
          if (bc_data->type_math != bc_type_math_dirichlet) {
            // and only apply them if the condition holds true (or if there's no condition at all)
            if (bc_data->condition.items == NULL || fabs(feenox_expression_eval(&bc_data->condition)) > 1e-3) {
//               feenox_call(fino_build_element_bc(bc_data, &feenox.pde.mesh->element[i]));
            }  
          }  
        }
      }
    }
  }

  feenox_call(feenox_build_assembly());

  
  return FEENOX_OK;
}



int feenox_elemental_objects_allocate(element_t *this) {

  feenox_call(feenox_elemental_objects_free());
      
  feenox.pde.n_local_nodes = this->type->nodes;
  feenox.pde.elemental_size = this->type->nodes * feenox.pde.dofs;
  
  feenox_check_alloc(feenox.pde.Ki = gsl_matrix_calloc(feenox.pde.elemental_size, feenox.pde.elemental_size));
  feenox_check_alloc(feenox.pde.Mi = gsl_matrix_calloc(feenox.pde.elemental_size, feenox.pde.elemental_size));
  feenox_check_alloc(feenox.pde.bi = gsl_vector_calloc(feenox.pde.elemental_size));
  
  return FEENOX_OK;

}


int feenox_elemental_objects_free(void) {

  if (feenox.pde.n_local_nodes != 0 && feenox.pde.elemental_size != 0) {
    gsl_matrix_free(feenox.pde.Ki);
    gsl_matrix_free(feenox.pde.Mi);
    gsl_vector_free(feenox.pde.bi);
  }
  
  feenox.pde.n_local_nodes = 0;
  feenox.pde.elemental_size = 0;
  
  
  return FEENOX_OK;

}


int feenox_build_element_volumetric(element_t *this) {

  if (this->physical_group == NULL) {
    feenox_push_error_message("volumetric element %d does not have an associated physical group", this->tag);
    return FEENOX_ERROR;
  }
  

  // total number of gauss points
  unsigned int V = this->type->gauss[feenox.pde.mesh->integration].V;
    
  if (feenox.pde.n_local_nodes != this->type->nodes) {
    feenox_call(feenox_elemental_objects_allocate(this));
  }
    
  // TODO: see if this is actually needed
/*  
  if (this->B == NULL) {
    feenox_check_alloc(this->B = calloc(V, sizeof(gsl_matrix *)));
  }
*/    
  // initialize to zero the elemental objects
  gsl_matrix_set_zero(feenox.pde.Ki);
  gsl_matrix_set_zero(feenox.pde.Mi);
  gsl_vector_set_zero(feenox.pde.bi);

  unsigned int v = 0;
  for (v = 0; v < V; v++) {
    // TODO: virtual
    feenox_call(feenox_build_element_volumetric_gauss_point_thermal(this, v));
  }
  
  // compute the indices of the DOFs to ensamble
  feenox_call(feenox_mesh_compute_dof_indices(this, feenox.pde.mesh));

  if (feenox.pde.K != NULL)  {
    petsc_call(MatSetValues(feenox.pde.K, feenox.pde.elemental_size, this->l, feenox.pde.elemental_size, this->l, gsl_matrix_ptr(feenox.pde.Ki, 0, 0), ADD_VALUES));
  }
  if (feenox.pde.M != NULL)  {
    petsc_call(MatSetValues(feenox.pde.M, feenox.pde.elemental_size, this->l, feenox.pde.elemental_size, this->l, gsl_matrix_ptr(feenox.pde.Mi, 0, 0), ADD_VALUES));
  }
  if (feenox.pde.b != NULL) {
    petsc_call(VecSetValues(feenox.pde.b, feenox.pde.elemental_size, this->l, gsl_vector_ptr(feenox.pde.bi, 0), ADD_VALUES));
  }

  return FEENOX_OK;
}

/*
int fino_build_element_bc(element_t *element, bc_t *bc) {

  if (fino.n_local_nodes != element->type->nodes) {
    feenox_call(fino_allocate_elemental_objects(element));
  }
  
  if (fino.Nb == NULL) {
    fino.Nb = gsl_vector_calloc(fino.degrees);
  }
  gsl_vector_set_zero(fino.Nb);
  gsl_vector_set_zero(fino.bi);

  if (fino.problem_family == problem_family_mechanical) {
    
    // TODO: poner un flag si se necesita
    if (element->type->dim == 1 || element->type->dim == 2) {
      feenox_call(mesh_compute_normal(element));
    }  
    
    feenox_call(fino_break_set_neumann(element, bc));
    
  } else if (fino.problem_family == problem_family_thermal) {
    
    if (bc->type_phys == bc_phys_heat_flux || bc->type_phys == bc_phys_heat_total) {
      if (strcmp(bc->expr[0].string, "0") != 0) { // para no tener que hacer cuentas si es adiabatico
        feenox_call(fino_thermal_set_heat_flux(element, bc));
      }
    } else if (bc->type_phys == bc_phys_convection) {
      feenox_call(fino_thermal_set_convection(element, bc));
    }
    
  }
  
  mesh_compute_l(fino.mesh, element);
  petsc_call(VecSetValues(fino.b, fino.elemental_size, element->l, gsl_vector_ptr(fino.bi, 0), ADD_VALUES));
  
  return WASORA_RUNTIME_OK;
  
}

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


int feenox_build_assembly(void) {
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
  if (feenox.pde.M != NULL) {
    petsc_call(MatAssemblyEnd(feenox.pde.M, MAT_FINAL_ASSEMBLY));
  }
  
  return FEENOX_OK;
}
