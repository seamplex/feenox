#include "feenox.h"
extern feenox_t feenox;

int feenox_solve_petsc_transient(void) {

#ifdef HAVE_PETSC

  // TODO: handle initial steady state
  // these might have been needed for computing the initial steady-state?
/*  
  if (feenox.pde.ksp != NULL) {
    petsc_call(KSPDestroy(&feenox.pde.ksp));
    feenox.pde.ksp = NULL;
  } else if (feenox.pde.snes != NULL) {
    petsc_call(SNESDestroy(&feenox.pde.snes));
    feenox.pde.snes = NULL;
  }
*/
  if (feenox.pde.ts == NULL) {
    // put these lines in a separate function
    petsc_call(TSCreate(PETSC_COMM_WORLD, &feenox.pde.ts));

    petsc_call(TSSetIFunction(feenox.pde.ts, NULL, fino_ts_residual, NULL));
    // TODO: check this
    // if we were given an initial condition then K does not exist
//    if (feenox.pde.initial_condition != NULL) {      
      feenox_call(feenox_build());
//    }  

    petsc_call(MatDuplicate((feenox.pde.has_jacobian == PETSC_FALSE) ? feenox.pde.K : feenox.pde.J, MAT_COPY_VALUES, &feenox.pde.J_tran));
    petsc_call(TSSetIJacobian(feenox.pde.ts, feenox.pde.J_tran, feenox.pde.J_tran, fino_ts_jacobian, NULL));

    // TODO: we already know which problem type we have
    petsc_call(TSSetProblemType(feenox.pde.ts, TS_NONLINEAR));
//    petsc_call(TSSetProblemType(feenox.pde.ts, TS_LINEAR));
    petsc_call(TSSetTimeStep(feenox.pde.ts, feenox_var_value(feenox_special_var(dt))));

    // if BCs depend on time we need DAEs
    petsc_call(TSSetEquationType(feenox.pde.ts, TS_EQ_IMPLICIT));      
//      petsc_call(TSSetEquationType(feenox.pde.ts, TS_EQ_DAE_IMPLICIT_INDEX1));
//      petsc_call(TSARKIMEXSetFullyImplicit(feenox.pde.ts, PETSC_TRUE)); 
    
    // TODO: TSAdapt
    
    feenox_call(feenox_setup_ts(feenox.pde.ts));

  }

  PetscInt ts_steps = 0;
  petsc_call(TSGetStepNumber(feenox.pde.ts, &ts_steps));
  petsc_call(TSSetMaxSteps(feenox.pde.ts, ts_steps+1));

  petsc_call(TSSolve(feenox.pde.ts, feenox.pde.phi));

  // this is the "new" dt
  petsc_call(TSGetTimeStep(feenox.pde.ts, feenox_value_ptr(feenox_special_var(dt))));  

#endif
  
  return FEENOX_OK;
  
}


#ifdef HAVE_PETSC

int feenox_setup_ts(TS ts) {
  
  // TODO: the default depends on the physics type
  if (feenox.pde.ts_type != NULL) {
    petsc_call(TSSetType(ts, feenox.pde.ts_type));
  } else {
    petsc_call(TSSetType(ts, TSBDF));
//      petsc_call(TSSetType(feenox.pde.ts, TSARKIMEX));
  }

  // TODO: choose
  TSAdapt adapt;
  petsc_call(TSGetAdapt(ts, &adapt));
  petsc_call(TSAdaptSetType(adapt, TSADAPTBASIC));

//  petsc_call(TSAdaptSetStepLimits(adapt, min_dt, max_dt));
  
  // TODO: choose
//  petsc_call(TSSetMaxStepRejections(feenox.pde.ts, 10000));
//  petsc_call(TSSetMaxSNESFailures(feenox.pde.ts, 1000));
//  petsc_call(TSSetExactFinalTime(feenox.pde.ts, TS_EXACTFINALTIME_STEPOVER));

  // options overwrite
  petsc_call(TSSetFromOptions(ts));    
  // TODO: this guy complains about DM (?)
//  petsc_call(TSSetUp(ts));    
  
  SNES snes;
  petsc_call(TSGetSNES(ts, &snes));
  if (snes != NULL) {
    feenox_call(feenox_setup_snes(snes));
  } else {
    KSP ksp;
    petsc_call(TSGetKSP(ts, &ksp));
    if (ksp != NULL) {
      feenox_call(feenox_setup_ksp(ksp));
    }
  }  



  return FEENOX_OK;
}

PetscErrorCode fino_ts_residual(TS ts, PetscReal t, Vec phi, Vec phi_dot, Vec r, void *ctx) {
  
//  static int count = 0;
  feenox_var_value(feenox_special_var(t)) = t;

//  if (fino.math_type == math_type_nonlinear) {
    // TODO: know when to recompute the matrix or not
    feenox_call(feenox_phi_to_solution(phi, 1));
    feenox_call(feenox_build());
    feenox_call(feenox_dirichlet_eval());
//    feenox_call(feenox_dirichlet_set_J(feenox.pde.J));
//  }  

  // compute the residual R(t,phi,phi_dot) = M*(phi_dot)_dirichlet + K*(phi)_dirichlet - b
  
  // TODO: store in a global temporary vector
  Vec tmp;
  VecDuplicate(phi, &tmp);

  // set dirichlet BCs on the time derivative and multiply by M
  VecCopy(phi_dot, tmp);
  feenox_call(feenox_dirichlet_set_phi_dot(tmp));
  petsc_call(MatMult(feenox.pde.M, tmp, r));

  // set dirichlet BCs on the solution and multiply by K
  VecCopy(phi, tmp);
  feenox_call(feenox_dirichlet_set_phi(tmp));
  petsc_call(MatMultAdd(feenox.pde.K, tmp, r, r));
  
  VecDestroy(&tmp);
  
  petsc_call(VecAXPY(r, -1.0, feenox.pde.b));

  // set dirichlet bcs on the residual
  feenox_call(feenox_dirichlet_set_r(r, phi));

  return FEENOX_OK;
}

PetscErrorCode fino_ts_jacobian(TS ts, PetscReal t, Vec phi, Vec phi_dot, PetscReal s, Mat J, Mat P, void *ctx) {

  // return (K + s*M)_dirichlet
  petsc_call(MatCopy(feenox.pde.K, J, SAME_NONZERO_PATTERN));
  petsc_call(MatAXPY(J, s, feenox.pde.M, SAME_NONZERO_PATTERN));
  feenox_call(feenox_dirichlet_set_J(J));
  
  return FEENOX_OK;
}
#endif