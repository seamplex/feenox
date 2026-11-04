/*------------ -------------- -------- --- ----- ---   --       -            -
 *  FeenoX parser for thermal_hydraulics-specific keywords
 *
 *  Copyright (C) 2026 Jeremy Theler
 *
 *  This file is part of FeenoX.
 *
 *  FeenoX is free software: you can redistribute it and/or modify
 *  it under the terms "of the GNU General Public License as published by
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
#include "../../parser/parser.h"
#include "thermal_hydraulics.h"

///pb_thermal_hydraulics+NONE+description The `thermal_hydraulics` problem does not need any extra keyword to `PROBLEM`.
int feenox_problem_parse_problem_thermal_hydraulics(const char *token) {

  // TODO: FLUID keyword to choose the equation of state
  // TODO: one or two phases
  if (token != NULL) {
    feenox_push_error_message("undefined keyword '%s' for thermal_hydraulics PROBLEM", token);
    return FEENOX_ERROR;
  } else {
    // if token is NULL we have to do the parse-time initialization
    feenox_call(feenox_problem_parse_time_init_thermal_hydraulics());
  }
 
  return FEENOX_OK;
}


int feenox_problem_parse_write_post_thermal_hydraulics(mesh_write_t *mesh_write, const char *token) {

  if (strcmp(token, "all") == 0) {
    
    feenox_call(feenox_problem_parse_write_post_thermal_hydraulics(mesh_write, "p"));
    feenox_call(feenox_problem_parse_write_post_thermal_hydraulics(mesh_write, "T"));
    feenox_call(feenox_problem_parse_write_post_thermal_hydraulics(mesh_write, "rho"));
    feenox_call(feenox_problem_parse_write_post_thermal_hydraulics(mesh_write, "h"));
    feenox_call(feenox_problem_parse_write_post_thermal_hydraulics(mesh_write, "v"));
    feenox_call(feenox_problem_parse_write_post_thermal_hydraulics(mesh_write, "mdot"));
    
  } else if (strcmp(token, "p") == 0 || strcmp(token, "pressure") == 0) {
    feenox_call(feenox_add_post_field(mesh_write, 1, &feenox.pde.solution[0]->name, NULL, field_location_nodes));
  } else if (strcmp(token, "T") == 0 || strcmp(token, "temperature") == 0) {
    feenox_call(feenox_add_post_field(mesh_write, 1, &feenox.pde.solution[1]->name, NULL, field_location_nodes));
  } else if (strcmp(token, "rho") == 0 || strcmp(token, "density") == 0) {
    feenox_call(feenox_add_post_field(mesh_write, 1, &thermal_hydraulics.rho_function->name, NULL, field_location_nodes));
  } else if (strcmp(token, "h") == 0 || strcmp(token, "enthalpy") == 0) {
    feenox_call(feenox_add_post_field(mesh_write, 1, &thermal_hydraulics.h_function->name, NULL, field_location_nodes));
  } else if (strcmp(token, "v") == 0 || strcmp(token, "velocity") == 0) {
    feenox_call(feenox_add_post_field(mesh_write, 1, &thermal_hydraulics.v_function->name, NULL, field_location_nodes));
  } else if (strcmp(token, "mdot") == 0 || strcmp(token, "mass_flow") == 0) {
    feenox_call(feenox_add_post_field(mesh_write, 1, &thermal_hydraulics.mdot_function->name, NULL, field_location_nodes));
  } else {
    feenox_push_error_message("undefined keyword '%s' for thermal_hydraulics WRITE_RESULTS", token);
    return FEENOX_ERROR;
  }
 
  return FEENOX_OK;
}
