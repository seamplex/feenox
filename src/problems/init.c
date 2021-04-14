/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's PDE initialization routines
 *
 *  Copyright (C) 2015--2021 Seamplex
 *
 *  This file is part of Fino <https://www.seamplex.com/feenox>.
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
#define _GNU_SOURCE  // for asprintf
#include "feenox.h"
extern feenox_t feenox;

#include <signal.h>   // for signal
#include <unistd.h>   // for sysconf


// this is a wrapper because PetscOptionsHasName changed its arguments after 3.7.0
#if PETSC_VERSION_LT(3,7,0)
 #define PetscOptionsHasNameWrapper(a, b, c) PetscOptionsHasName(a, b, c)
#else
 #define PetscOptionsHasNameWrapper(a, b, c) PetscOptionsHasName(PETSC_NULL, a, b, c)
#endif


// #define NAME_SIZE 32

int feenox_problem_init_parser_general(void) {

  if (sizeof(PetscReal) != sizeof(double)) {
    feenox_push_error_message("PETSc should be compiled with double-precision real scalar types and we have double = %d != PetscReal = %d", sizeof(double), sizeof(PetscReal));
    return FEENOX_ERROR;
  }
  
  // TODO: check versions from the header and from the library
  
  // we process the original command line (beucase the one that remains after getopt might have a different order)
  // for instance, "-log_summary" is trapped by feenox's getoopt as "-l"
  // so that needs to be rewritten as "--petsc_opt log_summary"
  // if the option has an argument it has to be put as "--slepc_opt pc_type=sor"
/*  
  for (i = 0; i < feenox.argc_orig; i++) {
    if (strcmp(feenox.argv_orig[i], "--petsc") == 0) {
      if (i >= (feenox.argc_orig-1)) {
        feenox_push_error_message("commandline option --petsc needs an argument");
        return FEENOX_ERROR;
      } else if (feenox.argv_orig[i+1][0] == '-') {
        feenox_push_error_message("the argument of commandline option --petsc should not start with a dash (it is added automatically)");
        return FEENOX_ERROR;
      }
      
      if ((dummy = strchr(feenox.argv_orig[i+1], '=')) != NULL)  {
        char *tmp1, *tmp2;
        *dummy = '\0';
        tmp1 = strdup(feenox.argv_orig[i+1]);
        tmp2 = strdup(dummy+1);
        feenox.argv_orig[i]   = realloc(feenox.argv_orig[i],   strlen(feenox.argv_orig[i+1])+6);
        feenox.argv_orig[i+1] = realloc(feenox.argv_orig[i+1], strlen(dummy+1)+6);
        snprintf(feenox.argv_orig[i], strlen(feenox.argv_orig[i+1])+4, "-%s", tmp1);
        snprintf(feenox.argv_orig[i+1], strlen(dummy+1)+4, "%s", tmp2);
        feenox_free(tmp1);
        feenox_free(tmp2);
        
      } else {
        char *tmp1;
        tmp1 = strdup(feenox.argv_orig[i+1]);
        feenox.argv_orig[i+1] = realloc(feenox.argv_orig[i+1], strlen(tmp1)+6);
        feenox.argv_orig[i][0] = '\0';
        snprintf(feenox.argv_orig[i+1], strlen(tmp1)+4, "-%s", tmp1);
        feenox_free(tmp1);
      }
      i++;
    }
  }
*/

#ifdef HAVE_SLEPC  
  // initialize SLEPc (which in turn initalizes PETSc)
  petsc_call(SlepcInitialize(&feenox.argc, &feenox.argv, (char*)0, PETSC_NULL));
#else
  // initialize PETSc
  petsc_call(PetscInitialize(&feenox.argc, &feenox.argv, (char*)0, PETSC_NULL));
#endif
  feenox.pde.petscinit_called = PETSC_TRUE;
  
  // segfaults are segfaults, try to leave PETSC out of them
  signal(SIGSEGV, SIG_DFL);

  // get the number of processes and the rank
  petsc_call(MPI_Comm_size(PETSC_COMM_WORLD, &feenox.nprocs));
  petsc_call(MPI_Comm_rank(MPI_COMM_WORLD, &feenox.rank));

  // TODO
  // install out error handler for PETSc
//  petsc_call(PetscPushErrorHandler(&feenox_handler, NULL));

  // register events
/*  
  petsc_call(PetscClassIdRegister("Fino", &feenox.pde.petsc_classid));

  petsc_call(PetscLogStageRegister("Assembly", &feenox.pde.petsc_stage_build));
  petsc_call(PetscLogStageRegister("Solution", &feenox.pde.petsc_stage_solve));
  petsc_call(PetscLogStageRegister("Stress", &feenox.pde.petsc_stage_solve));
  
  petsc_call(PetscLogEventRegister("feenox_build", feenox.pde.petsc_classid, &feenox.pde.petsc_event_build));
  petsc_call(PetscLogEventRegister("feenox_solve", feenox.pde.petsc_classid, &feenox.pde.petsc_event_solve));
  petsc_call(PetscLogEventRegister("feenox_stress", feenox.pde.petsc_classid, &feenox.pde.petsc_event_solve));
*/
  
  
  // Fino's special variables
///va+feenox_abstol+name feenox_abstol
///va+feenox_abstol+detail Absolute tolerance of the linear solver, as passed to PETSc’s
///va+feenox_abstol+detail [`KSPSetTolerances`](http://www.mcs.anl.gov/petsc/petsc-current/docs/manualpages/KSP/KSPSetTolerances.html)
  feenox.pde.vars.ksp_atol = feenox_define_variable_get_ptr("ksp_atol");
  // TODO: poner el default automaticamente with PETSC_DEFAULT
///va+feenox_abstol+detail Default `1e-50`.
  feenox_var_value(feenox.pde.vars.ksp_atol) = 1e-50;   // igual al de PETSc
 
///va+feenox_reltol+name feenox_reltol
///va+feenox_reltol+detail Relative tolerance of the linear solver,
///va+feenox_reltol+detail as passed to PETSc’s
///va+feenox_reltol+detail [`KSPSetTolerances`](http://www.mcs.anl.gov/petsc/petsc-current/docs/manualpages/KSP/KSPSetTolerances.html).
feenox.pde.vars.ksp_rtol = feenox_define_variable_get_ptr("ksp_rtol");
///va+feenox_reltol+detail Default `1e-6`.
  feenox_var_value(feenox.pde.vars.ksp_rtol) = 1e-6;    // el de PETSc es 1e-5
  
///va+feenox_divtol+name feenox_divtol
///va+feenox_divtol+detail Divergence tolerance,
///va+feenox_divtol+detail as passed to PETSc’s
///va+feenox_divtol+detail [`KSPSetTolerances`](http://www.mcs.anl.gov/petsc/petsc-current/docs/manualpages/KSP/KSPSetTolerances.html).
  feenox.pde.vars.ksp_divtol = feenox_define_variable_get_ptr("ksp_divtol");
///va+feenox_divtol+detail Default `1e+4`.  
  feenox_var_value(feenox.pde.vars.ksp_divtol) = 1e+4;  // igual al de PETSc
  
///va+feenox_max_iterations+name feenox_max_iterations
///va+feenox_max_iterations+detail Number of maximum iterations before diverging,
///va+feenox_max_iterations+detail as passed to PETSc’s
///va+feenox_max_iterations+detail [`KSPSetTolerances`](http://www.mcs.anl.gov/petsc/petsc-current/docs/manualpages/KSP/KSPSetTolerances.html).
  feenox.pde.vars.ksp_max_it = feenox_define_variable_get_ptr("ksp_max_it");
///va+feenox_max_iterations+detail Default `10000`.
  feenox_var_value(feenox.pde.vars.ksp_max_it) = 10000;   // igual al de PETSc

///va+feenox_gamg_threshold+name feenox_gamg_threshold
///va+feenox_gamg_threshold+detail Relative threshold to use for dropping edges in aggregation graph for the
///va+feenox_gamg_threshold+detail [Geometric Algebraic Multigrid Preconditioner](http://www.mcs.anl.gov/petsc/petsc-current/docs/manualpages/PC/PCGAMG.html)
///va+feenox_gamg_threshold+detail as passed to PETSc’s
///va+feenox_gamg_threshold+detail [`PCGAMGSetThreshold`](http://www.mcs.anl.gov/petsc/petsc-current/docs/manualpages/PC/PCGAMGSetThreshold.html).
///va+feenox_gamg_threshold+detail A value of 0.0 means keep all nonzero entries in the graph; negative means keep even zero entries in the graph.
  feenox.pde.vars.gamg_threshold = feenox_define_variable_get_ptr("gamg_threshold");
///va+feenox_gamg_threshold+detail Default `0.01`.  
  feenox_var_value(feenox.pde.vars.gamg_threshold) = 0.01;
  
///va+feenox_penalty_weight+name feenox_penalty_weight
///va+feenox_penalty_weight+detail The weight $w$ used when setting multi-freedom boundary conditions.
///va+feenox_penalty_weight+detail Higher values mean better precision in the constrain but distort
///va+feenox_penalty_weight+detail the matrix condition number. 
  feenox.pde.vars.penalty_weight = feenox_define_variable_get_ptr("penalty_weight");
///va+feenox_penalty_weight+detail Default is `1e8`.
  feenox_var_value(feenox.pde.vars.penalty_weight) = 1e8;  
  
///va+feenox_iterations+name feenox_iterations
///va+feenox_iterations+detail This variable contains the actual number of iterations used
///va+feenox_iterations+detail by the solver. It is set after `FINO_STEP`.
  feenox.pde.vars.iterations = feenox_define_variable_get_ptr("pde_iterations");
  
///va+feenox_residual_norm+name feenox_residual_norm
///va+feenox_residual_norm+detail This variable contains the residual obtained
///va+feenox_residual_norm+detail by the solver. It is set after `FINO_STEP`.
  feenox.pde.vars.residual_norm= feenox_define_variable_get_ptr("residual_norm");

///va+nodes_rough+name nodes_rough
///va+nodes_rough+detail The number of nodes of the mesh in `ROUGH` mode.
  feenox.pde.vars.nodes_rough = feenox_define_variable_get_ptr("nodes_rough");

  
  
///va+time_wall_build+name time_wall_build
///va+time_wall_build+detail Wall time insumed to build the problem matrices, in seconds.
  feenox.pde.vars.time_wall_build = feenox_define_variable_get_ptr("time_wall_build");

///va+time_wall_solve+name time_wall_solve
///va+time_wall_solve+detail Wall time insumed to solve the problem, in seconds.
  feenox.pde.vars.time_wall_solve = feenox_define_variable_get_ptr("time_wall_solve");

///va+time_wall_stress+name time_wall_stress
///va+time_wall_stress+detail Wall time insumed to compute the stresses, in seconds.
  feenox.pde.vars.time_wall_stress = feenox_define_variable_get_ptr("time_wall_stress");

///va+time_wall_total+name time_wall_total
///va+time_wall_total+detail Wall time insumed to initialize, build and solve, in seconds.
  feenox.pde.vars.time_wall_total = feenox_define_variable_get_ptr("time_wall_total");
  
///va+time_cpu_build+name time_cpu_build
///va+time_cpu_build+detail CPU time insumed to build the problem matrices, in seconds.
  feenox.pde.vars.time_cpu_build = feenox_define_variable_get_ptr("time_cpu_build");

///va+time_cpu_solve+name time_cpu_solve
///va+time_cpu_solve+detail CPU time insumed to solve the problem, in seconds.
  feenox.pde.vars.time_cpu_solve = feenox_define_variable_get_ptr("time_cpu_solve");

///va+time_cpu_stress+name time_cpu_stress
///va+time_cpu_stress+detail CPU time insumed to compute the stresses from the displacements, in seconds.
  feenox.pde.vars.time_cpu_stress = feenox_define_variable_get_ptr("time_cpu_stress");
  
///va+time_wall_total+name time_cpu_total
///va+time_wall_total+detail CPU time insumed to initialize, build and solve, in seconds.
  feenox.pde.vars.time_cpu_total = feenox_define_variable_get_ptr("time_cpu_total");
  
///va+time_petsc_build+name time_petsc_build
///va+time_petsc_build+detail CPU time insumed by PETSc to build the problem matrices, in seconds.
  feenox.pde.vars.time_petsc_build = feenox_define_variable_get_ptr("time_petsc_build");

///va+time_petsc_solve+name time_petsc_solve
///va+time_petsc_solve+detail CPU time insumed by PETSc to solve the eigen-problem, in seconds.
  feenox.pde.vars.time_petsc_solve = feenox_define_variable_get_ptr("time_petsc_solve");

///va+time_petsc_stress+name time_petsc_solve
///va+time_petsc_stress+detail CPU time insumed by PETSc to compute the stresses, in seconds.
  feenox.pde.vars.time_petsc_stress = feenox_define_variable_get_ptr("time_petsc_stress");
  
///va+time_wall_total+name time_wall_total
///va+time_wall_total+detail CPU time insumed by PETSc to initialize, build and solve, in seconds.
  feenox.pde.vars.time_petsc_total = feenox_define_variable_get_ptr("time_petsc_total");

  ///va+petsc_flops+name petsc_flops
///va+petsc_flops+detail Number of floating point operations performed by PETSc/SLEPc.
  feenox.pde.vars.flops_petsc = feenox_define_variable_get_ptr("flops_petsc");
         
///va+memory_available+name memory_available
///va+memory_available+detail Total available memory, in bytes.
  feenox.pde.vars.memory_available = feenox_define_variable_get_ptr("memory_available");
#ifdef HAVE_SYSCONF  
  feenox_var_value(feenox.pde.vars.memory_available) = (double)(sysconf(_SC_PHYS_PAGES)*sysconf(_SC_PAGESIZE));
#else
  feenox_var_value(feenox.pde.vars.memory_available) = -1;
#endif
  

///va+memory+name memory
///va+memory+detail Maximum resident set size (global memory used), in bytes.
  feenox.pde.vars.memory = feenox_define_variable_get_ptr("memory");
  
///va+memory_petsc+name memory_petsc
///va+memory_petsc+detail Maximum resident set size (memory used by PETSc), in bytes.
  feenox.pde.vars.memory_petsc = feenox_define_variable_get_ptr("memory_petsc");
  
  
  // empezamos con un valor muy negativo, si nadie lo toca ni calculamos la calidad
//  feenox.pde.gradient_quality_threshold = DEFAULT_GRADIENT_JACOBIAN_THRESHOLD;

  return FEENOX_OK;
}



