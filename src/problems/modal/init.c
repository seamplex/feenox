#include "feenox.h"
#include "modal.h"
extern feenox_t feenox;
modal_t modal;

int feenox_problem_parse_modal(const char *token) {
  
  if (token != NULL) {
    if (strcasecmp(token, "plane_stress") == 0) {
      modal.variant = variant_plane_stress;
  
    } else if (strcasecmp(token, "plane_strain") == 0) {
      modal.variant = variant_plane_strain;
  
    } else {
      feenox_push_error_message("undefined keyword '%s'", token);
      return FEENOX_ERROR;
    }
  } 
  
  return FEENOX_OK;
}  

int feenox_problem_init_parser_modal(void) {

  feenox.pde.problem_init_runtime_particular = feenox_problem_init_runtime_modal;
  feenox.pde.bc_parse = feenox_problem_bc_parse_modal;
  feenox.pde.setup_eps = feenox_problem_setup_eps_modal;
  feenox.pde.setup_ksp = feenox_problem_setup_ksp_modal;
  feenox.pde.setup_pc = feenox_problem_setup_pc_modal;
  feenox.pde.bc_set_dirichlet = feenox_problem_bc_set_dirichlet_modal;
  feenox.pde.build_element_volumetric_gauss_point = feenox_problem_build_volumetric_gauss_point_modal;
  feenox.pde.solve_post = feenox_problem_solve_post_modal;
  
  if (feenox.pde.symmetry_axis != symmetry_axis_none ||
                 modal.variant == variant_plane_stress ||
                 modal.variant == variant_plane_strain) {

    if (feenox.pde.dim != 0) {
      if (feenox.pde.dim != 2) {
        feenox_push_error_message("dimension inconsistency, expected DIMENSION 2");
        return FEENOX_ERROR;
      }
    } else {
      feenox.pde.dim = 2;
    }

    if (feenox.pde.dofs != 0) {
      if (feenox.pde.dofs != 2) {
        feenox_push_error_message("DOF inconsistency");
        return FEENOX_ERROR;
      }
    } else {
      feenox.pde.dofs = 2;
    }
  } else {
    feenox.pde.dofs = feenox.pde.dim;
  }  
  
  // if there are no explicit number of eigenvalues we set a non-zero value here
  if (feenox.pde.nev == 0) {
    feenox.pde.nev = DEFAULT_MODAL_MODES;
  }

  // vector problem    
  feenox.pde.dofs = feenox.pde.mesh->dim;
  
  // TODO: custom names
  feenox_check_alloc(feenox.pde.unknown_name = calloc(feenox.pde.dofs, sizeof(char *)));
  feenox_check_alloc(feenox.pde.unknown_name[0] = strdup("u"));
  if (feenox.pde.dofs > 1) {
    feenox_check_alloc(feenox.pde.unknown_name[1] = strdup("v"));
    if (feenox.pde.dofs > 2) {
      feenox_check_alloc(feenox.pde.unknown_name[2] = strdup("w"));
    }  
  }
  

  // we are FEM not FVM
  feenox.pde.mesh->data_type = data_type_node;
  feenox.mesh.default_field_location = field_location_nodes;
  feenox_call(feenox_problem_define_solutions());
  
///va+M_T+name M_T
///va+M_T+desc A scalar with the total mass\ $m$ computed from the mass matrix\ $M$ as
///va+M_T+desc 
///va+M_T+desc \[ M_T = \frac{1}{n_\text{DOFs}} \cdot \vec{1}^T \cdot M \cdot \vec{1} \]
///va+M_T+desc 
///va+M_T+desc where $n_\text{DOFs}$ is the number of degrees of freedoms per node.
///va+M_T+desc Note that this is only approximately equal to the actual mass whih is
///va+M_T+desc the integral of the density $\rho(x,y,z)$ over the problem domain.
  modal.M_T = feenox_define_variable_get_ptr("M_T");

///ve+f+name f
///ve+f+desc _Size:_ number of requested modes.
///ve+f+desc _Elements:_ The frequency $f_i$ of the $i$-th mode, in cycles per unit of time.
  modal.f = feenox_define_vector_get_ptr("f", feenox.pde.nev);

///ve+omega+name omega
///ve+omega+desc _Size:_ number of requested modes.
///ve+omega+desc _Elements:_ The angular frequency $\omega_i$ of the $i$-th mode, in radians per unit of time.
  modal.omega = feenox_define_vector_get_ptr("omega", feenox.pde.nev);


///ve+m+name m
///ve+m+desc _Size:_ number of requested modes.
///ve+m+desc _Elements:_ The generalized modal mass $M_i$ of the $i$-th mode computed as
///ve+m+desc
///ve+m+desc \[ \text{m}_i = \frac{1}{n_\text{DOFs}} \vec{\phi}_i^T \cdot M \cdot \vec{\phi}_i \]
///va+m+desc 
///va+m+desc where $n_\text{DOFs}$ is the number of degrees of freedoms per node, $M$ is the mass matrix
///va+m+desc and $\vec{\phi}_i$ is the $i$-th eigenvector normalized such that the largest element is equal to one.

  modal.m = feenox_define_vector_get_ptr("m", feenox.pde.nev);

///ve+L+name L
///ve+L+desc _Size:_ number of requested modes.
///ve+L+desc _Elements:_ The excitation factor $L_i$ of the $i$-th mode computed as
///ve+L+desc
///ve+L+desc \[ L_i = \frac{1}{n_\text{DOFs}} \cdot \vec{\phi}_i^T \cdot M \cdot \vec{1} \]
///va+L+desc 
///va+L+desc where $n_\text{DOFs}$ is the number of degrees of freedoms per node, $M$ is the mass matrix
///va+L+desc and $\vec{\phi}_i$ is the $i$-th eigenvector normalized such that the largest element is equal to one.
  modal.L = feenox_define_vector_get_ptr("L", feenox.pde.nev);

///ve+Gamma+name Gamma
///ve+Gamma+desc _Size:_ number of requested modes.
///ve+Gamma+desc _Elements:_ The participation factor $\Gamma_i$ of the $i$-th mode computed as
///ve+Gamma+desc
///ve+Gamma+desc \[ \Gamma_i = \frac{ \vec{\phi}_i^T \cdot M \cdot \vec{1} }{ \vec{\phi}_i^T \cdot M \cdot \vec{\phi}} \]
  modal.Gamma = feenox_define_vector_get_ptr("Gamma", feenox.pde.nev);

///ve+mu+name mu
///ve+mu+desc _Size:_ number of requested modes.
///ve+mu+desc _Elements:_ The relatve effective modal mass $\mu_i$ of the $i$-th mode computed as
///ve+mu+desc
///ve+mu+desc \[ \mu_i = \frac{L_i^2}{M_t \cdot n_\text{DOFs} \cdot m_i} \]
///ve+mu+desc
///ve+mu+desc Note that $\sum_{i=1}^N m_i = 1$, where $N$ is total number of degrees of freedom ($n_\text{DOFs}$ times the number of nodes).
  modal.mu = feenox_define_vector_get_ptr("mu", feenox.pde.nev);

///ve+Mu+name Mu
///ve+Mu+desc _Size:_ number of requested modes.
///ve+Mu+desc _Elements:_ The accumulated relative effective modal mass $\Mu_i$ up to the $i$-th mode computed as
///ve+Mu+desc
///ve+Mu+desc \[ \Mu_i = \sum_{j=1}^i \mu_i \]
///ve+Mu+desc
///ve+Mu+desc Note that $\Mu_N = 1$, where $N$ is total number of degrees of freedom ($n_\text{DOFs}$ times the number of nodes).
  modal.Mu = feenox_define_vector_get_ptr("Mu", feenox.pde.nev);

  // define eigenvectors (we don't know its size yet)
  feenox_check_alloc(feenox.pde.vectors.phi = calloc(feenox.pde.nev, sizeof(vector_t *)));;
  unsigned int i = 0;
  for (i = 0; i < feenox.pde.nev; i++) {
    char *modename = NULL;
    feenox_check_minusone(asprintf(&modename, "phi%d", i+1));
    feenox_check_alloc(feenox.pde.vectors.phi[i] = feenox_define_vector_get_ptr(modename, 0));
    feenox_free(modename);
  }

  return FEENOX_OK;
}


