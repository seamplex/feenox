/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for neutron diffusion FEM: initialization
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
neutron_diffusion_t neutron_diffusion;

int feenox_problem_parse_time_init_neutron_diffusion(void) {

///kw_pde+PROBLEM+detail  * `neutron_diffusion` multi-group core-level neutron diffusion with a FEM formulation 
  
#ifdef HAVE_PETSC
  // virtual methods
  feenox.pde.parse_bc = feenox_problem_bc_parse_neutron_diffusion;
  feenox.pde.parse_write_results = feenox_problem_parse_write_post_neutron_diffusion;
  
  feenox.pde.init_before_run = feenox_problem_init_runtime_neutron_diffusion;
  
#ifdef HAVE_SLEPC
  feenox.pde.setup_eps = feenox_problem_setup_eps_neutron_diffusion;
#endif
  feenox.pde.setup_ksp = feenox_problem_setup_ksp_neutron_diffusion;
  feenox.pde.setup_pc = feenox_problem_setup_pc_neutron_diffusion;
  
  feenox.pde.element_build_allocate_aux = feenox_problem_build_allocate_aux_neutron_diffusion;
  feenox.pde.element_build_volumetric = feenox_problem_build_volumetric_neutron_diffusion;
  feenox.pde.element_build_volumetric_at_gauss = feenox_problem_build_volumetric_gauss_point_neutron_diffusion;
  
  feenox.pde.solve_post = feenox_problem_solve_post_neutron_diffusion;
  feenox.pde.gradient_fill = feenox_problem_gradient_fill_neutron_diffusion;
  feenox.pde.gradient_nodal_properties = feenox_problem_gradient_properties_at_element_nodes_neutron_diffusion;
  feenox.pde.gradient_alloc_nodal_fluxes = feenox_problem_gradient_fluxes_at_node_alloc_neutron_diffusion;
  feenox.pde.gradient_add_elemental_contribution_to_node = feenox_problem_gradient_add_elemental_contribution_to_node_neutron_diffusion;
  feenox.pde.gradient_fill_fluxes = feenox_problem_gradient_fill_fluxes_neutron_diffusion;
  
  // we are FEM
  feenox.mesh.default_field_location = field_location_nodes;
  
  // default is 1 group
  if (neutron_diffusion.groups == 0) {
    neutron_diffusion.groups = 1;
  }
  // dofs = number of groups
  feenox.pde.dofs = neutron_diffusion.groups;
  
  // scalar fluxes and currents
  feenox_check_alloc(feenox.pde.unknown_name = calloc(neutron_diffusion.groups, sizeof(char *)));
  for (unsigned int g = 0; g < neutron_diffusion.groups; g++) {
///re_neutron_diffusion+phi+description The scalar flux for the group $g$ is `phig`, i.e. `phi1`, `phi2`, etc. This is the primary unknown of the problem.  
    feenox_check_minusone(asprintf(&feenox.pde.unknown_name[g], "phi%u", g+1));
  }

  // currents (we need a different loop from the one used for fluxes)
  feenox_check_alloc(neutron_diffusion.Jx = calloc(neutron_diffusion.groups, sizeof(function_t *)));
  if (feenox.pde.dim > 1) {
    feenox_check_alloc(neutron_diffusion.Jy = calloc(neutron_diffusion.groups, sizeof(function_t *)));
    if (feenox.pde.dim > 2) {
      feenox_check_alloc(neutron_diffusion.Jz = calloc(neutron_diffusion.groups, sizeof(function_t *)));
    }
  }
  
  
///re_neutron_diffusion+Jx+description The\ $x$ component of the neutron current for group\ $g$ is `Jxg`,  i.e. `Jx1`, `Jx2`. etc. This is a secondary unknown of the problem.  
///re_neutron_diffusion+Jx+description The\ $y$ component of the neutron current for group\ $g$ is `Jzg`,  i.e. `Jz1`, `Jz2`. etc. This is a secondary unknown of the problem.  
///re_neutron_diffusion+Jy+description Only available for two and three-dimensional problems.  
///re_neutron_diffusion+Jx+description The\ $z$ component of the neutron current for group\ $g$ is `Jzg`,  i.e. `Jz1`, `Jz2`. etc. This is a secondary unknown of the problem.  
///re_neutron_diffusion+Jz+description Only available for three-dimensional problems.
  for (unsigned int g = 0; g < neutron_diffusion.groups; g++) {
    char *name = NULL;
    feenox_check_minusone(asprintf(&name, "Jx%u", g+1));
    feenox_call(feenox_problem_define_solution_function(name, &neutron_diffusion.Jx[g], FEENOX_SOLUTION_GRADIENT));
    feenox_free(name);
    if (feenox.pde.dim > 1) {
      feenox_check_minusone(asprintf(&name, "Jy%u", g+1));
      feenox_call(feenox_problem_define_solution_function(name, &neutron_diffusion.Jy[g], FEENOX_SOLUTION_GRADIENT));
      feenox_free(name);
      if (feenox.pde.dim > 2) {
        feenox_check_minusone(asprintf(&name, "Jz%u", g+1));
        feenox_call(feenox_problem_define_solution_function(name, &neutron_diffusion.Jz[g], FEENOX_SOLUTION_GRADIENT));
        feenox_free(name);
      }
    }  
  }
  

  // TODO: for one group make an alias between phi1 and phi
  
///va_neutron_diffusion+chi+desc A vector of size groups with the fission spectrum.
///va_neutron_diffusion+chi+desc Default is one in the first group and zero in the rest.
  neutron_diffusion.chi = feenox_define_vector_get_ptr("chi", neutron_diffusion.groups);
  feenox_vector_set(neutron_diffusion.chi, 0, 1.0);
  

///va_neutron_diffusion+keff+desc The effective multiplication factor\ $k_\text{eff}$.
  neutron_diffusion.keff = feenox_define_variable_get_ptr("keff");
  
#endif
  
  return FEENOX_OK;
}


