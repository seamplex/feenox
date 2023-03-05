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
#include "neutron_transport.h"


int feenox_problem_build_allocate_aux_neutron_transport(unsigned int n_nodes) {
  
  neutron_transport.n_nodes = n_nodes;
  int dofs = neutron_transport.groups * neutron_transport.directions;
  int size = neutron_transport.n_nodes * neutron_transport.groups * neutron_transport.directions;
    
  if (neutron_transport.Ki != NULL) {
    gsl_matrix_free(neutron_transport.Ki);
    gsl_matrix_free(neutron_transport.Ai);
    gsl_matrix_free(neutron_transport.Xi);
    if (neutron_transport.has_sources) {
      gsl_vector_free(neutron_transport.Si);
    }
    
    
    gsl_matrix_free(neutron_transport.P);
    gsl_matrix_free(neutron_transport.OMEGAB);
    gsl_matrix_free(neutron_transport.AH);
    gsl_matrix_free(neutron_transport.Xi);
    if (neutron_transport.has_fission) {
      gsl_matrix_free(neutron_transport.XH);
    }
  }
    
  feenox_check_alloc(neutron_transport.Ki = gsl_matrix_calloc(size, size));
  feenox_check_alloc(neutron_transport.Ai = gsl_matrix_calloc(size, size));
  feenox_check_alloc(neutron_transport.Xi = gsl_matrix_calloc(size, size));
  if (neutron_transport.has_sources) {
    feenox_check_alloc(neutron_transport.Si = gsl_vector_calloc(size));
  }
  
  feenox_check_alloc(neutron_transport.P = gsl_matrix_calloc(dofs, size));
  feenox_check_alloc(neutron_transport.OMEGAB = gsl_matrix_calloc(dofs, size));
  feenox_check_alloc(neutron_transport.AH = gsl_matrix_calloc(dofs, size));
  if (neutron_transport.has_fission) {
    feenox_check_alloc(neutron_transport.XH = gsl_matrix_calloc(dofs, size));
  }
 
  return FEENOX_OK;
}

