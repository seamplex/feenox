/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for Laplace's equation: bulk elements
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
#include "laplace.h"
extern feenox_t feenox;
extern laplace_t laplace;
//double zero[3] = {0, 0, 0};

int feenox_problem_build_volumetric_gauss_point_laplace(element_t *this, unsigned int v) {

#ifdef HAVE_PETSC
  
  feenox_call(feenox_mesh_compute_w_at_gauss(this, v, feenox.pde.mesh->integration));
  feenox_call(feenox_mesh_compute_H_at_gauss(this, v, feenox.pde.dofs, feenox.pde.mesh->integration));
  feenox_call(feenox_mesh_compute_B_at_gauss(this, v, feenox.pde.dofs, feenox.pde.mesh->integration));
  double *x = NULL;
  if (laplace.space_dependent_source || laplace.space_dependent_mass) {
    feenox_call(feenox_mesh_compute_x_at_gauss(this, v, feenox.pde.mesh->integration));
    x = this->x[v];
  }
  
  // TODO: axisymmetric
//  r_for_axisymmetric = feenox_compute_r_for_axisymmetric(this, v);
  double r_for_axisymmetric = 1;
  double w = this->w[v] * r_for_axisymmetric;
  
  material_t *material = NULL;
  if (this->physical_group != NULL && this->physical_group->material != NULL) {
    material = this->physical_group->material;
  }

  // laplace stiffness matrix Bt*B
  gsl_blas_dgemm(CblasTrans, CblasNoTrans, w, this->B[v], this->B[v], 1.0, feenox.pde.Ki);
  
  // right-hand side
  // TODO: there should be a way to use BLAS instead of a for loop
  if (laplace.f.defined) {
    double f = laplace.f.eval(&laplace.f, x, material);
    unsigned int j = 0;
    for (j = 0; j < this->type->nodes; j++) {
      gsl_vector_add_to_element(feenox.pde.bi, j, w * this->type->gauss[feenox.pde.mesh->integration].h[v][j] * f);
    }
  }
  
  if (feenox.pde.has_jacobian) {
    // TODO: jacobian
  }
    
  // mass matrix Ht*rho*cp*H
  if (feenox.pde.has_mass) {
    double alpha = 0;
    if (laplace.alpha.defined) {
      alpha = laplace.alpha.eval(&laplace.alpha, x, material);
    } else {
      // this should have been already checked
      feenox_push_error_message("no alpha found");
      return FEENOX_ERROR;
    }
    feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, w * alpha, this->H[v], this->H[v], 1.0, feenox.pde.Mi));
  }
  

#endif
  
  return FEENOX_OK;
  
}
