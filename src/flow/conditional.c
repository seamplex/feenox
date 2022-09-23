/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox conditional functions
 *
 *  Copyright (C) 2009--2021 jeremy theler
 *
 *  This file is part of feenox.
 *
 *  feenox is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  feenox is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with feenox.  If not, see <http://www.gnu.org/licenses/>.
 *------------------- ------------  ----    --------  --     -       -         -
 */
#include "feenox.h"

int feenox_instruction_if(void *arg) {
  conditional_block_t *conditional_block = (conditional_block_t *)arg;
  int condition;

  condition = (int)feenox_expression_eval(&conditional_block->condition);

  if (condition) {
    conditional_block->evaluated_to_true = 1;
    conditional_block->evaluated_to_false = 0;
    feenox.next_instruction = conditional_block->first_true_instruction;
  } else {
    conditional_block->evaluated_to_true = 0;
    conditional_block->evaluated_to_false = 1;
    feenox.next_instruction = conditional_block->first_false_instruction;
  }

  return FEENOX_OK;
}

int feenox_instruction_else(void *arg) {
  conditional_block_t *conditional_block = (conditional_block_t *)arg;

  if (conditional_block->else_of == NULL) {
    feenox_push_error_message("impossible error in conditional of type A");
    return FEENOX_ERROR;
  }

  if (conditional_block->else_of->evaluated_to_true) {
    feenox.next_instruction = conditional_block->first_true_instruction;
  } else if (conditional_block->else_of->evaluated_to_false) {
    feenox.next_instruction = conditional_block->first_false_instruction;
  } else {
    feenox_push_error_message("impossible error in conditional of type B");
    return FEENOX_ERROR;
  }

  return FEENOX_OK;
}

int feenox_instruction_endif(void *arg) {
  return FEENOX_OK;
}
