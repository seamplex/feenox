/*------------ -------------- -------- --- ----- ---   --       -            -
 *  wasora M4 routines
 *
 *  Copyright (C) 2015--2020 Jeremy Theler
 *
 *  This file is part of wasora.
 *
 *  wasora is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  wasora is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with wasora.  If not, see <http://www.gnu.org/licenses/>.
 *------------------- ------------  ----    --------  --     -       -         -
 */

#define _GNU_SOURCE  
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#ifndef _WASORA_H_
#include "wasora.h"
#endif


int wasora_instruction_m4(void *arg) {
  m4_t *m4 = (m4_t *)arg;
  m4_macro_t *macro;

  char *input_file_path;
  char *output_file_path;
  char templ[BUFFER_SIZE];
  
  int n_args_for_m4;
  char **args_for_m4;
  
  int i, status;  
  pid_t pid, child;
  
  if ((input_file_path = wasora_evaluate_string(m4->input_file->format, m4->input_file->n_args, m4->input_file->arg)) == NULL) {
    return WASORA_RUNTIME_ERROR;
  }
  if ((output_file_path = wasora_evaluate_string(m4->output_file->format, m4->output_file->n_args, m4->output_file->arg)) == NULL) {
    return WASORA_RUNTIME_ERROR;
  }
  
  
  n_args_for_m4 = 2; // el ejecutable y el null del final
  LL_FOREACH(m4->macros, macro) {
    n_args_for_m4++;
  }
  
  // alocamos
  args_for_m4 = calloc(n_args_for_m4, sizeof(char *));
  i = 0;
  feenox_check_alloc(args_for_m4[i++] = strdup("m4"));
  // y loopeamos
  LL_FOREACH(m4->macros, macro) {  
    if (macro->print_token.text != NULL) {
      
      if (asprintf(&args_for_m4[i++], "-D%s=%s", macro->name, macro->print_token.text) == -1) {
        wasora_push_error_message("memory allocation error");
        return WASORA_RUNTIME_ERROR;
      }
      
    } else {
      // TODO: ver como hacer esto de una
      if (snprintf(templ, BUFFER_SIZE, "-D%%s=%s", macro->print_token.format) >= BUFFER_SIZE) {
        wasora_push_error_message("buffer not big enough");
        return WASORA_RUNTIME_ERROR;
      }
      
      if (asprintf(&args_for_m4[i++], templ, macro->name, wasora_evaluate_expression(&macro->print_token.expression)) == -1) {
        wasora_push_error_message("memory allocation error");
        return WASORA_RUNTIME_ERROR;
      }
    }
  }

  // esto no hace falta, pero lo hacemos por elegancia
  args_for_m4[i] = NULL;

  if ((pid = fork()) == 0) {
    
    int input_fd, output_fd;
    
    // abrimos input y output
    if ((input_fd = open(input_file_path, O_RDONLY)) == -1) {
      wasora_push_error_message("cannot read '%s'", input_file_path);
      return WASORA_RUNTIME_ERROR;
    }
    
    if ((output_fd = open(output_file_path, O_WRONLY | O_CREAT, 0666)) == -1) {
      wasora_push_error_message("cannot write '%s'", input_file_path);
      return WASORA_RUNTIME_ERROR;
    }
    
    // duplicamos y cerramos
    dup2(input_fd, 0);
    close(input_fd);

    dup2(output_fd, 1);
    close(output_fd);
    
    // exec!
    execvp("m4", args_for_m4);

    // if execv returns, it must have failed. 
    wasora_push_error_message("m4 not found");
    return WASORA_RUNTIME_ERROR;
    
  } else if (pid > 0) {
    
    // father
    if ((child = waitpid(pid, &status, 0)) == -1) {
      wasora_push_error_message("m4 exec failed");
      return WASORA_RUNTIME_ERROR;
    }
        
  } else {
    
    wasora_push_error_message("m4 fork() failed: %s", strerror(errno));
//    perror(NULL);
    return WASORA_RUNTIME_ERROR;
    
  }
  
  for (i = 0; i < n_args_for_m4; i++) {
    feenox_free(args_for_m4[i]);
  }
  feenox_free(args_for_m4);
  feenox_free(input_file_path);
  feenox_free(output_file_path);
  
  return WASORA_RUNTIME_OK;
  
}
