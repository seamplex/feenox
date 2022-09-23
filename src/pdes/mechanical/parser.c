/*------------ -------------- -------- --- ----- ---   --       -            -
 *  FeenoX parser for mechanical-specific keywords
 *
 *  Copyright (C) 2022 jeremy theler
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
 *  along with feenox.  If not, see <http://www.gnu.org/licenses/>.
 *------------------- ------------  ----    --------  --     -       -         -
 */
#include "feenox.h"
#include "../../parser/parser.h"
#include "mechanical.h"

int feenox_problem_parse_problem_mechanical(const char *token) {
  
///kw_pde+PROBLEM+detail  * `mechanical` solves the elastic problem using a displacement-based FEM formulation.
///kw_pde+PROBLEM+detail If the mesh is two-dimensional and not `AXISYMMETRIC`, either
///kw_pde+PROBLEM+detail `plane_stress` or `plane_strain` has to be given instead of `mechanical`.
  
  if (token != NULL) {
    if (strcasecmp(token, "plane_stress") == 0) {
      mechanical.variant = variant_plane_stress;
  
    } else if (strcasecmp(token, "plane_strain") == 0) {
      mechanical.variant = variant_plane_strain;
  
    } else {
      feenox_push_error_message("undefined keyword '%s'", token);
      return FEENOX_ERROR;
      
    }
  } 
  return FEENOX_OK;
} 


