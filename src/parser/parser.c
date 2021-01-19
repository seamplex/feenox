/*------------ -------------- -------- --- ----- ---   --       -            -
 *  FeenoX parser
 *
 *  Copyright (C) 2009--2021 jeremy theler
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
#include "parser.h"
feenox_parser_t feenox_parser;

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#if HAVE_SYSCONF
 #include <unistd.h>
#endif
/*
*/
/*
#include <ctype.h>
#include <math.h>
*/
/*
#include <dlfcn.h>

#include "builtin.h"

typedef struct varlist_t varlist_t;
struct varlist_t {
  var_t *var;
  varlist_t *next;
};
*/


int feenox_parse_main_input_file(const char *filepath) {
  
  // check the page size to use a reasonable buffer size
  // we can ask for more if we need
#if HAVE_SYSCONF
  feenox_parser.page_size = (size_t)sysconf(_SC_PAGESIZE);
#else  
  feenox_parser.page_size = 4096;  
#endif
  feenox_parser.actual_buffer_size = feenox_parser.page_size-64;
  
  feenox_parser.line = malloc(feenox_parser.actual_buffer_size);
  feenox_call(feenox_parse_input_file(filepath, 0, 0));
  free(feenox_parser.line);

/*  
  // TODO 
  if (feenox_parser.active_conditional_block != NULL) {
    feenox_push_error_message("conditional block not closed");
    return FEENOX_ERROR;
  }
 */


  return FEENOX_OK;
}



// parse an input file (it can be called recursively to handle INCLUDEs)
int feenox_parse_input_file(const char *filepath, int from, int to) {

  FILE *input_file_stream;
  int line_num, delta_line_num;

  if (filepath == NULL) {
    return FEENOX_OK;
  } else if (strcmp(filepath, "-") == 0) {
    input_file_stream = stdin;
  } else {
    if ((input_file_stream = feenox_fopen(filepath, "r")) == NULL) {
      if (strcmp(filepath, feenox.argv[feenox.optind]) == 0) {
        feenox_push_error_message("input file '%s' could not be opened: %s", filepath, strerror(errno));
      } else {
        feenox_push_error_message("included file '%s' could not be opened: %s", filepath, strerror(errno));
      }
      return FEENOX_ERROR;
    }  
  }

  // line-by-line parser
  line_num = 0;
  while ((delta_line_num = feenox_read_line(input_file_stream)) != 0) {
    // a negative delta_line_num indicates that the line asks for an argument $n
    // which was not provided but this can occur on a line which is ignored
    // by a from/to INCLUDE directive so it is not a catasthropic error
    line_num += abs(delta_line_num);
    
    if (feenox_parser.line[0] != '\0' && ((from == 0 || line_num >= from) && ((to == 0) || line_num <= to))) {
      // we do have to process the line so if delta_line_num is negative, we complain now
      if (delta_line_num < 0) {
        feenox_push_error_message("input file needs at least one more argument in commandline");
        return FEENOX_ERROR;
      }
      
      if (feenox_parse_line() != FEENOX_OK) {
        feenox_push_error_message("%s: %d:", filepath, line_num);
        return FEENOX_ERROR;
      }
    }
  }

  if (strcmp(filepath, "-") != 0) {
    fclose(input_file_stream);
  }
  
  return FEENOX_OK;

}




