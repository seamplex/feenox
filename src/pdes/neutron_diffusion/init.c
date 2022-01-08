/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for neutron diffusion FEM: initialization
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

int feenox_problem_init_parser_neutron_diffusion(void) {

#ifdef HAVE_PETSC
  feenox.pde.problem_init_runtime_particular = feenox_problem_init_runtime_neutron_diffusion;
  feenox.pde.bc_parse = feenox_problem_bc_parse_neutron_diffusion;
#ifdef HAVE_SLEPC
  feenox.pde.setup_eps = feenox_problem_setup_eps_neutron_diffusion;
#endif
  feenox.pde.setup_ksp = feenox_problem_setup_ksp_neutron_diffusion;
  feenox.pde.setup_pc = feenox_problem_setup_pc_neutron_diffusion;
  feenox.pde.bc_set_dirichlet = feenox_problem_bc_set_neutron_diffusion_null;
  feenox.pde.build_element_volumetric_gauss_point = feenox_problem_build_volumetric_gauss_point_neutron_diffusion;
  feenox.pde.solve_post = feenox_problem_solve_post_neutron_diffusion;
  
  // default is 1 group
  if (neutron_diffusion.groups == 0) {
    neutron_diffusion.groups = 1;
  }
  // dofs = number of groups
  feenox.pde.dofs = neutron_diffusion.groups;
  
  feenox_check_alloc(feenox.pde.unknown_name = calloc(feenox.pde.dofs, sizeof(char *)));
  unsigned int g = 0;
  for (g = 0; g < feenox.pde.dofs; g++) {
    feenox_check_minusone(asprintf(&feenox.pde.unknown_name[g], "phi%d", g+1));
  }
  feenox_call(feenox_problem_define_solutions());
  
  // TODO: for one group make an alias between phi1 and phi
  
  // we'd rather ser nodes than cells 
  feenox.mesh.default_field_location = field_location_nodes;
  
///va_neutron_diffusion+keff+desc The effective multiplication factor\ $k_\text{eff}$.
  neutron_diffusion.keff = feenox_define_variable_get_ptr("keff");

#endif
  
  return FEENOX_OK;
}


