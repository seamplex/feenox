/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox file access routines
 *
 *  Copyright (C) 2013-2021 jeremy theler
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

#include <stdio.h>
#include <errno.h>
#include <string.h>




FILE *feenox_fopen(const char *filepath, const char *mode) {
  
  FILE *handle;
  char *newtry;
  
  if ((handle = fopen(filepath, mode)) != NULL) {
    return handle;
  }
  
  newtry = malloc(strlen(feenox.main_input_dirname) + 1 + strlen(filepath) + 1);
  snprintf(newtry, strlen(feenox.main_input_dirname) + 1 + strlen(filepath) + 1, "%s/%s", feenox.main_input_dirname, filepath);
  
  handle = fopen(newtry, mode);
  free(newtry);
  return handle;
  
}

char *feenox_evaluate_string(char *format, int nargs, expr_t *arg) {

  int size = strlen(format)+1 + nargs*32;
  char *string = malloc(size+1);
  
  switch (nargs) {
    case 0:
      snprintf(string, size, "%s", format);
    break;
    case 1:
      snprintf(string, size, format,
              feenox_evaluate_expression(&arg[0]));
    break;
    case 2:
      snprintf(string, size, format,
              feenox_evaluate_expression(&arg[0]),
              feenox_evaluate_expression(&arg[1]));
    break;
    case 3:
      snprintf(string, size, format,
              feenox_evaluate_expression(&arg[0]),
              feenox_evaluate_expression(&arg[1]),
              feenox_evaluate_expression(&arg[2]));
    break;
    case 4:
      snprintf(string, size, format,
              feenox_evaluate_expression(&arg[0]),
              feenox_evaluate_expression(&arg[1]),
              feenox_evaluate_expression(&arg[2]),
              feenox_evaluate_expression(&arg[3]));
    break;
    case 5:
      snprintf(string, size, format,
              feenox_evaluate_expression(&arg[0]),
              feenox_evaluate_expression(&arg[1]),
              feenox_evaluate_expression(&arg[2]),
              feenox_evaluate_expression(&arg[3]),
              feenox_evaluate_expression(&arg[4]));
    break;
    case 6:
      snprintf(string, size, format,
              feenox_evaluate_expression(&arg[0]),
              feenox_evaluate_expression(&arg[1]),
              feenox_evaluate_expression(&arg[2]),
              feenox_evaluate_expression(&arg[3]),
              feenox_evaluate_expression(&arg[4]),
              feenox_evaluate_expression(&arg[5]));
    break;
    case 7:
      snprintf(string, size, format,
              feenox_evaluate_expression(&arg[0]),
              feenox_evaluate_expression(&arg[1]),
              feenox_evaluate_expression(&arg[2]),
              feenox_evaluate_expression(&arg[3]),
              feenox_evaluate_expression(&arg[4]),
              feenox_evaluate_expression(&arg[5]),
              feenox_evaluate_expression(&arg[6]));
    break;
    case 8:
      snprintf(string, size, format,
              feenox_evaluate_expression(&arg[0]),
              feenox_evaluate_expression(&arg[1]),
              feenox_evaluate_expression(&arg[2]),
              feenox_evaluate_expression(&arg[3]),
              feenox_evaluate_expression(&arg[4]),
              feenox_evaluate_expression(&arg[5]),
              feenox_evaluate_expression(&arg[6]),
              feenox_evaluate_expression(&arg[7]));
    break;
    default:
      feenox_push_error_message("more than eight arguments for string");
      return NULL;
    break;
  }

  return string;
  
}


int feenox_instruction_file_open(void *arg) {
  
  file_t *file = (file_t *)arg;
  char *newfilepath;
  
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
    if (file->mode == NULL) {
      feenox_push_error_message("unknown open mode for file '%s' ('%s')", file->name, file->path);
      return FEENOX_ERROR;
    }
    if ((file->pointer = feenox_fopen(file->path, file->mode)) == NULL) {
      feenox_push_error_message("'%s' when opening file '%s' with mode '%s'", strerror(errno), file->path, file->mode);
      return FEENOX_ERROR;
    }
  }
  
  free(newfilepath);
  return FEENOX_OK;
  
}


int feenox_instruction_file_close(void *arg) {
  
  file_t *file = (file_t *)arg;
  if (file->pointer != NULL) {
    fclose(file->pointer);
    file->pointer = NULL;
  }
  return FEENOX_ERROR;
  
}
