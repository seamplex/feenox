/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for neutron transport FEM: initialization
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
neutron_sn_t neutron_sn;

int feenox_problem_neutron_sn_init_cosines(double mu1) {
  // equation 21 in stammler
  // equation 4-8 in lewis
  int N_over_2 = neutron_sn.N/2;
  double *mu = NULL;
  feenox_check_alloc(mu = calloc(N_over_2, sizeof(double)));
  
  mu[0] = mu1;
  double C = 2*(1-3*gsl_pow_2(mu1))/(neutron_sn.N-2);
  for (int i = 1; i < N_over_2; i++) {
    mu[i] = sqrt(gsl_pow_2(mu1) + C*i);
  }
  
  // we have these triangles
  // S4
  // 1
  // 1 1
  //
  // S6
  // 1
  // 2 2
  // 1 2 1
  //
  // S8
  // 1
  // 2 2
  // 2 3 2
  // 1 2 2 1
  
  int m = 0;
  for (int row = 0; row < N_over_2; row++) {
    for (int col = 0; col <= row; col++) {
      int i = N_over_2 - row - 1;
      int j = col;
      int k = N_over_2 + 2 - 3 - i - j;
      
      neutron_sn.Omega[m][0] = mu[i];
      neutron_sn.Omega[m][1] = mu[j];
      neutron_sn.Omega[m][2] = (feenox.pde.dim == 3) ? mu[k] : 0;
      m++;
      
    }
  }
  
  feenox_free(mu);
  
  return FEENOX_OK;
}