int feenox_problem_define_solutions(void) {
  
  if (feenox.pde.dim == 0 || feenox.pde.dofs == 0) {
    feenox_push_error_message("do not know how many dimensions the problem has, tell me with DIMENSIONS in either FINO_PROBLEM or MESH");
    return FEENOX_ERROR;
  }

  feenox_check_alloc(feenox.pde.solution = calloc(feenox.pde.dofs, sizeof(function_t *)));
  feenox_check_alloc(feenox.pde.gradient = calloc(feenox.pde.dofs, sizeof(function_t *)));
  feenox_check_alloc(feenox.pde.delta_gradient = calloc(feenox.pde.dofs, sizeof(function_t *)));
  if (feenox.pde.nev > 0) {
    feenox_check_alloc(feenox.pde.mode = calloc(feenox.pde.dofs, sizeof(function_t *)));
  }

  unsigned int g = 0;
  for (g = 0; g < feenox.pde.dofs; g++) {
    char *name = NULL;
    if (feenox.pde.unknown_name == NULL) {
      feenox_check_minusone(asprintf(&name, "phi%d", g+1));
    } else {
      feenox_check_minusone(asprintf(&name, "%s", feenox.pde.unknown_name[g]));
    }
    feenox_check_alloc(feenox.pde.solution[g] = feenox_define_function_get_ptr(name, feenox.pde.dim));
    
    // esto lo ponemos aca por si alguien quiere hacer un PRINT o algo sin pasar por el STEP
    feenox.pde.solution[g]->mesh = (feenox.pde.rough==0)?feenox.pde.mesh:feenox.pde.mesh_rough;
    feenox_check_alloc(feenox.pde.solution[g]->var_argument = calloc(feenox.pde.dim, sizeof(var_t *)));
    feenox.pde.solution[g]->var_argument_allocated = 1;
    feenox.pde.solution[g]->type = function_type_pointwise_mesh_node;

    if (feenox.pde.nev == 0) {
      // las derivadas de las soluciones con respecto al espacio solo si no es modal
      feenox_check_alloc(feenox.pde.gradient[g] = calloc(feenox.pde.dim, sizeof(function_t *)));
      feenox_check_alloc(feenox.pde.delta_gradient[g] = calloc(feenox.pde.dim, sizeof(function_t *)));
      
      unsigned int m = 0;
      for (m = 0; m < feenox.pde.dim; m++) {
        feenox.pde.solution[g]->var_argument[m] = feenox.mesh.vars.arr_x[m];
      
        char *gradname = NULL;
        feenox_check_minusone(asprintf(&gradname, "d%sd%s", name, feenox.mesh.vars.arr_x[m]->name));
        feenox_check_alloc(feenox.pde.gradient[g][m] = feenox_define_function_get_ptr(gradname, feenox.pde.dim));
        feenox_free(gradname);
        
        feenox.pde.gradient[g][m]->mesh = feenox.pde.solution[g]->mesh;
        feenox.pde.gradient[g][m]->var_argument = feenox.pde.solution[g]->var_argument;
        feenox.pde.gradient[g][m]->type = function_type_pointwise_mesh_node;
        feenox.pde.gradient[g][m]->spatial_derivative_of = feenox.pde.solution[g];
        feenox.pde.gradient[g][m]->spatial_derivative_with_respect_to = m;
        
        // lo mismo para la incerteza
        feenox_check_minusone(asprintf(&gradname, "delta_d%sd%s", name, feenox.mesh.vars.arr_x[m]->name));
        feenox_check_alloc(feenox.pde.delta_gradient[g][m] = feenox_define_function_get_ptr(gradname, feenox.pde.dim));
        feenox_free(gradname);
        
        feenox.pde.delta_gradient[g][m]->mesh = feenox.pde.solution[g]->mesh;
        feenox.pde.delta_gradient[g][m]->var_argument = feenox.pde.solution[g]->var_argument;
        feenox.pde.delta_gradient[g][m]->type = function_type_pointwise_mesh_node;
      }
      
    } else {  
      // en modal tenemos muchas soluciones
      feenox_check_alloc(feenox.pde.mode[g] = calloc(feenox.pde.nev, sizeof(function_t *)));
      unsigned int i;
      for (i = 0; i < feenox.pde.nev; i++) {
        char *modename = NULL;
        feenox_check_minusone(asprintf(&modename, "%s%d", name, i+1));
        feenox_call(feenox_problem_define_solution_function(modename, &feenox.pde.mode[g][i]));
        feenox_free(modename);
        
        feenox.pde.mode[g][i]->mesh = feenox.pde.solution[g]->mesh;
        feenox.pde.mode[g][i]->var_argument = feenox.pde.solution[g]->var_argument;
        feenox.pde.mode[g][i]->type = feenox.pde.solution[g]->type;
      }
    }
    feenox_free(name);
  }

  return FEENOX_OK;
}


