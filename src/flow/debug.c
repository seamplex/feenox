/*------------ -------------- -------- --- ----- ---   --       -            -
 *  wasora debug function
 *
 *  Copyright (C) 2013 jeremy theler
 *  Copyright (C) 2015 Juan Pablo Gomez Omil
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

#include <stdio.h>
#include <string.h>
#include <sys/select.h>

#ifndef _WASORA_H_
#include "wasora.h"
#endif

#ifdef HAVE_READLINE
 #include <readline/readline.h>
 #include <readline/history.h>
#endif


char wasora_debug_help[] = "\
feenox's debug mode available commands:\n\
step | {empty line}             advance a single step\n\
continue                        continue with the execution\n\
help                            print this help\n\
breakpoint {expr}               set a conditional breakpoint when expr is true\n\
print {expr | vector | matrix}  watch an scalar expression, a vector or a matrix\n\
quit                            stop the execution and politely exit wasora\n\
{variable} = {expr}             assign variable to expr (the entered value will be\n\
                                   overwritten if the variable is assigned again\n\
                                   in the next step of the execution)\n\
\n\
any command may be replaced by the first letter only, i.e.\n\
 'print t/dt' can be written as 'p t/dt'\n\
and\n\
 'breakpoint t>0.499' can be written as 'b t>0.499'\n\
symbol names are autocompleted with tab key\n\
command history can be browsed with the arrow keys and reverse-searched with <C-r>.\n\
\n"; 


char **wasora_rl_completion(const char *text, int start, int end) {
  char **matches = NULL;

#if HAVE_READLINE
  matches = rl_completion_matches(text, wasora_rl_symbol_generator);
#endif

  return matches;
}

char *wasora_rl_symbol_generator(const char *text, int state) {
  static var_t *current_var;
  static vector_t *current_vector;
  static matrix_t *current_matrix;

  static int len;
  const char *name;

  // para state == 0 empezamos
  if (state == 0) {
    len = strlen(text);
    current_var = wasora.vars;
    current_vector = wasora.vectors;
    current_matrix = wasora.matrices;
  }

  while (current_var != NULL) {
    name = current_var->name;
    current_var = current_var->hh.next;
    if (len == 0 || strncmp(name, text, len) == 0) {
      return (strdup(name));
    }
  }

  while (current_vector != NULL) {
    name = current_vector->name;
    current_vector = current_vector->hh.next;
    if (len == 0 || strncmp(name, text, len) == 0) {
      return (strdup(name));
    }
  }

  while (current_matrix != NULL) {
    name = current_matrix->name;
    current_matrix = current_matrix->hh.next;
    if (len == 0 || strncmp(name, text, len) == 0) {
      return (strdup(name));
    }
  }

  // si llegamos hasta aca es porque no encontramos naranja
  return NULL;
}


void wasora_list_symbols(void) {

  var_t *var;
  vector_t *vector;
  matrix_t *matrix;
  function_t *function;

  for (var = wasora.vars; var != NULL; var = var->hh.next) {
    printf("%s (scalar variable)\n", var->name);
  }
  for (vector = wasora.vectors; vector != NULL; vector = vector->hh.next) {
    printf("%s (vector of size %d)\n", vector->name, vector->size);
  }
  for (matrix = wasora.matrices; matrix != NULL; matrix = matrix->hh.next) {
    printf("%s (matrix of %d rows and %d cols)\n", matrix->name, matrix->rows, matrix->cols);
  }
  for (function = wasora.functions; function != NULL; function = function->hh.next) {
    printf("%s (function of %d arguments)\n", function->name, function->n_arguments);
  }

  return;

}


void wasora_debug(void) {

#ifdef HAVE_READLINE
  fd_set rfds;
  struct timeval tv;
  int retval;
  char *debugcommand = NULL;
  char *token;
  char *argument;
//  char *dummy;
  char *line;
  double watch;
//  var_t *variable;
  vector_t *vector;
  matrix_t *matrix;
  int i, j;
  int new_on_gsl_error, old_on_gsl_error;  
  
  // get back if we are instructed to ignore debug or we are ran in parallel
  if (wasora.mode == mode_ignore_debug || wasora.nprocs > 1) {
    return;
  }
  
  // miramos si tocaron enter
  if (wasora.mode == mode_normal) {

    // a select le pasamos solo stdin (fd = 0) 
    FD_ZERO(&rfds);
    FD_SET(0, &rfds);

    // sin esperar, i.e. miramos si ya esta desbloqueada
    tv.tv_sec = 0;
    tv.tv_usec = 0;  
    retval = select(1, &rfds, NULL, NULL, &tv);        

    if (retval != 0) {
      wasora.mode = mode_debug;
      // TODO: tamanio dinamico! ver ejemplo de readline
      debugcommand = malloc(128);
      if (fgets(debugcommand, 128, stdin) == NULL) {
        return;
      }
    }
  }

  
  if (wasora.cond_breakpoint.n_tokens != 0 && wasora_evaluate_expression(&wasora.cond_breakpoint) != 0) {
    wasora.mode = mode_debug;
    wasora_destroy_expression(&wasora.cond_breakpoint);
    wasora.cond_breakpoint.n_tokens = 0;
  }

  if (wasora.mode == mode_debug || wasora.mode == mode_single_step) {
    do {
      if (debugcommand != NULL) {
        feenox_free(debugcommand);
      }
      debugcommand = readline("#) ");
      if (debugcommand != NULL && debugcommand[0] != '\0') {
        add_history(debugcommand);
        line = strdup(debugcommand);
      }

      token = wasora_get_next_token(debugcommand);

      if (token == NULL || strcmp(token, "step") == 0 || strcmp(token, "s") == 0) {
        wasora.mode = mode_single_step;  
        
      } else if (strcmp(token, "continue") == 0 || strcmp(token, "c") == 0) {
        wasora.mode = mode_normal;
        
      } else if (strcmp(token, "breakpoint") == 0 || strcmp(token, "b") == 0) {
        
        argument = debugcommand + strlen(token)+1;
        
        if (wasora_get_next_token(NULL) != NULL && wasora_parse_expression(argument, &wasora.cond_breakpoint) == 0) {
          fprintf(stderr, "next breakpoint will occur whenever '%s' is not zero\n", argument);
        } else {
          wasora_debug_printerror();
          fprintf(stderr, "no breakpoint set\n");
        }
        wasora.mode = mode_debug;
 
      } else if (strcmp(token, "print") == 0 || strcmp(token, "p") == 0) {
  
        argument = debugcommand + strlen(token)+1;
        
        if ((matrix = wasora_get_matrix_ptr(argument)) != NULL) {
          fprintf(stderr, "%s =\n", argument);
          for (i = 0; i < matrix->rows; i++) {
            fprintf(stderr, "    ");
            for (j = 0; j < matrix->cols; j++) {
              fprintf(stderr, "%e  ", gsl_matrix_get(wasora_value_ptr(matrix), i, j));
            }
            fprintf(stderr, "\n");
          }
        } else if ((vector = wasora_get_vector_ptr(argument)) != NULL) {
          fprintf(stderr, "%s =\n", argument);
          for (i = 0; i < vector->size; i++) {
            fprintf(stderr, "    %g\n", gsl_vector_get(wasora_value_ptr(vector), i));
          }
        } else {
          watch = wasora_evaluate_expression_in_string(argument);
          if (wasora.error_level == 0) {
            fprintf(stderr, "%s = %g\n", argument, watch);
          } else {
            wasora_debug_printerror();
          }
        }
        wasora.mode = mode_debug;
        
      } else if (strcmp(token, "quit") == 0 || strcmp(token, "q") == 0) {
        if (wasora_var(wasora_special_var(done_static)) == 1) {
          wasora_var(wasora_special_var(done_transient)) = 1;
        } else {
          wasora_var(wasora_special_var(done_static)) = 1;
        }
        wasora_var(wasora_special_var(done)) = 1;
        wasora.mode = mode_normal;
        
      } else if (strcmp(token, "help") == 0 || strcmp(token, "h") == 0) {
        fprintf(stderr, "%s", wasora_debug_help); 
        wasora.mode = mode_debug;

      } else if ((token = wasora_get_next_token(NULL)) != NULL && strcmp(token, "=") == 0) {
        
        assignment_t *assignment = calloc(1, sizeof(assignment_t));
        
        
        if(wasora_parse_assignment(line, assignment) == WASORA_PARSER_ERROR) {
          wasora_debug_printerror();
        } else {
          old_on_gsl_error = (int)wasora_var(wasora.special_vars.on_gsl_error);
          new_on_gsl_error = old_on_gsl_error | ON_ERROR_NO_QUIT;
          wasora_var(wasora.special_vars.on_gsl_error) = (double)new_on_gsl_error;
          if (wasora_instruction_assignment(assignment) == WASORA_PARSER_ERROR) {
            wasora_debug_printerror();
          }
          wasora_var(wasora.special_vars.on_gsl_error) = (double)old_on_gsl_error;
        }
        
        wasora_destroy_expression(&assignment->rhs);
        wasora_destroy_expression(&assignment->t_min);
        wasora_destroy_expression(&assignment->t_max);
        wasora_destroy_expression(&assignment->i_min);
        wasora_destroy_expression(&assignment->i_max);
        wasora_destroy_expression(&assignment->j_min);
        wasora_destroy_expression(&assignment->j_max);
        wasora_destroy_expression(&assignment->col);
        wasora_destroy_expression(&assignment->row);
        feenox_free(assignment);
        
        wasora.mode = mode_debug;
        
      } else {
        fprintf(stderr, "unknown command -- enter help to see a list\n");
        wasora.mode = mode_debug;
      }
    } while (wasora.mode == mode_debug);
  }    

#endif
  return;
}
  
void wasora_debug_printerror(void) {
  while (wasora.error_level > 0) {
    fprintf(stderr, "%s%s", wasora.error[wasora.error_level-1], (wasora.error_level != 1)?" ":"\n");
    wasora_pop_error_message();
  }
  return;
}
