/*------------ -------------- -------- --- ----- ---   --       -            -
 *  FeenoX parser for thermal-specific keywords
 *
 *  Copyright (C) 2023 jeremy theler
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
#include "thermal.h"

int feenox_problem_parse_problem_thermal(const char *token) {

///kw_pde+PROBLEM+detail  * `thermal` solves the heat conduction problem.

  // no need to parse anything;
  if (token != NULL) {
    feenox_push_error_message("undefined keyword '%s'", token);
    return FEENOX_ERROR;
  } else {
    // if token is NULL we have to do the parse-time initialization
    feenox_call(feenox_problem_parse_time_init_thermal());
  }
 
  return FEENOX_OK;
}


int feenox_problem_parse_write_post_thermal(mesh_write_t *mesh_write, const char *token) {

  if (strcmp(token, "all") == 0) {
    
    feenox_call(feenox_problem_parse_write_post_thermal(mesh_write, "temperature"));
    feenox_call(feenox_problem_parse_write_post_thermal(mesh_write, "heat_flux"));
    
  } else if (strcmp(token, "temperature") == 0 || strcmp(token, "T") == 0) {
    
    feenox_call(feenox_add_post_field(mesh_write, 1, &feenox.pde.solution[0]->name, NULL, field_location_nodes));
    
  } else if (strcmp(token, "heat_flux") == 0 || strcmp(token, "heat_fluxes") == 0) {
    
    char *tokens[3] = {NULL, NULL, NULL};
    tokens[0] = strdup((0 < feenox.pde.dim) ? thermal.qx->name : "0");
    tokens[1] = strdup((1 < feenox.pde.dim) ? thermal.qy->name : "0");
    tokens[2] = strdup((2 < feenox.pde.dim) ? thermal.qz->name : "0");
    feenox_call(feenox_add_post_field(mesh_write, 3, tokens, "heat_flux", field_location_nodes));
    
    for (unsigned int m = 0; m < 3; m++) {
      feenox_free(tokens[m]);
    }
    
  } else {
    feenox_push_error_message("undefined keyword '%s' for thermal WRITE_RESULTS", token);
    return FEENOX_ERROR;
  }
 
  return FEENOX_OK;
}
  