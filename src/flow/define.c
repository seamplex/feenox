/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox definition functions
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
#define _GNU_SOURCE  // for asprintf
#include "feenox.h"
extern feenox_t feenox;
extern builtin_function_t builtin_function[N_BUILTIN_FUNCTIONS];
extern builtin_vectorfunction_t builtin_vectorfunction[N_BUILTIN_VECTOR_FUNCTIONS];
extern builtin_functional_t builtin_functional[N_BUILTIN_FUNCTIONALS];

#include <ctype.h>
#include <string.h>

extern const char factorseparators[];

void feenox_realloc_variable_ptr(var_t *this, double *newptr, int copy_contents) {
  
  // si copy_contents es true copiamos el contenido del vector de feenox
  // antes de tirar el apuntador a la basura
  if (copy_contents) {
    *newptr = feenox_var_value(this);
  }
  
  // si el puntero es de feenox, lo liberamos
  if (this->reallocated == 0) {
    free(feenox_value_ptr(this));
  }
  
  this->reallocated = 1;
  feenox_value_ptr(this) = newptr;
  
  return;
}

void feenox_realloc_vector_ptr(vector_t *this, double *newptr, int copy_contents) {
  
  double *oldptr = gsl_vector_ptr(feenox_value_ptr(this), 0);

  // si copy_contents es true copiamos el contenido del vector de feenox
  // antes de tirar el apuntador a la basura
  if (copy_contents) {
    memcpy(newptr, oldptr, this->size * sizeof(double));
  }

  // si el puntero es de feenox, lo liberamos
  if (this->reallocated == 0) {
    if (feenox_value_ptr(this)->stride != 1) {
      feenox_push_error_message("vector '%s' cannot be realloced: stride not equal to 1", this->name);
      feenox_runtime_error();
    }
    if (feenox_value_ptr(this)->owner == 0) {
      feenox_push_error_message("vector '%s' cannot be realloced: not the data owner", this->name);
      feenox_runtime_error();
    }
    if (feenox_value_ptr(this)->block->data != feenox_value_ptr(this)->data) {
      feenox_push_error_message("vector '%s' cannot be realloced: data not pointing to block", this->name);
      feenox_runtime_error();
    }

    free(oldptr);
  }
  
  this->reallocated = 1;
  feenox_value_ptr(this)->data = newptr;
  
  return;
}

void feenox_realloc_matrix_ptr(matrix_t *matrix, double *newptr, int copy_contents) {
  
  double *oldptr = gsl_matrix_ptr(feenox_value_ptr(matrix), 0, 0);
  
  // si copy_contents es true copiamos el contenido del vector de feenox
  // antes de tirar el apuntador a la basura
  if (copy_contents) {
    memcpy(newptr, oldptr, matrix->rows*matrix->cols * sizeof(double));
  }
  
  // si el puntero es de feenox, lo liberamos
  if (matrix->realloced == 0) {
    free(oldptr);
  }
  
  matrix->realloced = 1;
  feenox_value_ptr(matrix)->data = newptr;
  
  return;
}


// visible through the API
int feenox_define_variable(const char *name) {

  var_t *var;
  
  if (feenox_check_name(name) != FEENOX_OK) {
    if ((var = feenox_get_variable_ptr(name)) != NULL) {
      // the variable already exists, check_name() should have complained
      // so we remove the error and return the existing variable
      feenox_pop_error_message();
      return FEENOX_OK;
    } else {
      // invalid name
      return FEENOX_ERROR;
    }
  }

  feenox_check_alloc(var = calloc(1, sizeof(var_t)));
  feenox_check_alloc(var->name = strdup(name));
  var->value = calloc(1, sizeof(double));
  var->initial_transient = calloc(1, sizeof(double));
  var->initial_static = calloc(1, sizeof(double));
  HASH_ADD_KEYPTR(hh, feenox.vars, var->name, strlen(var->name), var);

  return FEENOX_OK;
}

