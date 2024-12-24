
/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for neutron transport FEM: post
 *
 *  Copyright (C) 2023 Jeremy Theler
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

int feenox_problem_solve_post_neutron_sn(void) {

#ifdef HAVE_PETSC  
  if (neutron_sn.has_sources == 0) {
#ifdef HAVE_SLEPC
    if (feenox.pde.eigen_formulation == eigen_formulation_omega) {
      feenox_var_value(neutron_sn.keff) = 1.0/feenox.pde.eigenvalue[0];
    } else {
      feenox_var_value(neutron_sn.keff) = feenox.pde.eigenvalue[0];
    }
    
    // TODO: this is wrong
    // normalization without power
    double num = 0;
    double den = 0;
    int integration = feenox.pde.mesh->integration;
    for (size_t i = 0; i < feenox.pde.mesh->n_elements; i++) {
      element_t *element = &feenox.pde.mesh->element[i]; 
      if (element->type != NULL && element->type->dim == feenox.pde.dim) {
        num += element->type->volume(element);

        // for now I'd rather perform the computation myself instead of 
        // calling feenox_mesh_integral_over_element() because that wastes a lot of fem calls
        for (unsigned int q = 0; q < element->type->gauss[integration].Q; q++) {
          double wdet = feenox_fem_compute_w_det_at_gauss_integration(element, q, integration);
          for (unsigned int j = 0; j < element->type->nodes; j++) {
            double h = gsl_matrix_get(element->type->gauss[integration].H_c[q], 0, j);
            for (unsigned int g = 0; g < feenox.pde.dofs; g++) {
              den += wdet * h * feenox_vector_get(feenox.pde.solution[g]->vector_value, element->node[j]->index_mesh);
            }
          }
        }
      }
    }  
    
    double factor = num/den;
  
    // normalize the fluxes
    for (size_t j = 0; j < feenox.pde.mesh->n_nodes; j++) {
      for (unsigned int g = 0; g < feenox.pde.dofs; g++) {
        double xi = feenox_vector_get(feenox.pde.solution[g]->vector_value, j);
        feenox_vector_set(feenox.pde.solution[g]->vector_value, j, factor*xi);
      }
    }  
#endif
  }
  
  
  // compute the scalar fluxes out of the directional fluxes
  for (unsigned int g = 0; g < neutron_sn.groups; g++) {
    feenox_problem_fill_aux_solution(neutron_sn.phi[g]);
  }
  
  for (size_t j = 0; j < feenox.pde.mesh->n_nodes; j++) {
    for (unsigned int g = 0; g < neutron_sn.groups; g++) {
      double xi = 0;
      for (unsigned int m = 0; m < neutron_sn.directions; m++) {
        xi += neutron_sn.w[m] * feenox_vector_get(feenox.pde.solution[sn_dof_index(m,g)]->vector_value, j);
      }
      // TODO: wrapper, the pde has to set the function value not the vector
      feenox_vector_set(neutron_sn.phi[g]->vector_value, j, xi);
    }
  }  
  
  
#endif

  return FEENOX_OK;
}