int feenox_problem_define_solution_function(const char *name, function_t **function) {

  // aca la definimos para que este disponible en tiempo de parseo  
  if ((*function = feenox_define_function_get_ptr(name, feenox.pde.dim)) == NULL) {
    feenox_push_error_message("result function '%s' defined twice", name);
    return FEENOX_OK;
  }
  (*function)->mesh = feenox.pde.mesh; // esto puede cambiar a rough despues  
  feenox_problem_define_solution_clean_nodal_arguments(*function);
  (*function)->var_argument = feenox.pde.solution[0]->var_argument;
  (*function)->type = function_type_pointwise_mesh_node;

  return 0;
}


int fino_function_clean_nodal_data(function_t *function) {
 
  if (function != NULL && function->data_value != NULL) {  
    feenox_free(function->data_value);
    function->data_value = NULL;
  }
  
  return 0;
}

int feenox_problem_define_solution_clean_nodal_arguments(function_t *function) {
 
  if (function->data_argument != NULL) {
    unsigned int m = 0;
    for (m = 0; m < feenox.pde.dim; m++) {
      feenox_free(function->data_argument[m]);
    }
    feenox_free(function->data_argument);
  }
  
  return FEENOX_OK;
}



int plugin_init_before_run(void) {

  feenox.pde.global_size = 0;
  feenox.pde.spatial_unknowns = 0;
  feenox.pde.progress_r0 = 0;
  feenox.pde.already_built = PETSC_FALSE;
  feenox.pde.first_build = PETSC_TRUE;

//  feenox_call(feenox_problem_free());
  
  return FEENOX_OK;
}


