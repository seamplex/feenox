/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines to handle mechanical BCs
 *
 *  Copyright (C) 2021-2025 Jeremy Theler
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

int feenox_problem_bc_parse_mechanical(bc_data_t *bc_data, const char *lhs, char *rhs) {
///bc_mechanical+fixed+usage fixed
///bc_mechanical+fixed+desc Fix every displacement component to zero.
///bc_mechanical+u+usage u=<expr>
///bc_mechanical+u+desc Prescribe the $x$ displacement (Dirichlet).
///bc_mechanical+u+detail `u_x` is an alias. Use `u=0` to constrain the component to zero.
///bc_mechanical+u_x+usage u_x=<expr>
///bc_mechanical+u_x+desc Alias for `u`.
  if (strcmp(lhs, "fixed") == 0) {
    bc_data->type_phys = BC_TYPE_MECHANICAL_DISPLACEMENT;
    bc_data->type_math = bc_type_math_dirichlet;
    bc_data->set_essential = feenox_problem_bc_set_mechanical_displacement;
    bc_data->dof = -1;
    
  } else if ((strcmp(lhs, "u") == 0 || strcmp(lhs, "u_x") == 0) && strcmp(rhs, "") != 0) {
    bc_data->type_phys = BC_TYPE_MECHANICAL_DISPLACEMENT;
    bc_data->type_math = bc_type_math_dirichlet;
    bc_data->set_essential = feenox_problem_bc_set_mechanical_displacement;
    bc_data->dof = 0;
    
///bc_mechanical+v+usage v=<expr>
///bc_mechanical+v+desc Prescribe the $y$ displacement (Dirichlet).
///bc_mechanical+v+detail `u_y` is an alias. This component is available only in two- or three-dimensional problems.
///bc_mechanical+u_y+usage u_y=<expr>
///bc_mechanical+u_y+desc Alias for `v`.
  } else if ((strcmp(lhs, "v") == 0 || strcmp(lhs, "u_y") == 0) && strcmp(rhs, "") != 0) {
    bc_data->type_phys = BC_TYPE_MECHANICAL_DISPLACEMENT;
    bc_data->type_math = bc_type_math_dirichlet;
    bc_data->set_essential = feenox_problem_bc_set_mechanical_displacement;
    bc_data->dof = 1;
    if (feenox.pde.dofs < 1) {
      feenox_push_error_message("cannot set u (displacement in y) with DOFs < 2");
      return FEENOX_ERROR;
    }

///bc_mechanical+w+usage w=<expr>
///bc_mechanical+w+desc Prescribe the $z$ displacement (Dirichlet).
///bc_mechanical+w+detail `u_z` is an alias. This component is available only in three-dimensional problems.
///bc_mechanical+u_z+usage u_z=<expr>
///bc_mechanical+u_z+desc Alias for `w`.
  } else if ((strcmp(lhs, "w") == 0 || strcmp(lhs, "u_z") == 0) && strcmp(rhs, "") != 0) {
    bc_data->type_phys = BC_TYPE_MECHANICAL_DISPLACEMENT;
    bc_data->type_math = bc_type_math_dirichlet;
    bc_data->set_essential = feenox_problem_bc_set_mechanical_displacement;
    bc_data->dof = 2;
    if (feenox.pde.dofs < 2) {
      feenox_push_error_message("cannot set w (displacement in z) with DOFs < 3");
      return FEENOX_ERROR;
    }

///bc_mechanical+p+usage p=<expr>
///bc_mechanical+p+desc Apply a normal compressive pressure to a surface.
///bc_mechanical+p+detail Positive values act opposite to the outward normal. `compression` and `pressure` are aliases. Pressure is applied as force per unit area in consistent units.
///bc_mechanical+compression+usage compression=<expr>
///bc_mechanical+compression+desc Alias for `p`.
///bc_mechanical+pressure+usage pressure=<expr>
///bc_mechanical+pressure+desc Alias for `p`.
  } else if (strcmp(lhs, "p") == 0 || strcmp(lhs, "compression") == 0 || strcmp(lhs, "pressure") == 0) {
    bc_data->type_phys = BC_TYPE_MECHANICAL_PRESSURE_COMPRESSION;
    bc_data->type_math = bc_type_math_neumann;
    bc_data->set_natural = feenox_problem_bc_set_mechanical_compression;

///bc_mechanical+t+usage t=<expr>
///bc_mechanical+t+desc Apply a normal tensile traction to a surface.
///bc_mechanical+t+detail Positive values act in the outward-normal direction. `tension` is an alias.
///bc_mechanical+tension+usage tension=<expr>
///bc_mechanical+tension+desc Alias for `t`.
  } else if (strcmp(lhs, "t") == 0 || strcmp(lhs, "tension") == 0) {
    bc_data->type_phys = BC_TYPE_MECHANICAL_PRESSURE_TENSION;
    bc_data->type_math = bc_type_math_neumann;
    bc_data->set_natural = feenox_problem_bc_set_mechanical_tension;

///bc_mechanical+tx+usage tx=<expr>
///bc_mechanical+tx+desc Prescribe a global $x$ traction component on a surface, in force per unit area.
///bc_mechanical+ty+usage ty=<expr>
///bc_mechanical+ty+desc Prescribe a global $y$ traction component on a surface, in force per unit area.
///bc_mechanical+tz+usage tz=<expr>
///bc_mechanical+tz+desc Prescribe a global $z$ traction component on a surface, in force per unit area.
  } else if (strcmp(lhs, "tx") == 0 || strcmp(lhs, "ty") == 0 || strcmp(lhs, "tz") == 0) {
    bc_data->type_phys = BC_TYPE_MECHANICAL_TRACTION;
    bc_data->type_math = bc_type_math_neumann;
    bc_data->set_natural = feenox_problem_bc_set_mechanical_traction;
    if (strcmp(lhs, "tx") == 0) {
      bc_data->dof = 0;
    } else if (strcmp(lhs, "ty") == 0) {
      bc_data->dof = 1;
    } else if (strcmp(lhs, "tz") == 0) {
      bc_data->dof = 2;
    }

///bc_mechanical+Fx+usage Fx=<expr>
///bc_mechanical+Fx+desc Apply a resultant force in the global $x$ direction.
///bc_mechanical+Fx+detail The resultant is distributed over the selected physical group by its measure. `Fy` and `Fz` apply the other Cartesian components.
///bc_mechanical+Fy+usage Fy=<expr>
///bc_mechanical+Fy+desc Apply a resultant force in the global $y$ direction.
///bc_mechanical+Fz+usage Fz=<expr>
///bc_mechanical+Fz+desc Apply a resultant force in the global $z$ direction.
  } else if (strcmp(lhs, "Fx") == 0 || strcmp(lhs, "Fy") == 0 || strcmp(lhs, "Fz") == 0) {
    bc_data->type_phys = BC_TYPE_MECHANICAL_FORCE;
    bc_data->type_math = bc_type_math_neumann;
    bc_data->set_natural = feenox_problem_bc_set_mechanical_force;
    if (strcmp(lhs, "Fx") == 0) {
      bc_data->dof = 0;
    } else if (strcmp(lhs, "Fy") == 0) {
      bc_data->dof = 1;
    } else if (strcmp(lhs, "Fz") == 0) {
      bc_data->dof = 2;
    }

///bc_mechanical+symmetry+usage symmetry
///bc_mechanical+symmetry+desc Constrain displacement normal to the selected boundary to zero.
///bc_mechanical+symmetry+detail The outward-normal displacement is removed while tangential motion remains free. `tangential` is an alias. For curved or non-axis-aligned boundaries this is a multi-freedom constraint.
///bc_mechanical+tangential+usage tangential
///bc_mechanical+tangential+desc Alias for `symmetry`: allow only displacement tangent to the boundary.
  } else if (strcmp(lhs, "symmetry") == 0 || strcmp(lhs, "tangential") == 0) {
    bc_data->type_phys = BC_TYPE_MECHANICAL_TANGENTIAL_SYMMETRY;
    bc_data->type_math = bc_type_math_multifreedom;
    bc_data->set_essential = feenox_problem_bc_set_mechanical_symmetry;
    
///bc_mechanical+radial+usage radial
///bc_mechanical+radial+desc Allow only displacement radial to the selected physical group's center of gravity.
///bc_mechanical+radial+detail Tangential displacement is constrained. Combine with `symmetry`/`tangential` only when both radial and tangential constraints are intended.
  } else if (strcmp(lhs, "radial") == 0) {
    bc_data->type_phys = BC_TYPE_MECHANICAL_RADIAL_SYMMETRY;
    bc_data->type_math = bc_type_math_multifreedom;
    bc_data->set_essential = feenox_problem_bc_set_mechanical_radial;
    // TODO: x0, y0 and z0
    
///bc_mechanical+0+usage 0=<expr(u,v,w,x,y,z)>
///bc_mechanical+0+desc Impose a homogeneous linear multi-freedom displacement constraint $g(u,v,w,x,y,z)=0$.
///bc_mechanical+0+detail FeenoX linearizes the supplied expression with respect to the displacement components. Non-homogeneous right-hand sides are not supported by this form.
  } else if (strcmp(lhs, "0") == 0) {
    bc_data->type_phys = BC_TYPE_MECHANICAL_MULTIDOF_EXPRESSION;
    bc_data->type_math = bc_type_math_multifreedom;
    bc_data->set_essential = feenox_problem_bc_set_mechanical_multifreedom;
    
    // trick: the idea is that the user might write an expression of space
    // x,y,z but also maybe of u,v y w. However, u,v,w are functinos and not variables!
    // what we do is to define variables named U,V,W and string-replace  u,v,w -> U,V,W
    // in the entered expression
    
    // TODO: there should be a separator (i.e. operator) before and after
    char *s = rhs;
    while (*s != '\0') {
      if (*s == 'u') {
        *s = 'U';
      } else if (*s == 'v') {
        *s = 'V';
      } else if (*s == 'w') {
        *s = 'W';
      }
      s++;
    }
    
  } else if (strcmp(rhs, "") != 0) {
    feenox_push_error_message("unknown mechanical boundary condition '%s'", lhs);
    return FEENOX_ERROR;
  }
  
  if (rhs != NULL && strcmp(rhs, "0") != 0) {
    feenox_call(feenox_expression_parse(&bc_data->expr, rhs));
  }
  
  bc_data->space_dependent = feenox_depends_on_space(bc_data->expr.variables);
  bc_data->nonlinear = feenox_depends_on_function(bc_data->expr.functions, feenox.pde.solution[0]);
  if (feenox.pde.dofs > 1) {
    bc_data->nonlinear |= feenox_depends_on_function(bc_data->expr.functions, feenox.pde.solution[1]);
    if (feenox.pde.dofs > 2) {
      bc_data->nonlinear |= feenox_depends_on_function(bc_data->expr.functions, feenox.pde.solution[2]);
    }
  }

  if (bc_data->nonlinear && bc_data->type_math == bc_type_math_dirichlet) {
    feenox_push_error_message("essential boundary condition '%s' cannot depend on the unknown", rhs);
    return FEENOX_ERROR;
  }
  
  return FEENOX_OK;
}

