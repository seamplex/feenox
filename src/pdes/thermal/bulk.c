/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for the heat equation: bulk elements
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
#include "thermal.h"

// TODO: put the gauss loop inside this call so we can cache number of nodes, etc
int feenox_problem_build_volumetric_gauss_point_thermal(element_t *e, unsigned int v) {

#ifdef HAVE_PETSC
  
  feenox_call(feenox_mesh_compute_wHB_at_gauss(e, v));
  double *x = feenox_mesh_compute_x_if_needed(e, v, thermal.space_dependent_stiffness || thermal.space_dependent_source || thermal.space_dependent_mass);
  material_t *material = feenox_mesh_get_material(e);  
  
  // TODO: axisymmetric
//  r_for_axisymmetric = feenox_compute_r_for_axisymmetric(this, v);
  double r_for_axisymmetric = 1;
  double w = e->w[v] * r_for_axisymmetric;
  
  // thermal stiffness matrix Bt*k*B
  // if k depends on T then there should be a more efficient way of evaluating k
  double k = thermal.k.eval(&thermal.k, x, material);
  feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, w*k, e->B[v], e->B[v], 1.0, feenox.pde.Ki));
  
  // volumetric heat source term Ht*q
  // TODO: total source Q
  if (thermal.q.defined) {
    double q = thermal.q.eval(&thermal.q, x, material);
    gsl_vector_const_view H = gsl_matrix_const_row(e->H[v], 0);
#ifdef HAVE_GSL_VECTOR_AXPBY
    feenox_call(gsl_vector_axpby(w*q, &H.vector, 1.0, feenox.pde.bi));
#endif
  }
  
  if (feenox.pde.has_jacobian) {
    gsl_matrix *local_vec_T = NULL;
    unsigned int nodes = e->type->nodes;
    feenox_check_alloc(local_vec_T = gsl_matrix_calloc(nodes, 1));
    double T = 0;
    double Tj = 0;
    for (unsigned int j = 0; j < nodes; j++) {
      Tj = feenox_vector_get(feenox.pde.solution[0]->vector_value, e->node[j]->index_dof[0]);
      gsl_matrix_set(local_vec_T, j, 0, Tj);
      T += gsl_matrix_get(e->H[v], 0, j) * Tj; 
    }  

    if (thermal.temperature_dependent_stiffness) {
      double dkdT = feenox_expression_derivative_wrt_function(thermal.k.expr, feenox.pde.solution[0], T);
      gsl_matrix *TH = gsl_matrix_calloc(nodes, nodes);
      gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1, local_vec_T, e->H[v], 1, TH);

      // add a convenience function, mind the allocation
      gsl_matrix *BtB = gsl_matrix_calloc(nodes, nodes);
      gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1, e->B[v], e->B[v], 1, BtB);

      // debug
      gsl_matrix *BtBTH = gsl_matrix_calloc(nodes, nodes);
      gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1, BtB, TH, 1, BtBTH);
//      printf("element %ld gauss %d\n", e->tag, v);
//      feenox_debug_print_gsl_matrix(BtBTH, stdout);
//      printf("\n");

      gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, w*dkdT, BtB, TH, 1, feenox.pde.JKi);
    }

    if (thermal.temperature_dependent_source) {
      double dqdT = feenox_expression_derivative_wrt_function(thermal.q.expr, feenox.pde.solution[0], T);
      // mind the positive sign!
      feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, w*dqdT, e->H[v], e->H[v], 1.0, feenox.pde.Jbi));
    }  
  }
    
  // mass matrix Ht*rho*cp*H
  if (feenox.pde.has_mass) {
    double rhocp = 0;
    if (thermal.rhocp.defined) {
      rhocp = thermal.rhocp.eval(&thermal.rhocp, x, material);
    } else if (thermal.kappa.defined) {
      rhocp = k / thermal.kappa.eval(&thermal.kappa, x, material);
    } else if (thermal.rho.defined && thermal.cp.defined) {
      rhocp = thermal.rho.eval(&thermal.rho, x, material) * thermal.cp.eval(&thermal.cp, x, material);
    } else {
      // this should have been already checked
      feenox_push_error_message("no heat capacity found");
      return FEENOX_ERROR;
    }
    feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, w * rhocp, e->H[v], e->H[v], 1.0, feenox.pde.Mi));
  }
  

#endif
  
  return FEENOX_OK;
  
}
