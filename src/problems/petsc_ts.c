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
    // this fails in parallel because J has to be assembled
//    if (feenox.pde.initial_condition != NULL) {      
//      feenox_call(feenox_build());
//    }  

    Mat J = (feenox.pde.has_jacobian == PETSC_FALSE) ? feenox.pde.K : feenox.pde.J;
    petsc_call(TSSetIJacobian(feenox.pde.ts, J, J, fino_ts_jacobian, NULL));

    // TODO: we already know which problem type we have
//    petsc_call(TSSetProblemType(feenox.pde.ts, TS_NONLINEAR));
    petsc_call(TSSetProblemType(feenox.pde.ts, TS_LINEAR));    
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
  
  feenox_var_value(feenox_special_var(t)) = t;

//  if (fino.math_type == math_type_nonlinear) {
    // TODO: know when to recompute the matrix or not
    feenox_call(feenox_phi_to_solution(phi, 1));
    feenox_call(feenox_build());
    feenox_call(feenox_dirichlet_eval());
//  }  

//  printf("t = %g\n", t);
//  printf("phi\n");
//  VecView(phi, PETSC_VIEWER_STDOUT_WORLD);
//  printf("phi_dot\n");
//  VecView(phi_dot, PETSC_VIEWER_STDOUT_WORLD);
    
//  printf("M\n");
//  MatView(feenox.pde.M, PETSC_VIEWER_STDOUT_WORLD);
  
  // compute the residual R(t,phi,phi_dot) = K*phi + M*phi_dot - b
//  VecSet(r, 0.0);
  petsc_call(MatMult(feenox.pde.M, phi_dot, r));

//  printf("K\n");
//  MatView(feenox.pde.K, PETSC_VIEWER_STDOUT_WORLD);
  
//  printf("M phi_dot\n");
//  VecView(r, PETSC_VIEWER_STDOUT_WORLD);
  
  petsc_call(MatMultAdd(feenox.pde.K, phi, r, r));
//  printf("M phi_dot + K phi\n");
//  VecView(r, PETSC_VIEWER_STDOUT_WORLD);
  
  petsc_call(VecAXPY(r, -1.0, feenox.pde.b));
//  printf("M phi_dot + K phi - b\n");
//  VecView(r, PETSC_VIEWER_STDOUT_WORLD);

  // set dirichlet bcs  
  feenox_call(feenox_dirichlet_set_r(r, phi));
//  printf("M phi_dot + K phi - b with Dirichlet BCs\n");
//  VecView(r, PETSC_VIEWER_STDOUT_WORLD);
  
/*  
    PetscViewer viewer;
    PetscViewerASCIIOpen(PETSC_COMM_WORLD, "feenox-K.m", &viewer);
    PetscViewerSetType(viewer, PETSCVIEWERASCII);
    PetscViewerPushFormat(viewer, PETSC_VIEWER_ASCII_MATLAB);
    MatView(feenox.pde.K, viewer);
    PetscViewerDestroy(&viewer);

    PetscViewerASCIIOpen(PETSC_COMM_WORLD, "feenox-M.m", &viewer);
    PetscViewerSetType(viewer, PETSCVIEWERASCII);
    PetscViewerPushFormat(viewer, PETSC_VIEWER_ASCII_MATLAB);
    MatView(feenox.pde.M, viewer);
    PetscViewerDestroy(&viewer);
    
  exit(0);
*/  
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