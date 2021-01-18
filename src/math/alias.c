/*------------ -------------- -------- --- ----- ---   --       -            -
 *  FeenoX alias routines
 *
 *  Copyright (C) 2015 jeremy theler
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
 *  along with feenox.  If not, see <http://www.gnu.org/licenses/>.
 *------------------- ------------  ----    --------  --     -       -         -
 */
#include "feenox.h"
extern feenox_t feenox;

int feenox_instruction_alias(void *arg) {
  alias_t *alias = (alias_t *)arg;
  size_t row, col;
  
  if (!alias->initialized) {

    alias->initialized = 1;
    row = (size_t)(feenox_expression_eval(&alias->row)) - 1;
    col = (size_t)(feenox_expression_eval(&alias->col)) - 1;
    
    if (alias->matrix != NULL) {
      
      if (!alias->matrix->initialized) {
        feenox_call(feenox_matrix_init(alias->matrix));
      }
      if (row >= alias->matrix->rows) {
        feenox_push_error_message("row %d greater than matrix size %dx%d in alias '%s'", row, alias->matrix->rows, alias->matrix->cols, alias->new_variable->name);
      }
      if (col >= alias->matrix->cols) {
        feenox_push_error_message("col %d greater than matrix size %dx%d in alias '%s'", col, alias->matrix->rows, alias->matrix->cols, alias->new_variable->name);
      }
      

      feenox_realloc_variable_ptr(alias->new_variable, gsl_matrix_ptr(feenox_value_ptr(alias->matrix), row, col), 0);
      free(alias->new_variable->initial_static);
      alias->new_variable->initial_static = gsl_matrix_ptr(alias->matrix->initial_static, row, col);
      free(alias->new_variable->initial_transient);
      alias->new_variable->initial_transient = gsl_matrix_ptr(alias->matrix->initial_transient, row, col);
      
    } else if (alias->vector != NULL) {
      
      if (!alias->vector->initialized) {
        feenox_call(feenox_vector_init(alias->vector));
      }
      if (row >= alias->vector->size) {
        feenox_push_error_message("subscript %d greater than vector size %d in alias '%s'", row, alias->vector->size, alias->new_variable->name);
      }
      
      feenox_realloc_variable_ptr(alias->new_variable, gsl_vector_ptr(feenox_value_ptr(alias->vector), row), 0);
      free(alias->new_variable->initial_static);
      alias->new_variable->initial_static = gsl_vector_ptr(alias->vector->initial_static, row);
      free(alias->new_variable->initial_transient);
      alias->new_variable->initial_transient = gsl_vector_ptr(alias->vector->initial_transient, row);
      
    } else if (alias->variable != NULL) {
      
      feenox_realloc_variable_ptr(alias->new_variable, feenox_value_ptr(alias->variable), 0);
      free(alias->new_variable->initial_static);
      alias->new_variable->initial_static = alias->variable->initial_static;
      free(alias->new_variable->initial_transient);
      alias->new_variable->initial_transient = alias->variable->initial_transient;
      
    }
  }
  
  return FEENOX_OK;
}

