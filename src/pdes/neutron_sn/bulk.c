/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for neutron transport FEM: bulk elements
 *
 *  Copyright (C) 2023 jeremy theler
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
#include "neutron_sn.h"


int feenox_problem_build_allocate_aux_neutron_sn(unsigned int J) {
  
  neutron_sn.n_nodes = J;
  int G = neutron_sn.groups;
  int M = neutron_sn.directions;
  
  int MG = M*G;
  int JMG = J * MG;
    
  if (neutron_sn.Li != NULL) {
    gsl_matrix_free(neutron_sn.Li);
    gsl_matrix_free(neutron_sn.Ai);
    if (neutron_sn.has_fission) {
      gsl_matrix_free(neutron_sn.Fi);
    }
    if (neutron_sn.has_sources) {
      gsl_vector_free(neutron_sn.Si);
    }
    
    gsl_matrix_free(neutron_sn.P);
    gsl_matrix_free(neutron_sn.DB);
    gsl_matrix_free(neutron_sn.AH);
    if (neutron_sn.has_fission) {
      gsl_matrix_free(neutron_sn.Fi);
      gsl_matrix_free(neutron_sn.XH);
    }
  }
    
  feenox_check_alloc(neutron_sn.Li = gsl_matrix_calloc(JMG, JMG));
  feenox_check_alloc(neutron_sn.Ai = gsl_matrix_calloc(JMG, JMG));
  if (neutron_sn.has_fission) {
    feenox_check_alloc(neutron_sn.Fi = gsl_matrix_calloc(JMG, JMG));
  }
  if (neutron_sn.has_sources) {
    feenox_check_alloc(neutron_sn.Si = gsl_vector_calloc(JMG));
  }
  
  feenox_check_alloc(neutron_sn.P = gsl_matrix_calloc(MG, JMG));
  feenox_check_alloc(neutron_sn.DB = gsl_matrix_calloc(MG, JMG));
  feenox_check_alloc(neutron_sn.AH = gsl_matrix_calloc(MG, JMG));
  if (neutron_sn.has_fission) {
    feenox_check_alloc(neutron_sn.XH = gsl_matrix_calloc(MG, JMG));
  }
 
  return FEENOX_OK;
}

