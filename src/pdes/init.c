/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's PDE initialization routines
 *
 *  Copyright (C) 2015--2022 Seamplex
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
 *  along with feenox.  If not, see <https://www.gnu.org/licenses/>.
 *------------------- ------------  ----    --------  --     -       -         -
 */
#include "feenox.h"
extern feenox_t feenox;

#include <signal.h>   // for signal
#include <unistd.h>   // for sysconf


int feenox_problem_init_parser_general(void) {

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
    feenox_check_alloc(petsc_argv = calloc(feenox.argc, sizeof(char *)));
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
  // initialize SLEPc (which in turn initalizes PETSc) with the original argv & argc
  petsc_call(SlepcInitialize(&petsc_argc, &petsc_argv, (char*)0, PETSC_NULL));
  
  // check the headers correspond to the runtime
  petsc_call(SlepcGetVersionNumber(&major, &minor, &subminor, NULL));
  if (major != SLEPC_VERSION_MAJOR || minor != SLEPC_VERSION_MINOR || subminor != SLEPC_VERSION_SUBMINOR) {
    feenox_push_error_message("linked against SLEPc %d.%d.%d but using headers from %d.%d.%d", major, minor, subminor, SLEPC_VERSION_MAJOR, SLEPC_VERSION_MINOR, SLEPC_VERSION_SUBMINOR);
    return FEENOX_ERROR;
  }
 #else
  // initialize PETSc
  petsc_call(PetscInitialize(&petsc_argc, &petsc_argv, (char*)0, PETSC_NULL));
 #endif

  // check the headers correspond to the runtime
  petsc_call(PetscGetVersionNumber(&major, &minor, &subminor, NULL));
  if (major != PETSC_VERSION_MAJOR || minor != PETSC_VERSION_MINOR || subminor != PETSC_VERSION_SUBMINOR) {
    feenox_push_error_message("linked against PETSc %d.%d.%d but using headers from %d.%d.%d", major, minor, subminor, PETSC_VERSION_MAJOR, PETSC_VERSION_MINOR, PETSC_VERSION_SUBMINOR);
    return FEENOX_ERROR;
  }
  
  feenox.pde.petscinit_called = PETSC_TRUE;
  
  petsc_call(MPI_Comm_size(PETSC_COMM_WORLD, &feenox.n_procs));
  petsc_call(MPI_Comm_rank(PETSC_COMM_WORLD, &feenox.rank));

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
///va+ksp_rtol+detail [`STSetShift`](https://slepc.upv.es/documentation/current/docs/manualpages/ST/STSetShift.html).
  feenox.pde.vars.eps_st_sigma = feenox_define_variable_get_ptr("eps_st_sigma");
///va+snes_max_it+detail Default is zero.
  feenox_var_value(feenox.pde.vars.eps_st_sigma) = 0;
  
///va+eps_st_nu+name eps_st_nu
///va+eps_st_nu Value $\nu$ of the anti-shift for the Cayley spectral transformation
///va+eps_st_nu+detail as passed to SLEPc’s
///va+ksp_rtol+detail [`STCayleySetAntishift`](https://slepc.upv.es/documentation/current/docs/manualpages/ST/STCayleySetAntishift.html).
  feenox.pde.vars.eps_st_nu = feenox_define_variable_get_ptr("eps_st_nu");
///va+snes_max_it+detail Default is zero.
  feenox_var_value(feenox.pde.vars.eps_st_nu) = 0;
  
///va+gamg_threshold+name feenox_gamg_threshold
///va+gamg_threshold+detail Relative threshold to use for dropping edges in aggregation graph for the
///va+gamg_threshold+detail [Geometric Algebraic Multigrid Preconditioner](https://www.mcs.anl.gov/petsc/petsc-current/docs/manualpages/PC/PCGAMG.html)
///va+gamg_threshold+detail as passed to PETSc’s
///va+gamg_threshold+detail [`PCGAMGSetThreshold`](https://www.mcs.anl.gov/petsc/petsc-current/docs/manualpages/PC/PCGAMGSetThreshold.html).
///va+gamg_threshold+detail A value of 0.0 means keep all nonzero entries in the graph; negative means keep even zero entries in the graph.
  feenox.pde.vars.gamg_threshold = feenox_define_variable_get_ptr("gamg_threshold");
///va+feenox_gamg_threshold+detail Default `0.01`.  
  feenox_var_value(feenox.pde.vars.gamg_threshold) = 0.01;
  
///va+feenox_penalty_weight+name feenox_penalty_weight
///va+feenox_penalty_weight+detail The weight\ $w$ used when setting multi-freedom boundary conditions.
///va+feenox_penalty_weight+detail Higher values mean better precision in the constrain but worsen
///va+feenox_penalty_weight+detail the matrix condition number. 
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

    if (feenox.pde.nev == 0) {
      // the derivatives of the solutions with respect to space
      feenox_check_alloc(feenox.pde.gradient[g] = calloc(feenox.pde.dim, sizeof(function_t *)));
      feenox_check_alloc(feenox.pde.delta_gradient[g] = calloc(feenox.pde.dim, sizeof(function_t *)));
      
      unsigned int m = 0;
      for (m = 0; m < feenox.pde.dim; m++) {
        feenox.pde.solution[g]->var_argument[m] = feenox.mesh.vars.arr_x[m];
      
        char *gradname = NULL;
        feenox_check_minusone(asprintf(&gradname, "d%sd%s", name, dimension_name[m]));
        feenox_check_alloc(feenox.pde.gradient[g][m] = feenox_define_function_get_ptr(gradname, feenox.pde.dim));
        feenox_free(gradname);
        
        feenox.pde.gradient[g][m]->mesh = feenox.pde.solution[g]->mesh;
        feenox.pde.gradient[g][m]->var_argument = feenox.pde.solution[g]->var_argument;
        feenox.pde.gradient[g][m]->type = function_type_pointwise_mesh_node;
        feenox.pde.gradient[g][m]->spatial_derivative_of = feenox.pde.solution[g];
        // this flag is used when interpolating
        feenox.pde.gradient[g][m]->spatial_derivative_with_respect_to = m;
        // this flag is used to know if gradients have to be computed
        feenox.pde.gradient[g][m]->is_gradient = 1;
        
        // same for uncertainty
        feenox_check_minusone(asprintf(&gradname, "delta_d%sd%s", name, dimension_name[m]));
        feenox_check_alloc(feenox.pde.delta_gradient[g][m] = feenox_define_function_get_ptr(gradname, feenox.pde.dim));
        feenox_free(gradname);
        
        feenox.pde.delta_gradient[g][m]->mesh = feenox.pde.solution[g]->mesh;
        feenox.pde.delta_gradient[g][m]->var_argument = feenox.pde.solution[g]->var_argument;
        feenox.pde.delta_gradient[g][m]->type = function_type_pointwise_mesh_node;
        feenox.pde.delta_gradient[g][m]->is_gradient = 1;
      }
      
    } else {  
      
      // there are many solution functions in modal
      feenox_check_alloc(feenox.pde.mode[g] = calloc(feenox.pde.nev, sizeof(function_t *)));
      for (int i = 0; i < feenox.pde.nev; i++) {
        char *modename = NULL;
        feenox_check_minusone(asprintf(&modename, "%s%d", name, i+1));
        feenox_call(feenox_problem_define_solution_function(modename, &feenox.pde.mode[g][i], 0));
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

  // aca la definimos para que este disponible en tiempo de parseo  
  if ((*function = feenox_define_function_get_ptr(name, feenox.pde.dim)) == NULL) {
    feenox_push_error_message("result function '%s' defined twice", name);
    return FEENOX_OK;
  }
  (*function)->mesh = feenox.pde.mesh; // esto puede cambiar a rough despues  
  feenox_problem_define_solution_clean_nodal_arguments(*function);
  (*function)->var_argument = feenox.pde.solution[0]->var_argument;
  // TODO: what about FVM?
  (*function)->type = function_type_pointwise_mesh_node;
  (*function)->is_gradient = is_gradient;

  return 0;
}

/*
int fino_function_clean_nodal_data(function_t *function) {
 
  if (function != NULL && function->data_value != NULL) {  
    feenox_free(function->data_value);
    function->data_value = NULL;
  }
  
  return 0;
}
*/
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
    petsc_call(PetscOptionsHasName(PETSC_NULL, PETSC_NULL, "-progress", &feenox.pde.progress_ascii));
  }  

///op+mumps+option `--mumps`
///op+mumps+desc ask PETSc to use the direct linear solver MUMPS
  petsc_call(PetscOptionsHasName(PETSC_NULL, PETSC_NULL, "-mumps", &flag));
  if (flag == PETSC_TRUE) {
    feenox.pde.ksp_type = strdup("mumps");
    feenox.pde.pc_type = strdup("mumps");
  }

///op+linear+option `--linear`
///op+linear+desc force FeenoX to solve the PDE problem as linear
  petsc_call(PetscOptionsHasName(PETSC_NULL, PETSC_NULL, "-linear", &flag));
  if (flag == PETSC_TRUE) {
    feenox.pde.math_type = math_type_linear;
  }

///op+non-linear+option `--non-linear`
///op+non-linear+desc force FeenoX to solve the PDE problem as non-linear
  petsc_call(PetscOptionsHasName(PETSC_NULL, PETSC_NULL, "-non-linear", &flag));
  if (flag == PETSC_TRUE) {
    feenox.pde.math_type = math_type_nonlinear;
  }
  petsc_call(PetscOptionsHasName(PETSC_NULL, PETSC_NULL, "-nonlinear", &flag));
  if (flag == PETSC_TRUE) {
    feenox.pde.math_type = math_type_nonlinear;
  }
  
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
  unsigned int m = 0;
  unsigned int g = 0;
  for (g = 0; g < feenox.pde.dofs; g++) {
    for (m = 0; m < feenox.pde.dim; m++) {
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


  physical_group_t *physical_group = NULL;
  // this is a loop over a hash, not over a linked list
  for (physical_group = feenox.pde.mesh->physical_groups; physical_group != NULL; physical_group = physical_group->hh.next) {
    if (physical_group->bcs != NULL && physical_group->n_elements == 0) {
      feenox_push_error_message("physical entity '%s' has a BC but no associated elements", physical_group->name);
      return FEENOX_ERROR;
    }

    if (physical_group->material != NULL && physical_group->n_elements == 0) {
      feenox_push_error_message("physical group '%s' has a material but no associated elements", physical_group->name);
      return FEENOX_ERROR;
    }
  }

  bc_t *bc = NULL;
  // this is a loop over a hash, not over a linked list
  for (bc = feenox.mesh.bcs; bc != NULL; bc = bc->hh.next) {
    if (bc->has_explicit_groups == 0) {
      HASH_FIND_STR(feenox.pde.mesh->physical_groups, bc->name, physical_group);
    
      if (physical_group == NULL) {
        feenox_push_error_message("boundary condition '%s' does not have a physical group in mesh file '%s'", bc->name, feenox.pde.mesh->file->path);
        return FEENOX_ERROR;
      }
    
      if (physical_group->n_elements == 0) {
        feenox_push_error_message("physical group for boundary condition '%s' does not have any elements", bc->name);
        return FEENOX_ERROR;
      }
    }
  }

  // allocate global petsc objects
  if (feenox.pde.size_global == 0) {
    feenox_push_error_message("internal error, problem init did not set global size");
    return FEENOX_ERROR;
  }
  
  
  feenox_var_value(feenox.pde.vars.total_dofs) = (double)(feenox.pde.size_global);
  
  // TODO: choose width from input
  feenox.pde.width = GSL_MAX(feenox.pde.mesh->max_nodes_per_element, feenox.pde.mesh->max_first_neighbor_nodes) * feenox.pde.dofs;
  
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
  feenox.pde.first_element = (feenox.pde.mesh->n_elements / feenox.n_procs) * feenox.rank;
  if (feenox.pde.mesh->n_elements % feenox.n_procs > feenox.rank) {
    feenox.pde.first_element += feenox.rank;
    feenox.pde.last_element = feenox.pde.first_element + (feenox.pde.mesh->n_elements / feenox.n_procs) + 1;
  } else {  
    feenox.pde.first_element += feenox.pde.mesh->n_elements % feenox.n_procs;
    feenox.pde.last_element = feenox.pde.first_element + (feenox.pde.mesh->n_elements / feenox.n_procs);
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
    
      // in some cases (e.g. neutron) we do not know if we have to solve
      // a KSP or an EPS until we read the material data, which is far too late
      // in that case, we usually do not care about all the modes so feenox.pde.mode is null
      if (feenox.pde.nev > 0 && feenox.pde.mode != NULL) {
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
#endif
  
  return FEENOX_OK;
}

#ifdef HAVE_PETSC
Mat feenox_problem_create_matrix(const char *name) {
  
  Mat A = NULL;
  petsc_call_null(MatCreate(PETSC_COMM_WORLD, &A));
  petsc_call_null(MatSetSizes(A, feenox.pde.size_local, feenox.pde.size_local, feenox.pde.size_global, feenox.pde.size_global));
  petsc_call_null(MatSetFromOptions(A));
  // TODO:  MatXAIJSetPreallocation
  petsc_call_null(MatMPIAIJSetPreallocation(A, feenox.pde.width, PETSC_NULL, feenox.pde.width, PETSC_NULL));
  petsc_call_null(MatSeqAIJSetPreallocation(A, feenox.pde.width, PETSC_NULL));

  // this flag needs the matrix type to be set, and we had just set it with setfromoptions   
  petsc_call_null(MatSetOption(A, MAT_KEEP_NONZERO_PATTERN, PETSC_TRUE));

  if (name != NULL) {
    petsc_call_null(PetscObjectSetName((PetscObject)(A), name));
  }
  
  if (feenox.pde.allow_new_nonzeros) {
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
