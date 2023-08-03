/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for neutron diffusion FEM: bulk elements
 *
 *  Copyright (C) 2021--2023 jeremy theler
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
#include "neutron_diffusion.h"

int feenox_problem_build_allocate_aux_neutron_diffusion(unsigned int J) {
  
  neutron_diffusion.n_nodes = J;
  int G = neutron_diffusion.groups;
  int D = feenox.pde.dim;
  int GJ = G*J;
    
  if (neutron_diffusion.Li != NULL) {
    gsl_matrix_free(neutron_diffusion.Li);
    gsl_matrix_free(neutron_diffusion.Ai);
    gsl_matrix_free(neutron_diffusion.DB);
    gsl_matrix_free(neutron_diffusion.AH);
    if (neutron_diffusion.has_fission) {
      gsl_matrix_free(neutron_diffusion.Fi);
      gsl_matrix_free(neutron_diffusion.FX);
    }
  }
    
  feenox_check_alloc(neutron_diffusion.Li = gsl_matrix_calloc(GJ, GJ));
  feenox_check_alloc(neutron_diffusion.Ai = gsl_matrix_calloc(GJ, GJ));
  feenox_check_alloc(neutron_diffusion.DB = gsl_matrix_calloc(G*D, G*J));
  feenox_check_alloc(neutron_diffusion.AH = gsl_matrix_calloc(G, GJ));
  if (neutron_diffusion.has_fission) {
    feenox_check_alloc(neutron_diffusion.Fi = gsl_matrix_calloc(GJ, GJ));
    feenox_check_alloc(neutron_diffusion.FX = gsl_matrix_calloc(G, GJ));
  }

  return FEENOX_OK;
}



int feenox_problem_build_volumetric_gauss_point_neutron_diffusion(element_t *e, unsigned int q) {

#ifdef HAVE_PETSC
  
  double *x = feenox_fem_compute_x_at_gauss_if_needed(e, q, neutron_diffusion.space_XS);
  material_t *material = feenox_fem_get_material(e);
  
  gsl_matrix_set_zero(neutron_diffusion.D_G);
  gsl_matrix_set_zero(neutron_diffusion.R);
  if (neutron_diffusion.has_fission) {
    gsl_matrix_set_zero(neutron_diffusion.X);
  }
  
  for (int g = 0; g < neutron_diffusion.groups; g++) {
    // independent sources
    if (neutron_diffusion.has_sources) {
      // TODO: macro to make it nicer
      gsl_vector_set(neutron_diffusion.s, g, neutron_diffusion.S[g].eval(&neutron_diffusion.S[g], x, material));
    }
    
    // scattering and fission
    for (int g_prime = 0; g_prime < neutron_diffusion.groups; g_prime++) {
      gsl_matrix_set(neutron_diffusion.R, g, g_prime, -neutron_diffusion.Sigma_s[g_prime][g].eval(&neutron_diffusion.Sigma_s[g_prime][g], x, material));
    
      if (neutron_diffusion.has_fission) {
        gsl_matrix_set(neutron_diffusion.X, g, g_prime, neutron_diffusion.chi[g]*neutron_diffusion.nu_Sigma_f[g_prime].eval(&neutron_diffusion.nu_Sigma_f[g_prime], x, material));
      }  
    }
    
    // absorption
    double xi = gsl_matrix_get(neutron_diffusion.R, g, g);
    if (neutron_diffusion.Sigma_t[g].defined) {
      xi += neutron_diffusion.Sigma_t[g].eval(&neutron_diffusion.Sigma_t[g], x, material);
    } else {
      xi += neutron_diffusion.Sigma_a[g].eval(&neutron_diffusion.Sigma_a[g], x, material);
      for (int g_prime = 0; g_prime < neutron_diffusion.groups; g_prime++) {
        xi += neutron_diffusion.Sigma_s[g][g_prime].eval(&neutron_diffusion.Sigma_s[g][g_prime], x, material);
      }
    }
    gsl_matrix_set(neutron_diffusion.R, g, g, xi);
    
    // leaks
    for (int m = 0; m < feenox.pde.dim; m++) {
      if (neutron_diffusion.D[g].defined) {
        xi = neutron_diffusion.D[g].eval(&neutron_diffusion.D[g], x, material);
      } else if (neutron_diffusion.Sigma_t[g].defined) {
        xi = 1.0/(3.0*neutron_diffusion.Sigma_t[g].eval(&neutron_diffusion.Sigma_t[g], x, material));
      } else {
        feenox_push_error_message("neither D nor Sigma_t given for group %d", g);
        return FEENOX_ERROR;
      }

      unsigned int index = m*neutron_diffusion.groups + g;
      gsl_matrix_set(neutron_diffusion.D_G, index, index, xi);
    }
  }

  if (neutron_diffusion.n_nodes != e->type->nodes) {
    feenox_call(feenox_problem_build_allocate_aux_neutron_diffusion(e->type->nodes));
  }

  gsl_matrix_set_zero(neutron_diffusion.Li);
  gsl_matrix_set_zero(neutron_diffusion.Ai);
  if (neutron_diffusion.has_fission) {
    gsl_matrix_set_zero(neutron_diffusion.Fi);
  }
  
  // elemental stiffness for the diffusion term B'*D*B
  // TODO: convenience call for A'*B*A? that'd need an intermediate alloc
  feenox_call(gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, neutron_diffusion.D_G, e->B_G[q], 0.0, neutron_diffusion.DB));
  feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, e->w[q], e->B_G[q], neutron_diffusion.DB, 1.0, neutron_diffusion.Li));

  // elemental scattering H'*A*H
  // TODO: can we mix Ai and Ki?
  feenox_call(gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, neutron_diffusion.R, e->type->H_Gc[q], 0.0, neutron_diffusion.AH));
  feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, e->w[q],  e->type->H_Gc[q], neutron_diffusion.AH, 1.0, neutron_diffusion.Ai));
  
  // elemental fission matrix
  if (neutron_diffusion.has_fission) {
    feenox_call(gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, neutron_diffusion.X, e->type->H_Gc[q], 0, neutron_diffusion.FX));
    feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, e->w[q], e->type->H_Gc[q], neutron_diffusion.FX, 1.0, neutron_diffusion.Fi));
  }
  
  
  if (neutron_diffusion.has_sources) {
    feenox_call(gsl_blas_dgemv(CblasTrans, e->w[q], e->type->H_Gc[q], neutron_diffusion.s, 1.0, feenox.fem.bi));
  }
  
  // for source-driven problems
  //   Ki = Li + Ai - Xi
  // for criticallity problems
  //   Ki = Li + Ai
  //   Mi = Xi
  gsl_matrix_add(neutron_diffusion.Li, neutron_diffusion.Ai);
  if (neutron_diffusion.has_fission) {
    if (neutron_diffusion.has_sources) {
      gsl_matrix_scale(neutron_diffusion.Fi, -1.0);
      gsl_matrix_add(neutron_diffusion.Li, neutron_diffusion.Fi);
    } else {
      gsl_matrix_add(feenox.fem.Mi, neutron_diffusion.Fi);
    }  
  }
  gsl_matrix_add(feenox.fem.Ki, neutron_diffusion.Li);
  
 
#endif
  
  return FEENOX_OK;
  
}

