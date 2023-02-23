/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for the heat equation: post
 *
 *  Copyright (C) 2021 jeremy theler
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
#include "thermal.h"

// TODO: move to heatflux

int feenox_problem_solve_post_thermal(void) {

  if (thermal.T_max->used == 0 && thermal.T_min->used == 0) {
    return FEENOX_OK;
  }
  
  
  double T = 0;
  feenox_var_value(thermal.T_max) = -INFTY;
  feenox_var_value(thermal.T_min) = +INFTY;
  
  size_t j = 0;
  for (j = 0; j < feenox.pde.mesh->n_nodes; j++) {
    T = feenox.pde.solution[0]->data_value[j];
    if (T > feenox_var_value(thermal.T_max)) {
      feenox_var_value(thermal.T_max) = T;
    }
    if (T < feenox_var_value(thermal.T_min)) {
      feenox_var_value(thermal.T_min) = T;
    }
  }  

  return FEENOX_OK;
}
