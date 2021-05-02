/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's non-linear solver using PETSc routines
 *
 *  Copyright (C) 2020 jeremy theler
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
#include "feenox.h"
extern feenox_t feenox;


PetscErrorCode feenox_snes_residual(SNES snes, Vec phi, Vec r,void *ctx) {


  // this check is only to avoid building the first time because we already
  // built K in order to create the SNES, but the rest of the step we need
  // to re-build always because we are sure the problem is non-linear
  if (feenox.pde.already_built == PETSC_FALSE) {
    // pass phi to the solution becuase K (and the BCs) might depend on phi (and its spatial derivatives)
    feenox_call(feenox_phi_to_solution(phi, 1));
    feenox_call(feenox_build());
    feenox_call(feenox_dirichlet_eval());
  }  
  
  petsc_call(MatMult(feenox.pde.K, phi, r));
  petsc_call(VecAXPY(r, -1.0, feenox.pde.b));
  feenox_call(feenox_dirichlet_set_r(r, phi));
  
  feenox.pde.already_built = PETSC_FALSE;
  
  return 0;
}

PetscErrorCode feenox_snes_jacobian(SNES snes,Vec phi, Mat J, Mat P, void *ctx) {
  
  // this should be J = K + K'*phi - b'
  petsc_call(MatCopy(feenox.pde.J, J, SAME_NONZERO_PATTERN));
  feenox_call(feenox_dirichlet_set_J(J));
  petsc_call(MatCopy(J, P, SAME_NONZERO_PATTERN));
  
  return 0;
}

int feenox_solve_petsc_nonlinear(void) {

  KSP ksp;
  PC pc;
  Mat J;
  Vec r;
  SNESConvergedReason reason;
  PetscInt       its;
  
//  time_checkpoint(build_begin);
  
  if (feenox.pde.snes == NULL) {
    petsc_call(SNESCreate(PETSC_COMM_WORLD, &feenox.pde.snes));

    if (feenox.pde.snes_type != NULL) {
      // if we have an explicit type, we set it
      petsc_call(SNESSetType(feenox.pde.snes, feenox.pde.snes_type));
    }
    
    // we build the matrices here and put a flag that it is already built
    // so we do not build it again in the first step of the SNES
    feenox_call(feenox_build());
    feenox_call(feenox_dirichlet_eval());
    feenox.pde.already_built = PETSC_TRUE;
//    time_checkpoint(build_end);
    
    // monitor
//    petsc_call(SNESMonitorSet(feenox.pde.snes, feenox.pde.snes_monitor, NULL, 0));

    // options
    petsc_call(SNESSetFromOptions(feenox.pde.snes));
  }  
    
  petsc_call(VecDuplicate(feenox.pde.phi, &r));
  petsc_call(MatDuplicate(feenox.pde.K, MAT_COPY_VALUES, &J));
  
  petsc_call(SNESSetFunction(feenox.pde.snes, r, feenox_snes_residual, NULL));
  petsc_call(SNESSetJacobian(feenox.pde.snes, J, J, feenox_snes_jacobian, NULL));

  // TODO
/*  
  petsc_call(SNESSetTolerances(feenox.pde.snes, feenox_var_value(feenox.pde.vars.snes_atol),
                                                feenox_var_value(feenox.pde.vars.snes_rtol),
                                                PETSC_DEFAULT,
                                                (PetscInt)feenox_var_value(feenox.pde.vars.snes_max_it),
                                                PETSC_DEFAULT));
 */

  // customize ksp and pc (this needs to come after setting the jacobian)
  petsc_call(SNESGetKSP(feenox.pde.snes, &ksp));
  feenox_call(feenox_set_ksp(ksp));

  petsc_call(KSPGetPC(ksp, &pc));
  feenox_call(feenox_set_pc(pc));
  
  // initial guess
  feenox_call(feenox_dirichlet_set_phi(feenox.pde.phi));
  
  // solve
  feenox.pde.progress_last = 0; // reset the progress bar counter
  petsc_call(SNESSolve(feenox.pde.snes, NULL, feenox.pde.phi));
  
  // check convergence
  petsc_call(SNESGetConvergedReason(feenox.pde.snes, &reason));
  if (reason < 0) {
    feenox_push_error_message("PETSc's non-linear solver did not converge with reason '%s' (%d)", SNESConvergedReasons[reason], reason);
    return FEENOX_ERROR;
  }
  
  
  // finish the progress line
  if (feenox.pde.progress_ascii == PETSC_TRUE) {
    int i;
    if (feenox.nprocs == 1) {
      for (i = (int)(100*feenox.pde.progress_last); i < 100; i++) {
        printf(CHAR_PROGRESS_SOLVE);
      }
    }
    if (feenox.rank == 0) {
      printf("\n");
      fflush(stdout);
    }  
  }
  
  petsc_call(SNESGetIterationNumber(feenox.pde.snes, &its));
  feenox_var_value(feenox.pde.vars.iterations) = (double)its;
  
  return FEENOX_OK;

}

PetscErrorCode feenox_snes_monitor(SNES snes, PetscInt n, PetscReal rnorm, void *dummy) {
  int i;
  double current_progress;
  
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

//  printf("%d %e %e\n", n, rnorm/feenox.pde.progress_r0, current_progress);
  
  if (feenox.pde.progress_ascii == PETSC_TRUE) {
    for (i = (int)(100*feenox.pde.progress_last); i < (int)(100*current_progress); i++) {
      printf(CHAR_PROGRESS_SOLVE);
      fflush(stdout);
    }
    feenox.pde.progress_last = current_progress;
  }

  return 0;
}