// feenox line parser
int feenox_parse_line(void) {

  char *token;

  if ((token = feenox_get_next_token(feenox_parser.line)) != NULL) {
    
///kw+INCLUDE+usage INCLUDE
///kw+INCLUDE+desc Include another FeenoX input file.
    if (strcasecmp(token, "INCLUDE") == 0) {
      feenox_call(feenox_parse_include());
      return FEENOX_OK;
      
///kw+ABORT+usage ABORT
///kw+ABORT+desc Catastrophically abort the execution and quit FeenoX.
    } else if (strcasecmp(token, "ABORT") == 0) {
      feenox_call(feenox_parse_abort());
      return FEENOX_OK;
      
///kw+DEFAULT_ARGUMENT_VALUE+usage DEFAULT_ARGUMENT_VALUE
///kw+DEFAULT_ARGUMENT_VALUE+desc Give a default value for an optional commandline argument.
    } else if (strcasecmp(token, "DEFAULT_ARGUMENT_VALUE") == 0) {
      feenox_call(feenox_parse_default_argument_value());
      return FEENOX_OK;
      
///kw+IMPLICIT+usage IMPLICIT
///kw+IMPLICIT+desc Define whether implicit definition of variables is allowed or not.
    } else if (strcasecmp(token, "IMPLICIT") == 0) {
      feenox_call(feenox_parse_implicit());
      return FEENOX_OK;
      
///kw+TIME_PATH+usage TIME_PATH
///kw+TIME_PATH+desc Force time-dependent problems to pass through specific instants of time.
    } else if (strcasecmp(token, "TIME_PATH") == 0) {
      feenox_call(feenox_parse_time_path());
      return FEENOX_OK;
      
///kw+INITIAL_CONDITIONS+usage INITIAL_CONDITIONS
///kw+INITIAL_CONDITIONS+desc Define how initial conditions of DAE problems are computed.
    } else if (strcasecmp(token, "INITIAL_CONDITIONS") == 0 || strcasecmp(token, "INITIAL_CONDITIONS_MODE") == 0) {
      feenox_call(feenox_parse_initial_conditions());
      return FEENOX_OK;
      
///kw+VAR+usage VAR
///kw+VAR+desc Explicitly define one or more scalar variables.
    } else if (strcasecmp(token, "VAR") == 0  || strcasecmp(token, "VARIABLE") == 0 ||
               strcasecmp(token, "VARS") == 0 || strcasecmp(token, "VARIABLES") == 0) {
      feenox_call(feenox_parse_variables());
      return FEENOX_OK;
    
///kw+ALIAS+usage ALIAS
///kw+ALIAS+desc Define a scalar alias of an already-defined indentifier.
    } else if (strcasecmp(token, "ALIAS") == 0) {
      feenox_call(feenox_parse_alias());
      return FEENOX_OK;
      
///kw+VECTOR+usage VECTOR
///kw+VECTOR+desc Define a vector.
    } else if (strcasecmp(token, "VECTOR") == 0) {
      feenox_call(feenox_parse_vector());
      return FEENOX_OK;

///kw+SORT_VECTOR+usage SORT_VECTOR
///kw+SORT_VECTOR+desc Sort the elements of a vector, optionally making the same rearrangement in another vector.
    } else if (strcasecmp(token, "SORT_VECTOR") == 0 || strcasecmp(token, "VECTOR_SORT") == 0) {
      feenox_call(feenox_parse_sort_vector());
      return FEENOX_OK;

///kw+MATRIX+usage MATRIX
///kw+MATRIX+desc Define a matrix.
    } else if (strcasecmp(token, "MATRIX") == 0) {
      feenox_call(feenox_parse_matrix());
      return FEENOX_OK;
      
///kw+FUNCTION+usage FUNCTION
///kw+FUNCTION+desc Define a function of one or more variables.
    } else if (strcasecmp(token, "FUNCTION") == 0) {
      feenox_call(feenox_parse_function());
      return FEENOX_OK;
    
///kw+PRINT+desc Write plain-text and/or formatted data to the standard output or into an output file.
///kw+PRINT+usage PRINT
    } else if (strcasecmp(token, "PRINT") == 0) {
      feenox_call(feenox_parse_print());
      return FEENOX_OK;

///kw+FILE+desc Define a file with a particularly formatted name to be used either as input or as output.
///kw+FILE+usage < FILE | OUTPUT_FILE | INPUT_FILE >
    } else if (strcasecmp(token, "FILE") == 0) {
      feenox_call(feenox_parse_file(NULL));
      return FEENOX_OK;
    } else if (strcasecmp(token, "OUTPUT_FILE") == 0) {
      feenox_call(feenox_parse_file("w"));
      return FEENOX_OK;
    } else if (strcasecmp(token, "INPUT_FILE") == 0) {
      feenox_call(feenox_parse_file("r"));
      return FEENOX_OK;
      
///kw+OPEN+desc Explicitly open a file for input/output.
///kw+OPEN+usage OPEN
    } else if (strcasecmp(token, "OPEN") == 0) {
      feenox_call(feenox_parse_open_close("OPEN"));
      return FEENOX_OK;

///kw+CLOSE+desc Explicitly close a file after input/output.
///kw+CLOSE+usage CLOSE
    } else if (strcasecmp(token, "CLOSE") == 0) {
      feenox_call(feenox_parse_open_close("CLOSE"));
      return FEENOX_OK;

///kw+IF+desc Execute a set of instructions if a condition is met.
///kw+IF+usage IF expr @
///kw+IF+usage  <block_of_instructions_if_expr_is_true> @
    } else if (strcasecmp(token, "IF") == 0) {
      feenox_call(feenox_parse_if());
      return FEENOX_OK;
///kw+IF+usage [ ELSE  @
///kw+IF+usage  <block_of_instructions_if_expr_is_false> ] @
    } else if (strcasecmp(token, "ELSE") == 0) {
      feenox_call(feenox_parse_else());
      return FEENOX_OK;
///kw+IF+usage ENDIF
    } else if (strcasecmp(token, "ENDIF") == 0) {
      feenox_call(feenox_parse_endif());
      return FEENOX_OK;
      
    }
      
  }


      
/*
      

    } else if (strcasecmp(token, "IF") == 0) {


    // ---- ELSE  ----------------------------------------------------
    } else if (strcasecmp(token, "ELSE") == 0) {


// ---- ENDIF  ----------------------------------------------------
    } else if (strcasecmp(token, "ENDIF") == 0) {



// ---- SEMAPHORE ----------------------------------------------------
///kw+SEMAPHORE+desc Perform either a wait or a post operation on a named shared semaphore.
///kw+SEMAPHORE+usage [ SEMAPHORE | SEM ]
    } else if ((strcasecmp(token, "SEMAPHORE") == 0) || (strcasecmp(token, "SEM") == 0)) {

      struct semaphore_t *semaphore;
      semaphore = calloc(1, sizeof(struct semaphore_t));
      LL_APPEND(feenox.semaphores, semaphore);

///kw+SEMAPHORE+usage <name>
      if (feenox_parser_string(&semaphore->name) != FEENOX_OK) {
        return FEENOX_ERROR;
      }

///kw+SEMAPHORE+usage { WAIT | POST }
      char *keywords[] = {"WAIT", "POST", ""};
      int values[] = {feenox_sem_wait, feenox_sem_post, 0};
      feenox_call(feenox_parser_keywords_ints(keywords, values, (int *)&semaphore->operation));

      if (feenox_define_instruction(feenox_instruction_sem, semaphore) == NULL) {
        return FEENOX_ERROR;
      }

      return FEENOX_OK;

// ---- READ / WRITE  ----------------------------------------------------
///kw+READ+desc Read data (variables, vectors o matrices) from files or shared-memory segments.
///kw+WRITE+desc Write data (variables, vectors o matrices) to files or shared-memory segments.
///kw+WRITE+desc See the `READ` keyword for usage details.
///kw+READ+usage [ READ | WRITE ]
    } else if ((strcasecmp(token, "READ") == 0) || (strcasecmp(token, "WRITE") == 0)) {

      io_t *io = calloc(1, sizeof(io_t));
      LL_APPEND(feenox.ios, io);

      if (strcasecmp(token, "READ") == 0) {
        io->direction = io_read;
      } else if (strcasecmp(token, "WRITE") == 0) {
        io->direction = io_write;
      }

      while ((token = feenox_get_next_token(NULL)) != NULL) {
        // ---- SHM_OBJECT ---------------------------------------------------------
///kw+READ+usage [ SHM <name> ]
        if (strcasecmp(token, "SHM") == 0 || strcasecmp(token, "SHM_OBJECT") == 0) {

          io->type = io_shm;

          if (feenox_parser_string(&io->shm_name) != FEENOX_OK) {
            return FEENOX_ERROR;
          }

        // ---- FILE_PATH ----------------------------------------------------
///kw+READ+usage [ { ASCII_FILE_PATH | BINARY_FILE_PATH } <file_path> ]
        } else if (strcasecmp(token, "ASCII_FILE_PATH") == 0 || strcasecmp(token, "BINARY_FILE_PATH") == 0) {

          char mode[2];
          if (io->direction == io_read) {
            sprintf(mode, "r");
          } else if (io->direction == io_write) {
            sprintf(mode, "w");
          }
            
          if (strcasecmp(token, "ASCII_FILE_PATH") == 0) {
            io->type = io_file_ascii;
          } else if (strcasecmp(token, "BINARY_FILE_PATH") == 0) {
            io->type = io_file_binary;
          }

          feenox_call(feenox_parser_file_path(&io->file, mode));

        // ---- FILE ----------------------------------------------------
///kw+READ+usage [ { ASCII_FILE | BINARY_FILE } <identifier> ]
        } else if (strcasecmp(token, "ASCII_FILE") == 0 || strcasecmp(token, "BINARY_FILE") == 0) {

          if (strcasecmp(token, "ASCII_FILE") == 0) {
            io->type = io_file_ascii;
          } else if (strcasecmp(token, "BINARY_FILE") == 0) {
            io->type = io_file_binary;
          }

          if (feenox_parser_file(&io->file) != FEENOX_OK) {
            return FEENOX_ERROR;
          }

          // ---- FILE ----------------------------------------------------
///kw+READ+usage [ IGNORE_NULL ]
          } else if (strcasecmp(token, "IGNORE_NULL") == 0) {

            io->ignorenull = 1;

          } else  {
///kw+READ+usage [ object_1 object_2 ... object_n ]

          // cosas
          io->n_things++;
          io_thing_t *thing = calloc(1, sizeof(io_thing_t));
          LL_APPEND(io->things, thing);

          // si pusieron un corchete quieren un pedazo solamente
          if ((dummy = strchr(token, '<')) != NULL) {
            *dummy = '\0';
          }

          if ((thing->variable = feenox_get_variable_ptr(token)) != NULL) {

            // manzana, despues sumamos uno al tamanio en feenox_io_init()
            ;

          } else if ((thing->vector = feenox_get_vector_ptr(token)) != NULL) {

            if (dummy != NULL) {
              *dummy = '<';
              feenox_call(feenox_parse_range(dummy, '<', ':', '>', &thing->expr_row_min, &thing->expr_row_max));
            }

          } else if ((thing->matrix = feenox_get_matrix_ptr(token)) != NULL) {

            if (dummy != NULL) {
              *dummy = '<';
              feenox_call(feenox_parse_range(dummy, '<', ':', ';', &thing->expr_row_min, &thing->expr_row_max));
              feenox_call(feenox_parse_range(strchr(dummy, ';'), ';', ':', '>', &thing->expr_col_min, &thing->expr_col_max));
            }

 
          } else {
            
            if (feenox_parse_expression(token, &thing->expr) != FEENOX_OK) {
              feenox_push_error_message("undefined keyword, variable, vector, matrix, alias or invalid expression '%s'", token);
              return FEENOX_ERROR;
            }
            
            if (io->direction == io_read) {
              feenox_push_error_message("expressions cannot be used in a READ instruction", token);
              return FEENOX_ERROR;
            }
          
          }
        }
      }


      if (io->type == io_undefined)  {
        feenox_push_error_message("undefined I/O resource type");
        return FEENOX_ERROR;
      }

      if (feenox_define_instruction(feenox_instruction_io, io) == NULL) {
        return FEENOX_ERROR;
      }

      return FEENOX_OK;



// --- PRINT_FUNCTION -----------------------------------------------------
///kw+PRINT_FUNCTION+desc Print one or more functions as a table of values of dependent and independent variables.
///kw+PRINT_FUNCTION+usage PRINT_FUNCTION
///kw+PRINT_FUNCTION+usage <function_1> [ { function_2 | expr_1 } ... { function_n | expr_n-1 } ]
    } else if (strcasecmp(token, "PRINT_FUNCTION") == 0) {

      print_function_t *print_function;
      function_t *dummy_function;

      print_function = calloc(1, sizeof(print_function_t));
      LL_APPEND(feenox.print_functions, print_function);


      while ((token = feenox_get_next_token(NULL)) != NULL) {
///kw+PRINT_FUNCTION+usage [ FILE <file_id> |
        if (strcasecmp(token, "FILE") == 0) {
          feenox_call(feenox_parser_file(&print_function->file));
///kw+PRINT_FUNCTION+usage FILE_PATH <file_path> ]
        } else if (strcasecmp(token, "FILE_PATH") == 0) {
          feenox_call(feenox_parser_file_path(&print_function->file, "w"));

///kw+PRINT_FUNCTION+usage [ HEADER ]
        } else if (strcasecmp(token, "HEADER") == 0) {
          print_function->header = 1;
          
///kw+PRINT_FUNCTION+usage [ MIN <expr_1> <expr_2> ... <expr_m> ]
        } else if (strcasecmp(token, "MIN") == 0) {

          if (print_function->first_function == NULL) {
            feenox_push_error_message("MIN before actual function, cannot determine number of arguments");
            return FEENOX_ERROR;
          }

          if (feenox_parser_expressions(&print_function->range.min, print_function->first_function->n_arguments) != FEENOX_OK) {
            return FEENOX_ERROR;
          }


///kw+PRINT_FUNCTION+usage [ MAX <expr_1> <expr_2> ... <expr_m> ]
        } else if (strcasecmp(token, "MAX") == 0) {

          if (print_function->first_function == NULL) {
            feenox_push_error_message("MAX before actual function, cannot determine number of arguments");
            return FEENOX_ERROR;
          }

          if (feenox_parser_expressions(&print_function->range.max, print_function->first_function->n_arguments) != FEENOX_OK) {
            return FEENOX_ERROR;
          }

///kw+PRINT_FUNCTION+usage [ STEP <expr_1> <expr_2> ... <expr_m> ]
        } else if (strcasecmp(token, "STEP") == 0) {

          if (print_function->first_function == NULL) {
            feenox_push_error_message("STEP before actual function, cannot determine number of arguments");
            return FEENOX_ERROR;
          }

          if (feenox_parser_expressions(&print_function->range.step, print_function->first_function->n_arguments) != FEENOX_OK) {
            return FEENOX_ERROR;
          }

///kw+PRINT_FUNCTION+usage [ NSTEPs <expr_1> <expr_2> ... <expr_m> ]
        } else if (strcasecmp(token, "NSTEPS") == 0) {

          if (print_function->first_function == NULL) {
            feenox_push_error_message("NSTEPS before actual function, cannot determine number of arguments");
            return FEENOX_ERROR;
          }

          if (feenox_parser_expressions(&print_function->range.nsteps, print_function->first_function->n_arguments) != FEENOX_OK) {
            return FEENOX_ERROR;
          }

///kw+PRINT_FUNCTION+usage [ FORMAT <print_format> ]
        } else if (strcasecmp(token, "FORMAT") == 0) {
          if (feenox_parser_string(&print_function->format) != FEENOX_OK) {
            return FEENOX_ERROR;
          }
          
///kw+PRINT_FUNCTION+usage [ PHYSICAL_ENTITY <name> ]
        } else if (strcasecmp(token, "PHYSICAL_ENTITY") == 0) {
          char *name;
          feenox_call(feenox_parser_string(&name));
          if ((print_function->physical_entity = feenox_get_physical_entity_ptr(name, NULL)) == NULL) {
            feenox_push_error_message("unknown physical entity '%s'", name);
            free(name);
            return FEENOX_ERROR;
          }

        } else {

          // agregamos un eslabon a la lista de tokens
          print_token_t *print_token = calloc(1, sizeof(print_token_t));
          LL_APPEND(print_function->tokens, print_token);

          print_token->text = strdup(token);   // nos quedamos con el texto para el header
          
          if ((dummy_function = feenox_get_function_ptr(token)) != NULL) {
            print_token->function = dummy_function;
            if (print_function->first_function == NULL) {
              // es la primer funcion
              print_function->first_function = dummy_function;

            } else if (dummy_function->n_arguments !=  print_function->first_function->n_arguments) {
              feenox_push_error_message("functions do not have the same number of arguments");
              return FEENOX_ERROR;

            }

          } else {
            feenox_call(feenox_parse_expression(token, &print_token->expression));

          }
        }
      }

      if (print_function->first_function == NULL) {
        feenox_push_error_message("at least one function expected");
        return FEENOX_ERROR;
      }
      
      if (print_function->first_function->type == type_algebraic || print_function->first_function->type == type_routine) {
        if (print_function->range.min == NULL) {
          feenox_push_error_message("need MIN keyword (function %s is not point-wise defined)", print_function->first_function->name);
          return FEENOX_ERROR;
        }
        if (print_function->range.max == NULL) {
          feenox_push_error_message("need MAX keyword (function %s is not point-wise defined)", print_function->first_function->name);
          return FEENOX_ERROR;
        }
        if (print_function->range.step == NULL && print_function->range.nsteps == NULL) {
          feenox_push_error_message("need either STEP or NSTEPS keyword (function %s is not point-wise defined)", print_function->first_function->name);
          return FEENOX_ERROR;
        }
      }
      
      if (print_function->separator == NULL) {
        print_function->separator = strdup(DEFAULT_PRINT_SEPARATOR);
      }

      if (print_function->format == NULL) {
        print_function->format = strdup(DEFAULT_PRINT_FORMAT);
      }

      if (print_function->file == NULL) {
        print_function->file = feenox.special_files.stdout_;
      }

      if (feenox_define_instruction(feenox_instruction_print_function, print_function) == NULL) {
        return FEENOX_ERROR;
      }

      return FEENOX_OK;

// --- PRINT_VECTOR -----------------------------------------------------
///kw+PRINT_VECTOR+desc Print the elements of one or more vectors.
///kw+PRINT_VECTOR+usage PRINT_VECTOR
    } else if (strcasecmp(token, "PRINT_VECTOR") == 0) {

      print_vector_t *print_vector;

      print_vector = calloc(1, sizeof(print_vector_t));
      LL_APPEND(feenox.print_vectors, print_vector);

      while ((token = feenox_get_next_token(NULL)) != NULL) {

///kw+PRINT_VECTOR+usage [ FILE <file_id> ]
        if (strcasecmp(token, "FILE") == 0) {
          feenox_call(feenox_parser_file(&print_vector->file));
///kw+PRINT_VECTOR+usage FILE_PATH <file_path> ]
        } else if (strcasecmp(token, "FILE_PATH") == 0) {
          feenox_call(feenox_parser_file_path(&print_vector->file, "w"));

///kw+PRINT_VECTOR+usage [ { VERTICAL | HORIZONTAL } ]
  } else if (strcasecmp(token, "VERTICAL") == 0) {
          print_vector->horizontal = 0;

        } else if (strcasecmp(token, "HORIZONTAL") == 0) {
          print_vector->horizontal = 1;

///kw+PRINT_VECTOR+usage [ ELEMS_PER_LINE <expr> ]
        } else if (strcasecmp(token, "ELEMS_PER_LINE") == 0) {
          print_vector->horizontal = 1;
          if (feenox_parser_expression(&print_vector->elems_per_line) != FEENOX_OK) {
            return FEENOX_ERROR;
          }

///kw+PRINT_VECTOR+usage [ FORMAT <print_format> ]
        } else if (strcasecmp(token, "FORMAT") == 0) {
          if (feenox_parser_string(&print_vector->format) != FEENOX_OK) {
            return FEENOX_ERROR;
          }

        } else {

///kw+PRINT_VECTOR+usage <vector_1> [ vector_2 ... vector_n ]
          // agregamos un eslabon a la lista de tokens
          print_token_t *print_token = calloc(1, sizeof(print_token_t));
          LL_APPEND(print_vector->tokens, print_token);

          if ((print_token->vector = feenox_get_vector_ptr(token)) != NULL) {
            if (print_vector->first_vector == NULL) {
              // es el primer vector
              print_vector->first_vector = print_token->vector;
            } 
            
          } else {
            feenox_call(feenox_parse_expression(token, &print_token->expression));
            
          }
        }
      }


      // llenamos defaults
      if (print_vector->format == NULL) {
        print_vector->format = strdup(DEFAULT_PRINT_FORMAT);
      }

      if (print_vector->separator == NULL) {
        print_vector->separator = strdup(DEFAULT_PRINT_SEPARATOR);
      }

      if (print_vector->file == NULL) {
        print_vector->file = feenox.special_files.stdout_;
      }

      if (feenox_define_instruction(feenox_instruction_print_vector, print_vector) == NULL) {
        return FEENOX_ERROR;
      }

      return FEENOX_OK;
      
// --- SOLVE -----------------------------------------------------
///kw+SOLVE+desc Solve a non-linear system of\ $n$ equations with\ $n$ unknowns.
///kw+SOLVE+example solve1.was solve2.was
///kw+SOLVE+usage SOLVE
      
    } else if (strcasecmp(token, "SOLVE") == 0) {

      double xi;
      int i;
      solve_t *solve;
      solve = calloc(1, sizeof(solve_t));
      LL_APPEND(feenox.solves, solve);

///kw+SOLVE+usage <n> 
      feenox_call(feenox_parser_expression_in_string(&xi));
      if ((solve->n = (int)(xi)) <= 0) {
        feenox_push_error_message("expected a positive number of unknowns instead of %d", solve->n);
        return FEENOX_ERROR;
      }

      solve->unknown = calloc(solve->n, sizeof(var_t *));
      solve->residual = calloc(solve->n, sizeof(expr_t));
      solve->guess = calloc(solve->n, sizeof(expr_t));

      while ((token = feenox_get_next_token(NULL)) != NULL) {
///kw+SOLVE+usage UNKNOWNS <var_1> <var_2> ... <var_n>
        if (strcasecmp(token, "UNKNOWNS") == 0) {
          for (i = 0; i < solve->n; i++) {
            if ((token = feenox_get_next_token(NULL)) == NULL) {
              feenox_push_error_message("expected %d variables and found only %d", solve->n, i);
            }
            if ((solve->unknown[i] = feenox_get_or_define_variable_ptr(token)) == NULL) {
              return FEENOX_ERROR;
            }
          }
          
///kw+SOLVE+usage RESIDUALS <expr_1> <expr_2> ... <expr_n> ]
        } else if (strcasecmp(token, "RESIDUALS") == 0) {
          for (i = 0; i < solve->n; i++) {
            if ((token = feenox_get_next_token(NULL)) == NULL) {
              feenox_push_error_message("expected %d expressions and found only %d", solve->n, i);
            }
            feenox_call(feenox_parse_expression(token, &solve->residual[i]));
          }
      
///kw+SOLVE+usage GUESS <expr_1> <expr_2> ... <expr_n> ]
        } else if (strcasecmp(token, "GUESS") == 0) {
          for (i = 0; i < solve->n; i++) {
            if ((token = feenox_get_next_token(NULL)) == NULL) {
              feenox_push_error_message("expected %d expressions and found only %d", solve->n, i);
            }
            feenox_call(feenox_parse_expression(token, &solve->guess[i]));
          }
      
///kw+SOLVE+usage [ METHOD
        } else if (strcasecmp(token, "METHOD") == 0) {

          if ((token = feenox_get_next_token(NULL)) == NULL) {
            feenox_push_error_message("expected method name");
            return FEENOX_ERROR;
          }

///kw+SOLVE+usage {
///kw+SOLVE+usage dnewton |
          if (strcasecmp(token, "dnewton") == 0) {
            solve->type = gsl_multiroot_fsolver_dnewton;
///kw+SOLVE+usage hybrid |
          } else if (strcasecmp(token, "hybrid") == 0) {
            solve->type = gsl_multiroot_fsolver_hybrid;
///kw+SOLVE+usage hybrids |
          } else if (strcasecmp(token, "hybrids") == 0) {
            solve->type = gsl_multiroot_fsolver_hybrids;
///kw+SOLVE+usage broyden }
          } else if (strcasecmp(token, "broyden") == 0) {
            solve->type = gsl_multiroot_fsolver_hybrid;
          }
///kw+SOLVE+usage ]
          
///kw+SOLVE+usage [ EPSABS <expr> ]
        } else if (strcasecmp(token, "EPSABS") == 0) {

          feenox_call(feenox_parser_expression(&solve->epsabs));

///kw+SOLVE+usage [ EPSREL <expr> ]
        } else if (strcasecmp(token, "EPSREL") == 0) {

          feenox_call(feenox_parser_expression(&solve->epsrel));

///kw+SOLVE+usage [ MAX_ITER <expr> ]
        } else if (strcasecmp(token, "MAX_ITER") == 0) {

          feenox_call(feenox_parser_expression_in_string(&xi));
          if ((solve->max_iter = (int)xi) < 0) {
            feenox_push_error_message("expected a positive integer for MAX_ITER");
            return FEENOX_ERROR;
          }

///kw+SOLVE+usage [ VERBOSE ]
        } else if (strcasecmp(token, "VERBOSE") == 0) {

          solve->verbose = 1;          

        } else {
          feenox_push_error_message("unkown keyword '%s'", token);
          return FEENOX_ERROR;
        }
        
      }
        
      if (solve->residual[0].n_tokens == 0) {
        feenox_push_error_message("no RESIDUALs to solve");
        return FEENOX_ERROR;
      }
      
      if (solve->type == NULL) {
        solve->type = DEFAULT_SOLVE_METHOD;
      }

      if (feenox_define_instruction(feenox_instruction_solve, solve) == NULL) {
        return FEENOX_ERROR;
      }

      return FEENOX_OK;      

// --- M4 -----------------------------------------------------
///kw+M4+desc Call the `m4` macro processor with definitions from feenox variables or expressions.
///kw+M4+usage M4
    } else if (strcasecmp(token, "M4") == 0) {

      m4_t *m4;
      m4 = calloc(1, sizeof(m4_t));
      LL_APPEND(feenox.m4s, m4);

      while ((token = feenox_get_next_token(NULL)) != NULL) {
        
///kw+M4+usage { INPUT_FILE <file_id> |
        if (strcasecmp(token, "INPUT_FILE") == 0) {
          feenox_call(feenox_parser_file(&m4->input_file));
          
///kw+M4+usage FILE_PATH <file_path> }
        } else if (strcasecmp(token, "INPUT_FILE_PATH") == 0) {
          feenox_call(feenox_parser_file_path(&m4->input_file, "r"));

///kw+M4+usage { OUTPUT_FILE <file_id> |
        } else if (strcasecmp(token, "OUTPUT_FILE") == 0) {
          feenox_call(feenox_parser_file(&m4->output_file));
          
///kw+M4+usage OUTPUT_FILE_PATH <file_path> }
        } else if (strcasecmp(token, "OUTPUT_FILE_PATH") == 0) {
          feenox_call(feenox_parser_file_path(&m4->output_file, "w"));

///kw+M4+usage [ EXPAND <name> ] ... }
        } else if (strcasecmp(token, "EXPAND") == 0) {
          
          m4_macro_t *macro = calloc(1, sizeof(m4_macro_t));
          LL_APPEND(m4->macros, macro);
          
          feenox_call(feenox_parser_string(&macro->name));
          macro->print_token.format = strdup(DEFAULT_M4_FORMAT);
          if (feenox_parse_expression(macro->name, &macro->print_token.expression) != FEENOX_OK) {
            feenox_push_error_message("m4 expansion of '%s' failed", macro->name);
            return FEENOX_ERROR;
          }

///kw+M4+usage [ MACRO <name> [ <format> ] <definition> ] ... }
        } else if (strcasecmp(token, "MACRO") == 0) {
          
          m4_macro_t *macro = calloc(1, sizeof(m4_macro_t));
          LL_APPEND(m4->macros, macro);
          
          feenox_call(feenox_parser_string(&macro->name));
          
          if ((token = feenox_get_next_token(NULL)) == NULL) {
            feenox_push_error_message("expected either macro format or definition");
            return FEENOX_ERROR;
          }

          if (token[0] == '%') {
            macro->print_token.format = strdup(token);
            if ((token = feenox_get_next_token(NULL)) == NULL) {
              feenox_push_error_message("expected macro definition");
              return FEENOX_ERROR;
            }
            if (macro->print_token.format[0] == 'd') {
              macro->print_token.format[0] = 'g';
            }
          } else {
            macro->print_token.format = strdup(DEFAULT_M4_FORMAT);
          }

          // si el formato es %s o la expresion no evalua, lo tomamos como string
          if (strcasecmp(macro->print_token.format, "%s") == 0 ||
              feenox_parse_expression(token, &macro->print_token.expression) != FEENOX_OK) {
            macro->print_token.text = strdup(token);
          }
          
        } else {
          feenox_push_error_message("unkown keyword '%s'", token);
          return FEENOX_ERROR;
        }
      }

      if (feenox_define_instruction(feenox_instruction_m4, m4) == NULL) {
        return FEENOX_ERROR;
      }

      return FEENOX_OK;

// --- SHELL -----------------------------------------------------
///kw+SHELL+desc Execute a shell command.
///kw+SHELL+usage SHELL
    } else if (strcasecmp(token, "SHELL") == 0) {

      shell_t *shell;
      shell = calloc(1, sizeof(shell_t));
      LL_APPEND(feenox.shells, shell);

///kw+SHELL+usage <print_format> [ expr_1 expr_2 ... expr_n ]
      if (feenox_parser_string_format(&shell->format, &shell->n_args) != FEENOX_OK) {
        return FEENOX_ERROR;
      }

      shell->arg = calloc(shell->n_args, sizeof(expr_t));
      for (i = 0; i < shell->n_args; i++) {
        if (feenox_parser_expression(&shell->arg[i]) != FEENOX_OK) {
          return FEENOX_ERROR;
        }
      }

      if (feenox_define_instruction(feenox_instruction_shell, shell) == NULL) {
        return FEENOX_ERROR;
      }

      return FEENOX_OK;

// ----- HISTORY  -----------------------------------------------------------------
///kw+HISTORY+desc Record the time history of a variable as a function of time.
///kw+HISTORY+usage HISTORY
    } else if ((strcasecmp(token, "HISTORY") == 0)) {

      history_t *history;
      history = calloc(1, sizeof(history_t));
      LL_APPEND(feenox.histories, history);

///kw+HISTORY+usage <variable>
      // el nombre de la variable
      if (feenox_parser_variable(&history->variable)) {
        return FEENOX_ERROR;
      }

///kw+HISTORY+usage <function>
      // el nombre de la funcion
      if ((token = feenox_get_next_token(NULL)) == NULL) {
        feenox_push_error_message("expected function name");
        return FEENOX_ERROR;
      }

      if ((history->function = feenox_define_function(token, 1)) == NULL) {
        return FEENOX_ERROR;
      }
      // proposed by rvignolo
//      history->function->arg_name = malloc(1 * sizeof(char *));
//      history->function->arg_name[0] = strdup(feenox.special_vars.t->name);

      if (feenox_define_instruction(feenox_instruction_history, history) == NULL) {
        return FEENOX_ERROR;
      }

      return FEENOX_OK;

    // ----- PARAMETRIC  -----------------------------------------------------------------
///kw+PARAMETRIC+desc Systematically sweep a zone of the parameter space, i.e. perform a parametric run.
    } else if ((strcasecmp(token, "PARAMETRIC") == 0)) {

      varlist_t *varlist = NULL;
      varlist_t *varitem, *tmp;
      
      double xi;
      // suponemos que type = linear
      feenox.parametric.type = parametric_linear;
      feenox.parametric_mode = 1;

///kw+PARAMETRIC+usage PARAMETRIC
///kw+PARAMETRIC+usage <var_1> [ ... <var_n> ]
      while ((token = feenox_get_next_token(NULL)) != NULL) {
        
        if (strcasecmp(token, "TYPE") == 0) {
///kw+PARAMETRIC+usage [ TYPE { linear  | logarithmic  | random  | gaussianrandom  | sobol  | niederreiter  | halton  | reversehalton } ]
          char *keywords[] = {"linear", "logarithmic", "random", "gaussianrandom", "sobol", "niederreiter", "halton", "reversehalton", ""};
          int values[] = {parametric_linear,
                          parametric_logarithmic,
                          parametric_random,
                          parametric_gaussianrandom,
                          parametric_sobol,
                          parametric_niederreiter,
                          parametric_halton,
                          parametric_reversehalton,
                          0};
          
          feenox_call(feenox_parser_keywords_ints(keywords, values, (int *)&feenox.parametric.type));
          
///kw+PARAMETRIC+usage [ MIN <num_expr_1> ... <num_expr_n> ]
        } else if (strcasecmp(token, "MIN") == 0) {

          if (feenox.parametric.dimensions == 0) {
            feenox_push_error_message("MIN before variables, cannot determine number of arguments");
            return FEENOX_ERROR;
          }

          feenox_call(feenox_parser_expressions(&feenox.parametric.range.min, feenox.parametric.dimensions));

///kw+PARAMETRIC+usage [ MAX <num_expr_1> ... <num_expr_n> ]
        } else if (strcasecmp(token, "MAX") == 0) {

          if (feenox.parametric.dimensions == 0) {
            feenox_push_error_message("MAX before variables, cannot determine number of arguments");
            return FEENOX_ERROR;
          }

          feenox_call(feenox_parser_expressions(&feenox.parametric.range.max, feenox.parametric.dimensions));
          
///kw+PARAMETRIC+usage [ STEP <num_expr_1> ... <num_expr_n> ]
        } else if (strcasecmp(token, "STEP") == 0) {

          if (feenox.parametric.dimensions == 0) {
            feenox_push_error_message("STEP before variables, cannot determine number of arguments");
            return FEENOX_ERROR;
          }

          feenox_call(feenox_parser_expressions(&feenox.parametric.range.step, feenox.parametric.dimensions));

///kw+PARAMETRIC+usage [ NSTEPS <num_expr_1> ... <num_expr_n> ]
        } else if (strcasecmp(token, "NSTEPS") == 0) {

          if (feenox.parametric.dimensions == 0) {
            feenox_push_error_message("NSTEPS before variables, cannot determine number of arguments");
            return FEENOX_ERROR;
          }

          feenox_call(feenox_parser_expressions(&feenox.parametric.range.nsteps, feenox.parametric.dimensions));

///kw+PARAMETRIC+usage [ OUTER_STEPS <num_expr> ]
        } else if (strcasecmp(token, "OUTER_STEPS") == 0) {

          feenox_call(feenox_parser_expression_in_string(&xi));
          if ((feenox.parametric.outer_steps = (int)xi) <= 0) {
            feenox_push_error_message("OUTER_STEPS has to be positive");
            return FEENOX_ERROR;
          }

///kw+PARAMETRIC+usage [ MAX_DAUGHTERS <num_expr>  ]
        } else if (strcasecmp(token, "MAX_DAUGHTERS") == 0) {

          feenox_call(feenox_parser_expression_in_string(&xi));
          feenox.parametric.max_daughters = (int)(ceil(xi));

///kw+PARAMETRIC+usage [ OFFSET <num_expr> ]
        } else if (strcasecmp(token, "OFFSET") == 0) {
          feenox_call(feenox_parser_expression_in_string(&xi));
          if ((feenox.parametric.offset = (int)(round(xi))) < 0) {
            feenox_push_error_message("expected a non-negative offset");
            return FEENOX_ERROR;
          }

///kw+PARAMETRIC+usage [ ADIABATIC ]
        } else if (strcasecmp(token, "ADIABATIC") == 0) {
            feenox.parametric.adiabatic = 1;

        } else {

          if (feenox.parametric.range.min != NULL ||
              feenox.parametric.range.max != NULL ||
              feenox.parametric.range.step != NULL  ||
              feenox.parametric.range.nsteps != NULL) {

            feenox_push_error_message("already given MIN, MAX, STEP or NSTEPS, cannot add variable '%s' to the parametric list", token);
            return FEENOX_ERROR;
          }
          
          
          varitem = calloc(1, sizeof(varlist_t));
          if ((varitem->var = feenox_get_variable_ptr(token)) == NULL) {
            if ((varitem->var = feenox_define_variable(token)) == NULL) {
              return FEENOX_ERROR;
            }
          } 
          
          LL_APPEND(varlist, varitem);
          feenox.parametric.dimensions++;

        }
      }

      if (feenox.parametric.dimensions == 0) {
        feenox_push_error_message("no parametric variables given");
        return FEENOX_ERROR;
      }
      
      feenox.parametric.variable = calloc(feenox.parametric.dimensions, sizeof(var_t *));
          
      varitem = varlist;
      for (i = 0; i < feenox.parametric.dimensions; i++) {
        feenox.parametric.variable[i] = varitem->var;
        if ((varitem = varitem->next) == NULL && i != feenox.parametric.dimensions-1) {
          feenox_push_error_message("internal mismatch in number of fit parameter %d", i);
          return FEENOX_ERROR;
        }
      }
      
      if (feenox_define_instruction(feenox_instruction_parametric, NULL) == NULL) {
        return FEENOX_ERROR;
      }
      LL_FOREACH_SAFE(varlist, varitem, tmp) {
        LL_DELETE(varlist, varitem);
        free(varitem);
      }
      return FEENOX_OK;

    // ----- FIT  -----------------------------------------------------------------
    } else if ((strcasecmp(token, "FIT") == 0)) {

///kw+FIT+desc Fit a function of one or more arguments to a set of pointwise-defined data.
///kw+FIT+detail The function with the data has to be point-wise defined
///kw+FIT+detail (i.e. a `FUNCTION` read from a file with inline `DATA`).
///kw+FIT+detail The function to be fitted has to be parametrized with at least one of
///kw+FIT+detail the variables provided after the `VIA` keyword.
///kw+FIT+detail Only the names of the functions have to be given, not the arguments.
///kw+FIT+detail Both functions have to have the same number of arguments.
      
///kw+FIT+usage FIT
      char *sigma_name;
      varlist_t *varlist = NULL;
      varlist_t *varitem;

      feenox.fit_mode = 1;

///kw+FIT+usage <function_to_be_fitted> 
      // la funcion cuyos parametros vamos a ajustar
      if ((token = feenox_get_next_token(NULL)) == NULL) {
        feenox_push_error_message("expected function name");
        return FEENOX_ERROR;
      }

      if ((feenox.fit.function = feenox_get_function_ptr(token)) == NULL) {
        if (strchr(token, '(') != NULL) {
          feenox_push_error_message("function '%s' undefined (only the function name is to be provided, do not include its arguments)", token);
        } else {
          feenox_push_error_message("function '%s' undefined", token);
        }
        return FEENOX_ERROR;
      }

///kw+FIT+usage TO <function_with_data>
      if ((token = feenox_get_next_token(NULL)) == NULL) {
        feenox_push_error_message("expected function name");
        return FEENOX_ERROR;
      }
      if (strcasecmp(token, "TO") != 0) {
        feenox_push_error_message("expected keyword 'TO' instead of '%s'", token);
      }

// la funcion con los datos experimentales
      if ((token = feenox_get_next_token(NULL)) == NULL) {
        feenox_push_error_message("expected function name");
        return FEENOX_ERROR;
      }
      if ((feenox.fit.data = feenox_get_function_ptr(token)) == NULL) {
        if (strchr(token, '(') != NULL) {
          feenox_push_error_message("function '%s' undefined (only the function name is to be provided, do not include its arguments)", token);
        } else {
          feenox_push_error_message("function '%s' undefined", token);
        }
        return FEENOX_ERROR;
      }

      if (feenox.fit.function->n_arguments != feenox.fit.data->n_arguments) {
        feenox_push_error_message("function '%s' has %d arguments and '%s' has %d", feenox.fit.function->name, feenox.fit.function->n_arguments, feenox.fit.data->name, feenox.fit.data->n_arguments);
        return FEENOX_ERROR;
      }

      if ((feenox.fit.n = feenox.fit.data->data_size) == 0) {
        feenox_push_error_message("function '%s' has to be point-wise defined", feenox.fit.data->name);
        return FEENOX_ERROR;
      }

///kw+FIT+usage VIA
///kw+FIT+detail The initial guess of the solution is given by the initial value of the variables listed in the `VIA` keyword.
      if ((token = feenox_get_next_token(NULL)) == NULL) {
        feenox_push_error_message("expected function name");
        return FEENOX_ERROR;
      }
      if (strcasecmp(token, "VIA") != 0) {
        feenox_push_error_message("expected keyword 'VIA' instead of '%s'", token);
        return FEENOX_ERROR;
      }

///kw+FIT+usage <var_1> <var_2> ... <var_n>@
      while ((token = feenox_get_next_token(NULL)) != NULL) {

        // TODO: convergencia por gradiente, incertezas, covarianza, incrementos para gradiente automatico
///kw+FIT+usage [ GRADIENT <expr_1> <expr_2> ... <expr_n> ]@
///kw+FIT+detail Analytical expressions for the gradient of the function to be fitted with respect
///kw+FIT+detail to the parameters to be fitted can be optionally given with the `GRADIENT` keyword.
///kw+FIT+detail If none is provided, the gradient will be computed numerically using finite differences.
        if (strcasecmp(token, "GRADIENT") == 0) {
 
          if (feenox.fit.p == 0) {
            feenox_push_error_message("GRADIENT keyword before parameters");
            return FEENOX_ERROR;
          }
          
          feenox.fit.gradient = malloc(feenox.fit.p * sizeof(expr_t));
          for (i = 0; i < feenox.fit.p; i++) {
            if ((token = feenox_get_next_token(NULL)) == NULL) {
              feenox_push_error_message("expected an expression");
              return FEENOX_ERROR;
            }

            feenox_call(feenox_parse_expression(token, &feenox.fit.gradient[i]));
          }

///kw+FIT+usage [ RANGE_MIN <expr_1> <expr_2> ... <expr_j> ]@
///kw+FIT+detail A range over which the residuals are to be minimized can be given with `RANGE_MIN` and `RANGE_MAX`.
///kw+FIT+detail The expressions give the range of the arguments of the functions, not of the parameters.
///kw+FIT+detail For multidimensional fits, the range is an hypercube.
///kw+FIT+detail If no range is given, all the definition points of the function with the data are used for the fit.
        } else if (strcasecmp(token, "RANGE_MIN") == 0) {

          if (feenox.fit.data->n_arguments == 0) {
            feenox_push_error_message("RANGE_MIN before target function, cannot determine number of arguments");
            return FEENOX_ERROR;
          }

          feenox_call(feenox_parser_expressions(&feenox.fit.range.min, feenox.fit.data->n_arguments));

///kw+FIT+usage [ RANGE_MAX <expr_1> <expr_2> ... <expr_n> ]@
        } else if (strcasecmp(token, "RANGE_MAX") == 0) {

          if (feenox.fit.data->n_arguments == 0) {
            feenox_push_error_message("RANGE_MAX before target function, cannot determine number of arguments");
            return FEENOX_ERROR;
          }

          feenox_call(feenox_parser_expressions(&feenox.fit.range.max, feenox.fit.data->n_arguments));
          
///kw+FIT+usage [ DELTAEPSREL <expr> ]
///kw+FIT+detail Convergence can be controlled by giving the relative and absolute tolreances with
///kw+FIT+detail `DELTAEPSREL` (default `DEFAULT_NLIN_FIT_EPSREL`) and `DELTAEPSABS` (default `DEFAULT_NLIN_FIT_EPSABS`),
///kw+FIT+detail and with the maximum number of iterations `MAX_ITER` (default DEFAULT_NLIN_FIT_MAX_ITER).
          
        } else if (strcasecmp(token, "DELTAEPSREL") == 0) {

          feenox_call(feenox_parser_expression(&feenox.fit.deltaepsrel));

///kw+FIT+usage [ DELTAEPSABS <expr> ]
        } else if (strcasecmp(token, "DELTAEPSABS") == 0) {

          feenox_call(feenox_parser_expression(&feenox.fit.deltaepsabs));

///kw+FIT+usage [ MAX_ITER <expr> ]@
        } else if (strcasecmp(token, "MAX_ITER") == 0) {

          double xi;
          
          feenox_call(feenox_parser_expression_in_string(&xi));
          if ((feenox.fit.max_iter = (int)xi) < 0) {
            feenox_push_error_message("expected a positive integer for MAX_ITER");
            return FEENOX_ERROR;
          }

///kw+FIT+usage [ VERBOSE ]
        } else if (strcasecmp(token, "VERBOSE") == 0) {
///kw+FIT+detail If the optional keyword `VERBOSE` is given, some data of the intermediate steps is written in the standard output.

          feenox.fit.verbose = 1;

///kw+FIT+usage [ RERUN | DO_NOT_RERUN ]@
        } else if (strcasecmp(token, "DO_NOT_RERUN") == 0 || strcasecmp(token, "NORERUN") == 0) {

          feenox.fit.norerun = 1;

        } else if (strcasecmp(token, "RERUN") == 0) {

          feenox.fit.norerun = 0;

        } else {

          varitem = calloc(1, sizeof(varlist_t));
          if ((varitem->var = feenox_get_variable_ptr(token)) == NULL) {
            feenox_push_error_message("unknown variable '%s'", token);
            return FEENOX_ERROR;
          } 
          
          LL_APPEND(varlist, varitem);
          feenox.fit.p++;

        }
      }

      if (feenox.fit.p == 0) {
        feenox_push_error_message("no fit parameters given");
        return FEENOX_ERROR;
      }

      feenox.fit.param = malloc(feenox.fit.p * sizeof(var_t *));
      feenox.fit.sigma = malloc(feenox.fit.p * sizeof(var_t *));
      varitem = varlist;
      for (i = 0; i < feenox.fit.p; i++) {
        feenox.fit.param[i] = varitem->var;
        
        sigma_name = malloc(strlen(varitem->var->name)+strlen("sigma_")+8);
        snprintf(sigma_name, strlen(varitem->var->name)+strlen("sigma_")+8, "sigma_%s", varitem->var->name);
        if ((feenox.fit.sigma[i] = feenox_define_variable(sigma_name)) == NULL) {
          return FEENOX_ERROR;
        }
        free(sigma_name);
        
        if ((varitem = varitem->next) == NULL && i != feenox.fit.p-1) {
          feenox_push_error_message("internal mismatch in number of fit parameter %d", i);
          return FEENOX_ERROR;
        }
      }

      if (feenox.fit.algorithm == NULL) {
        feenox.fit.algorithm = DEFAULT_NLIN_FIT_METHOD;
      }
      if (feenox.fit.max_iter == 0) {
        feenox.fit.max_iter = DEFAULT_NLIN_FIT_MAX_ITER;
      }

      return FEENOX_OK;

    // ----- MINIMIZE  -----------------------------------------------------------------
///kw+MINIMIZE+usage MINIMIZE
///kw+MINIMIZE+desc Find the combination of arguments that give a (relative) minimum of a function.
    } else if (strcasecmp(token, "MINIMIZE") == 0 || strcasecmp(token, "OPTIMIZE") == 0) {

      // ponemos esto en true porque normalmente no queremos que imprima todo el chorizo
      feenox.min_mode = 1;

///kw+MINIMIZE+usage <function>@
///kw+FIT+detail The combination of arguments that minimize the function are computed and stored in the variables.
///kw+FIT+detail So if `f(x,y)` is to be minimized, after a `MINIMIZE f` both `x` and `y` would have the appropriate values.
      if ((token = feenox_get_next_token(NULL)) == NULL) {
        feenox_push_error_message("expected a function");
        return FEENOX_ERROR;
      }
      if ((feenox.min.function = feenox_get_function_ptr(token)) == NULL) {
        if (strchr(token, '(') != NULL) {
          feenox_push_error_message("function '%s' undefined (only the function name should be provided, do not include its arguments)", token);
        } else {
          feenox_push_error_message("function '%s' undefined", token);
        }
        return FEENOX_ERROR;
      }

      feenox.min.n = feenox.min.function->n_arguments;
      feenox.min.x = calloc(feenox.min.n, sizeof(var_t *));
      for (i = 0; i < feenox.min.n; i++) {
        feenox.min.x[i] = feenox.min.function->var_argument[i];
      }

      while ((token = feenox_get_next_token(NULL)) != NULL) {
///kw+MINIMIZE+usage [ METHOD {
///kw+FIT+detail The details of the method used can be found in [GSL’s documentation](https:\/\/www.gnu.org/software/gsl/doc/html/multimin.html).
///kw+FIT+detail Some of them use derivatives and some of them do not.
///kw+FIT+detail Default method is `DEFAULT_MINIMIZER_METHOD`, which does not need derivatives.
        if (strcasecmp(token, "METHOD") == 0 || strcasecmp(token, "ALGORITHM") == 0) {
          if ((token = feenox_get_next_token(NULL)) == NULL) {
            feenox_push_error_message("expected algorithm name");
            return FEENOX_ERROR;
          }

///kw+MINIMIZE+usage nmsimplex2 |
          } else if (strcasecmp(token, "nmsimplex2") == 0) {
            feenox.min.f_type = gsl_multimin_fminimizer_nmsimplex2;
///kw+MINIMIZE+usage nmsimplex |
          } else if (strcasecmp(token, "nmsimplex") == 0) {
            feenox.min.f_type = gsl_multimin_fminimizer_nmsimplex;
///kw+MINIMIZE+usage nmsimplex2rand |
          } else if (strcasecmp(token, "nmsimplex2rand") == 0) {
            feenox.min.f_type = gsl_multimin_fminimizer_nmsimplex2rand;
///kw+MINIMIZE+usage conjugate_fr |
          if (strcasecmp(token, "conjugate_fr") == 0) {
            feenox.min.fdf_type = gsl_multimin_fdfminimizer_conjugate_fr;
///kw+MINIMIZE+usage conjugate_pr |
          } else if (strcasecmp(token, "conjugate_pr") == 0) {
            feenox.min.fdf_type = gsl_multimin_fdfminimizer_conjugate_pr;
///kw+MINIMIZE+usage vector_bfgs2 |
          } else if (strcasecmp(token, "vector_bfgs2") == 0) {
            feenox.min.fdf_type = gsl_multimin_fdfminimizer_vector_bfgs2;
///kw+MINIMIZE+usage vector_bfgs |
          } else if (strcasecmp(token, "vector_bfgs") == 0) {
            feenox.min.fdf_type = gsl_multimin_fdfminimizer_vector_bfgs;
///kw+MINIMIZE+usage steepest_descent}@
          } else if (strcasecmp(token, "steepest_descent") == 0) {
            feenox.min.fdf_type = gsl_multimin_fdfminimizer_steepest_descent;
          } else {
            feenox_push_error_message("unknown minimization method '%s'", token);
            return FEENOX_ERROR;
          }

///kw+MINIMIZE+usage [ GRADIENT <expr_1> <expr_2> ... <expr_n> ]@
        } else if (strcasecmp(token, "GRADIENT") == 0) {

          feenox.min.gradient = malloc(feenox.min.n * sizeof(expr_t));
          for (i = 0; i < feenox.min.n; i++) {
            if ((token = feenox_get_next_token(NULL)) == NULL) {
              feenox_push_error_message("expected an expression");
              return FEENOX_ERROR;
            }

            feenox_call(feenox_parse_expression(token, &feenox.min.gradient[i]));
          }

///kw+MINIMIZE+usage [ GUESS <expr_1> <expr_2> ... <expr_n> ]@
        } else if (strcasecmp(token, "GUESS") == 0) {

          feenox.min.guess = malloc(feenox.min.n * sizeof(expr_t));
          for (i = 0; i < feenox.min.n; i++) {
            if ((token = feenox_get_next_token(NULL)) == NULL) {
              feenox_push_error_message("expected an expression");
              return FEENOX_ERROR;
            }

            feenox_call(feenox_parse_expression(token, &feenox.min.guess[i]));
          }

///kw+MINIMIZE+usage [ MIN <expr_1> <expr_2> ... <expr_n> ]@
        } else if (strcasecmp(token, "MIN") == 0) {

          if (feenox.min.n == 0) {
            feenox_push_error_message("MIN before target function, cannot determine number of arguments");
            return FEENOX_ERROR;
          }

          if (feenox_parser_expressions(&feenox.min.range.min, feenox.min.n) != FEENOX_OK) {
            return FEENOX_ERROR;
          }

///kw+MINIMIZE+usage [ MAX <expr_1> <expr_2> ... <expr_n> ]@
        } else if (strcasecmp(token, "MAX") == 0) {

          if (feenox.min.n == 0) {
            feenox_push_error_message("MAX before target function, cannot determine number of arguments");
            return FEENOX_ERROR;
          }

          if (feenox_parser_expressions(&feenox.min.range.max, feenox.min.n) != FEENOX_OK) {
            return FEENOX_ERROR;
          }

///kw+MINIMIZE+usage [ STEP <expr_1> <expr_2> ... <expr_n> ]@
        } else if (strcasecmp(token, "STEP") == 0) {

          if (feenox.min.n == 0) {
            feenox_push_error_message("STEP before target function, cannot determine number of arguments");
            return FEENOX_ERROR;
          }
          
          // los algoritmos con derivadas necesitan un solo step
          // los sin derivadas necesitan n steps
          // si fdf_type no es null, entonces estamos con derivadas
          // igual despues chequeamos que no se mezclen
          feenox.min.n_steps = (feenox.min.f_type != NULL) ? feenox.min.n : 1;
          feenox.min.range.step = malloc(feenox.min.n_steps * sizeof(expr_t));

          if (feenox_parser_expressions(&feenox.min.range.step, feenox.min.n_steps) != FEENOX_OK) {
            return FEENOX_ERROR;
          }
        } else if (strcasecmp(token, "VERBOSE") == 0) {
          feenox.min.verbose = 1;

        } else if (strcasecmp(token, "NORERUN") == 0) {
          feenox.min.norerun = 1;

        } else {

///kw+MINIMIZE+usage [ MAX_ITER <expr> ]
///kw+MINIMIZE+usage [ TOL <expr> ]
///kw+MINIMIZE+usage [ GRADTOL <expr> ]@
///kw+MINIMIZE+usage [ VERBOSE ]
///kw+MINIMIZE+usage [ NORERUN ]@
          // en una sola linea para el generador del lexer de pygments
          char *keywords[] = {"MAX_ITER", "TOL", "GRADTOL", ""};
          //, "POPULATION", "GA_STEPS"};
          expr_t *expressions[] = {
            &feenox.min.max_iter,
            &feenox.min.tol,
            &feenox.min.gradtol,
//            &feenox.min.population,
//            &feenox.min.ga_steps
            NULL,
          };

          if (feenox_parser_match_keyword_expression(token, keywords, expressions, sizeof(expressions)/sizeof(expr_t *)) == WASORA_PARSER_UNHANDLED) {
            feenox_push_error_message("unknown keyword '%s'", token);
            return FEENOX_ERROR;
          }
        }
      }


      if (feenox.min.function == NULL && feenox.min.siman_Efunc == NULL) {
        feenox_push_error_message("nothing to optimize!");
        return FEENOX_ERROR;
      }

      if (feenox.min.genetic != 0) {
        if (feenox.min.range.min == NULL) {
          feenox_push_error_message("need a MIN keyword for genetic algorithms");
          return FEENOX_ERROR;
        }

        if (feenox.min.range.max == NULL) {
          feenox_push_error_message("need a MAX keyword for genetic algorithms");
          return FEENOX_ERROR;
        }
      }

      if (feenox.min.fdf_type == NULL && feenox.min.f_type == NULL
       && feenox.min.siman_type == NULL && feenox.min.genetic == 0) {
        feenox.min.f_type = DEFAULT_MINIMIZER_METHOD;
      }

      if (feenox.min.siman_Efunc != NULL) {
        if (feenox.min.siman_init == NULL) {
          feenox_push_error_message("a siman init routine has to be provided");
          return FEENOX_ERROR;
        }
        if (feenox.min.siman_step == NULL) {
          feenox_push_error_message("a siman step routine has to be provided");
          return FEENOX_ERROR;
        }
        if (feenox.min.siman_copy == NULL) {
          feenox_push_error_message("a siman copy routine has to be provided");
          return FEENOX_ERROR;
        }
        if (feenox.min.siman_copy_construct == NULL) {
          feenox_push_error_message("a siman copy_construct routine has to be provided");
          return FEENOX_ERROR;
        }
        if (feenox.min.siman_destroy == NULL) {
          feenox_push_error_message("a siman destroy routine has to be provided");
          return FEENOX_ERROR;
        }
      }

      return FEENOX_OK;

    // ----- PHASE_SPACE -----------------------------------------------------------
    } else if (strcasecmp(token, "PHASE_SPACE") == 0) {
///kw+PHASE_SPACE+usage PHASE_SPACE { <vars> | <vectors> | <matrices> }
///kw+PHASE_SPACE+desc Define which variables, vectors and/or matrices belong to the phase space of the DAE system to be solved.

      var_t *variable;
      vector_t *vector;
      matrix_t *matrix;
      char *buffer;

      if (feenox_dae.dimension != 0) {
        feenox_push_error_message("PHASE_SPACE keyword already given");
        return FEENOX_ERROR;
      }
#ifdef HAVE_IDA
      if (sizeof(realtype) != sizeof(double)) {
        feenox_push_error_message("\nSUNDIALS was compiled using a different word size than feenox.\nPlease recompile with double precision floating point arithmetic.");
        return FEENOX_ERROR;
      }
#endif

      i = 0;
      while ((token = feenox_get_next_token(NULL)) != NULL) {

        phase_object_t *phase_object = calloc(1, sizeof(phase_object_t));
        LL_APPEND(feenox_dae.phase_objects, phase_object);

        if ((vector = feenox_get_vector_ptr(token)) != NULL) {

          expr_t *size_expr;
          
          phase_object->offset = i;
          phase_object->vector = vector;
          phase_object->name = vector->name;

          buffer = malloc(strlen(vector->name)+8);
          sprintf(buffer, "%s_dot", vector->name);
          
          // chapuceria, aca habria que duplicar un expr_t completo
          size_expr = malloc(sizeof(expr_t));
          feenox_parse_expression(vector->size_expr->string, size_expr);
          phase_object->vector_dot = feenox_define_vector(buffer, vector->size, size_expr, NULL);
          
          free(buffer);

        } else if ((matrix = feenox_get_matrix_ptr(token)) != NULL) {

          expr_t *rows_expr;
          expr_t *cols_expr;
          
          phase_object->offset = i;
          phase_object->matrix = matrix;
          phase_object->name = matrix->name;

          buffer = malloc(strlen(matrix->name)+8);
          sprintf(buffer, "%s_dot", matrix->name);
          
          // chapuceria, aca habria que duplicar los expr_ts completos
          rows_expr = malloc(sizeof(expr_t));
          feenox_parse_expression(matrix->rows_expr->string, rows_expr);
          cols_expr = malloc(sizeof(expr_t));
          feenox_parse_expression(matrix->cols_expr->string, cols_expr);
          
          phase_object->matrix_dot = feenox_define_matrix(buffer, matrix->rows, rows_expr, matrix->cols, cols_expr, NULL);
          
          free(buffer);

        } else {

          if ((variable = feenox_get_or_define_variable_ptr(token)) == NULL) {
            return FEENOX_ERROR;
          }

          phase_object->variable = variable;
          phase_object->name = variable->name;

          buffer = malloc(strlen(variable->name)+8);
          sprintf(buffer, "%s_dot", variable->name);
          phase_object->variable_dot = feenox_define_variable(buffer);
          free(buffer);

        }
      }

      return FEENOX_OK;
              
    // ----- DIFFERENTIAL -----------------------------------------------------------
    } else if (strcasecmp(token, "DIFFERENTIAL") == 0) {
///kw+DIFFERENTIAL+desc Explicitly mark variables, vectors or matrices as “differential” to compute intial conditions of DAE systems.
///kw+DIFFERENTIAL+usage DIFFERENTIAL { <var_1> <var_2> ... | <vector_1> <vector_2> ... | <matrix_1> <matrix_2> ... }
      
      phase_object_t *phase_object;
      
      while ((token = feenox_get_next_token(NULL)) != NULL) {
        LL_FOREACH(feenox_dae.phase_objects, phase_object) {
          if ((phase_object->variable != NULL && strcmp(token, phase_object->variable->name) == 0) ||
              (phase_object->vector   != NULL && strcmp(token, phase_object->vector->name) == 0) ||
              (phase_object->matrix   != NULL && strcmp(token, phase_object->matrix->name) == 0) ) {
            phase_object->differential = 1;
          }
        }          
      }
      
      return FEENOX_OK;
      
      // --- DAE -----------------------------------------------------
    } else if (token[0] == '0' || strstr(feenox.line, ".=") != NULL) {
///kw+_.=+desc Add an equation to the DAE system to be solved in the phase space spanned by `PHASE_SPACE`.
///kw+_.=+usage { 0[(i[,j]][<imin:imax[;jmin:jmax]>] | <expr1> } .= <expr2>
      char *equation;
      char *dummy;
      char *bracket;
      phase_object_t *phase_object;
      
      dae_t *dae = calloc(1, sizeof(dae_t));
      
      if (feenox_dae.daes == NULL) {
        // si es la primera dae, anotamos a que instruccion corresponde
        if ((feenox_dae.instruction = feenox_define_instruction(&feenox_instruction_dae, NULL)) == NULL) {
          return FEENOX_ERROR;
        }
        // y ponemos una finite-state machine en modo "leyendo daes"
//        feenox_dae.reading_daes = 1;
      }
      
      
      LL_APPEND(feenox_dae.daes, dae);

      if (token[0] == '0') {
        // ecuacion escrita en forma implicita
        dae->equation_type |= EQN_FLAG_IMPLICIT;
        if ((dummy = strchr((char *)(token+strlen(token)+1), '=')) == NULL) {
          feenox_push_error_message("expecting equal sign after keyword 0");
          return FEENOX_ERROR;
        }
        equation = strdup(dummy+1);

        if (strncmp(token, "0(i,j)", 6) == 0) {
          //  ecuacion implicita y matricial
          dae->equation_type |= EQN_FLAG_MATRICIAL;
          if ((dae->matrix = feenox_get_first_matrix(equation)) == NULL) {
            feenox_push_error_message("matrix equations need at least one matrix in the expression");
            return FEENOX_ERROR;
          }
          
          if (token[6] == '<') {
            feenox_call(feenox_parse_range(token+6, '<', ':', ';', &dae->expr_i_min, &dae->expr_i_max));
            feenox_call(feenox_parse_range(strchr(dummy+6, ';'), ';', ':', '>', &dae->expr_j_min, &dae->expr_j_max));
          }
          
        } else if (strncmp(token, "0(i)", 4) == 0) {
          // ecuacion implicita y vectorial
          dae->equation_type |= EQN_FLAG_VECTORIAL;
          if ((dae->vector = feenox_get_first_vector(equation)) == NULL) {
            feenox_push_error_message("vectorial equations need at least one vector in the expression");
            return FEENOX_ERROR;
          }

          if (token[4] == '<') {
            feenox_call(feenox_parse_range(token+4, '<', ':', '>', &dae->expr_i_min, &dae->expr_i_max));
          }
          
        } else {
          dae->equation_type |= EQN_FLAG_SCALAR;
        }
        
        

      } else if (strstr(feenox.line, ".=") != NULL) {

        // el tipo puso una ecuacion explicita
        dae->equation_type |= EQN_FLAG_EXPLICIT;

        if (strchr((char *)(token+strlen(token)+1), '=') == NULL) {
          feenox_push_error_message("syntax error, the equal sign should come right after the dot with no spaces between them");
          return FEENOX_ERROR;
        }
        dummy = strchr((char *)(token+strlen(token)+1), '=')+1;
        equation = malloc(strlen(dummy)+strlen(token)+16);

        // si en el miembro izquierdo hay un vector o una matriz, entonces araca
        if ((dae->vector = feenox_get_first_vector(token)) != NULL) {
          dae->equation_type |= EQN_FLAG_VECTORIAL;
          if ((bracket = strchr(token, '<')) != NULL) {
            feenox_call(feenox_parse_range(bracket, '<', ':', '>', &dae->expr_i_min, &dae->expr_i_max));
            *bracket = '\0';
          }
          
        } else if ((dae->matrix = feenox_get_first_matrix(token)) != NULL) {
          dae->equation_type |= EQN_FLAG_MATRICIAL;
          if ((bracket = strchr(token, '<')) != NULL) {
            feenox_call(feenox_parse_range(bracket, '<', ':', ';', &dae->expr_i_min, &dae->expr_i_max));
            feenox_call(feenox_parse_range(strchr(bracket, ';'), ';', ':', '>', &dae->expr_j_min, &dae->expr_j_max));
            *bracket = '\0';
          }
          
        } else {
          dae->equation_type |= EQN_FLAG_SCALAR;
        }

        // pasamos el miembro derecho para el otro lado cambiado de signo
        sprintf(equation, "(%s)-(%s)", token, dummy);

      } else {
        return FEENOX_ERROR;
      }


      // vemos si es diferencial o algebraica: si aparece alguna derivada en la
      // expresion implicita, es diferencial
      if ((dummy = feenox_get_first_dot(equation)) != NULL) {
        int found = 0;
        dae->equation_type |= EQN_FLAG_DIFFERENTIAL;
        LL_FOREACH(feenox_dae.phase_objects, phase_object) {
          if (strcmp(dummy, phase_object->name) == 0) {
            phase_object->differential = 1;
            found = 1;
          }
        }
        free(dummy);
        if (found == 0) {
          feenox_push_error_message("requested derivative of object '%s' but it is not in the phase space", dummy);
          return FEENOX_ERROR;
        }

      } else {
        dae->equation_type |= EQN_FLAG_ALGEBRAIC;
      }

      // la ecuacion propiamente dicha      
      feenox_call(feenox_parse_expression(equation, &dae->residual));

      free(equation);

      return FEENOX_OK;

    // ----- ASSIGNMENT -----------------------------------------------------------
    } else if ((token = feenox_get_next_token(NULL)) != NULL && strcmp(token, "=") == 0) {
///kw+=+desc Assign an expression to a variable, a vector or a matrix.
///kw+=+usage <var>[ [<expr_tmin>, <expr_tmax>] | @<expr_t> ] = <expr>
///kw+=+usage <vector>(<expr_i>)[<expr_i_min, expr_i_max>] [ [<expr_tmin>, <expr_tmax>] | @<expr_t> ] = <expr>
///kw+=+usage <matrix>(<expr_i>,<expr_j>)[<expr_i_min, expr_i_max; expr_j_min, expr_j_max>] [ [<expr_tmin>, <expr_tmax>] | @<expr_t> ] = <expr>

      // metemos de prepo un nuevo elemento en la linked list
      assignment_t *assignment = calloc(1, sizeof(assignment_t));
      LL_APPEND(feenox.assignments, assignment);
      
      feenox_call(feenox_parse_assignment(feenox.line, assignment));
      
      if (feenox_define_instruction(feenox_instruction_assignment, assignment) == NULL) {
        return FEENOX_ERROR;
      }

      return FEENOX_OK;

    } else {
      // si no entendimos la linea, probamos pasarsela al parser de mallas
      strcpy(line, feenox.line);
      return feenox_mesh_parse_line(line);    
    }

  }
*/
  feenox_push_error_message("unknown keyword '%s'", feenox_parser.line);
  return FEENOX_ERROR;

}


