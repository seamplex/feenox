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

int feenox_problem_build_volumetric_neutron_sn(element_t *e) {

  if (neutron_sn.space_XS == 0) {
    feenox_call(feenox_problem_neutron_sn_eval_XS(feenox_fem_get_material(e), NULL));
  }

  return FEENOX_OK;
}

int feenox_problem_neutron_sn_eval_XS(material_t *material, double *x) {
  
  // initialize to zero
  gsl_matrix_set_zero(neutron_sn.R);
  if (neutron_sn.has_fission) {
    gsl_matrix_set_zero(neutron_sn.X);
  }
  if (neutron_sn.has_sources) {
    gsl_vector_set_zero(neutron_sn.s);
  }

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
      int diag = sn_dof_index(m,g);
      gsl_vector_set(neutron_sn.s, diag, neutron_sn.S[g].value);
        
      // scattering and fission
      for (unsigned int g_prime = 0; g_prime < neutron_sn.groups; g_prime++) {
        for (unsigned int m_prime = 0; m_prime < neutron_sn.directions; m_prime++) {
          // scattering
          double accum = -neutron_sn.w[m_prime] * neutron_sn.Sigma_s0[g_prime][g].value;
          // anisotropic scattering l = 1
          if (neutron_sn.Sigma_s1[g_prime][g].defined) {
            accum -= neutron_sn.w[m_prime] * neutron_sn.Sigma_s1[g_prime][g].value * 3.0 * feenox_mesh_dot(neutron_sn.Omega[m], neutron_sn.Omega[m_prime]);
          }
          gsl_matrix_set(neutron_sn.R, diag, sn_dof_index(m_prime,g_prime), accum);

          // fision
          if (neutron_sn.has_fission) {
            gsl_matrix_set(neutron_sn.X, diag, sn_dof_index(m_prime,g_prime), +neutron_sn.w[m_prime] * feenox_vector_get(neutron_sn.chi, g) * neutron_sn.nu_Sigma_f[g_prime].value);
          }
        }
      }

      // absorption
      double accum = gsl_matrix_get(neutron_sn.R, diag, diag);
      if (neutron_sn.Sigma_t[g].defined) {
        accum += neutron_sn.Sigma_t[g].value;
      } else {
        accum += neutron_sn.Sigma_a[g].value;
        for (unsigned int g_prime = 0; g_prime < neutron_sn.groups; g_prime++) {
          accum += neutron_sn.Sigma_s0[g][g_prime].value;
        }
      }
      gsl_matrix_set(neutron_sn.R, diag, diag, accum);
    }
  }
  
  return FEENOX_OK;
}

int feenox_problem_build_volumetric_gauss_point_neutron_sn(element_t *e, unsigned int q) {

#ifdef HAVE_PETSC

  if (neutron_sn.space_XS != 0) {
    double *x = feenox_fem_compute_x_at_gauss(e, q, feenox.pde.mesh->integration);
    feenox_call(feenox_problem_neutron_sn_eval_XS(feenox_fem_get_material(e), x));
  }
  
//  printf("build %ld %d\n", e->tag, q);
  // petrov stabilization matrix
  int MG = neutron_sn.directions * neutron_sn.groups;
  double tau = feenox_var_value(neutron_sn.sn_alpha) * e->type->size(e);
  
  gsl_matrix *H_G = feenox_fem_compute_H_Gc_at_gauss(e, q, feenox.pde.mesh->integration);
  gsl_matrix *B = feenox_fem_compute_B_at_gauss(e, q, feenox.pde.mesh->integration);
  feenox_call(gsl_matrix_memcpy(neutron_sn.P, H_G));
  for (unsigned int j = 0; j < neutron_sn.n_nodes; j++) {
    int MGj = MG*j;
    for (unsigned int m = 0; m < neutron_sn.directions; m++) {
      for (unsigned int d = 0; d < feenox.pde.dim; d++) {
        double value = tau * neutron_sn.Omega[m][d] * gsl_matrix_get(B, d, j);
        for (unsigned int g = 0; g < neutron_sn.groups; g++) {
          int diag = sn_dof_index(m,g);
          gsl_matrix_add_to_element(neutron_sn.P, diag, MGj + diag, value);
        }
      }
    }
  }

  // petrov-stabilized leakage term
  double wdet = feenox_fem_compute_w_det_at_gauss(e, q, feenox.pde.mesh->integration);
  gsl_matrix *B_G = feenox_fem_compute_B_G_at_gauss(e, q, feenox.pde.mesh->integration);
  feenox_call(feenox_blas_PtCB(neutron_sn.P, neutron_sn.D, B_G, neutron_sn.DB, wdet, neutron_sn.Li));

  // petrov-stabilized removal term
  feenox_call(feenox_blas_PtCB(neutron_sn.P, neutron_sn.R, H_G, neutron_sn.AH, wdet, neutron_sn.Ai));
  
  // petrov-stabilized fission term
  if (neutron_sn.has_fission) {
    feenox_call(feenox_blas_PtCB(neutron_sn.P, neutron_sn.X, H_G, neutron_sn.XH, wdet, neutron_sn.Fi));
  }
  
  // petrov-stabilized source term
  if (neutron_sn.has_sources) {
    feenox_call(feenox_blas_Atb(neutron_sn.P, neutron_sn.s, wdet, neutron_sn.Si));
  }
  
  // for source-driven problems
  //   K = Ki + Ai - Xi
  // for criticallity problems
  //   K = Ki + Ai
  //   M = Xi
  feenox_call(gsl_matrix_add(neutron_sn.Li, neutron_sn.Ai));
  if (neutron_sn.has_fission) {
    if (neutron_sn.has_sources) {
      feenox_call(gsl_matrix_scale(neutron_sn.Fi, -1.0));
      feenox_call(gsl_matrix_add(neutron_sn.Li, neutron_sn.Fi));
    } else {
      feenox_call(gsl_matrix_add(feenox.fem.Mi, neutron_sn.Fi));
    }  
  }

  if (neutron_sn.has_sources) {
    feenox_call(gsl_vector_add(feenox.fem.bi, neutron_sn.Si));
  }
  
  feenox_call(gsl_matrix_add(feenox.fem.Ki, neutron_sn.Li));
#endif
  
  return FEENOX_OK;
  
}

