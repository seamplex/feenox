/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's linear solver using PETSc routines
 *
 *  Copyright (C) 2015--2023 jeremy theler
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
 *  along with FeenoX.  If not, see <http://www.gnu.org/licenses/>.
 *------------------- ------------  ----    --------  --     -       -         -
 */
#include "feenox.h"

int feenox_problem_solve_petsc_linear(void) {

#ifdef HAVE_PETSC

  // build ---------------------------------------------------------------------
  petsc_call(PetscLogStagePush(feenox.pde.stage_build));  

  feenox_call(feenox_problem_build());
  feenox_call(feenox_problem_dirichlet_eval());
  feenox_call(feenox_problem_dirichlet_set_K());
  
  petsc_call(PetscLogStagePop());
  // ---------------------------------------------------------------------------
  
  if (feenox.pde.missed_dump != NULL) {
    feenox_call(feenox_instruction_dump(feenox.pde.missed_dump));
  }

  // solve ---------------------------------------------------------------------
  petsc_call(PetscLogStagePush(feenox.pde.stage_solve));  
  
  // create a KSP object if needed
  if (feenox.pde.ksp == NULL) {
    petsc_call(KSPCreate(PETSC_COMM_WORLD, &feenox.pde.ksp));
    
    // set the monitor for the ascii progress
    if (feenox.pde.progress_ascii == PETSC_TRUE) {  
      petsc_call(KSPMonitorSet(feenox.pde.ksp, feenox_problem_ksp_monitor, NULL, 0));
    }

    petsc_call(KSPSetOperators(feenox.pde.ksp, feenox.pde.K_bc, feenox.pde.K_bc));
    
    feenox_call(feenox_problem_setup_ksp(feenox.pde.ksp));
  }

  
  // check if the stiffness matrix K has a near nullspace 
  // and pass it on to K_bc
  MatNullSpace near_null_space = NULL;
  petsc_call(MatGetNearNullSpace(feenox.pde.K, &near_null_space));
  if (near_null_space != NULL) {
    petsc_call(MatSetNearNullSpace(feenox.pde.K_bc, near_null_space));
  }
  
  
  // try to use the solution as the initial guess (it already has Dirichlet BCs
  // but in quasi-static it has the previous solution which should be similar)
  // mumps cannot be used with a non-zero guess  
/*  
  if ((feenox.pde.ksp_type == NULL || strcasecmp(feenox.pde.ksp_type, "mumps") != 0) &&
      (feenox.pde.pc_type  == NULL || strcasecmp(feenox.pde.pc_type,  "mumps") != 0)) {
    petsc_call(KSPSetInitialGuessNonzero(feenox.pde.ksp, PETSC_TRUE));
  } 
*/
  feenox.pde.progress_last = 0;

  // do the work!
  // TODO: have a flag in case we want to just build
  petsc_call(KSPSolve(feenox.pde.ksp, feenox.pde.b_bc, feenox.pde.phi));
  
  // check for convergence
  KSPConvergedReason reason = 0;
  petsc_call(KSPGetConvergedReason(feenox.pde.ksp, &reason));
  if (reason < 0) {
    // if the input file asks for a DUMP then we do it now
    if (feenox.pde.dumps != NULL) {
      feenox_instruction_dump((void *)feenox.pde.dumps);
    }
    
    feenox_push_error_message("PETSc's linear solver did not converge with reason '%s' (%d)", KSPConvergedReasons[reason], reason);
    // TODO: dive into the PC
    return FEENOX_ERROR;
  }

  // finish the progress line
  if (feenox.pde.progress_ascii == PETSC_TRUE) {
    if (feenox.mpi_size == 1) {
      int i = 0;
      for (i = (int)(100*feenox.pde.progress_last); i < 100; i++) {
        printf(CHAR_PROGRESS_SOLVE);
      }
    }
    if (feenox.mpi_rank == 0) {
      printf("\n");
      fflush(stdout);
    }  
  }
  petsc_call(PetscLogStagePop());
  // ---------------------------------------------------------------------------

#endif
  return FEENOX_OK;

}

#ifdef HAVE_PETSC

