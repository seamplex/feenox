/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for modal analysis: post
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
#include "modal.h"

int feenox_problem_solve_post_modal(void) {

#ifdef HAVE_SLEPC

  if (modal.f->used || modal.omega->used) {
    double omega = 0;
    for (unsigned int i = 0; i < feenox.pde.nev; i++) {
      if (feenox.pde.eigen_formulation == eigen_formulation_omega) {
        omega = sqrt(feenox.pde.eigenvalue[i]);
      } else {
        omega = 1.0/sqrt(feenox.pde.eigenvalue[i]);
      }

      feenox_call(feenox_vector_set(modal.omega, i, omega));
      // convert it to cycles per time
      feenox_call(feenox_vector_set(modal.f, i, omega/(2*M_PI)));
    }
  }
  
  if (modal.M_T->used ||
      modal.m->used ||
      modal.L->used ||
      modal.Gamma->used ||
      modal.mu->used ||
      modal.Mu->used) {
  
    Vec one = NULL;
    petsc_call(VecDuplicate(feenox.pde.phi, &one));
    petsc_call(VecSet(one, 1.0));

    Vec Mone = NULL;
    petsc_call(VecDuplicate(feenox.pde.phi, &Mone));
    petsc_call(MatMult(feenox.pde.M, one, Mone));

    // phi depends on i, so here we allocate but fill it below
    Vec Mphi = NULL;
    petsc_call(VecDuplicate(feenox.pde.phi, &Mphi));

    // total mass (scalar)
    PetscScalar oneMone = 0;
    petsc_call(VecDot(one, Mone, &oneMone));
    feenox_var_value(modal.M_T) = oneMone/(PetscScalar)feenox.pde.dofs;

    // accumulator
    PetscScalar Mu = 0;

    // TODO: this is pretty inefficient
    PetscScalar norm = 0;
    PetscScalar mu = 0;
    PetscScalar phiMphi = 0;
    PetscScalar phiMone = 0;
    PetscScalar Gamma = 0;
    for (unsigned int i = 0; i < feenox.pde.nev; i++) {

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
      PetscScalar chi = 0;
      PetscScalar xi = 0;
      PetscInt index = 0;
      for (size_t j = 0; j < feenox.pde.spatial_unknowns; j++) {
        chi = 0;
        for (unsigned int g = 0; g < feenox.pde.dofs; g++) {
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
  }
  
#endif

  return FEENOX_OK;
}