int feenox_problem_init_runtime_neutron_diffusion(void) {

#ifdef HAVE_PETSC  
  // we are FEM
  feenox.mesh.default_field_location = field_location_nodes;
  feenox.pde.mesh->data_type = data_type_node;
  feenox.pde.spatial_unknowns = feenox.pde.mesh->n_nodes;
  
  // initialize XSs
  int G = neutron_diffusion.groups;  
  double fission_spectrum_integral = 0;
  feenox_check_alloc(neutron_diffusion.D          = calloc(G, sizeof(distribution_t)));
  feenox_check_alloc(neutron_diffusion.Sigma_t    = calloc(G, sizeof(distribution_t)));
  feenox_check_alloc(neutron_diffusion.Sigma_a    = calloc(G, sizeof(distribution_t)));
  feenox_check_alloc(neutron_diffusion.nu_Sigma_f = calloc(G, sizeof(distribution_t)));
  feenox_check_alloc(neutron_diffusion.S          = calloc(G, sizeof(distribution_t)));
  feenox_check_alloc(neutron_diffusion.Sigma_s    = calloc(G, sizeof(distribution_t *)));
  for (unsigned int g = 0; g < G; g++) {
    char *name = NULL;

    feenox_check_minusone(asprintf(&name, "D%u", g+1));
    feenox_distribution_init(&neutron_diffusion.D[g], name);
    if (neutron_diffusion.D[g].defined) {
      neutron_diffusion.D[g].non_uniform = feenox_depends_on_space(neutron_diffusion.D[g].dependency_variables);
      neutron_diffusion.space_XS |= neutron_diffusion.D[g].non_uniform;
    }
    feenox_free(name);
    
    feenox_check_minusone(asprintf(&name, "Sigma_t%u", g+1));
    feenox_distribution_init(&neutron_diffusion.Sigma_t[g], name);
    if (neutron_diffusion.Sigma_t[g].defined) {
      neutron_diffusion.Sigma_t[g].non_uniform = feenox_depends_on_space(neutron_diffusion.Sigma_t[g].dependency_variables);
      neutron_diffusion.space_XS |= neutron_diffusion.Sigma_t[g].non_uniform;
    }
    feenox_free(name);
    
    feenox_check_minusone(asprintf(&name, "Sigma_a%u", g+1));
    feenox_distribution_init(&neutron_diffusion.Sigma_a[g], name);
    if (neutron_diffusion.Sigma_a[g].defined) {
      neutron_diffusion.Sigma_a[g].non_uniform = feenox_depends_on_space(neutron_diffusion.Sigma_a[g].dependency_variables);
      neutron_diffusion.space_XS |= neutron_diffusion.Sigma_a[g].non_uniform;
    }
    feenox_free(name);

    feenox_check_minusone(asprintf(&name, "nuSigma_f%u", g+1));
    feenox_distribution_init(&neutron_diffusion.nu_Sigma_f[g], name);
    if (neutron_diffusion.nu_Sigma_f[g].defined) {
      neutron_diffusion.has_fission = 1;
      neutron_diffusion.nu_Sigma_f[g].non_uniform = feenox_depends_on_space(neutron_diffusion.nu_Sigma_f[g].dependency_variables);
      neutron_diffusion.space_XS |= neutron_diffusion.nu_Sigma_f[g].non_uniform;
    }
    feenox_free(name);

    feenox_check_minusone(asprintf(&name, "S%u", g+1));
    feenox_distribution_init(&neutron_diffusion.S[g], name);
    if (neutron_diffusion.S[g].defined) {
      neutron_diffusion.has_sources = 1;
      neutron_diffusion.S[g].non_uniform = feenox_depends_on_space(neutron_diffusion.S[g].dependency_variables);
      neutron_diffusion.space_XS |= neutron_diffusion.S[g].non_uniform;
    }
    feenox_free(name);
    
    feenox_check_alloc(neutron_diffusion.Sigma_s[g] = calloc(feenox.pde.dofs, sizeof(distribution_t)));
    unsigned int g_prime = 0;
    for (g_prime = 0; g_prime < feenox.pde.dofs; g_prime++) {
      feenox_check_minusone(asprintf(&name, "Sigma_s%u.%u", g+1, g_prime+1));
      feenox_distribution_init(&neutron_diffusion.Sigma_s[g][g_prime], name);
      if (neutron_diffusion.Sigma_s[g][g_prime].defined) {
        neutron_diffusion.Sigma_s[g][g_prime].non_uniform = feenox_depends_on_space(neutron_diffusion.Sigma_s[g][g_prime].dependency_variables);
      }  
      feenox_free(name);
      
      // try again with another underscore
      if (neutron_diffusion.Sigma_s[g][g_prime].defined == 0) {
        feenox_check_minusone(asprintf(&name, "Sigma_s%u_%u", g+1, g_prime+1));
        feenox_distribution_init(&neutron_diffusion.Sigma_s[g][g_prime], name);
        if (neutron_diffusion.Sigma_s[g][g_prime].defined) {
          neutron_diffusion.Sigma_s[g][g_prime].non_uniform = feenox_depends_on_space(neutron_diffusion.Sigma_s[g][g_prime].dependency_variables);
        }  
        feenox_free(name);
      }
    }
    
    fission_spectrum_integral += feenox_vector_get(neutron_diffusion.chi, g);
  }

  if (neutron_diffusion.has_sources == 0 && neutron_diffusion.has_fission == 0) {
    feenox_push_error_message("neither fission nor sources found");
    return FEENOX_ERROR;
  }
  
  if (fabs(fission_spectrum_integral-1) > 1e-6) {
    feenox_push_error_message("fission spectrum is not normalized, its integral is %g", fission_spectrum_integral);
    return FEENOX_ERROR;
  }
  
  if (neutron_diffusion.has_sources == 0) {
#ifdef HAVE_SLEPC    
    // TODO: higher harmonics?
    feenox.pde.nev = 1;
    
    // define eigenvectors (we don't know its size yet)
    feenox_check_alloc(feenox.pde.vectors.phi = calloc(feenox.pde.nev, sizeof(vector_t *)));
    for (unsigned int g = 0; g < feenox.pde.nev; g++) {
      char *modename = NULL;
      feenox_check_minusone(asprintf(&modename, "eig%u", g+1));
      feenox_check_alloc(feenox.pde.vectors.phi[g] = feenox_define_vector_get_ptr(modename, 0));
      feenox_free(modename);
    }
#else
    feenox_push_error_message("criticality problems cannot be solved without SLEPc");
    return FEENOX_ERROR;
#endif
  }
  
  // allocate elemental XS matrices
  feenox_check_alloc(neutron_diffusion.D_G = gsl_matrix_calloc(G * feenox.pde.dim, G * feenox.pde.dim));
  feenox_check_alloc(neutron_diffusion.R   = gsl_matrix_calloc(G, G));
  feenox_check_alloc(neutron_diffusion.X   = gsl_matrix_calloc(G, G));
  feenox_check_alloc(neutron_diffusion.s   = gsl_vector_calloc(G));
  

  feenox.pde.math_type = neutron_diffusion.has_sources ? math_type_linear : math_type_eigen;
  feenox.pde.solve     = neutron_diffusion.has_sources ? feenox_problem_solve_petsc_linear : feenox_problem_solve_slepc_eigen;
  
  feenox.pde.has_stiffness = 1;
  feenox.pde.has_mass = !neutron_diffusion.has_sources;
  feenox.pde.has_rhs  =  neutron_diffusion.has_sources;
  
  feenox.pde.has_jacobian_K = 0;
  feenox.pde.has_jacobian_M = 0;
  feenox.pde.has_jacobian_b = 0;
  feenox.pde.has_jacobian = feenox.pde.has_jacobian_K || feenox.pde.has_jacobian_M || feenox.pde.has_jacobian_b;
  
  feenox.pde.symmetric_K = 0;
  feenox.pde.symmetric_M = 0;
  
  // see if we have to compute gradients
  for (unsigned int g = 0; g < neutron_diffusion.groups; g++) {
    feenox.pde.compute_gradients |= (neutron_diffusion.Jx != NULL && neutron_diffusion.Jx[g]->used) ||
                                    (neutron_diffusion.Jy != NULL && neutron_diffusion.Jy[g]->used) ||
                                    (neutron_diffusion.Jz != NULL && neutron_diffusion.Jz[g]->used);
  }
#endif
  return FEENOX_OK;
}

