/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for neutron diffusion FEM: bulk elements
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
#include "neutron_diffusion.h"
extern feenox_t feenox;
extern neutron_diffusion_t neutron_diffusion;

int feenox_problem_build_volumetric_gauss_point_neutron_diffusion(element_t *this, unsigned int v) {

#ifdef HAVE_PETSC
  
  feenox_call(feenox_mesh_compute_wHB_at_gauss(this, v));
  
  double *x = NULL;
  if (neutron_diffusion.space_XS) {
    feenox_call(feenox_mesh_compute_x_at_gauss(this, v, feenox.pde.mesh->integration));
    x = this->x[v];
  }
  
  
  material_t *material = NULL;
  if (this->physical_group != NULL && this->physical_group->material != NULL) {
    material = this->physical_group->material;
  }
  
  // TODO
  double *chi = calloc(feenox.pde.dofs, sizeof(double));
  chi[0] = 1;
  
  unsigned int J = this->type->nodes;
  gsl_matrix *D = gsl_matrix_calloc(feenox.pde.dofs * feenox.pde.dim, feenox.pde.dofs * feenox.pde.dim);
  gsl_matrix *A = gsl_matrix_calloc(feenox.pde.dofs, feenox.pde.dofs);
  gsl_matrix *X = gsl_matrix_calloc(feenox.pde.dofs, feenox.pde.dofs);
  gsl_vector *S = gsl_vector_calloc(feenox.pde.dofs);
  
  
  unsigned int g = 0;
  unsigned int g_prime = 0;
  for (g = 0; g < feenox.pde.dofs; g++) {
    // independent sources
    if (neutron_diffusion.has_sources) {
      // TODO: macro to make it nicer
      gsl_vector_set(S, g, neutron_diffusion.source[g].eval(&neutron_diffusion.source[g], x, material));
    }
    
    // scattering and fission
    for (g_prime = 0; g_prime < feenox.pde.dofs; g_prime++) {
      gsl_matrix_set(A, g, g_prime, -neutron_diffusion.sigma_s[g_prime][g].eval(&neutron_diffusion.sigma_s[g_prime][g], x, material));
    
      if (neutron_diffusion.has_fission) {
        gsl_matrix_set(X, g, g_prime, chi[g]*neutron_diffusion.nu_sigma_f[g_prime].eval(&neutron_diffusion.nu_sigma_f[g_prime], x, material));
      }  
    }
    
//    feenox_debug_print_gsl_matrix(A, stdout);
    
    // absorption
    double xi = gsl_matrix_get(A, g, g);
    if (neutron_diffusion.sigma_t[g].defined) {
      xi += neutron_diffusion.sigma_t[g].eval(&neutron_diffusion.sigma_t[g], x, material);
    } else {
      xi += neutron_diffusion.sigma_a[g].eval(&neutron_diffusion.sigma_a[g], x, material);
      for (g_prime = 0; g_prime < feenox.pde.dofs; g_prime++) {
        xi += neutron_diffusion.sigma_s[g][g_prime].eval(&neutron_diffusion.sigma_s[g][g_prime], x, material);
      }
    }
    gsl_matrix_set(A, g, g, xi);
//    printf("%ld %d\n", this->index, v);
//    feenox_debug_print_gsl_matrix(A, stdout);
    
    // leaks
    unsigned int m = 0;
    for (m = 0; m < feenox.pde.dim; m++) {
      if (neutron_diffusion.D[g].defined) {
        xi = neutron_diffusion.D[g].eval(&neutron_diffusion.D[g], x, material);
      } else if (neutron_diffusion.sigma_t[g].defined) {
        xi = 1.0/(3.0*neutron_diffusion.sigma_t[g].eval(&neutron_diffusion.sigma_t[g], x, material));
      } else {
        feenox_push_error_message("neither D nor sigma_t given for group %d", g);
        return FEENOX_ERROR;
      }

      unsigned int index = m*feenox.pde.dofs + g;
      gsl_matrix_set(D, index, index, xi);
    }
  }
  
  gsl_matrix *Ki = gsl_matrix_calloc(J*feenox.pde.dofs, J*feenox.pde.dofs);
  gsl_matrix *Ai = gsl_matrix_calloc(J*feenox.pde.dofs, J*feenox.pde.dofs);
  gsl_matrix *Xi = gsl_matrix_calloc(J*feenox.pde.dofs, J*feenox.pde.dofs);
  


  // elemental stiffness for the diffusion term B'*D*B
  gsl_matrix *DB = gsl_matrix_calloc(feenox.pde.dofs * feenox.pde.dim, feenox.pde.dofs * J);
  feenox_call(gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, D, this->B[v], 0.0, DB));
  feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, this->w[v], this->B[v], DB, 1.0, Ki));
//   printf("Ki\n");
//   feenox_debug_print_gsl_matrix(Ki, stdout);

  // elemental scattering H'*A*H
  // TODO: can we mix Ai and Ki?
 
  gsl_matrix *AH = gsl_matrix_calloc(feenox.pde.dofs, feenox.pde.dofs * J);
  feenox_call(gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, A, this->H[v], 0.0, AH));
  feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, this->w[v], this->H[v], AH, 1.0, Ai));
//   printf("Ai\n");
//   feenox_debug_print_gsl_matrix(Ai, stdout);
  
  // elemental fission matrix
  gsl_matrix *XH = NULL;
  if (neutron_diffusion.has_fission) {
    XH = gsl_matrix_calloc(feenox.pde.dofs, feenox.pde.dofs * J);
    feenox_call(gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, X, this->H[v], 0, XH));
    feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, this->w[v], this->H[v], XH, 1.0, Xi));
  }
//   printf("Xi\n");
//   feenox_debug_print_gsl_matrix(Xi, stdout);
  
  
  if (neutron_diffusion.has_sources) {
    feenox_call(gsl_blas_dgemv(CblasTrans, this->w[v], this->H[v], S, 1.0, feenox.pde.bi));
  }
  
  // for source-driven problems
  //   K = Ki + Ai - Xi
  // for criticallity problems
  //   K = Ki + Ai
  //   M = Xi
  gsl_matrix_add(Ki, Ai);
  if (neutron_diffusion.has_fission) {
    if (neutron_diffusion.has_sources) {
      gsl_matrix_scale(Xi, -1.0);
      gsl_matrix_add(Ki, Xi);
    } else {
      gsl_matrix_add(feenox.pde.Mi, Xi);
    }  
  }
  gsl_matrix_add(feenox.pde.Ki, Ki);
  

  gsl_matrix_free(DB);
  gsl_matrix_free(AH);
  gsl_matrix_free(XH);
  
  gsl_matrix_free(Ki);
  gsl_matrix_free(Ai);
  gsl_matrix_free(Xi);
  
  gsl_matrix_free(D);
  gsl_matrix_free(A);
  gsl_matrix_free(X);
  gsl_vector_free(S);
  
  
  feenox_free(chi);
  
#endif
  
  return FEENOX_OK;
  
}

