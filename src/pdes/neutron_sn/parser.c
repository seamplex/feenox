/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for neutron transport FEM: input parsing
 *
 *  Copyright (C) 2023 jeremy theler
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
      if (neutron_sn.N != 2 &&
          neutron_sn.N != 4 &&
          neutron_sn.N != 6 &&
          neutron_sn.N != 8) {
        feenox_push_error_message("S%d is not supported, only S2, S4, S6 and S8 are", neutron_sn.N);
        return FEENOX_ERROR;
      }
      
    } else {
      feenox_push_error_message("undefined keyword '%s'", token);
      return FEENOX_ERROR;
    }
  } 
  
  return FEENOX_OK;
}  



int feenox_problem_parse_post_neutron_sn(mesh_write_t *mesh_write, const char *token) {

  if (strcmp(token, "all") == 0) {
    feenox_call(feenox_problem_parse_post_neutron_sn(mesh_write, "scalar_fluxes"));
    feenox_call(feenox_problem_parse_post_neutron_sn(mesh_write, "angular_fluxes"));
    
  } else if (strcmp(token, "scalar_flux") == 0 || strcmp(token, "scalar_fluxes") == 0) {
    
    for (unsigned int g = 0; g < neutron_sn.groups; g++) {
      feenox_call(feenox_add_post_field(mesh_write, 1, &neutron_sn.phi[g]->name, NULL, field_location_nodes));
    }
    
  } else if (strcmp(token, "angular_flux") == 0 || strcmp(token, "angular_fluxes") == 0) {

    for (int n = 0; n < neutron_sn.directions; n++) {
      for (int g = 0; g < neutron_sn.groups; g++) {
        feenox_call(feenox_add_post_field(mesh_write, 1, &feenox.pde.unknown_name[n * neutron_sn.groups + g], NULL, field_location_nodes));
      }
    }
    
    
  } else {
    feenox_push_error_message("undefined keyword '%s' for neutron_sn WRITE_RESULTS", token);
    return FEENOX_ERROR;
  }
  
  return FEENOX_OK;
}

