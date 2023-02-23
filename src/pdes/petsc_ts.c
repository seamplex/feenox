/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's transient solver using PETSc routines
 *
 *  Copyright (C) 2021 jeremy theler
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

int feenox_problem_solve_petsc_transient(void) {

#ifdef HAVE_PETSC

  if (feenox_var_value(feenox_special_var(in_static))) {
    // if we are in the static step and no initial condition was given, we solve a steady state
    if (feenox.pde.initial_condition == NULL) {
      if (feenox.pde.math_type == math_type_linear) {
        feenox_call(feenox_problem_solve_petsc_linear());
        petsc_call(KSPDestroy(&feenox.pde.ksp));
        feenox.pde.ksp = NULL;
      
      } else if (feenox.pde.math_type == math_type_nonlinear) {
        feenox_call(feenox_problem_solve_petsc_nonlinear());
        petsc_call(SNESDestroy(&feenox.pde.snes));
        feenox.pde.snes = NULL;
      }
    } else {
      feenox_function_to_phi(feenox.pde.initial_condition, feenox.pde.phi);
    }
    
    if (feenox.pde.ts == NULL) {
      petsc_call(TSCreate(PETSC_COMM_WORLD, &feenox.pde.ts));
      petsc_call(TSSetIFunction(feenox.pde.ts, NULL, feenox_ts_residual, NULL));

      // if we have an initial condition then matrices do not exist yet
      if (feenox.pde.initial_condition != NULL) {
        feenox_call(feenox_problem_build());
      }  
      petsc_call(MatDuplicate(feenox.pde.has_jacobian ? feenox.pde.JK : feenox.pde.K, MAT_COPY_VALUES, &feenox.pde.J_tran));
      petsc_call(TSSetIJacobian(feenox.pde.ts, feenox.pde.J_tran, feenox.pde.J_tran, feenox_ts_jacobian, NULL));

      petsc_call(TSSetProblemType(feenox.pde.ts, (feenox.pde.math_type == math_type_linear) ? TS_LINEAR : TS_NONLINEAR));

      // if BCs depend on time we need DAEs
      petsc_call(TSSetEquationType(feenox.pde.ts, TS_EQ_IMPLICIT));      
  //      petsc_call(TSSetEquationType(feenox.pde.ts, TS_EQ_DAE_IMPLICIT_INDEX1));
  //      petsc_call(TSARKIMEXSetFullyImplicit(feenox.pde.ts, PETSC_TRUE)); 

      feenox_call(feenox_problem_setup_ts(feenox.pde.ts));

      petsc_call(TSSetTimeStep(feenox.pde.ts, feenox_special_var_value(dt)));
      petsc_call(TSSetMaxTime(feenox.pde.ts, 0.0));
      petsc_call(TSSolve(feenox.pde.ts, feenox.pde.phi));

      petsc_call(TSSetMaxTime(feenox.pde.ts, feenox_var_value(feenox_special_var(end_time))));
      petsc_call(TSSetExactFinalTime(feenox.pde.ts, TS_EXACTFINALTIME_MATCHSTEP));
    }  
    
    return FEENOX_OK;
    
  }
  

  PetscInt ts_step = 0;
  petsc_call(TSGetStepNumber(feenox.pde.ts, &ts_step));
  petsc_call(TSSetMaxSteps(feenox.pde.ts, ts_step+1));

  petsc_call(TSSetTimeStep(feenox.pde.ts, feenox_special_var_value(dt)));
  petsc_call(TSSolve(feenox.pde.ts, feenox.pde.phi));  
//  petsc_call(TSStep(feenox.pde.ts));
  petsc_call(TSGetTime(feenox.pde.ts, feenox_value_ptr(feenox_special_var(t))));  
  petsc_call(TSGetTimeStep(feenox.pde.ts, feenox_value_ptr(feenox_special_var(dt))));  
  
#endif
  
  return FEENOX_OK;
  
}


#ifdef HAVE_PETSC

