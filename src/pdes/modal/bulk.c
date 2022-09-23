/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for modal analysis: bulk elements
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

int feenox_problem_build_volumetric_gauss_point_modal(element_t *e, unsigned int v) {

#ifdef HAVE_PETSC
  
  feenox_call(feenox_mesh_compute_wHB_at_gauss(e, v));
  
  double *x = NULL;
  if (modal.space_E || modal.space_nu || modal.space_rho) {
    feenox_call(feenox_mesh_compute_x_at_gauss(e, v, feenox.pde.mesh->integration));
    x = e->x[v];
  }
  
  // TODO: axisymmetric
//  r_for_axisymmetric = feenox_compute_r_for_axisymmetric(this, v);
  double r_for_axisymmetric = 1;
  double w = e->w[v] * r_for_axisymmetric;
  
  material_t *material = NULL;
  if (e->physical_group != NULL && e->physical_group->material != NULL) {
    material = e->physical_group->material;
  }

  gsl_matrix *C = gsl_matrix_calloc(6, 6);
  double E = modal.E.eval(&modal.E, x, material);
  double nu = modal.nu.eval(&modal.nu, x, material);
  double rho = modal.rho.eval(&modal.rho, x, material);
  
  double lambda = E*nu/((1+nu)*(1-2*nu));
  double mu = 0.5*E/(1+nu);
  double lambda2mu = lambda + 2*mu;
  
  gsl_matrix_set(C, 0, 0, lambda2mu);
  gsl_matrix_set(C, 0, 1, lambda);
  gsl_matrix_set(C, 0, 2, lambda);

  gsl_matrix_set(C, 1, 0, lambda);
  gsl_matrix_set(C, 1, 1, lambda2mu);
  gsl_matrix_set(C, 1, 2, lambda);

  gsl_matrix_set(C, 2, 0, lambda);
  gsl_matrix_set(C, 2, 1, lambda);
  gsl_matrix_set(C, 2, 2, lambda2mu);
  
  gsl_matrix_set(C, 3, 3, mu);
  gsl_matrix_set(C, 4, 4, mu);
  gsl_matrix_set(C, 5, 5, mu);
  
  unsigned int J = e->type->nodes;
  gsl_matrix *B = gsl_matrix_calloc(6, feenox.pde.dofs * J);
  gsl_matrix *CB = gsl_matrix_calloc(6, feenox.pde.dofs * J);
  
  gsl_matrix *dhdx = e->dhdx[v];

  unsigned int j = 0;
  for (j = 0; j < J; j++) {
    gsl_matrix_set(B, 0, 3*j+0, gsl_matrix_get(dhdx, j, 0));
      
    gsl_matrix_set(B, 1, 3*j+1, gsl_matrix_get(dhdx, j, 1));
      
    gsl_matrix_set(B, 2, 3*j+2, gsl_matrix_get(dhdx, j, 2));
    
    gsl_matrix_set(B, 3, 3*j+0, gsl_matrix_get(dhdx, j, 1));
    gsl_matrix_set(B, 3, 3*j+1, gsl_matrix_get(dhdx, j, 0));

    gsl_matrix_set(B, 4, 3*j+1, gsl_matrix_get(dhdx, j, 2));
    gsl_matrix_set(B, 4, 3*j+2, gsl_matrix_get(dhdx, j, 1));

    gsl_matrix_set(B, 5, 3*j+0, gsl_matrix_get(dhdx, j, 2));
    gsl_matrix_set(B, 5, 3*j+2, gsl_matrix_get(dhdx, j, 0));
  }

  // elemental stiffness B'*C*B
  feenox_call(gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, C, B, 0, CB));
  feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, w, B, CB, 1.0, feenox.pde.Ki));

  // elemental mass H'*rho*H
  feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, w * rho, e->H[v], e->H[v], 1.0, feenox.pde.Mi));
  
  gsl_matrix_free(B);
  gsl_matrix_free(CB);
  gsl_matrix_free(C);
  
#endif
  
  return FEENOX_OK;
  
}