char *feenox_get_nth_token(char *string, int n) {

  char *backup;
  char *token;
  char *desired_token;
  int i;

  backup = strdup(string);

  if ((token = strtok(backup, UNQUOTED_DELIM)) == NULL) {
    free(backup);
    return NULL;
  }
  i = 1;

  while (i < n) {
    if ((token = strtok(NULL, UNQUOTED_DELIM)) == NULL) {
      free(backup);
      return NULL;
    }
    i++;
  }

  desired_token = strdup(token);
  free(backup);

  return desired_token;


}



int feenox_parse_include(void) {
  
  
///kw+INCLUDE+detail Includes the input file located in the string `file_path` at the current location.
///kw+INCLUDE+detail The effect is the same as copying and pasting the contents of the included file
///kw+INCLUDE+detail at the location of the `INCLUDE` keyword. The path can be relative or absolute.
///kw+INCLUDE+detail Note, however, that when including files inside `IF` blocks that instructions are
///kw+INCLUDE+detail conditionally-executed but all definitions (such as function definitions) are processed at
///kw+INCLUDE+detail parse-time independently from the evaluation of the conditional.
///kw+INCLUDE+detail The included file has to be an actual file path (i.e. it cannot be a FeenoX `FILE`)
///kw+INCLUDE+detail because it needs to be resolved at parse time. Yet, the name can contain a
///kw+INCLUDE+detail commandline replacement argument such as `$1` so `INCLUDE $1.fee` will include the
///kw+INCLUDE+detail file specified after the main input file in the command line.

  char *token;
  char *filepath;
  double xi;
  int from = 0;
  int to = 0;
    
///kw+INCLUDE+usage <file_path>
  if ((filepath = feenox_get_next_token(NULL)) == NULL) {
    feenox_push_error_message("expected file path");
    return FEENOX_ERROR;
  }

  while ((token = feenox_get_next_token(NULL)) != NULL) {
///kw+INCLUDE+usage [ FROM <num_expr> ]
///kw+INCLUDE+detail The optional `FROM` and `TO` keywords can be used to include only portions of a file.          
    if (strcasecmp(token, "FROM") == 0) {
      feenox_call(feenox_parser_expression_in_string(&xi));
      if ((from = (int)(xi)) <= 0) {
        feenox_push_error_message("expected a positive line number for FROM instead of '%s'", token);
        return FEENOX_ERROR;
      }
///kw+INCLUDE+usage [ TO <num_expr> ]
    } else if (strcasecmp(token, "TO") == 0) {
      feenox_call(feenox_parser_expression_in_string(&xi));
      if ((to = (int)(xi)) <= 0) {
        feenox_push_error_message("expected a positive line number for TO instead of '%s'", token);
        return FEENOX_ERROR;
      }
    }
  }

  feenox_call(feenox_parse_input_file(filepath, from, to));

  return FEENOX_OK;
}


