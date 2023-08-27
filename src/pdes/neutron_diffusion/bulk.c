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
    gsl_matrix_free(neutron_diffusion.RH);
    if (neutron_diffusion.has_fission) {
      gsl_matrix_free(neutron_diffusion.Fi);
      gsl_matrix_free(neutron_diffusion.XH);
    }
  }
    
  feenox_check_alloc(neutron_diffusion.Li = gsl_matrix_calloc(GJ, GJ));
  feenox_check_alloc(neutron_diffusion.Ai = gsl_matrix_calloc(GJ, GJ));
  feenox_check_alloc(neutron_diffusion.DB = gsl_matrix_calloc(G*D, G*J));
  feenox_check_alloc(neutron_diffusion.RH = gsl_matrix_calloc(G, GJ));
  if (neutron_diffusion.has_fission) {
    feenox_check_alloc(neutron_diffusion.Fi = gsl_matrix_calloc(GJ, GJ));
    feenox_check_alloc(neutron_diffusion.XH = gsl_matrix_calloc(G, GJ));
  }

  return FEENOX_OK;
}


int feenox_problem_build_volumetric_neutron_diffusion(element_t *e) {
  if (neutron_diffusion.space_XS == 0) {
    feenox_call(feenox_problem_neutron_diffusion_eval_XS(feenox_fem_get_material(e), NULL));
  }
  return FEENOX_OK;
}


int feenox_problem_neutron_diffusion_eval_XS(material_t *material, double *x) {
  
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
    double val = gsl_matrix_get(neutron_diffusion.R, g, g);
    if (neutron_diffusion.Sigma_t[g].defined) {
      val += neutron_diffusion.Sigma_t[g].eval(&neutron_diffusion.Sigma_t[g], x, material);
    } else {
      val += neutron_diffusion.Sigma_a[g].eval(&neutron_diffusion.Sigma_a[g], x, material);
      for (int g_prime = 0; g_prime < neutron_diffusion.groups; g_prime++) {
        val += neutron_diffusion.Sigma_s[g][g_prime].eval(&neutron_diffusion.Sigma_s[g][g_prime], x, material);
      }
    }
    gsl_matrix_set(neutron_diffusion.R, g, g, val);
    
    // leaks
    for (int m = 0; m < feenox.pde.dim; m++) {
      if (neutron_diffusion.D[g].defined) {
        val = neutron_diffusion.D[g].eval(&neutron_diffusion.D[g], x, material);
      } else if (neutron_diffusion.Sigma_t[g].defined) {
        val = 1.0/(3.0*neutron_diffusion.Sigma_t[g].eval(&neutron_diffusion.Sigma_t[g], x, material));
      } else {
        feenox_push_error_message("neither D nor Sigma_t given for group %d", g);
        return FEENOX_ERROR;
      }

      unsigned int index = m*neutron_diffusion.groups + g;
      gsl_matrix_set(neutron_diffusion.D_G, index, index, val);
    }
  }

  return FEENOX_OK;  
}


int feenox_problem_build_volumetric_gauss_point_neutron_diffusion(element_t *e, unsigned int q) {

#ifdef HAVE_PETSC
  
  if (neutron_diffusion.space_XS != 0) {
    double *x = feenox_fem_compute_x_at_gauss(e, q, feenox.pde.mesh->integration);
    feenox_call(feenox_problem_neutron_diffusion_eval_XS(feenox_fem_get_material(e), x));
  }

  // elemental stiffness for the diffusion term B'*D*B
  double wdet = feenox_fem_compute_w_det_at_gauss(e, q, feenox.pde.mesh->integration);
  gsl_matrix *B = feenox_fem_compute_B_G_at_gauss(e, q, feenox.pde.mesh->integration);
  feenox_call(feenox_blas_BtCB(B, neutron_diffusion.D_G, neutron_diffusion.DB, wdet, neutron_diffusion.Li));

  // elemental scattering H'*A*H
  gsl_matrix *H = feenox_fem_compute_H_Gc_at_gauss(e, q, feenox.pde.mesh->integration);
  feenox_call(feenox_blas_BtCB(H, neutron_diffusion.R, neutron_diffusion.RH, wdet, neutron_diffusion.Ai));
  
  // elemental fission matrix
  if (neutron_diffusion.has_fission) {
    feenox_call(feenox_blas_BtCB(H, neutron_diffusion.X, neutron_diffusion.XH, wdet, neutron_diffusion.Fi));
  }
  
  if (neutron_diffusion.has_sources) {
    feenox_call(feenox_blas_Atb_accum(H, neutron_diffusion.s, wdet, feenox.fem.bi));
  }
  
  // for source-driven problems
  //   Ki = Li + Ai - Xi
  // for criticallity problems
  //   Ki = Li + Ai
  //   Mi = Xi
  feenox_call(gsl_matrix_add(neutron_diffusion.Li, neutron_diffusion.Ai));
  if (neutron_diffusion.has_fission) {
    if (neutron_diffusion.has_sources) {
      feenox_call(gsl_matrix_scale(neutron_diffusion.Fi, -1.0));
      feenox_call(gsl_matrix_add(neutron_diffusion.Li, neutron_diffusion.Fi));
    } else {
      feenox_call(gsl_matrix_add(feenox.fem.Mi, neutron_diffusion.Fi));
    }  
  }
  feenox_call(gsl_matrix_add(feenox.fem.Ki, neutron_diffusion.Li));
 
#endif
  
  return FEENOX_OK;
  
}

