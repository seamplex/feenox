/*------------ -------------- -------- --- ----- ---   --       -            -
 *  FeenoX vector routines
 *
 *  Copyright (C) 2015--2017 Jeremy Theler
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

// TODO: put the realloc thing inside this one
int feenox_create_pointwise_function_vectors(function_t *function) {

  // the independent values
  char *name = NULL;
  feenox_check_minusone(asprintf(&name, "vec_%s", function->name));
  feenox_check_alloc(function->vector_value = feenox_define_vector_get_ptr(name, function->data_size));
  if (function->data_size != 0) {
    feenox_call(feenox_vector_init(function->vector_value, FEENOX_VECTOR_NO_INITIAL));
  }
  feenox_free(name);

  // the arguments
  feenox_check_alloc(function->vector_argument = calloc(function->n_arguments, sizeof(vector_t *)));
  if (function->var_argument == NULL) {
    feenox_check_alloc(function->var_argument = calloc(function->n_arguments, sizeof(double *)));
  }
  for (int i = 0; i < function->n_arguments; i++) {
    if (function->var_argument[i] == NULL) {
      function->var_argument[i] = feenox.mesh.vars.arr_x[i];
    }
    feenox_check_minusone(asprintf(&name, "vec_%s_%s", function->name, function->var_argument[i]->name));
    feenox_check_alloc(function->vector_argument[i] = feenox_define_vector_get_ptr(name, function->data_size));
    if (function->data_size != 0) {
      feenox_call(feenox_vector_init(function->vector_argument[i], FEENOX_VECTOR_NO_INITIAL));
    }
    feenox_free(name);
  }  
    
  return FEENOX_OK;
}

double feenox_vector_get(vector_t *this, const size_t i) {

  if (this->initialized == 0) {
    if (feenox_vector_init(this, FEENOX_VECTOR_INITIAL) != FEENOX_OK) {
      feenox_push_error_message("initialization of vector '%s' failed", this->name);
      feenox_runtime_error();
    }
  }
  
  return gsl_vector_get(feenox_value_ptr(this), i);
}

double feenox_vector_get_initial_static(vector_t *this, const size_t i) {
  
  if (this->initialized == 0) {
    if (feenox_vector_init(this, FEENOX_VECTOR_INITIAL) != FEENOX_OK) {
      feenox_push_error_message("initialization of vector '%s' failed", this->name);
      feenox_runtime_error();
    }
  }
  
  return gsl_vector_get(this->initial_static, i);
}

double feenox_vector_get_initial_transient(vector_t *this, const size_t i) {
  
  if (this->initialized == 0) {
    if (feenox_vector_init(this, FEENOX_VECTOR_INITIAL) != FEENOX_OK) {
      feenox_push_error_message("initialization of vector '%s' failed", this->name);
      feenox_runtime_error();
    }
  }
  
  return gsl_vector_get(this->initial_transient, i);
}

int feenox_vector_set(vector_t *this, const size_t i, double value) {
  
  if (this->initialized == 0) {
    if (feenox_vector_init(this, FEENOX_VECTOR_INITIAL) != FEENOX_OK) {
      feenox_push_error_message("initialization of vector '%s' failed", this->name);
      return FEENOX_ERROR;
    }
  }
  
  gsl_vector_set(feenox_value_ptr(this), i, value);
  
  return FEENOX_OK;
}

int feenox_vector_add(vector_t *this, const size_t i, double value) {
  
  if (this->initialized == 0) {
    if (feenox_vector_init(this, FEENOX_VECTOR_INITIAL) != FEENOX_OK) {
      feenox_push_error_message("initialization of vector '%s' failed", this->name);
      return FEENOX_ERROR;
    }
  }
  
  gsl_vector_set(feenox_value_ptr(this), i, gsl_vector_get(feenox_value_ptr(this), i) + value);
  
  return FEENOX_OK;
}

int feenox_vector_set_size(vector_t *this, size_t size) {
  if (this->initialized) {
    feenox_push_error_message("cannot set vector '%s' size, it is already initialized", this->name);
    return FEENOX_ERROR;
  }
  
  this->size = size;
  
  return FEENOX_OK;
}

size_t feenox_vector_get_size(vector_t *this) {
  return this->size;
}


// no_initial = 0 means allocate and initialize initial static and initial transient
// no_initial = 1 means do not allocate initial
int feenox_vector_init(vector_t *this, int no_initial) {

  int size;
  int i;
  expr_t *data;

  if (this->initialized) {
    return FEENOX_OK;
  }

  if ((size = this->size) == 0 && (size = (int)(round(feenox_expression_eval(&this->size_expr)))) == 0) {
    feenox_push_error_message("vector '%s' has zero size at initialization", this->name);
    return FEENOX_ERROR;
  } else if (size < 0) {
    feenox_push_error_message("vector '%s' has negative size %d at initialization", this->name, size);
    return FEENOX_ERROR;
  }
  
  this->size = size;
  feenox_value_ptr(this) = gsl_vector_calloc(size);
  if (no_initial == 0) {
    this->initial_static = gsl_vector_calloc(size);
    this->initial_transient = gsl_vector_calloc(size);
  }  

  if (this->datas != NULL) {
    i = 0;
    LL_FOREACH(this->datas, data) {
      gsl_vector_set(feenox_value_ptr(this), i++, feenox_expression_eval(data));
    }
  }

/*  
  if (this->function != NULL) {
    feenox_realloc_vector_ptr(this, this->function->data_value, 0);
  }
*/  
  this->initialized = 1;
  
  return FEENOX_OK;

}

int feenox_instruction_sort_vector(void *arg) {
  
  sort_vector_t *sort_vector = (sort_vector_t *)arg;
  
  if (sort_vector->v2 == NULL)
    gsl_sort_vector(sort_vector->v1->value);
  else
    gsl_sort_vector2(sort_vector->v1->value, sort_vector->v2->value);
  
  if (sort_vector->descending) {
    gsl_vector_reverse(sort_vector->v1->value);
    
    if (sort_vector->v2 != NULL)
      gsl_vector_reverse(sort_vector->v2->value);
  }
  
  return FEENOX_OK;
}
