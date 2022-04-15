/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox routines to handle mechanical BCs
 *
 *  Copyright (C) 2021-2022 jeremy theler
 *
 *  This file is part of Feenox <https://www.seamplex.com/feenox>.
 *
 *  feenox is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Feenox is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Feenox.  If not, see <http://www.gnu.org/licenses/>.
 *------------------- ------------  ----    --------  --     -       -         -
 */
#include "feenox.h"
#include "mechanical.h"
extern feenox_t feenox;
extern mechanical_t mechanical;

int feenox_problem_bc_parse_mechanical(bc_data_t *bc_data, const char *lhs, const char *rhs) {
  
  // TODO: document BCs with triple comments
  if (strcmp(lhs, "fixed") == 0) {
    bc_data->type_phys = BC_TYPE_MECHANICAL_DISPLACEMENT;
    bc_data->type_math = bc_type_math_dirichlet;
    bc_data->dof = -1;
    
  } else if (strcmp(lhs, "u") == 0 || strcmp(lhs, "u_x") == 0) {
    bc_data->type_phys = BC_TYPE_MECHANICAL_DISPLACEMENT;
    bc_data->type_math = bc_type_math_dirichlet;
    bc_data->dof = 0;
    
  } else if (strcmp(lhs, "v") == 0 || strcmp(lhs, "u_y") == 0) {
    bc_data->type_phys = BC_TYPE_MECHANICAL_DISPLACEMENT;
    bc_data->type_math = bc_type_math_dirichlet;
    bc_data->dof = 1;
    if (feenox.pde.dofs < 1) {
      feenox_push_error_message("cannot set u (displacement in y) with DOFs < 2");
      return FEENOX_ERROR;
    }

  } else if (strcmp(lhs, "w") == 0 || strcmp(lhs, "u_z") == 0) {
    bc_data->type_phys = BC_TYPE_MECHANICAL_DISPLACEMENT;
    bc_data->type_math = bc_type_math_dirichlet;
    bc_data->dof = 2;
    if (feenox.pde.dofs < 2) {
      feenox_push_error_message("cannot set w (displacement in z) with DOFs < 3");
      return FEENOX_ERROR;
    }

  } else if (strcmp(lhs, "p") == 0 || strcmp(lhs, "compression") == 0 || strcmp(lhs, "pressure") == 0) {
    bc_data->type_phys = BC_TYPE_MECHANICAL_PRESSURE_COMPRESSION;
    bc_data->type_math = bc_type_math_neumann;
    bc_data->set = feenox_problem_bc_set_mechanical_compression;

  } else if (strcmp(lhs, "t") == 0 || strcmp(lhs, "tension") == 0) {
    bc_data->type_phys = BC_TYPE_MECHANICAL_PRESSURE_TENSION;
    bc_data->type_math = bc_type_math_neumann;
    bc_data->set = feenox_problem_bc_set_mechanical_tension;

  } else if (strcmp(lhs, "tx") == 0 || strcmp(lhs, "ty") == 0 || strcmp(lhs, "tz") == 0) {
    bc_data->type_phys = BC_TYPE_MECHANICAL_TRACTION;
    bc_data->type_math = bc_type_math_neumann;
    bc_data->set = feenox_problem_bc_set_mechanical_traction;
    if (strcmp(lhs, "tx") == 0) {
      bc_data->dof = 0;
    } else if (strcmp(lhs, "ty") == 0) {
      bc_data->dof = 1;
    } else if (strcmp(lhs, "tz") == 0) {
      bc_data->dof = 2;
    }

  } else if (strcmp(lhs, "Fx") == 0 || strcmp(lhs, "Fy") == 0 || strcmp(lhs, "Fz") == 0) {
    bc_data->type_phys = BC_TYPE_MECHANICAL_FORCE;
    bc_data->type_math = bc_type_math_neumann;
    bc_data->set = feenox_problem_bc_set_mechanical_force;
    if (strcmp(lhs, "Fx") == 0) {
      bc_data->dof = 0;
    } else if (strcmp(lhs, "Fy") == 0) {
      bc_data->dof = 1;
    } else if (strcmp(lhs, "Fz") == 0) {
      bc_data->dof = 2;
    }

  } else if (strcmp(lhs, "symmetry") == 0 || strcmp(lhs, "tangential") == 0) {
    bc_data->type_phys = BC_TYPE_MECHANICAL_TANGENTIAL_SYMMETRY;
    bc_data->type_math = bc_type_math_multifreedom;
    // TODO: x0, y0 and z0
    
  } else {
    feenox_push_error_message("unknown mechanical boundary condition '%s'", lhs);
    return FEENOX_ERROR;
  }
  
  if (rhs != NULL && strcmp(rhs, "0") != 0) {
    feenox_call(feenox_expression_parse(&bc_data->expr, rhs));
  }
  
  bc_data->space_dependent = feenox_expression_depends_on_space(bc_data->expr.variables);
  bc_data->nonlinear = feenox_expression_depends_on_function(bc_data->expr.functions, feenox.pde.solution[0]);
  if (feenox.pde.dofs > 1) {
    bc_data->nonlinear |= feenox_expression_depends_on_function(bc_data->expr.functions, feenox.pde.solution[1]);
    if (feenox.pde.dofs > 2) {
      bc_data->nonlinear |= feenox_expression_depends_on_function(bc_data->expr.functions, feenox.pde.solution[2]);
    }
  }

  if (bc_data->nonlinear && bc_data->type_phys == BC_TYPE_MECHANICAL_DISPLACEMENT) {
    feenox_push_error_message("essential boundary condition '%s' cannot depend on temperature", rhs);
    return FEENOX_ERROR;
  }
  
  return FEENOX_OK;
}

