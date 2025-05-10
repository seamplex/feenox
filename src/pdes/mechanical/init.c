/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox elastic mechanical initialization routines
 *
 *  Copyright (C) 2021--2025 Jeremy Theler
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
#include "mechanical.h"
mechanical_t mechanical;

int feenox_problem_parse_time_init_mechanical(void) {
#ifdef HAVE_PETSC
  // virtual methods
  feenox.pde.parse_bc = feenox_problem_bc_parse_mechanical;
  feenox.pde.parse_write_results = feenox_problem_parse_write_post_mechanical;

  feenox.pde.init_before_run = feenox_problem_init_runtime_mechanical;

  feenox.pde.setup_snes = feenox_problem_setup_snes_mechanical;
  feenox.pde.setup_ksp = feenox_problem_setup_ksp_mechanical;
  feenox.pde.setup_pc = feenox_problem_setup_pc_mechanical;
  
  feenox.pde.element_build_allocate_aux = feenox_problem_build_allocate_aux_mechanical;
  feenox.pde.element_build_volumetric_at_gauss = feenox_problem_build_volumetric_gauss_point_mechanical;
  
  feenox.pde.solve_post = feenox_problem_solve_post_mechanical;
  feenox.pde.gradient_fill = feenox_problem_gradient_fill_mechanical;
  feenox.pde.gradient_nodal_properties = feenox_problem_gradient_properties_at_element_nodes_mechanical;
  feenox.pde.gradient_alloc_nodal_fluxes = feenox_problem_gradient_fluxes_at_node_alloc_mechanical;
  feenox.pde.gradient_add_elemental_contribution_to_node = feenox_problem_gradient_add_elemental_contribution_to_node_mechanical;
  feenox.pde.gradient_fill_fluxes = feenox_problem_gradient_fill_fluxes_mechanical;
  
  // we are FEM
  feenox.mesh.default_field_location = field_location_nodes;
  
  // move symmetry_axis which is a general PDE setting to
  // the mechanically-particular axisymmetric variant
  if (feenox.pde.symmetry_axis != symmetry_axis_none) {
      mechanical.variant = variant_axisymmetric;
  }
  
  // check consistency of problem type and dimensions
  if (mechanical.variant == variant_axisymmetric ||
      mechanical.variant == variant_plane_stress ||
      mechanical.variant == variant_plane_strain) {

    if (feenox.pde.dim != 0) {
      if (feenox.pde.dim != 2) {
        feenox_push_error_message("dimension inconsistency, expected 2 dimensions not %d", feenox.pde.dim);
        return FEENOX_ERROR;
      }
    } else {
      feenox.pde.dim = 2;
    }

    if (feenox.pde.dofs != 0) {
      if (feenox.pde.dofs != 2) {
        feenox_push_error_message("DOF inconsistency, expected DOFs per node = 2");
        return FEENOX_ERROR;
      }
    } else {
      feenox.pde.dofs = 2;
    }
    
  } else {
    if (feenox.pde.dim == 0) {
      // default is 3d
      feenox.pde.dim = 3;
    } else if (feenox.pde.dim == 1)   {
      feenox_push_error_message("cannot solve 1D mechanical problems");
      return FEENOX_ERROR;
    } else if (feenox.pde.dim == 2)   {
      feenox_push_error_message("to solve 2D problems give either plane_stress, plane_strain or axisymmetric");
      return FEENOX_ERROR;
    } else if (feenox.pde.dim != 3) {
      feenox_push_error_message("dimension inconsistency, expected DIM 2 or 3 instead of %d", feenox.pde.dim);
      return FEENOX_ERROR;
    }
    feenox.pde.dofs = feenox.pde.dim;
  }
  
  // TODO: custom names
  // TODO: document
  feenox_check_alloc(feenox.pde.unknown_name = calloc(feenox.pde.dofs, sizeof(char *)));
  feenox_check_alloc(feenox.pde.unknown_name[0] = strdup("u"));
  if (feenox.pde.dofs > 1) {
    feenox_check_alloc(feenox.pde.unknown_name[1] = strdup("v"));
    if (feenox.pde.dofs > 2) {
      feenox_check_alloc(feenox.pde.unknown_name[2] = strdup("w"));
    }  
  }
  
  // ------- elasticity-related outputs -----------------------------------  
  // TODO: document
  feenox_call(feenox_problem_define_solution_function("exx", &mechanical.exx, FEENOX_SOLUTION_GRADIENT));
  feenox_call(feenox_problem_define_solution_function("eyy", &mechanical.eyy, FEENOX_SOLUTION_GRADIENT));
  feenox_call(feenox_problem_define_solution_function("exy", &mechanical.exy, FEENOX_SOLUTION_GRADIENT));

  if (feenox.pde.dofs == 3) {
    feenox_call(feenox_problem_define_solution_function("ezz", &mechanical.ezz, FEENOX_SOLUTION_GRADIENT));
    feenox_call(feenox_problem_define_solution_function("eyz", &mechanical.eyz, FEENOX_SOLUTION_GRADIENT));
    feenox_call(feenox_problem_define_solution_function("ezx", &mechanical.ezx, FEENOX_SOLUTION_GRADIENT));
  }

  feenox_call(feenox_problem_define_solution_function("sigmax", &mechanical.sigmax, FEENOX_SOLUTION_GRADIENT));
  feenox_call(feenox_problem_define_solution_function("sigmay", &mechanical.sigmay, FEENOX_SOLUTION_GRADIENT));
  feenox_call(feenox_problem_define_solution_function("tauxy", &mechanical.tauxy, FEENOX_SOLUTION_GRADIENT));

  if (feenox.pde.dofs == 3) {
    feenox_call(feenox_problem_define_solution_function("sigmaz", &mechanical.sigmaz, FEENOX_SOLUTION_GRADIENT));
    feenox_call(feenox_problem_define_solution_function("tauyz", &mechanical.tauyz, FEENOX_SOLUTION_GRADIENT));
    feenox_call(feenox_problem_define_solution_function("tauzx", &mechanical.tauzx, FEENOX_SOLUTION_GRADIENT));
  }

  feenox_call(feenox_problem_define_solution_function("sigma1", &mechanical.sigma1, FEENOX_SOLUTION_GRADIENT));
  feenox_call(feenox_problem_define_solution_function("sigma2", &mechanical.sigma2, FEENOX_SOLUTION_GRADIENT));
  feenox_call(feenox_problem_define_solution_function("sigma3", &mechanical.sigma3, FEENOX_SOLUTION_GRADIENT));
  feenox_call(feenox_problem_define_solution_function("sigma", &mechanical.sigma, FEENOX_SOLUTION_GRADIENT));
//  feenox_call(feenox_problem_define_solution_function("delta_sigma", &mechanical.delta_sigma, FEENOX_SOLUTION_GRADIENT));
  feenox_call(feenox_problem_define_solution_function("tresca", &mechanical.tresca, FEENOX_SOLUTION_GRADIENT));

///va+ldef+detail Flag that turns the large-deformation formulation on or off.
///va+ldef+detail It can also be turned on with the `NONLINEAR` keyword in `PROBLEM`
///va+ldef+detail or with the `--non-linear` command-line option.
  feenox_check_alloc(mechanical.ldef = feenox_define_variable_get_ptr("ldef"));

///va+ldef+detail Flag that asks FeenoX to check if the deformations are large after
///va+ldef+detail solving a linear problem. If they are, a warning is issued.
  feenox_check_alloc(mechanical.ldef_check = feenox_define_variable_get_ptr("ldef_check"));
  
// these are for the algebraic expressions in the  implicitly-defined BCs
// i.e. 0=u*nx+v*ny or 0=u*y-v*x
// here they are defined as uppercase because there already exist functions named u, v and w
// but the parser changes their case when an implicit BC is read
  mechanical.displ_for_bc[0]= feenox_define_variable_get_ptr("U");
  mechanical.displ_for_bc[1]= feenox_define_variable_get_ptr("V");
  mechanical.displ_for_bc[2]= feenox_define_variable_get_ptr("W");

  ///va+strain_energy+detail The strain energy stored in the solid, computed as
///va+strain_energy+detail $1/2 \cdot \vec{u}^T  K \vec{u}$
///va+strain_energy+detail where $\vec{u}$ is the displacements vector and $K$ is the stiffness matrix.
  feenox_check_alloc(mechanical.strain_energy = feenox_define_variable_get_ptr("strain_energy"));

///va+displ_max+detail The module of the maximum displacement of the elastic problem.
  feenox_check_alloc(mechanical.displ_max = feenox_define_variable_get_ptr("displ_max"));

///va+displ_max_x+detail The\ $x$ coordinate of the maximum displacement of the elastic problem.
  feenox_check_alloc(mechanical.displ_max_x = feenox_define_variable_get_ptr("displ_max_x"));
///va+displ_max_y+detail The\ $y$ coordinate of the maximum displacement of the elastic problem.
  feenox_check_alloc(mechanical.displ_max_y = feenox_define_variable_get_ptr("displ_max_y"));
///va+displ_max_z+detail The\ $z$ coordinate of the maximum displacement of the elastic problem.
  feenox_check_alloc(mechanical.displ_max_z = feenox_define_variable_get_ptr("displ_max_z"));

///va+u_at_displ_max+detail The\ $x$ component\ $u$ of the maximum displacement of the elastic problem.
  feenox_check_alloc(mechanical.u_at_displ_max = feenox_define_variable_get_ptr("u_at_displ_max"));
///va+v_at_displ_max+detail The\ $y$ component\ $v$ of the maximum displacement of the elastic problem.
  feenox_check_alloc(mechanical.v_at_displ_max = feenox_define_variable_get_ptr("v_at_displ_max"));
///va+w_at_displ_max+detail The\ $z$ component\ $w$ of the maximum displacement of the elastic problem.
  feenox_check_alloc(mechanical.w_at_displ_max = feenox_define_variable_get_ptr("w_at_displ_max"));
  
///va+sigma_max+detail The maximum von Mises stress\ $\sigma$ of the elastic problem.
  feenox_check_alloc(mechanical.sigma_max = feenox_define_variable_get_ptr("sigma_max"));

///va+delta_sigma_max+detail The uncertainty of the maximum Von Mises stress\ $\sigma$ of the elastic problem.
///va+delta_sigma_max+detail Not to be confused with the maximum uncertainty of the Von Mises stress.
  feenox_check_alloc(mechanical.delta_sigma_max = feenox_define_variable_get_ptr("delta_sigma_max"));
  
///va+sigma_max_x+detail The\ $x$ coordinate of the maximum von Mises stress\ $\sigma$ of the elastic problem.
  feenox_check_alloc(mechanical.sigma_max_x = feenox_define_variable_get_ptr("sigma_max_x"));
///va+sigma_max_y+detail The\ $x$ coordinate of the maximum von Mises stress\ $\sigma$ of the elastic problem.
  feenox_check_alloc(mechanical.sigma_max_y = feenox_define_variable_get_ptr("sigma_max_y"));
///va+sigma_max_z+detail The\ $x$ coordinate of the maximum von Mises stress\ $\sigma$ of the elastic problem.
  feenox_check_alloc(mechanical.sigma_max_z = feenox_define_variable_get_ptr("sigma_max_z"));
  
///va+u_at_sigma_max+detail The\ $x$ component\ $u$ of the displacement where the maximum von Mises stress\ $\sigma$ of the elastic problem is located.
  feenox_check_alloc(mechanical.u_at_sigma_max = feenox_define_variable_get_ptr("u_at_sigma_max"));
///va+v_at_sigma_max+detail The\ $y$ component\ $v$ of the displacement where the maximum von Mises stress\ $\sigma$ of the elastic problem is located.
  feenox_check_alloc(mechanical.v_at_sigma_max = feenox_define_variable_get_ptr("v_at_sigma_max"));
///va+w_at_sigma_max+detail The\ $z$ component\ $w$ of the displacement where the maximum von Mises stress\ $\sigma$ of the elastic problem is located.
  feenox_check_alloc(mechanical.w_at_sigma_max = feenox_define_variable_get_ptr("w_at_sigma_max"));
#endif  
  return FEENOX_OK;
}