int feenox_problem_setup_ts(TS ts) {
  
  // TODO: the default depends on the physics type
  petsc_call(TSSetType(ts, (feenox.pde.ts_type != NULL) ? feenox.pde.ts_type : TSBDF));

  TSAdapt adapt;
  petsc_call(TSGetAdapt(ts, &adapt));
  petsc_call(TSAdaptSetType(adapt, (feenox.pde.ts_adapt_type != NULL) ? feenox.pde.ts_adapt_type : TSADAPTBASIC));
  // the factors are so we can pass min_dt = max_dt
  petsc_call(TSAdaptSetStepLimits(adapt, (feenox_special_var_value(min_dt) > 0) ? (1-1e-8)*feenox_special_var_value(min_dt) : PETSC_DEFAULT,
                                         (feenox_special_var_value(max_dt) > 0) ? (1+1e-8)*feenox_special_var_value(max_dt) : PETSC_DEFAULT));
  
  // TODO: choose
//  petsc_call(TSSetMaxStepRejections(feenox.pde.ts, 10000));
//  petsc_call(TSSetMaxSNESFailures(feenox.pde.ts, 1000));

  // options overwrite
  petsc_call(TSSetFromOptions(ts));    
  // TODO: this guy complains about DM (?)
//  petsc_call(TSSetUp(ts));    
  
  SNES snes;
  petsc_call(TSGetSNES(ts, &snes));
  if (snes != NULL) {
    feenox_call(feenox_problem_setup_snes(snes));
  } else {
    KSP ksp;
    petsc_call(TSGetKSP(ts, &ksp));
    if (ksp != NULL) {
      feenox_call(feenox_problem_setup_ksp(ksp));
    }
  }  



  return FEENOX_OK;
}

PetscErrorCode feenox_ts_residual(TS ts, PetscReal t, Vec phi, Vec phi_dot, Vec r, void *ctx) {
  
//  static int count = 0;
  feenox_var_value(feenox_special_var(t)) = t;
  
  if (feenox.pde.math_type == math_type_nonlinear) {
    feenox_call(feenox_problem_phi_to_solution(phi));
  }
  
  // TODO: for time-dependent neumann BCs it should not be needed to re-build the whole matrix, just the RHS
  feenox_call(feenox_problem_build());

  // TODO: be smart about this too
  feenox_call(feenox_problem_dirichlet_eval());  

  // compute the residual R(t,phi,phi_dot) = M*(phi_dot)_dirichlet + K*(phi)_dirichlet - b
  
  // TODO: store in a global temporary vector
  Vec tmp;
  petsc_call(VecDuplicate(phi, &tmp));

  // set dirichlet BCs on the time derivative and multiply by M
  petsc_call(VecCopy(phi_dot, tmp));
  feenox_call(feenox_problem_dirichlet_set_phi_dot(tmp));
  petsc_call(MatMult(feenox.pde.M, tmp, r));
  
  // set dirichlet BCs on the solution and multiply by K
  petsc_call(VecCopy(phi, tmp));
  feenox_call(feenox_problem_dirichlet_set_phi(tmp));

  petsc_call(MatMultAdd(feenox.pde.K, tmp, r, r));
  petsc_call(VecDestroy(&tmp));
  
  petsc_call(VecAXPY(r, -1.0, feenox.pde.b));

  // set dirichlet bcs on the residual
  feenox_call(feenox_problem_dirichlet_set_r(r, phi));
  
  return FEENOX_OK;
}

PetscErrorCode feenox_ts_jacobian(TS ts, PetscReal t, Vec phi, Vec phi_dot, PetscReal s, Mat J, Mat P, void *ctx) {

  // return (K + s*M)_dirichlet
  petsc_call(MatCopy(feenox.pde.K, J, SAME_NONZERO_PATTERN));
  petsc_call(MatAXPY(J, s, feenox.pde.M, SAME_NONZERO_PATTERN));
  feenox_call(feenox_problem_dirichlet_set_J(J));
  
  return FEENOX_OK;
}

#endif
