#include "feenox.h"
#include "mechanical.h"
extern feenox_t feenox;
mechanical_t mechanical;

int feenox_problem_parse_mechanical(const char *token) {
  
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

  feenox.pde.problem_init_runtime_particular = feenox_problem_init_runtime_mechanical;
//  feenox.pde.bc_parse = feenox_problem_bc_parse_mechanical;
//  feenox.pde.bc_set_dirichlet = feenox_problem_bc_set_dirichlet_mechanical;
//  feenox.pde.build_element_volumetric_gauss_point = feenox_problem_build_volumetric_gauss_point_mechanical;
//  feenox.pde.solve_post = feenox_problem_solve_post_mechanical;
  
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

  // TODO: describir las funciones para reference
  feenox_call(feenox_problem_define_solution_function("sigmax", &mechanical.sigmax));
  feenox_call(feenox_problem_define_solution_function("sigmay", &mechanical.sigmay));
  feenox_call(feenox_problem_define_solution_function("tauxy", &mechanical.tauxy));

  if (feenox.pde.dofs == 3) {
    feenox_call(feenox_problem_define_solution_function("sigmaz", &mechanical.sigmaz));
    feenox_call(feenox_problem_define_solution_function("tauyz", &mechanical.tauyz));
    feenox_call(feenox_problem_define_solution_function("tauzx", &mechanical.tauzx));
  }

  feenox_call(feenox_problem_define_solution_function("sigma1", &mechanical.sigma1));
  feenox_call(feenox_problem_define_solution_function("sigma2", &mechanical.sigma2));
  feenox_call(feenox_problem_define_solution_function("sigma3", &mechanical.sigma3));
  feenox_call(feenox_problem_define_solution_function("sigma", &mechanical.sigma));
  feenox_call(feenox_problem_define_solution_function("delta_sigma", &mechanical.delta_sigma));
  feenox_call(feenox_problem_define_solution_function("tresca", &mechanical.tresca));


// these are for the algebraic expressions in the  which are implicitly-defined BCs
// i.e. 0=u*nx+v*ny
// here they are defined as uppercase because there already exist functions named u, v and w
// but the parser changes their case when an implicit BC is read
/*  
  mechanical.vars.U[0]= feenox_define_variable_get_ptr("U");
  mechanical.vars.U[1]= feenox_define_variable_get_ptr("V");
  mechanical.vars.U[2]= feenox_define_variable_get_ptr("W");
 */

///va+strain_energy+name strain_energy
///va+strain_energy+detail The strain energy stored in the solid, computed as
///va+strain_energy+detail $1/2 \cdot \vec{u}^T  K \vec{u}$
///va+strain_energy+detail where $\vec{u}$ is the displacements vector and $K$ is the stiffness matrix.
  mechanical.strain_energy = feenox_define_variable_get_ptr("strain_energy");

  ///va+displ_max+name displ_max
///va+displ_max+detail The module of the maximum displacement of the elastic problem.
  mechanical.displ_max = feenox_define_variable_get_ptr("displ_max");

///va+displ_max_x+name displ_max_x
///va+displ_max_x+detail The\ $x$ coordinate of the maximum displacement of the elastic problem.
  mechanical.displ_max_x = feenox_define_variable_get_ptr("displ_max_x");
///va+displ_max_y+name displ_max_y
///va+displ_max_y+detail The\ $y$ coordinate of the maximum displacement of the elastic problem.
  mechanical.displ_max_y = feenox_define_variable_get_ptr("displ_max_y");
///va+displ_max_z+name displ_max_z
///va+displ_max_z+detail The\ $z$ coordinate of the maximum displacement of the elastic problem.
  mechanical.displ_max_z = feenox_define_variable_get_ptr("displ_max_z");

///va+u_at_displ_max+name u_at_displ_max
///va+u_at_displ_max+detail The\ $x$ component\ $u$ of the maximum displacement of the elastic problem.
  mechanical.u_at_displ_max = feenox_define_variable_get_ptr("u_at_displ_max");
///va+v_at_displ_max+name v_at_displ_max
///va+v_at_displ_max+detail The\ $y$ component\ $v$ of the maximum displacement of the elastic problem.
  mechanical.v_at_displ_max = feenox_define_variable_get_ptr("v_at_displ_max");
///va+w_at_displ_max+name w_at_displ_max
///va+w_at_displ_max+detail The\ $z$ component\ $w$ of the maximum displacement of the elastic problem.
  mechanical.w_at_displ_max = feenox_define_variable_get_ptr("w_at_displ_max");
  
///va+sigma_max+name sigma_max
///va+sigma_max+detail The maximum von Mises stress\ $\sigma$ of the elastic problem.
  mechanical.sigma_max = feenox_define_variable_get_ptr("sigma_max");

///va+delta_sigma_max+name delta_sigma_max
///va+delta_sigma_max+detail The uncertainty of the maximum Von Mises stress\ $\sigma$ of the elastic problem.
///va+delta_sigma_max+detail Not to be confused with the maximum uncertainty of the Von Mises stress.
  mechanical.delta_sigma_max = feenox_define_variable_get_ptr("delta_sigma_max");
  
///va+sigma_max_x+name sigma_max_x
///va+sigma_max_x+detail The\ $x$ coordinate of the maximum von Mises stress\ $\sigma$ of the elastic problem.
  mechanical.sigma_max_x = feenox_define_variable_get_ptr("sigma_max_x");
///va+sigma_max_y+name sigma_max_y
///va+sigma_max_y+detail The\ $x$ coordinate of the maximum von Mises stress\ $\sigma$ of the elastic problem.
  mechanical.sigma_max_y = feenox_define_variable_get_ptr("sigma_max_y");
///va+sigma_max_z+name sigma_max_z
///va+sigma_max_z+detail The\ $x$ coordinate of the maximum von Mises stress\ $\sigma$ of the elastic problem.
  mechanical.sigma_max_z = feenox_define_variable_get_ptr("sigma_max_z");
  
///va+u_at_sigma_max+name u_at_sigma_max
///va+u_at_sigma_max+detail The\ $x$ component\ $u$ of the displacement where the maximum von Mises stress\ $\sigma$ of the elastic problem is located.
  mechanical.u_at_sigma_max = feenox_define_variable_get_ptr("u_at_sigma_max");
///va+v_at_sigma_max+name v_at_sigma_max
///va+v_at_sigma_max+detail The\ $y$ component\ $v$ of the displacement where the maximum von Mises stress\ $\sigma$ of the elastic problem is located.
  mechanical.v_at_sigma_max = feenox_define_variable_get_ptr("v_at_sigma_max");
///va+w_at_sigma_max+name w_at_sigma_max
///va+w_at_sigma_max+detail The\ $z$ component\ $w$ of the displacement where the maximum von Mises stress\ $\sigma$ of the elastic problem is located.
  mechanical.w_at_sigma_max = feenox_define_variable_get_ptr("w_at_sigma_max");
  
  return FEENOX_OK;
}


int feenox_problem_init_runtime_mechanical(void) {
  return FEENOX_OK;
}


#ifdef HAVE_PETSC
int feenox_problem_mechanical_compute_rigid_nullspace(MatNullSpace *nullspace) {
  
  Vec vec_coords;
  if (feenox.pde.K != NULL) {
    petsc_call(MatCreateVecs(feenox.pde.K, NULL, &vec_coords));
  } else {
    feenox_check_alloc(vec_coords = feenox_create_vector("coordinates"));
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
//    petsc_call(MatSetNearNullSpace(feenox.pde.K, feenox.pde.null_space));
//    petsc_call(MatSetNearNullSpace(feenox.pde.K_bc, feenox.pde.null_space));
  petsc_call(VecDestroy(&vec_coords));

  return FEENOX_OK;
}
    
#endif