int feenox_problem_bc_set_mechanical_displacement(element_t *element, bc_data_t *bc_data, size_t node_global_index) {
  
#ifdef HAVE_PETSC
  
  if (bc_data->dof != -1) {
    
    // only one dof
    feenox_call(feenox_problem_dirichlet_add(feenox.pde.mesh->node[node_global_index].index_dof[bc_data->dof], feenox_expression_eval(&bc_data->expr)));
    
  } else {
    
    // -1 means all dofs
    feenox_call(feenox_problem_dirichlet_add(feenox.pde.mesh->node[node_global_index].index_dof[0], 0));
    if (feenox.pde.dofs > 1) {
      feenox_call(feenox_problem_dirichlet_add(feenox.pde.mesh->node[node_global_index].index_dof[1], 0));
      if (feenox.pde.dofs > 2) {
        feenox_call(feenox_problem_dirichlet_add(feenox.pde.mesh->node[node_global_index].index_dof[2], 0));
      }
    }
    
  }  
  
#endif
  
  return FEENOX_OK;
}

int feenox_problem_bc_set_mechanical_multifreedom(element_t *element, bc_data_t *bc_data, size_t node_global_index) {
  
#ifdef HAVE_PETSC
  
  
  if (bc_data->type_phys == BC_TYPE_MECHANICAL_TANGENTIAL_SYMMETRY) {
    // TODO: check if we can get away with a regular dirichlet BC
    
    // outward normal (smoothed over all elements on the physical group of the BC)
    PetscScalar n_element[3] = {0, 0, 0};
    PetscScalar n_nodal[3] = {0, 0, 0};
    element_ll_t *element_item = NULL;
    LL_FOREACH(feenox.pde.mesh->node[node_global_index].element_list, element_item) {
      element_t *e = element_item->element;
      if (e != NULL && e->type->dim == (feenox.pde.dim-1) && e->physical_group == element->physical_group) {
        feenox_call(feenox_mesh_compute_outward_normal(element_item->element, n_element));
        n_nodal[0] += n_element[0];
        n_nodal[1] += n_element[1];
        n_nodal[2] += n_element[2];
      }
    }
    double norm = gsl_hypot3(n_nodal[0], n_nodal[1], n_nodal[2]);
    if (PetscLikely(norm != 0)) {
      n_nodal[0] /= norm;
      n_nodal[1] /= norm;
      n_nodal[2] /= norm;
    } else {
      feenox_push_error_message("outward normal has zero norm");
      return FEENOX_ERROR;
    }
    
    n_nodal[0] = -1;
    n_nodal[1] = 0;
    n_nodal[2] = 0;
    
    feenox_call(feenox_problem_multifreedom_add(node_global_index, n_nodal));

  }  
  // TODO: radial & generic expression
  
#endif
  
  return FEENOX_OK;
}

