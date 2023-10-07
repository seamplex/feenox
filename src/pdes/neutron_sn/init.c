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

int feenox_problem_parse_time_init_neutron_sn(void) {

///kw_pde+PROBLEM+detail  * `neutron_sn` multi-group core-level neutron transport using 
///kw_pde+PROBLEM+detail     - discrete ordinates $S_N$ for angular discretization, and
///kw_pde+PROBLEM+detail     - isoparametric finite elements for spatial discretization.

  // virtual methods
#ifdef HAVE_PETSC
  feenox.pde.parse_bc = feenox_problem_bc_parse_neutron_sn;
  feenox.pde.parse_write_results = feenox_problem_parse_write_post_neutron_sn;

  feenox.pde.init_before_run = feenox_problem_init_runtime_neutron_sn;
  
#ifdef HAVE_SLEPC
  feenox.pde.setup_eps = feenox_problem_setup_eps_neutron_sn;
#endif
  feenox.pde.setup_ksp = feenox_problem_setup_ksp_neutron_sn;
  feenox.pde.setup_pc = feenox_problem_setup_pc_neutron_sn;
  
  feenox.pde.element_build_allocate_aux = feenox_problem_build_allocate_aux_neutron_sn;
  feenox.pde.element_build_volumetric = feenox_problem_build_volumetric_neutron_sn;
  feenox.pde.element_build_volumetric_at_gauss = feenox_problem_build_volumetric_gauss_point_neutron_sn;
  
  feenox.pde.solve_post = feenox_problem_solve_post_neutron_sn;

  // we are FEM
  feenox.mesh.default_field_location = field_location_nodes;
  
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
      break;
    case 2:
      neutron_sn.directions = 0.5*neutron_sn.N*(neutron_sn.N+2);
      break;
    case 3:
      neutron_sn.directions = neutron_sn.N*(neutron_sn.N+2);
      break;
  }
  
  
  // dofs = number of directions * number of groups
  feenox.pde.dofs =  neutron_sn.directions * neutron_sn.groups;
  
  // ------- neutron transport outputs -----------------------------------  
  
  // the angular fluxes psi
  feenox_check_alloc(feenox.pde.unknown_name = calloc(feenox.pde.dofs, sizeof(char *)));
  // TODO: document from comments
  for (unsigned int m = 0; m < neutron_sn.directions; m++) {
    for (unsigned int g = 0; g < neutron_sn.groups; g++) {
      feenox_check_minusone(asprintf(&feenox.pde.unknown_name[m * neutron_sn.groups + g], "psi%u.%u", m+1, g+1));
    }
  }

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
        feenox_push_error_message("unsupported N = %d in 1D", neutron_sn.N);
        return FEENOX_ERROR;
      break;
    }
  } else {
    // table 4-1 page 162 from lewiss
    // we could have used file sndir2.dat from fentraco, but the values are different

    // first set the weights as all the possible permutations in the first octant
    // and then we fill in the others
    // the initial cosine directions passed to feenox_problem_neutron_sn_init_cosines()
    // are taken from table 4-1 from lewiss (p 162)
    // which coincide with table 1 p 208 from stammler-abbate
    
    // TODO: allow user provided first cosine and weights
    
    double *weights_array = NULL;
    switch (neutron_sn.N) {
      case 2:
      {
        feenox_call(feenox_problem_neutron_sn_init_cosines(1.0/M_SQRT3));
        feenox_check_alloc(weights_array = calloc(1, sizeof(double)));
        weights_array[0] = 1;
      }
      break;
      
      case 4:
      {
        feenox_call(feenox_problem_neutron_sn_init_cosines(0.3500212));
        feenox_check_alloc(weights_array = calloc(1, sizeof(double)));
        weights_array[0] = 1.0/3.0;
      }
      break;
      
      case 6:
      {
        feenox_call(feenox_problem_neutron_sn_init_cosines(0.2666355));
        feenox_check_alloc(weights_array = calloc(2, sizeof(double)));
        weights_array[0] = 0.1761263;
        weights_array[1] = 0.1572071;

      }
      break;
      
      case 8:
      {
        feenox_call(feenox_problem_neutron_sn_init_cosines(0.2182179));
        feenox_check_alloc(weights_array = calloc(3, sizeof(double)));
        weights_array[0] = 0.1209877;
        weights_array[1] = 0.0907407;
        weights_array[2] = 0.0925926;       
      }
      break;
      
      case 10:
      {
        feenox_call(feenox_problem_neutron_sn_init_cosines(0.1893213));
        feenox_check_alloc(weights_array = calloc(4, sizeof(double)));
        weights_array[0] = 0.1402771*2/M_PI;
        weights_array[1] = 0.1139285*2/M_PI;
        weights_array[2] = 0.0707546*2/M_PI;       
        weights_array[3] = 0.0847101*2/M_PI;       
      }
      break;

      case 12:
      {
        feenox_call(feenox_problem_neutron_sn_init_cosines(0.1672126));
        feenox_check_alloc(weights_array = calloc(5, sizeof(double)));
        weights_array[0] = 0.0707626;
        weights_array[1] = 0.0558811;
        weights_array[2] = 0.0373377;
        weights_array[3] = 0.0502819;
        weights_array[4] = 0.0258513;
      }
      break;
      
      case 16:
      {
        feenox_call(feenox_problem_neutron_sn_init_cosines(0.1389568));
        feenox_check_alloc(weights_array = calloc(8, sizeof(double)));
        weights_array[0] = 0.0489872;
        weights_array[1] = 0.0413296;
        weights_array[2] = 0.0212326;
        weights_array[3] = 0.0256207;
        weights_array[4] = 0.0360486;
        weights_array[5] = 0.0144589;
        weights_array[6] = 0.0344958;
        weights_array[7] = 0.0085179;
      }
      break;
        
      default: 
        feenox_push_error_message("unsupported N = %d in %dD", neutron_sn.N, feenox.pde.dim);
        return FEENOX_ERROR;
      break;
    }
    
    // from the weights array now assign one weight to each direction
    feenox_call(feenox_problem_neutron_sn_init_triangles(weights_array));
    feenox_free(weights_array);
    
    // print the nice SN triangle
/*    
    int N_over_2 = neutron_sn.N/2;
    for (int i = 0; i < N_over_2; i++) {
      for (int j = 0; j < N_over_2-1-i; j++) {
        printf(" ");
      }
      for (int j = 0; j < i+1; j++) {
        printf("%d ", 1+neutron_sn.SN_triangle[i][j]);
      }
      printf("\n");
    }
*/
    
    // we have filled the first octant, now fill in the other ones
    // this is for 2 and 3 dimensions only
    int N_octs = (feenox.pde.dim == 2) ? 4 : 8;
    int J_octs = neutron_sn.directions / N_octs;
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
  if (fabs(s-1) > 1e-6) {
    feenox_push_error_message("S%d weights do not sum up to one but to %g", neutron_sn.N, s);
    return FEENOX_ERROR;
  }
  
  for (unsigned int d = 0; d < feenox.pde.dim; d++) {
    s = 0;
    for (unsigned int n = 0; n < neutron_sn.directions; n++) {
      s += neutron_sn.w[n] * neutron_sn.Omega[n][d];
    }
    if (fabs(s) > 1e-6) {
      feenox_push_error_message("S%d weights are not symmetric (zero != %g)", neutron_sn.N, s);
      return FEENOX_ERROR;
    }
    
    s = 0;
    for (unsigned int n = 0; n < neutron_sn.directions; n++) {
      s += neutron_sn.w[n] * gsl_pow_2(neutron_sn.Omega[n][d]);
    }
    if (fabs(s-1.0/3.0) > 1e-6) {
      feenox_push_error_message("S%d weights are not symmetric (one third != %g)", neutron_sn.N, s);
      return FEENOX_ERROR;
    }
  }
  
  // ---------------------------------------------------------------------------
  
#endif
  
  return FEENOX_OK;
}