int feenox_problem_build_volumetric_gauss_point_neutron_sn(element_t *e, unsigned int q) {

#ifdef HAVE_PETSC

  feenox_call(feenox_mesh_compute_wHB_at_gauss(e, q));
  double *x = feenox_mesh_compute_x_at_gauss_if_needed(e, q, neutron_sn.space_XS);
  material_t *material = feenox_mesh_get_material(e);
  
  // initialize to zero
  gsl_matrix_set_zero(neutron_sn.R);
  if (neutron_sn.has_fission) {
    gsl_matrix_set_zero(neutron_sn.X);
  }
  if (neutron_sn.has_sources) {
    gsl_vector_set_zero(neutron_sn.s);
  }

  // TODO: if XS are uniform then compute only once these cached properties
  for (int g = 0; g < neutron_sn.groups; g++) {
    // independent sources
    if (neutron_sn.S[g].defined) {
      neutron_sn.S[g].eval(&neutron_sn.S[g], x, material);
    }
    
    // fission
    if (neutron_sn.nu_Sigma_f[g].defined) {
      neutron_sn.nu_Sigma_f[g].eval(&neutron_sn.nu_Sigma_f[g], x, material);
    }
    
    // scattering
    for (int g_prime = 0; g_prime < neutron_sn.groups; g_prime++) {
      if (neutron_sn.Sigma_s0[g_prime][g].defined) {
        neutron_sn.Sigma_s0[g_prime][g].eval(&neutron_sn.Sigma_s0[g_prime][g], x, material);
      }
      if (neutron_sn.Sigma_s1[g_prime][g].defined) {
        neutron_sn.Sigma_s1[g_prime][g].eval(&neutron_sn.Sigma_s1[g_prime][g], x, material);
      }
    }
    
    // absorption
    if (neutron_sn.Sigma_t[g].defined) {
      neutron_sn.Sigma_t[g].eval(&neutron_sn.Sigma_t[g], x, material);
    }
    if (neutron_sn.Sigma_a[g].defined) {
      neutron_sn.Sigma_a[g].eval(&neutron_sn.Sigma_a[g], x, material);
    }
  }
       
  for (unsigned int m = 0; m < neutron_sn.directions; m++) {
    for (unsigned int g = 0; g < neutron_sn.groups; g++) {
      // independent sources
      int diag = dof_index(m,g);
      gsl_vector_set(neutron_sn.s, diag, neutron_sn.S[g].value);
        
      // scattering and fission
      for (unsigned int g_prime = 0; g_prime < neutron_sn.groups; g_prime++) {
        for (unsigned int m_prime = 0; m_prime < neutron_sn.directions; m_prime++) {
          // scattering
          double s = -neutron_sn.w[m_prime] * neutron_sn.Sigma_s0[g_prime][g].value;
          // anisotropic scattering l = 1
          if (neutron_sn.Sigma_s1[g_prime][g].defined) {
            s -= neutron_sn.w[m_prime] * neutron_sn.Sigma_s1[g_prime][g].value * 3.0 * feenox_mesh_dot(neutron_sn.Omega[m], neutron_sn.Omega[m_prime]);
          }
          gsl_matrix_set(neutron_sn.R, diag, dof_index(m_prime,g_prime), s);

          // fision
          if (neutron_sn.has_fission) {
            gsl_matrix_set(neutron_sn.X, diag, dof_index(m_prime,g_prime), +neutron_sn.w[m_prime] * neutron_sn.chi[g] * neutron_sn.nu_Sigma_f[g_prime].value);
          }
        }
      }

      // absorption
      double xi = gsl_matrix_get(neutron_sn.R, diag, diag);
      if (neutron_sn.Sigma_t[g].defined) {
        xi += neutron_sn.Sigma_t[g].value;
      } else {
        xi += neutron_sn.Sigma_a[g].value;
        for (unsigned int g_prime = 0; g_prime < neutron_sn.groups; g_prime++) {
          xi += neutron_sn.Sigma_s0[g][g_prime].value;
        }
      }
      gsl_matrix_set(neutron_sn.R, diag, diag, xi);
    }
  }

  // TODO: store current number of nodes in feenox.pde and call the method automatically?
  // this should be done outside the gauss loop!
  if (neutron_sn.n_nodes != e->type->nodes) {
    feenox_call(feenox_problem_build_allocate_aux_neutron_sn(e->type->nodes));
  }
  
  // initialize Ki Ai Xi Si <- 0
  gsl_matrix_set_zero(neutron_sn.Li);
  gsl_matrix_set_zero(neutron_sn.Ai);
  if (neutron_sn.has_fission) {
    gsl_matrix_set_zero(neutron_sn.Fi);
  }
  if (neutron_sn.has_sources) {
    gsl_vector_set_zero(neutron_sn.Si);
  }

  // petrov stabilization
  int MG = neutron_sn.directions * neutron_sn.groups;
//  double tau = 0.5 * feenox_var_value(neutron_sn.sn_alpha) * neutron_sn.supg_dimension_factor * e->type->volume(e) / e->type->area(e);
  double tau = feenox_var_value(neutron_sn.sn_alpha) * e->type->size(e);
  
  feenox_call(gsl_matrix_memcpy(neutron_sn.P, e->type->H_Gc[q]));
  for (unsigned int j = 0; j < neutron_sn.n_nodes; j++) {
    int MGj = MG*j;
    for (unsigned int n = 0; n < neutron_sn.directions; n++) {
      for (unsigned int m = 0; m < feenox.pde.dim; m++) {
        double xi = tau * neutron_sn.Omega[n][m] * gsl_matrix_get(e->B[q], m, j);
        for (unsigned int g = 0; g < neutron_sn.groups; g++) {
          int diag = dof_index(n,g);
          gsl_matrix_add_to_element(neutron_sn.P, diag, MGj + diag, xi);
        }
      }
    }
  }

  // petrov-stabilized leakage term
  feenox_call(gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, neutron_sn.D, e->B_G[q], 0.0, neutron_sn.DB));
  feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, e->w[q], neutron_sn.P, neutron_sn.DB, 1.0, neutron_sn.Li));

  // petrov-stabilized removal term
  feenox_call(gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, neutron_sn.R, e->type->H_Gc[q], 0.0, neutron_sn.AH));
  feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, e->w[q], neutron_sn.P, neutron_sn.AH, 1.0, neutron_sn.Ai));
  
  // petrov-stabilized fission term
  if (neutron_sn.has_fission) {
    feenox_call(gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, neutron_sn.X, e->type->H_Gc[q], 0.0, neutron_sn.XH));
    feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, e->w[q], neutron_sn.P, neutron_sn.XH, 1.0, neutron_sn.Fi));
  }
  // petrov-stabilized source term
  if (neutron_sn.has_sources) {
    feenox_call(gsl_blas_dgemv(CblasTrans, e->w[q], neutron_sn.P, neutron_sn.s, 1.0, neutron_sn.Si));
  }
  
  // for source-driven problems
  //   K = Ki + Ai - Xi
  // for criticallity problems
  //   K = Ki + Ai
  //   M = Xi
  gsl_matrix_add(neutron_sn.Li, neutron_sn.Ai);
  if (neutron_sn.has_fission) {
    if (neutron_sn.has_sources) {
      gsl_matrix_scale(neutron_sn.Fi, -1.0);
      gsl_matrix_add(neutron_sn.Li, neutron_sn.Fi);
    } else {
      gsl_matrix_add(feenox.pde.Mi, neutron_sn.Fi);
    }  
  }

  if (neutron_sn.has_sources) {
      gsl_vector_add(feenox.pde.bi, neutron_sn.Si);
  }
  
  gsl_matrix_add(feenox.pde.Ki, neutron_sn.Li);
#endif
  
  return FEENOX_OK;
  
}