int plugin_finalize(void) {

//  feenox_call(feenox_problem_free());

  if (feenox.pde.petscinit_called) {
#ifdef HAVE_SLEPC  
    petsc_call(SlepcFinalize());
#else
    petsc_call(PetscFinalize());
#endif
  }
  
  return FEENOX_OK;
}

int feenox_problem_init_runtime_general(void) {

  // command-line arguments that take precedence over the options in the input file
  // check for further commandline options
  // see if the user asked for mumps in the command line
  PetscBool flag = PETSC_FALSE;
  petsc_call(PetscOptionsHasNameWrapper(PETSC_NULL, "--mumps", &flag));
  if (flag == PETSC_TRUE) {
    feenox.pde.ksp_type = strdup("mumps");
    feenox.pde.pc_type = strdup("mumps");
  }

  // see if the user asked for progress in the command line
  if (feenox.pde.progress_ascii == PETSC_FALSE) {
    petsc_call(PetscOptionsHasNameWrapper(PETSC_NULL, "--progress", &feenox.pde.progress_ascii));
  }  

  // see if the user asked for a forced problem type
  petsc_call(PetscOptionsHasNameWrapper(PETSC_NULL, "--linear", &flag));
  if (flag == PETSC_TRUE) {
    feenox.pde.math_type = math_type_linear;
  }
  petsc_call(PetscOptionsHasNameWrapper(PETSC_NULL, "--non-linear", &flag));
  if (flag == PETSC_TRUE) {
    feenox.pde.math_type = math_type_nonlinear;
  }
  petsc_call(PetscOptionsHasNameWrapper(PETSC_NULL, "--nonlinear", &flag));
  if (flag == PETSC_TRUE) {
    feenox.pde.math_type = math_type_nonlinear;
  }
  
  
  // do some check
  if (feenox.pde.mesh == NULL) {
    feenox_push_error_message("no mesh defined");
    return FEENOX_ERROR;
  }

  physical_group_t *physical_group;
  for (physical_group = feenox.pde.mesh->physical_groups; physical_group != NULL; physical_group = physical_group->hh.next) {

    // TODO: poner una variable para elegir esto
    if (physical_group->bcs != NULL && physical_group->n_elements == 0) {
      feenox_push_error_message("physical entity '%s' has a BC but no associated elements", physical_group->name);
      return FEENOX_ERROR;
    }

    if (physical_group->material != NULL && physical_group->n_elements == 0) {
      feenox_push_error_message("physical group '%s' has a material but no associated elements", physical_group->name);
      return FEENOX_ERROR;
    }
  }


  // allocate global petsc objects
  int width = GSL_MAX(feenox.pde.mesh->max_nodes_per_element, feenox.pde.mesh->max_first_neighbor_nodes) * feenox.pde.dofs;

  // ask how many local nodes we own
  feenox.pde.nodes_local = PETSC_DECIDE;
  PetscInt n_nodes = feenox.pde.mesh->n_nodes;
  petsc_call(PetscSplitOwnership(PETSC_COMM_WORLD, &feenox.pde.nodes_local, &n_nodes));
  feenox.pde.size_local = feenox.pde.dofs * feenox.pde.nodes_local;
  
  // the global stiffnes matrix
  petsc_call(MatCreate(PETSC_COMM_WORLD, &feenox.pde.K));
  petsc_call(PetscObjectSetName((PetscObject)feenox.pde.K, "K"));
  petsc_call(MatSetSizes(feenox.pde.K, feenox.pde.size_local, feenox.pde.size_local, feenox.pde.global_size, feenox.pde.global_size));
  petsc_call(MatSetFromOptions(feenox.pde.K));
  petsc_call(MatMPIAIJSetPreallocation(feenox.pde.K, width, PETSC_NULL, width, PETSC_NULL));
  petsc_call(MatSeqAIJSetPreallocation(feenox.pde.K, width, PETSC_NULL));
  petsc_call(MatSetOption(feenox.pde.K, MAT_KEEP_NONZERO_PATTERN, PETSC_TRUE));

  if (feenox.pde.allow_new_nonzeros) {
    petsc_call(MatSetOption(feenox.pde.K, MAT_NEW_NONZERO_ALLOCATION_ERR, PETSC_FALSE));
  }  
  
  if (feenox.pde.dofs > 1) {
    petsc_call(MatSetBlockSize(feenox.pde.K, feenox.pde.dofs));
  }

  // the solution (unknown) vector
  petsc_call(MatCreateVecs(feenox.pde.K, &feenox.pde.phi, NULL));
  petsc_call(PetscObjectSetName((PetscObject)feenox.pde.phi, "phi"));
  petsc_call(VecSetFromOptions(feenox.pde.phi));
  // explicit initial value
  petsc_call(VecSet(feenox.pde.phi, 0));


  if (feenox.pde.has_rhs) {
    // the right-hand-side vector
    petsc_call(MatCreateVecs(feenox.pde.K, NULL, &feenox.pde.b));
    petsc_call(PetscObjectSetName((PetscObject)feenox.pde.b, "b"));
    petsc_call(VecSetFromOptions(feenox.pde.b));
  }
  
  if (feenox.pde.has_mass) {
    // the mass matrix for modal or heat transient
    petsc_call(MatCreate(PETSC_COMM_WORLD, &feenox.pde.M));
    petsc_call(PetscObjectSetName((PetscObject)feenox.pde.M, "M"));
    petsc_call(MatSetSizes(feenox.pde.M, feenox.pde.size_local, feenox.pde.size_local, feenox.pde.global_size, feenox.pde.global_size));
    petsc_call(MatSetFromOptions(feenox.pde.M));
    petsc_call(MatMPIAIJSetPreallocation(feenox.pde.M, width, PETSC_NULL, width, PETSC_NULL));
    petsc_call(MatSeqAIJSetPreallocation(feenox.pde.M, width, PETSC_NULL));
    petsc_call(MatSetOption(feenox.pde.M, MAT_KEEP_NONZERO_PATTERN, PETSC_TRUE));
    if (feenox.pde.dofs > 1) {
      petsc_call(MatSetBlockSize(feenox.pde.M, feenox.pde.dofs));
    }
    if (feenox.pde.allow_new_nonzeros) {
      petsc_call(MatSetOption(feenox.pde.M, MAT_NEW_NONZERO_ALLOCATION_ERR, PETSC_FALSE));
    }  
  }
  
  
  // ask for the local ownership range
  petsc_call(MatGetOwnershipRange(feenox.pde.K, &feenox.pde.first_row, &feenox.pde.last_row));
  feenox.pde.first_node = feenox.pde.first_row / feenox.pde.dofs;
  feenox.pde.last_node = feenox.pde.last_row / feenox.pde.dofs;
  
  // TODO: honor mesh partitions
  // https://lists.mcs.anl.gov/pipermail/petsc-users/2014-April/021433.html
  feenox.pde.first_element = (feenox.pde.mesh->n_elements / feenox.nprocs) * feenox.rank;
  if (feenox.pde.mesh->n_elements % feenox.nprocs > feenox.rank) {
    feenox.pde.first_element += feenox.rank;
    feenox.pde.last_element = feenox.pde.first_element + (feenox.pde.mesh->n_elements / feenox.nprocs) + 1;
  } else {  
    feenox.pde.first_element += feenox.pde.mesh->n_elements % feenox.nprocs;
    feenox.pde.last_element = feenox.pde.first_element + (feenox.pde.mesh->n_elements / feenox.nprocs);
  }  

  // fill in the holders of the continuous functions that will hold the solution
  
  if (feenox.pde.rough == 0) {
    unsigned int g = 0;
    if (feenox.pde.solution == NULL) {
      feenox_push_error_message("inconsistent internal state, solution funcions not allocated", physical_group->name);
      return FEENOX_ERROR;
    }
    for (g = 0; g < feenox.pde.dofs; g++) {
      feenox.pde.solution[g]->mesh = feenox.pde.mesh;
      feenox.pde.solution[g]->data_size = feenox.pde.spatial_unknowns;
      feenox.pde.solution[g]->data_argument = feenox.pde.mesh->nodes_argument;
      feenox_check_alloc(feenox.pde.solution[g]->data_value = calloc(feenox.pde.spatial_unknowns, sizeof(double)));
    
      if (feenox.pde.nev > 0) {
        unsigned int i = 0;
        for (i = 0; i < feenox.pde.nev; i++) {
          feenox.pde.mode[g][i]->mesh = feenox.pde.mesh;
          feenox.pde.mode[g][i]->data_argument = feenox.pde.solution[0]->data_argument;
          feenox.pde.mode[g][i]->data_size = feenox.pde.mesh->n_nodes;
          feenox_check_alloc(feenox.pde.mode[g][i]->data_value = calloc(feenox.pde.spatial_unknowns, sizeof(double)));
        }
      }
    }
    
  } else {
/*
    mesh_post_t *post; 
    
    feenox_call(feenox_init_rough_mesh());
    // maybe the macros fill_* can be used
    for (g = 0; g < feenox.pde.degrees; g++) {
      feenox.pde.solution[g]->mesh = feenox.pde.mesh_rough;
      feenox.pde.solution[g]->data_size = feenox.pde.mesh_rough->n_nodes;
      feenox.pde.solution[g]->data_argument = feenox.pde.mesh_rough->nodes_argument;
      feenox.pde.solution[g]->data_value = calloc(feenox.pde.mesh_rough->n_nodes, sizeof(double));
    }
    
    // si estamos en rough tenemos que cambiar la malla de salida de los MESH_POSTs
    LL_FOREACH(feenox.mesh.posts, post) {
      if (post->mesh == feenox.pde.mesh) {
        post->mesh = feenox.pde.mesh_rough;
      }
    }
*/
  }

  feenox_call(feenox_mesh_init_nodal_indexes(feenox.pde.mesh, feenox.pde.dofs));
  
  return FEENOX_OK;
}

