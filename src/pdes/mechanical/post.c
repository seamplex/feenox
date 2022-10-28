/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox mechanical post-processing routines
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

int feenox_problem_solve_post_mechanical(void) {

  int uses_displ_max = (mechanical.displ_max->used ||
                   mechanical.displ_max_x->used ||
                   mechanical.displ_max_y->used ||
                   mechanical.displ_max_z->used ||
                   mechanical.u_at_displ_max->used ||
                   mechanical.v_at_displ_max->used ||
                   mechanical.w_at_displ_max->used);
  int uses_sigma_max = (mechanical.sigma_max->used ||
                   mechanical.sigma_max_x->used ||
                   mechanical.sigma_max_y->used ||
                   mechanical.sigma_max_z->used ||
                   mechanical.u_at_sigma_max->used ||
                   mechanical.v_at_sigma_max->used ||
                   mechanical.w_at_sigma_max->used);
  if (uses_displ_max == 0 && uses_sigma_max == 0) {
    return FEENOX_OK;
  }
  
  double displ2 = 0;
  double max_displ2 = 0;
  double sigma_max = 0;
  
  for (size_t j = 0; j < feenox.pde.mesh->n_nodes; j++) {
    
    if (uses_displ_max) {
      displ2 = 0;
      for (unsigned int g = 0; g < feenox.pde.dofs; g++) {
        displ2 += gsl_pow_2(feenox.pde.solution[g]->data_value[j]);
      }
      if (displ2 >= max_displ2) {
        max_displ2 = displ2;

        feenox_var_value(mechanical.displ_max) = sqrt(displ2);
        feenox_var_value(mechanical.displ_max_x) = feenox.pde.solution[0]->data_argument[0][j];
        feenox_var_value(mechanical.displ_max_y) = feenox.pde.solution[0]->data_argument[1][j];
        if (feenox.pde.dim == 3) {
          feenox_var_value(mechanical.displ_max_z) = feenox.pde.solution[0]->data_argument[2][j];
        }

        feenox_var_value(mechanical.u_at_displ_max) = feenox.pde.solution[0]->data_value[j];
        feenox_var_value(mechanical.v_at_displ_max) = feenox.pde.solution[1]->data_value[j];
        if (feenox.pde.dim == 3) {
          feenox_var_value(mechanical.w_at_displ_max) = feenox.pde.solution[2]->data_value[j];
        }
      }
    }
    
    if (uses_sigma_max) {
      if (mechanical.sigma->data_value[j] > sigma_max) {
        feenox_var_value(mechanical.sigma_max) = mechanical.sigma->data_value[j];
        // TODO: not cache friendly!
        feenox_var_value(mechanical.sigma_max_x) = feenox.pde.solution[0]->data_argument[0][j];
        feenox_var_value(mechanical.sigma_max_y) = feenox.pde.solution[0]->data_argument[1][j];
        if (feenox.pde.dim == 3) {
          feenox_var_value(mechanical.sigma_max_z) = feenox.pde.solution[0]->data_argument[2][j];
        }  

        feenox_var_value(mechanical.u_at_sigma_max) = feenox.pde.solution[0]->data_value[j];
        feenox_var_value(mechanical.v_at_sigma_max) = feenox.pde.solution[1]->data_value[j];
        if (feenox.pde.dim == 3) {
          feenox_var_value(mechanical.w_at_sigma_max) = feenox.pde.solution[2]->data_value[j];
        }
      }
    }
    
  }  
  
  return FEENOX_OK;
}