int feenox_parse_default_argument_value(void) {
  
///kw+DEFAULT_ARGUMENT_VALUE+detail If a `$n` construction is found in the input file but the
///kw+DEFAULT_ARGUMENT_VALUE+detail commandline argument was not given, the default behavior is to
///kw+DEFAULT_ARGUMENT_VALUE+detail fail complaining that an extra argument has to be given in the
///kw+DEFAULT_ARGUMENT_VALUE+detail commandline. With this keyword, a default value can be assigned if
///kw+DEFAULT_ARGUMENT_VALUE+detail no argument is given, thus avoiding the failure and making the argument
///kw+DEFAULT_ARGUMENT_VALUE+detail optional.
///kw+DEFAULT_ARGUMENT_VALUE+detail The `<constant>` should be 1, 2, 3, etc. and `<string>` will be expanded
///kw+DEFAULT_ARGUMENT_VALUE+detail character-by-character where the `$n` construction is. 
///kw+DEFAULT_ARGUMENT_VALUE+detail Whether the resulting expression is to be interpreted as a string or as a
///kw+DEFAULT_ARGUMENT_VALUE+detail numerical expression will depend on the context.
  

  char *token;
  int n;

///kw+DEFAULT_ARGUMENT_VALUE+usage <constant>
  if ((token = feenox_get_next_token(NULL)) == NULL) {
    feenox_push_error_message("expected argument number for DEFAULT_ARGUMENT_VALUE");
    return FEENOX_ERROR;
  }

  if ((n = (int)feenox_evaluate_expression_in_string(token)) <= 0) {
    feenox_push_error_message("expected a positive value instead of 'token", token);
    return FEENOX_ERROR;
  }

///kw+DEFAULT_ARGUMENT_VALUE+usage <string>
  if ((token = feenox_get_next_token(NULL)) == NULL) {
    feenox_push_error_message("expected value for DEFAULT_ARUGMENT_VALUE number %d", n);
    return FEENOX_ERROR;
  }

  if ((feenox.optind+n) >= feenox.argc) {
    if ((feenox.argv = realloc(feenox.argv, sizeof(char *)*(feenox.optind+n + 1))) == NULL) {
      feenox_push_error_message("failed to allocate memory for DEFAULT_ARGUMENT_VALUE");
      return FEENOX_ERROR;
    }
    feenox.argc = feenox.optind+n + 1;
    feenox.argv[feenox.optind+n] = strdup(token);
  }
  
  return FEENOX_OK;
}

