#include "feenox.h"
extern feenox_t feenox;

int feenox_instruction_solve_problem(void *arg) {
  
#ifdef HAVE_PETSC

  // initialize only if we did not initialize before
  if (feenox.pde.spatial_unknowns == 0) {
    feenox_call(feenox.pde.problem_init_runtime_particular());
    if (feenox.pde.solve == NULL) {
      feenox_push_error_message("internal mismatch, undefined solver");
      return FEENOX_ERROR;
    }
    feenox_call(feenox_problem_init_runtime_general());
  }

  if (feenox_var_value(feenox_special_var(in_static)) && feenox.pde.initial_condition != NULL) {
    // fill the petsc vector with the data from the initial condition function of space
    size_t j = 0;
    for (j = feenox.pde.first_node; j < feenox.pde.last_node; j++) {
      petsc_call(VecSetValue(feenox.pde.phi, feenox.pde.mesh->node[j].index_dof[0], feenox_function_eval(feenox.pde.initial_condition, feenox.pde.mesh->node[j].x), INSERT_VALUES));
    }
    // TODO: add prefixes to allow -vec_view
    petsc_call(VecAssemblyBegin(feenox.pde.phi));
    petsc_call(VecAssemblyEnd(feenox.pde.phi));
  } 
  
  // solve the problem with this per-mathematics virtual method
  // (which in turn calls a per-physics matrix & vector builds)
  feenox_call(feenox.pde.solve());  
  // TODO: how to do this in parallel?
  feenox_call(feenox_phi_to_solution(feenox.pde.phi, PETSC_FALSE));

  
/*  
  feenox_var(feenox.pde.vars.time_petsc_build) += feenox.pde.petsc.build_end - feenox.pde.petsc.build_begin;
  feenox_var(feenox.pde.vars.time_wall_build)  += feenox.pde.wall.build_end  - feenox.pde.wall.build_begin;
  feenox_var(feenox.pde.vars.time_cpu_build)   += feenox.pde.cpu.build_end   - feenox.pde.cpu.build_begin;
  
  feenox_var(feenox.pde.vars.time_petsc_solve) += feenox.pde.petsc.solve_end - feenox.pde.petsc.solve_begin;
  feenox_var(feenox.pde.vars.time_wall_solve)  += feenox.pde.wall.solve_end  - feenox.pde.wall.solve_begin;
  feenox_var(feenox.pde.vars.time_cpu_solve)   += feenox.pde.cpu.solve_end   - feenox.pde.cpu.solve_begin;

  feenox_var(feenox.pde.vars.time_petsc_stress) += feenox.pde.petsc.stress_end - feenox.pde.petsc.stress_begin;
  feenox_var(feenox.pde.vars.time_wall_stress)  += feenox.pde.wall.stress_end  - feenox.pde.wall.stress_begin;
  feenox_var(feenox.pde.vars.time_cpu_stress)   += feenox.pde.cpu.stress_end   - feenox.pde.cpu.stress_begin;
  
  feenox_var(feenox.pde.vars.time_petsc_total) += feenox_var(feenox.pde.vars.time_petsc_build) + feenox_var(feenox.pde.vars.time_petsc_solve) + feenox_var(feenox.pde.vars.time_petsc_stress);
  feenox_var(feenox.pde.vars.time_wall_total)  += feenox_var(feenox.pde.vars.time_wall_build)  + feenox_var(feenox.pde.vars.time_wall_solve)  + feenox_var(feenox.pde.vars.time_wall_stress);
  feenox_var(feenox.pde.vars.time_cpu_total)   += feenox_var(feenox.pde.vars.time_cpu_build)   + feenox_var(feenox.pde.vars.time_cpu_solve)   + feenox_var(feenox.pde.vars.time_cpu_stress);

  getrusage(RUSAGE_SELF, &feenox.pde.resource_usage);
  feenox_value(feenox.pde.vars.memory) = (double)(1024.0*feenox.pde.resource_usage.ru_maxrss);
*/
#endif  
  return FEENOX_OK;
}

#ifdef HAVE_PETSC
int feenox_phi_to_solution(Vec phi, PetscBool compute_gradients) {

  VecScatter         vscat;
  Vec                phi_full;
  PetscScalar        *phi_full_array;
  PetscInt           nlocal;

  // TODO: if nprocs == 1 then we already have the full vector
  petsc_call(VecScatterCreateToAll(phi, &vscat, &phi_full));
  petsc_call(VecScatterBegin(vscat, phi, phi_full, INSERT_VALUES,SCATTER_FORWARD));
  petsc_call(VecScatterEnd(vscat, phi, phi_full, INSERT_VALUES,SCATTER_FORWARD));

  petsc_call(VecGetLocalSize(phi_full, &nlocal));
  if (nlocal != feenox.pde.global_size) {
    feenox_push_error_message("internal check of problem size with scatter failed, %d != %d\n", nlocal, feenox.pde.global_size);
    return FEENOX_OK;
  }
  petsc_call(VecGetArray(phi_full, &phi_full_array));
  
  // make up G functions with the solution
  size_t j = 0;
  for (j = 0; j < feenox.pde.spatial_unknowns; j++) {
    
    if (feenox.pde.mesh->node[j].phi == NULL) {
      feenox_check_alloc(feenox.pde.mesh->node[j].phi = calloc(feenox.pde.dofs, sizeof(double)));
    }
    
    unsigned int g = 0;
    for (g = 0; g < feenox.pde.dofs; g++) {
      feenox.pde.mesh->node[j].phi[g] = phi_full_array[feenox.pde.mesh->node[j].index_dof[g]];

      // if we are not in rough mode we fill the solution here
      // because it is easier, in rough mode we need to
      // iterate over the elements instead of over the nodes
      if (feenox.pde.rough == 0) {
        feenox.pde.solution[g]->data_value[j] = feenox.pde.mesh->node[j].phi[g];
      }

      if (feenox.pde.nev > 1) {
        double xi = 0;
        unsigned int i = 0;
        for (i = 0; i < feenox.pde.nev; i++) {
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

/*  
  if (compute_gradients) {
    // TODO: function pointers
    if (feenox.pde.problem_family == problem_family_mechanical) {
  
      time_checkpoint(stress_begin);
      feenox_call(feenox_compute_strain_energy());
      if (feenox.pde.gradient_evaluation != gradient_none) {
        feenox_call(feenox_break_compute_stresses());
      }  
      time_checkpoint(stress_end);
      
    } else if (feenox.pde.problem_family == problem_family_thermal) {
       
      feenox_call(feenox_thermal_compute_fluxes());
    }  
  }
*/    
  
  
  return FEENOX_OK;
}
#endif

int feenox_problem_init_default(void) {
  return FEENOX_OK;
}
