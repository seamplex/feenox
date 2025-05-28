/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox saint venant-kirchoff mechanical material
 *
 *  Copyright (C) 2025 Jeremy Theler
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
#include "../mechanical.h"

int feenox_mechanical_material_init_svk(material_t *material, int i) {
  
  if (mechanical.variant != variant_full) {
    feenox_push_error_message("hyperelastic SVK materials cannot be used in plane stress/strain");
    return FEENOX_ERROR;
  }
  
  // TO-DO: lambda and mu
  return (mechanical.E.defined_per_group[i] && mechanical.nu.defined_per_group[i]) ? material_model_hyperelastic_svk : material_model_unknown;
}

int feenox_mechanical_material_setup_svk(void) {
  mechanical.uniform_properties = (mechanical.E.non_uniform == 0 && mechanical.nu.non_uniform == 0);
  mechanical.compute_material_tangent = feenox_problem_mechanical_compute_tangent_matrix_C_linear_elastic;
  mechanical.compute_PK2 = feenox_problem_build_mechanical_stress_measure_linear_elastic;
  mechanical.compute_stress_from_strain = feenox_stress_from_strain_linear;
  mechanical.nonlinear_material = 1;
  
  return FEENOX_OK;
}