PetscErrorCode feenox_problem_ksp_monitor(KSP ksp, PetscInt n, PetscReal rnorm, void *dummy) {
//  feenox_value(feenox.pde.vars.iterations) = (double)n;
//  feenox_var_value(feenox.pde.vars.residual_norm) = rnorm;
  int i;
  double current_progress;
  
  if (feenox.mpi_rank == 0) {
  
    if (feenox.pde.progress_r0 == 0) {
      feenox.pde.progress_r0 = rnorm;
    }
  
    if (rnorm < 1e-20) {
      current_progress = 1;
    } else {
      current_progress = log((rnorm/feenox.pde.progress_r0))/log(feenox_var_value(feenox.pde.vars.ksp_rtol));
      if (current_progress > 1) {
        current_progress = 1;
      }
    } 

//    printf("%d %e %.0f\n", n, rnorm/r0, 100*current_progress);
    
    if (feenox.pde.progress_ascii == PETSC_TRUE) {
      if (feenox.pde.progress_last < current_progress) {
        for (i = (int)(100*feenox.pde.progress_last); i < (int)(100*current_progress); i++) {
          printf(CHAR_PROGRESS_SOLVE);
          fflush(stdout);
        }
        feenox.pde.progress_last = current_progress;
      }  
    }
  }  

  return 0;
}



