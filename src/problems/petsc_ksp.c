/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's linear solver using PETSc routines
 *
 *  Copyright (C) 2015--2020 jeremy theler
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
 *  along with feenox.  If not, see <http://www.gnu.org/licenses/>.
 *------------------- ------------  ----    --------  --     -       -         -
 */
#include "feenox.h"
extern feenox_t feenox;

int feenox_solve_petsc_linear(void) {

#ifdef HAVE_PETSC
  
  KSPConvergedReason reason;
  
//  time_checkpoint(solve_begin);
  // create a KSP object if needed
  if (feenox.pde.ksp == NULL) {
    petsc_call(KSPCreate(PETSC_COMM_WORLD, &feenox.pde.ksp));
    
    // set the monitor for the ascii progress
    if (feenox.pde.progress_ascii == PETSC_TRUE) {  
      petsc_call(KSPMonitorSet(feenox.pde.ksp, feenox_ksp_monitor, NULL, 0));
    }
    
    // TODO: this call sets up the nearnullspace, that should be in another place
    //       because it is problem-dependent
    feenox_call(feenox_setup_ksp(feenox.pde.ksp));
  }

//  time_checkpoint(build_begin);
  feenox_call(feenox_build());
  feenox_call(feenox_dirichlet_eval());
  feenox_call(feenox_dirichlet_set_K());  
//  time_checkpoint(build_end);
  petsc_call(KSPSetOperators(feenox.pde.ksp, feenox.pde.K_bc, feenox.pde.K_bc));
  
  // try to use the solution as the initial guess (it already has Dirichlet BCs
  // but in quasi-static it has the previous solution which should be similar)
  // mumps cannot be used with a non-zero guess  
  if ((feenox.pde.ksp_type == NULL || strcasecmp(feenox.pde.ksp_type, "mumps") != 0) &&
      (feenox.pde.pc_type  == NULL || strcasecmp(feenox.pde.pc_type,  "mumps") != 0)) {
    petsc_call(KSPSetInitialGuessNonzero(feenox.pde.ksp, PETSC_TRUE));
  } 
  feenox.pde.progress_last = 0;
  
  
  // do the work!
  petsc_call(KSPSolve(feenox.pde.ksp, feenox.pde.b_bc, feenox.pde.phi));
  
  // check for convergence
  petsc_call(KSPGetConvergedReason(feenox.pde.ksp, &reason));
  if (reason < 0) {
    feenox_push_error_message("PETSc's linear solver did not converge with reason '%s' (%d)", KSPConvergedReasons[reason], reason);
    return FEENOX_ERROR;
  }

  // finish the progress line
  if (feenox.pde.progress_ascii == PETSC_TRUE) {
    if (feenox.nprocs == 1) {
      int i = 0;
      for (i = (int)(100*feenox.pde.progress_last); i < 100; i++) {
        printf(CHAR_PROGRESS_SOLVE);
      }
    }
    if (feenox.rank == 0) {
      printf("\n");
      fflush(stdout);
    }  
  }

//  time_checkpoint(solve_end);

#endif
  return FEENOX_OK;

}

#ifdef HAVE_PETSC

