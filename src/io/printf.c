/*------------ -------------- -------- --- ----- ---   --       -            -
 *  FeenoX printf instruction
 *
 *  Copyright (C) 2023 jeremy theler
 *
 *  This file is part of feenox.
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

#include <stdio.h>
#include <string.h>


int feenox_instruction_printf(void *arg) {
  printf_t *printf = (printf_t *)arg;

  char *string = feenox_evaluate_string(printf->format_string, printf->n_args, printf->expressions);
  fprintf(printf->file->pointer, "%s", string);
  fflush(printf->file->pointer);
  feenox_free(string);

  return FEENOX_OK;

}


