#include "feenox.h"
#include "modal.h"
extern feenox_t feenox;
extern modal_t modal;

int feenox_problem_solve_post_modal(void) {

#ifdef HAVE_SLEPC

  Vec one;
  petsc_call(VecDuplicate(feenox.pde.phi, &one));
  petsc_call(VecSet(one, 1.0));

  Vec Mone;
  petsc_call(VecDuplicate(feenox.pde.phi, &Mone));
  petsc_call(MatMult(feenox.pde.M, one, Mone));

  // phi depends on i, so here we allocate but fill it below
  Vec Mphi;
  petsc_call(VecDuplicate(feenox.pde.phi, &Mphi));

  // total mass (scalar)
  PetscScalar oneMone;
  petsc_call(VecDot(one, Mone, &oneMone));
  feenox_var_value(modal.M_T) = oneMone/(PetscScalar)feenox.pde.dofs;
  
  // accumulator
  PetscScalar Mu = 0;
  
  // TODO: this is pretty inefficient
  PetscScalar omega, norm, mu, phiMphi, phiMone, Gamma;
  unsigned int i;
  for (i = 0; i < feenox.pde.nev; i++) {
    if (feenox.pde.eigen_formulation == eigen_formulation_omega) {
      omega = sqrt(feenox.pde.eigenvalue[i]);
    } else {
      omega = 1.0/sqrt(feenox.pde.eigenvalue[i]);
    }
    
    feenox_call(feenox_vector_set(modal.omega, i, omega));
    // convert it to cycles per time
    feenox_call(feenox_vector_set(modal.f, i, omega/(2*M_PI)));

    // normalization works like this: first we normalize to max = 1
    // so we can compare the scalar products with the mass matrix
    // as a norm against 1'*M*1
    petsc_call(VecNorm(feenox.pde.eigenvector[i], NORM_INFINITY, &norm));
    petsc_call(VecScale(feenox.pde.eigenvector[i], 1.0/norm));

    // now we compute the product M*phi which we are going to use
    // below in 1'*M*phi and in phi'*M*phi
    petsc_call(MatMult(feenox.pde.M, feenox.pde.eigenvector[i], Mphi));
    
    // modal mass
    petsc_call(VecDot(feenox.pde.eigenvector[i], Mphi, &phiMphi));
    feenox_call(feenox_vector_set(modal.m, i, phiMphi));

    // excitation factor
    petsc_call(VecDot(feenox.pde.eigenvector[i], Mone, &phiMone));
    feenox_call(feenox_vector_set(modal.L, i, phiMone));

    // participacion factor
    Gamma = phiMone/(feenox.pde.dofs * phiMphi);
    feenox_call(feenox_vector_set(modal.Gamma, i, Gamma));
    
    // effective mass
    mu = gsl_pow_2(phiMone)/(oneMone * phiMphi);
    feenox_call(feenox_vector_set(modal.mu, i, mu));
    
    // accumulated effective mass
    Mu += mu;
    feenox_call(feenox_vector_set(modal.Mu, i, Mu));
    
    // now we have to re-normalize the eigenvector such that the maximum displacement
    // sqrt(u^2+v^2+w^2) is equal to one and then we multiply by the excitation factor Gamma
    // TODO: parallel
    PetscScalar norm = -1;
    PetscScalar chi, xi;
    PetscInt index = 0;
    size_t j = 0;
    unsigned int g = 0;
    for (j = 0; j < feenox.pde.spatial_unknowns; j++) {
      chi = 0;
      for (g = 0; g < feenox.pde.dofs; g++) {
        index = feenox.pde.mesh->node[j].index_dof[g];
        feenox_call(VecGetValues(feenox.pde.eigenvector[i], 1, &index, &xi));
        chi += gsl_pow_2(xi);
      }
      
      if (chi > norm) {
        norm = chi;
      }
    }  
    feenox_call(VecScale(feenox.pde.eigenvector[i], Gamma/sqrt(norm)));
  }
  
#endif

  return FEENOX_OK;
}