#ifdef HAVE_PETSC
int feenox_problem_setup_pc_neutron_diffusion(PC pc) {

  PCType pc_type = NULL;
  petsc_call(PCGetType(pc, &pc_type));
  if (pc_type == NULL) {
    if (neutron_diffusion.has_sources == 0) {
      // PC for EPS
#ifdef PETSC_HAVE_MUMPS
    // if we don't set the pc type here then we PCFactorSetMatSolverType does not work
      petsc_call(PCSetType(pc, feenox.pde.symmetric_K ? PCCHOLESKY : PCLU));
      petsc_call(PCFactorSetMatSolverType(pc, MATSOLVERMUMPS));
#else
      // TODO: this will complain in parallel
//      petsc_call(PCSetType(pc, PCLU));    
#endif
    } else {
      // plain PC
      // defaults
    }
  }
  
  return FEENOX_OK;
}

int feenox_problem_setup_ksp_neutron_diffusion(KSP ksp ) {

  KSPType ksp_type = NULL;
  petsc_call(KSPGetType(ksp, &ksp_type));
  if (ksp_type == NULL) {
    if (neutron_diffusion.has_sources == 0) {
      // KSP for EPS
      // defaults
    } else {
      // plain KSP
      // defaults
    }
  }  

  return FEENOX_OK;
}
#endif

#ifdef HAVE_SLEPC
int feenox_problem_setup_eps_neutron_diffusion(EPS eps) {

  petsc_call(EPSSetProblemType(eps, (neutron_diffusion.groups == 1) ? EPS_PGNHEP : EPS_GNHEP));
  
  // we expect the eigenvalue to be near one and an absolute test is faster
  petsc_call(EPSSetConvergenceTest(feenox.pde.eps, EPS_CONV_ABS));
  
  // offsets around one
  if (feenox_var_value(feenox.pde.vars.eps_st_sigma) == 0)
  {
    feenox_var_value(feenox.pde.vars.eps_st_sigma) = 1.0;
  }
  if (feenox_var_value(feenox.pde.vars.eps_st_nu) == 0)
  {
    feenox_var_value(feenox.pde.vars.eps_st_nu) = 1.0;
  }
  
  if (feenox.pde.eigen_formulation == eigen_formulation_omega) {
    petsc_call(EPSSetWhichEigenpairs(eps, EPS_SMALLEST_MAGNITUDE));
  } else {
    petsc_call(EPSSetWhichEigenpairs(eps, EPS_LARGEST_MAGNITUDE));
  }
  

  return FEENOX_OK;
}
#endif

