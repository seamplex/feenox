/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's non-linear solver using PETSc routines
 *
 *  Copyright (C) 2020--2021 Jeremy Theler
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

int feenox_problem_solve_petsc_nonlinear(void) {

#ifdef HAVE_PETSC

  if (feenox.pde.snes == NULL) {
    // TODO: move to a separate function
    petsc_call(SNESCreate(PETSC_COMM_WORLD, &feenox.pde.snes));

    // monitor
//    petsc_call(SNESMonitorSet(feenox.pde.snes, feenox.pde.snes_monitor, NULL, 0));

    feenox_check_alloc(feenox.pde.r = feenox_problem_create_vector("r"));
    petsc_call(SNESSetFunction(feenox.pde.snes, feenox.pde.r, feenox_snes_residual, NULL));
    feenox_check_alloc(feenox.pde.J_snes = feenox_problem_create_matrix("J_snes"));
    petsc_call(SNESSetJacobian(feenox.pde.snes, feenox.pde.J_snes, feenox.pde.J_snes, feenox_snes_jacobian, NULL));    

    // TODO
    feenox_call(feenox_problem_setup_snes(feenox.pde.snes));
  }

  // solve
  feenox.pde.progress_last = 0; // reset the progress bar counter
  if (feenox.pde.do_not_solve == 0) {
    petsc_call(SNESSolve(feenox.pde.snes, NULL, feenox.pde.phi));

    // check convergence
    SNESConvergedReason reason;
    petsc_call(SNESGetConvergedReason(feenox.pde.snes, &reason));
    if (reason < 0) {
      feenox_push_error_message("PETSc's non-linear solver did not converge with reason '%s' (%d)", SNESConvergedReasons[reason], reason);
      // TODO: go deeper into the KSP and PC
      return FEENOX_ERROR;
    }
  }

  // finish the progress line
  if (feenox.pde.progress_ascii == PETSC_TRUE) {
    int i;
    if (feenox.mpi_size == 1) {
      for (i = (int)(100*feenox.pde.progress_last); i < 100; i++) {
        printf(CHAR_PROGRESS_SOLVE);
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

#ifdef HAVE_PETSC
int feenox_problem_setup_snes(SNES snes) {
  // TODO: have an explicit default
  // TODO: set the line search
  if (feenox.pde.snes_type != NULL) {
    // if we have an explicit type, we set it
    petsc_call(SNESSetType(snes, feenox.pde.snes_type));
  }

  if (feenox.pde.ls_type != NULL) {
    SNESLineSearch  ls;
    petsc_call(SNESGetLineSearch(snes, &ls));
    petsc_call(SNESLineSearchSetType(ls, feenox.pde.ls_type));
  }

  if (feenox.pde.setup_snes != NULL) {
    feenox_call(feenox.pde.setup_snes(snes));
  }
  petsc_call(SNESSetTolerances(snes, feenox_var_value(feenox.pde.vars.snes_atol),
                                     feenox_var_value(feenox.pde.vars.snes_rtol),
                                     feenox_var_value(feenox.pde.vars.snes_stol),
                                     (PetscInt)feenox_var_value(feenox.pde.vars.snes_max_it),
                                     PETSC_DEFAULT));
#if PETSC_VERSION_LT(3,22,0)
  petsc_call(SNESSetDivergenceTolerance(snes, -1));
#else
  petsc_call(SNESSetDivergenceTolerance(snes, PETSC_UNLIMITED));
#endif

  petsc_call(SNESSetFromOptions(snes));
  // TODO: this call complains about DM (?) in thermal-slab-transient.fee 
  // petsc_call(SNESSetUp(snes));

  // customize ksp (and pc)---this needs to come after setting the jacobian
  KSP ksp;
  petsc_call(SNESGetKSP(snes, &ksp));
  feenox_call(feenox_problem_setup_ksp(ksp));

  return FEENOX_OK;
}

PetscErrorCode feenox_snes_residual(SNES snes, Vec phi, Vec r, void *ctx) {
  // set dirichlet BCs on the solution
  if (feenox.pde.phi_bc == NULL) {
    feenox_check_alloc(feenox.pde.phi_bc = feenox_problem_create_vector("phi_bc"));
  }
  feenox_call(feenox_problem_dirichlet_eval());  
  petsc_call(VecCopy(phi, feenox.pde.phi_bc));
  feenox_call(feenox_problem_dirichlet_set_phi(feenox.pde.phi_bc));

  // build both the residual and the jacobian
  feenox_call(feenox_problem_phi_to_solution(feenox.pde.phi_bc));
  feenox_call(feenox_problem_build());

  if (feenox.pde.has_internal_fluxes) {
    // if the problem provides an internal flux, use it
    petsc_call(VecCopy(feenox.pde.f, r));
  } else {
    // otherwise make it up from the stiffness and the solution
    petsc_call(MatMult(feenox.pde.K, feenox.pde.phi_bc, r));
  }

//  printf("r as in f\n");
//  petsc_call(VecView(r, PETSC_VIEWER_STDOUT_SELF));

  // subtract b
  petsc_call(VecAXPY(r, -1.0, feenox.pde.b));
//  printf("r as in f-b\n");
//  VecView(r, PETSC_VIEWER_STDOUT_SELF);

  // set dirichlet BCs on the residual
  feenox_call(feenox_problem_dirichlet_set_r(r, phi));

//  printf("r as in f-b + bcs\n");
//  VecView(r, PETSC_VIEWER_STDOUT_SELF);

//  printf("solution\n");
//  VecView(phi, PETSC_VIEWER_STDOUT_SELF);

  return FEENOX_OK;
}


PetscErrorCode feenox_snes_jacobian(SNES snes,Vec phi, Mat J_snes, Mat P, void *ctx) {

  // J_snes = (K + JK - Jb)_bc
  // TODO: we want SAME_NONZERO_PATTERN!
  petsc_call(MatAssemblyBegin(feenox.pde.K, MAT_FINAL_ASSEMBLY));
  petsc_call(MatAssemblyEnd(feenox.pde.K, MAT_FINAL_ASSEMBLY));
  petsc_call(MatAssemblyBegin(J_snes, MAT_FINAL_ASSEMBLY));
  petsc_call(MatAssemblyEnd(J_snes, MAT_FINAL_ASSEMBLY));
  petsc_call(MatCopy(feenox.pde.K, J_snes, DIFFERENT_NONZERO_PATTERN));

//  printf("K\n");
//  MatView(feenox.pde.K, PETSC_VIEWER_STDOUT_SELF);

//  printf("JK\n");
//  MatView(feenox.pde.JK, PETSC_VIEWER_STDOUT_SELF);

  if (feenox.pde.has_jacobian_K) {
    petsc_call(MatAXPY(J_snes, +1.0, feenox.pde.JK, DIFFERENT_NONZERO_PATTERN));
  }

  if (feenox.pde.has_jacobian_b) {
    petsc_call(MatAXPY(J_snes, -1.0, feenox.pde.Jb, DIFFERENT_NONZERO_PATTERN));
  }

  feenox_call(feenox_problem_dirichlet_set_J(J_snes));

//  printf("jacobian\n");
//  MatView(J_snes, PETSC_VIEWER_STDOUT_SELF);

  return FEENOX_OK;
}


PetscErrorCode feenox_snes_monitor(SNES snes, PetscInt n, PetscReal rnorm, void *dummy) {

  if (feenox.pde.progress_r0 == 0) {
    feenox.pde.progress_r0 = rnorm;
  }

  double current_progress = (rnorm > 1e-20) ? log((rnorm/feenox.pde.progress_r0))/log(feenox_var_value(feenox.pde.vars.ksp_rtol)) : 1;
  if (current_progress > 1) {
    current_progress = 1;
  }

  if (feenox.pde.progress_ascii == PETSC_TRUE) {
    size_t i = 0;
    for (i = (size_t)(100*feenox.pde.progress_last); i < (size_t)(100*current_progress); i++) {
      printf(CHAR_PROGRESS_SOLVE);
      fflush(stdout);
    }
    feenox.pde.progress_last = current_progress;
  }

  return 0;
}

#endif
