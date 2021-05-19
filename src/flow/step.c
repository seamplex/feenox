/*------------ -------------- -------- --- ----- ---   --       -            -
 *  FeenoX step
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
extern feenox_t feenox;

int feenox_step(instruction_t *first, instruction_t *last) {

  // set the special variable done to true if we are done so it's available during the step
  if ((int)(feenox_special_var_value(in_static))) {
    if ((int)(feenox_special_var_value(step_static)) != 1 && (int)(feenox_special_var_value(step_static)) >= (int)(feenox_special_var_value(static_steps))) {
      feenox_special_var_value(done_static) = 1;
    }
  }
  if (feenox_special_var_value(end_time) == 0) {
    feenox_special_var_value(done) = feenox_special_var_value(done_static);
  } else if (feenox_special_var_value(t) >= (feenox_special_var_value(end_time) - 1e-1*feenox_special_var_value(dt))) {
    feenox_special_var_value(done) = 1;
  }  
  
  // sweep the first & last range but minding the conditional blocks
  instruction_t *ip = first;
  while (ip != last) {
    feenox_call(ip->routine(ip->argument));

    if (feenox.next_instruction != NULL) {
      ip = feenox.next_instruction;
      feenox.next_instruction = NULL;
    } else {
      ip = ip->next;
    }
  }


  // put done back in true if anyone set it to zero for whatever reason
  if ((int)(feenox_special_var_value(in_static))) {
    if ((int)(feenox_special_var_value(step_static)) >= (int)(feenox_special_var_value(static_steps))) {
      feenox_special_var_value(done_static) = 1;
    }
  }
  if (feenox_special_var_value(end_time) == 0) {
    feenox_special_var_value(done) = feenox_special_var_value(done_static);
  } else if (feenox_special_var_value(t) >= (feenox_special_var_value(end_time) - 1e-1*feenox_special_var_value(dt))) {
    feenox_special_var_value(done) = 1;
  }  

  return FEENOX_OK;

}