int feenox_parse_abort(void) {
  
///kw+ABORT+detail Whenever the instruction `ABORT` is executed, FeenoX quits with a non-zero error leve.
///kw+ABORT+detail It does not close files nor unlock shared memory objects.
///kw+ABORT+detail The objective of this instruction is to either debug complex input files
///kw+ABORT+detail by using only parts of them or to conditionally abort the execution using `IF` clauses.
  
  feenox_call(feenox_add_instruction(feenox_instruction_abort, NULL));

  return FEENOX_OK;
}



int feenox_parse_implicit(void) {
  
///kw+IMPLICIT+detail By default, FeenoX allows variables (but not vectors nor matrices) to be
///kw+IMPLICIT+detail implicitly declared. To avoid introducing errors due to typos, explicit
///kw+IMPLICIT+detail declaration of variables can be forced by giving `IMPLICIT NONE`.
///kw+IMPLICIT+detail Whether implicit declaration is allowed or explicit declaration is required
///kw+IMPLICIT+detail depends on the last `IMPLICIT` keyword given, which by default is `ALLOWED`.
///kw+IMPLICIT+example implicit.was

///kw+IMPLICIT+usage { NONE | ALLOWED }
  char *keywords[] = {"NONE", "ALLOWED", ""};
  int values[] = {1, 0, 0};
  feenox_call(feenox_parser_keywords_ints(keywords, values, &feenox_parser.implicit_none));

  return FEENOX_OK;
}  


int feenox_parse_time_path(void) {
  
///kw+TIME_PATH+detail The time step `dt` will be reduced whenever the distance between
///kw+TIME_PATH+detail the current time `t` and the next expression in the list is greater
///kw+TIME_PATH+detail than `dt` so as to force `t` to coincide with the expressions given.
///kw+TIME_PATH+detail The list of expresssions should evaluate to a sorted list of values for all times.          

  char *token;
  
///kw+TIME_PATH+usage <expr_1> [ <expr_2>  [ ... <expr_n> ] ]
  while ((token = feenox_get_next_token(NULL)) != NULL) {
    feenox_call(feenox_add_time_path(token));
  }

  // y apuntamos el current al primero
  feenox.time_path_current = feenox.time_paths;

  return FEENOX_OK;
}

int feenox_parse_initial_conditions(void) {
 
///kw+INITIAL_CONDITIONS+detail In DAE problems, initial conditions may be either:
///kw+INITIAL_CONDITIONS+detail @
///kw+INITIAL_CONDITIONS+detail  * equal to the provided expressions (`AS_PROVIDED`)@
///kw+INITIAL_CONDITIONS+detail  * the derivatives computed from the provided phase-space variables (`FROM_VARIABLES`)@
///kw+INITIAL_CONDITIONS+detail  * the phase-space variables computed from the provided derivatives (`FROM_DERIVATIVES`)@
///kw+INITIAL_CONDITIONS+detail @
///kw+INITIAL_CONDITIONS+detail In the first case, it is up to the user to fulfill the DAE system at\ $t = 0$.
///kw+INITIAL_CONDITIONS+detail If the residuals are not small enough, a convergence error will occur.
///kw+INITIAL_CONDITIONS+detail The `FROM_VARIABLES` option means calling IDA’s `IDACalcIC` routine with the parameter `IDA_YA_YDP_INIT`. 
///kw+INITIAL_CONDITIONS+detail The `FROM_DERIVATIVES` option means calling IDA’s `IDACalcIC` routine with the parameter IDA_Y_INIT.
///kw+INITIAL_CONDITIONS+detail Wasora should be able to automatically detect which variables in phase-space are differential and
///kw+INITIAL_CONDITIONS+detail which are purely algebraic. However, the [`DIFFERENTIAL`] keyword may be used to explicitly define them.
///kw+INITIAL_CONDITIONS+detail See the (SUNDIALS documentation)[https:/\/computation.llnl.gov/casc/sundials/documentation/ida_guide.pdf] for further information.
  
///kw+INITIAL_CONDITIONS+usage { AS_PROVIDED | FROM_VARIABLES | FROM_DERIVATIVES }
  char *keywords[] = {"AS_PROVIDED",
                      "FROM_VARIABLES",
                      "FROM_DERIVATIVES", ""};
  int values[] = {initial_conditions_as_provided,
                  initial_conditions_from_variables,
                  initial_conditions_from_derivatives, 0};
  feenox_call(feenox_parser_keywords_ints(keywords, values, (int *)(&feenox.dae.initial_conditions_mode)));

  return FEENOX_OK;

}