int feenox_problem_setup_ksp(KSP ksp) {

// the KSP type
#ifdef PETSC_HAVE_MUMPS
  if ((feenox.pde.ksp_type != NULL && strcasecmp(feenox.pde.ksp_type, "mumps") == 0) ||
      (feenox.pde.pc_type  != NULL && strcasecmp(feenox.pde.pc_type,  "mumps") == 0)) {
    // mumps is a particular case, see feenox_problem_setup_pc
    petsc_call(KSPSetType(ksp, KSPPREONLY));
  } else if (feenox.pde.ksp_type != NULL) {
#else
  if (feenox.pde.ksp_type != NULL) {
#endif
    petsc_call(KSPSetType(ksp, feenox.pde.ksp_type));
  }
  
  if (feenox.pde.setup_ksp != NULL) {
    feenox_call(feenox.pde.setup_ksp(ksp));
  }

  if (feenox.pde.symmetric_K) {
    // K is symmetric. Set symmetric flag to enable ICC/Cholesky preconditioner
    // TODO: this is K for ksp but J for snes
    if (feenox.pde.has_stiffness) {
      if (feenox.pde.K != NULL) {
        petsc_call(MatSetOption(feenox.pde.K, MAT_SYMMETRIC, PETSC_TRUE));  
        petsc_call(MatSetOption(feenox.pde.K, MAT_SPD, PETSC_TRUE));
        
      }
      if (feenox.pde.K_bc != NULL) {
        petsc_call(MatSetOption(feenox.pde.K_bc, MAT_SYMMETRIC, PETSC_TRUE));  
        petsc_call(MatSetOption(feenox.pde.K_bc, MAT_SPD, PETSC_TRUE));  
      }  
    }  
    if (feenox.pde.has_mass) {
      if (feenox.pde.M != NULL) {
        petsc_call(MatSetOption(feenox.pde.M, MAT_SYMMETRIC, PETSC_TRUE));  
        petsc_call(MatSetOption(feenox.pde.M, MAT_SPD, PETSC_TRUE));  
      }
      if (feenox.pde.M_bc != NULL) {
        petsc_call(MatSetOption(feenox.pde.M_bc, MAT_SYMMETRIC, PETSC_TRUE));  
      }  
    }  
    if (feenox.pde.has_jacobian_K) {
      if (feenox.pde.JK != NULL) {
        petsc_call(MatSetOption(feenox.pde.JK, MAT_SYMMETRIC, PETSC_TRUE));  
        petsc_call(MatSetOption(feenox.pde.JK, MAT_SPD, PETSC_TRUE));  
      }  
    }
    if (feenox.pde.has_jacobian_b) {
      if (feenox.pde.Jb != NULL) {
        petsc_call(MatSetOption(feenox.pde.Jb, MAT_SYMMETRIC, PETSC_TRUE));  
        petsc_call(MatSetOption(feenox.pde.Jb, MAT_SPD, PETSC_TRUE));  
      }  
    }  
  }  
  
  petsc_call(KSPSetTolerances(ksp, feenox_var_value(feenox.pde.vars.ksp_rtol),
                                   feenox_var_value(feenox.pde.vars.ksp_atol),
                                   feenox_var_value(feenox.pde.vars.ksp_divtol),
                                   (PetscInt)feenox_var_value(feenox.pde.vars.ksp_max_it)));
  
  PC pc = NULL;
  petsc_call(KSPGetPC(ksp, &pc));
  if (pc == NULL) {
    feenox_push_error_message("cannot get preconditioner object");
    return FEENOX_ERROR;
  }
  feenox_call(feenox_problem_setup_pc(pc));

  // read command-line options
  petsc_call(KSPSetFromOptions(ksp));
  
  return FEENOX_OK;
}

int feenox_problem_setup_pc(PC pc) {

  // if we were asked for mumps, then either LU o cholesky needs to be used
  // and MatSolverType to mumps
#ifdef PETSC_HAVE_MUMPS  
  if ((feenox.pde.ksp_type != NULL && strcasecmp(feenox.pde.ksp_type, "mumps") == 0) ||
      (feenox.pde.pc_type  != NULL && strcasecmp(feenox.pde.pc_type,  "mumps") == 0)) {
#if PETSC_VERSION_GT(3,9,0)
    // need to set this guy otherwise PCFactorSetMatSolverType does not work
    petsc_call(PCSetType(pc, feenox.pde.symmetric_K ? PCCHOLESKY : PCLU));
    petsc_call(PCFactorSetMatSolverType(pc, MATSOLVERMUMPS));

#else
    feenox_push_error_message("MUMPS solver needs at least PETSc 3.9");
    return FEENOX_ERROR;
#endif
  } else {
#endif
    if (feenox.pde.pc_type != NULL) {
      petsc_call(PCSetType(pc, feenox.pde.pc_type));
    }
#ifdef PETSC_HAVE_MUMPS  
  }  
#endif
  
  if (feenox.pde.setup_pc != NULL) {
    feenox_call(feenox.pde.setup_pc(pc));
  }
  
  // if using MUMPS, set icntl if needed
#ifdef PETSC_HAVE_MUMPS  
#ifdef HAVE_SLEPC
  // in EPS we need to set the ST type to create it, otherwise the icntls do not work
  if (feenox.pde.eps != NULL) {
    ST st = NULL;
    petsc_call(EPSGetST(feenox.pde.eps, &st));
    STType st_type = NULL;
    petsc_call(STGetType(st, &st_type));
    if (st_type == NULL) {
      feenox_push_error_message("internal error, ST type is not set");
      return FEENOX_ERROR;
    }
    // force creation of the operator
    petsc_call(STGetOperator(st, NULL));
  }
#endif
  
  MatSolverType mat_solver_type = NULL;
  petsc_call(PCFactorGetMatSolverType(pc, &mat_solver_type));
  if (mat_solver_type != NULL && strcmp(mat_solver_type, MATSOLVERMUMPS) == 0) {
    if (feenox_var_value(feenox.pde.vars.mumps_icntl_14) != 0) {
#if PETSC_VERSION_LT(3,19,0)
      petsc_call(PCSetUp(pc));
#endif
      Mat F;
      petsc_call(PCFactorGetMatrix(pc, &F));
      petsc_call(MatMumpsSetIcntl(F, 14, (PetscInt)feenox_var_value(feenox.pde.vars.mumps_icntl_14)));
    }
  }
#endif
  
  PCType pc_type;
  petsc_call(PCGetType(pc, &pc_type));
  if (pc_type != NULL && strcmp(pc_type, PCGAMG) == 0) {
    petsc_call(PCGAMGSetThreshold(pc, &feenox_var_value(feenox.pde.vars.gamg_threshold), 1));
  }
  
  // read command-line options
  petsc_call(PCSetFromOptions(pc));

  return FEENOX_OK;
}

#endif