int feenox_parse_linearize_stress(void) {
  
  feenox_linearize_t *linearize = NULL;
  feenox_check_alloc(linearize = calloc(1, sizeof(feenox_linearize_t)));
  
  char *token = NULL;
  while ((token = feenox_get_next_token(NULL)) != NULL) {
///kw_pde+LINEARIZE_STRESS+usage FROM <x1> <y1> <z1>
///kw_pde+LINEARIZE_STRESS+detail The stress classification line (SCL) defined by the coordinates of
///kw_pde+LINEARIZE_STRESS+detail the points\ $[x_1, y_1, z_1]$ and $[x_2, y_2, z_2]$.
///kw_pde+LINEARIZE_STRESS+detail For two-dimensional problems, the $z$ coordinate has to be given as well.
    if (strcasecmp(token, "FROM") == 0) {
      feenox_call(feenox_parser_expression(&linearize->x1));
      feenox_call(feenox_parser_expression(&linearize->y1));
      feenox_call(feenox_parser_expression(&linearize->z1));
      
///kw_pde+LINEARIZE_STRESS+usage TO <x2> <y2> <z2> @
    } else if (strcasecmp(token, "TO") == 0) {
      feenox_call(feenox_parser_expression(&linearize->x2));
      feenox_call(feenox_parser_expression(&linearize->y2));
      feenox_call(feenox_parser_expression(&linearize->z2));

///kw_pde+LINEARIZE_STRESS+detail The linearized membrane, membrane plus bending and peak total stresses are 
///kw_pde+LINEARIZE_STRESS+detail stored in the variables given by the keywords `M`, `MB` and `P`, respectively.
///kw_pde+LINEARIZE_STRESS+detail These three variables use the von\ Mises stress intensity.
///kw_pde+LINEARIZE_STRESS+usage [ M <variable> ]
    } else if (strcasecmp(token, "M") == 0) {
      feenox_call(feenox_parser_get_or_define_variable(&linearize->M));

///kw_pde+LINEARIZE_STRESS+usage [ MB <variable> ]
    } else if (strcasecmp(token, "MB") == 0) {
      feenox_call(feenox_parser_get_or_define_variable(&linearize->MB));
      
///kw_pde+LINEARIZE_STRESS+usage [ P <variable> ] @
    } else if (strcasecmp(token, "P") == 0) {
      feenox_call(feenox_parser_get_or_define_variable(&linearize->P));

///kw_pde+LINEARIZE_STRESS+detail Variables `Mt`, `MBt` and `Pt` use the Tresca stress intensity.
///kw_pde+LINEARIZE_STRESS+usage [ Mt <variable> ]
    } else if (strcasecmp(token, "Mt") == 0) {
      feenox_call(feenox_parser_get_or_define_variable(&linearize->Mt));

///kw_pde+LINEARIZE_STRESS+usage [ MBt <variable> ]
    } else if (strcasecmp(token, "MBt") == 0) {
      feenox_call(feenox_parser_get_or_define_variable(&linearize->MBt));
      
///kw_pde+LINEARIZE_STRESS+usage [ Pt <variable> ] @
    } else if (strcasecmp(token, "Pt") == 0) {
      feenox_call(feenox_parser_get_or_define_variable(&linearize->Pt));

///kw_pde+LINEARIZE_STRESS+detail Variables `M1` (or 2 or 3), `MB1` (or 2 or 3) and `P1` (or 2 or 3)
///kw_pde+LINEARIZE_STRESS+detail use the principal stress 1 (or 2 or 3).
///kw_pde+LINEARIZE_STRESS+usage [ M1 <variable> ]
    } else if (strcasecmp(token, "M1") == 0) {
      feenox_call(feenox_parser_get_or_define_variable(&linearize->M1));

///kw_pde+LINEARIZE_STRESS+usage [ MB1 <variable> ]
    } else if (strcasecmp(token, "MB1") == 0) {
      feenox_call(feenox_parser_get_or_define_variable(&linearize->MB1));
      
///kw_pde+LINEARIZE_STRESS+usage [ P1 <variable> ] @
    } else if (strcasecmp(token, "P1") == 0) {
      feenox_call(feenox_parser_get_or_define_variable(&linearize->P1));

      
///kw_pde+LINEARIZE_STRESS+usage [ M2 <variable> ]
    } else if (strcasecmp(token, "M2") == 0) {
      feenox_call(feenox_parser_get_or_define_variable(&linearize->M2));

///kw_pde+LINEARIZE_STRESS+usage [ MB2 <variable> ]
    } else if (strcasecmp(token, "MB2") == 0) {
      feenox_call(feenox_parser_get_or_define_variable(&linearize->MB2));
      
///kw_pde+LINEARIZE_STRESS+usage [ P2 <variable> ] @
    } else if (strcasecmp(token, "P2") == 0) {
      feenox_call(feenox_parser_get_or_define_variable(&linearize->P2));
      

///kw_pde+LINEARIZE_STRESS+usage [ M3 <variable> ]
    } else if (strcasecmp(token, "M3") == 0) {
      feenox_call(feenox_parser_get_or_define_variable(&linearize->M3));

///kw_pde+LINEARIZE_STRESS+usage [ MB3 <variable> ]
    } else if (strcasecmp(token, "MB3") == 0) {
      feenox_call(feenox_parser_get_or_define_variable(&linearize->MB3));
      
///kw_pde+LINEARIZE_STRESS+usage [ P3 <variable> ] @
    } else if (strcasecmp(token, "P3") == 0) {
      feenox_call(feenox_parser_get_or_define_variable(&linearize->P3));
      
      
      
///kw_pde+LINEARIZE_STRESS+usage [ FILE <file> ]
///kw_pde+LINEARIZE_STRESS+detail If the `FILE` keyword is given, the total, membrane and membrane plus bending
///kw_pde+LINEARIZE_STRESS++detail stresses are written as a function of a scalar $t \in [0,1]$.
///kw_pde+LINEARIZE_STRESS++detail Moreover, the individual elements of the membrane and bending stress tensors are written
///kw_pde+LINEARIZE_STRESS++detail within comments (i.e. lines starting with the hash symbol `#`).
//TODO: decir como se plotea y como se hace un PDF
      
    } else if (strcasecmp(token, "FILE") == 0) {
      feenox_call(feenox_parser_file(&linearize->file));
      
    } else {
      feenox_push_error_message("unknown keyword '%s'", token);
      return FEENOX_ERROR;
    }

  }
  
  feenox_call(feenox_add_instruction(feenox_instruction_linearize, linearize));
  LL_APPEND(mechanical.linearizes, linearize);
  
  return FEENOX_OK;
}