// visible through the API
int feenox_define_alias(const char *new_name, const char *existing_object, const char *row, const char *col) {

  feenox_call((feenox_check_name(new_name)));
  
  alias_t *alias = calloc(1, sizeof(alias_t));
  feenox_call(feenox_define_variable(new_name));
  if ((alias->new_variable = feenox_get_variable_ptr(new_name)) == NULL) {
    return FEENOX_ERROR;
  }
  
  if ((alias->matrix = feenox_get_matrix_ptr(existing_object)) == NULL) {
    if ((alias->vector = feenox_get_vector_ptr(existing_object)) == NULL) {
      if ((alias->variable = feenox_get_variable_ptr(existing_object)) == NULL) {
       feenox_push_error_message("unknown allegedly existing object '%s'", existing_object);
       return FEENOX_ERROR;
      }
    }
  }
  
  if (row != NULL) {
    feenox_call(feenox_expression_parse(&alias->row, row));
  }
  if (col != NULL) {
    feenox_call(feenox_expression_parse(&alias->col, col));
  }
  
  LL_APPEND(feenox.aliases, alias);
  feenox_call(feenox_add_instruction(feenox_instruction_alias, alias));
  
  return FEENOX_OK;
}

// API
int feenox_define_vector(const char *name, const char *size) {

  vector_t *vector;
  char *dummy;
  
  // since there are names which can come from physical names or some other weird places
  // we replace spaces with underscores
  while ((dummy = strchr(name, ' ')) != NULL) {
    *dummy = '_';
  }

  feenox_call(feenox_check_name(name));

  feenox_check_alloc(vector = calloc(1, sizeof(vector_t)));
  feenox_check_alloc(vector->name = strdup(name));
  feenox_call(feenox_expression_parse(&vector->size_expr, size));

  HASH_ADD_KEYPTR(hh, feenox.vectors, vector->name, strlen(vector->name), vector);

  return FEENOX_OK;

}

vector_t *feenox_define_vector_get_ptr(const char *name, size_t size) {
  // this function is called from the code, that alredy knows what the vector size is
  // the function below is called from the API that allows a string
  // this is ugly but we don't need too much performance
  char *size_string;
  asprintf(&size_string, "%ld", size);
  if (feenox_define_vector(name, size_string) != FEENOX_OK) {
    return NULL;
  }
  free(size_string);
  return feenox_get_vector_ptr(name);
}


// API
int feenox_vector_attach_function(const char *name, const char *function_data) {
  
  vector_t *vector;
  function_t *function;
  if ((vector = feenox_get_vector_ptr(name)) == NULL) {
    feenox_push_error_message("unkown vector '%s'", name);
    return FEENOX_ERROR;
  }
  if ((function = feenox_get_function_ptr(function_data)) == NULL) {
    feenox_push_error_message("unkown function '%s'", function_data);
    return FEENOX_ERROR;
  }
  
  vector->function = function;
  
  return FEENOX_OK;
}

int feenox_vector_attach_data(const char *name, expr_t *datas) {

  vector_t *vector;
  if ((vector = feenox_get_vector_ptr(name)) == NULL) {
    feenox_push_error_message("unkown vector '%s'", name);
    return FEENOX_ERROR;
  }
  
  vector->datas = datas;

  return FEENOX_OK;
}


// API
int feenox_define_matrix(const char *name, const char *rows, const char *cols) {

  feenox_call(feenox_check_name(name));

  matrix_t *matrix;
  feenox_check_alloc(matrix = calloc(1, sizeof(matrix_t)));
  feenox_check_alloc(matrix->name = strdup(name));
  feenox_call(feenox_expression_parse(&matrix->rows_expr, rows));
  feenox_call(feenox_expression_parse(&matrix->cols_expr, cols));

  HASH_ADD_KEYPTR(hh, feenox.matrices, matrix->name, strlen(matrix->name), matrix);

  return FEENOX_OK;

}

int feenox_matrix_attach_data(const char *name, expr_t *datas) {

  matrix_t *matrix;
  if ((matrix = feenox_get_matrix_ptr(name)) == NULL) {
    feenox_push_error_message("unkown matrix '%s'", name);
    return FEENOX_ERROR;
  }
  
  matrix->datas = datas;

  return FEENOX_OK;
}

function_t *feenox_define_function_get_ptr(const char *name, unsigned int n_arguments) {
  if (feenox_define_function(name, n_arguments) != FEENOX_OK) {
    return NULL;
  }
  return feenox_get_function_ptr(name);
}