int feenox_problem_init_runtime_mechanical(void) {

#ifdef HAVE_PETSC 
  feenox.pde.mesh->data_type = data_type_node;
  feenox.pde.spatial_unknowns = feenox.pde.mesh->n_nodes;

  // initialize distributions
  
  // first see if we have linear elastic
  feenox_call(feenox_distribution_init(&mechanical.E, "E"));  
  feenox_call(feenox_distribution_init(&mechanical.nu, "nu"));  
  
  // TODO: allow different volumes to have different material models
  if (mechanical.E.defined && mechanical.nu.defined) {
    if (mechanical.E.full == 0) {
      feenox_push_error_message("Young modulus 'E' is not defined over all volumes");
      return FEENOX_ERROR;
    }
    if (mechanical.nu.full == 0) {
      feenox_push_error_message("Poisson’s ratio 'nu' is not defined over all volumes");
      return FEENOX_ERROR;
    }
    mechanical.material_model = material_model_elastic_isotropic;
  } else if (mechanical.E.defined) {
    feenox_push_error_message("Young modulus 'E' defined but Poisson’s ratio 'nu' not defined");
    return FEENOX_ERROR;
  } else if (mechanical.nu.defined) {
    feenox_push_error_message("Poisson’s ratio 'nu' defined but Young modulus 'E' not defined");
    return FEENOX_ERROR;
  }
  
  // see if there are orthotropic properties
  feenox_call(feenox_distribution_init(&mechanical.E_x, "Ex"));
  if (mechanical.E_x.defined == 0) {
    feenox_call(feenox_distribution_init(&mechanical.E_x, "E_x"));
  }
  feenox_call(feenox_distribution_init(&mechanical.E_y, "Ey"));
  if (mechanical.E_y.defined == 0) {
    feenox_call(feenox_distribution_init(&mechanical.E_y, "E_y"));
  }
  feenox_call(feenox_distribution_init(&mechanical.E_z, "Ez"));
  if (mechanical.E_z.defined == 0) {
    feenox_call(feenox_distribution_init(&mechanical.E_z, "E_z"));
  }

  feenox_call(feenox_distribution_init(&mechanical.nu_xy, "nuxy"));
  if (mechanical.nu_xy.defined == 0) {
    feenox_call(feenox_distribution_init(&mechanical.nu_xy, "nu_xy"));
  }
  feenox_call(feenox_distribution_init(&mechanical.nu_yz, "nuyz"));
  if (mechanical.nu_yz.defined == 0) {
    feenox_call(feenox_distribution_init(&mechanical.nu_yz, "nu_yz"));
  }
  feenox_call(feenox_distribution_init(&mechanical.nu_zx, "nuzx"));
  if (mechanical.nu_zx.defined == 0) {
    feenox_call(feenox_distribution_init(&mechanical.nu_zx, "nu_zx"));
  }
  
  feenox_call(feenox_distribution_init(&mechanical.G_xy, "Gxy"));
  if (mechanical.G_xy.defined == 0) {
    feenox_call(feenox_distribution_init(&mechanical.G_xy, "G_xy"));
  }
  feenox_call(feenox_distribution_init(&mechanical.G_yz, "Gyz"));
  if (mechanical.G_yz.defined == 0) {
    feenox_call(feenox_distribution_init(&mechanical.G_yz, "G_yz"));
  }
  feenox_call(feenox_distribution_init(&mechanical.G_zx, "Gzx"));
  if (mechanical.G_zx.defined == 0) {
    feenox_call(feenox_distribution_init(&mechanical.G_zx, "G_zx"));
  }
  
  // check for consistency
  int n_ortho = mechanical.E_x.defined   + mechanical.E_y.defined   + mechanical.E_z.defined   +
                mechanical.nu_xy.defined + mechanical.nu_yz.defined + mechanical.nu_zx.defined +
                mechanical.G_xy.defined  + mechanical.G_yz.defined  + mechanical.G_zx.defined;
  
  if (n_ortho > 0) {
    if (mechanical.material_model == material_model_elastic_isotropic) {
      feenox_push_error_message("both isotropic and orthotropic properties given, choose one");
      return FEENOX_ERROR;
    } else if (n_ortho < 9) {
      feenox_push_error_message("%d orthotropic properties missing", 9-n_ortho);
      return FEENOX_ERROR;
    } else if (mechanical.material_model == material_model_unknown) {
      mechanical.material_model = material_model_elastic_orthotropic;
    }
  }
  
  // thermal expansion model
  // first try isotropic
  feenox_call(feenox_distribution_init(&mechanical.alpha, "alpha"));
  if (mechanical.alpha.defined) {
    mechanical.thermal_expansion_model = thermal_expansion_model_isotropic;
  } else {
    // see if there are orthotropic properties
    feenox_call(feenox_distribution_init(&mechanical.alpha_x, "alphax"));
    if (mechanical.alpha_x.defined == 0) {
      feenox_call(feenox_distribution_init(&mechanical.alpha_x, "alpha_x"));
    }
    feenox_call(feenox_distribution_init(&mechanical.alpha_y, "alphay"));
    if (mechanical.alpha_y.defined == 0) {
      feenox_call(feenox_distribution_init(&mechanical.alpha_y, "alpha_y"));
    }
    feenox_call(feenox_distribution_init(&mechanical.alpha_z, "alphaz"));
    if (mechanical.alpha_z.defined == 0) {
      feenox_call(feenox_distribution_init(&mechanical.alpha_z, "alpha_z"));
    }
  }
  
  // check for consistency
  n_ortho = mechanical.alpha_x.defined + mechanical.alpha_y.defined + mechanical.alpha_z.defined;
  
  if (n_ortho > 0) {
    if (mechanical.thermal_expansion_model == thermal_expansion_model_isotropic) {
      feenox_push_error_message("both isotropic and orthotropic thermal expansion coefficients given, choose one");
      return FEENOX_ERROR;
    } else if (n_ortho < 3) {
      feenox_push_error_message("%d orthotropic thermal expansion coefficients missing", 3-n_ortho);
      return FEENOX_ERROR;
    } else if (mechanical.thermal_expansion_model == thermal_expansion_model_none) {
      mechanical.thermal_expansion_model = thermal_expansion_model_orthotropic;
    }
  }
    
  // temperature used for the thermal expansion
  feenox_call(feenox_distribution_init(&mechanical.T, "T"));

  // reference temperature: it has to be a variable
  feenox_call(feenox_distribution_init(&mechanical.T_ref, "T0"));
  if (mechanical.T_ref.defined == 0) {
    feenox_call(feenox_distribution_init(&mechanical.T_ref, "T_0"));
  }
  if (mechanical.T_ref.defined) {
    if (mechanical.T_ref.non_uniform) {
      feenox_push_error_message("reference temperature T0 has to be uniform");
      return FEENOX_ERROR;
    }
    // TODO: it is hard to know if a variable will be constant in time...
/*    
    if (mechanical.T_ref.constant == 0) {
      feenox_push_error_message("reference temperature T0 has to be constant");
      return FEENOX_ERROR;
    }
 */
    // evaluate reference temperature and store it in T0
    mechanical.T0 = mechanical.T_ref.eval(&mechanical.T_ref, NULL, NULL);
  }
  
  
  // volumetric force densities
  feenox_call(feenox_distribution_init(&mechanical.f_x, "fx"));
  if (mechanical.f_x.defined == 0) {
    feenox_call(feenox_distribution_init(&mechanical.f_x, "f_x"));
  }
  feenox_call(feenox_distribution_init(&mechanical.f_y, "fy"));
  if (mechanical.f_y.defined == 0) {
    feenox_call(feenox_distribution_init(&mechanical.f_y, "f_y"));
  }
  feenox_call(feenox_distribution_init(&mechanical.f_z, "fz"));
  if (mechanical.f_z.defined == 0) {
    feenox_call(feenox_distribution_init(&mechanical.f_z, "f_z"));
  }
  
  

  // set material model virtual methods
  switch (mechanical.material_model) {
  
    case material_model_elastic_isotropic:
      mechanical.uniform_C = (mechanical.E.non_uniform == 0 && mechanical.nu.non_uniform == 0);
      if (mechanical.variant == variant_full) {
      
        mechanical.compute_C = feenox_problem_mechanical_compute_C_elastic_isotropic;
        mechanical.compute_stress_from_strain = mechanical.uniform_C ? feenox_stress_from_strain : feenox_stress_from_strain_elastic_isotropic;
      
      } else if (mechanical.variant == variant_plane_stress) {      
      
        mechanical.compute_C = feenox_problem_mechanical_compute_C_elastic_plane_stress;  
        mechanical.compute_stress_from_strain = feenox_stress_from_strain_elastic_isotropic;
      
      } else if (mechanical.variant == variant_plane_strain) {  
      
        mechanical.compute_C = feenox_problem_mechanical_compute_C_elastic_plane_strain;
        mechanical.compute_stress_from_strain = feenox_stress_from_strain_elastic_isotropic;
      
      }  
    break;
    
    case material_model_elastic_orthotropic:
      
      if (mechanical.variant != variant_full) {
        feenox_push_error_message("elastic orthotropic materials cannot be used in plane stress/strain");
        return FEENOX_ERROR;
      }

      mechanical.compute_C = feenox_problem_mechanical_compute_C_elastic_orthotropic;
      mechanical.compute_stress_from_strain = feenox_stress_from_strain;
    
    break;
    
    default:
      feenox_push_error_message("unknown material model, usual way to go is to define E and nu");
      return FEENOX_ERROR;
    break;
  }

  // size of stress-strain matrix
  if (mechanical.variant == variant_full) {
    mechanical.stress_strain_size = 6;
  } else if (mechanical.variant == variant_axisymmetric) {
    mechanical.stress_strain_size = 4;
  } else if (mechanical.variant == variant_plane_stress || mechanical.variant == variant_plane_strain) {
    mechanical.stress_strain_size = 3;
  } else {
    feenox_push_error_message("internal mismatch, unknown variant");
    return FEENOX_ERROR;
  }
  
  // allocate stress-strain objects
  feenox_check_alloc(mechanical.C = gsl_matrix_calloc(mechanical.stress_strain_size, mechanical.stress_strain_size));
  if (mechanical.uniform_C) {
    // cache properties
    feenox_call(mechanical.compute_C(NULL, NULL));
  } 

  switch (mechanical.thermal_expansion_model) {
    case thermal_expansion_model_isotropic:
      mechanical.compute_thermal_strain = feenox_problem_mechanical_compute_thermal_strain_isotropic;
      mechanical.compute_thermal_stress = feenox_problem_mechanical_compute_thermal_stress_isotropic;
    break;
    case thermal_expansion_model_orthotropic:
      mechanical.compute_thermal_strain = feenox_problem_mechanical_compute_thermal_strain_orthotropic;
      mechanical.compute_thermal_stress = feenox_problem_mechanical_compute_thermal_stress_orthotropic;
    break;
    default:
    break;
  }

  if (mechanical.thermal_expansion_model != thermal_expansion_model_none) {
    feenox_check_alloc(mechanical.et = gsl_vector_calloc(mechanical.stress_strain_size));
    feenox_check_alloc(mechanical.Cet = gsl_vector_calloc(mechanical.stress_strain_size));
  }
  
  if (feenox.pde.math_type == math_type_automatic) {
    if (feenox_var_value(mechanical.ldef) == 0) {
      feenox.pde.math_type = math_type_linear;
      feenox_var_value(mechanical.ldef_check) = 1;
    } else {
      feenox.pde.math_type = math_type_nonlinear;
    }
  }
  
  if (feenox.pde.math_type == math_type_linear) {
    feenox.pde.solve = feenox_problem_solve_petsc_linear;
  } else if (feenox.pde.math_type == math_type_nonlinear) {
    feenox.pde.solve = feenox_problem_solve_petsc_nonlinear;
    feenox.pde.element_build_volumetric_at_gauss = feenox_problem_build_volumetric_gauss_point_mechanical_nonlinear;
    feenox.pde.has_internal_fluxes = 1;
    feenox_var_value(mechanical.ldef) = 1;
    feenox_var_value(mechanical.ldef_check) = 0;
  } else {
    feenox_push_error_message("unknown math problem type %d", feenox.pde.math_type);
  }
  
  feenox.pde.has_stiffness = 1;
  // TODO: transient
  feenox.pde.has_mass = 0;
  feenox.pde.has_rhs = 1;
  
  feenox.pde.has_jacobian_K = feenox.pde.math_type == math_type_nonlinear;
  feenox.pde.has_jacobian_M = 0;
  feenox.pde.has_jacobian_b = 0;
  feenox.pde.has_jacobian = feenox.pde.has_jacobian_K || feenox.pde.has_jacobian_M || feenox.pde.has_jacobian_b;
  
  feenox.pde.symmetric_K = 1;
  feenox.pde.symmetric_M = 1;

  // see if we have to compute gradients
  feenox.pde.compute_gradients |= 
                                  (mechanical.exx    != NULL && mechanical.exx->used) ||
                                  (mechanical.eyy    != NULL && mechanical.eyy->used) ||
                                  (mechanical.ezz    != NULL && mechanical.ezz->used) ||
                                  (mechanical.exy    != NULL && mechanical.exy->used) ||
                                  (mechanical.eyz    != NULL && mechanical.eyz->used) ||
                                  (mechanical.ezx    != NULL && mechanical.ezx->used) ||
                                  (mechanical.sigmax != NULL && mechanical.sigmax->used) ||
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
      

#endif
  return FEENOX_OK;
}