/*  
int feenox_init_rough_mesh(void) {
  
  int i, i_global;
  int j, j_global;
  int m;

  element_t *element;
  node_t *node;
  element_list_item_t *element_list;
  
  if (feenox.pde.mesh_rough == NULL) {
    feenox_push_error_message("rough mesh is NULL");
    return FEENOX_ERROR;
  }
  
  // primera pasada, copiamos lo que podemos, alocamos y contamos la cantidad de nodos
  // ya la alocamos en parser
  feenox.pde.mesh_rough->bulk_dimensions = feenox.pde.mesh->bulk_dimensions;
  feenox.pde.mesh_rough->spatial_dimensions = feenox.pde.mesh->spatial_dimensions;
  feenox.pde.mesh_rough->n_elements = feenox.pde.mesh->n_elements;
  feenox.pde.mesh_rough->element = calloc(feenox.pde.mesh_rough->n_elements, sizeof(element_t));
  feenox.pde.mesh_rough->n_nodes = 0;
  i_global = 0;
  for (i = 0; i < feenox.pde.mesh_rough->n_elements; i++) {
    if (feenox.pde.mesh->element[i].type->dim == feenox.pde.mesh_rough->bulk_dimensions) {
      element = &feenox.pde.mesh_rough->element[i_global];

      element->index = i_global;
      element->tag = i+1;
      element->type = feenox.pde.mesh->element[i].type;
      element->physical_group = feenox.pde.mesh->element[i].physical_group;

      feenox.pde.mesh_rough->n_nodes += element->type->nodes;
      i_global++;
    }  
  }
  
  feenox.pde.mesh_rough->n_elements = i_global;
  feenox.pde.mesh_rough->element = realloc(feenox.pde.mesh_rough->element, feenox.pde.mesh_rough->n_elements*sizeof(element_t));
  
  // segunda pasada, creamos los nodos
  j_global = 0;
  feenox.pde.mesh_rough->node = calloc(feenox.pde.mesh_rough->n_nodes, sizeof(node_t));
  for (i = 0; i < feenox.pde.mesh_rough->n_elements; i++) {
    element = &feenox.pde.mesh_rough->element[i];
    element->node = calloc(element->type->nodes, sizeof(node_t));
    
    for (j = 0; j < element->type->nodes; j++) {
      node = &feenox.pde.mesh_rough->node[j_global];
      node->tag = j_global+1;
      node->index_mesh = j_global;
      node->x[0] = feenox.pde.mesh->element[element->tag-1].node[j]->x[0];
      node->x[1] = feenox.pde.mesh->element[element->tag-1].node[j]->x[1];
      node->x[2] = feenox.pde.mesh->element[element->tag-1].node[j]->x[2];
      
      node->phi = feenox.pde.mesh->element[element->tag-1].node[j]->phi;
//      node->dphidx = feenox.pde.mesh->element[element->tag-1].dphidx_node[j];
      
      element_list = calloc(1, sizeof(element_list_item_t));
      element_list->element = element;
      LL_APPEND(node->associated_elements, element_list);
      
      element->node[j] = node;
      
      j_global++;
    }
  }

  // rellenamos un array de nodos que pueda ser usado como argumento de funciones
  feenox.pde.mesh_rough->nodes_argument = calloc(feenox.pde.mesh_rough->spatial_dimensions, sizeof(double *));
  for (m = 0; m < feenox.pde.mesh_rough->spatial_dimensions; m++) {
    feenox.pde.mesh_rough->nodes_argument[m] = calloc(feenox.pde.mesh_rough->n_nodes, sizeof(double));
    for (j = 0; j < feenox.pde.mesh_rough->n_nodes; j++) {
      feenox.pde.mesh_rough->nodes_argument[m][j] = feenox.pde.mesh_rough->node[j].x[m]; 
    }
  }
  
  // ponemos a disposicion la cantidad de nodos rough
  feenox_var_value(feenox.pde.vars.nodes_rough) = feenox.pde.mesh_rough->n_nodes;
  
  return WASORA_RUNTIME_OK;

}
*/
  