int feenox_parse_variables(void) {
  
  char *token;

///kw+VAR+detail When implicit definition is allowed (see [`IMPLICIT`]), scalar variables
///kw+VAR+detail need not to be defined before being used if from the context FeenoX can tell
///kw+VAR+detail that an scalar variable is needed. For instance, when defining a function like
///kw+VAR+detail `f(x) = x^2` it is not needed to declare `x` explictly as a scalar variable.
///kw+VAR+detail But if one wants to define a function like `g(x) = integral(f(x'), x', 0, x)` then
///kw+VAR+detail the variable `x'` needs to be explicitly defined as `VAR x'` before the integral. 

///kw+VAR+usage <name_1> [ <name_2> ] ... [ <name_n> ]
  
  while ((token = feenox_get_next_token(NULL)) != NULL) {
    feenox_call(feenox_define_variable(token));
  }

  return FEENOX_OK;
  
}


int feenox_parse_alias(void) {

///kw+ALIAS+detail The existing object can be a variable, a vector element or a matrix element. 
///kw+ALIAS+detail In the first case, the name of the variable should be given as the existing object.
///kw+ALIAS+detail In the second case, to alias the second element of vector `v` to the new name `new`, `v(2)` should be given as the existing object.
///kw+ALIAS+detail In the third case, to alias second element (2,3) of matrix `M` to the new name `new`, `M(2,3)` should be given as the existing object.
      
  char *left;
  char *keyword;
  char *right;
  char *existing_object;
  char *new_name;
  
  char *dummy_openpar = NULL;
  char *dummy_comma = NULL;
  char *dummy_closepar = NULL;
  char *row = NULL;
  char *col = NULL;
  
  
///kw+ALIAS+usage { <new_var_name> IS <existing_object> | <existing_object> AS <new_name> }
  feenox_call(feenox_parser_string(&left));
  feenox_call(feenox_parser_string(&keyword));
  feenox_call(feenox_parser_string(&right));

  if (strcasecmp(keyword, "IS") == 0) {
    new_name = left;
    existing_object = right;
  } else if (strcasecmp(keyword, "AS") == 0) {
    new_name = right;
    existing_object = left;
  } else {
    feenox_push_error_message("either IS or AS expected instead of '%s'", keyword);
    return FEENOX_ERROR;
  }

  // if there are parenthesis they are sub-indexes
  if ((dummy_openpar = strchr(existing_object, '(')) != NULL) {
    dummy_comma = strchr(existing_object, ',');
    if ((dummy_closepar = strchr(existing_object, ')')) == NULL) {
      feenox_push_error_message("expecting closing parenthesis in expression '%s'", existing_object);
    }
    
    *dummy_openpar = '\0';
    *dummy_closepar = '\0';
    row = dummy_openpar + 1;
    if (dummy_comma != NULL) {
      *dummy_comma = '\0';
      col = dummy_comma + 1;
    }
  }
  
  feenox_call(feenox_define_alias(new_name, existing_object, row, col));
  
  free(left);
  free(keyword);
  free(right);
      
  return FEENOX_OK;
}

int feenox_parse_vector(void) {
  char *token = NULL;
  char *name = NULL;
  char *size = NULL;
  char *function_data = NULL;
  char *dummy_openpar = NULL;
  char *dummy_closepar = NULL;
  expr_t *datas = NULL;

///kw+VECTOR+detail A new vector of the prescribed size is defined. The size can be an expression which will be
///kw+VECTOR+detail evaluated the very first time the vector is used and then kept at that constant value.
  
///kw+VECTOR+usage <name>
  feenox_call(feenox_parser_string(&name));
  
  // if there are parenthesis, the size is between them
  if ((dummy_openpar = strchr(name, '(')) != NULL) {
    if ((dummy_closepar = strchr(name, ')')) == NULL) {
      feenox_push_error_message("expecting closing parenthesis in expression '%s'", name);
    }
    *dummy_openpar = '\0';
    *dummy_closepar = '\0';
    size = dummy_openpar+1;
  }

  while ((token = feenox_get_next_token(NULL)) != NULL) {

///kw+VECTOR+usage SIZE <expr>
    if (strcasecmp(token, "SIZE") == 0) {
      feenox_call(feenox_parser_string(&size));

///kw+VECTOR+detail If the keyword `FUNCTION_DATA` is given, the elements of the vector will be synchronized
///kw+VECTOR+detail with the inpedendent values of the function, which should be point-wise defined.
///kw+VECTOR+detail The sizes of both the function and the vector should match.
      
///kw+VECTOR+usage [ FUNCTION_DATA <function> ]
    } else if (strcasecmp(token, "FUNCTION_DATA") == 0) {
      feenox_call(feenox_parser_string(&function_data));
    

///kw+VECTOR+detail All elements will be initialized to zero unless `DATA` is given (which should be the last keyword of the line),
///kw+VECTOR+detail in which case the expressions will be evaluated the very first time the vector is used
///kw+VECTOR+detail and assigned to each of the elements.
///kw+VECTOR+detail If there are less elements than the vector size, the remaining values will be zero.
///kw+VECTOR+detail If there are more elements than the vector size, the values will be ignored.
///kw+VECTOR+usage [ DATA <expr_1> <expr_2> ... <expr_n> |
    } else if (strcasecmp(token, "DATA") == 0) {
      while ((token = feenox_get_next_token(NULL)) != NULL) {
        expr_t *data = calloc(1, sizeof(expr_t));
        feenox_call(feenox_expression_parse(data, token));
        LL_APPEND(datas, data);
      }
    }
  }

  if (size == NULL) {
    feenox_push_error_message("vector '%s' does not have a size", name);
    return FEENOX_ERROR;
  }
  
  feenox_call(feenox_define_vector(name, size));
  
  if (function_data != NULL) {
    feenox_call(feenox_vector_attach_function(name, function_data));
  }
  if (datas != NULL) {
    feenox_call(feenox_vector_attach_data(name, datas));
  }

  free(name);
  if (dummy_openpar == NULL) {
    free(size);
  }
  free(function_data);
    
  return FEENOX_OK;
      
}

int feenox_parse_matrix(void) {
  char *token = NULL;
  char *name = NULL;
  char *rows = NULL;
  char *cols = NULL;
  char *dummy_openpar = NULL;
  char *dummy_comma = NULL;
  char *dummy_closepar = NULL;
  expr_t *datas = NULL;

///kw+MATRIX+detail A new matrix of the prescribed size is defined. The number of rows and columns can be an expression which will be
///kw+MATRIX+detail evaluated the very first time the matrix is used and then kept at those constant values.
  
///kw+MATRIX+usage <name>
  feenox_call(feenox_parser_string(&name));
  
  // if there are parenthesis, the size is between them
  if ((dummy_openpar = strchr(name, '(')) != NULL) {
    if ((dummy_comma = strchr(name, ',')) == NULL) {
      feenox_push_error_message("expecting comma in expression '%s'", name);
    }
    if ((dummy_closepar = strchr(name, ')')) == NULL) {
      feenox_push_error_message("expecting closing parenthesis in expression '%s'", name);
    }
    *dummy_openpar = '\0';
    *dummy_comma = '\0';
    *dummy_closepar = '\0';
    rows = dummy_openpar+1;
    cols = dummy_comma+1;
  }

  while ((token = feenox_get_next_token(NULL)) != NULL) {

///kw+MATRIX+usage ROWS <expr>
    if (strcasecmp(token, "ROWS") == 0) {
      feenox_call(feenox_parser_string(&rows));
///kw+MATRIX+usage COLS <expr>
    } else if (strcasecmp(token, "COLS") == 0) {
      feenox_call(feenox_parser_string(&cols));

///kw+MATRIX+detail All elements will be initialized to zero unless `DATA` is given (which should be the last keyword of the line),
///kw+MATRIX+detail in which case the expressions will be evaluated the very first time the matrix is used
///kw+MATRIX+detail and row-major-assigned to each of the elements.
///kw+MATRIX+detail If there are less elements than the matrix size, the remaining values will be zero.
///kw+MATRIX+detail If there are more elements than the matrix size, the values will be ignored.
///kw+MATRIX+usage [ DATA <expr_1> <expr_2> ... <expr_n> |
    } else if (strcasecmp(token, "DATA") == 0) {
      while ((token = feenox_get_next_token(NULL)) != NULL) {
        expr_t *data = calloc(1, sizeof(expr_t));
        feenox_call(feenox_expression_parse(data, token));
        LL_APPEND(datas, data);
      }
    }
  }

  if (rows == NULL) {
    feenox_push_error_message("matrix '%s' does not have a row size", name);
    return FEENOX_ERROR;
  }
  if (cols == NULL) {
    feenox_push_error_message("matrix '%s' does not have a column size", name);
    return FEENOX_ERROR;
  }
  
  feenox_call(feenox_define_matrix(name, rows, cols));
  
  if (datas != NULL) {
    feenox_call(feenox_matrix_attach_data(name, datas));
  }

  free(name);
  if (dummy_openpar == NULL) {
    free(rows);
    free(cols);
  }
    
  return FEENOX_OK;
      
}


