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
#include "neutron_transport.h"
#include "../../parser/parser.h"

int feenox_problem_parse_problem_neutron_transport(const char *token) {
  
  if (token != NULL) {
    if (strcasecmp(token, "GROUPS") == 0) {
      double xi = 0;
      feenox_call(feenox_parser_expression_in_string(&xi));
      neutron_transport.groups = (unsigned int)(xi);
      
    } else if (strcasecmp(token, "SN") == 0 || strcasecmp(token, "N") == 0) {
      double xi = 0;
      feenox_call(feenox_parser_expression_in_string(&xi));
      neutron_transport.N = (unsigned int)(xi);
      if (neutron_transport.N != 2 &&
          neutron_transport.N != 4 &&
          neutron_transport.N != 6 &&
          neutron_transport.N != 8) {
        feenox_push_error_message("only S2, S4, S6 and S6 are supported, S%d is not", neutron_transport.N);
      }
      
    } else {
      feenox_push_error_message("undefined keyword '%s'", token);
      return FEENOX_ERROR;
    }
  } 
  
  return FEENOX_OK;
}  

