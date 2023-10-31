/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for neutron diffusion FEM: boundary conditions
 *
 *  Copyright (C) 2021--2023 jeremy theler
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
#include "neutron_diffusion.h"
#include "pdes/neutron_sn/neutron_sn.h"

int feenox_problem_bc_parse_neutron_diffusion(bc_data_t *bc_data, const char *lhs, char *rhs) {

  if (strcmp(lhs, "null") == 0) {
    bc_data->type_math = bc_type_math_dirichlet;
    bc_data->dof = -1;
    bc_data->set_essential = feenox_problem_bc_set_neutron_diffusion_null;

  } else if (strcmp(lhs, "symmetry") == 0 || strcmp(lhs, "mirror") == 0) {
    bc_data->type_math = bc_type_math_neumann;
    bc_data->dof = -1;

  } else if (strcmp(lhs, "vacuum") == 0) {
    bc_data->type_math = bc_type_math_robin;
    
    bc_data->set_natural = feenox_problem_bc_set_neutron_diffusion_vacuum;
    bc_data->fills_matrix = 1;
    bc_data->dof = -1;

  } else if (strncmp(lhs, "phi", 3) == 0) {
    bc_data->type_math = bc_type_math_dirichlet;
    int dof = 0;
    if (sscanf(lhs+3, "%d", &dof) != 1) {
      feenox_push_error_message("could not read group number in neutron_diffusion boundary condition '%s'", lhs);
      return FEENOX_ERROR;
    }
    if (dof < 0 || dof > neutron_diffusion.groups) {
      feenox_push_error_message("invalid group number in neutron_diffusion boundary condition '%s'", lhs);
      return FEENOX_ERROR;
    }
    bc_data->dof = dof-1;
    bc_data->set_essential = feenox_problem_bc_set_neutron_diffusion_flux;

  } else if (strncmp(lhs, "J", 1) == 0) {
    bc_data->type_math = bc_type_math_neumann;
    int dof = 0;
    if (sscanf(lhs+1, "%d", &dof) != 1) {
      feenox_push_error_message("could not read group number in neutron_diffusion boundary condition '%s'", lhs);
      return FEENOX_ERROR;
    }
    if (dof < 0 || dof > neutron_diffusion.groups) {
      feenox_push_error_message("invalid group number in neutron_diffusion boundary condition '%s'", lhs);
      return FEENOX_ERROR;
    }
    bc_data->dof = dof-1;
    bc_data->set_natural = feenox_problem_bc_set_neutron_diffusion_current;
    neutron_diffusion.has_sources = 1;

    
  } else {
    feenox_push_error_message("unknown neutron_diffusion boundary condition '%s'", lhs);
    return FEENOX_ERROR;
  }

  feenox_call(feenox_expression_parse(&bc_data->expr, rhs));

  // TODO: initial guesses
  // if (bc_data->type_phys == BC_TYPE_THERMAL_TEMPERATURE || bc_data->type_phys == BC_TYPE_THERMAL_CONVECTION_TEMPERATURE ) {
  //   thermal.guessed_initial_guess += feenox_expression_eval(&bc_data->expr);
  //   thermal.n_bc_temperatures++;
  // }

  bc_data->space_dependent = feenox_expression_depends_on_space(bc_data->expr.variables);
  bc_data->nonlinear = feenox_expression_depends_on_function(bc_data->expr.functions, feenox.pde.solution[0]);


  if (bc_data->nonlinear && bc_data->type_math == bc_type_math_dirichlet) {
    feenox_push_error_message("essential boundary condition '%s' cannot depend on flux", rhs);
    return FEENOX_ERROR;
  }



  // TODO: check consistency, non-linearities, etc.
  
  return FEENOX_OK;
}


// this virtual method fills in the dirichlet indexes and values with bc_data
int feenox_problem_bc_set_neutron_diffusion_null(bc_data_t *this, element_t *e, size_t j_global) {

#ifdef HAVE_PETSC
  for (unsigned int g = 0; g < feenox.pde.dofs; g++) {
    feenox_call(feenox_problem_dirichlet_add(feenox.pde.mesh->node[j_global].index_dof[g], 0));
  }
#endif
  
  return FEENOX_OK;
}


// this virtual method fills in the dirichlet indexes and values with bc_data
int feenox_problem_bc_set_neutron_diffusion_flux(bc_data_t *this, element_t *e, size_t j_global) {

#ifdef HAVE_PETSC
  feenox_call(feenox_problem_dirichlet_add(feenox.pde.mesh->node[j_global].index_dof[this->dof], feenox_expression_eval(&this->expr)));
#endif
  return FEENOX_OK;
}


// this virtual method builds the surface elemental matrix
int feenox_problem_bc_set_neutron_diffusion_vacuum(bc_data_t *this, element_t *e, unsigned int q) {
  
#ifdef HAVE_PETSC

  feenox_fem_compute_x_at_gauss_if_needed_and_update_var(e, q, feenox.pde.mesh->integration, this->space_dependent);
  double coeff = (this->expr.items != NULL) ? feenox_expression_eval(&this->expr) : 0.5;
  
  // TODO: convenience call like  feenox_problem_rhs_set()?
  double wdet = feenox_fem_compute_w_det_at_gauss_integration(e, q, feenox.pde.mesh->integration);
  gsl_matrix *H = feenox_fem_compute_H_Gc_at_gauss(e, q, feenox.pde.mesh->integration);
  feenox_call(feenox_blas_BtB_accum(H, wdet*coeff, feenox.fem.Ki));
  
#endif
  
  return FEENOX_OK;
}



int feenox_problem_bc_set_neutron_diffusion_current(bc_data_t *this, element_t *e, unsigned int q) {
  
#ifdef HAVE_PETSC
  
  feenox_fem_compute_x_at_gauss_if_needed_and_update_var(e, q, feenox.pde.mesh->integration, this->space_dependent);
  double *current = calloc(neutron_sn.groups, sizeof(double)); 
  current[this->dof] = feenox_expression_eval(&this->expr);
  feenox_call(feenox_problem_rhs_add(e, q, current));
  feenox_free(current);
  
#endif
  
  return FEENOX_OK;
}
