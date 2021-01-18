/*------------ -------------- -------- --- ----- ---   --       -            -
 *  wasora user routine calling
 *
 *  Copyright (C) 2009--2013 jeremy theler
 *
 *  This file is part of wasora.
 *
 *  wasora is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  wasora is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with wasora.  If not, see <http://www.gnu.org/licenses/>.
 *------------------- ------------  ----    --------  --     -       -         -
 */
#ifndef _WASORA_H_
#include "wasora.h"
#endif

int wasora_instruction_call(void *arg) {
  call_t *call = (call_t *)arg;
  expr_t *argument;

  int i = 0;

  LL_FOREACH(call->arguments, argument) {
    call->x[i++] = wasora_evaluate_expression(argument);
  }

  if (call->n_arguments != 0) {
    call->function(call->x);
  } else {
    call->function(NULL);
  }

  return WASORA_RUNTIME_OK;
}

