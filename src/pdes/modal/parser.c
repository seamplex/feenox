/*------------ -------------- -------- --- ----- ---   --       -            -
 *  FeenoX parser for modal-specific keywords
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
#include "modal.h"

int feenox_problem_parse_problem_modal(const char *token) {

///kw_pde+PROBLEM+detail  * `modal` computes the natural mechanical frequencies and oscillation modes.
#ifndef HAVE_SLEPC
  feenox_push_error_message("modal problems need a FeenoX binary linked against SLEPc.");
  return FEENOX_ERROR;
#endif
      
  if (token != NULL) {
    if (strcasecmp(token, "plane_stress") == 0) {
      modal.variant = variant_plane_stress;
  
    } else if (strcasecmp(token, "plane_strain") == 0) {
      modal.variant = variant_plane_strain;
  
    } else {
      feenox_push_error_message("undefined keyword '%s'", token);
      return FEENOX_ERROR;
    }
  } 
  
  return FEENOX_OK;
}

int feenox_problem_parse_post_modal(mesh_write_t *mesh_write, const char *token) {

  if (strcmp(token, "all") == 0) {
    ;
  } else {
    feenox_push_error_message("undefined keyword '%s' for modal WRITE_RESULTS", token);
    return FEENOX_ERROR;
  }
  
  return FEENOX_OK;
}
  