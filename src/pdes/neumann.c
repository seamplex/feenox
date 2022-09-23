/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's handling of Neumann BCs
 *
 *  Copyright (C) 2021--2022 jeremy theler
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
extern double zero[3];

int feenox_problem_bc_natural_set(element_t *e, unsigned int v, double *value) {

#ifdef HAVE_PETSC
  for (unsigned int g = 0; g < feenox.pde.dofs; g++) {
    gsl_vector_set(feenox.pde.Nb, g, value[g]);
  }  
  double w = feenox_problem_bc_natural_weight(e, v);
  feenox_call(gsl_blas_dgemv(CblasTrans, w, e->H[v], feenox.pde.Nb, 1.0, feenox.pde.bi));
#endif
  
  return FEENOX_OK;
}

double feenox_problem_bc_natural_weight(element_t *e, unsigned int v) {
  feenox_mesh_compute_wH_at_gauss(e, v);

  // TODO: axisymmetric
//  r_for_axisymmetric = feenox_compute_r_for_axisymmetric(this, v);
  double r_for_axisymmetric = 1;
  return e->w[v] * r_for_axisymmetric;
}

double *feenox_problem_bc_natural_x(element_t *e, bc_data_t *bc_data, unsigned int v) {
  double *x = zero;
  if (bc_data->space_dependent) {
    feenox_mesh_compute_x_at_gauss(e, v, feenox.pde.mesh->integration);
    x = e->x[v];
    feenox_mesh_update_coord_vars(x);
  }
  
  return x;
}

