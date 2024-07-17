/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's PDE initialization routines
 *
 *  Copyright (C) 2015--2023 Seamplex
 *
 *  This file is part of FeenoX <https://www.seamplex.com/feenox>.
 *
 *  feenox is free software: you can redistribute it and/or modify
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
 *  along with FeenoX.  If not, see <https://www.gnu.org/licenses/>.
 *------------------- ------------  ----    --------  --     -       -         -
 */
#include "feenox.h"

#include <signal.h>   // for signal
#include <unistd.h>   // for sysconf
#include <stdlib.h>   // for setenv

int feenox_problem_parse_time_init(void) {

#ifdef HAVE_PETSC
  if (feenox.pde.petscinit_called == PETSC_TRUE) {
    return FEENOX_OK;
  }
  
  if (sizeof(PetscReal) != sizeof(PetscScalar)) {
    feenox_push_error_message("PETSc should be compiled with real scalar types and we have PetscReal = %d and PetscScalar = %d", sizeof(PetscReal), sizeof(PetscScalar));
    return FEENOX_ERROR;
  }  
  if (sizeof(PetscScalar) != sizeof(double)) {
    feenox_push_error_message("PETSc should be compiled with double-precision real scalar types and we have double = %d and PetscScalar = %d", sizeof(double), sizeof(PetscReal));
    return FEENOX_ERROR;
  }

  // we already have processed basic options, now we loop over the original argv and convert
  // double-dash options to single-dash so --snes_view transforms into -snes_view
  // and split equal signs into two arguments, i.e. --mg_levels_pc_type=sor transforms into
  // -mg_levels_pc_type and sor separately
  int petsc_argc = 0;
  char **petsc_argv = NULL;
  if (feenox.argc != 0) {  // in benchmark argc might be zero
    // worst-case scenario: all strings have arguments that we have to split into two
    feenox_check_alloc(petsc_argv = calloc(2*feenox.argc, sizeof(char *)));
    petsc_argv[0] = feenox.argv_orig[0];
    for (int i = 0; i < feenox.argc; i++) {
      if (strlen(feenox.argv_orig[i]) > 2 && feenox.argv_orig[i][0] == '-' && feenox.argv_orig[i][1] == '-') {
        feenox_check_alloc(petsc_argv[++petsc_argc] = strdup(feenox.argv_orig[i]+1));
        char *value = NULL;
        if ((value = strchr(petsc_argv[petsc_argc], '=')) != NULL) {
          *value = '\0';
          petsc_argv[++petsc_argc] = strdup(value+1);
        }
      }
    }
    // this one takes into account argv[0]
    petsc_argc++;
  }
  
  PetscInt major = 0;
  PetscInt minor = 0;
  PetscInt subminor = 0;
#ifdef HAVE_SLEPC  
  // initialize SLEPc (which in turn initializes PETSc) with the original argv & argc
  petsc_call(SlepcInitialize(&petsc_argc, &petsc_argv, (char*)0, PETSC_NULLPTR));
  
  // check the headers correspond to the runtime
  petsc_call(SlepcGetVersionNumber(&major, &minor, &subminor, NULL));
  if (major != SLEPC_VERSION_MAJOR || minor != SLEPC_VERSION_MINOR || subminor != SLEPC_VERSION_SUBMINOR) {
    feenox_push_error_message("linked against SLEPc %d.%d.%d but using headers from %d.%d.%d", major, minor, subminor, SLEPC_VERSION_MAJOR, SLEPC_VERSION_MINOR, SLEPC_VERSION_SUBMINOR);
    return FEENOX_ERROR;
  }
#else
  // initialize PETSc
  petsc_call(PetscInitialize(&petsc_argc, &petsc_argv, (char*)0, PETSC_NULLPTR));
#endif
  feenox_free(petsc_argv);

  // check the headers correspond to the runtime
  petsc_call(PetscGetVersionNumber(&major, &minor, &subminor, NULL));
  if (major != PETSC_VERSION_MAJOR || minor != PETSC_VERSION_MINOR || subminor != PETSC_VERSION_SUBMINOR) {
    feenox_push_error_message("linked against PETSc %d.%d.%d but using headers from %d.%d.%d", major, minor, subminor, PETSC_VERSION_MAJOR, PETSC_VERSION_MINOR, PETSC_VERSION_SUBMINOR);
    return FEENOX_ERROR;
  }
  
  feenox.pde.petscinit_called = PETSC_TRUE;
  
  petsc_call(MPI_Comm_size(PETSC_COMM_WORLD, &feenox.mpi_size));
  feenox_special_var_value(mpi_size) = (double)feenox.mpi_size;
  
  petsc_call(MPI_Comm_rank(PETSC_COMM_WORLD, &feenox.mpi_rank));
  feenox_special_var_value(mpi_rank) = (double)feenox.mpi_rank;
  
  petsc_call(PetscLogStageRegister("init", &feenox.pde.stage_init));
  petsc_call(PetscLogStageRegister("build", &feenox.pde.stage_build));
  petsc_call(PetscLogStageRegister("solve", &feenox.pde.stage_solve));
  petsc_call(PetscLogStageRegister("post", &feenox.pde.stage_post));
  
  // segfaults are segfaults, try to leave PETSC out of them
  signal(SIGSEGV, SIG_DFL);
  
  // TODO: install out error handler for PETSc
//  petsc_call(PetscPushErrorHandler(&feenox_handler, NULL));

///va+ksp_atol+name ksp_atol
///va+ksp_atol+detail Absolute tolerance of the iterative linear solver, as passed to PETSc’s
///va+ksp_atol+detail [`KSPSetTolerances`](https://www.mcs.anl.gov/petsc/petsc-current/docs/manualpages/KSP/KSPSetTolerances.html)
  feenox.pde.vars.ksp_atol = feenox_define_variable_get_ptr("ksp_atol");
  // TODO: set to PETSC_DEFAULT?
///va+ksp_atol+detail Default `1e-50`.
  feenox_var_value(feenox.pde.vars.ksp_atol) = 1e-50;   // same as PETSc
 
///va+ksp_rtol+name ksp_rtol
///va+ksp_rtol+detail Relative tolerance of the iterative linear solver,
///va+ksp_rtol+detail as passed to PETSc’s
///va+ksp_rtol+detail [`KSPSetTolerances`](https://www.mcs.anl.gov/petsc/petsc-current/docs/manualpages/KSP/KSPSetTolerances.html).
feenox.pde.vars.ksp_rtol = feenox_define_variable_get_ptr("ksp_rtol");
///va+ksp_rtol+detail Default `1e-6`.
  feenox_var_value(feenox.pde.vars.ksp_rtol) = 1e-6;    // PETSc is 1e-5
  
///va+ksp_divtol+name ksp_divtol
///va+ksp_divtol+detail Divergence tolerance of the iterative linear solver,
///va+ksp_divtol+detail as passed to PETSc’s
///va+ksp_divtol+detail [`KSPSetTolerances`](https://www.mcs.anl.gov/petsc/petsc-current/docs/manualpages/KSP/KSPSetTolerances.html).
  feenox.pde.vars.ksp_divtol = feenox_define_variable_get_ptr("ksp_divtol");
///va+ksp_divtol+detail Default `1e+4`.  
  feenox_var_value(feenox.pde.vars.ksp_divtol) = 1e+4;  // same as PETSc
  
///va+ksp_max_it+name ksp_max_it
///va+ksp_max_it+detail Number of maximum iterations of the iterative linear solver before diverging,
///va+ksp_max_it+detail as passed to PETSc’s
///va+ksp_max_it+detail [`KSPSetTolerances`](https://www.mcs.anl.gov/petsc/petsc-current/docs/manualpages/KSP/KSPSetTolerances.html).
  feenox.pde.vars.ksp_max_it = feenox_define_variable_get_ptr("ksp_max_it");
///va+ksp_max_it+detail Default `10,000`.
  feenox_var_value(feenox.pde.vars.ksp_max_it) = 10000;   // same as PETSc


///va+snes_atol+name snes_atol
///va+snes_atol+detail Absolute tolerance of the non-linear solver, as passed to PETSc’s
///va+snes_atol+detail [`SNESSetTolerances`](https://www.mcs.anl.gov/petsc/petsc-current/docs/manualpages/SNES/SNESSetTolerances.html)
  feenox.pde.vars.snes_atol = feenox_define_variable_get_ptr("snes_atol");
///va+snes_atol+detail Default `1e-50`.
  feenox_var_value(feenox.pde.vars.snes_atol) = 1e-50;   // same as PETSc
 
///va+snes_rtol+name snes_rtol
///va+snes_rtol+detail Relative tolerance of the non-linear solver,
///va+snes_rtol+detail as passed to PETSc’s
///va+snes_rtol+detail [`SNESSetTolerances`](https://www.mcs.anl.gov/petsc/petsc-current/docs/manualpages/SNES/SNESSetTolerances.html).
feenox.pde.vars.snes_rtol = feenox_define_variable_get_ptr("snes_rtol");
///va+feenox_snes_rtol+detail Default `1e-8`.
  feenox_var_value(feenox.pde.vars.snes_rtol) = 1e-8;    // same as PETSc
  
///va+snes_stol+name snes_stol
///va+snes_stol+detail Convergence tolerance of the non-linear solver in terms of the norm of the change in the solution between steps,
///va+snes_stol+detail as passed to PETSc’s
///va+snes_stol+detail [`SNESSetTolerances`](https://www.mcs.anl.gov/petsc/petsc-current/docs/manualpages/SNES/SNESSetTolerances.html).
  feenox.pde.vars.snes_stol = feenox_define_variable_get_ptr("snes_stol");
///va+snes_stol+detail Default `1e-8`.  
  feenox_var_value(feenox.pde.vars.snes_stol) = 1e-8;  // same as PETSc

///va+snes_max_it+name snes_max_it
///va+snes_max_itdetail Number of maximum iterations of the non-linear solver before diverging,
///va+snes_max_it+detail as passed to PETSc’s
///va+snes_max_it+detail [`SNESSetTolerances`](https://www.mcs.anl.gov/petsc/petsc-current/docs/manualpages/SNES/SNESSetTolerances.html).
  feenox.pde.vars.snes_max_it = feenox_define_variable_get_ptr("snes_max_it");
///va+snes_max_it+detail Default `50`.
  feenox_var_value(feenox.pde.vars.snes_max_it) = 50;   // same as PETSc


  
///va+ts_atol+name ts_atol
///va+ts_atol+detail Absolute tolerance of the transient solver, as passed to PETSc’s
///va+ts_atol+detail [`TSSetTolerances`](https://petsc.org/release/docs/manualpages/TS/TSSetTolerances.html)
  feenox.pde.vars.ts_atol = feenox_define_variable_get_ptr("ts_atol");
///va+ts_rtol+detail Default `1e-4`.
  feenox_var_value(feenox.pde.vars.ts_atol) = 1e-4;   // same as PETSc
 
///va+ts_rtol+name ts_rtol
///va+ts_rtol+detail Relative tolerance of the transient solver, as passed to PETSc’s
///va+ts_rtol+detail [`TSSetTolerances`](https://petsc.org/release/docs/manualpages/TS/TSSetTolerances.html).
feenox.pde.vars.ts_rtol = feenox_define_variable_get_ptr("ts_rtol");
///va+ts_rtol+detail Default `1e-4`.
  feenox_var_value(feenox.pde.vars.ts_rtol) = 1e-4;    // same as PETSc
  
///va+eps_tol+name eps_tol
///va+eps_tol+detail Tolerance (relative to the matrix norms) of the eigen solver,
///va+ksp_rtol+detail as passed to SLEPc’s
///va+ksp_rtol+detail [`EPSSetTolerances`](https://slepc.upv.es/documentation/current/docs/manualpages/EPS/EPSSetTolerances.html).
feenox.pde.vars.eps_tol = feenox_define_variable_get_ptr("eps_tol");
///va+ksp_rtol+detail Default `1e-8`.
  feenox_var_value(feenox.pde.vars.eps_tol) = 1e-8;    // same as SLEPc
  
///va+eps_max_it+name eps_max_it
///va+eps_max_it Number of maximum iterations allowed in the eigen solver,
///va+eps_max_it+detail as passed to SLEPc’s
///va+ksp_rtol+detail [`EPSSetTolerances`](https://slepc.upv.es/documentation/current/docs/manualpages/EPS/EPSSetTolerances.html).
  feenox.pde.vars.eps_max_it = feenox_define_variable_get_ptr("eps_max_it");
///va+snes_max_it+detail Default is a solver-dependent reasonable value`.
  feenox_var_value(feenox.pde.vars.eps_max_it) = PETSC_DEFAULT;

///va+eps_st_sigma+name eps_st_sigma
///va+eps_st_sigma Shift $\sigma$ associated with the spectral transformation. 
///va+eps_st_sigma+detail as passed to SLEPc’s
///va+eps_st_sigma+detail [`STSetShift`](https://slepc.upv.es/documentation/current/docs/manualpages/ST/STSetShift.html).
  feenox.pde.vars.eps_st_sigma = feenox_define_variable_get_ptr("eps_st_sigma");
///va+eps_st_sigma+detail Default is zero (but some PDEs can change it).
  feenox_var_value(feenox.pde.vars.eps_st_sigma) = 0;
  
///va+eps_st_nu+name eps_st_nu
///va+eps_st_nu Value $\nu$ of the anti-shift for the Cayley spectral transformation
///va+eps_st_nu+detail as passed to SLEPc’s
///va+eps_st_nu+detail [`STCayleySetAntishift`](https://slepc.upv.es/documentation/current/docs/manualpages/ST/STCayleySetAntishift.html).
  feenox.pde.vars.eps_st_nu = feenox_define_variable_get_ptr("eps_st_nu");
///va+eps_st_nu+detail Default is zero (but some PDEs can change it).
  feenox_var_value(feenox.pde.vars.eps_st_nu) = 0;
  
///va+mumps_icntl_14+name mumps_icntl_14
///va+mumps_icntl_14 MUMPS icntl value with the offset 14, relaxation value for the internal array.
///va+mumps_icntl_14+detail For some complex problems this value needs to be set. See MUMPS manual for details.
  feenox.pde.vars.mumps_icntl_14 = feenox_define_variable_get_ptr("mumps_icntl_14");
///va+mumps_icntl_14+detail Default is zero, which means use MUMPS default.
  feenox_var_value(feenox.pde.vars.mumps_icntl_14) = 0;
  
///va+gamg_threshold+name gamg_threshold
///va+gamg_threshold Relative threshold to use for dropping edges in aggregation graph when using GAMG.
///va+gamg_threshold+detail See `PCGAMGSetThreshold()` in the PETSc documentation for further details.
  feenox.pde.vars.gamg_threshold = feenox_define_variable_get_ptr("gamg_threshold");
///va+gamg_threshold+detail Default is zero (note that PETSc's default is -1).
  feenox_var_value(feenox.pde.vars.gamg_threshold) = 0;

///va+penalty_weight+name feenox_penalty_weight
///va+penalty_weight+detail The weight\ $w$ used when setting multi-freedom boundary conditions.
///va+penalty_weight+detail Higher values mean better precision in the constrain but worsen
///va+penalty_weight+detail the matrix condition number. 
  feenox.pde.vars.penalty_weight = feenox_define_variable_get_ptr("penalty_weight");
///va+feenox_penalty_weight+detail Default is `1e8`.
  feenox_var_value(feenox.pde.vars.penalty_weight) = 1e8;  
  
///va+nodes_rough+name nodes_rough
///va+nodes_rough+detail The number of nodes of the mesh in `ROUGH` mode.
  feenox.pde.vars.nodes_rough = feenox_define_variable_get_ptr("nodes_rough");

///va+total_dofs+name total_dofs
///va+total_dogs+detail The total number of degrees of freedom of the PDE being solved for.
  feenox.pde.vars.total_dofs = feenox_define_variable_get_ptr("total_dofs");
  
///va+memory_available+name memory_available
///va+memory_available+detail Total available memory, in bytes.
  feenox.pde.vars.memory_available = feenox_define_variable_get_ptr("memory_available");
#ifdef HAVE_SYSCONF  
  feenox_var_value(feenox.pde.vars.memory_available) = (double)(sysconf(_SC_PHYS_PAGES)*sysconf(_SC_PAGESIZE));
#else
  feenox_var_value(feenox.pde.vars.memory_available) = -1;
#endif
  
#endif  
  return FEENOX_OK;
}



