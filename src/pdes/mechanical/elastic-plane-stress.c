/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox elastic isotropic plane-stress mechanical material
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

int feenox_problem_build_compute_mechanical_C_elastic_plane_stress(const double *x, material_t *material) {
  
  double E = mechanical.E.eval(&mechanical.E, x, material);
  double nu = mechanical.nu.eval(&mechanical.nu, x, material);
  
  double c1 = E/(1-nu*nu);
  double c2 = nu * c1;
  
  feenox_lowlevel_matrix_set(mechanical.C, 0, 0, c1);
  feenox_lowlevel_matrix_set(mechanical.C, 0, 1, c2);
    
  feenox_lowlevel_matrix_set(mechanical.C, 1, 0, c2);
  feenox_lowlevel_matrix_set(mechanical.C, 1, 1, c1);

  feenox_lowlevel_matrix_set(mechanical.C, 2, 2, c1*0.5*(1-nu));
    
  return FEENOX_OK;
}