PetscErrorCode feenox_ksp_monitor(KSP ksp, PetscInt n, PetscReal rnorm, void *dummy) {
//  feenox_value(feenox.pde.vars.iterations) = (double)n;
//  feenox_var_value(feenox.pde.vars.residual_norm) = rnorm;
  int i;
  double current_progress;
  
  if (feenox.rank == 0) {
  
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



int feenox_setup_ksp(KSP ksp) {

// the KSP type
#ifdef PETSC_HAVE_MUMPS
  if ((feenox.pde.ksp_type != NULL && strcasecmp(feenox.pde.ksp_type, "mumps") == 0) ||
      (feenox.pde.pc_type  != NULL && strcasecmp(feenox.pde.pc_type,  "mumps") == 0)) {
    // mumps is a particular case, see feenox_set_pc
    KSPSetType(ksp, KSPPREONLY);
  } else if (feenox.pde.ksp_type != NULL) {
#else
  if (feenox.pde.ksp_type != NULL) {
#endif
    petsc_call(KSPSetType(ksp, feenox.pde.ksp_type));
  } else {
    // by default use whatever PETSc/SLEPc like
    petsc_call(KSPSetType(ksp, KSPGMRES));
  }

  
  if (feenox.pde.symmetric_K == PETSC_TRUE) {
    // K is symmetric. Set symmetric flag to enable ICC/Cholesky preconditioner
    // TODO: this is K for ksp but J for snes
    if (feenox.pde.has_stiffness) {
      petsc_call(MatSetOption(feenox.pde.K, MAT_SYMMETRIC, PETSC_TRUE));  
    }  
    if (feenox.pde.has_jacobian_K) {
      petsc_call(MatSetOption(feenox.pde.JK, MAT_SYMMETRIC, PETSC_TRUE));  
    }
    if (feenox.pde.has_jacobian_b) {
      petsc_call(MatSetOption(feenox.pde.Jb, MAT_SYMMETRIC, PETSC_TRUE));  
    }  
  }  
  
  petsc_call(KSPSetTolerances(ksp, feenox_var_value(feenox.pde.vars.ksp_rtol),
                                   feenox_var_value(feenox.pde.vars.ksp_atol),
                                   feenox_var_value(feenox.pde.vars.ksp_divtol),
                                   (PetscInt)feenox_var_value(feenox.pde.vars.ksp_max_it)));
  
//  petsc_call(KSPSetUp(ksp));
//  SNESSetUp(feenox.pde.snes);
  
  PC pc;
  petsc_call(KSPGetPC(ksp, &pc));
  feenox_call(feenox_setup_pc(pc));
//  petsc_call(PCSetUp(pc));
  
  // read command-line options
  petsc_call(KSPSetFromOptions(ksp));

  return FEENOX_OK;
}

int feenox_setup_pc(PC pc) {

//  PetscInt i, j;
  PCType pc_type;

//  PetscInt nearnulldim = 0;
//  MatNullSpace nullsp = NULL;
//  PetscScalar  dots[5];
//  Vec          *nullvec;  
//  PetscReal    *coords;
//  Vec          vec_coords;

  // if we were asked for mumps, then either LU o cholesky needs to be used
  // and MatSolverType to mumps
#ifdef PETSC_HAVE_MUMPS  
  if ((feenox.pde.ksp_type != NULL && strcasecmp(feenox.pde.ksp_type, "mumps") == 0) ||
      (feenox.pde.pc_type  != NULL && strcasecmp(feenox.pde.pc_type,  "mumps") == 0)) {
#if PETSC_VERSION_GT(3,9,0)
    petsc_call(MatSetOption(feenox.pde.K, MAT_SPD, PETSC_TRUE)); /* set MUMPS id%SYM=1 */
    petsc_call(PCSetType(pc, PCCHOLESKY));

    petsc_call(PCFactorSetMatSolverType(pc, MATSOLVERMUMPS));

// if we want to set further options for mumps we need to retrieve the matrix
// and then call MatMumpsSetIcntl()
//  petsc_call(PCFactorSetUpMatSolverType(pc)); /* call MatGetFactor() to create F */
//  petsc_call(PCFactorGetMatrix(pc, &F));    
    
#else
    feenox_push_error_message("MUMPS solver needs at least PETSc 3.9");
    return FEENOX_ERROR;
#endif
  } else {
#endif
    if (feenox.pde.pc_type != NULL) {
      petsc_call(PCSetType(pc, feenox.pde.pc_type));
    } else {
      petsc_call(PCSetType(pc, PCGAMG));
    }
#ifdef PETSC_HAVE_MUMPS  
  }  
#endif
  
  
  petsc_call(PCGetType(pc, &pc_type));
  if (pc_type != NULL && strcmp(pc_type, PCGAMG) == 0) {
#if PETSC_VERSION_LT(3,8,0)
    PCGAMGSetThreshold(pc, (PetscReal)feenox_var_value(feenox.pde.vars.gamg_threshold));
#else
    PCGAMGSetThreshold(pc, (PetscReal *)(feenox_value_ptr(feenox.pde.vars.gamg_threshold)), 1);
#endif
    petsc_call(PCGAMGSetNSmooths(pc, 1));
  }

/*  
  if (feenox.pde.problem_family == problem_family_mechanical) {
    // las coordenadas (solo para break)
  // http://computation.llnl.gov/casc/linear_solvers/pubs/Baker-2009-elasticity.pdf
    // ojo que si estamos en node ordering no podemos usar set_near_nullspace_rigidbody
    if (feenox.pde.mesh->ordering == ordering_dof_major && feenox.pde.set_near_nullspace == set_near_nullspace_rigidbody) {
      feenox.pde.set_near_nullspace = set_near_nullspace_feenox;
    }

    switch(feenox.pde.set_near_nullspace) {

      case set_near_nullspace_rigidbody:
        petsc_call(MatCreateVecs(feenox.pde.K, NULL, &vec_coords));
        petsc_call(VecSetBlockSize(vec_coords, feenox.pde.degrees));
        petsc_call(VecSetUp(vec_coords));
        petsc_call(VecGetArray(vec_coords, &coords));

        for (j = feenox.pde.first_node; j < feenox.pde.last_node; j++) {          
          for (d = 0; d < feenox.pde.dimensions; d++) {
            coords[feenox.pde.mesh->node[j].index_dof[d]-feenox.pde.first_row] = feenox.pde.mesh->node[j].x[d];
          }
        }

        petsc_call(VecRestoreArray(vec_coords, &coords));
        petsc_call(MatNullSpaceCreateRigidBody(vec_coords, &nullsp));
        petsc_call(MatSetNearNullSpace(feenox.pde.K, nullsp));
        petsc_call(MatNullSpaceDestroy(&nullsp));
        petsc_call(VecDestroy(&vec_coords));
      break;

      case set_near_nullspace_feenox:
        nearnulldim = 6; 
        petsc_call(PetscMalloc1(nearnulldim, &nullvec));
        for (i = 0; i < nearnulldim; i++) {
          petsc_call(MatCreateVecs(feenox.pde.K, &nullvec[i], NULL));
        }
        for (j = 0; j < feenox.pde.mesh->n_nodes; j++) {
          // traslaciones
          VecSetValue(nullvec[0], feenox.pde.mesh->node[j].index_dof[0], 1.0, INSERT_VALUES);
          VecSetValue(nullvec[1], feenox.pde.mesh->node[j].index_dof[1], 1.0, INSERT_VALUES);
          VecSetValue(nullvec[2], feenox.pde.mesh->node[j].index_dof[2], 1.0, INSERT_VALUES);

          // rotaciones
          VecSetValue(nullvec[3], feenox.pde.mesh->node[j].index_dof[0], +feenox.pde.mesh->node[j].x[1], INSERT_VALUES);
          VecSetValue(nullvec[3], feenox.pde.mesh->node[j].index_dof[1], -feenox.pde.mesh->node[j].x[0], INSERT_VALUES);

          VecSetValue(nullvec[4], feenox.pde.mesh->node[j].index_dof[1], -feenox.pde.mesh->node[j].x[2], INSERT_VALUES);
          VecSetValue(nullvec[4], feenox.pde.mesh->node[j].index_dof[2], +feenox.pde.mesh->node[j].x[1], INSERT_VALUES);

          VecSetValue(nullvec[5], feenox.pde.mesh->node[j].index_dof[0], +feenox.pde.mesh->node[j].x[2], INSERT_VALUES);
          VecSetValue(nullvec[5], feenox.pde.mesh->node[j].index_dof[2], -feenox.pde.mesh->node[j].x[0], INSERT_VALUES);
        }

        for (i = 0; i < 3; i++) {
          VecNormalize(nullvec[i], PETSC_NULL);
        }

        // from MatNullSpaceCreateRigidBody()
        for (i = 3; i < nearnulldim; i++) {
          // Orthonormalize vec[i] against vec[0:i-1]
          VecMDot(nullvec[i], i, nullvec, dots);
          for (j= 0; j < i; j++) {
            dots[j] *= -1.;
          }
          VecMAXPY(nullvec[i],i,dots,nullvec);
          VecNormalize(nullvec[i], PETSC_NULL);
        }

        petsc_call(MatNullSpaceCreate(PETSC_COMM_WORLD, PETSC_FALSE, nearnulldim, nullvec, &nullsp));
        petsc_call(MatSetNearNullSpace(feenox.pde.K, nullsp));
      break;  

      case set_near_nullspace_none:
        ;
      break;
    }
  }
*/  
  return FEENOX_OK;
}

#endif