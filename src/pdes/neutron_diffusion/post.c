
/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for neutron diffusion FEM: post
 *
 *  Copyright (C) 2021 Jeremy Theler
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

int feenox_problem_solve_post_neutron_diffusion(void) {

#ifdef HAVE_PETSC  
  if (neutron_diffusion.has_sources == 0) {
#ifdef HAVE_SLEPC
    if (feenox.pde.eigen_formulation == eigen_formulation_omega) {
      feenox_var_value(neutron_diffusion.keff) = 1.0/feenox.pde.eigenvalue[0];
    } else {
      feenox_var_value(neutron_diffusion.keff) = feenox.pde.eigenvalue[0];
    }
    
    // normalization without power
    double num = 0;
    double den = 0;
    for (size_t i = 0; i < feenox.pde.mesh->n_elements; i++) {
      element_t *element = &feenox.pde.mesh->element[i]; 
      if (element->type != NULL && element->type->dim == feenox.pde.dim) {
        num += element->type->volume(element);
        for (unsigned int g = 0; g < feenox.pde.dofs; g++) {
          den += feenox_mesh_integral_over_element(element, feenox.pde.mesh, feenox.pde.solution[g]);
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
#endif

  return FEENOX_OK;
}
