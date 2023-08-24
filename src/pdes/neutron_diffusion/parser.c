/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for neutron diffusion FEM: input parsing
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
#include "neutron_diffusion.h"
#include "../../parser/parser.h"

int feenox_problem_parse_problem_neutron_diffusion(const char *token) {
  
  if (token != NULL) {
    if (strcasecmp(token, "GROUPS") == 0) {
      double xi = 0;
      feenox_call(feenox_parser_expression_in_string(&xi));
      neutron_diffusion.groups = (unsigned int)(xi);
      
    } else {
      feenox_push_error_message("undefined keyword '%s'", token);
      return FEENOX_ERROR;
    }
  } else {
    // if token is NULL we have to do the parse-time initialization
    feenox_call(feenox_problem_parse_time_init_neutron_diffusion());
  }  
  return FEENOX_OK;
}  

int feenox_problem_parse_write_post_neutron_diffusion(mesh_write_t *mesh_write, const char *token) {

  if (strcmp(token, "all") == 0) {
    feenox_call(feenox_problem_parse_write_post_neutron_diffusion(mesh_write, "fluxes"));
    
  } else if (strcmp(token, "flux") == 0 || strcmp(token, "fluxes") == 0) {

    for (int g = 0; g < neutron_diffusion.groups; g++) {
      feenox_call(feenox_add_post_field(mesh_write, 1, &feenox.pde.unknown_name[g], NULL, field_location_nodes));
    }
  } else {
    feenox_push_error_message("undefined keyword '%s' for neutron_diffusion WRITE_RESULTS", token);
    return FEENOX_ERROR;
  }
  
  return FEENOX_OK;
}
