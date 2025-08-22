/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's transient solver using PETSc routines
 *
 *  Copyright (C) 2021-2025 Jeremy Theler
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
        
        // when using PSEUDO, we should not solve the initial SNES because
        // in pseudo we do not have to scale the BCs so the full problem is
        // already set up at t=0 and then we don't take advantage of pseudo
        int do_not_solve_update = 0;
        int do_not_solve_old = feenox.pde.do_not_solve;
        if (feenox.pde.ts_type != NULL && strcmp(feenox.pde.ts_type, TSPSEUDO) == 0) {
          do_not_solve_update = 1;
          feenox.pde.do_not_solve = 1;
        }
        
        feenox_call(feenox_problem_solve_petsc_nonlinear());
        petsc_call(SNESDestroy(&feenox.pde.snes));
        feenox.pde.snes = NULL;
        
        if (do_not_solve_update) {
          feenox.pde.do_not_solve = do_not_solve_old;
        }          
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

      // THINK! whats is going on here?
      feenox_check_alloc(feenox.pde.J_ts = feenox_problem_create_matrix("J_ts"));
//      petsc_call(MatDuplicate(feenox.pde.has_jacobian_K ? feenox.pde.JK : feenox.pde.K, MAT_COPY_VALUES, &feenox.pde.J_ts));
      petsc_call(TSSetIJacobian(feenox.pde.ts, feenox.pde.J_ts, feenox.pde.J_ts, feenox_ts_jacobian, NULL));

      petsc_call(TSSetProblemType(feenox.pde.ts, (feenox.pde.math_type == math_type_linear) ? TS_LINEAR : TS_NONLINEAR));

      // if BCs depend on time we need DAEs
      petsc_call(TSSetEquationType(feenox.pde.ts, TS_EQ_IMPLICIT));      
  //      petsc_call(TSSetEquationType(feenox.pde.ts, TS_EQ_DAE_IMPLICIT_INDEX1));
  //      petsc_call(TSARKIMEXSetFullyImplicit(feenox.pde.ts, PETSC_TRUE)); 

      feenox_call(feenox_problem_setup_ts(feenox.pde.ts));

      petsc_call(TSSetTimeStep(feenox.pde.ts, feenox_special_var_value(dt)));
      petsc_call(TSSetMaxTime(feenox.pde.ts, 0.0));
      petsc_call(TSSolve(feenox.pde.ts, feenox.pde.phi));

      petsc_call(TSSetMaxTime(feenox.pde.ts, feenox_special_var_value(end_time)));
      petsc_call(TSSetExactFinalTime(feenox.pde.ts, TS_EXACTFINALTIME_MATCHSTEP));
    }  
    
    return FEENOX_OK;
    
  }
  
  if (feenox.pde.do_not_solve == 0) {  
    PetscInt ts_step = 0;
    petsc_call(TSGetStepNumber(feenox.pde.ts, &ts_step));
    petsc_call(TSSetMaxSteps(feenox.pde.ts, ts_step+1));

    petsc_call(TSSetMaxTime(feenox.pde.ts, feenox_special_var_value(end_time)));
    petsc_call(TSSetTimeStep(feenox.pde.ts, feenox_special_var_value(dt)));
    petsc_call(TSSolve(feenox.pde.ts, feenox.pde.phi));  
    petsc_call(TSGetTime(feenox.pde.ts, feenox_value_ptr(feenox_special_var(t))));  
    petsc_call(TSGetTimeStep(feenox.pde.ts, feenox_value_ptr(feenox_special_var(dt))));  
  }
  
#endif
  
  return FEENOX_OK;
  
}


#ifdef HAVE_PETSC

int feenox_problem_setup_ts(TS ts) {
  
  
  if (feenox.pde.ts_type != NULL) {
    petsc_call(TSSetType(ts, feenox.pde.ts_type));
  // } else if (feenox.pde.transient_type == transient_type_quasistatic) {
  //   // TODO: the default depends on the physics type
  //   petsc_call(TSSetType(ts, TSBEULER));
  } else {
    // TODO: the default depends on the physics type
    petsc_call(TSSetType(ts, TSBDF));
  }

  TSAdapt adapt;
  petsc_call(TSGetAdapt(ts, &adapt));
  petsc_call(TSAdaptSetType(adapt, (feenox.pde.ts_adapt_type != NULL) ? feenox.pde.ts_adapt_type : TSADAPTBASIC));
  // the factors are so we can pass min_dt = max_dt
  petsc_call(TSAdaptSetStepLimits(adapt, (feenox_special_var_value(min_dt) > 0) ? (1-1e-8)*feenox_special_var_value(min_dt) : PETSC_DEFAULT,
                                         (feenox_special_var_value(max_dt) > 0) ? (1+1e-8)*feenox_special_var_value(max_dt) : feenox_special_var_value(end_time)));
  
  // TODO: choose
//  petsc_call(TSSetMaxStepRejections(feenox.pde.ts, 10000));
//  petsc_call(TSSetMaxSNESFailures(feenox.pde.ts, 1000));

  // PETSc 3.19 is buggy and needs this guys here
  // the problem is that calling TSSetFromOptions here sets the line search to bt
  // and we want to default to basic
  if (PETSC_VERSION_MAJOR == 3 && PETSC_VERSION_MINOR == 19) {
    petsc_call(TSSetFromOptions(ts));    
  }
  
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

  // options overwrite
  petsc_call(TSSetFromOptions(ts));    


  return FEENOX_OK;
}