int feenox_problem_init_runtime_modal(void) {
  
  feenox.pde.spatial_unknowns = feenox.pde.mesh->n_nodes;
  feenox.pde.global_size = feenox.pde.spatial_unknowns * feenox.pde.dofs;

  // set the size of the eigenvectors (we did not know their size in init_parser() above
  unsigned int i = 0;
  for (i = 0; i < feenox.pde.nev; i++) {
    feenox_call(feenox_vector_set_size(feenox.pde.vectors.phi[i], feenox.pde.global_size));
  }
  
  // initialize distributions
  feenox_call(feenox_distribution_init(&modal.E, "E"));
  if (modal.E.defined == 0) {
    feenox_push_error_message("undefined elastic modulus 'E'");
    return FEENOX_ERROR;
  }
  if (modal.E.full == 0) {
    feenox_push_error_message("elastic modulus 'E' is not defined over all volumes");
    return FEENOX_ERROR;
  }
  modal.E.space_dependent = feenox_expression_depends_on_space(modal.E.dependency_variables);
  

  feenox_call(feenox_distribution_init(&modal.nu, "nu"));
  if (modal.nu.defined == 0) {
    feenox_push_error_message("undefined Poisson ratio 'nu'");
    return FEENOX_ERROR;
  }
  if (modal.nu.full == 0) {
    feenox_push_error_message("Poisson ratio 'nu' is not defined over all volumes");
    return FEENOX_ERROR;
  }
  modal.nu.space_dependent = feenox_expression_depends_on_space(modal.nu.dependency_variables);


  feenox_call(feenox_distribution_init(&modal.rho, "rho"));
  if (modal.rho.defined == 0) {
    feenox_push_error_message("undefined density 'rho'");
    return FEENOX_ERROR;
  }
  if (modal.rho.full == 0) {
    feenox_push_error_message("density 'rho' is not defined over all volumes");
    return FEENOX_ERROR;
  }
  modal.rho.space_dependent = feenox_expression_depends_on_space(modal.rho.dependency_variables);
  

  feenox.pde.math_type = math_type_eigen;
  feenox.pde.solve = feenox_solve_slepc_eigen;
  
  feenox.pde.has_stiffness = PETSC_TRUE;
  feenox.pde.has_mass = PETSC_TRUE;
  feenox.pde.has_rhs = PETSC_FALSE;
  
  feenox.pde.has_jacobian_K = PETSC_FALSE;
  feenox.pde.has_jacobian_M = PETSC_FALSE;
  feenox.pde.has_jacobian_b = PETSC_FALSE;
  feenox.pde.has_jacobian = feenox.pde.has_jacobian_K || feenox.pde.has_jacobian_M || feenox.pde.has_jacobian_b;
  
  feenox.pde.symmetric_K = PETSC_TRUE;
  feenox.pde.symmetric_M = PETSC_TRUE;

  return FEENOX_OK;
}