int feenox_problem_bc_set_mechanical_displacement(bc_data_t *this, element_t *e, size_t j_global) {
#ifdef HAVE_PETSC
  
  if (this->dof != -1) {
    
    // only one dof
    feenox_call(feenox_problem_dirichlet_add(j_global, this->dof, feenox_expression_eval(&this->expr)));
    
  } else {
    
    // -1 means all dofs (and the only possibility is to have all them equal to zero)
    feenox_call(feenox_problem_dirichlet_add(j_global, 0, 0));
    if (feenox.pde.dofs > 1) {
      feenox_call(feenox_problem_dirichlet_add(j_global, 1, 0));
      if (feenox.pde.dofs > 2) {
        feenox_call(feenox_problem_dirichlet_add(j_global, 2, 0));
      }
    }
    
  }  
  
#endif
  
  return FEENOX_OK;
}

int feenox_problem_bc_set_mechanical_symmetry(bc_data_t *this, element_t *e, size_t j_global) {
  
#ifdef HAVE_PETSC
  // outward normal (smoothed over all elements on the physical group of the BC)
  // TODO: choose to smooth or to use the local one
  PetscScalar normal[3] = {0, 0, 0};
  PetscScalar n_element[3] = {0, 0, 0};
  element_ll_t *element_item = NULL;
  LL_FOREACH(feenox.pde.mesh->node[j_global].element_list, element_item) {
    element_t *e_prime = element_item->element;
    if (e_prime != NULL && e_prime->type->dim == (feenox.pde.dim-1) && e_prime->physical_group == e->physical_group) {
      feenox_call(feenox_mesh_compute_outward_normal(element_item->element, n_element));
      normal[0] += n_element[0];
      normal[1] += n_element[1];
      normal[2] += n_element[2];
    }
  }

  int coordinate_direction = -1;
  double norm = gsl_hypot3(normal[0], normal[1], normal[2]);
  if (feenox_likely(norm != 0)) {
    // if the outward normal coincides with one of the three axes, we can get away with a regular dirichlet BC
    for (int g = 0; g < 3; g++) {
      normal[g] /= norm;
      if (fabs(normal[g]) > (1-1e-4)) {
        coordinate_direction = g;
      }  
    }
  } else {
    feenox_push_error_message("outward normal has zero norm");
    return FEENOX_ERROR;
  }

  // if the condition results in a direction normal to one of the three coordinate planes
  // then we set a traditional dirichlet bc (i.e. u=0 or v=0 or w=0)
  // otherwise we need a generic multifreedom
  if (coordinate_direction != -1) {
    feenox_call(feenox_problem_dirichlet_add(j_global, coordinate_direction, 0));
  } else {
    feenox_call(feenox_problem_multifreedom_add(j_global, normal));
  }  
#endif
    
  return FEENOX_OK;
}