int feenox_problem_init_parser_neutron_sn(void) {

///kw_pde+PROBLEM+detail  * `neutron_sn` multi-group core-level neutron transport using 
///kw_pde+PROBLEM+detail     - discrete ordinates $S_N$ for angular discretization, and
///kw_pde+PROBLEM+detail     - isoparametric finite elements for spatial discretization.

  // we are FEM
  feenox.mesh.default_field_location = field_location_nodes;

#ifdef HAVE_PETSC
  feenox.pde.init_runtime_particular = feenox_problem_init_runtime_neutron_sn;
  feenox.pde.bc_parse = feenox_problem_bc_parse_neutron_sn;
#ifdef HAVE_SLEPC
  feenox.pde.setup_eps = feenox_problem_setup_eps_neutron_sn;
#endif
  feenox.pde.setup_ksp = feenox_problem_setup_ksp_neutron_sn;
  feenox.pde.setup_pc = feenox_problem_setup_pc_neutron_sn;
  feenox.pde.build_element_volumetric_gauss_point = feenox_problem_build_volumetric_gauss_point_neutron_sn;
  feenox.pde.solve_post = feenox_problem_solve_post_neutron_sn;
  
  // default is 1 group
  if (neutron_sn.groups == 0) {
    neutron_sn.groups = 1;
  }
  
  // default is N=2
  if (neutron_sn.N == 0) {
    neutron_sn.N = 2;
  }
  if (neutron_sn.N % 2 != 0) {
    feenox_push_error_message("number of ordinates N = %d has to be even", neutron_sn.N);
    return FEENOX_ERROR;
  }
  
  // stammler's eq 6.19
  switch(feenox.pde.dim) {
    case 1:
      neutron_sn.directions = neutron_sn.N;
//      neutron_sn.supg_dimension_factor = 1;
      break;
    case 2:
      neutron_sn.directions = 0.5*neutron_sn.N*(neutron_sn.N+2);
//      neutron_sn.supg_dimension_factor = 4;
//      neutron_sn.supg_dimension_factor = 6;
      break;
    case 3:
      neutron_sn.directions = neutron_sn.N*(neutron_sn.N+2);
//      neutron_sn.supg_dimension_factor = 6;
//      neutron_sn.supg_dimension_factor = 12;
//      neutron_sn.supg_dimension_factor = 16;
      break;
  }
  
  
  // dofs = number of directions * number of groups
  feenox.pde.dofs =  neutron_sn.directions * neutron_sn.groups;
  
  // ------- neutron transport outputs -----------------------------------  
  
  // the angular fluxes psi
  feenox_check_alloc(feenox.pde.unknown_name = calloc(feenox.pde.dofs, sizeof(char *)));
  // TODO: document from comments
  for (int n = 0; n < neutron_sn.directions; n++) {
    for (int g = 0; g < neutron_sn.groups; g++) {
      feenox_check_minusone(asprintf(&feenox.pde.unknown_name[n * neutron_sn.groups + g], "psi%d.%d", n+1, g+1));
    }
  }
  feenox_call(feenox_problem_define_solutions());

  // the scalar fluxes psi
  // TODO: document
  // TODO: for one group make an alias between phi1 and phi
  feenox_check_alloc(neutron_sn.phi = calloc(neutron_sn.groups, sizeof(function_t *)));
  for (unsigned int g = 0; g < neutron_sn.groups; g++) {
    char *name = NULL;
    feenox_check_minusone(asprintf(&name, "phi%u", g+1));
    feenox_call(feenox_problem_define_solution_function(name, &neutron_sn.phi[g], FEENOX_SOLUTION_NOT_GRADIENT));
    feenox_free(name);
  }
  
///va_neutron_sn+keff+desc The effective multiplication factor\ $k_\text{eff}$.
  neutron_sn.keff = feenox_define_variable_get_ptr("keff");

///va_neutron_sn+sn_alpha+desc The stabilization parameter\ $\alpha$ for $S_N$.
  neutron_sn.sn_alpha = feenox_define_variable_get_ptr("sn_alpha");
  feenox_var_value(neutron_sn.sn_alpha) = 0.5;
  
  // ------------ initialize the SN weights ------------------------------------
  // TODO: when adding FVM, these are the same so we should move it into a single location
  feenox_check_alloc(neutron_sn.w = calloc(neutron_sn.directions, sizeof(double)));
  feenox_check_alloc(neutron_sn.Omega = calloc(neutron_sn.directions, sizeof(double *)));
  for (int m = 0; m < neutron_sn.directions; m++) {
    feenox_check_alloc(neutron_sn.Omega[m] = calloc(3, sizeof(double)));
  }
  
  if (feenox.pde.dim == 1) {
    // table 3-1 page 121 lewiss
    // in 1D the directions are the zeros of the legendre polymonials
    // and the weights are such the gauss quadrature is exact
    switch (neutron_sn.N) {
      case 2:
        neutron_sn.Omega[0][0] = +1.0/M_SQRT3;
        neutron_sn.w[0] = 1.0/2.0;

        neutron_sn.Omega[1][0] = -neutron_sn.Omega[0][0];
        neutron_sn.w[1] = neutron_sn.w[0];
      break;
      case 4:
        neutron_sn.Omega[0][0] =  sqrt(3.0/7.0 - 2.0/7.0 * sqrt(6.0/5.0));
        neutron_sn.w[0] = 0.6521451549/2.0;

        neutron_sn.Omega[1][0] =  sqrt(3.0/7.0 + 2.0/7.0 * sqrt(6.0/5.0));
        neutron_sn.w[1] = 0.3478548451/2.0;

        neutron_sn.Omega[2][0] = -neutron_sn.Omega[0][0];
        neutron_sn.w[2] = neutron_sn.w[0];

        neutron_sn.Omega[3][0] = -neutron_sn.Omega[1][0];
        neutron_sn.w[3] = neutron_sn.w[1];
      break;

      case 6:
        neutron_sn.Omega[0][0] = 0.2386191860;
        neutron_sn.w[0] = 0.4679139346/2.0;

        neutron_sn.Omega[1][0] = 0.6612093864;
        neutron_sn.w[1] = 0.3607615730/2.0;

        neutron_sn.Omega[2][0] = 0.9324695142;
        neutron_sn.w[2] = 0.1713244924/2.0;

        neutron_sn.Omega[3][0] = -neutron_sn.Omega[0][0];
        neutron_sn.w[3] = neutron_sn.w[0];

        neutron_sn.Omega[4][0] = -neutron_sn.Omega[1][0];
        neutron_sn.w[4] = neutron_sn.w[1];

        neutron_sn.Omega[5][0] = -neutron_sn.Omega[2][0];
        neutron_sn.w[5] = neutron_sn.w[2];
      break;

      case 8:
        neutron_sn.Omega[0][0] = 0.1834346424;
        neutron_sn.w[0] = 0.3626837834/2.0;

        neutron_sn.Omega[1][0] = 0.5255324099;
        neutron_sn.w[1] = 0.3137066459/2.0;

        neutron_sn.Omega[2][0] = 0.7966664774;
        neutron_sn.w[2] = 0.2223810344/2.0;

        neutron_sn.Omega[3][0] = 0.9602898564;
        neutron_sn.w[3] = 0.1012285363/2.0;

        neutron_sn.Omega[4][0] = -neutron_sn.Omega[0][0];
        neutron_sn.w[4] = neutron_sn.w[0];

        neutron_sn.Omega[5][0] = -neutron_sn.Omega[1][0];
        neutron_sn.w[5] = neutron_sn.w[1];

        neutron_sn.Omega[6][0] = -neutron_sn.Omega[2][0];
        neutron_sn.w[6] = neutron_sn.w[2];

        neutron_sn.Omega[7][0] = -neutron_sn.Omega[3][0];
        neutron_sn.w[7] = neutron_sn.w[3];
      break;

      default:
        feenox_push_error_message("unsupported N = %d", neutron_sn.N);
        return FEENOX_ERROR;
      break;
    }
  } else {
    // table 4-1 page 162 from lewiss
    // we could have used file sndir2.dat from fentraco, but the values are different

    // this is for 2 and 3 dimensions
    int N_octs = (feenox.pde.dim == 2) ? 4 : 8;
    int J_octs = neutron_sn.directions / N_octs;

    // first set the weights as all the possible permutations in the first octant
    // and then we fill in the others
    // the initial cosine directions passed to feenox_problem_neutron_sn_init_cosines()
    // are taken from table 4-1 from lewiss (p 162)
    // which coincide with table 1 p 208 from stammler-abbate
    
    switch (neutron_sn.N) {
      case 2:
      {
        feenox_call(feenox_problem_neutron_sn_init_cosines(1.0/M_SQRT3));
        
        neutron_sn.w[0] = 1.0/(double)(N_octs);
      }
      break;
      
      case 4:
      {
        feenox_call(feenox_problem_neutron_sn_init_cosines(0.3500212));
        double s4_w1 = 1.0/3.0;

        neutron_sn.w[0] = s4_w1/(double)(N_octs);
        
        neutron_sn.w[1] = s4_w1/(double)(N_octs);
        neutron_sn.w[2] = s4_w1/(double)(N_octs);
      }
      break;
      
      case 6:
      {
        feenox_call(feenox_problem_neutron_sn_init_cosines(0.2666355));
        
        double s6_w1 = 0.1761263;
        double s6_w2 = 0.1572071;

        neutron_sn.w[0] = s6_w1/(double)(N_octs);
        
        neutron_sn.w[1] = s6_w2/(double)(N_octs);
        neutron_sn.w[2] = s6_w2/(double)(N_octs);
        
        neutron_sn.w[3] = s6_w1/(double)(N_octs);
        neutron_sn.w[4] = s6_w2/(double)(N_octs);
        neutron_sn.w[5] = s6_w1/(double)(N_octs);
      }
      break;
      
      case 8:
      {
        feenox_call(feenox_problem_neutron_sn_init_cosines(0.2182179));

        double s8_w1 = 0.1209877;
        double s8_w2 = 0.0907407;
        double s8_w3 = 0.0925926;       
        

        neutron_sn.w[0] = s8_w1/(double)(N_octs);
        
        neutron_sn.w[1] = s8_w2/(double)(N_octs);
        neutron_sn.w[2] = s8_w2/(double)(N_octs);
        
        neutron_sn.w[3] = s8_w2/(double)(N_octs);
        neutron_sn.w[4] = s8_w3/(double)(N_octs);
        neutron_sn.w[5] = s8_w2/(double)(N_octs);
        
        neutron_sn.w[6] = s8_w1/(double)(N_octs);
        neutron_sn.w[7] = s8_w2/(double)(N_octs);
        neutron_sn.w[8] = s8_w2/(double)(N_octs);
        neutron_sn.w[9] = s8_w1/(double)(N_octs);
        
      }
      break;
        
      default: 
        feenox_push_error_message("unsupported N = %d in %dD", neutron_sn.N, feenox.pde.dim);
        return FEENOX_ERROR;
      break;
    }
    
   
    // we have filled the first octant, now fill in the other ones
    for (int n = 1; n < N_octs; n++) {
      for (int j = 0; j < J_octs; j++) {
        neutron_sn.Omega[n*J_octs + j][0] = ((n & 1) ? (-1) : (+1)) * neutron_sn.Omega[j][0];
        neutron_sn.Omega[n*J_octs + j][1] = ((n & 2) ? (-1) : (+1)) * neutron_sn.Omega[j][1];
        neutron_sn.Omega[n*J_octs + j][2] = ((n & 4) ? (-1) : (+1)) * neutron_sn.Omega[j][2];
        neutron_sn.w[n*J_octs + j] = neutron_sn.w[j];
      }
    }
  }
  
  // checks
  double s = 0;
  for (unsigned int n = 0; n < neutron_sn.directions; n++) {
    s += neutron_sn.w[n];
  }
  assert(fabs(s-1) < 1e-6);
  
  for (unsigned int d = 0; d < feenox.pde.dim; d++) {
    s = 0;
    for (unsigned int n = 0; n < neutron_sn.directions; n++) {
      s += neutron_sn.w[n] * neutron_sn.Omega[n][d];
    }
    assert(fabs(s) < 1e-6);
    
    s = 0;
    for (unsigned int n = 0; n < neutron_sn.directions; n++) {
      s += neutron_sn.w[n] * gsl_pow_2(neutron_sn.Omega[n][d]);
    }
    assert(fabs(s-1.0/3.0) < 1e-6);
  }
  
  // ---------------------------------------------------------------------------
  
#endif
  
  return FEENOX_OK;
}