#ifdef HAVE_PETSC
int feenox_problem_setup_pc_modal(PC pc) {

  PCType pc_type = NULL;
  petsc_call(PCGetType(pc, &pc_type));
  if (pc_type == NULL) {
#ifdef PETSC_HAVE_MUMPS
    petsc_call(PCSetType(pc, PCCHOLESKY));
    petsc_call(PCFactorSetMatSolverType(pc, MATSOLVERMUMPS));
#else
    // TODO: this will complain in parallel
    petsc_call(PCSetType(pc, PCLU));    
#endif
  }
  
  petsc_call(PCGetType(pc, &pc_type));
  if (strcmp(pc_type, PCGAMG) == 0) {

    if (modal.rigid_body_base == NULL) {
      feenox_problem_mechanical_compute_rigid_nullspace(&modal.rigid_body_base);
    }  
    
    petsc_call(MatSetNearNullSpace(feenox.pde.K, modal.rigid_body_base));
    petsc_call(MatSetNearNullSpace(feenox.pde.K_bc, modal.rigid_body_base));
    petsc_call(MatSetNearNullSpace(feenox.pde.M, modal.rigid_body_base));
    petsc_call(MatSetNearNullSpace(feenox.pde.M_bc, modal.rigid_body_base));
  }
  
  return FEENOX_OK;
}