int feenox_problem_bc_set_mechanical_radial(bc_data_t *this, element_t *e, size_t j_global) {
  
#ifdef HAVE_PETSC
  double x[3] = {0,0,0};
  double eps = 1e-2;

  if (e->physical_group->volume == 0) {
    feenox_call(feenox_physical_group_compute_volume(e->physical_group, feenox.pde.mesh));
  }

  // TODO! read center of the radial condition
  for (int g = 0; g < 3; g++) {
//      x[g] = feenox.pde.mesh->node[node_global_index].x[g] - ((bc_data->expr[g].items == NULL) ? element->physical_entity->cog[d] : feenox_expression_eval(bc_data->expr[g]));
    x[g] = feenox.pde.mesh->node[j_global].x[g] - e->physical_group->cog[g];
  }  

  double coefficients[3] = {0,0,0};

  // x-y
  if (fabs(x[0]) > eps && fabs(x[1]) > eps) {
    coefficients[0] = +x[1];
    coefficients[1] = -x[0];
    coefficients[2] = 0;
    feenox_call(feenox_problem_multifreedom_add(j_global, coefficients));
  }      

  // x-z
  if (fabs(x[0]) > eps && fabs(x[2]) > eps) {
    coefficients[0] = +x[2];
    coefficients[1] = 0;
    coefficients[2] = -x[0];
    feenox_call(feenox_problem_multifreedom_add(j_global, coefficients));
  }      

  // y-z
  if (fabs(x[1]) > eps && fabs(x[2]) > eps) {
    coefficients[0] = 0;
    coefficients[1] = +x[2];
    coefficients[2] = -x[1];
    feenox_call(feenox_problem_multifreedom_add(j_global, coefficients));
  }      
#endif

  return FEENOX_OK;
}