int feenox_define_function(const char *name, unsigned int n_arguments) {
  
  function_t *function;

  feenox_call(feenox_check_name(name));

  feenox_check_alloc(function = calloc(1, sizeof(function_t)));
  feenox_check_alloc(function->name = strdup(name));
  function->n_arguments = n_arguments;
  
  feenox_check_alloc(function->var_argument = calloc(n_arguments, sizeof(var_t *)));
  function->var_argument_allocated = 1;
  feenox_check_alloc(function->column = calloc((n_arguments+1), sizeof(int)));
  
  // default columns
  int i;
  for (i = 0; i < n_arguments+1; i++) {
    function->column[i] = i+1;
  }
  
  HASH_ADD_KEYPTR(hh, feenox.functions, function->name, strlen(function->name), function);

  return FEENOX_OK;
}

int feenox_function_set_argument_variable(const char *name, unsigned int i, const char *variable_name) {
  
  function_t *function;
  if ((function = feenox_get_function_ptr(name)) == NULL) {
    feenox_push_error_message("unkown function '%s'", name);
    return FEENOX_ERROR;
  }
  
  if (i < 0) {
    feenox_push_error_message("negative argument number '%d'", i);
    return FEENOX_ERROR;
  }
  if (i >= function->n_arguments) {
    feenox_push_error_message("argument number '%d' greater or equal than the number of arguments '%d' (they start from zero)", i, function->n_arguments);
    return FEENOX_ERROR;
  }
  
  if ((function->var_argument[i] = feenox_get_or_define_variable_get_ptr(variable_name)) == NULL) {
    return FEENOX_ERROR;
  }
  
  // mark that we received an argument to see if we got them all when initializing
  function->n_arguments_given++;
  
  return FEENOX_OK;
}


int feenox_function_set_expression(const char *name, const char *expression) {
  
  function_t *function;
  if ((function = feenox_get_function_ptr(name)) == NULL) {
    feenox_push_error_message("unkown function '%s'", name);
    return FEENOX_ERROR;
  }
  
  function->type = function_type_algebraic;
  feenox_call(feenox_expression_parse(&function->algebraic_expression, expression));  
  
  return FEENOX_OK;
}



int feenox_define_file(const char *name, const char *format, unsigned int n_format_args, const char *mode) {

  file_t *file;

  HASH_FIND_STR(feenox.files, name, file);
  if (file != NULL) {
    feenox_push_error_message("there already exists a file named '%s'", name);
    return FEENOX_ERROR;
  }

  feenox_check_alloc(file = calloc(1, sizeof(file_t)));
  feenox_check_alloc(file->name = strdup(name));
  feenox_check_alloc(file->format = strdup(format));
  if ((file->n_format_args = n_format_args) > 0) {
    feenox_check_alloc(file->arg = calloc(file->n_format_args, sizeof(expr_t *)));
  }  
  if (mode != NULL && strcmp(mode, "") == 0) {
    feenox_check_alloc(file->mode = strdup(mode));
  }

  HASH_ADD_KEYPTR(hh, feenox.files, file->name, strlen(file->name), file);

  return FEENOX_OK;
}

file_t *feenox_get_or_define_file_get_ptr(const char *name) {
  file_t *file;
  if ((file = feenox_get_file_ptr(name)) == NULL) {
    if (feenox_define_file(name, name, 0, "") != FEENOX_OK) {
      return NULL;
    }
    if ((file = feenox_get_file_ptr(name)) == NULL) {
      return NULL;
    }
  }
  return file;
}



int feenox_file_set_path_argument(const char *name, unsigned int i, const char *expression) {
  
  file_t *file;
  if ((file = feenox_get_file_ptr(name)) == NULL) {
    feenox_push_error_message("unkown file '%s'", name);
    return FEENOX_ERROR;
  }
  
  if (i < 0) {
    feenox_push_error_message("negative argument number '%d'", i);
    return FEENOX_ERROR;
  }
  if (i >= file->n_format_args) {
    feenox_push_error_message("argument number '%d' greater or equal than the number of arguments '%d' (they start from zero)", i, file->n_format_args);
    return FEENOX_ERROR;
  }
  
  feenox_call(feenox_expression_parse(&file->arg[i], expression));
  
  // mark that we received an argument to see if we got them all when initializing
  file->n_format_args_given++;
  
  return FEENOX_OK;
}


