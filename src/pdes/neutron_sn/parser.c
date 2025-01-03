/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for neutron transport FEM: input parsing
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
#include "../../parser/parser.h"

int feenox_problem_parse_problem_neutron_sn(const char *token) {
  
  if (token != NULL) {
    if (strcasecmp(token, "GROUPS") == 0) {
      double xi = 0;
      feenox_call(feenox_parser_expression_in_string(&xi));
      neutron_sn.groups = (unsigned int)(xi);
      
    } else if (strcasecmp(token, "SN") == 0 || strcasecmp(token, "N") == 0) {
      double xi = 0;
      feenox_call(feenox_parser_expression_in_string(&xi));
      neutron_sn.N = (unsigned int)(xi);
      if (neutron_sn.N % 2 == 1) {
        feenox_push_error_message("The N in SN should be even and %d is not (as far as I know)", neutron_sn.N);
        return FEENOX_ERROR;
      }
    } else {
      feenox_push_error_message("undefined keyword '%s'", token);
      return FEENOX_ERROR;
    }
  } else {
    // if token is NULL we have to do the parse-time initialization
    feenox_call(feenox_problem_parse_time_init_neutron_sn());
  }  
  return FEENOX_OK;
}  



int feenox_problem_parse_write_post_neutron_sn(mesh_write_t *mesh_write, const char *token) {

  if (strcmp(token, "all") == 0) {
    feenox_call(feenox_problem_parse_write_post_neutron_sn(mesh_write, "scalar_fluxes"));
    feenox_call(feenox_problem_parse_write_post_neutron_sn(mesh_write, "angular_fluxes"));
    
  } else if (strcmp(token, "scalar_flux") == 0 || strcmp(token, "scalar_fluxes") == 0) {
    
    for (unsigned int g = 0; g < neutron_sn.groups; g++) {
      feenox_call(feenox_add_post_field(mesh_write, 1, &neutron_sn.phi[g]->name, NULL, field_location_nodes));
    }
    
  } else if (strcmp(token, "angular_flux") == 0 || strcmp(token, "angular_fluxes") == 0) {

    for (unsigned int g = 0; g < neutron_sn.groups; g++) {
      for (unsigned int m = 0; m < neutron_sn.directions; m++) {
        feenox_call(feenox_add_post_field(mesh_write, 1, &feenox.pde.unknown_name[sn_dof_index(m,g)], NULL, field_location_nodes));
      }
    }
    
    
  } else {
    feenox_push_error_message("undefined keyword '%s' for neutron_sn WRITE_RESULTS", token);
    return FEENOX_ERROR;
  }
  
  return FEENOX_OK;
}

