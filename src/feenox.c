/*------------ -------------- -------- --- ----- ---   --       -            -
 *  FeenoX main function
 *
 *  Copyright (C) 2009--2021 jeremy theler
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
#include "feenox.h"
feenox_t feenox;

#include <stdlib.h>

int main(int argc, char **argv) {

  if (feenox_initialize(argc, argv) != FEENOX_OK) {
    feenox_pop_errors();
    exit(EXIT_FAILURE);
  }
  
  if (feenox_run() != FEENOX_OK) {
    feenox_runtime_error();
    exit(EXIT_FAILURE);
  }

  feenox_finalize();

  return 0;
}
