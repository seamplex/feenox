/*------------ -------------- -------- --- ----- ---   --       -            -
 *  FeenoX matrix routines
 *
 *  Copyright (C) 2015 Jeremy Theler
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

double feenox_matrix_get(matrix_t *matrix, const size_t i,  const size_t j) {
  
  if (!matrix->initialized) {
    feenox_call(feenox_matrix_init(matrix));
  }
  
  return gsl_matrix_get(feenox_value_ptr(matrix), i, j);
}

double feenox_matrix_get_initial_static(matrix_t *matrix, const size_t i,  const size_t j) {
  
  if (!matrix->initialized) {
    feenox_call(feenox_matrix_init(matrix));
  }
  
  return gsl_matrix_get(matrix->initial_static, i, j);
}

double feenox_matrix_get_initial_transient(matrix_t *matrix, const size_t i,  const size_t j) {
  
  if (!matrix->initialized) {
    feenox_call(feenox_matrix_init(matrix));
  }
  
  return gsl_matrix_get(matrix->initial_transient, i, j);
}


int feenox_matrix_init(matrix_t *matrix) {

  int rows, cols;
  int i, j;
  expr_t *data;

  if ((rows = (int)(round(feenox_expression_eval(&matrix->rows_expr)))) == 0 &&
      (rows = matrix->rows) == 0) {
    feenox_push_error_message("matrix '%s' has zero rows", matrix->name);
    return FEENOX_ERROR;
  } else if (rows < 0) {
    feenox_push_error_message("matrix '%s' has negative rows %d", matrix->name, rows);
    return FEENOX_ERROR;
  }
  
  if ((cols = (int)(round(feenox_expression_eval(&matrix->cols_expr)))) == 0  && (cols = matrix->cols) == 0) {
    feenox_push_error_message("matrix '%s' has zero cols", matrix->name);
    return FEENOX_ERROR;
  } else if (cols < 0) {
    feenox_push_error_message("matrix '%s' has negative cols %d", matrix->name, cols);
    return FEENOX_ERROR;
  }
  
  matrix->rows = rows;
  matrix->cols = cols;
  feenox_value_ptr(matrix) = gsl_matrix_calloc(rows, cols);
  matrix->initial_static = gsl_matrix_calloc(rows, cols);
  matrix->initial_transient = gsl_matrix_calloc(rows, cols);
  
  if (matrix->datas != NULL) {
    i = 0;
    j = 0;
    LL_FOREACH(matrix->datas, data) {
      gsl_matrix_set(feenox_value_ptr(matrix), i, j++, feenox_expression_eval(data));
      if (j == matrix->cols) {
        j = 0;
        i++;
      }
    }
  }
  
  matrix->initialized = 1;
  
  return FEENOX_OK;

}
