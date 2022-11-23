/*------------ -------------- -------- --- ----- ---   --       -            -
 *  FeenoX matrix routines
 *
 *  Copyright (C) 2015,2022 jeremy theler
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

lowlevel_matrix_t *feenox_lowlevel_matrix_calloc(const size_t rows, const size_t cols) {
#ifdef HAVE_GSL
  return gsl_matrix_calloc(rows, cols);
#else
  lowlevel_matrix_t *lowlevel_matrix = NULL;
  feenox_check_alloc_null(lowlevel_matrix = malloc(sizeof(lowlevel_matrix_t)));
  lowlevel_matrix->size1 = rows;
  lowlevel_matrix->size2 = cols;
  feenox_check_alloc_null(lowlevel_matrix->data = calloc(rows*cols, sizeof(double)));
  return lowlevel_matrix;
#endif
}

int feenox_lowlevel_matrix_set_zero(lowlevel_matrix_t *A) {
#ifdef HAVE_GSL
  return gsl_matrix_set_zero(A);
#else
  return FEENOX_OK;
#endif
}

int feenox_lowlevel_matrix_free(lowlevel_matrix_t **this) {
#ifdef HAVE_GSL
  return gsl_matrix_free(*this);
  *this = NULL;
#else
  lowlevel_matrix_t *A = *this;
  free(A->data);
  feenox_free(A);
#endif
  return FEENOX_OK;
}

double *feenox_lowlevel_matrix_get_ptr_ij(lowlevel_matrix_t *this, const size_t i, const size_t j) {
#ifdef HAVE_GSL
  return gsl_matrix_get_ptr(this, i, j);
#else
  return &(this->data[i*this->size1 + j]);
#endif
}

double *feenox_lowlevel_matrix_get_ptr(lowlevel_matrix_t *this) {
#ifdef HAVE_GSL
  return gsl_matrix_get_ptr(this, 0, 0);
#else
  return this->data;
#endif
}

double feenox_lowlevel_matrix_get(lowlevel_matrix_t *this, const size_t i, const size_t j) {
#ifdef HAVE_GSL
  return gsl_matrix_get(this, i, j);
#else
  return this->data[i*this->size1 + j];
#endif
}

int feenox_lowlevel_matrix_set(lowlevel_matrix_t *this, const size_t i, const size_t j, const double value) {
#ifdef HAVE_GSL
  return gsl_matrix_set(this, i, j, value);
#else
  this->data[i*this->size1 + j] = value;
  return FEENOX_OK;
#endif
}

int feenox_lowlevel_matrix_add_element_to_existing(lowlevel_matrix_t *this, const size_t i, const size_t j, const double value) {
#ifdef HAVE_GSL
  return gsl_matrix_set(this, i, j, gsl_matrix_get(matrix,i,j) + value);
#else
  this->data[i*this->size1 + j] += value;
  return FEENOX_OK;
#endif
}

int feenox_lowlevel_vector_add_element_to_existing(lowlevel_vector_t *this, const size_t i, const double value) {
#ifdef HAVE_GSL
  return gsl_vector_set(this, i, gsl_vector_get(matrix,j) + value);
#else
  this->data[i] += value;
  return FEENOX_OK;
#endif
}



double feenox_matrix_get(matrix_t *this, const size_t i,  const size_t j) {
  
  if (!this->initialized) {
    feenox_call(feenox_matrix_init(this));
  }
  
  return feenox_lowlevel_matrix_get(feenox_value_ptr(this), i, j);
}

double feenox_matrix_get_initial_static(matrix_t *this, const size_t i,  const size_t j) {
  
  if (!this->initialized) {
    feenox_call(feenox_matrix_init(this));
  }
  
  return feenox_lowlevel_matrix_get(this->initial_static, i, j);
}

double feenox_matrix_get_initial_transient(matrix_t *this, const size_t i,  const size_t j) {
  
  if (!this->initialized) {
    feenox_call(feenox_matrix_init(this));
  }
  
  return feenox_lowlevel_matrix_get(this->initial_transient, i, j);
}

int feenox_matrix_set(matrix_t *this, const size_t i, const size_t j, double value) {
  
  if (this->initialized == 0) {
    if (feenox_matrix_init(this) != FEENOX_OK) {
      feenox_push_error_message("initialization of vector '%s' failed", this->name);
      return FEENOX_ERROR;
    }
  }
  
  feenox_lowlevel_matrix_set(feenox_value_ptr(this), i, j, value);
  
  return FEENOX_OK;
}

int feenox_matrix_init(matrix_t *this) {

  int rows, cols;
  int i, j;
  expr_t *data;

  if ((rows = (int)(round(feenox_expression_eval(&this->rows_expr)))) == 0 &&
      (rows = this->rows) == 0) {
    feenox_push_error_message("matrix '%s' has zero rows", this->name);
    return FEENOX_ERROR;
  } else if (rows < 0) {
    feenox_push_error_message("matrix '%s' has negative rows %d", this->name, rows);
    return FEENOX_ERROR;
  }
  
  if ((cols = (int)(round(feenox_expression_eval(&this->cols_expr)))) == 0  && (cols = this->cols) == 0) {
    feenox_push_error_message("matrix '%s' has zero cols", this->name);
    return FEENOX_ERROR;
  } else if (cols < 0) {
    feenox_push_error_message("matrix '%s' has negative cols %d", this->name, cols);
    return FEENOX_ERROR;
  }
  
  this->rows = rows;
  this->cols = cols;
  feenox_value_ptr(this) = feenox_lowlevel_matrix_calloc(rows, cols);
  this->initial_static = feenox_lowlevel_matrix_calloc(rows, cols);
  this->initial_transient = feenox_lowlevel_matrix_calloc(rows, cols);
  
  if (this->datas != NULL) {
    i = 0;
    j = 0;
    LL_FOREACH(this->datas, data) {
      feenox_matrix_set(this, i, j++, feenox_expression_eval(data));
      if (j == this->cols) {
        j = 0;
        i++;
      }
    }
  }
  
  this->initialized = 1;
  
  return FEENOX_OK;

}
