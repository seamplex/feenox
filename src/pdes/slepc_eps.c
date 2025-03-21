/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's eigen solver using SLEPc routines
 *
 *  Copyright (C) 2021--2023 Jeremy Theler
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

int feenox_problem_solve_slepc_eigen(void) {

#ifdef HAVE_SLEPC

  // build ---------------------------------------------------------------------
  petsc_call(PetscLogStagePush(feenox.pde.stage_build));  

  // we need the matrices to set the operators
  feenox_call(feenox_problem_build());
  feenox_call(feenox_problem_dirichlet_eval());
  feenox_call(feenox_problem_dirichlet_set_K());  
  feenox_call(feenox_problem_dirichlet_set_M());
  
  petsc_call(PetscLogStagePop());
  // ---------------------------------------------------------------------------
  
  if (feenox.pde.missed_dump != NULL) {
    feenox_call(feenox_instruction_dump(feenox.pde.missed_dump));
  }

  // solve ---------------------------------------------------------------------
  petsc_call(PetscLogStagePush(feenox.pde.stage_solve));    
  if (feenox.pde.eps == NULL) {
    petsc_call(EPSCreate(PETSC_COMM_WORLD, &feenox.pde.eps));
    if (feenox.pde.eps_type != NULL) {
      petsc_call(EPSSetType(feenox.pde.eps, feenox.pde.eps_type));
    }

    // here we might choose whether to use lambda or omega
    if (feenox.pde.setup_eps != NULL) {
      feenox_call(feenox.pde.setup_eps(feenox.pde.eps));
    }  

    // default is lambda but each PDE should choose whatever works better
    if (feenox.pde.eigen_formulation == eigen_formulation_undefined) {
      feenox.pde.eigen_formulation = eigen_formulation_lambda;
    }
    
    // operators depending on formulation
    if (feenox.pde.eigen_formulation == eigen_formulation_lambda) {
      petsc_call(EPSSetOperators(feenox.pde.eps, feenox.pde.M_bc, feenox.pde.K_bc));
      petsc_call(EPSSetWhichEigenpairs(feenox.pde.eps, EPS_LARGEST_MAGNITUDE));
    } else if (feenox.pde.eigen_formulation == eigen_formulation_omega) {
      petsc_call(EPSSetOperators(feenox.pde.eps, feenox.pde.K_bc, feenox.pde.M_bc));
      petsc_call(EPSSetWhichEigenpairs(feenox.pde.eps, EPS_SMALLEST_MAGNITUDE));
    } else {
      feenox_push_error_message("internal error, neither omega nor lambda set");
      return FEENOX_ERROR;
    }
    
    
    // get the associated spectral transformation
    ST st = NULL;
    petsc_call(EPSGetST(feenox.pde.eps, &st));
    if (st == NULL) {
      feenox_push_error_message("cannot retrieve spectral transformation object");
      return FEENOX_ERROR;
    }
    if (feenox.pde.st_type != NULL) {
      petsc_call(STSetType(st, feenox.pde.st_type));
    } else {
      if ((feenox.pde.eigen_formulation == eigen_formulation_lambda && feenox.pde.eigen_dirichlet_zero == eigen_dirichlet_zero_M) ||
          (feenox.pde.eigen_formulation == eigen_formulation_omega && feenox.pde.eigen_dirichlet_zero == eigen_dirichlet_zero_K)) {
        petsc_call(STSetType(st, STSHIFT));
      } else {
        petsc_call(STSetType(st, STSINVERT));
      }
    }

    STType sttype = NULL;
    feenox_call(STGetType(st, &sttype));
    if (strcmp(sttype, STSINVERT) == 0 || strcmp(sttype, STCAYLEY) == 0) {
      // shift and invert needs a target
      petsc_call(EPSSetTarget(feenox.pde.eps, 0));
      petsc_call(EPSSetWhichEigenpairs(feenox.pde.eps, EPS_TARGET_MAGNITUDE));
    }
    
    // shift and invert offsets
    if (feenox_var_value(feenox.pde.vars.eps_st_sigma) != 0) {
      petsc_call(STSetShift(st, feenox_var_value(feenox.pde.vars.eps_st_sigma)));
    }
    if (feenox_var_value(feenox.pde.vars.eps_st_nu) != 0) {
      petsc_call(STCayleySetAntishift(st, feenox_var_value(feenox.pde.vars.eps_st_nu)));
    }
    

    // tolerances
    petsc_call(EPSSetTolerances(feenox.pde.eps, feenox_var_value(feenox.pde.vars.eps_tol),
                                                (PetscInt)feenox_var_value(feenox.pde.vars.eps_max_it)));
    
    // setup the linear solver
    KSP ksp = NULL;
    petsc_call(STGetKSP(st, &ksp));
    if (ksp == NULL) {
      feenox_push_error_message("cannot retrieve linear solver object");
      return FEENOX_ERROR;
    }
    feenox_call(feenox_problem_setup_ksp(ksp));

    // this should be faster but it is not
    // TODO: let the user choose
    EPSProblemType eps_type;
    petsc_call(EPSGetProblemType(feenox.pde.eps, &eps_type));
    if (eps_type == 0)  {
      petsc_call(EPSSetProblemType(feenox.pde.eps, (feenox.pde.symmetric_K && feenox.pde.symmetric_M) ? EPS_GHEP : EPS_GNHEP));
    }
    
    // specify how many eigenvalues (and eigenvectors) to compute.
    if (feenox.pde.eps_ncv.items != NULL) {
      petsc_call(EPSSetDimensions(feenox.pde.eps, feenox.pde.nev, (PetscInt)(feenox_expression_eval(&feenox.pde.eps_ncv)), PETSC_DEFAULT));
    } else {
      petsc_call(EPSSetDimensions(feenox.pde.eps, feenox.pde.nev, PETSC_DEFAULT, PETSC_DEFAULT));
    }
    
    feenox_check_alloc(feenox.pde.eigenvalue = calloc(feenox.pde.nev, sizeof(PetscScalar)));
    feenox_check_alloc(feenox.pde.eigenvector = calloc(feenox.pde.nev, sizeof(Vec)));

    petsc_call(EPSSetFromOptions(feenox.pde.eps));
    
  }

  petsc_call(EPSSetInitialSpace(feenox.pde.eps, 1, &feenox.pde.phi));
  petsc_call(EPSSolve(feenox.pde.eps));
  PetscInt nconv = 0;
  petsc_call(EPSGetConverged(feenox.pde.eps, &nconv));

  if (nconv == 0) {
    feenox_push_error_message("no converged eigen-pairs found (%d requested)", feenox.pde.nev);
    return FEENOX_ERROR;
  } else if (nconv < feenox.pde.nev) {
    feenox_push_error_message("eigen-solver obtained only %d converged eigen-pairs (%d requested)", nconv, feenox.pde.nev);
    return FEENOX_ERROR;
  }

  PetscScalar imag = 0;
  unsigned int i = 0;
  for (i = 0; i < feenox.pde.nev; i++) {
    petsc_call(MatCreateVecs(feenox.pde.K, NULL, &feenox.pde.eigenvector[i]));
    petsc_call(EPSGetEigenpair(feenox.pde.eps, i, &feenox.pde.eigenvalue[i], &imag, feenox.pde.eigenvector[i], PETSC_NULLPTR));

    // TODO: should we allow complex eigenvalues?
    if (fabs(imag) > feenox_var_value(feenox.pde.vars.eps_tol)) {
      feenox_push_error_message("the eigenvalue %d is complex (%g + i %g)", i+1, feenox.pde.eigenvalue[i], imag);
      return FEENOX_ERROR;
    }
  }
  petsc_call(PetscLogStagePop());
  // ---------------------------------------------------------------------------
#endif
  
  return FEENOX_OK;
  
}