int feenox_problem_define_solutions(void) {
  
#ifdef HAVE_PETSC
  char dimension_name[3][2] = {"x", "y", "z"};
  if (feenox.pde.dim == 0) {
    feenox_push_error_message("do not know how many dimensions the problem has, tell me with DIMENSIONS in either PROBLEM or READ_MESH");
    return FEENOX_ERROR;
  }
  if (feenox.pde.dofs == 0) {
    feenox_push_error_message("do not know how many degrees of freedom this problem has");
    return FEENOX_ERROR;
  }
  
  if (feenox.pde.solution != NULL) {
    feenox_push_error_message("solutions already defined");
    return FEENOX_ERROR;
  }
  feenox_check_alloc(feenox.pde.solution = calloc(feenox.pde.dofs, sizeof(function_t *)));
  feenox_check_alloc(feenox.pde.gradient = calloc(feenox.pde.dofs, sizeof(function_t *)));
  feenox_check_alloc(feenox.pde.delta_gradient = calloc(feenox.pde.dofs, sizeof(function_t *)));
  if (feenox.pde.nev > 0) {
    feenox_check_alloc(feenox.pde.mode = calloc(feenox.pde.dofs, sizeof(function_t *)));
  }

  for (unsigned int g = 0; g < feenox.pde.dofs; g++) {
    char *name = NULL;
    if (feenox.pde.unknown_name == NULL) {
      feenox_check_minusone(asprintf(&name, "phi%u", g+1));
    } else {
      feenox_check_minusone(asprintf(&name, "%s", feenox.pde.unknown_name[g]));
    }
    feenox_check_alloc(feenox.pde.solution[g] = feenox_define_function_get_ptr(name, feenox.pde.dim));
    feenox.pde.solution[g]->mesh = feenox.pde.mesh;

    if (feenox.pde.nev == 0) {
      // the derivatives of the solutions with respect to space
      feenox_check_alloc(feenox.pde.gradient[g] = calloc(feenox.pde.dim, sizeof(function_t *)));
      feenox_check_alloc(feenox.pde.delta_gradient[g] = calloc(feenox.pde.dim, sizeof(function_t *)));
      
      for (unsigned int d = 0; d < feenox.pde.dim; d++) {
        feenox.pde.solution[g]->var_argument[d] = feenox.mesh.vars.arr_x[d];
      
        char *gradname = NULL;
        feenox_check_minusone(asprintf(&gradname, "d%sd%s", name, dimension_name[d]));
        feenox_check_alloc(feenox.pde.gradient[g][d] = feenox_define_function_get_ptr(gradname, feenox.pde.dim));
        feenox_free(gradname);
        
        feenox.pde.gradient[g][d]->mesh = feenox.pde.solution[g]->mesh;
        feenox.pde.gradient[g][d]->var_argument = feenox.pde.solution[g]->var_argument;
        feenox.pde.gradient[g][d]->type = function_type_pointwise_mesh_node;
        feenox.pde.gradient[g][d]->spatial_derivative_of = feenox.pde.solution[g];
        // this flag is used when interpolating
        feenox.pde.gradient[g][d]->spatial_derivative_with_respect_to = d;
        // this flag is used to know if gradients have to be computed
        feenox.pde.gradient[g][d]->is_gradient = 1;
        
        // same for uncertainty
/*        
        feenox_check_minusone(asprintf(&gradname, "delta_d%sd%s", name, dimension_name[d]));
        feenox_check_alloc(feenox.pde.delta_gradient[g][d] = feenox_define_function_get_ptr(gradname, feenox.pde.dim));
        feenox_free(gradname);
        
        feenox.pde.delta_gradient[g][d]->mesh = feenox.pde.solution[g]->mesh;
        feenox.pde.delta_gradient[g][d]->var_argument = feenox.pde.solution[g]->var_argument;
        feenox.pde.delta_gradient[g][d]->type = function_type_pointwise_mesh_node;
        feenox.pde.delta_gradient[g][d]->is_gradient = 1;
*/
      }
      
    } else {  
      
      // there are many solution functions in modal
      feenox_check_alloc(feenox.pde.mode[g] = calloc(feenox.pde.nev, sizeof(function_t *)));
      for (int i = 0; i < feenox.pde.nev; i++) {
        char *modename = NULL;
        feenox_check_minusone(asprintf(&modename, "%s%d", name, i+1));
        feenox_call(feenox_problem_define_solution_function(modename, &feenox.pde.mode[g][i], FEENOX_SOLUTION_NOT_GRADIENT));
        feenox_free(modename);
        
        feenox.pde.mode[g][i]->mesh = feenox.pde.solution[g]->mesh;
        feenox.pde.mode[g][i]->var_argument = feenox.pde.solution[g]->var_argument;
        
      }
    }
    feenox_free(name);
  }
#endif
  return FEENOX_OK;
}


