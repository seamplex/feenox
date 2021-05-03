#include "feenox.h"
extern feenox_t feenox;

int feenox_solve_petsc_transient(void) {

#ifdef HAVE_PETSC
  
 PetscInt ts_steps;

  if (feenox.pde.ksp != NULL) {
    petsc_call(KSPDestroy(&feenox.pde.ksp));
    feenox.pde.ksp = NULL;
  } else if (feenox.pde.snes != NULL) {
    petsc_call(SNESDestroy(&feenox.pde.snes));
    feenox.pde.snes = NULL;
  }

   if (feenox.pde.ts == NULL) {
    petsc_call(TSCreate(PETSC_COMM_WORLD, &feenox.pde.ts));
    petsc_call(TSSetProblemType(feenox.pde.ts, TS_NONLINEAR));

    petsc_call(TSSetIFunction(feenox.pde.ts, NULL, fino_ts_residual, NULL));

    // si nos dieron una condicion inicial entonces feenox.pde.K no existe
    // en paralelo esto falla porque feenox.pde.J tiene que estar ensamblada y toda la milonga
    if (feenox.pde.initial_condition != NULL) {      
      feenox_call(feenox_build());
    }  
    // check if we have a jacobian or not
//    petsc_call(MatDuplicate(feenox.pde.K, MAT_DO_NOT_COPY_VALUES, &feenox.pde.J));
//    petsc_call(TSSetIJacobian(feenox.pde.ts, feenox.pde.J, feenox.pde.J, fino_ts_jacobian, NULL));

    petsc_call(TSSetTimeStep(feenox.pde.ts, feenox_var_value(feenox_special_var(dt))));

    // if BCs depend on time we need DAEs
    petsc_call(TSSetEquationType(feenox.pde.ts, TS_EQ_IMPLICIT));      
//      petsc_call(TSSetEquationType(feenox.pde.ts, TS_EQ_DAE_IMPLICIT_INDEX1));
//      petsc_call(TSARKIMEXSetFullyImplicit(feenox.pde.ts, PETSC_TRUE)); 

    // TODO: the default depends on the problem type
    if (feenox.pde.ts_type != NULL) {
      petsc_call(TSSetType(feenox.pde.ts, feenox.pde.ts_type));
    } else {
      petsc_call(TSSetType(feenox.pde.ts, TSBDF));
//        petsc_call(TSSetType(feenox.pde.ts, TSARKIMEX));
    }

    // TODO: choose
    petsc_call(TSSetMaxStepRejections(feenox.pde.ts, 10000));
    petsc_call(TSSetMaxSNESFailures(feenox.pde.ts, 1000));
    petsc_call(TSSetExactFinalTime(feenox.pde.ts, TS_EXACTFINALTIME_STEPOVER));

    // options overwrite
    petsc_call(TSSetFromOptions(feenox.pde.ts));    
  }

  petsc_call(TSGetStepNumber(feenox.pde.ts, &ts_steps));
  petsc_call(TSSetMaxSteps(feenox.pde.ts, ts_steps+1));

  petsc_call(TSSolve(feenox.pde.ts, feenox.pde.phi));
  petsc_call(feenox_phi_to_solution(feenox.pde.phi, 1));

  petsc_call(TSGetStepNumber(feenox.pde.ts, &ts_steps));
  petsc_call(TSGetTimeStep(feenox.pde.ts, feenox_value_ptr(feenox_special_var(dt))));  

#endif
  
  return FEENOX_OK;
  
}


#ifdef HAVE_PETSC
PetscErrorCode fino_ts_residual(TS ts, PetscReal t, Vec phi, Vec phi_dot, Vec r, void *ctx) {
  
  feenox_var_value(feenox_special_var(t)) = t;

//  if (fino.math_type == math_type_nonlinear) {
    // TODO: separate volumetric from surface elements
    // (in case only natural BCs change with time)
    feenox_call(feenox_phi_to_solution(phi, 1));
    feenox_call(feenox_build());
    feenox_call(feenox_dirichlet_eval());
//  }  
    
  // compute the residual R(t,phi,phi_dot) = K*phi + M*phi_dot - b
  petsc_call(MatMult(feenox.pde.K, phi, r));
  petsc_call(MatMultAdd(feenox.pde.M, phi_dot, r, r));
  petsc_call(VecAXPY(r, -1.0, feenox.pde.b));

  // set dirichlet bcs  
  feenox_call(feenox_dirichlet_set_r(r, phi));
  
  return FEENOX_OK;
}

PetscErrorCode fino_ts_jacobian(TS ts, PetscReal t, Vec T, Vec T_dot, PetscReal s, Mat J, Mat P,void *ctx) {

  Mat M;
  
  petsc_call(MatCopy(feenox.pde.K, J, SUBSET_NONZERO_PATTERN));
  feenox_call(feenox_dirichlet_set_J(J));
  
  petsc_call(MatDuplicate(feenox.pde.M, MAT_COPY_VALUES, &M));
  feenox_call(feenox_dirichlet_set_dRdphi_dot(M));

  petsc_call(MatAXPY(J, s, M, SAME_NONZERO_PATTERN));
  petsc_call(MatCopy(J, P, SAME_NONZERO_PATTERN));
  petsc_call(MatDestroy(&M));
  
  return FEENOX_OK;
}
#endif