int feenox_parse_function(void) {

  char *token = NULL;
  char *dummy_openpar = NULL;
  char *arguments = NULL;
  char *name = NULL;
  
  char **arg_name = NULL;
  int n_arguments;
  int i;

///kw+FUNCTION+usage <function_name>(<var_1>[,var2,...,var_n])
///kw+FUNCTION+detail The number of variables $n$ is given by the number of arguments given between parenthesis after the function name.
///kw+FUNCTION+detail The arguments are defined as new variables if they had not been already defined explictly as scalar variables.
  feenox_call(feenox_parser_string(&name));
  
  if ((dummy_openpar = strchr(name, '(')) == NULL) {
    feenox_push_error_message("expected opening parenthesis '(' after function name '%s' (no spaces allowed)", token);
    return FEENOX_ERROR;
  }

  arguments = strdup(dummy_openpar);
  feenox_strip_blanks(arguments);
  if ((n_arguments = feenox_count_arguments(arguments, NULL)) <= 0) {
    return FEENOX_ERROR;
  }
  feenox_call(feenox_read_arguments(arguments, n_arguments, &arg_name, NULL));
  free(arguments);

  // we can now define the function and set the arguments
  *dummy_openpar = '\0';
  feenox_call(feenox_define_function(name, n_arguments));
  for (i = 0; i < n_arguments; i++) {
    feenox_call(feenox_function_set_argument_variable(name, i, arg_name[i]));
  }
  
  while ((token = feenox_get_next_token(NULL)) != NULL && strcasecmp(token, "DATA") != 0) {
///kw+FUNCTION+usage {
///kw+FUNCTION+usage [ = <expr> |
///kw+FUNCTION+detail If the function is given as an algebraic expression, the short-hand operator `:=` can be used.
///kw+FUNCTION+detail That is to say, `FUNCTION f(x) = x^2` is equivalent to `f(x) := x^2`.

    if (strcasecmp(token, "=") == 0 || strcasecmp(token, ":=") == 0) {
      feenox_call(feenox_function_set_expression(name, token + ((token[0] == ':')? 3 : 2)));
      break;    // we are done with the while() over the line
    }
  }

/*      
///kw+FUNCTION+usage FILE_PATH <file_path> |
    } else if (strcasecmp(token, "FILE_PATH") == 0) {
///kw+FUNCTION+detail If a `FILE_PATH` is given, an ASCII file containing at least $n+1$ columns is expected.
///kw+FUNCTION+detail By default, the first $n$ columns are the values of the arguments and the last column is the value of the function at those points.
///kw+FUNCTION+detail The order of the columns can be changed with the keyword `COLUMNS`, which expects $n+1$ expressions corresponding to the column numbers.

      // TODO: poder pedir que se refresquen los datos en cada paso
      function->type = type_pointwise_file;
      feenox_parser_string(&function->data_file);

///kw+FUNCTION+usage ROUTINE <name> |
    } else if (strcasecmp(token, "ROUTINE") == 0) {
///kw+FUNCTION+detail A function of type `ROUTINE` calls an already-defined user-provided routine using the `CALL` keyword and passes the values of the variables in each required evaluation as a `double *` argument.

      function->type = type_routine;

      if ((token = feenox_get_next_token(NULL)) == NULL) {
        feenox_push_error_message("expected routine name");
        return FEENOX_ERROR;
      }

      if ((function->routine = feenox_get_routine_ptr(token)) == NULL) {
        feenox_push_error_message("undefined routine '%s'", token);
        return FEENOX_ERROR;
      }

///kw+FUNCTION+usage | MESH <name> { DATA <new_vector_name> | VECTOR <existing_vector_name> } { NODES | CELLS } |
///kw+FUNCTION+detail If `MESH` is given, the definition points are the nodes or the cells of the mesh.
///kw+FUNCTION+detail The function arguments should be $(x)$, $(x,y)$ or $(x,y,z)$ matching the dimension the mesh.
///kw+FUNCTION+detail If the keyword `DATA` is used, a new empty vector of the appropriate size is defined.
///kw+FUNCTION+detail The elements of this new vector can be assigned to the values of the function at the $i$-th node or cell.
///kw+FUNCTION+detail If the keyword `VECTOR` is used, the values of the dependent variable are taken to be the values of the already-existing vector.
///kw+FUNCTION+detail Note that this vector should have the size of the number of nodes or cells the mesh has, depending on whether `NODES` or `CELLS` is given.

    } else if (strcasecmp(token, "MESH") == 0) {

      if ((token = feenox_get_next_token(NULL)) == NULL) {
        feenox_push_error_message("expected mesh name");
        return FEENOX_ERROR;
      }

      if ((function->mesh = feenox_get_mesh_ptr(token)) == NULL) {
        feenox_push_error_message("undefined mesh '%s'", token);
        return FEENOX_ERROR;
      }

      if ((token = feenox_get_next_token(NULL)) == NULL) {
        feenox_push_error_message("expected keyword DATA or VECTOR");
        return FEENOX_ERROR;
      }

      if (strcasecmp(token, "DATA") == 0) {
        if ((token = feenox_get_next_token(NULL)) == NULL) {
          feenox_push_error_message("expected data name");
          return FEENOX_ERROR;
        }
        if ((function->vector_value = feenox_define_vector(token, 0, NULL, NULL)) == NULL) {
          return FEENOX_ERROR;
        }
      } else if (strcasecmp(token, "VECTOR") == 0) {
        if ((token = feenox_get_next_token(NULL)) == NULL) {
          feenox_push_error_message("expected vector name");
          return FEENOX_ERROR;
        }
        if ((function->vector_value = feenox_define_vector(token, 0, NULL, NULL)) == NULL) {
          return FEENOX_ERROR;
        }
      } else {
        feenox_push_error_message("expected keyword DATA or VECTOR instead of '%s'", token);
        return FEENOX_ERROR;
      }

      if ((token = feenox_get_next_token(NULL)) == NULL) {
        feenox_push_error_message("expected either NODES or CELLS");
        return FEENOX_ERROR;
      }
      if (strcasecmp(token, "NODES") == 0) {
        function->type = type_pointwise_mesh_node;
      } else if (strcasecmp(token, "CELLS") == 0) {
        function->type = type_pointwise_mesh_cell;
      } else {
        feenox_push_error_message("expected either NODES or CELLS instead of '%s'", token);
        return FEENOX_ERROR;
      }


      if (function->var_argument[0] != feenox_mesh.vars.x) {
        feenox_push_error_message("first argument of function '%s' should be 'x'", function->name);
        return FEENOX_ERROR;
      }
      if (function->n_arguments > 1 && function->var_argument[1] != feenox_mesh.vars.y) {
        feenox_push_error_message("second argument of function '%s' should be 'y'", function->name);
        return FEENOX_ERROR;
      }
      if (function->n_arguments > 2 && function->var_argument[2] != feenox_mesh.vars.z) {
        feenox_push_error_message("third argument of function '%s' should be 'z'", function->name);
        return FEENOX_ERROR;
      }
      if (function->n_arguments > 3) {
        feenox_push_error_message("function '%s' cannot be MESH and have more than 3 arguments", function->name);
        return FEENOX_ERROR;
      }

///kw+FUNCTION+usage [ VECTOR_DATA <vector_1> <vector_2> ... <vector_n> <vector_n+1> ]
///kw+FUNCTION+usage }
///kw+FUNCTION+detail If `VECTOR_DATA` is given, a set of $n+1$ vectors of the same size is expected.
///kw+FUNCTION+detail The first $n+1$ correspond to the arguments and the last one is the function value.

    } else if (strcasecmp(token, "VECTOR_DATA") == 0 || strcasecmp(token, "VECTORS") == 0) {

      function->type = type_pointwise_vector;
      function->vector_argument = calloc(nargs, sizeof(vector_t *));
      function->data_argument = calloc(nargs, sizeof(double *));
      function->data_size = 0;

      for (i = 0; i < nargs; i++) {
        if (feenox_parser_vector(&function->vector_argument[i]) != FEENOX_OK) {
          return FEENOX_ERROR;
        }
      }

      if (feenox_parser_vector(&function->vector_value) != FEENOX_OK) {
        return FEENOX_ERROR;
      }


///kw+FUNCTION+usage [COLUMNS <expr_1> <expr_2> ... <expr_n> <expr_n+1> ]
    } else if (strcasecmp(token, "COLUMNS") == 0) {

      if (nargs == 0) {
        feenox_push_error_message("number of arguments is equal to zero");
        return FEENOX_ERROR;
      }

      for (i = 0; i < nargs+1; i++) {
        if ((token = feenox_get_next_token(NULL)) == NULL) {
          feenox_push_error_message("expected %d columns specifications", nargs);
          return FEENOX_ERROR;
        }
        function->column[i] = (int)(feenox_evaluate_expression_in_string(token));

      }

///kw+FUNCTION+usage [ INTERPOLATION
///kw+FUNCTION+detail Interpolation schemes can be given for either one or multi-dimensional functions with `INTERPOLATION`.
    } else if (strcasecmp(token, "INTERPOLATION") == 0) {

      if ((token = feenox_get_next_token(NULL)) == NULL) {
        feenox_push_error_message("expected interpolation method");
        return FEENOX_ERROR;
      }

///kw+FUNCTION+detail Available schemes for $n=1$ are:
///kw+FUNCTION+detail @
///kw+FUNCTION+usage {
///kw+FUNCTION+usage linear |
///kw+FUNCTION+detail  * linear
      if (strcasecmp(token, "linear") == 0) {
        function->interp_type = *gsl_interp_linear;
///kw+FUNCTION+usage polynomial |
///kw+FUNCTION+detail  * polynomial, the grade is equal to the number of data minus one
      } else if (strcasecmp(token, "polynomial") == 0) {
        function->interp_type = *gsl_interp_polynomial;
///kw+FUNCTION+usage spline |
///kw+FUNCTION+detail  * spline, cubic (needs at least 3 points)
      } else if (strcasecmp(token, "spline") == 0 || strcasecmp(token, "cspline") == 0 || strcasecmp(token, "splines") == 0) {
        function->interp_type = *gsl_interp_cspline;
///kw+FUNCTION+usage spline_periodic |
///kw+FUNCTION+detail  * spline_periodic 
      } else if (strcasecmp(token, "spline_periodic") == 0 || strcasecmp(token, "cspline_periodic") == 0 || strcasecmp(token, "splines_periodic") == 0) {
        function->interp_type = *gsl_interp_cspline_periodic;
///kw+FUNCTION+usage akima |
///kw+FUNCTION+detail  * akima (needs at least 5 points)
      } else if (strcasecmp(token, "akima") == 0) {
        function->interp_type = *gsl_interp_akima;
///kw+FUNCTION+usage akima_periodic |
///kw+FUNCTION+detail  * akima_periodic (needs at least 5 points)
      } else if (strcasecmp(token, "akima_periodic") == 0) {
        function->interp_type = *gsl_interp_akima_periodic;

///kw+FUNCTION+usage steffen |
///kw+FUNCTION+detail  * steffen, always-monotonic splines-like (available only with GSL >= 2.0)
      } else if (strcasecmp(token, "steffen") == 0) {
#if (GSL_MAJOR_VERSION < 2)  
//            feenox_push_error_message("inerpolation steffen is available only for GSL >= 2.0, you have version %s", GSL_VERSION);
        function->interp_type = *gsl_interp_linear;
#else
        function->interp_type = *gsl_interp_steffen;
#endif
///kw+FUNCTION+detail @ 
///kw+FUNCTION+detail Default interpolation scheme for one-dimensional functions is `DEFAULT_INTERPOLATION`.
///kw+FUNCTION+detail @ 
///kw+FUNCTION+detail Available schemes for $n>1$ are:
///kw+FUNCTION+detail @
///kw+FUNCTION+usage nearest |
///kw+FUNCTION+detail  * nearest, $f(\vec{x})$ is equal to the value of the closest definition point
      } else if (strcasecmp(token, "nearest") == 0) {
        function->multidim_interp = nearest;
///kw+FUNCTION+usage shepard |
///kw+FUNCTION+detail  * shepard, [inverse distance weighted average definition points](https:/\/en.wikipedia.org/wiki/Inverse_distance_weighting) (might lead to inefficient evaluation)
      } else if (strcasecmp(token, "shepard") == 0) {
        function->multidim_interp = shepard;
///kw+FUNCTION+usage shepard_kd |
///kw+FUNCTION+detail  * shepard_kd, [average of definition points within a kd-tree](https:/\/en.wikipedia.org/wiki/Inverse_distance_weighting#Modified_Shepard&#39;s_method) (more efficient evaluation provided `SHEPARD_RADIUS` is set to a proper value)
      } else if (strcasecmp(token, "shepard_kd") == 0 || strcasecmp(token, "modified_shepard") == 0) {
        function->multidim_interp = shepard_kd;
///kw+FUNCTION+usage bilinear
///kw+FUNCTION+detail  * bilinear, only available if the definition points configure an structured hypercube-like grid. If $n>3$, `SIZES` should be given.
      } else if (strcasecmp(token, "bilinear") == 0 || strcasecmp(token, "rectangle") == 0 || strcasecmp(token, "rectangular") == 0) {
        function->multidim_interp = bilinear;
///kw+FUNCTION+usage } ]
      } else {
        feenox_push_error_message("undefined interpolation method '%s'", token);
        return FEENOX_ERROR;
      }
///kw+FUNCTION+detail @

///kw+FUNCTION+usage [ INTERPOLATION_THRESHOLD <expr> ]
///kw+FUNCTION+detail For $n>1$, if the euclidean distance between the arguments and the definition points is smaller than `INTERPOLATION_THRESHOLD`, the definition point is returned and no interpolation is performed.
///kw+FUNCTION+detail Default value is square root of `DEFAULT_MULTIDIM_INTERPOLATION_THRESHOLD`.
    } else if (strcasecmp(token, "INTERPOLATION_THRESHOLD") == 0) {

       feenox_parser_expression(&function->expr_multidim_threshold);

///kw+FUNCTION+usage [ SHEPARD_RADIUS <expr> ]
///kw+FUNCTION+detail The initial radius of points to take into account in `shepard_kd` is given by `SHEPARD_RADIUS`. If no points are found, the radius is double until at least one definition point is found.
///kw+FUNCTION+detail The radius is doubled until at least one point is found.
///kw+FUNCTION+detail Default is `DEFAULT_SHEPARD_RADIUS`.
    } else if (strcasecmp(token, "SHEPARD_RADIUS") == 0) {

       feenox_parser_expression(&function->expr_shepard_radius);

///kw+FUNCTION+usage [ SHEPARD_EXPONENT <expr> ]
///kw+FUNCTION+detail The exponent of the `shepard` method is given by `SHEPARD_EXPONENT`.
///kw+FUNCTION+detail Default is `DEFAULT_SHEPARD_EXPONENT`.
    } else if (strcasecmp(token, "SHEPARD_EXPONENT") == 0) {
       feenox_parser_expression(&function->expr_shepard_exponent);

///kw+FUNCTION+usage [ SIZES <expr_1> <expr_2> ... <expr_n> ]
///kw+FUNCTION+detail When requesting `bilinear` interpolation for $n>3$, the number of definition points for each argument variable has to be given with `SIZES`,
    } else if (strcasecmp(token, "SIZES") == 0) {

      function->expr_rectangular_mesh_size = calloc(function->n_arguments, sizeof(expr_t));

      for (i = 0; i < function->n_arguments; i++) {
        feenox_call(feenox_parser_expression(&function->expr_rectangular_mesh_size[i]));
      }

///kw+FUNCTION+usage [ X_INCREASES_FIRST <expr> ]
///kw+FUNCTION+detail and wether the definition data is sorted with the first argument changing first (`X_INCREASES_FIRST` evaluating to non-zero) or with the last argument changing first (zero).
    } else if (strcasecmp(token,"X_INCREASES_FIRST") == 0) {

      feenox_call(feenox_parser_expression(&function->expr_x_increases_first));

    } else {
      feenox_push_error_message("unknown keyword '%s'", token);
      return FEENOX_ERROR;
    }
  }


  // si no nos dieron expresion algebraica, hay que leer e interpolar datos
  if (function->algebraic_expression.n_tokens == 0) {
    if (function->type == type_pointwise_file) {

      // si nos dieron un archivo, leemos de ahi
      FILE *data_file;
      char data_line[BUFFER_SIZE*BUFFER_SIZE];
      int n_columns;

      if ((data_file = feenox_fopen(function->data_file, "r")) == NULL) {
        feenox_push_error_message("\"%s\" opening file '%s'", strerror(errno), function->data_file);
        return FEENOX_ERROR;
      }

      // contamos cuantas lineas no vacias hay -> ese es el tamanio del size
      function->data_size = 0;
      while (feenox_read_data_line(data_file, data_line) != 0) {
        if (data_line[0] != '\0') {
          // en la primera vuelta miramos cuantas columnas hay
          if (function->data_size == 0) {
            n_columns = 0;
            token = strtok(data_line, " \t");
            while (token != NULL) {
              n_columns++;
              token = strtok(NULL, " \t");
            }

            if (n_columns < (nargs+1)) {
              feenox_push_error_message("at least %d columns expected but %d were given in  file '%s'", nargs+1, n_columns, function->data_file);
              return FEENOX_ERROR;
            }

          }
          function->data_size++;
        }
      }

      function->data_argument = calloc(nargs, sizeof(double *));
      function->data_argument_alloced = 1;
      for (i = 0; i < nargs; i++) {
        function->data_argument[i] = calloc(function->data_size, sizeof(double));
      }
      function->data_value = calloc(function->data_size, sizeof(double));

      rewind(data_file);

      // ahora leemos los datos
      j = 0;
      while (feenox_read_data_line(data_file, data_line) != 0) {

        if (data_line[0] != '\0') {
          for (i = 0; i < nargs; i++) {
            if ((token = feenox_get_nth_token(data_line, function->column[i])) == NULL) {
              feenox_push_error_message("wrong-formatted file '%s' at line %d", function->data_file, j+1);
              return FEENOX_ERROR;
            }
            sscanf(token, "%lf", &function->data_argument[i][j]);
            free(token);

            //  para poder meter steps o numeros repetidos
            if (nargs == 1 && j > 0 && gsl_fcmp(function->data_argument[i][j], function->data_argument[i][j-1], 1e-12) == 0) {
              if (j >= 2) {
                // si es un step tratamos de manejarlo
                function->data_argument[i][j] += 0.005*(function->data_argument[i][j-1]-function->data_argument[i][j-2]);
              } else {
                // si es el primer punto, lo tiramos
                function->data_size--;
                j--;
              }
            }

          }
          if ((token = feenox_get_nth_token(data_line, function->column[i])) == NULL) {
            feenox_push_error_message("not enough columns in file '%s' at line %d", function->data_file, j+1);
            return FEENOX_ERROR;
          }
          sscanf(token, "%lf", &function->data_value[j]);
          free(token);

          j++;
        }
      }

      fclose(data_file);


///kw+FUNCTION+usage [ DATA <num_1> <num_2> ... <num_N> ]
///kw+FUNCTION+detail The function can be pointwise-defined inline in the input using `DATA`. This should be the last keyword of the line, followed by $N=k\cdot (n+1)$ expresions giving $k$ definition points: $n$ arguments and the value of the function.
///kw+FUNCTION+detail Multiline continuation using brackets `{` and `}` can be used for a clean data organization. See the examples.
    } else if (token != NULL && strcasecmp(token, "DATA") == 0) {
      // leemos del mismo archivo de entrada
      dummy = token + strlen(token)+1;

      // nos dieron los datos hard en el feenox_input
      backup1 = malloc(strlen(dummy)+8);
      snprintf(backup1, strlen(dummy)+8, "dummy %s", dummy);
      backup2 = malloc(strlen(dummy)+8);
      snprintf(backup2, strlen(dummy)+8, "dummy %s", dummy);

      // contamos cuanta informacion hay
      function->data_size = 0;
      if ((token = feenox_get_next_token(backup1)) == NULL) {
        return FEENOX_ERROR;
      }
      while ((token = feenox_get_next_token(NULL)) != NULL) {
        function->data_size++;
      }
      if (function->data_size % (nargs+1) != 0) {
        feenox_push_error_message("data mismatch for function '%s'", function->name);
        return FEENOX_ERROR;
      }
      function->data_size /= (nargs+1);

      function->data_argument = calloc(nargs, sizeof(double *));
      function->data_argument_alloced = 1;
      for (i = 0; i < nargs; i++) {
        function->data_argument[i] = calloc(function->data_size, sizeof(double));
      }

      function->data_value = calloc(function->data_size, sizeof(double));

      // leemos la informacion
      if ((token = feenox_get_next_token(backup2)) == NULL) {
        return FEENOX_ERROR;
      }
      for (i = 0; i < function->data_size; i++) {
        // argumentos
        for (j = 0; j < nargs; j++) {
          if ((token = feenox_get_next_token(NULL)) == NULL) {
            return FEENOX_ERROR;
          }
          function->data_argument[j][i] = feenox_evaluate_expression_in_string(token);

          //  para poder meter steps
          if (nargs == 1 && i >= 2 && function->data_argument[j][i] == function->data_argument[j][i-1]) {
              function->data_argument[j][i] += 0.005*(function->data_argument[j][i-1]-function->data_argument[j][i-2]);
            }
          }

        // valor
        if ((token = feenox_get_next_token(NULL)) == NULL) {
          return FEENOX_ERROR;
        }
        function->data_value[i] = feenox_evaluate_expression_in_string(token);
      }

      free(backup2);
      free(backup1);
    }


    // creamos tres variables (constantes) extra: func_a, func_b y func_n
    if (nargs == 1) {
      var_t *dummy_var;
      char *dummy_aux = malloc(strlen(function->name) + 4);

      snprintf(dummy_aux, strlen(function->name) + 4, "%s_a", function->name);
      if ((dummy_var = feenox_define_variable(dummy_aux)) == NULL) {
        return FEENOX_ERROR;
      }
      if (function->data_size != 0) {
        feenox_realloc_variable_ptr(dummy_var, &function->data_argument[0][0], 0);
      }

      snprintf(dummy_aux, strlen(function->name) + 4, "%s_b", function->name);
      if ((dummy_var = feenox_define_variable(dummy_aux)) == NULL) {
        return FEENOX_ERROR;
      }
      if (function->data_size != 0) {
        feenox_realloc_variable_ptr(dummy_var, &function->data_argument[0][function->data_size-1], 0);
      }

      snprintf(dummy_aux, strlen(function->name) + 4, "%s_n", function->name);
      if ((dummy_var = feenox_define_variable(dummy_aux)) == NULL) {
        return FEENOX_ERROR;
      }
      if (function->data_size != 0) {
        feenox_value(dummy_var) = (double)function->data_size;
      }

      free(dummy_aux);
    }

  }
*/

  // clean up this (partial) mess
  if (arg_name != NULL) {
    for (i = 0; i < n_arguments; i++) {
      free(arg_name[i]);
    }
    free(arg_name);
  }
  free(name);
  
  return FEENOX_OK;
  
}

