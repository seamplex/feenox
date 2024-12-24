/*------------ -------------- -------- --- ----- ---   --       -            -
 *  FeenoX parser for laplace-specific keywords
 *
 *  Copyright (C) 2023 Jeremy Theler
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
#include "laplace.h"

///pb_laplace+NONE+description Laplace's equation does not need any extra keyword to `PROBLEM`.
int feenox_problem_parse_problem_laplace(const char *token) {

///kw_pde+PROBLEM+detail  * `laplace` solves the Laplace (or Poisson) equation.
  
  // no need to parse anything;
  if (token != NULL) {
    feenox_push_error_message("undefined keyword '%s' for laplace PROBLEM", token);
    return FEENOX_ERROR;
  } else {
    // if token is NULL we have to do the parse-time initialization
    feenox_call(feenox_problem_parse_time_init_laplace());
  }
 
  return FEENOX_OK;
}


int feenox_problem_parse_write_post_laplace(mesh_write_t *mesh_write, const char *token) {

  if (strcmp(token, "all") == 0) {
    
    feenox_call(feenox_problem_parse_write_post_laplace(mesh_write, "phi"));
    feenox_call(feenox_problem_parse_write_post_laplace(mesh_write, "gradient"));
    
  } else if (strcmp(token, "phi") == 0) {
    
    feenox_call(feenox_add_post_field(mesh_write, 1, &feenox.pde.solution[0]->name, NULL, field_location_nodes));
    
  } else if (strcmp(token, "grad_phi") == 0 || strcmp(token, "gradient") == 0) {
    
    char *tokens[3] = {NULL, NULL, NULL};
    for (unsigned int m = 0; m < 3; m++) {
      tokens[m] = strdup((m < feenox.pde.dim) ? feenox.pde.gradient[0][m]->name : "0");
    }
    
    feenox_call(feenox_add_post_field(mesh_write, 3, tokens, "grad_phi", field_location_nodes));
    
    for (unsigned int m = 0; m < 3; m++) {
      feenox_free(tokens[m]);
    }
    
  } else {
    feenox_push_error_message("undefined keyword '%s' for laplace WRITE_RESULTS", token);
    return FEENOX_ERROR;
  }
 
  return FEENOX_OK;
}
  