/*
loadable_routine_t *feenox_define_loadable_routine(char *name, void *library) {

  loadable_routine_t *loadable_routine;
  char *error;

  HASH_FIND_STR(feenox.loadable_routines, name, loadable_routine);
  if (loadable_routine != NULL) {
    feenox_push_error_message("there already exists a loadable routine named '%s'", name);
    return NULL;
  }

  loadable_routine = calloc(1, sizeof(loadable_routine_t));
  loadable_routine->name = strdup(name);

  // reseteamos errores como dice el manual
  dlerror();
  // cargamos la rutina
  loadable_routine->routine = dlsym(library, loadable_routine->name);
  // y vemos si se prendio algun error
   if ((error = dlerror()) != NULL) {
     feenox_push_error_message("'%s' when loading routine '%s'", error, loadable_routine->name);
     dlclose(library);
     return NULL;
   }

  HASH_ADD_KEYPTR(hh, feenox.loadable_routines, loadable_routine->name, strlen(loadable_routine->name), loadable_routine);

  return loadable_routine;
}

*/

var_t *feenox_define_variable_get_ptr(const char *name) {
  if (feenox_define_variable(name) != FEENOX_OK) {
    return NULL;
  }
  return feenox_get_variable_ptr(name);
}

var_t *feenox_get_or_define_variable_get_ptr(const char *name) {
  var_t *var;

  if ((var = feenox_get_variable_ptr(name)) == NULL) {
    if (feenox_define_variable(name) != FEENOX_OK) {
      return NULL;
    }
    var = feenox_get_variable_ptr(name);
  }

  return var;
}
  
// check if an object name is valid and is available for further usage
int feenox_check_name(const char *name) {
  char *s;
  
  var_t *var;
  vector_t *vector;
  matrix_t *matrix;
  function_t *function;

  if (name == NULL) {
    feenox_push_error_message("(name is null)");
    return FEENOX_ERROR;
  }

  if (isdigit(name[0])) {
    feenox_push_error_message("invalid object name '%s' (it starts with a digit)", name);
    return FEENOX_ERROR;
  }
  
  if ((s = strpbrk(name, factorseparators)) != NULL) {
    feenox_push_error_message("invalid object name '%s' (it contains a '%c')", name, s[0]);
    return FEENOX_ERROR;
  }
  
  HASH_FIND_STR(feenox.vars, name, var);
  if (var != NULL) {
    feenox_push_error_message("there already exists a variable named '%s'", name);
    return FEENOX_ERROR;
  }

  HASH_FIND_STR(feenox.vectors, name, vector);
  if (vector != NULL) {
    feenox_push_error_message("there already exists a vector named '%s'", name);
    return FEENOX_ERROR;
  }

  HASH_FIND_STR(feenox.matrices, name, matrix);
  if (matrix != NULL) {
    feenox_push_error_message("there already exists a matrix named '%s'", name);
    return FEENOX_ERROR;
  }

  HASH_FIND_STR(feenox.functions, name, function);
  if (function != NULL) {
    feenox_push_error_message("there already exists a function named '%s'", name);
    return FEENOX_ERROR;
  }

  int i;
  for (i = 0; i < N_BUILTIN_FUNCTIONS; i++) {
    if (builtin_function[i].name != NULL && strcmp(name, builtin_function[i].name) == 0) {
      feenox_push_error_message("there already exists a built-in function named '%s'", name);
      return FEENOX_ERROR;
    }
  }

  // TODO
  
/*  
  for (i = 0; i < N_BUILTIN_VECTOR_FUNCTIONS; i++) {
    if (builtin_vectorfunction[i].name != NULL && strcmp(name, builtin_vectorfunction[i].name) == 0) {
      feenox_push_error_message("there already exists a built-in vector function named '%s'", name);
      return FEENOX_ERROR;
    }
  }

  for (i = 0; i < N_BUILTIN_FUNCTIONALS; i++) {
    if (builtin_functional[i].name != NULL && strcmp(name, builtin_functional[i].name) == 0) {
      feenox_push_error_message("there already exists a built-in functional named '%s'", name);
      return FEENOX_ERROR;
    }
  }
*/

  return FEENOX_OK;

}
