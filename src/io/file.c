/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox file access routines
 *
 *  Copyright (C) 2013-2021 Jeremy Theler
 *
 *  This file is part of feenox.
 *
 *  feenox is free software: you can redistribute it and/or modify
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

FILE *feenox_fopen(const char *filepath, const char *mode) {
  
  FILE *handle;
  if ((handle = fopen(filepath, mode)) == NULL) {
    // try harder! if the name does not start with a slash, prepend 
    // the input file's directory to find files in the same directory 
    if (filepath[0] != '/') {
      char *last_attempt;
      // TODO: check return
      feenox_check_minusone_null(asprintf(&last_attempt, "%s/%s", feenox.main_input_dirname, filepath));
      handle = fopen(last_attempt, mode);
      feenox_free(last_attempt);
    }
  }
  
  return handle;
  
}

char *feenox_evaluate_string(const char *restrict format, int n_args, expr_t *arg) {

  char *string = NULL;
  
  switch (n_args) {
    case 0:
      feenox_check_minusone_null(asprintf(&string, "%s", format));
    break;
    case 1:
      feenox_check_minusone_null(asprintf(&string, format,
              feenox_expression_eval(&arg[0])));
    break;
    case 2:
      feenox_check_minusone_null(asprintf(&string, format,
              feenox_expression_eval(&arg[0]),
              feenox_expression_eval(&arg[1])));
    break;
    case 3:
      feenox_check_minusone_null(asprintf(&string, format,
              feenox_expression_eval(&arg[0]),
              feenox_expression_eval(&arg[1]),
              feenox_expression_eval(&arg[2])));
    break;
    case 4:
      feenox_check_minusone_null(asprintf(&string, format,
              feenox_expression_eval(&arg[0]),
              feenox_expression_eval(&arg[1]),
              feenox_expression_eval(&arg[2]),
              feenox_expression_eval(&arg[3])));
    break;
    case 5:
      feenox_check_minusone_null(asprintf(&string, format,
              feenox_expression_eval(&arg[0]),
              feenox_expression_eval(&arg[1]),
              feenox_expression_eval(&arg[2]),
              feenox_expression_eval(&arg[3]),
              feenox_expression_eval(&arg[4])));
    break;
    case 6:
      feenox_check_minusone_null(asprintf(&string, format,
              feenox_expression_eval(&arg[0]),
              feenox_expression_eval(&arg[1]),
              feenox_expression_eval(&arg[2]),
              feenox_expression_eval(&arg[3]),
              feenox_expression_eval(&arg[4]),
              feenox_expression_eval(&arg[5])));
    break;
    case 7:
      feenox_check_minusone_null(asprintf(&string, format,
              feenox_expression_eval(&arg[0]),
              feenox_expression_eval(&arg[1]),
              feenox_expression_eval(&arg[2]),
              feenox_expression_eval(&arg[3]),
              feenox_expression_eval(&arg[4]),
              feenox_expression_eval(&arg[5]),
              feenox_expression_eval(&arg[6])));
    break;
    case 8:
      feenox_check_minusone_null(asprintf(&string, format,
              feenox_expression_eval(&arg[0]),
              feenox_expression_eval(&arg[1]),
              feenox_expression_eval(&arg[2]),
              feenox_expression_eval(&arg[3]),
              feenox_expression_eval(&arg[4]),
              feenox_expression_eval(&arg[5]),
              feenox_expression_eval(&arg[6]),
              feenox_expression_eval(&arg[7])));
    break;
    default:
      feenox_push_error_message("more than eight arguments for printf-like format");
      feenox_free(string);
      return NULL;
    break;
  }

  return string;
  
}


int feenox_instruction_file_open(void *arg) {
  
  file_t *file = (file_t *)arg;
  
  // stdin is already opened
  if (file == feenox.special_files._stdin) {
    return FEENOX_OK;
  }
  
  char *newfilepath = NULL;
  if ((newfilepath = feenox_evaluate_string(file->format, file->n_format_args, file->arg)) == NULL) {
    return FEENOX_ERROR;
  }

  if (file->pointer == NULL || file->path == NULL || strcmp(newfilepath, file->path) != 0) {
    file->path = realloc(file->path, strlen(newfilepath)+1);
    strcpy(file->path, newfilepath);
    if (file->pointer != NULL) {
      fclose(file->pointer);
      file->pointer = NULL;
    }
    if (file->mode == NULL || strcmp(file->mode, "") == 0) {
      feenox_push_error_message("unknown open mode for file '%s' ('%s')", file->name, file->path);
      feenox_free(newfilepath);
      return FEENOX_ERROR;
    }
    if ((file->pointer = feenox_fopen(file->path, file->mode)) == NULL) {
      feenox_push_error_message("'%s' when opening file '%s' with mode '%s'", strerror(errno), file->path, file->mode);
      return FEENOX_ERROR;
    }  
  }
  
  feenox_free(newfilepath);
  return FEENOX_OK;
  
}


int feenox_instruction_file_close(void *arg) {
  
  file_t *file = (file_t *)arg;
  if (file->pointer != NULL) {
    fclose(file->pointer);
    file->pointer = NULL;
  }
  return FEENOX_OK;
  
}