int feenox_problem_init_runtime_neutron_sn(void) {

#ifdef HAVE_PETSC  
  feenox.pde.mesh->data_type = data_type_node;
  feenox.pde.spatial_unknowns = feenox.pde.mesh->n_nodes;

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
        gsl_matrix_set(neutron_sn.D, sn_dof_index(m,g), d*MG + sn_dof_index(m,g), neutron_sn.Omega[m][d]);
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
    // if we don't set the pc type here then we PCFactorSetMatSolverType does not work
    petsc_call(PCSetType(pc, feenox.pde.symmetric_K ? PCCHOLESKY : PCLU));
#ifdef PETSC_HAVE_MUMPS
    petsc_call(PCFactorSetMatSolverType(pc, MATSOLVERMUMPS));
#else
    // defaults so in serial it should be petsc's lu and in parallel ilu (I think)
//    petsc_call(PCSetType(pc, PCLU));
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
  petsc_call(EPSSetProblemType(eps, EPS_GNHEP));

  // we expect the eigenvalue to be near one and an absolute test is fater
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
  
  return FEENOX_OK;
}
#endif


int feenox_problem_neutron_sn_init_cosines(double mu1) {
  int N_over_2 = neutron_sn.N/2;
  double *mu = NULL;
  feenox_check_alloc(mu = calloc(N_over_2, sizeof(double)));
  
  // equation 21 in stammler
  // equation 4-8 in lewis
  mu[0] = mu1;
  double C = 2*(1-3*gsl_pow_2(mu1))/(neutron_sn.N-2);
  for (int i = 1; i < N_over_2; i++) {
    mu[i] = sqrt(gsl_pow_2(mu1) + C*i);
  }
  
  // now we have to assign the cosines to each of the m directions
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



int feenox_problem_neutron_sn_init_triangles(double *weights_array) {
  
  int N_over_2 = neutron_sn.N/2;
  
  // allocate stuff
  int ***tmp = NULL;
  feenox_check_alloc(tmp = calloc(6, sizeof(int **)));
  for (int p = 0; p < 6; p++) {
    feenox_check_alloc(tmp[p] = calloc(N_over_2, sizeof(int *)));
    for (int j = 0; j < N_over_2; j++) {
      feenox_check_alloc(tmp[p][j] = calloc(N_over_2, sizeof(int)));
    }
  }
  feenox_check_alloc(neutron_sn.SN_triangle = calloc(N_over_2, sizeof(int *)));
  for (int j = 0; j < N_over_2; j++) {
    feenox_check_alloc(neutron_sn.SN_triangle[j] = calloc(N_over_2, sizeof(int *)));
  }

  int *weight_map = NULL;
  feenox_check_alloc(weight_map = calloc(neutron_sn.N*(neutron_sn.N+1)/2, sizeof(int)));

  // fill in the 6 temporary triangles
  int m = 1;
  for (int i = N_over_2-1; i >= 0; i--) {
    for (int j = 0; j < i+1; j++) {
      tmp[0][i][j] = m;
      m++;
    }
  }

  for (int i = 0; i < N_over_2; i++) {
    for (int j = 0; j < i+1; j++) {
      tmp[1][i][i-j] = tmp[0][i][j];
    }
  }
  
  for (int i = 0; i < N_over_2; i++) {
    for (int j = 0; j < i+1; j++) {
      tmp[2][i][j] = tmp[1][N_over_2-i-1 + j][j];
    }
  }

  for (int i = 0; i < N_over_2; i++) {
    for (int j = 0; j < i+1; j++) {
      tmp[3][i][j] = tmp[0][N_over_2-j-1][N_over_2-i-1];
    }
  }
  
  for (int i = 0; i < N_over_2; i++) {
    for (int j = 0; j < i+1; j++) {
      tmp[4][i][j] = tmp[3][N_over_2-i-1 + j][j];
    }
  }

  for (int i = 0; i < N_over_2; i++) {
    for (int j = 0; j < i+1; j++) {
      tmp[5][i][j] = tmp[0][N_over_2-i-1 + j][j];
    }
  }
  
  // compute the min and create a map
  for (int i = 0; i < N_over_2; i++) {
    for (int j = 0; j < i+1; j++) {
      int min = neutron_sn.N;
      for (int p = 0; p < 6; p++) {
        if (tmp[p][i][j] < min) {
          min = tmp[p][i][j];
        }
      }
      
      neutron_sn.SN_triangle[i][j] = min;
      weight_map[min] = 1;
    }
  }
  
  // assign the weight and create the SN triangle 
  double N_octs = (feenox.pde.dim == 2) ? 4 : 8;
  m = 0;
  for (int i = 0; i < N_over_2; i++) {
    for (int j = 0; j < i+1; j++) {
      int w = 0;
      for (int k = 0; k < neutron_sn.SN_triangle[i][j]; k++) {
        w += weight_map[k] != 0;
      }
      neutron_sn.w[m] = weights_array[w]/N_octs;
      neutron_sn.SN_triangle[i][j] = w;
      m++;
    }
  }
  

  for (int p = 0; p < 6; p++) {
    for (int i = 0; i < N_over_2; i++) {
      feenox_free(tmp[p][i]);
    }
    feenox_free(tmp[p]);
  }
  feenox_free(tmp);
  feenox_free(weight_map);
  
  return FEENOX_OK;

}
