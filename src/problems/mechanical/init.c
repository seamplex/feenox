#include "feenox.h"
feenox_t feenox;

int feenox_problem_init_parser_mechanical(void) {
  
  if (feenox.pde.variant == variant_axisymmetric ||
      feenox.pde.variant == variant_plane_stress ||
      feenox.pde.variant == variant_plane_strain) {

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
  feenox_call(feenox_problem_define_solution_function("sigmax", &feenox.pde.sigmax));
  feenox_call(feenox_problem_define_solution_function("sigmay", &feenox.pde.sigmay));
  feenox_call(feenox_problem_define_solution_function("tauxy", &feenox.pde.tauxy));

  if (feenox.pde.dim == 3) {
    feenox_call(feenox_problem_define_solution_function("sigmaz", &feenox.pde.sigmaz));
    feenox_call(feenox_problem_define_solution_function("tauyz", &feenox.pde.tauyz));
    feenox_call(feenox_problem_define_solution_function("tauzx", &feenox.pde.tauzx));
  }

  feenox_call(feenox_problem_define_solution_function("sigma1", &feenox.pde.sigma1));
  feenox_call(feenox_problem_define_solution_function("sigma2", &feenox.pde.sigma2));
  feenox_call(feenox_problem_define_solution_function("sigma3", &feenox.pde.sigma3));
  feenox_call(feenox_problem_define_solution_function("sigma", &feenox.pde.sigma));
  feenox_call(feenox_problem_define_solution_function("delta_sigma", &feenox.pde.delta_sigma));
  feenox_call(feenox_problem_define_solution_function("tresca", &feenox.pde.tresca));
  


// these are for the algebraic expressions in the  which are implicitly-defined BCs
// i.e. 0=u*nx+v*ny
// here they are defined as uppercase because there already exist functions named u, v and w
// but the parser changes their case when an implicit BC is read
  feenox.pde.vars.U[0]= feenox_define_variable_get_ptr("U");
  feenox.pde.vars.U[1]= feenox_define_variable_get_ptr("V");
  feenox.pde.vars.U[2]= feenox_define_variable_get_ptr("W");

///va+strain_energy+name strain_energy
///va+strain_energy+detail The strain energy stored in the solid, computed as
///va+strain_energy+detail $1/2 \cdot \vec{u}^T  K \vec{u}$
///va+strain_energy+detail where $\vec{u}$ is the displacements vector and $K$ is the stiffness matrix.
  feenox.pde.vars.strain_energy = feenox_define_variable_get_ptr("strain_energy");

  ///va+displ_max+name displ_max
///va+displ_max+detail The module of the maximum displacement of the elastic problem.
  feenox.pde.vars.displ_max = feenox_define_variable_get_ptr("displ_max");

///va+displ_max_x+name displ_max_x
///va+displ_max_x+detail The\ $x$ coordinate of the maximum displacement of the elastic problem.
  feenox.pde.vars.displ_max_x = feenox_define_variable_get_ptr("displ_max_x");
///va+displ_max_y+name displ_max_y
///va+displ_max_y+detail The\ $y$ coordinate of the maximum displacement of the elastic problem.
  feenox.pde.vars.displ_max_y = feenox_define_variable_get_ptr("displ_max_y");
///va+displ_max_z+name displ_max_z
///va+displ_max_z+detail The\ $z$ coordinate of the maximum displacement of the elastic problem.
  feenox.pde.vars.displ_max_z = feenox_define_variable_get_ptr("displ_max_z");

///va+u_at_displ_max+name u_at_displ_max
///va+u_at_displ_max+detail The\ $x$ component\ $u$ of the maximum displacement of the elastic problem.
  feenox.pde.vars.u_at_displ_max = feenox_define_variable_get_ptr("u_at_displ_max");
///va+v_at_displ_max+name v_at_displ_max
///va+v_at_displ_max+detail The\ $y$ component\ $v$ of the maximum displacement of the elastic problem.
  feenox.pde.vars.v_at_displ_max = feenox_define_variable_get_ptr("v_at_displ_max");
///va+w_at_displ_max+name w_at_displ_max
///va+w_at_displ_max+detail The\ $z$ component\ $w$ of the maximum displacement of the elastic problem.
  feenox.pde.vars.w_at_displ_max = feenox_define_variable_get_ptr("w_at_displ_max");
  
///va+sigma_max+name sigma_max
///va+sigma_max+detail The maximum von Mises stress\ $\sigma$ of the elastic problem.
  feenox.pde.vars.sigma_max = feenox_define_variable_get_ptr("sigma_max");

///va+delta_sigma_max+name delta_sigma_max
///va+delta_sigma_max+detail The uncertainty of the maximum Von Mises stress\ $\sigma$ of the elastic problem.
///va+delta_sigma_max+detail Not to be confused with the maximum uncertainty of the Von Mises stress.
  feenox.pde.vars.delta_sigma_max = feenox_define_variable_get_ptr("delta_sigma_max");
  
///va+sigma_max_x+name sigma_max_x
///va+sigma_max_x+detail The\ $x$ coordinate of the maximum von Mises stress\ $\sigma$ of the elastic problem.
  feenox.pde.vars.sigma_max_x = feenox_define_variable_get_ptr("sigma_max_x");
///va+sigma_max_y+name sigma_max_y
///va+sigma_max_y+detail The\ $x$ coordinate of the maximum von Mises stress\ $\sigma$ of the elastic problem.
  feenox.pde.vars.sigma_max_y = feenox_define_variable_get_ptr("sigma_max_y");
///va+sigma_max_z+name sigma_max_z
///va+sigma_max_z+detail The\ $x$ coordinate of the maximum von Mises stress\ $\sigma$ of the elastic problem.
  feenox.pde.vars.sigma_max_z = feenox_define_variable_get_ptr("sigma_max_z");
  
///va+u_at_sigma_max+name u_at_sigma_max
///va+u_at_sigma_max+detail The\ $x$ component\ $u$ of the displacement where the maximum von Mises stress\ $\sigma$ of the elastic problem is located.
  feenox.pde.vars.u_at_sigma_max = feenox_define_variable_get_ptr("u_at_sigma_max");
///va+v_at_sigma_max+name v_at_sigma_max
///va+v_at_sigma_max+detail The\ $y$ component\ $v$ of the displacement where the maximum von Mises stress\ $\sigma$ of the elastic problem is located.
  feenox.pde.vars.v_at_sigma_max = feenox_define_variable_get_ptr("v_at_sigma_max");
///va+w_at_sigma_max+name w_at_sigma_max
///va+w_at_sigma_max+detail The\ $z$ component\ $w$ of the displacement where the maximum von Mises stress\ $\sigma$ of the elastic problem is located.
  feenox.pde.vars.w_at_sigma_max = feenox_define_variable_get_ptr("w_at_sigma_max");
  
  return FEENOX_OK;
}


int feenox_problem_init_runtime_mechanical(void) {
  return FEENOX_OK;
}