PetscErrorCode feenox_ts_residual(TS ts, PetscReal t, Vec phi, Vec phi_dot, Vec r, void *ctx) {
  
  feenox_special_var_value(t) = t;

  // TODO: store in a global temporary vector
  Vec phi_bc;
  petsc_call(VecDuplicate(phi, &phi_bc));
  petsc_call(VecCopy(phi, phi_bc));

  feenox_call(feenox_problem_dirichlet_eval());  
  if (feenox.pde.math_type == math_type_nonlinear) {
    feenox_call(feenox_problem_dirichlet_set_phi(phi_bc));
    feenox_call(feenox_problem_phi_to_solution(phi_bc, 0));
    if (feenox.pde.phi_bc != NULL) {
      petsc_call(VecCopy(phi_bc, feenox.pde.phi_bc));
    }
  }
  
  // TODO: for time-dependent neumann BCs it should not be needed to re-build the whole matrix, just the RHS
  feenox_call(feenox_problem_build());

  // compute the residual R(t,phi,phi_dot) = M*(phi_dot)_dirichlet + K*(phi)_dirichlet - b  (linear)
  //                                       = M*(phi_dot)_dirichlet + f(phi)_dirichlet - b   (non-linear)
  
  // TODO: start with the K/f term and add the mass later
  // set dirichlet BCs on the time derivative and multiply by M
  if (feenox.pde.M != NULL) {
    Vec phi_dot_bc;
    petsc_call(VecDuplicate(phi, &phi_dot_bc));
    petsc_call(VecCopy(phi_dot, phi_dot_bc));
    feenox_call(feenox_problem_dirichlet_set_phi_dot(phi_dot_bc));
    petsc_call(MatMult(feenox.pde.M, phi_dot_bc, r));
    petsc_call(VecDestroy(&phi_dot_bc));
  } else  {
    petsc_call(VecZeroEntries(r));
  }
  
  // set dirichlet BCs on the solution compute the residual
  if (feenox.pde.has_internal_fluxes) {
    // if the problem provides an internal flux, use it
    petsc_call(VecCopy(feenox.pde.f, r));
  } else {
    // otherwise make it up from the stiffness and the solution
    petsc_call(MatMultAdd(feenox.pde.K, phi_bc, r, r));
  }
  petsc_call(VecDestroy(&phi_bc));
  
  petsc_call(VecAXPY(r, -1.0, feenox.pde.b));

  // set dirichlet bcs on the residual
  feenox_call(feenox_problem_dirichlet_set_r(r, phi));

  
  return FEENOX_OK;
}

PetscErrorCode feenox_ts_jacobian(TS ts, PetscReal t, Vec phi, Vec phi_dot, PetscReal s, Mat J_ts, Mat P, void *ctx) {

  petsc_call(MatAssemblyBegin(feenox.pde.K, MAT_FINAL_ASSEMBLY));
  petsc_call(MatAssemblyEnd(feenox.pde.K, MAT_FINAL_ASSEMBLY));
  petsc_call(MatAssemblyBegin(J_ts, MAT_FINAL_ASSEMBLY));
  petsc_call(MatAssemblyEnd(J_ts, MAT_FINAL_ASSEMBLY));
  
  // return (K + JK - Jb + s*M)_bc
  petsc_call(MatCopy(feenox.pde.K, J_ts, DIFFERENT_NONZERO_PATTERN));
  if (feenox.pde.has_jacobian_K) {
    petsc_call(MatAXPY(J_ts, +1.0, feenox.pde.JK, DIFFERENT_NONZERO_PATTERN));
  }
  if (feenox.pde.has_jacobian_b) {
    petsc_call(MatAXPY(J_ts, -1.0, feenox.pde.Jb, DIFFERENT_NONZERO_PATTERN));
  }
  
  if (feenox.pde.has_mass) {
    petsc_call(MatAXPY(J_ts, s, feenox.pde.M, DIFFERENT_NONZERO_PATTERN));
  }
  feenox_call(feenox_problem_dirichlet_set_J(J_ts));
  
  return FEENOX_OK;
}

#endif