int feenox_problem_define_solution_function(const char *name, function_t **function, int is_gradient) {

  // we can only define what's available at parse time
  if ((*function = feenox_define_function_get_ptr(name, feenox.pde.dim)) == NULL) {
    feenox_push_error_message("result function '%s' defined twice", name);
    return FEENOX_ERROR;
  }
  // we don't have a valid mesh here, do we?
//  (*function)->mesh = feenox.pde.mesh; // esto puede cambiar a rough despues  
//  feenox_problem_define_solution_clean_nodal_arguments(*function);
  if (feenox.pde.solution == NULL) {
    feenox_call(feenox_problem_define_solutions());
  }
  
  (*function)->var_argument = feenox.pde.solution[0]->var_argument;
  (*function)->type = (feenox.mesh.default_field_location == field_location_cells) ? function_type_pointwise_mesh_cell : function_type_pointwise_mesh_node;
  (*function)->is_gradient = is_gradient;

  return FEENOX_OK;
}

int feenox_problem_fill_aux_solution(function_t *function) {
  function->mesh = feenox.pde.rough==0 ? feenox.pde.mesh : feenox.pde.mesh_rough;
  function->data_size = function->mesh->n_nodes;
  feenox_call(feenox_create_pointwise_function_vectors(function));
  for (unsigned int d = 0; d < function->n_arguments; d++) {
    feenox_realloc_vector_ptr(function->vector_argument[d], gsl_vector_ptr(function->mesh->nodes_argument[d], 0), 0);
  }

  return FEENOX_OK;
}

