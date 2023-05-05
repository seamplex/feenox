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


int feenox_problem_build_allocate_aux_neutron_sn(unsigned int n_nodes) {
  
  neutron_sn.n_nodes = n_nodes;
  int dofs = neutron_sn.groups * neutron_sn.directions;
  int size = neutron_sn.n_nodes * neutron_sn.groups * neutron_sn.directions;
    
  if (neutron_sn.Ki != NULL) {
    gsl_matrix_free(neutron_sn.Ki);
    gsl_matrix_free(neutron_sn.Ai);
    gsl_matrix_free(neutron_sn.Xi);
    if (neutron_sn.has_sources) {
      gsl_vector_free(neutron_sn.Si);
    }
    
    
    gsl_matrix_free(neutron_sn.P);
    gsl_matrix_free(neutron_sn.OMEGAB);
    gsl_matrix_free(neutron_sn.AH);
    gsl_matrix_free(neutron_sn.Xi);
    if (neutron_sn.has_fission) {
      gsl_matrix_free(neutron_sn.XH);
    }
  }
    
  feenox_check_alloc(neutron_sn.Ki = gsl_matrix_calloc(size, size));
  feenox_check_alloc(neutron_sn.Ai = gsl_matrix_calloc(size, size));
  feenox_check_alloc(neutron_sn.Xi = gsl_matrix_calloc(size, size));
  if (neutron_sn.has_sources) {
    feenox_check_alloc(neutron_sn.Si = gsl_vector_calloc(size));
  }
  
  feenox_check_alloc(neutron_sn.P = gsl_matrix_calloc(dofs, size));
  feenox_check_alloc(neutron_sn.OMEGAB = gsl_matrix_calloc(dofs, size));
  feenox_check_alloc(neutron_sn.AH = gsl_matrix_calloc(dofs, size));
  if (neutron_sn.has_fission) {
    feenox_check_alloc(neutron_sn.XH = gsl_matrix_calloc(dofs, size));
  }
 
  return FEENOX_OK;
}

int feenox_problem_build_volumetric_gauss_point_neutron_sn(element_t *e, unsigned int v) {

#ifdef HAVE_PETSC

  feenox_call(feenox_mesh_compute_wHB_at_gauss(e, v));
  double *x = feenox_mesh_compute_x_if_needed(e, v, neutron_sn.space_XS);
  material_t *material = feenox_mesh_get_material(e);
  
  // initialize to zero
  gsl_matrix_set_zero(neutron_sn.removal);
  if (neutron_sn.has_fission) {
    gsl_matrix_set_zero(neutron_sn.nufission);
  }
  if (neutron_sn.has_sources) {
    gsl_vector_set_zero(neutron_sn.src);
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
       
  for (unsigned int n = 0; n < neutron_sn.directions; n++) {
    for (unsigned int g = 0; g < neutron_sn.groups; g++) {
      // independent sources
      int diag = dof_index(n,g);
      gsl_vector_set(neutron_sn.src, diag, neutron_sn.S[g].value);
        
      // scattering and fission
      for (unsigned int g_prime = 0; g_prime < neutron_sn.groups; g_prime++) {
        for (unsigned int n_prime = 0; n_prime < neutron_sn.directions; n_prime++) {
          // scattering
          double xi = -neutron_sn.w[n_prime] * neutron_sn.Sigma_s0[g_prime][g].value;
          // anisotropic scattering l = 1
          if (neutron_sn.Sigma_s1[g_prime][g].defined) {
            xi -= neutron_sn.w[n_prime] * neutron_sn.Sigma_s1[g_prime][g].value * 3.0 * feenox_mesh_dot(neutron_sn.Omega[n], neutron_sn.Omega[n_prime]);
          }
          gsl_matrix_set(neutron_sn.removal, diag, dof_index(n_prime,g_prime), xi);

          // fision
          if (neutron_sn.has_fission) {
            gsl_matrix_set(neutron_sn.nufission, diag, dof_index(n_prime,g_prime), +neutron_sn.w[n_prime] * neutron_sn.chi[g] * neutron_sn.nu_Sigma_f[g_prime].value);
          }
        }
      }

      // absorption
      double xi = gsl_matrix_get(neutron_sn.removal, diag, diag);
      if (neutron_sn.Sigma_t[g].defined) {
        xi += neutron_sn.Sigma_t[g].value;
      } else {
        xi += neutron_sn.Sigma_a[g].value;
        for (unsigned int g_prime = 0; g_prime < neutron_sn.groups; g_prime++) {
          xi += neutron_sn.Sigma_s0[g][g_prime].value;
        }
      }
      gsl_matrix_set(neutron_sn.removal, diag, diag, xi);
    }
  }

  // TODO: store current number of nodes in feenox.pde and call the method automatically?
  // this should be done outside the gauss loop!
  if (neutron_sn.n_nodes != e->type->nodes) {
    feenox_call(feenox_problem_build_allocate_aux_neutron_sn(e->type->nodes));
  }
  
  // initialize Ki Ai Xi Si <- 0
  gsl_matrix_set_zero(neutron_sn.Ki);
  gsl_matrix_set_zero(neutron_sn.Ai);
  if (neutron_sn.has_fission) {
    gsl_matrix_set_zero(neutron_sn.Xi);
  }
  if (neutron_sn.has_sources) {
    gsl_vector_set_zero(neutron_sn.Si);
  }

  // petrov stabilization
  int NG = neutron_sn.directions * neutron_sn.groups;
//  double tau = 0.5 * feenox_var_value(neutron_sn.sn_alpha) * neutron_sn.supg_dimension_factor * e->type->volume(e) / e->type->area(e);
  double tau = feenox_var_value(neutron_sn.sn_alpha) * e->type->size(e);
  
  feenox_call(gsl_matrix_memcpy(neutron_sn.P, e->H[v]));
  for (unsigned int j = 0; j < neutron_sn.n_nodes; j++) {
    int NGj = NG*j;
    for (unsigned int n = 0; n < neutron_sn.directions; n++) {
      for (unsigned int m = 0; m < feenox.pde.dim; m++) {
        double xi = tau * neutron_sn.Omega[n][m] * gsl_matrix_get(e->dhdx[v], j, m);
        for (unsigned int g = 0; g < neutron_sn.groups; g++) {
          int diag = dof_index(n,g);
          gsl_matrix_add_to_element(neutron_sn.P, diag, NGj + diag, xi);
        }
      }
    }
  }

  // petrov-stabilized leakage term
  feenox_call(gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, neutron_sn.OMEGA, e->B[v], 0.0, neutron_sn.OMEGAB));
  feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, e->w[v], neutron_sn.P, neutron_sn.OMEGAB, 1.0, neutron_sn.Ki));