/*
int feenox_problem_free(void) {
  int g, d;

  if (feenox.pde.mesh != NULL && feenox.pde.mesh->n_elements != 0) {
    for (g = 0; g < feenox.pde.degrees; g++) {
      for (d = 0; d < feenox.pde.dimensions; d++) {
        if (feenox.pde.gradient != NULL && feenox.pde.gradient[g] != NULL) {  
          feenox_free(feenox.pde.gradient[g][d]->data_value);
          feenox.pde.gradient[g][d]->data_value = NULL;
        }  
        if (feenox.pde.delta_gradient != NULL && feenox.pde.delta_gradient[g] != NULL) {  
          feenox_free(feenox.pde.delta_gradient[g][d]->data_value);
          feenox.pde.delta_gradient[g][d]->data_value = NULL;
        }  
      }
      
      feenox_free(feenox.pde.solution[g]->data_value);
      feenox.pde.solution[g]->data_value = NULL;      
    }
    
    if (feenox.pde.sigma != NULL) {
      feenox_free(feenox.pde.sigmax->data_value);
      if (feenox.pde.dimensions > 1) {
        feenox_free(feenox.pde.sigmay->data_value);
        feenox_free(feenox.pde.tauxy->data_value);
        if (feenox.pde.dimensions > 2) {
          feenox_free(feenox.pde.sigmaz->data_value);
          feenox_free(feenox.pde.tauyz->data_value);
          feenox_free(feenox.pde.tauzx->data_value);
        }  
      }
      feenox_free(feenox.pde.sigma1->data_value);
      feenox_free(feenox.pde.sigma2->data_value);
      feenox_free(feenox.pde.sigma3->data_value);
      feenox_free(feenox.pde.sigma->data_value);
      feenox_free(feenox.pde.delta_sigma->data_value);
      feenox_free(feenox.pde.tresca->data_value);
      
      feenox.pde.sigmax->data_value = NULL;
      if (feenox.pde.dimensions > 1) {
        feenox.pde.sigmay->data_value = NULL;
        feenox.pde.tauxy->data_value = NULL;
        if (feenox.pde.dimensions > 2) {
          feenox.pde.sigmaz->data_value = NULL;
          feenox.pde.tauyz->data_value = NULL;
          feenox.pde.tauzx->data_value = NULL;
        }
      }
      feenox.pde.sigma1->data_value = NULL;
      feenox.pde.sigma2->data_value = NULL;
      feenox.pde.sigma3->data_value = NULL;
      feenox.pde.sigma->data_value = NULL;
      feenox.pde.delta_sigma->data_value = NULL;
      feenox.pde.tresca->data_value = NULL;
    }
    
    mesh_free(feenox.pde.mesh);
    
  }

  if (feenox.pde.unknown_name != NULL) {
    if (feenox.pde.degrees != 0) {
      for (g = 0; g < feenox.pde.degrees; g++) {
        feenox_free(feenox.pde.unknown_name[g]);
        feenox.pde.unknown_name[g] = NULL;
      }
    }  
    feenox_free(feenox.pde.unknown_name);
    feenox.pde.unknown_name = NULL;
  }
  
  
  feenox_call(feenox_free_elemental_objects());
  
  if (feenox.pde.problem_family == problem_family_mechanical) {
    
    feenox_function_clean_nodal_data(feenox.pde.sigma1);
    feenox_function_clean_nodal_data(feenox.pde.sigma2);
    feenox_function_clean_nodal_data(feenox.pde.sigma3);
    feenox_function_clean_nodal_data(feenox.pde.sigma);
    feenox_function_clean_nodal_data(feenox.pde.delta_sigma);
    feenox_function_clean_nodal_data(feenox.pde.tresca);
    
  }
  
  feenox.pde.n_dirichlet_rows = 0;
  feenox_free(feenox.pde.dirichlet_indexes);
  feenox_free(feenox.pde.dirichlet_values);
     
  if (feenox.pde.phi != PETSC_NULL) {
    petsc_call(VecDestroy(&feenox.pde.phi));
  }
  if (feenox.pde.K != PETSC_NULL) {
    petsc_call(MatDestroy(&feenox.pde.K));
  }
  if (feenox.pde.K_nobc != PETSC_NULL) {
    petsc_call(MatDestroy(&feenox.pde.K_nobc));
  }
  
  if (feenox.pde.J != PETSC_NULL) {
    petsc_call(MatDestroy(&feenox.pde.J));
  }
  if (feenox.pde.M != PETSC_NULL) {
    petsc_call(MatDestroy(&feenox.pde.M));
  }
  if (feenox.pde.b != PETSC_NULL) {
    petsc_call(VecDestroy(&feenox.pde.b));
  }
  if (feenox.pde.b_nobc != PETSC_NULL) {
    petsc_call(VecDestroy(&feenox.pde.b_nobc));
  }

  // mind the order!
  if (feenox.pde.ts != PETSC_NULL) {
    petsc_call(TSDestroy(&feenox.pde.ts));
  }
  if (feenox.pde.snes != PETSC_NULL) {
    petsc_call(SNESDestroy(&feenox.pde.snes));
  }
  if (feenox.pde.ksp != PETSC_NULL) {
    petsc_call(KSPDestroy(&feenox.pde.ksp));
  }
  
#ifdef HAVE_SLEPC  
  if (feenox.pde.eps != PETSC_NULL) {
//    petsc_call(EPSDestroy(&feenox.pde.eps));
  }
#endif
  
  return WASORA_RUNTIME_OK;

}

int feenox_function_clean_nodal_data(function_t *function) {
 
  if (function != NULL && function->data_value != NULL) {  
    feenox_free(function->data_value);
    function->data_value = NULL;
  }
  
  return 0;
}

int feenox_function_clean_nodal_arguments(function_t *function) {
 
  int d;

  if (function->data_argument != NULL) {
    for (d = 0; d < feenox.pde.dimensions; d++) {
      feenox_free(function->data_argument[d]);
    }
    feenox_free(function->data_argument);
  }
  
  return 0;
}

int feenox_define_result_function(char *name, function_t **function) {

  // aca la definimos para que este disponible en tiempo de parseo  
  if ((*function = feenox_define_function(name, feenox.pde.dimensions)) == NULL) {
    feenox_push_error_message("result function '%s' defined twice", name);
    return FEENOX_ERROR;
  }
  (*function)->mesh = feenox.pde.mesh; // esto puede cambiar a rough despues  
  feenox_function_clean_nodal_arguments(*function);
  (*function)->var_argument = feenox.pde.solution[0]->var_argument;
  (*function)->type = type_pointwise_mesh_node;

  return 0;
}
*/