int feenox_problem_setup_ksp_modal(KSP ksp ) {

  KSPType ksp_type = NULL;
  petsc_call(KSPGetType(ksp, &ksp_type));
  if (ksp_type == NULL) {
    // if the user did not choose anything, we default to preonly + direct solver
    petsc_call(KSPSetType(ksp, KSPPREONLY));
  }  

  return FEENOX_OK;
}
#endif

#ifdef HAVE_SLEPC
// these two are not 
int feenox_problem_setup_eps_modal(EPS eps) {

  ST st = NULL;
  petsc_call(EPSGetST(feenox.pde.eps, &st));
  if (st == NULL) {
    feenox_push_error_message("error getting spectral transform object");
    return FEENOX_ERROR;
  }
  
  // the user might have already set a custom ST, se we peek what it is
  STType st_type = NULL;
  petsc_call(STGetType(st, &st_type));
  
  if (st_type != NULL && feenox.pde.eigen_formulation == eigen_formulation_undefined) {
    // if there is no formulation but an st_type, choose an appropriate one
    feenox.pde.eigen_formulation = (strcmp(st_type, STSHIFT) == 0) ? eigen_formulation_lambda : eigen_formulation_omega;
  }
  
  if (feenox.pde.eigen_formulation == eigen_formulation_omega) {
    if (st_type != NULL) {
      if (strcmp(st_type, STSINVERT) != 0) {
        feenox_push_error_message("eigen formulation omega needs shift-and-invert spectral transformation");
        return FEENOX_ERROR;
      }  
    } else {
      petsc_call(STSetType(st, STSINVERT));
    }
    // shift and invert needs a target
    petsc_call(EPSSetTarget(eps, feenox_var_value(feenox.pde.vars.eps_st_sigma)));
    
  } else {
    // lambda needs shift
    if (st_type != NULL) {
      if (strcmp(st_type, STSHIFT) != 0) {
        feenox_push_error_message("eigen formulation lambda needs shift spectral transformation");
        return FEENOX_ERROR;
      }
    } else {
      petsc_call(STSetType(st, STSHIFT));
    }
    // seek for largest lambda (i.e. smallest omega)
    petsc_call(EPSSetWhichEigenpairs(eps, EPS_LARGEST_MAGNITUDE));
  }  

  // to be able to solve free-body vibrations we have to set a deflation space
  if (modal.has_dirichlet_bcs == 0) {
    if (modal.rigid_body_base == NULL) {
      feenox_problem_mechanical_compute_rigid_nullspace(&modal.rigid_body_base);
    }  
  
    // getvecs below needs a const vec pointer
    PetscBool has_const;
    PetscInt n;
    const Vec *vecs;
    Vec rigid[6];
    petsc_call(MatNullSpaceGetVecs(modal.rigid_body_base, &has_const, &n, &vecs));
  
    // but eps needs modifiable vectors so we copy them
    unsigned int k = 0;
    for (k = 0; k < n; k++) {
      petsc_call(VecDuplicate(vecs[k], &rigid[k]));
      petsc_call(VecCopy(vecs[k], rigid[k]));
    }
    
    petsc_call(EPSSetDeflationSpace(eps, n, rigid));
  }   
  
  return FEENOX_OK;
}
#endif