//  feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, e->w[v], e->H[v], neutron_sn.OMEGAB, 1.0, neutron_sn.Ki));

  // petrov-stabilized revmoval term
  feenox_call(gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, neutron_sn.removal, e->H[v], 0.0, neutron_sn.AH));
  feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, e->w[v], neutron_sn.P, neutron_sn.AH, 1.0, neutron_sn.Ai));
//  feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, e->w[v], e->H[v], neutron_sn.AH, 1.0, neutron_sn.Ai));
  
  // petrov-stabilized fission term
  if (neutron_sn.has_fission) {
    feenox_call(gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, neutron_sn.nufission, e->H[v], 0.0, neutron_sn.XH));
    feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, e->w[v], neutron_sn.P, neutron_sn.XH, 1.0, neutron_sn.Xi));
//    feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, e->w[v], e->H[v], neutron_sn.XH, 1.0, neutron_sn.Xi));
  }
  // petrov-stabilized source term
  if (neutron_sn.has_sources) {
    feenox_call(gsl_blas_dgemv(CblasTrans, e->w[v], neutron_sn.P, neutron_sn.src, 1.0, neutron_sn.Si));
//    feenox_call(gsl_blas_dgemv(CblasTrans, e->w[v], e->H[v],             neutron_sn.src, 1.0, feenox.pde.bi));
  }
  
  // for source-driven problems
  //   K = Ki + Ai - Xi
  // for criticallity problems
  //   K = Ki + Ai
  //   M = Xi
  gsl_matrix_add(neutron_sn.Ki, neutron_sn.Ai);
  if (neutron_sn.has_fission) {
    if (neutron_sn.has_sources) {
      gsl_matrix_scale(neutron_sn.Xi, -1.0);
      gsl_matrix_add(neutron_sn.Ki, neutron_sn.Xi);
    } else {
      gsl_matrix_add(feenox.pde.Mi, neutron_sn.Xi);
    }  
  }

  if (neutron_sn.has_sources) {
      gsl_vector_add(feenox.pde.bi, neutron_sn.Si);
  }
  
  gsl_matrix_add(feenox.pde.Ki, neutron_sn.Ki);
#endif
  
  return FEENOX_OK;
  
}