int feenox_parse_sort_vector(void) {
      
  char *token;
  sort_vector_t *sort_vector = calloc(1, sizeof(sort_vector_t));

///kw+SORT_VECTOR+usage <vector>
  feenox_call(feenox_parser_vector(&sort_vector->v1));

  while ((token = feenox_get_next_token(NULL)) != NULL) {
        
///kw+SORT_VECTOR+detail This instruction sorts the elements of `<vector>` into either ascending or descending numerical order.
///kw+SORT_VECTOR+detail If `<other_vector>` is given, the same rearrangement is made on it.
///kw+SORT_VECTOR+detail Default is ascending order.
    
///kw+SORT_VECTOR+usage [ ASCENDING | DESCENDING ]
    if (strcasecmp(token, "ASCENDING") == 0 || strcasecmp(token, "ASCENDING_ORDER") == 0) {
      sort_vector->descending = 0;
    } else if (strcasecmp(token, "DESCENDING") == 0 || strcasecmp(token, "DESCENDING_ORDER") == 0) {
      sort_vector->descending = 1;
        
///kw+SORT_VECTOR+usage [ <other_vector> ]
    } else if ((sort_vector->v2 = feenox_get_vector_ptr(token)) != NULL) {
      continue;
    } else {
      feenox_push_error_message("unknown keyword or vector identifier '%s'", token);
      return FEENOX_ERROR;
    }
  }
      
  feenox_call(feenox_add_instruction(feenox_instruction_sort_vector, sort_vector));
      
  return FEENOX_OK;
        
}


int feenox_parse_file(char *mode) {
 
  char *token = NULL;
  char *name = NULL;
  char *format = NULL;
  char *custom_mode = NULL;
  char **arg = NULL;
  int n_args = 0;

///kw+FILE+usage <name>
  if (feenox_parser_string(&name) != FEENOX_OK) {
    return FEENOX_ERROR;
  }

///kw+FILE+detail For reading or writing into files with a fixed path, this instruction is usually not needed as
///kw+FILE+detail the `FILE` keyword of other instructions (such as `PRINT` or `MESH`) can take a fixed-string path as an argument.
///kw+FILE+detail However, if the file name changes as the execution progresses (say because one file for each step is needed),
///kw+FILE+detail then an explicit `FILE` needs to be defined with this keyword and later referenced by the given name.  
  
  while ((token = feenox_get_next_token(NULL)) != NULL) {
///kw+FILE+usage PATH <format>
///kw+FILE+usage expr_1 expr_2 ... expr_n
///kw+FILE+detail The path should be given as a `printf`-like format string followed by the expressions which shuold be
///kw+FILE+detail evaluated in order to obtain the actual file path. The expressions will always be floating-point expressions,
///kw+FILE+detail but the particular integer specifier `%d` is allowed and internally transformed to `%.0f`.
    if (strcasecmp(token, "PATH") == 0) {
      feenox_call(feenox_parser_string_format(&format, &n_args));

      if (n_args != 0) {
        arg = calloc(n_args, sizeof(char *));
        int i;
        for (i = 0; i < n_args; i++) {
          feenox_call(feenox_parser_string(&arg[i]));
        }
      }

///kw+FILE+detail The file can be explicitly defined and `INPUT`, `OUTPUT` or a certain `fopen()` mode can be given (i.e. "a").
///kw+FILE+detail If not explicitly given, the nature of the file will be taken from context, i.e. `FILE`s in `PRINT`
///kw+FILE+detail will be `OUTPUT`  and `FILE`s in `FUNCTION` will be `INPUT`.
   
///kw+FILE+usage [ INPUT | OUTPUT | MODE <fopen_mode> ]
    } else if (strcasecmp(token, "MODE") == 0) {
      feenox_call(feenox_parser_string(&custom_mode));
    } else if (strcasecmp(token, "INPUT") == 0) {
      custom_mode = strdup("r");
    } else if (strcasecmp(token, "OUTPUT") == 0) {
      custom_mode = strdup("w");

///kw+FILE+detail This keyword justs defines the `FILE`, it does not open it.
///kw+FILE+detail The file will be actually openened (and eventually closed) automatically.
///kw+FILE+detail In the rare case where the automated opening and closing does not fit the expected workflow,
///kw+FILE+detail the file can be explicitly opened or closed with the instructions `FILE_OPEN` and `FILE_CLOSE`.
    } else {
      feenox_push_error_message("unknown keyword '%s'", token);
      return FEENOX_ERROR;
    }
  }

  // the custom mode takes precedende over the argument's mode
  if (custom_mode == NULL) {
    custom_mode = mode;
  }

  feenox_call(feenox_define_file(name, format, n_args, mode));
  
  int i;
  for (i = 0; i < n_args; i++) {
    feenox_call(feenox_file_set_path_argument(name, i, arg[i]));
  }

  free(format);
  free(custom_mode);
  free(name);
  
  return FEENOX_OK;
}


int feenox_parse_open_close(const char *what) {

  char *token = NULL;
  char *name = NULL;
  char *mode = NULL;
  
///kw+OPEN+usage <name>
///kw+CLOSE+usage <name>
  if (feenox_parser_string(&name) != FEENOX_OK) {
    return FEENOX_ERROR;
  }

///kw+OPEN+detail The given `<name>` can be either a fixed-string path or an already-defined `FILE`.
///kw+CLOSE+detail The given `<name>` can be either a fixed-string path or an already-defined `FILE`.
  
  if (strcmp(what, "OPEN") == 0) {
    while ((token = feenox_get_next_token(NULL)) != NULL) {
///kw+OPEN+detail The mode is only taken into account if the file is not already defined.
///kw+OPEN+detail Default is write `w`.
    
///kw+OPEN+usage [ MODE <fopen_mode> ]
      if (strcasecmp(token, "MODE") == 0) {
        feenox_call(feenox_parser_string(&mode));
      }  
    }  
  }
  
  
  file_t *file;
  if ((file = feenox_get_file_ptr(name)) == NULL) {
    feenox_call(feenox_define_file(name, name, 0, mode));
    if ((file = feenox_get_file_ptr(name)) == NULL) {
      return FEENOX_ERROR;
    }
  }

  // to add an instruction from the API one needs to pass a string and not a file_t   
  if (strcmp(what, "OPEN") == 0) {
    feenox_call(feenox_add_instruction(feenox_instruction_file_open, file));
  } else if (strcmp(what, "CLOSE") == 0) {
    feenox_call(feenox_add_instruction(feenox_instruction_file_close, file));
  }
  
  return FEENOX_OK;  
}

int feenox_parse_print(void) {
 
  char *token;
  print_t *print;
  matrix_t *dummy_matrix;
  vector_t *dummy_vector;
  int n;

  // I don't expect anybody to want to use this PRINT instruction through the API
  // so we just parse and define everything here
  print = calloc(1, sizeof(print_t));

  char *keywords[] = {"SKIP_STEP", "SKIP_STATIC_STEP", "SKIP_TIME", "SKIP_HEADER_STEP", ""};
  expr_t *expressions[] = {
    &print->skip_step,
    &print->skip_static_step,
    &print->skip_time,
    &print->skip_header_step,
    NULL
  };

///kw+PRINT+detail Each argument `object` which is not a keyword of the `PRINT` instruction will be part of the output.
///kw+PRINT+detail Objects can be either a matrix, a vector or any valid scalar algebraic expression.
///kw+PRINT+detail If the given object cannot be solved into a valid matrix, vector or expression, it is treated as a string literal
///kw+PRINT+detail if `IMPLICIT` is `ALLOWED`, otherwise a parser error is raised.
///kw+PRINT+detail To explicitly interpret an object as a literal string even if it resolves to a valid numerical expression,
///kw+PRINT+detail it should be prefixed with the `TEXT` keyword such as `PRINT TEXT 1+1` that would print `1+1` instead of `2`.
///kw+PRINT+detail Objects and string literals can be mixed and given in any order.
///kw+PRINT+detail Hashes `#` appearing literal in text strings have to be quoted to prevent the parser to treat them as comments
///kw+PRINT+detail within the FeenoX input file and thus ignoring the rest of the line, like `PRINT "\# this is a printed comment"`.
///kw+PRINT+detail Whenever an argument starts with a porcentage sign `%`, it is treated as a C `printf`-compatible format
///kw+PRINT+detail specifier and all the objects that follow it are printed using the given format until a new format definition is found.
///kw+PRINT+detail The objects are treated as double-precision floating point numbers, so only floating point formats should be given.
///kw+PRINT+detail See the `printf(3)` man page for further details. The default format is `DEFAULT_PRINT_FORMAT`.
///kw+PRINT+detail Matrices, vectors, scalar expressions, format modifiers and string literals can be given in any desired order,
///kw+PRINT+detail and are processed from left to right.
///kw+PRINT+detail Vectors are printed element-by-element in a single row. See `PRINT_VECTOR` to print vectors column-wise.
///kw+PRINT+detail Matrices are printed element-by-element in a single line using row-major ordering if mixed
///kw+PRINT+detail with other objects but in the natural row and column fashion if it is the only given object in the `PRINT` instruction.

  while ((token = feenox_get_next_token(NULL)) != NULL) {
///kw+PRINT+usage [ FILE < <file_path> | <file_id> > ]
///kw+PRINT+detail If the `FILE` keyword is not provided, default is to write to `stdout`.
    if (strcasecmp(token, "FILE") == 0 || strcasecmp(token, "FILE_PATH") == 0) {
      feenox_call(feenox_parser_file(&print->file));
      // we don't have the file name becuase it was already consumed by the above call
      // and we do not expect the PRINT instruction to be used from the API
      // so we directly access the internals of the structure
      if (print->file->mode == NULL) {
        print->file->mode = strdup("w");
      }
 
///kw+PRINT+usage [ SEP <string> ]
///kw+PRINT+detail The `SEP` keyword expects a string used to separate printed objects.
///kw+PRINT+detail To print objects without any separation in between give an empty string like `SEP ""`.
///kw+PRINT+detail The default is a tabulator character 'DEFAULT_PRINT_SEPARATOR' character. 
    } else if (strcasecmp(token, "SEP") == 0 || strcasecmp(token, "SEPARATOR") == 0) {
      feenox_call(feenox_parser_string(&print->separator));

///kw+PRINT+usage [ NONEWLINE ]
    } else if (strcasecmp(token, "NONEWLINE") == 0) {
///kw+PRINT+detail If the `NONEWLINE` keyword is not provided, default is to write a newline `\n` character after
///kw+PRINT+detail all the objects are processed.
///kw+PRINT+detail Otherwise, if the last token to be printed is a numerical value, a separator string will be printed but not the newline `\n` character.
///kw+PRINT+detail If the last token is a string, neither the separator nor the newline will be printed.
      print->nonewline = 1;

///kw+PRINT+detail To print an empty line write `PRINT` without arguments.
      
///kw+PRINT+usage [ HEADER ]
    } else if (strcasecmp(token, "HEADER") == 0) {
      print->header = 1;
///kw+PRINT+detail If the `HEADER` keyword is given, a single line containing the literal text
///kw+PRINT+detail given for each object is printed at the very first time the `PRINT` instruction is
///kw+PRINT+detail processed, starting with a hash `#` character.           

///kw+PRINT+usage [ SKIP_STEP <expr> ]
///kw+PRINT+detail By default the `PRINT` instruction is evaluated every step.
///kw+PRINT+detail If the `SKIP_STEP` (`SKIP_STATIC_STEP`) keyword is given, the instruction is processed 
///kw+PRINT+detail only every the number of transient (static) steps that results in evaluating the expression,
///kw+PRINT+detail which may not be constant.
///kw+PRINT+detail The `SKIP_HEADER_STEP` keyword works similarly for the optional `HEADER` but
///kw+PRINT+detail by default it is only printed once. The `SKIP_TIME` keyword use time advancements
///kw+PRINT+detail to choose how to skip printing and may be useful for non-constant time-step problems.

///kw+PRINT+usage [ SKIP_STATIC_STEP <expr> ]
///kw+PRINT+usage [ SKIP_TIME <expr> ]
///kw+PRINT+usage [ SKIP_HEADER_STEP <expr> ]
    } else if ((n = feenox_parser_match_keyword_expression(token, keywords, expressions, sizeof(expressions)/sizeof(expr_t *))) != FEENOX_UNHANDLED) {
      if (n == FEENOX_ERROR) {
        return FEENOX_ERROR;
      }

    } else {
      
///kw+PRINT+usage [ <object_1> <object_2> ... <object_n> ]
///kw+PRINT+usage [ TEXT <string_1> ... TEXT <string_n> ]
      print_token_t *print_token = calloc(1, sizeof(print_token_t));
      LL_APPEND(print->tokens, print_token);

      if (token[0] == '%') {
        print_token->format = strdup(token);

      } else if (strcasecmp(token, "STRING") == 0 || strcasecmp(token, "TEXT") == 0) {
        if (feenox_parser_string(&print_token->text) != FEENOX_OK) {
          return FEENOX_ERROR;
        }

      } else if ((dummy_matrix = feenox_get_matrix_ptr(token)) != NULL) {
        print_token->text = strdup(token);   // nos quedamos con el texto para el header
        print_token->matrix = dummy_matrix;

      } else if ((dummy_vector = feenox_get_vector_ptr(token)) != NULL) {
        print_token->text = strdup(token);   // nos quedamos con el texto para el header
        print_token->vector = dummy_vector;

      } else {
        if (feenox_expression_parse(&print_token->expression, token) != FEENOX_OK) {

          // let the not-resolved expressions to be casted as string literals 
          // only if implicit is allowed
          if (feenox_parser.implicit_none) {
            feenox_push_error_message("implicit definition is not allowed and expression '%s' is invalid", token);
            return FEENOX_ERROR;
          } else {
            print_token->expression.factors = NULL;
            print_token->text = strdup(token);
            feenox_pop_error_message();
          }
        } else {
          print_token->text = strdup(token);   // nos quedamos con el texto para el header
        }
      }
    }
  }

  // default separator
  if (print->separator == NULL) {
    print->separator = strdup(DEFAULT_PRINT_SEPARATOR);
  }
  
  // default file is stdout
  if (print->file == NULL) {
    print->file = feenox.special_files.stdout_;
  }
  
  LL_APPEND(feenox.prints, print);
  feenox_call(feenox_add_instruction(feenox_instruction_print, print));
  
  return FEENOX_OK;
}

int feenox_parse_if(void) {
  
  conditional_block_t *conditional_block = calloc(1, sizeof(conditional_block_t));
  feenox_call(feenox_parser_expression(&conditional_block->condition));

  if (feenox_get_next_token(NULL) != NULL) {
    feenox_push_error_message("conditional blocks should start in a separate line");
    return FEENOX_ERROR;
  }

  conditional_block->father = feenox.active_conditional_block;

  feenox_call(feenox_add_instruction(feenox_instruction_if, conditional_block));

  LL_APPEND(feenox.conditional_blocks, conditional_block);
  feenox.active_conditional_block = conditional_block;

  return FEENOX_OK;
}

int feenox_parse_else(void) {
  
  conditional_block_t *conditional_block = calloc(1, sizeof(conditional_block_t));

  if (feenox.active_conditional_block->else_of != NULL) {
    feenox_push_error_message("more than one ELSE clause for a single IF clause");
    return FEENOX_ERROR;
  }
  
  if (feenox_get_next_token(NULL) != NULL) {
    feenox_push_error_message("conditional blocks should start in a separate line");
    return FEENOX_ERROR;
  }

  instruction_t *instruction = feenox_add_instruction_and_get_ptr(feenox_instruction_else, conditional_block);
  if (instruction == NULL) {
    return FEENOX_ERROR;
  }
  feenox.active_conditional_block->first_false_instruction = instruction;
  conditional_block->else_of = feenox.active_conditional_block;
  conditional_block->father = feenox.active_conditional_block->father;
      
  LL_APPEND(feenox.conditional_blocks, conditional_block);
  feenox.active_conditional_block = conditional_block;

  return FEENOX_OK;
}

int feenox_parse_endif(void) {
  
  instruction_t *instruction = feenox_add_instruction_and_get_ptr(feenox_instruction_endif, feenox.active_conditional_block);
  if (instruction == NULL) {
    return FEENOX_ERROR;
  }

  if (feenox.active_conditional_block->else_of == NULL) {
    feenox.active_conditional_block->first_false_instruction = instruction;
  } else {
    feenox.active_conditional_block->first_true_instruction = instruction;
  }

  feenox.active_conditional_block = feenox.active_conditional_block->father;

  return EXIT_SUCCESS;
}