typedef struct {
  expr_t *expr;
  int dof;
} feenox_gsl_function_of_uvw_params_t;

int feenox_problem_bc_set_mechanical_multifreedom(bc_data_t *this, element_t *e, size_t j_global) {
  
#ifdef HAVE_PETSC
  feenox_gsl_function_of_uvw_params_t params = { &this->expr, -1 };
  gsl_function F = {feenox_mechanical_gsl_function_of_uvw, &params};

  double coefficients[3] = {0, 0, 0};

  // TODO: choose
  double h = 1e-5;
  double result = 0;
  double abserr = 0;
  for (int g = 0; g < 3; g++) {
    params.dof = g;
    gsl_deriv_central(&F, 0, h, &result, &abserr);
    coefficients[g] = -result;
  }  

  // TODO: non-homogeneous RHS
  feenox_call(feenox_problem_multifreedom_add(j_global, coefficients));
#endif
  
  return FEENOX_OK;
}

int feenox_problem_bc_set_mechanical_tension(bc_data_t *this, element_t *e, unsigned int q) {
  feenox_call(feenox_problem_bc_set_mechanical_normal_stress(this, e, q, +1));
  return FEENOX_OK;
}

int feenox_problem_bc_set_mechanical_compression(bc_data_t *this, element_t *e, unsigned int q) {
  feenox_call(feenox_problem_bc_set_mechanical_normal_stress(this, e, q, -1));
  return FEENOX_OK;
}