int feenox_problem_bc_set_mechanical_tension(element_t *element, bc_data_t *bc_data, unsigned int v) {
  feenox_call(feenox_problem_bc_set_mechanical_normal_stress(element, bc_data, v, +1));
  return FEENOX_OK;
}

int feenox_problem_bc_set_mechanical_compression(element_t *element, bc_data_t *bc_data, unsigned int v) {
  feenox_call(feenox_problem_bc_set_mechanical_normal_stress(element, bc_data, v, -1));
  return FEENOX_OK;
}

// this virtual method builds the surface elemental matrix
int feenox_problem_bc_set_mechanical_normal_stress(element_t *element, bc_data_t *bc_data, unsigned int v, signed int sign) {

  // maybe this check can be made on the dimension of the physical entity at parse time
  if ((feenox.pde.dim - element->type->dim) != 1) {
    feenox_push_error_message("pressure BCs can only be applied to surfaces");
    return FEENOX_ERROR;
  }
  
#ifdef HAVE_PETSC


  // outward normal
  double n[3];
  feenox_call(feenox_mesh_compute_outward_normal(element, n));

  // TODO: cache if not space dependent
  if (bc_data->space_dependent) {
    feenox_mesh_compute_x_at_gauss(element, v, feenox.pde.mesh->integration);
    feenox_mesh_update_coord_vars(element->x[v]);
  }
  double p = feenox_expression_eval(&bc_data->expr);
  
  // remember that here p > 0 means compression
  double t[3];
  for (unsigned int g = 0; g < feenox.pde.dim; g++) {
    t[g] = sign * p * n[g];
  }
  
  feenox_call(feenox_problem_bc_natural_set(element, v, t));
  
#endif
  
  return FEENOX_OK;
}



int feenox_problem_bc_set_mechanical_traction(element_t *element, bc_data_t *bc_data, unsigned int v) {

#ifdef HAVE_PETSC
  // TODO: cache if not space dependent
  // TODO: have different functions, one for space and one for constant?
  if (bc_data->space_dependent) {
    feenox_mesh_compute_x_at_gauss(element, v, feenox.pde.mesh->integration);
    feenox_mesh_update_coord_vars(element->x[v]);
  }
  // TODO: set all the DOFs at the same time
  double t[3] = {0,0,0};
  // TODO: wrap feenox_expression_eval() with vitrual methods according to the dependence of the bc
  t[bc_data->dof] = feenox_expression_eval(&bc_data->expr);
//  printf("%g\n", t[bc_data->dof]);
  feenox_call(feenox_problem_bc_natural_set(element, v, t));
  
#endif
  
  return FEENOX_OK;
}


int feenox_problem_bc_set_mechanical_force(element_t *element, bc_data_t *bc_data, unsigned int v) {

#ifdef HAVE_PETSC
  // TODO: cache if not space dependent
  if (bc_data->space_dependent) {
    feenox_mesh_compute_x_at_gauss(element, v, feenox.pde.mesh->integration);
    feenox_mesh_update_coord_vars(element->x[v]);
  }
  // TODO: set all the DOFs at the same time
  double t[3] = {0,0,0};
  if (element->physical_group->volume == 0) {
    feenox_physical_group_compute_volume(element->physical_group, feenox.pde.mesh);
  }
  t[bc_data->dof] = feenox_expression_eval(&bc_data->expr) / element->physical_group->volume;
//  printf("%g\n", t[bc_data->dof]);
  feenox_call(feenox_problem_bc_natural_set(element, v, t));
  
#endif
  
  return FEENOX_OK;
}
