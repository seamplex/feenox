/*------------ -------------- -------- --- ----- ---   --       -            -
 *  wasora shell routines
 *
 *  Copyright (C) 2012--2013 jeremy theler
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

#include <stdlib.h>

#ifndef _WASORA_H_
#include "wasora.h"
#endif


int wasora_instruction_shell(void *arg) {
  shell_t *shell = (shell_t *)arg;
  int status;
  char *command = wasora_evaluate_string(shell->format, shell->n_args, shell->arg);
  
  status = system(command);
  if (!status) {
    ; // hacemos algo?
  }
  feenox_free(command);
  
  return WASORA_RUNTIME_OK;
}
