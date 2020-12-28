/*------------ -------------- -------- --- ----- ---   --       -            -
 *  FeenoX main function
 *
 *  Copyright (C) 2009--2020 jeremy theler
 *
 *  This file is part of FeenoX.
 *
 *  FeenoX is free software: you can redistribute it and/or modify
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

#include <stdio.h>

#include "feenox.h"


int main(int argc, char **argv) {

  feenox_initialize(argc, argv);

  if (feenox_parse_main_input_file(feenox.main_input_filepath) != FEENOX_PARSER_OK) {
    feenox_pop_errors();
    exit(EXIT_FAILURE);
  }
 
/*  
  // vemos como tenemos que correr
  if (show_version) {
    feenox_show_version(1);
    exit(EXIT_SUCCESS);
  } else if (feenox.mode == mode_list_vars) {
    feenox_list_symbols();
    i = WASORA_RUNTIME_OK;
  } else if (feenox.parametric_mode) {
    i = feenox_parametric_run();
  } else if (feenox.min_mode) {
    i = feenox_min_run();
  } else if (feenox.fit_mode) {
    i = feenox_fit_run();
  } else {
    i = feenox_standard_run();
  }
  
  if (i != WASORA_RUNTIME_OK) {
//    feenox_pop_errors();
    feenox_runtime_error();
  }

  // bye bye
  feenox_finalize();
*/  
   return 0;
}