int feenox_problem_build_volumetric_gauss_point_neutron_transport(element_t *e, unsigned int v) {

#ifdef HAVE_PETSC

  feenox_call(feenox_mesh_compute_wHB_at_gauss(e, v));
  double *x = feenox_mesh_compute_x_if_needed(e, v, neutron_transport.space_XS);
  material_t *material = feenox_mesh_get_material(e);
  
  // initialize to zero
  gsl_matrix_set_zero(neutron_transport.removal);
  if (neutron_transport.has_fission) {
    gsl_matrix_set_zero(neutron_transport.nufission);
  }
  if (neutron_transport.has_sources) {
    gsl_vector_set_zero(neutron_transport.src);
  }

  // TODO: if XS are uniform then compute only once these cached properties
  for (int g = 0; g < neutron_transport.groups; g++) {
    // independent sources
    if (neutron_transport.S[g].defined) {
      neutron_transport.S[g].eval(&neutron_transport.S[g], x, material);
    }
    
    // fission
    if (neutron_transport.nu_Sigma_f[g].defined) {
      neutron_transport.nu_Sigma_f[g].eval(&neutron_transport.nu_Sigma_f[g], x, material);
    }
    
    // scattering
    for (int g_prime = 0; g_prime < neutron_transport.groups; g_prime++) {
      if (neutron_transport.Sigma_s0[g_prime][g].defined) {
        neutron_transport.Sigma_s0[g_prime][g].eval(&neutron_transport.Sigma_s0[g_prime][g], x, material);
      }
      if (neutron_transport.Sigma_s1[g_prime][g].defined) {
        neutron_transport.Sigma_s1[g_prime][g].eval(&neutron_transport.Sigma_s1[g_prime][g], x, material);
      }
    }
    
    // absorption
    if (neutron_transport.Sigma_t[g].defined) {
      neutron_transport.Sigma_t[g].eval(&neutron_transport.Sigma_t[g], x, material);
    }
    if (neutron_transport.Sigma_a[g].defined) {
      neutron_transport.Sigma_a[g].eval(&neutron_transport.Sigma_a[g], x, material);
    }
  }
       
  for (unsigned int n = 0; n < neutron_transport.directions; n++) {
    for (unsigned int g = 0; g < neutron_transport.groups; g++) {
      // independent sources
      int diag = dof_index(n,g);
      gsl_vector_set(neutron_transport.src, diag, neutron_transport.S[g].value);
        
      // scattering and fission
      for (unsigned int g_prime = 0; g_prime < neutron_transport.groups; g_prime++) {
        for (unsigned int n_prime = 0; n_prime < neutron_transport.directions; n_prime++) {
          // scattering
          double xi = -neutron_transport.w[n_prime] * neutron_transport.Sigma_s0[g_prime][g].value;
          // anisotropic scattering l = 1
          if (neutron_transport.Sigma_s1[g_prime][g].defined) {
            xi -= neutron_transport.w[n_prime] * neutron_transport.Sigma_s1[g_prime][g].value * 3.0 * feenox_mesh_dot(neutron_transport.Omega[n], neutron_transport.Omega[n_prime]);
          }
          gsl_matrix_set(neutron_transport.removal, diag, dof_index(n_prime,g_prime), xi);

          // fision
          if (neutron_transport.has_fission) {
            gsl_matrix_set(neutron_transport.nufission, diag, dof_index(n_prime,g_prime), +neutron_transport.w[n_prime] * neutron_transport.chi[g] * neutron_transport.nu_Sigma_f[g_prime].value);
          }
        }
      }

      // absorption
      double xi = gsl_matrix_get(neutron_transport.removal, diag, diag);
      if (neutron_transport.Sigma_t[g].defined) {
        xi += neutron_transport.Sigma_t[g].value;
      } else {
        xi += neutron_transport.Sigma_a[g].value;
        for (unsigned int g_prime = 0; g_prime < neutron_transport.groups; g_prime++) {
          xi += neutron_transport.Sigma_s0[g][g_prime].value;
        }
      }
      gsl_matrix_set(neutron_transport.removal, diag, diag, xi);
    }
  }

  // TODO: store current number of nodes in feenox.pde and call the method automatically?
  // this should be done outside the gauss loop!
  if (neutron_transport.n_nodes != e->type->nodes) {
    feenox_call(feenox_problem_build_allocate_aux_neutron_transport(e->type->nodes));
  }
  
  // initialize Ki Ai Xi Si <- 0
  gsl_matrix_set_zero(neutron_transport.Ki);
  gsl_matrix_set_zero(neutron_transport.Ai);
  if (neutron_transport.has_fission) {
    gsl_matrix_set_zero(neutron_transport.Xi);
  }
  if (neutron_transport.has_sources) {
    gsl_vector_set_zero(neutron_transport.Si);
  }

  // petrov stabilization
  double tau = feenox_var_value(neutron_transport.sn_alpha) * 0.5 * gsl_hypot3(e->node[1]->x[0]-e->node[0]->x[0],
                                                                               e->node[1]->x[1]-e->node[0]->x[1],
                                                                               e->node[1]->x[2]-e->node[0]->x[2]);
  for (unsigned int j = 0; j < neutron_transport.n_nodes; j++) {
    double xi = e->type->gauss[feenox.pde.mesh->integration].h[v][j];
    for (unsigned int n = 0; n < neutron_transport.directions; n++) {
      for (unsigned int g = 0; g < neutron_transport.groups; g++) {
        // parte base, igual a las h
        int diag = dof_index(n,g);
        gsl_matrix_set(neutron_transport.P, diag, neutron_transport.directions * neutron_transport.groups * j + diag, xi);
        // correccion
        for (unsigned int m = 0; m < feenox.pde.dim; m++) {
          gsl_matrix_add_to_element(neutron_transport.P, diag, neutron_transport.directions * neutron_transport.groups * j + diag, 
                             tau * neutron_transport.Omega[n][m] * gsl_matrix_get(e->dhdx[v], j, m));
        }
      }
    }
  }

  // petrov-stabilized leakage term
  feenox_call(gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, neutron_transport.OMEGA, e->B[v], 0.0, neutron_transport.OMEGAB));
  feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, e->w[v], neutron_transport.P, neutron_transport.OMEGAB, 1.0, neutron_transport.Ki));

  // petrov-stabilized revmoval term
  feenox_call(gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, neutron_transport.removal, e->H[v], 0.0, neutron_transport.AH));
  feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, e->w[v], neutron_transport.P, neutron_transport.AH, 1.0, neutron_transport.Ai));
  
  // petrov-stabilized fission term
  if (neutron_transport.has_fission) {
    feenox_call(gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1, neutron_transport.nufission, e->H[v], 0.0, neutron_transport.XH));
    feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, e->w[v], neutron_transport.P, neutron_transport.XH, 1.0, neutron_transport.Xi));
  }
  // petrov-stabilized source term
  if (neutron_transport.has_sources) {
    feenox_call(gsl_blas_dgemv(CblasTrans, e->w[v], neutron_transport.P, neutron_transport.src, 1, neutron_transport.Si));
  }
  
  // for source-driven problems
  //   K = Ki + Ai - Xi
  // for criticallity problems
  //   K = Ki + Ai
  //   M = Xi
  gsl_matrix_add(neutron_transport.Ki, neutron_transport.Ai);
  if (neutron_transport.has_fission) {
    if (neutron_transport.has_sources) {
      gsl_matrix_scale(neutron_transport.Xi, -1.0);
      gsl_matrix_add(neutron_transport.Ki, neutron_transport.Xi);
    } else {
      gsl_matrix_add(feenox.pde.Mi, neutron_transport.Xi);
    }  
  }

  if (neutron_transport.has_sources) {
      gsl_vector_add(feenox.pde.bi, neutron_transport.Si);
  }
  
  gsl_matrix_add(feenox.pde.Ki, neutron_transport.Ki);
#endif
  
  return FEENOX_OK;
  
}