#ifdef HAVE_PETSC
int feenox_problem_compute_rigid_nullspace(MatNullSpace *nullspace) {
  
  Vec vec_coords = NULL;
  if (feenox.pde.K != NULL) {
    petsc_call(MatCreateVecs(feenox.pde.K, NULL, &vec_coords));
  } else {
    feenox_check_alloc(vec_coords = feenox_problem_create_vector("coordinates"));
  }  
  petsc_call(VecSetBlockSize(vec_coords, feenox.pde.dim));
  petsc_call(VecSetUp(vec_coords));

  PetscScalar *coords = NULL;
  petsc_call(VecGetArray(vec_coords, &coords));
  for (size_t j = feenox.pde.first_node; j < feenox.pde.last_node; j++) {          
    for (unsigned int d = 0; d < feenox.pde.dim; d++) {
      coords[feenox.pde.mesh->node[j].index_dof[d] - feenox.pde.first_row] = feenox.pde.mesh->node[j].x[d];
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
    petsc_call(KSPSetType(ksp, (feenox.pde.symmetric_K && feenox.pde.symmetric_M) ? KSPCG : KSPGMRES));
  }  

  return FEENOX_OK;
}

int feenox_problem_setup_snes_mechanical(SNES snes) {

  // TODO! set a default snes type
//  SNESType snes_type = NULL;
//  petsc_call(SNESGetType(snes, &snes_type));
  
  SNESLineSearch ls;
  petsc_call(SNESGetLineSearch(snes, &ls));
  
  SNESLineSearchType ls_type;
  petsc_call(SNESLineSearchGetType(ls, &ls_type));
  
  if (ls_type == NULL) {
    petsc_call(SNESLineSearchSetType(ls, SNESLINESEARCHBASIC));
  }
  return FEENOX_OK;
}

#endif