int feenox_problem_init_runtime_general(void) {

#ifdef HAVE_PETSC

  // first set options from PETSC_OPTIONS
  if (feenox.pde.petsc_options != NULL) {
    petsc_call(PetscOptionsInsertString(NULL, feenox.pde.petsc_options));
  }
  
  // command-line arguments take precedence over the options in the input file
  // so we have to read them here and overwrite what he have so far
  // recall that we alreay stripped off one dash from the original argv array

  PetscBool flag = PETSC_FALSE;
  
///op+progress+option `--progress`
///op+progress+desc print ASCII progress bars when solving PDEs
  if (feenox.pde.progress_ascii == PETSC_FALSE) {
    petsc_call(PetscOptionsHasName(PETSC_NULLPTR, PETSC_NULLPTR, "-progress", &feenox.pde.progress_ascii));
  }  

///op+mumps+option `--mumps`
///op+mumps+desc ask PETSc to use the direct linear solver MUMPS
  petsc_call(PetscOptionsHasName(PETSC_NULLPTR, PETSC_NULLPTR, "-mumps", &flag));
  if (flag == PETSC_TRUE) {
    feenox.pde.ksp_type = strdup("mumps");
    feenox.pde.pc_type = strdup("mumps");
  }

// read these guys just to avoid the "unused database option" complain
  petsc_call(PetscOptionsHasName(PETSC_NULLPTR, PETSC_NULLPTR, "-linear", &flag));
  petsc_call(PetscOptionsHasName(PETSC_NULLPTR, PETSC_NULLPTR, "-non-linear", &flag));
  petsc_call(PetscOptionsHasName(PETSC_NULLPTR, PETSC_NULLPTR, "-nonlinear", &flag));
  
  // check if the dimensions match
  if (feenox.pde.dim != 0 && feenox.pde.mesh != NULL && feenox.pde.mesh->dim != 0 && feenox.pde.dim != feenox.pde.mesh->dim) {
    feenox_push_error_message("dimension mismatch, in PROBLEM %d != in READ_MESH %d", feenox.pde.dim, feenox.pde.mesh->dim);
    return FEENOX_ERROR;
  }
  
  // conversely, if we got them there, put it on the mesh
  if (feenox.pde.mesh != NULL && feenox.pde.mesh->dim == 0 && feenox.pde.dim != 0) {
    feenox.pde.mesh->dim = feenox.pde.dim;
  }

  if (feenox.pde.dim == 0) {
    feenox_push_error_message("could not determine the dimension of the problem, give them using DIMENSIONS in either READ_MESH or PROBLEM");
    return FEENOX_ERROR;
  }
  
  // link the solution functions with the mesh
  // and check if we have to compute gradients
  for (unsigned int g = 0; g < feenox.pde.dofs; g++) {
    for (unsigned int m = 0; m < feenox.pde.dim; m++) {
      feenox.pde.solution[g]->var_argument[m] = feenox.mesh.vars.arr_x[m];
      if (feenox.pde.gradient != NULL && feenox.pde.gradient[g] != NULL && feenox.pde.gradient[g][m] != NULL) {
         feenox.pde.compute_gradients |= feenox.pde.gradient[g][m]->used;
      }   
    }
    feenox.pde.solution[g]->mesh = (feenox.pde.rough==0) ? feenox.pde.mesh : feenox.pde.mesh_rough;
    feenox_check_alloc(feenox.pde.solution[g]->var_argument = calloc(feenox.pde.dim, sizeof(var_t *)));
    feenox.pde.solution[g]->var_argument[0] = feenox.mesh.vars.x;
    if (feenox.pde.dim > 1) {
      feenox.pde.solution[g]->var_argument[1] = feenox.mesh.vars.y;
      if (feenox.pde.dim > 2) {
        feenox.pde.solution[g]->var_argument[2] = feenox.mesh.vars.z;
      }
    }
    feenox.pde.solution[g]->var_argument_allocated = 1;
    feenox.pde.solution[g]->type = function_type_pointwise_mesh_node;
  }  


  if (feenox.pde.mesh != NULL) {
    // this is a loop over a hash, not over a linked list
    for (physical_group_t *physical_group = feenox.pde.mesh->physical_groups; physical_group != NULL; physical_group = physical_group->hh.next) {
      if (physical_group->bcs != NULL && physical_group->n_elements == 0) {
        feenox_push_error_message("physical entity '%s' has a BC but no associated elements", physical_group->name);
        return FEENOX_ERROR;
      }

      if (physical_group->material != NULL && physical_group->n_elements == 0) {
        feenox_push_error_message("physical group '%s' has a material but no associated elements", physical_group->name);
        return FEENOX_ERROR;
      }
    }

    if (feenox.pde.unresolved_bcs == unresolved_bcs_detect) {
      // this is a loop over a hash, not over a linked list
      for (bc_t *bc = feenox.mesh.bcs; bc != NULL; bc = bc->hh.next) {
        if (bc->has_explicit_groups == 0) {
          physical_group_t *physical_group;
          HASH_FIND_STR(feenox.pde.mesh->physical_groups, bc->name, physical_group);
    
          if (physical_group == NULL) {
            feenox_push_error_message("boundary condition '%s' does not have a physical group in mesh file '%s'. Set ALLOW_UNRESOLVED_BCS if you want to skip this check.", bc->name, feenox.pde.mesh->file->path);
            return FEENOX_ERROR;
          }
      
          if (physical_group->n_elements == 0) {
            feenox_push_error_message("physical group for boundary condition '%s' does not have any elements. Set ALLOW_UNRESOLVED_BCS if you want to skip this check.", bc->name);
            return FEENOX_ERROR;
          }
        }
      }
    }
  }

  // allocate global petsc objects
  if (feenox.pde.size_global == 0) {
    if (feenox.pde.spatial_unknowns == 0) {
      feenox_push_error_message("internal error, problem init did not set global spatial unknowns");
      return FEENOX_ERROR;
    }
    feenox.pde.size_global = feenox.pde.spatial_unknowns * feenox.pde.dofs;
  }

  // set the size of the eigenvectors (we did not know their size in init_parser() above
  for (int i = 0; i < feenox.pde.nev; i++) {
    feenox_call(feenox_vector_set_size(feenox.pde.vectors.phi[i], feenox.pde.size_global));
  }

  
  
  feenox_var_value(feenox.pde.vars.total_dofs) = (double)(feenox.pde.size_global);
  
  // TODO: choose width from input
//#if PETSC_VERSION_LT(3,19,0)
  feenox.pde.width = GSL_MAX(feenox.pde.mesh->max_nodes_per_element, feenox.pde.mesh->max_first_neighbor_nodes) * feenox.pde.dofs;
//#endif
  
  // ask how many local nodes we own
  feenox.pde.nodes_local = PETSC_DECIDE;
  PetscInt n_nodes = feenox.pde.mesh->n_nodes;
  petsc_call(PetscSplitOwnership(PETSC_COMM_WORLD, &feenox.pde.nodes_local, &n_nodes));
  feenox.pde.size_local = feenox.pde.dofs * feenox.pde.nodes_local;
  
  // the global stiffnes matrix
  feenox_check_alloc(feenox.pde.K = feenox_problem_create_matrix("K"));
  
  // the solution (unknown) vector
  feenox.pde.phi = feenox_problem_create_vector("phi");
  // explicit initial value
  petsc_call(VecSet(feenox.pde.phi, 0));

  // the right-hand-side vector
  if (feenox.pde.has_rhs) {
    feenox.pde.b = feenox_problem_create_vector("b");
  }
  
  // the mass matrix for modal or heat transient
  if (feenox.pde.has_mass) {
    feenox_check_alloc(feenox.pde.M = feenox_problem_create_matrix("M"));
  }
  
  if (feenox.pde.has_jacobian_K) {
    feenox_check_alloc(feenox.pde.JK = feenox_problem_create_matrix("JK"));
  }
  if (feenox.pde.has_jacobian_b) {
    feenox_check_alloc(feenox.pde.Jb = feenox_problem_create_matrix("Jb"));
  }
  
  // ask for the local ownership range
  petsc_call(MatGetOwnershipRange(feenox.pde.K, &feenox.pde.first_row, &feenox.pde.last_row));
  feenox.pde.first_node = feenox.pde.first_row / feenox.pde.dofs;
  feenox.pde.last_node = feenox.pde.last_row / feenox.pde.dofs;
  
  // TODO: honor mesh partitions
  // https://lists.mcs.anl.gov/pipermail/petsc-users/2014-April/021433.html
  if (feenox.pde.mesh->mpi_matches_partitions == mpi_matches_partitions_no) {
    feenox_call(feenox_compute_first_last_element(feenox.pde.mesh));
  }

  // fill in the holders of the continuous functions that will hold the solution
  
  if (feenox.pde.rough == 0) {
    if (feenox.pde.solution == NULL) {
      feenox_push_error_message("inconsistent internal state, solution functions not allocated");
      return FEENOX_ERROR;
    }
    for (unsigned int g = 0; g < feenox.pde.dofs; g++) {
      feenox.pde.solution[g]->mesh = feenox.pde.mesh;
      feenox.pde.solution[g]->data_size = feenox.pde.spatial_unknowns;
      feenox_create_pointwise_function_vectors(feenox.pde.solution[g]);
      for (unsigned int d = 0; d < feenox.pde.solution[g]->n_arguments; d++) {
        feenox_realloc_vector_ptr(feenox.pde.solution[g]->vector_argument[d], gsl_vector_ptr(feenox.pde.solution[g]->mesh->nodes_argument[d], 0), 0);
      }
    
      // in some cases (e.g. neutron) we do not know if we have to solve
      // a KSP or an EPS until we read the material data, which is far too late
      // in that case, we usually do not care about all the modes so feenox.pde.mode is null
      if (feenox.pde.nev > 0 && feenox.pde.mode != NULL) {
        unsigned int i = 0;
        for (i = 0; i < feenox.pde.nev; i++) {
          feenox.pde.mode[g][i]->mesh = feenox.pde.mesh;
          feenox.pde.mode[g][i]->data_size = feenox.pde.mesh->n_nodes;
          feenox_create_pointwise_function_vectors(feenox.pde.mode[g][i]);
          for (unsigned int d = 0; d < feenox.pde.mode[g][i]->n_arguments; d++) {
            feenox_realloc_vector_ptr(feenox.pde.mode[g][i]->vector_argument[d], gsl_vector_ptr(feenox.pde.mode[g][i]->mesh->nodes_argument[d], 0), 0);
          }
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
#endif
  
  return FEENOX_OK;
}

#ifdef HAVE_PETSC
Mat feenox_problem_create_matrix(const char *name) {
  
  Mat A = NULL;
  petsc_call_null(MatCreate(PETSC_COMM_WORLD, &A));
  petsc_call_null(MatSetSizes(A, feenox.pde.size_local, feenox.pde.size_local, feenox.pde.size_global, feenox.pde.size_global));
  petsc_call_null(MatSetFromOptions(A));
  if (feenox.pde.pre_allocate == PETSC_TRUE || PETSC_VERSION_LT(3,19,0)) {
    petsc_call_null(MatMPIAIJSetPreallocation(A, feenox.pde.width, PETSC_NULLPTR, feenox.pde.width, PETSC_NULLPTR));
    petsc_call_null(MatSeqAIJSetPreallocation(A, feenox.pde.width, PETSC_NULLPTR));
  }

  // this flag needs the matrix type to be set, and we had just set it with setfromoptions   
  petsc_call_null(MatSetOption(A, MAT_KEEP_NONZERO_PATTERN, PETSC_TRUE));

  if (name != NULL) {
    petsc_call_null(PetscObjectSetName((PetscObject)(A), name));
  }
  
  if (feenox.pde.allow_new_nonzeros == PETSC_TRUE) {
    petsc_call_null(MatSetOption(A, MAT_NEW_NONZERO_ALLOCATION_ERR, PETSC_FALSE));
  }  
  
  if (feenox.pde.dofs > 1) {
    petsc_call_null(MatSetBlockSize(A, feenox.pde.dofs));
  }  

  return A;  
}

Vec feenox_problem_create_vector(const char *name) {
  
  if (feenox.pde.K == NULL) {
    feenox_push_error_message("stiffness matrix is not created yet");
    return NULL;
  }
  
  Vec v;
  petsc_call_null(MatCreateVecs(feenox.pde.K, &v, NULL));
  petsc_call_null(VecSetFromOptions(v));

  if (name != NULL) {
    petsc_call_null(PetscObjectSetName((PetscObject)(v), name));
  }
  
  return v;  
}
#endif

// TODO: see if PetscSplitOwnership(MPI_Comm comm, PetscInt *n, PetscInt *N) give the same results
int feenox_compute_first_last_element(mesh_t *mesh) {
  mesh->first_element = (mesh->n_elements / feenox.mpi_size) * feenox.mpi_rank;
  if (mesh->n_elements % feenox.mpi_size > feenox.mpi_rank) {
    mesh->first_element += feenox.mpi_rank;
    mesh->last_element = mesh->first_element + (mesh->n_elements / feenox.mpi_size) + 1;
  } else {  
    mesh->first_element += mesh->n_elements % feenox.mpi_size;
    mesh->last_element = mesh->first_element + (mesh->n_elements / feenox.mpi_size);
  }  
  
  return FEENOX_OK;
}