int feenox_problem_init_runtime_neutron_diffusion(void) {

#ifdef HAVE_PETSC  
  // we are FEM not FVM
  feenox.pde.mesh->data_type = data_type_node;
  feenox.pde.spatial_unknowns = feenox.pde.mesh->n_nodes;
  feenox.pde.size_global = feenox.pde.spatial_unknowns * feenox.pde.dofs;

  // set the size of the eigenvectors (we did not know their size in init_parser() above
  unsigned int i = 0;
  for (i = 0; i < feenox.pde.nev; i++) {
    feenox_call(feenox_vector_set_size(feenox.pde.vectors.phi[i], feenox.pde.size_global));
  }
  
  // initialize XSs
  feenox_check_alloc(neutron_diffusion.D = calloc(feenox.pde.dofs, sizeof(distribution_t)));
  feenox_check_alloc(neutron_diffusion.sigma_t = calloc(feenox.pde.dofs, sizeof(distribution_t)));
  feenox_check_alloc(neutron_diffusion.sigma_a = calloc(feenox.pde.dofs, sizeof(distribution_t)));
  feenox_check_alloc(neutron_diffusion.nu_sigma_f = calloc(feenox.pde.dofs, sizeof(distribution_t)));
  feenox_check_alloc(neutron_diffusion.source = calloc(feenox.pde.dofs, sizeof(distribution_t)));
  feenox_check_alloc(neutron_diffusion.sigma_s = calloc(feenox.pde.dofs, sizeof(distribution_t *)));
  unsigned int g = 0;
  for (g = 0; g < feenox.pde.dofs; g++) {
    char *name = NULL;

    feenox_check_minusone(asprintf(&name, "D%d", g+1));
    feenox_distribution_init(&neutron_diffusion.D[g], name);
    if (neutron_diffusion.D[g].defined) {
      neutron_diffusion.D[g].uniform = feenox_expression_depends_on_space(neutron_diffusion.D[g].dependency_variables);
    }
    feenox_free(name);
    
    feenox_check_minusone(asprintf(&name, "Sigma_t%d", g+1));
    feenox_distribution_init(&neutron_diffusion.sigma_t[g], name);
    if (neutron_diffusion.sigma_t[g].defined) {
      neutron_diffusion.sigma_t[g].uniform = feenox_expression_depends_on_space(neutron_diffusion.sigma_t[g].dependency_variables);
    }
    feenox_free(name);
    
    feenox_check_minusone(asprintf(&name, "Sigma_a%d", g+1));
    feenox_distribution_init(&neutron_diffusion.sigma_a[g], name);
    if (neutron_diffusion.sigma_a[g].defined) {
      neutron_diffusion.sigma_a[g].uniform = feenox_expression_depends_on_space(neutron_diffusion.sigma_a[g].dependency_variables);
    }  
    feenox_free(name);

    feenox_check_minusone(asprintf(&name, "nuSigma_f%d", g+1));
    feenox_distribution_init(&neutron_diffusion.nu_sigma_f[g], name);
    if (neutron_diffusion.nu_sigma_f[g].defined) {
      neutron_diffusion.has_fission = 1;
      neutron_diffusion.nu_sigma_f[g].uniform = feenox_expression_depends_on_space(neutron_diffusion.nu_sigma_f[g].dependency_variables);
    }  
    feenox_free(name);

    feenox_check_minusone(asprintf(&name, "S%d", g+1));
    feenox_distribution_init(&neutron_diffusion.source[g], name);
    if (neutron_diffusion.source[g].defined) {
      neutron_diffusion.has_sources = 1;
      neutron_diffusion.source[g].uniform = feenox_expression_depends_on_space(neutron_diffusion.source[g].dependency_variables);
    }  
    feenox_free(name);
    
    feenox_check_alloc(neutron_diffusion.sigma_s[g] = calloc(feenox.pde.dofs, sizeof(distribution_t)));
    unsigned int g_prime = 0;
    for (g_prime = 0; g_prime < feenox.pde.dofs; g_prime++) {
      feenox_check_minusone(asprintf(&name, "Sigma_s%d.%d", g+1, g_prime+1));
      feenox_distribution_init(&neutron_diffusion.sigma_s[g][g_prime], name);
      if (neutron_diffusion.sigma_s[g][g_prime].defined) {
        neutron_diffusion.sigma_s[g][g_prime].uniform = feenox_expression_depends_on_space(neutron_diffusion.sigma_s[g][g_prime].dependency_variables);
      }  
      feenox_free(name);
    }
  }


  if (neutron_diffusion.has_sources == 0 && neutron_diffusion.has_fission == 0) {
    feenox_push_error_message("neither fission nor sources found");
    return FEENOX_ERROR;
  }
  
  if (neutron_diffusion.has_sources == 0) {
#ifdef HAVE_SLEPC    
    // TODO: higher harmonics?
    feenox.pde.nev = 1;
    
    // define eigenvectors (we don't know its size yet)
    feenox_check_alloc(feenox.pde.vectors.phi = calloc(feenox.pde.nev, sizeof(vector_t *)));
    for (g = 0; g < feenox.pde.nev; g++) {
      char *modename = NULL;
      feenox_check_minusone(asprintf(&modename, "eig%d", g+1));
      feenox_check_alloc(feenox.pde.vectors.phi[g] = feenox_define_vector_get_ptr(modename, 0));
      feenox_free(modename);
    }
#else
    feenox_push_error_message("criticality problems cannot be solved without SLEPc");
    return FEENOX_ERROR;
#endif
  }
  

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
      petsc_call(PCSetType(pc, PCCHOLESKY));
      petsc_call(PCFactorSetMatSolverType(pc, MATSOLVERMUMPS));
#else
      // TODO: this will complain in parallel
      petsc_call(PCSetType(pc, PCLU));    
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

  // generalized non-hermitian problem
//  petsc_call(EPSSetProblemType(eps, EPS_GNHEP));
  
  if (feenox.pde.eigen_formulation == eigen_formulation_omega) {
    petsc_call(EPSSetWhichEigenpairs(eps, EPS_SMALLEST_MAGNITUDE));
  } else {
    petsc_call(EPSSetWhichEigenpairs(eps, EPS_LARGEST_MAGNITUDE));
  }
  

  return FEENOX_OK;
}
#endif