// this virtual method builds the surface elemental matrix
int feenox_problem_bc_set_mechanical_normal_stress(bc_data_t *this, element_t *e, unsigned int q, signed int sign) {

  // maybe this check can be made on the dimension of the physical entity at parse time
  if ((feenox.pde.dim - e->type->dim) != 1) {
    feenox_push_error_message("pressure BCs can only be applied to surfaces");
    return FEENOX_ERROR;
  }
  
#ifdef HAVE_PETSC


  // outward normal
  double n[3];
  feenox_call(feenox_mesh_compute_outward_normal(e, n));

  // TODO: cache if not space dependent
  feenox_fem_compute_x_at_gauss_if_needed_and_update_var(e, q, feenox.pde.mesh->integration, this->space_dependent);
  double p = feenox_expression_eval(&this->expr);
  
  // remember that here p > 0 means compression
  double t[3];
  for (unsigned int g = 0; g < feenox.pde.dim; g++) {
    t[g] = sign * p * n[g];
  }
  
  //feenox_call(feenox_problem_bc_natural_set(e, v, t));
  feenox_call(feenox_problem_rhs_add(e, q, t));
  
#endif
  
  return FEENOX_OK;
}



int feenox_problem_bc_set_mechanical_traction(bc_data_t *this, element_t *e, unsigned int q) {

#ifdef HAVE_PETSC
  // TODO: cache if not space dependent
  // TODO: have different functions, one for space and one for constant?
  feenox_fem_compute_x_at_gauss_if_needed_and_update_var(e, q, feenox.pde.mesh->integration, this->space_dependent);

  // TODO: set all the DOFs at the same time
  double t[3] = {0,0,0};
  // TODO: wrap feenox_expression_eval() with virtual methods according to the dependence of the bc
  t[this->dof] = feenox_expression_eval(&this->expr);
//  printf("%g\n", t[bc_data->dof]);
//  feenox_call(feenox_problem_bc_natural_set(e, v, t));
  
  feenox_call(feenox_problem_rhs_add(e, q, t));  
  
#endif
  
  return FEENOX_OK;
}


int feenox_problem_bc_set_mechanical_force(bc_data_t *this, element_t *e, unsigned int q) {

#ifdef HAVE_PETSC
  // TODO: cache if not space dependent
  feenox_fem_compute_x_at_gauss_if_needed_and_update_var(e, q, feenox.pde.mesh->integration, this->space_dependent);
  // TODO: set all the DOFs at the same time
  double t[3] = {0,0,0};
  if (e->physical_group->volume == 0) {
    feenox_call(feenox_physical_group_compute_volume(e->physical_group, feenox.pde.mesh));
  }
  t[this->dof] = feenox_expression_eval(&this->expr) / e->physical_group->volume;
  feenox_call(feenox_problem_rhs_add(e, q, t));  
  
#endif
  
  return FEENOX_OK;
}


// wrapper to compute derivatives with GSL
double feenox_mechanical_gsl_function_of_uvw(double x, void *params) {
  feenox_gsl_function_of_uvw_params_t *p = (feenox_gsl_function_of_uvw_params_t *)params;

  feenox_var_value(mechanical.displ_for_bc[0]) = 0;
  feenox_var_value(mechanical.displ_for_bc[1]) = 0;
  feenox_var_value(mechanical.displ_for_bc[2]) = 0;
  feenox_var_value(mechanical.displ_for_bc[p->dof]) = x;
  
  double y = feenox_expression_eval(p->expr);

  if (gsl_isnan(y) || gsl_isinf(y)) {
    feenox_nan_error();
  }

  return y;

}