int feenox_problem_init_runtime_neutron_sn(void) {

#ifdef HAVE_PETSC  
  feenox.pde.mesh->data_type = data_type_node;
  feenox.pde.spatial_unknowns = feenox.pde.mesh->n_nodes;
  feenox.pde.size_global = feenox.pde.spatial_unknowns * feenox.pde.dofs;

  // set the size of the eigenvectors (we did not know their size in init_parser() above
  for (PetscInt i = 0; i < feenox.pde.nev; i++) {
    feenox_call(feenox_vector_set_size(feenox.pde.vectors.phi[i], feenox.pde.size_global));
  }
  
  // initialize XSs
  // TODO: allow not to give the group number in one-group problems
  unsigned int G = neutron_sn.groups;
  feenox_check_alloc(neutron_sn.Sigma_t    = calloc(G, sizeof(distribution_t)));
  feenox_check_alloc(neutron_sn.Sigma_a    = calloc(G, sizeof(distribution_t)));
  feenox_check_alloc(neutron_sn.nu_Sigma_f = calloc(G, sizeof(distribution_t)));
  feenox_check_alloc(neutron_sn.S          = calloc(G, sizeof(distribution_t)));
  feenox_check_alloc(neutron_sn.Sigma_s0   = calloc(G, sizeof(distribution_t *)));
  feenox_check_alloc(neutron_sn.Sigma_s1   = calloc(G, sizeof(distribution_t *)));
  for (unsigned int g = 0; g < G; g++) {
    char *name = NULL;

    feenox_check_minusone(asprintf(&name, "Sigma_t%d", g+1));
    feenox_distribution_init(&neutron_sn.Sigma_t[g], name);
    if (neutron_sn.Sigma_t[g].defined) {
      neutron_sn.Sigma_t[g].uniform = feenox_expression_depends_on_space(neutron_sn.Sigma_t[g].dependency_variables);
    }
    feenox_free(name);
    
    feenox_check_minusone(asprintf(&name, "Sigma_a%d", g+1));
    feenox_distribution_init(&neutron_sn.Sigma_a[g], name);
    if (neutron_sn.Sigma_a[g].defined) {
      neutron_sn.Sigma_a[g].uniform = feenox_expression_depends_on_space(neutron_sn.Sigma_a[g].dependency_variables);
    }  
    feenox_free(name);

    feenox_check_minusone(asprintf(&name, "nuSigma_f%d", g+1));
    feenox_distribution_init(&neutron_sn.nu_Sigma_f[g], name);
    if (neutron_sn.nu_Sigma_f[g].defined) {
      neutron_sn.has_fission = 1;
      neutron_sn.nu_Sigma_f[g].uniform = feenox_expression_depends_on_space(neutron_sn.nu_Sigma_f[g].dependency_variables);
    }  
    feenox_free(name);

    feenox_check_minusone(asprintf(&name, "S%d", g+1));
    feenox_distribution_init(&neutron_sn.S[g], name);
    if (neutron_sn.S[g].defined) {
      neutron_sn.has_sources = 1;
      neutron_sn.S[g].uniform = feenox_expression_depends_on_space(neutron_sn.S[g].dependency_variables);
    }  
    feenox_free(name);
    
    feenox_check_alloc(neutron_sn.Sigma_s0[g] = calloc(G, sizeof(distribution_t)));
    feenox_check_alloc(neutron_sn.Sigma_s1[g] = calloc(G, sizeof(distribution_t)));
    for (int g_prime = 0; g_prime < G; g_prime++) {
      feenox_check_minusone(asprintf(&name, "Sigma_s%d.%d", g+1, g_prime+1));
      feenox_distribution_init(&neutron_sn.Sigma_s0[g][g_prime], name);
      if (neutron_sn.Sigma_s0[g][g_prime].defined) {
        neutron_sn.Sigma_s0[g][g_prime].uniform = feenox_expression_depends_on_space(neutron_sn.Sigma_s0[g][g_prime].dependency_variables);
      }  
      feenox_free(name);
      
      feenox_check_minusone(asprintf(&name, "Sigma_s_one%d.%d", g+1, g_prime+1));
      feenox_distribution_init(&neutron_sn.Sigma_s1[g][g_prime], name);
      if (neutron_sn.Sigma_s1[g][g_prime].defined) {
        neutron_sn.Sigma_s1[g][g_prime].uniform = feenox_expression_depends_on_space(neutron_sn.Sigma_s1[g][g_prime].dependency_variables);
      }  
      feenox_free(name);
      
    }
  }


  if (neutron_sn.has_sources == 0 && neutron_sn.has_fission == 0) {
    feenox_push_error_message("neither fission nor sources found");
    return FEENOX_ERROR;
  }
  
  if (neutron_sn.has_sources == 0) {
#ifdef HAVE_SLEPC    
    feenox.pde.math_type = math_type_eigen;
    feenox.pde.solve     = feenox_problem_solve_slepc_eigen;
    feenox.pde.has_mass  = 1;
    feenox.pde.has_rhs   = 0;
    
    // TODO: higher harmonics?
    feenox.pde.nev = 1;
    
    // define eigenvectors (we don't know its size yet)
    feenox_check_alloc(feenox.pde.vectors.phi = calloc(feenox.pde.nev, sizeof(vector_t *)));
    for (int g = 0; g < feenox.pde.nev; g++) {
      char *modename = NULL;
      feenox_check_minusone(asprintf(&modename, "eig%d", g+1));
      feenox_check_alloc(feenox.pde.vectors.phi[g] = feenox_define_vector_get_ptr(modename, 0));
      feenox_free(modename);
    }
#else
    feenox_push_error_message("criticality problems cannot be solved without SLEPc");
    return FEENOX_ERROR;
#endif
  } else {
    // TODO: non-linear?
    feenox.pde.math_type = math_type_linear;
    feenox.pde.solve     = feenox_problem_solve_petsc_linear;
    feenox.pde.has_mass  = 0;
    feenox.pde.has_rhs   = 1;
  }
  
  // allocate elemental XS matrices
  unsigned int M = neutron_sn.directions;
  unsigned int MG = M*G;
  feenox_check_alloc(neutron_sn.R   = gsl_matrix_calloc(MG, MG));
  feenox_check_alloc(neutron_sn.X   = gsl_matrix_calloc(MG, MG));
  feenox_check_alloc(neutron_sn.s   = gsl_vector_calloc(MG));  
  feenox_check_alloc(neutron_sn.chi = calloc(G, sizeof(double)));
  // TODO: read a vector called "chi"
  neutron_sn.chi[0] = 1;
  
  // direction matrix
  feenox_check_alloc(neutron_sn.D = gsl_matrix_calloc(MG, MG * feenox.pde.dim));
  for (unsigned int m = 0; m < M; m++) {
    for (unsigned int g = 0; g < G; g++) {
      for (unsigned int d = 0; d < feenox.pde.dim; d++) {
        gsl_matrix_set(neutron_sn.D, dof_index(m,g), d*MG + dof_index(m,g), neutron_sn.Omega[m][d]);
      }
    }
  }  
  

  
  feenox.pde.has_stiffness = 1;
  
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
int feenox_problem_setup_pc_neutron_sn(PC pc) {

  PCType pc_type = NULL;
  petsc_call(PCGetType(pc, &pc_type));
  if (pc_type == NULL) {
    petsc_call(PCSetType(pc, PCLU));
#ifdef PETSC_HAVE_MUMPS
      petsc_call(PCFactorSetMatSolverType(pc, MATSOLVERMUMPS));
#endif
  }
  
  return FEENOX_OK;
}

int feenox_problem_setup_ksp_neutron_sn(KSP ksp ) {

  KSPType ksp_type = NULL;
  petsc_call(KSPGetType(ksp, &ksp_type));
  if (ksp_type == NULL) {
    petsc_call(KSPSetType(ksp, KSPPREONLY))
  }  

  return FEENOX_OK;
}
#endif

#ifdef HAVE_SLEPC
int feenox_problem_setup_eps_neutron_sn(EPS eps) {

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

