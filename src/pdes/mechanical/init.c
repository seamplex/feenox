#include "feenox.h"
#include "mechanical.h"
extern feenox_t feenox;
mechanical_t mechanical;

int feenox_problem_parse_problem_mechanical(const char *token) {
  
  if (token != NULL) {
    if (strcasecmp(token, "plane_stress") == 0) {
      mechanical.variant = variant_plane_stress;
  
    } else if (strcasecmp(token, "plane_strain") == 0) {
      mechanical.variant = variant_plane_strain;
  
    } else {
      feenox_push_error_message("undefined keyword '%s'", token);
      return FEENOX_ERROR;
      
    }
  } 
  return FEENOX_OK;
} 

int feenox_problem_init_parser_mechanical(void) {
#ifdef HAVE_PETSC
  feenox.pde.problem_init_runtime_particular = feenox_problem_init_runtime_mechanical;
  feenox.pde.bc_parse = feenox_problem_bc_parse_mechanical;
  feenox.pde.setup_ksp = feenox_problem_setup_ksp_mechanical;
  feenox.pde.setup_pc = feenox_problem_setup_pc_mechanical;
//  feenox.pde.setup_snes = feenox_problem_setup_snes_mechanical;
  
  feenox.pde.bc_set_dirichlet = feenox_problem_bc_set_mechanical_displacement;
  feenox.pde.build_element_volumetric_gauss_point = feenox_problem_build_volumetric_gauss_point_mechanical;
  feenox.pde.solve_post = feenox_problem_solve_post_mechanical;
  
  feenox.pde.feenox_problem_gradient_fill = feenox_problem_gradient_fill_mechanical;
  feenox.pde.feenox_problem_gradient_properties_at_element_nodes = feenox_problem_gradient_properties_at_element_nodes_mechanical;
  feenox.pde.feenox_problem_gradient_fluxes_at_node_alloc = feenox_problem_gradient_fluxes_at_node_alloc_mechanical;
  feenox.pde.feenox_problem_gradient_add_elemental_contribution_to_node = feenox_problem_gradient_add_elemental_contribution_to_node_mechanical;
  feenox.pde.feenox_problem_gradient_fill_fluxes = feenox_problem_gradient_fill_fluxes_mechanical;
  
  if (feenox.pde.symmetry_axis != symmetry_axis_none ||
      mechanical.variant == variant_plane_stress ||
      mechanical.variant == variant_plane_strain) {

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

  // TODO: custom names
  feenox_check_alloc(feenox.pde.unknown_name = calloc(feenox.pde.dofs, sizeof(char *)));
  feenox_check_alloc(feenox.pde.unknown_name[0] = strdup("u"));
  if (feenox.pde.dofs > 1) {
    feenox_check_alloc(feenox.pde.unknown_name[1] = strdup("v"));
    if (feenox.pde.dofs > 2) {
      feenox_check_alloc(feenox.pde.unknown_name[2] = strdup("w"));
    }  
  }
  
  feenox_call(feenox_problem_define_solutions());

  feenox_call(feenox_problem_define_solution_function("sigmax", &mechanical.sigmax, 1));
  feenox_call(feenox_problem_define_solution_function("sigmay", &mechanical.sigmay, 1));
  feenox_call(feenox_problem_define_solution_function("tauxy", &mechanical.tauxy, 1));

  if (feenox.pde.dofs == 3) {
    feenox_call(feenox_problem_define_solution_function("sigmaz", &mechanical.sigmaz, 1));
    feenox_call(feenox_problem_define_solution_function("tauyz", &mechanical.tauyz, 1));
    feenox_call(feenox_problem_define_solution_function("tauzx", &mechanical.tauzx, 1));
  }

  feenox_call(feenox_problem_define_solution_function("sigma1", &mechanical.sigma1, 1));
  feenox_call(feenox_problem_define_solution_function("sigma2", &mechanical.sigma2, 1));
  feenox_call(feenox_problem_define_solution_function("sigma3", &mechanical.sigma3, 1));
  feenox_call(feenox_problem_define_solution_function("sigma", &mechanical.sigma, 1));
//  feenox_call(feenox_problem_define_solution_function("delta_sigma", &mechanical.delta_sigma, 1));
  feenox_call(feenox_problem_define_solution_function("tresca", &mechanical.tresca, 1));


// these are for the algebraic expressions in the  which are implicitly-defined BCs
// i.e. 0=u*nx+v*ny
// here they are defined as uppercase because there already exist functions named u, v and w
// but the parser changes their case when an implicit BC is read
/*  
  mechanical.vars.U[0]= feenox_define_variable_get_ptr("U");
  mechanical.vars.U[1]= feenox_define_variable_get_ptr("V");
  mechanical.vars.U[2]= feenox_define_variable_get_ptr("W");
 */

///va+strain_energy+detail The strain energy stored in the solid, computed as
///va+strain_energy+detail $1/2 \cdot \vec{u}^T  K \vec{u}$
///va+strain_energy+detail where $\vec{u}$ is the displacements vector and $K$ is the stiffness matrix.
  mechanical.strain_energy = feenox_define_variable_get_ptr("strain_energy");

///va+displ_max+detail The module of the maximum displacement of the elastic problem.
  mechanical.displ_max = feenox_define_variable_get_ptr("displ_max");

///va+displ_max_x+detail The\ $x$ coordinate of the maximum displacement of the elastic problem.
  mechanical.displ_max_x = feenox_define_variable_get_ptr("displ_max_x");
///va+displ_max_y+detail The\ $y$ coordinate of the maximum displacement of the elastic problem.
  mechanical.displ_max_y = feenox_define_variable_get_ptr("displ_max_y");
///va+displ_max_z+detail The\ $z$ coordinate of the maximum displacement of the elastic problem.
  mechanical.displ_max_z = feenox_define_variable_get_ptr("displ_max_z");

///va+u_at_displ_max+detail The\ $x$ component\ $u$ of the maximum displacement of the elastic problem.
  mechanical.u_at_displ_max = feenox_define_variable_get_ptr("u_at_displ_max");
///va+v_at_displ_max+detail The\ $y$ component\ $v$ of the maximum displacement of the elastic problem.
  mechanical.v_at_displ_max = feenox_define_variable_get_ptr("v_at_displ_max");
///va+w_at_displ_max+detail The\ $z$ component\ $w$ of the maximum displacement of the elastic problem.
  mechanical.w_at_displ_max = feenox_define_variable_get_ptr("w_at_displ_max");
  
///va+sigma_max+detail The maximum von Mises stress\ $\sigma$ of the elastic problem.
  mechanical.sigma_max = feenox_define_variable_get_ptr("sigma_max");

///va+delta_sigma_max+detail The uncertainty of the maximum Von Mises stress\ $\sigma$ of the elastic problem.
///va+delta_sigma_max+detail Not to be confused with the maximum uncertainty of the Von Mises stress.
  mechanical.delta_sigma_max = feenox_define_variable_get_ptr("delta_sigma_max");
  
///va+sigma_max_x+detail The\ $x$ coordinate of the maximum von Mises stress\ $\sigma$ of the elastic problem.
  mechanical.sigma_max_x = feenox_define_variable_get_ptr("sigma_max_x");
///va+sigma_max_y+detail The\ $x$ coordinate of the maximum von Mises stress\ $\sigma$ of the elastic problem.
  mechanical.sigma_max_y = feenox_define_variable_get_ptr("sigma_max_y");
///va+sigma_max_z+detail The\ $x$ coordinate of the maximum von Mises stress\ $\sigma$ of the elastic problem.
  mechanical.sigma_max_z = feenox_define_variable_get_ptr("sigma_max_z");
  
///va+u_at_sigma_max+detail The\ $x$ component\ $u$ of the displacement where the maximum von Mises stress\ $\sigma$ of the elastic problem is located.
  mechanical.u_at_sigma_max = feenox_define_variable_get_ptr("u_at_sigma_max");
///va+v_at_sigma_max+detail The\ $y$ component\ $v$ of the displacement where the maximum von Mises stress\ $\sigma$ of the elastic problem is located.
  mechanical.v_at_sigma_max = feenox_define_variable_get_ptr("v_at_sigma_max");
///va+w_at_sigma_max+detail The\ $z$ component\ $w$ of the displacement where the maximum von Mises stress\ $\sigma$ of the elastic problem is located.
  mechanical.w_at_sigma_max = feenox_define_variable_get_ptr("w_at_sigma_max");
#endif  
  return FEENOX_OK;
}


int feenox_problem_init_runtime_mechanical(void) {

#ifdef HAVE_PETSC 
  // we are FEM not FVM
  feenox.pde.mesh->data_type = data_type_node;
  feenox.pde.spatial_unknowns = feenox.pde.mesh->n_nodes;
  feenox.pde.global_size = feenox.pde.spatial_unknowns * feenox.pde.dofs;

  // initialize distributions
  feenox_distribution_define_mandatory(mechanical, E, "E", "elastic modulus");
  feenox_distribution_define_mandatory(mechanical, nu, "nu", "Poisson’s ratio");
  
  feenox_call(feenox_distribution_init(&mechanical.alpha, "alpha"));
  mechanical.alpha.space_dependent = feenox_expression_depends_on_space(mechanical.alpha.dependency_variables);

  feenox_call(feenox_distribution_init(&mechanical.T, "T"));
  mechanical.T.space_dependent = feenox_expression_depends_on_space(mechanical.T.dependency_variables);
  
  // TODO: read T0
  
  // TODO: check nonlinearity!
  feenox.pde.math_type = math_type_linear;
  feenox.pde.solve = feenox_problem_solve_petsc_linear;
  
  feenox.pde.has_stiffness = PETSC_TRUE;
  // TODO: transient
  feenox.pde.has_mass = PETSC_FALSE;
  feenox.pde.has_rhs = PETSC_TRUE;
  
  feenox.pde.has_jacobian_K = PETSC_FALSE;
  feenox.pde.has_jacobian_M = PETSC_FALSE;
  feenox.pde.has_jacobian_b = PETSC_FALSE;
  feenox.pde.has_jacobian = feenox.pde.has_jacobian_K || feenox.pde.has_jacobian_M || feenox.pde.has_jacobian_b;
  
  feenox.pde.symmetric_K = PETSC_TRUE;
  feenox.pde.symmetric_M = PETSC_TRUE;

  // see if we have to compute gradients
  feenox.pde.compute_gradients |= (mechanical.sigmax != NULL && mechanical.sigmax->used) ||
                                  (mechanical.sigmay != NULL && mechanical.sigmay->used) ||
                                  (mechanical.sigmaz != NULL && mechanical.sigmaz->used) ||
                                  (mechanical.tauxy  != NULL && mechanical.tauxy->used) ||
                                  (mechanical.tauyz  != NULL && mechanical.tauyz->used) ||
                                  (mechanical.tauzx  != NULL && mechanical.tauzx->used) ||
                                  (mechanical.sigma1 != NULL && mechanical.sigma1->used) || 
                                  (mechanical.sigma2 != NULL && mechanical.sigma2->used) ||
                                  (mechanical.sigma3 != NULL && mechanical.sigma3->used) ||
                                  (mechanical.sigma  != NULL && mechanical.sigma->used) || 
                                  (mechanical.tresca != NULL && mechanical.tresca->used);
      

  return FEENOX_OK;
#endif
}


#ifdef HAVE_PETSC
int feenox_problem_compute_rigid_nullspace(MatNullSpace *nullspace) {
  
  Vec vec_coords;
  if (feenox.pde.K != NULL) {
    petsc_call(MatCreateVecs(feenox.pde.K, NULL, &vec_coords));
  } else {
    feenox_check_alloc(vec_coords = feenox_problem_create_vector("coordinates"));
  }  
  petsc_call(VecSetBlockSize(vec_coords, feenox.pde.dofs));
  petsc_call(VecSetUp(vec_coords));

  PetscScalar *coords;
  size_t j = 0;
  unsigned int m = 0;
  petsc_call(VecGetArray(vec_coords, &coords));
  for (j = feenox.pde.first_node; j < feenox.pde.last_node; j++) {          
    for (m = 0; m < feenox.pde.dofs; m++) {
      coords[feenox.pde.mesh->node[j].index_dof[m] - feenox.pde.first_row] = feenox.pde.mesh->node[j].x[m];
    }
  }
  petsc_call(VecRestoreArray(vec_coords, &coords));
    
  petsc_call(MatNullSpaceCreateRigidBody(vec_coords, nullspace));
  petsc_call(VecDestroy(&vec_coords));

  return FEENOX_OK;
}

int feenox_problem_setup_pc_mechanical(PC pc) {

  PCType pc_type = NULL;
  petsc_call(PCGetType(pc, &pc_type));
  if (pc_type == NULL) {
    // TODO: should we use mumps by default if available?
    petsc_call(PCSetType(pc, PCGAMG));
  }
  
  petsc_call(PCGetType(pc, &pc_type));
  if (strcmp(pc_type, PCGAMG) == 0) {

    if (mechanical.rigid_body_base == NULL) {
      feenox_problem_compute_rigid_nullspace(&mechanical.rigid_body_base);
    }  
    
    if (feenox.pde.has_stiffness) {
      petsc_call(MatSetNearNullSpace(feenox.pde.K, mechanical.rigid_body_base));
    }
    if (feenox.pde.has_mass) {
      petsc_call(MatSetNearNullSpace(feenox.pde.M, mechanical.rigid_body_base));
    }  
  }
  
  return FEENOX_OK;
}

int feenox_problem_setup_ksp_mechanical(KSP ksp) {

  KSPType ksp_type = NULL;
  petsc_call(KSPGetType(ksp, &ksp_type));
  if (ksp_type == NULL) {
    // if the user did not choose anything, we default to CG or GMRES
    petsc_call(KSPSetType(ksp, (feenox.pde.symmetric_K == PETSC_TRUE && feenox.pde.symmetric_M == PETSC_TRUE) ? KSPCG : KSPGMRES));
  }  

  return FEENOX_OK;
}

#endif
