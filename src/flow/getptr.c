/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox pointer lookup functions
 *
 *  Copyright (C) 2009--2013 jeremy theler
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
extern builtin_function_t       builtin_function[N_BUILTIN_FUNCTIONS];
extern builtin_functional_t     builtin_functional[N_BUILTIN_FUNCTIONALS];
extern builtin_vectorfunction_t builtin_vectorfunction[N_BUILTIN_VECTOR_FUNCTIONS];

#include <string.h>

extern const char factorseparators[];

var_t *feenox_get_variable_ptr(const char *name) {
  var_t *var;
  HASH_FIND_STR(feenox.vars, name, var);
  return var;
}

vector_t *feenox_get_vector_ptr(const char *name) {
  vector_t *vector;
  HASH_FIND_STR(feenox.vectors, name, vector);
  return vector;
}

matrix_t *feenox_get_matrix_ptr(const char *name) {
  matrix_t *matrix;
  HASH_FIND_STR(feenox.matrices, name, matrix);
  return matrix;
}

function_t *feenox_get_function_ptr(const char *name) {
  function_t *function;
  HASH_FIND_STR(feenox.functions, name, function);
  return function;
}

file_t *feenox_get_file_ptr(const char *name) {
  file_t *file;
  HASH_FIND_STR(feenox.files, name, file);
  return file;
}
/*
double (*feenox_get_routine_ptr(const char *name))(const double *) {
  loadable_routine_t *loadable_routine;
  HASH_FIND_STR(feenox.loadable_routines, name, loadable_routine);
  if (loadable_routine == NULL) {
    return NULL;
  }
  return loadable_routine->routine;
}

loadable_routine_t *feenox_get_loadable_routine(const char *name) {
  loadable_routine_t *loadable_routine;
  HASH_FIND_STR(feenox.loadable_routines, name, loadable_routine);
  return loadable_routine;
}
*/

builtin_function_t *feenox_get_builtin_function_ptr(const char *name) {
  int i;

  for (i = 0; i < N_BUILTIN_FUNCTIONS; i++) {
    if (strcmp(name, builtin_function[i].name) == 0) {
      return &builtin_function[i];
    }
  }

  return NULL;

}

builtin_vectorfunction_t *feenox_get_builtin_vectorfunction_ptr(const char *name) {
  int i;

  for (i = 0; i < N_BUILTIN_VECTOR_FUNCTIONS; i++) {
    if (strcmp(name, builtin_vectorfunction[i].name) == 0) {
      return &builtin_vectorfunction[i];
    }
  }

  return NULL;

}

builtin_functional_t *feenox_get_builtin_functional_ptr(const char *name) {
  int i;

  for (i = 0; i < N_BUILTIN_FUNCTIONALS; i++) {
    if (strcmp(name, builtin_functional[i].name) == 0) {
      return &builtin_functional[i];
    }
  }
  
  return NULL;

}

vector_t *feenox_get_first_vector(const char *s) {
  
  char *line = NULL;
  feenox_check_alloc_null(line = strdup(s));
  vector_t *wanted = NULL;
  
  char *factor = strtok(line, factorseparators);
  while (factor != NULL) {
    if ((wanted = feenox_get_vector_ptr(factor)) != NULL) {
      feenox_free(line);
      return wanted;
    }
    factor = strtok(NULL, factorseparators);
  }
  
  feenox_free(line);
  return NULL;
  
}

/*
matrix_t *feenox_get_first_matrix(const char *s) {
  
  char *line = strdup(s);
  char *factor;
  matrix_t *wanted;
  
  factor = strtok(line, factorseparators);
  while (factor != NULL) {
    if ((wanted = feenox_get_matrix_ptr(factor)) != NULL) {
      feenox_free(line);
      return wanted;
    }
    factor = strtok(NULL, factorseparators);
  }
  
  feenox_free(line);
  return NULL;
  
}

char *feenox_get_first_dot(const char *s) {

  char *line = strdup(s);
  char *token;
  char *dummy;
  char *wanted;

  token = strtok(line, factorseparators);
  while (token != NULL) {

    if ((dummy = strstr(token, "_dot")) != NULL) {
      *dummy = '\0';
      wanted = strdup(token);
      feenox_free(line);
      return wanted;
    }

    token = strtok(NULL, factorseparators);
  }

  feenox_free(line);
  return NULL;

}
*/
