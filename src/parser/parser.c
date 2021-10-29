/*------------ -------------- -------- --- ----- ---   --       -            -
 *  FeenoX parser
 *
 *  Copyright (C) 2009--2021 jeremy theler
 *
 *  This file is part of FeenoX.
 *
 *  FeenoX is free software: you can redistribute it and/or modify
 *  it under the terms "of the GNU General Public License as published by
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


int feenox_parse_main_input_file(const char *filepath) {
  
  // check the page size to use a reasonable buffer size
  // we can ask for more if we need
#if HAVE_SYSCONF
  feenox_parser.page_size = (size_t)sysconf(_SC_PAGESIZE);
#else  
  feenox_parser.page_size = 4096;  
#endif
  feenox_parser.actual_buffer_size = feenox_parser.page_size-64;
  
  feenox_check_alloc(feenox_parser.line = malloc(feenox_parser.actual_buffer_size));
  feenox_call(feenox_parse_input_file(filepath, 0, 0));
  feenox_free(feenox_parser.line);

  if (feenox_parser.active_conditional_block != NULL) {
    feenox_push_error_message("conditional block not closed");
    return FEENOX_ERROR;
  }

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
      
      // the line is broken by strtok inside parse_line() and we need
      // the original for assignements and equations
      feenox_check_alloc(feenox_parser.full_line = strdup(feenox_parser.line));
      if (feenox_parse_line() != FEENOX_OK) {
        feenox_free(feenox_parser.full_line);
        feenox_push_error_message("%s: %d:", filepath, line_num);
        return FEENOX_ERROR;
      }
      feenox_free(feenox_parser.full_line);
    }
  }

  if (strcmp(filepath, "-") != 0) {
    fclose(input_file_stream);
  }
  
  return FEENOX_OK;

}




// feenox line parser
int feenox_parse_line(void) {

  char *equal_sign = NULL;
  char *token = NULL;
  
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
      
///kw_dae+TIME_PATH+usage TIME_PATH
///kw_dae+TIME_PATH+desc Force time-dependent problems to pass through specific instants of time.
    } else if (strcasecmp(token, "TIME_PATH") == 0) {
      feenox_call(feenox_parse_time_path());
      return FEENOX_OK;
      
///kw_dae+INITIAL_CONDITIONS+usage INITIAL_CONDITIONS
///kw_dae+INITIAL_CONDITIONS+desc Define how initial conditions of DAE problems are computed.
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
///kw+FUNCTION+desc Define a scalar function of one or more variables.
    } else if (strcasecmp(token, "FUNCTION") == 0) {
      feenox_call(feenox_parse_function());
      return FEENOX_OK;
    
///kw+PRINT+desc Write plain-text and/or formatted data to the standard output or into an output file.
///kw+PRINT+usage PRINT
    } else if (strcasecmp(token, "PRINT") == 0) {
      feenox_call(feenox_parse_print());
      return FEENOX_OK;

///kw+PRINT_FUNCTION+desc Print one or more functions as a table of values of dependent and independent variables.
///kw+PRINT_FUNCTION+usage PRINT_FUNCTION
    } else if (strcasecmp(token, "PRINT_FUNCTION") == 0) {
      feenox_call(feenox_parse_print_function());
      return FEENOX_OK;
      
///kw+PRINT_VECTOR+desc Print the elements of one or more vectors, one element per line.
///kw+PRINT_VECTOR+usage PRINT_VECTOR
    } else if (strcasecmp(token, "PRINT_VECTOR") == 0) {
      feenox_call(feenox_parse_print_vector());
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

///kw_dae+PHASE_SPACE+desc Asks FeenoX to solve a set of algebraic-differntial equations and define the variables, vectors and/or matrices that span the phase space.
///kw_dae+PHASE_SPACE+usage PHASE_SPACE
      // -----  -----------------------------------------------------------
    } else if (strcasecmp(token, "PHASE_SPACE") == 0) {
      feenox_call(feenox_parse_phase_space());
      return FEENOX_OK;

///kw_pde+READ_MESH+desc Read an unstructured mesh and (optionally) functions of space-time from a file.
///kw_pde+READ_MESH+usage READ_MESH
      // -----  -----------------------------------------------------------
    } else if (strcasecmp(token, "READ_MESH") == 0) {
      feenox_call(feenox_parse_read_mesh());
      return FEENOX_OK;

///kw_pde+WRITE_MESH+desc Write a mesh and functions of space-time to a file for post-processing.
///kw_pde+WRITE_MESH+usage WRITE_MESH
      // -----  -----------------------------------------------------------
    } else if (strcasecmp(token, "WRITE_MESH") == 0) {
      feenox_call(feenox_parse_write_mesh());
      return FEENOX_OK;

///kw_pde+PROBLEM+desc Ask FeenoX to solve a partial-differential equation problem.
///kw_pde+PROBLEM+usage PROBLEM
    } else if (strcasecmp(token, "PROBLEM") == 0) {
#ifdef HAVE_PETSC      
      feenox_call(feenox_parse_problem());
      return FEENOX_OK;
#else
      feenox_push_error_message("FeenoX is not compiled with PETSc so it cannot solve PROBLEMs");
      return FEENOX_ERROR;
#endif      
      
///kw_pde+PHYSICAL_GROUP+desc Explicitly defines a physical group of elements on a mesh.
///kw_pde+PHYSICAL_GROUP+usage PHYSICAL_GROUP
      // -----  -----------------------------------------------------------
    } else if (strcasecmp(token, "PHYSICAL_GROUP") == 0) {
      feenox_call(feenox_parse_physical_group());
      return FEENOX_OK;

///kw_pde+MATERIAL+desc Define a material its and properties to be used in volumes.
///kw_pde+MATERIAL+usage MATERIAL
      // -----  -----------------------------------------------------------
    } else if (strcasecmp(token, "MATERIAL") == 0) {
      feenox_call(feenox_parse_material());
      return FEENOX_OK;

///kw_pde+BC+desc Define a boundary condition to be applied to faces, edges and/or vertices.
///kw_pde+BC+usage BC
      // -----  -----------------------------------------------------------
    } else if (strcasecmp(token, "BC") == 0 || strcasecmp(token, "BOUNDARY_CONDITION") == 0) {
      feenox_call(feenox_parse_bc());
      return FEENOX_OK;
      
///kw_pde+SOLVE_PROBLEM+desc Explicitly solve the PDE problem.
///kw_pde+SOLVE_PROBLEM+usage SOLVE_PROBLEM
    } else if (strcasecmp(token, "SOLVE_PROBLEM") == 0) {
#ifdef HAVE_PETSC      
      feenox_call(feenox_parse_solve_problem());
      return FEENOX_OK;
#else
      feenox_push_error_message("FeenoX is not compiled with PETSc so it cannot solve PROBLEMs");
      return FEENOX_ERROR;
#endif      

///kw_pde+INTEGRATE+desc Spatially integrate a function or expression over a mesh (or a subset of it).
///kw_pde+INTEGRATE+usage INTEGRATE
      // -----  -----------------------------------------------------------
    } else if (strcasecmp(token, "INTEGRATE") == 0) {
      feenox_call(feenox_parse_integrate());
      return FEENOX_OK;

///kw_pde+FIND_EXTREMA+desc Find and/or compute the absolute extrema of a function or expression over a mesh (or a subset of it).
///kw_pde+FIND_EXTREMA+usage FIND_EXTREMA
      // -----  -----------------------------------------------------------
    } else if (strcasecmp(token, "FIND_EXTREMA") == 0) {
      feenox_call(feenox_parse_find_extrema());
      return FEENOX_OK;

///kw+FIT+desc Find parameters to fit an analytical function to a pointwise-defined function.
///kw+FIT+usage FIT
      // -----  -----------------------------------------------------------
    } else if (strcasecmp(token, "FIT") == 0) {
      feenox_call(feenox_parse_fit());
      return FEENOX_OK;
      
///kw_pde+DUMP+desc Dump raw PETSc objects used to solve PDEs into files.
///kw_pde+DUMP+usage DUMP
      // -----  -----------------------------------------------------------
    } else if (strcasecmp(token, "DUMP") == 0) {
      feenox_call(feenox_parse_dump());
      return FEENOX_OK;
      
// this should come last because there is no actual keyword apart from the equal sign
// so if we came down here, then that means that any line containing a '=' that has
// not been already processed must be one of these
//  i.   an algebraic function
//  ii.  an equation for the DAE
//  iii. an assignment
     
// TODO: explain syntax for reference      
    } else if ((equal_sign = strstr(feenox_parser.full_line, ":=")) != NULL ||
               (equal_sign = strstr(feenox_parser.full_line, ".=")) != NULL ||
               (equal_sign = strchr(feenox_parser.full_line, '=')) != NULL) {
        
        enum { parser_assignment, parser_dae, parser_function } type;
        
        // first see if there is an explicit sign
        switch (*equal_sign) {
          case '=':
            type = parser_assignment;
          break;
          case ':':
            type = parser_function;
          break;
          case '.':
            type = parser_dae;
          break;
          default:
            feenox_push_error_message("unexpected character '%c'", *equal_sign);
            return FEENOX_ERROR;
          break;  
        }     

        *equal_sign = '\0';
        char *lhs = feenox_parser.full_line;
        char *rhs = equal_sign + 1 + (equal_sign[1] == '=');
        
        // try to figure out if it is a DAE or a function automatically
        if (type == parser_assignment) {

          char *lhs_tmp = NULL;
          feenox_check_alloc(lhs_tmp = strdup(lhs));
          char *lhs_object = NULL;
          lhs_object = strtok(lhs_tmp, factorseparators);
          
          // check if it is a function
          int length_full = strlen(lhs);
          int length_object = strlen(lhs_object);
          if (length_full > length_object && lhs[length_object] == '(') {
            type = parser_function;
          } else {
            
            // check if the object belongs to the phase space
            phase_object_t *phase_object = NULL;
            LL_FOREACH(feenox.dae.phase_objects, phase_object) {
              
              if ((phase_object->variable != NULL      && strcmp(lhs_object, phase_object->variable->name) == 0) ||
                  (phase_object->variable_dot != NULL  && strcmp(lhs_object, phase_object->variable_dot->name) == 0) ||
                  (phase_object->vector != NULL        && strcmp(lhs_object, phase_object->vector->name) == 0) ||
                  (phase_object->vector_dot != NULL    && strcmp(lhs_object, phase_object->vector_dot->name) == 0) ||
                  (phase_object->matrix != NULL        && strcmp(lhs_object, phase_object->matrix->name) == 0) ||
                  (phase_object->matrix_dot != NULL    && strcmp(lhs_object, phase_object->matrix_dot->name) == 0)) {
                type = parser_dae;
              }
              
            }
          }
          feenox_free(lhs_tmp);
        }
        
        
        switch (type) {
          case parser_assignment:
            feenox_call(feenox_add_assignment(lhs, rhs));
          break;
          case parser_function:
          {
            char *name;
            feenox_call(feenox_add_function_from_string(lhs, &name));
            feenox_call(feenox_function_set_expression(name, rhs));
            feenox_free(name);
          }  
          break;  
          case parser_dae:
            // TODO: handle vector/matrix DAEs
            feenox_call(feenox_add_dae(lhs, rhs));
          break;
        }

        return FEENOX_OK;
    }
      
  }


      
/*

// ---- SEMAPHORE ----------------------------------------------------
//kw+SEMAPHORE+desc Perform either a wait or a post operation on a named shared semaphore.
//kw+SEMAPHORE+usage [ SEMAPHORE | SEM ]
    } else if ((strcasecmp(token, "SEMAPHORE") == 0) || (strcasecmp(token, "SEM") == 0)) {

      struct semaphore_t *semaphore;
      semaphore = calloc(1, sizeof(struct semaphore_t));
      LL_APPEND(feenox.semaphores, semaphore);

//kw+SEMAPHORE+usage <name>
      if (feenox_parser_string(&semaphore->name) != FEENOX_OK) {
        return FEENOX_ERROR;
      }

//kw+SEMAPHORE+usage { WAIT | POST }
      char *keywords[] = {"WAIT", "POST", ""};
      int values[] = {feenox_sem_wait, feenox_sem_post, 0};
      feenox_call(feenox_parser_keywords_ints(keywords, values, (int *)&semaphore->operation));

      if (feenox_define_instruction(feenox_instruction_sem, semaphore) == NULL) {
        return FEENOX_ERROR;
      }

      return FEENOX_OK;

// ---- READ / WRITE  ----------------------------------------------------
//kw+READ+desc Read data (variables, vectors o matrices) from files or shared-memory segments.
//kw+WRITE+desc Write data (variables, vectors o matrices) to files or shared-memory segments.
//kw+WRITE+desc See the `READ` keyword for usage details.
//kw+READ+usage [ READ | WRITE ]
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
//kw+READ+usage [ SHM <name> ]
        if (strcasecmp(token, "SHM") == 0 || strcasecmp(token, "SHM_OBJECT") == 0) {

          io->type = io_shm;

          if (feenox_parser_string(&io->shm_name) != FEENOX_OK) {
            return FEENOX_ERROR;
          }

        // ---- FILE_PATH ----------------------------------------------------
//kw+READ+usage [ { ASCII_FILE_PATH | BINARY_FILE_PATH } <file_path> ]
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
//kw+READ+usage [ { ASCII_FILE | BINARY_FILE } <identifier> ]
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
//kw+READ+usage [ IGNORE_NULL ]
          } else if (strcasecmp(token, "IGNORE_NULL") == 0) {

            io->ignorenull = 1;

          } else  {
//kw+READ+usage [ object_1 object_2 ... object_n ]

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

      
// --- SOLVE -----------------------------------------------------
//kw+SOLVE+desc Solve a non-linear system of\ $n$ equations with\ $n$ unknowns.
//TODO: example
//kw+SOLVE+example solve1.was solve2.was
//kw+SOLVE+usage SOLVE
      
    } else if (strcasecmp(token, "SOLVE") == 0) {

      double xi;
      int i;
      solve_t *solve;
      solve = calloc(1, sizeof(solve_t));
      LL_APPEND(feenox.solves, solve);

//kw+SOLVE+usage <n> 
      feenox_call(feenox_parser_expression_in_string(&xi));
      if ((solve->n = (int)(xi)) <= 0) {
        feenox_push_error_message("expected a positive number of unknowns instead of %d", solve->n);
        return FEENOX_ERROR;
      }

      solve->unknown = calloc(solve->n, sizeof(var_t *));
      solve->residual = calloc(solve->n, sizeof(expr_t));
      solve->guess = calloc(solve->n, sizeof(expr_t));

      while ((token = feenox_get_next_token(NULL)) != NULL) {
//kw+SOLVE+usage UNKNOWNS <var_1> <var_2> ... <var_n>
        if (strcasecmp(token, "UNKNOWNS") == 0) {
          for (i = 0; i < solve->n; i++) {
            if ((token = feenox_get_next_token(NULL)) == NULL) {
              feenox_push_error_message("expected %d variables and found only %d", solve->n, i);
            }
            if ((solve->unknown[i] = feenox_get_or_define_variable_ptr(token)) == NULL) {
              return FEENOX_ERROR;
            }
          }
          
//kw+SOLVE+usage RESIDUALS <expr_1> <expr_2> ... <expr_n> ]
        } else if (strcasecmp(token, "RESIDUALS") == 0) {
          for (i = 0; i < solve->n; i++) {
            if ((token = feenox_get_next_token(NULL)) == NULL) {
              feenox_push_error_message("expected %d expressions and found only %d", solve->n, i);
            }
            feenox_call(feenox_parse_expression(token, &solve->residual[i]));
          }
      
//kw+SOLVE+usage GUESS <expr_1> <expr_2> ... <expr_n> ]
        } else if (strcasecmp(token, "GUESS") == 0) {
          for (i = 0; i < solve->n; i++) {
            if ((token = feenox_get_next_token(NULL)) == NULL) {
              feenox_push_error_message("expected %d expressions and found only %d", solve->n, i);
            }
            feenox_call(feenox_parse_expression(token, &solve->guess[i]));
          }
      
//kw+SOLVE+usage [ METHOD
        } else if (strcasecmp(token, "METHOD") == 0) {

          if ((token = feenox_get_next_token(NULL)) == NULL) {
            feenox_push_error_message("expected method name");
            return FEENOX_ERROR;
          }

//kw+SOLVE+usage {
//kw+SOLVE+usage dnewton |
          if (strcasecmp(token, "dnewton") == 0) {
            solve->type = gsl_multiroot_fsolver_dnewton;
//kw+SOLVE+usage hybrid |
          } else if (strcasecmp(token, "hybrid") == 0) {
            solve->type = gsl_multiroot_fsolver_hybrid;
//kw+SOLVE+usage hybrids |
          } else if (strcasecmp(token, "hybrids") == 0) {
            solve->type = gsl_multiroot_fsolver_hybrids;
//kw+SOLVE+usage broyden }
          } else if (strcasecmp(token, "broyden") == 0) {
            solve->type = gsl_multiroot_fsolver_hybrid;
          }
//kw+SOLVE+usage ]
          
//kw+SOLVE+usage [ EPSABS <expr> ]
        } else if (strcasecmp(token, "EPSABS") == 0) {

          feenox_call(feenox_parser_expression(&solve->epsabs));

//kw+SOLVE+usage [ EPSREL <expr> ]
        } else if (strcasecmp(token, "EPSREL") == 0) {

          feenox_call(feenox_parser_expression(&solve->epsrel));

//kw+SOLVE+usage [ MAX_ITER <expr> ]
        } else if (strcasecmp(token, "MAX_ITER") == 0) {

          feenox_call(feenox_parser_expression_in_string(&xi));
          if ((solve->max_iter = (int)xi) < 0) {
            feenox_push_error_message("expected a positive integer for MAX_ITER");
            return FEENOX_ERROR;
          }

//kw+SOLVE+usage [ VERBOSE ]
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



// ----- HISTORY  -----------------------------------------------------------------
//kw+HISTORY+desc Record the time history of a variable as a function of time.
//kw+HISTORY+usage HISTORY
    } else if ((strcasecmp(token, "HISTORY") == 0)) {

      history_t *history;
      history = calloc(1, sizeof(history_t));
      LL_APPEND(feenox.histories, history);

//kw+HISTORY+usage <variable>
      // el nombre de la variable
      if (feenox_parser_variable(&history->variable)) {
        return FEENOX_ERROR;
      }

//kw+HISTORY+usage <function>
      // el nombre de la funcion
      if ((token = feenox_get_next_token(NULL)) == NULL) {
        feenox_push_error_message("expected function name");
        return FEENOX_ERROR;
      }

      if ((history->function = feenox_define_function(token, 1)) == NULL) {
        return FEENOX_ERROR;
      }
      // proposed by rvignolo
//      feenox_check_alloc(history->function->arg_name = malloc(1 * sizeof(char *)));
//      feenox_check_alloc(history->function->arg_name[0] = strdup(feenox.special_vars.t->name));

      if (feenox_define_instruction(feenox_instruction_history, history) == NULL) {
        return FEENOX_ERROR;
      }

      return FEENOX_OK;
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

  feenox_check_alloc_null(backup = strdup(string));

  if ((token = strtok(backup, UNQUOTED_DELIM)) == NULL) {
    feenox_free(backup);
    return NULL;
  }
  i = 1;

  while (i < n) {
    if ((token = strtok(NULL, UNQUOTED_DELIM)) == NULL) {
      feenox_free(backup);
      return NULL;
    }
    i++;
  }

  feenox_check_alloc_null(desired_token = strdup(token));
  feenox_free(backup);

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

  char *token = NULL;
  char *filepath = NULL;
  double xi = 0;
  unsigned int from = 0;
  unsigned int to = 0;
    
///kw+INCLUDE+usage <file_path>
  feenox_call(feenox_parser_string(&filepath));

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
  feenox_free(filepath);

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

  if ((n = (int)feenox_expression_evaluate_in_string(token)) <= 0) {
    feenox_push_error_message("expected a positive value instead of 'token", token);
    return FEENOX_ERROR;
  }

///kw+DEFAULT_ARGUMENT_VALUE+usage <string>
  if ((token = feenox_get_next_token(NULL)) == NULL) {
    feenox_push_error_message("expected value for DEFAULT_ARUGMENT_VALUE number %d", n);
    return FEENOX_ERROR;
  }

  if ((feenox.optind+n) >= feenox.argc) {
    feenox.argc = feenox.optind+n + 1;
    feenox_check_alloc(feenox.argv = realloc(feenox.argv, sizeof(char *)*(feenox.argc + 1)));
    feenox_check_alloc(feenox.argv[feenox.optind+n] = strdup(token));
    
    feenox_check_alloc(feenox.argv_orig = realloc(feenox.argv_orig, sizeof(char *)*(feenox.argc + 1)));
    feenox_check_alloc(feenox.argv_orig[feenox.optind+n] = strdup(token));
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
//TODO: example
//kw+IMPLICIT+example implicit.was

///kw+IMPLICIT+usage { NONE | ALLOWED }
  char *keywords[] = {"NONE", "ALLOWED", ""};
  int values[] = {1, 0, 0};
  feenox_call(feenox_parser_keywords_ints(keywords, values, &feenox_parser.implicit_none));

  return FEENOX_OK;
}  


int feenox_parse_time_path(void) {
  
///kw_dae+TIME_PATH+detail The time step `dt` will be reduced whenever the distance between
///kw_dae+TIME_PATH+detail the current time `t` and the next expression in the list is greater
///kw_dae+TIME_PATH+detail than `dt` so as to force `t` to coincide with the expressions given.
///kw_dae+TIME_PATH+detail The list of expresssions should evaluate to a sorted list of values for all times.          

  char *token;
  
///kw_dae+TIME_PATH+usage <expr_1> [ <expr_2>  [ ... <expr_n> ] ]
  while ((token = feenox_get_next_token(NULL)) != NULL) {
    feenox_call(feenox_add_time_path(token));
  }

  // y apuntamos el current al primero
  feenox.time_path_current = feenox.time_paths;

  return FEENOX_OK;
}

int feenox_parse_initial_conditions(void) {
 
///kw_dae+INITIAL_CONDITIONS+detail In DAE problems, initial conditions may be either:
///kw_dae+INITIAL_CONDITIONS+detail @
///kw_dae+INITIAL_CONDITIONS+detail  * equal to the provided expressions (`AS_PROVIDED`)@
///kw_dae+INITIAL_CONDITIONS+detail  * the derivatives computed from the provided phase-space variables (`FROM_VARIABLES`)@
///kw_dae+INITIAL_CONDITIONS+detail  * the phase-space variables computed from the provided derivatives (`FROM_DERIVATIVES`)@
///kw_dae+INITIAL_CONDITIONS+detail @
///kw_dae+INITIAL_CONDITIONS+detail In the first case, it is up to the user to fulfill the DAE system at\ $t = 0$.
///kw_dae+INITIAL_CONDITIONS+detail If the residuals are not small enough, a convergence error will occur.
///kw_dae+INITIAL_CONDITIONS+detail The `FROM_VARIABLES` option means calling IDA’s `IDACalcIC` routine with the parameter `IDA_YA_YDP_INIT`. 
///kw_dae+INITIAL_CONDITIONS+detail The `FROM_DERIVATIVES` option means calling IDA’s `IDACalcIC` routine with the parameter IDA_Y_INIT.
///kw_dae+INITIAL_CONDITIONS+detail Wasora should be able to automatically detect which variables in phase-space are differential and
///kw_dae+INITIAL_CONDITIONS+detail which are purely algebraic. However, the [`DIFFERENTIAL`] keyword may be used to explicitly define them.
///kw_dae+INITIAL_CONDITIONS+detail See the (SUNDIALS documentation)[https:/\/computation.llnl.gov/casc/sundials/documentation/ida_guide.pdf] for further information.
  
///kw_dae+INITIAL_CONDITIONS+usage { AS_PROVIDED | FROM_VARIABLES | FROM_DERIVATIVES }
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
    if ((dummy_closepar = strrchr(existing_object, ')')) == NULL) {
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
  
  feenox_free(left);
  feenox_free(keyword);
  feenox_free(right);
      
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
  
  // if there are brackets or parenthesis, the size is between them
  if ((dummy_openpar = strchr(name, '[')) != NULL || (dummy_openpar = strchr(name, '(')) != NULL) {
    if ((dummy_closepar = strrchr(name, ']')) == NULL && (dummy_closepar = strrchr(name, ')')) == NULL) {
      feenox_push_error_message("expecting closing parenthesis in expression '%s'", name);
      return FEENOX_ERROR;
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

/*  
  if (function_data != NULL) {
    feenox_call(feenox_vector_attach_function(name, function_data));
  }
*/
  if (datas != NULL) {
    feenox_call(feenox_vector_attach_data(name, datas));
  }

  feenox_free(name);
  if (dummy_openpar == NULL) {
    feenox_free(size);
  }
  feenox_free(function_data);
    
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
    if ((dummy_closepar = strrchr(name, ')')) == NULL) {
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

  feenox_free(name);
  if (dummy_openpar == NULL) {
    feenox_free(rows);
    feenox_free(cols);
  }
    
  return FEENOX_OK;
      
}


int feenox_parse_function(void) {

///kw+FUNCTION+usage <function_name>(<var_1>[,var2,...,var_n]) {
///kw+FUNCTION+usage @  
///kw+FUNCTION+detail The number of variables $n$ is given by the number of arguments given between parenthesis after the function name.
///kw+FUNCTION+detail The arguments are defined as new variables if they had not been already defined explictly as scalar variables.
  char *token = NULL;
  feenox_call(feenox_parser_string(&token));
  
  char *dummy_openpar = NULL;
  if ((dummy_openpar = strchr(token, '(')) == NULL) {
    feenox_push_error_message("expected opening parenthesis '(' after function name '%s' (no spaces allowed)", token);
    return FEENOX_ERROR;
  }

  char *name = NULL;
  feenox_call(feenox_add_function_from_string(token, &name));
  function_t *function = NULL;
  if ((function = feenox_get_function_ptr(name)) == NULL) {
    feenox_push_error_message("unkown function '%s'", name);
    return FEENOX_ERROR;
  }      
  
  unsigned int *columns = NULL;
  file_t *file = NULL;
  while ((token = feenox_get_next_token(NULL)) != NULL) {
///kw+FUNCTION+usage = <expr> |
///kw+FUNCTION+usage @  
///kw+FUNCTION+detail If the function is given as an algebraic expression, the short-hand operator `=`
///kw+FUNCTION+detail (or `:=` for compatiblity with Maxima) can be used.
///kw+FUNCTION+detail That is to say, `FUNCTION f(x) = x^2` is equivalent to `f(x) = x^2` (or `f(x) := x^2`).

    if (strcasecmp(token, "=") == 0 || strcasecmp(token, ":=") == 0) {
      feenox_call(feenox_function_set_expression(name, token + ((token[0] == ':')? 3 : 2)));
      break;    // we are done with the while() over the line

///kw+FUNCTION+usage FILE { <file> } |
///kw+FUNCTION+usage @  
    } else if (strcasecmp(token, "FILE") == 0 || strcasecmp(token, "FILE_PATH") == 0) {
///kw+FUNCTION+detail If a `FILE` is given, an ASCII file containing at least $n+1$ columns is expected.
///kw+FUNCTION+detail By default, the first $n$ columns are the values of the arguments and the last column
///kw+FUNCTION+detail is the value of the function at those points.
///kw+FUNCTION+detail The order of the columns can be changed with the keyword `COLUMNS`,
///kw+FUNCTION+detail which expects $n+1$ expressions corresponding to the column numbers.
      feenox_call(feenox_parser_file(&file));
      file->mode = "r";

///kw+FUNCTION+usage VECTORS <vector_1> <vector_2> ... <vector_n> <vector_data> |
///kw+FUNCTION+usage @  
///kw+FUNCTION+detail If `VECTORS` is given, a set of $n+1$ vectors of the same size is expected.
///kw+FUNCTION+detail The first $n$ correspond to the arguments and the last one to the function values.
    } else if (strcasecmp(token, "VECTORS") == 0) {
      feenox_call(feenox_parse_function_vectors(function));
      
///kw+FUNCTION+usage DATA <num_1> <num_2> ... <num_N>
///kw+FUNCTION+usage @
///kw+FUNCTION+usage }
///kw+FUNCTION+usage @
///kw+FUNCTION+detail The function can be pointwise-defined inline in the input using `DATA`.
///kw+FUNCTION+detail This should be the last keyword of the line, followed by $N=k \cdot (n+1)$ expresions
///kw+FUNCTION+detail giving\ $k$ definition points: $n$ arguments and the value of the function.
///kw+FUNCTION+detail Multiline continuation using brackets `{` and `}` can be used for a clean data organization.
    } else if (strcasecmp(token, "DATA") == 0) {
      feenox_call(feenox_parse_function_data(function));

///kw+FUNCTION+usage [ COLUMNS <expr_1> <expr_2> ... <expr_n> <expr_n+1> ]
///kw+FUNCTION+usage @
    } else if (strcasecmp(token, "COLUMNS") == 0) {

      if (function->n_arguments == 0) {
        feenox_push_error_message("number of arguments is equal to zero");
        return FEENOX_ERROR;
      }

      feenox_check_alloc(columns = calloc(function->n_arguments+1, sizeof(int)));
      unsigned int i = 0;
      double xi = 0;
      for (i = 0; i < function->n_arguments+1; i++) {
        feenox_call(feenox_parser_expression_in_string(&xi));
        columns[i] = (unsigned int)(xi);
      }
      
///kw+FUNCTION+detail Interpolation schemes can be given for either one or multi-dimensional functions with `INTERPOLATION`.
///kw+FUNCTION+detail Available schemes for $n=1$ are:
///kw+FUNCTION+detail @
///kw+FUNCTION+usage [ INTERPOLATION
///kw+FUNCTION+usage {
///kw+FUNCTION+usage linear |
///kw+FUNCTION+detail  * linear
///kw+FUNCTION+usage polynomial |
///kw+FUNCTION+detail  * polynomial, the grade is equal to the number of data minus one
///kw+FUNCTION+usage spline |
///kw+FUNCTION+detail  * spline, cubic (needs at least 3 points)
///kw+FUNCTION+usage spline_periodic |
///kw+FUNCTION+detail  * spline_periodic 
///kw+FUNCTION+usage akima |
///kw+FUNCTION+detail  * akima (needs at least 5 points)
///kw+FUNCTION+usage akima_periodic |
///kw+FUNCTION+detail  * akima_periodic (needs at least 5 points)
///kw+FUNCTION+usage steffen |
///kw+FUNCTION+usage @
///kw+FUNCTION+detail  * steffen, always-monotonic splines-like interpolator
///kw+FUNCTION+detail @
///kw+FUNCTION+detail Default interpolation scheme for one-dimensional functions is `DEFAULT_INTERPOLATION`.
///kw+FUNCTION+detail @
///kw+FUNCTION+detail Available schemes for $n>1$ are:
///kw+FUNCTION+detail @
///kw+FUNCTION+usage nearest |
///kw+FUNCTION+detail  * nearest, $f(\vec{x})$ is equal to the value of the closest definition point
///kw+FUNCTION+usage shepard |
///kw+FUNCTION+detail  * shepard, [inverse distance weighted average definition points](https:/\/en.wikipedia.org/wiki/Inverse_distance_weighting) (might lead to inefficient evaluation)
///kw+FUNCTION+usage shepard_kd |
///kw+FUNCTION+detail  * shepard_kd, [average of definition points within a kd-tree](https:/\/en.wikipedia.org/wiki/Inverse_distance_weighting#Modified_Shepard&#39;s_method) (more efficient evaluation provided `SHEPARD_RADIUS` is set to a proper value)
///kw+FUNCTION+usage bilinear
///kw+FUNCTION+detail  * bilinear, only available if the definition points configure an structured hypercube-like grid. If $n>3$, `SIZES` should be given.
///kw+FUNCTION+usage } ]
///kw+FUNCTION+usage @
    } else if (strcasecmp(token, "INTERPOLATION") == 0) {
      token = feenox_get_next_token(NULL);
      feenox_call(feenox_function_set_interpolation(name, token));

///kw+FUNCTION+detail @
///kw+FUNCTION+usage [ INTERPOLATION_THRESHOLD <expr> ]
///kw+FUNCTION+detail For $n>1$, if the euclidean distance between the arguments and the definition points is smaller than `INTERPOLATION_THRESHOLD`, the definition point is returned and no interpolation is performed.
///kw+FUNCTION+detail Default value is square root of `DEFAULT_MULTIDIM_INTERPOLATION_THRESHOLD`.
      } else if (strcasecmp(token, "INTERPOLATION_THRESHOLD") == 0) {

       feenox_parser_expression(&function->expr_multidim_threshold);

///kw+FUNCTION+detail @
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
       
    } else {
      feenox_push_error_message("unknown keyword '%s'", token);
      return FEENOX_ERROR;
    }
  }
       
/*
//kw+FUNCTION+usage [ SIZES <expr_1> <expr_2> ... <expr_n> ]
//kw+FUNCTION+detail When requesting `bilinear` interpolation for $n>3$, the number of definition points for each argument variable has to be given with `SIZES`,
    } else if (strcasecmp(token, "SIZES") == 0) {

      function->expr_rectangular_mesh_size = calloc(function->n_arguments, sizeof(expr_t));

      for (i = 0; i < function->n_arguments; i++) {
        feenox_call(feenox_parser_expression(&function->expr_rectangular_mesh_size[i]));
      }

//kw+FUNCTION+usage [ X_INCREASES_FIRST <expr> ]
//kw+FUNCTION+detail and wether the definition data is sorted with the first argument changing first (`X_INCREASES_FIRST` evaluating to non-zero) or with the last argument changing first (zero).
    } else if (strcasecmp(token,"X_INCREASES_FIRST") == 0) {

      feenox_call(feenox_parser_expression(&function->expr_x_increases_first));

    } else {
      feenox_push_error_message("unknown keyword '%s'", token);
      return FEENOX_ERROR;
    }
  }
*/
  
  if (file != NULL) {
    feenox_call(feenox_function_set_file(name, file, columns));
  }
  
  feenox_free(columns);
  feenox_free(name);
  
  return FEENOX_OK;
  
}

int feenox_parse_function_data(function_t *function) {

  function->type = function_type_pointwise_data;
  
  size_t size0 = 4096/sizeof(double);
  size_t size = size0;
  double *buffer = NULL;
  feenox_check_alloc(buffer = malloc(size * sizeof(double)));
  
  char *token = NULL;
  size_t n = 0;
  while ((token = feenox_get_next_token(NULL)) != NULL) {
    if (n == size) {
      size += size0;
      feenox_check_alloc(buffer = realloc(buffer, size*sizeof(double)));
    }
    
    expr_t *expr = NULL;
    feenox_check_alloc(expr = calloc(1, sizeof(expr_t)));
    feenox_call(feenox_expression_parse(expr, token));
    buffer[n++] = feenox_expression_eval(expr);
    feenox_free(expr);
  }

  feenox_call(function_set_buffered_data(function, buffer, n, function->n_arguments+1, NULL));
  feenox_free(buffer);
  
  return FEENOX_OK;
}
  

int feenox_parse_function_vectors(function_t *function) {

  function->type = function_type_pointwise_data;
  feenox_check_alloc(function->vector_argument = calloc(function->n_arguments, sizeof(vector_t *)));
  feenox_check_alloc(function->data_argument = calloc(function->n_arguments, sizeof(double *)));
  
  unsigned int i = 0;
  for (i = 0; i < function->n_arguments; i++) {
    feenox_call(feenox_parser_vector(&function->vector_argument[i]));
  } 
  feenox_call(feenox_parser_vector(&function->vector_value));
  
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
      feenox_check_alloc(custom_mode = strdup("r"));
    } else if (strcasecmp(token, "OUTPUT") == 0) {
      feenox_check_alloc(custom_mode = strdup("w"));

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

  feenox_free(format);
  feenox_free(custom_mode);
  feenox_free(name);
  
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
///kw+PRINT+detail Vectors are printed element-by-element in a single row.
///kw+PRINT+detail See `PRINT_VECTOR` to print one or more vectors with one element per line (i.e. vertically).
///kw+PRINT+detail Matrices are printed element-by-element in a single line using row-major ordering if mixed
///kw+PRINT+detail with other objects but in the natural row and column fashion
///kw+PRINT+detail if it is the only given object in the `PRINT` instruction.

///kw+PRINT+usage [ <object_1> <object_2> ... <object_n> ]
///kw+PRINT+usage [ TEXT <string_1> ... TEXT <string_n> ]
///kw+PRINT+usage @
  
  while ((token = feenox_get_next_token(NULL)) != NULL) {
///kw+PRINT+usage [ FILE { <file_path> | <file_id> } ]
///kw+PRINT+detail If the `FILE` keyword is not provided, default is to write to `stdout`.
    if (strcasecmp(token, "FILE") == 0 || strcasecmp(token, "FILE_PATH") == 0) {
      feenox_call(feenox_parser_file(&print->file));
      // we don't have the file name becuase it was already consumed by the above call
      // and we do not expect the PRINT instruction to be used from the API
      // so we directly access the internals of the structure
      if (print->file->mode == NULL) {
        feenox_check_alloc(print->file->mode = strdup("w"));
      }
 
///kw+PRINT+usage [ HEADER ]
    } else if (strcasecmp(token, "HEADER") == 0) {
      print->header = 1;
///kw+PRINT+detail If the `HEADER` keyword is given, a single line containing the literal text
///kw+PRINT+detail given for each object is printed at the very first time the `PRINT` instruction is
///kw+PRINT+detail processed, starting with a hash `#` character.           

///kw+PRINT+usage [ NONEWLINE ]
    } else if (strcasecmp(token, "NONEWLINE") == 0) {
///kw+PRINT+detail If the `NONEWLINE` keyword is not provided, default is to write a newline `\n` character after
///kw+PRINT+detail all the objects are processed.
///kw+PRINT+detail Otherwise, if the last token to be printed is a numerical value, a separator string will be printed but not the newline `\n` character.
///kw+PRINT+detail If the last token is a string, neither the separator nor the newline will be printed.
      print->nonewline = 1;

///kw+PRINT+usage [ SEP <string> ]
///kw+PRINT+detail The `SEP` keyword expects a string used to separate printed objects.
///kw+PRINT+detail To print objects without any separation in between give an empty string like `SEP ""`.
///kw+PRINT+detail The default is a tabulator character 'DEFAULT_PRINT_SEPARATOR' character. 
    } else if (strcasecmp(token, "SEP") == 0 || strcasecmp(token, "SEPARATOR") == 0) {
      feenox_call(feenox_parser_string(&print->separator));
      
///kw+PRINT+detail To print an empty line write `PRINT` without arguments.

///kw+PRINT+usage @
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
      
///kw+PRINT+usage @
// see the objects above
      print_token_t *print_token = calloc(1, sizeof(print_token_t));
      LL_APPEND(print->tokens, print_token);

      if (token[0] == '%') {
        feenox_check_alloc(print_token->format = strdup(token));

      } else if (strcasecmp(token, "STRING") == 0 || strcasecmp(token, "TEXT") == 0) {
        if (feenox_parser_string(&print_token->text) != FEENOX_OK) {
          return FEENOX_ERROR;
        }

      } else if ((dummy_matrix = feenox_get_matrix_ptr(token)) != NULL) {
        feenox_check_alloc(print_token->text = strdup(token));   // nos quedamos con el texto para el header
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
            print_token->expression.items = NULL;
            feenox_check_alloc(print_token->text = strdup(token));
            feenox_pop_error_message();
          }
        } else {
          feenox_check_alloc(print_token->text = strdup(token));   // text for the header
        }
      }
    }
  }

  // default separator
  if (print->separator == NULL) {
    feenox_check_alloc(print->separator = strdup(DEFAULT_PRINT_SEPARATOR));
  }
  
  // default file is stdout
  if (print->file == NULL) {
    print->file = feenox.special_files.stdout_;
  }
  
  LL_APPEND(feenox.prints, print);
  feenox_call(feenox_add_instruction(feenox_instruction_print, print));
  
  return FEENOX_OK;
}

int feenox_parse_print_function(void) {

  char *token;

  print_function_t *print_function = NULL;
  feenox_check_alloc(print_function = calloc(1, sizeof(print_function_t)));

  while ((token = feenox_get_next_token(NULL)) != NULL) {

///kw+PRINT_FUNCTION+detail Each argument should be either a function or an expression.
///kw+PRINT_FUNCTION+detail The output of this instruction consists of\ $n+k$ columns,
///kw+PRINT_FUNCTION+detail where\ $n$ is the number of arguments of the first function of the list
///kw+PRINT_FUNCTION+detail and\ $k$ is the number of functions and expressions given.
///kw+PRINT_FUNCTION+detail The first\ $n$ columns are the arguments (independent variables) and 
///kw+PRINT_FUNCTION+detail the last\ $k$ one has the evaluated functions and expressions.
///kw+PRINT_FUNCTION+detail The columns are separated by a tabulator, which is the format that most
///kw+PRINT_FUNCTION+detail plotting tools understand.
///kw+PRINT_FUNCTION+detail Only function names without arguments are expected.
///kw+PRINT_FUNCTION+detail All functions should have the same number of arguments.
///kw+PRINT_FUNCTION+detail Expressions can involve the arguments of the first function.
    
///kw+PRINT_FUNCTION+usage <function_1> [ { function | expr } ... { function | expr } ]

///kw+PRINT_FUNCTION+usage @
///kw+PRINT_FUNCTION+usage [ FILE { <file_path> | <file_id> } ]
///kw+PRINT_FUNCTION+detail If the `FILE` keyword is not provided, default is to write to `stdout`.
    if (strcasecmp(token, "FILE") == 0 || strcasecmp(token, "FILE_PATH") == 0) {
      feenox_call(feenox_parser_file(&print_function->file));
      if (print_function->file->mode == NULL) {
        feenox_check_alloc(print_function->file->mode = strdup("w"));
      }
      
///kw+PRINT_FUNCTION+usage [ HEADER ]
///kw+PRINT_FUNCTION+detail If `HEADER` is given, the output is prepended with a single line containing the
///kw+PRINT_FUNCTION+detail names of the arguments and the names of the functions, separated by tabs.
///kw+PRINT_FUNCTION+detail The header starts with a hash\ `#` that usually acts as a comment and is ignored
///kw+PRINT_FUNCTION+detail by most plotting tools.
    } else if (strcasecmp(token, "HEADER") == 0) {
      print_function->header = 1;
          
///kw+PRINT_FUNCTION+usage @
///kw+PRINT_FUNCTION+usage [ MIN <expr_1> <expr_2> ... <expr_k> ]
///kw+PRINT_FUNCTION+detail If there is no explicit range where to evaluate the functions and the first function
///kw+PRINT_FUNCTION+detail is point-wise defined, they are evalauted at the points of definition of the first one.
///kw+PRINT_FUNCTION+detail The range can be explicitly given as a product of\ $n$ ranges\ $[x_{i,\min},x_{i,\max}]$
///kw+PRINT_FUNCTION+detail for $i=1,\dots,n$.      
      
///kw+PRINT_FUNCTION+detail The values $x_{i,\min}$ and $x_{i,\max}$ are given with the `MIN` _and_ `MAX` keywords.
    } else if (strcasecmp(token, "MIN") == 0) {
      if (print_function->first_function == NULL) {
        feenox_push_error_message("MIN before actual function, cannot determine number of arguments");
        return FEENOX_ERROR;
      }

      if (feenox_parser_expressions(&print_function->range.min, print_function->first_function->n_arguments) != FEENOX_OK) {
        return FEENOX_ERROR;
      }

///kw+PRINT_FUNCTION+usage [ MAX <expr_1> <expr_2> ... <expr_k> ]
    } else if (strcasecmp(token, "MAX") == 0) {

      if (print_function->first_function == NULL) {
        feenox_push_error_message("MAX before actual function, cannot determine number of arguments");
        return FEENOX_ERROR;
      }

      if (feenox_parser_expressions(&print_function->range.max, print_function->first_function->n_arguments) != FEENOX_OK) {
        return FEENOX_ERROR;
      }

///kw+PRINT_FUNCTION+detail The discretization steps of the ranges are given by either `STEP` that gives\ $\delta x$ _or_
///kw+PRINT_FUNCTION+detail `NSTEPS` that gives the number of steps.
///kw+PRINT_FUNCTION+usage @
///kw+PRINT_FUNCTION+usage [ STEP <expr_1> <expr_2> ... <expr_k> ]
    } else if (strcasecmp(token, "STEP") == 0) {

      if (print_function->first_function == NULL) {
        feenox_push_error_message("STEP before actual function, cannot determine number of arguments");
        return FEENOX_ERROR;
      }

      if (feenox_parser_expressions(&print_function->range.step, print_function->first_function->n_arguments) != FEENOX_OK) {
        return FEENOX_ERROR;
      }

///kw+PRINT_FUNCTION+usage [ NSTEPs <expr_1> <expr_2> ... <expr_k> ]
    } else if (strcasecmp(token, "NSTEPS") == 0) {

      if (print_function->first_function == NULL) {
        feenox_push_error_message("NSTEPS before actual function, cannot determine number of arguments");
        return FEENOX_ERROR;
      }

      if (feenox_parser_expressions(&print_function->range.nsteps, print_function->first_function->n_arguments) != FEENOX_OK) {
        return FEENOX_ERROR;
      }
///kw+PRINT_FUNCTION+detail If the first function is not point-wise defined, the ranges are mandatory.

///kw+PRINT_FUNCTION+usage @
///kw+PRINT_FUNCTION+usage [ FORMAT <print_format> ]
    } else if (strcasecmp(token, "FORMAT") == 0) {
      // TODO: like in PRINT  
      if (feenox_parser_string(&print_function->format) != FEENOX_OK) {
          return FEENOX_ERROR;
      }
          
// TODO: mesh & physical group
//kw+PRINT_FUNCTION+usage [ PHYSICAL_ENTITY <name> ]
/*
    } else if (strcasecmp(token, "PHYSICAL_ENTITY") == 0) {
      char *name;
      feenox_call(feenox_parser_string(&name));
      if ((print_function->physical_entity = feenox_get_physical_entity_ptr(name, NULL)) == NULL) {
        feenox_push_error_message("unknown physical entity '%s'", name);
        feenox_free(name);
        return FEENOX_ERROR;
      }
*/
    } else {

      // add an item to the list of tokens
      print_token_t *print_token = NULL;
      feenox_check_alloc(print_token = calloc(1, sizeof(print_token_t)));
      LL_APPEND(print_function->tokens, print_token);

      feenox_check_alloc(print_token->text = strdup(token));   // text for the header
      function_t *function = NULL;
      if ((function = feenox_get_function_ptr(token)) != NULL) {
        function->used = 1;
        print_token->function = function;
        if (print_function->first_function == NULL) {
          // the first function
          print_function->first_function = function;
        } else if (function->n_arguments !=  print_function->first_function->n_arguments) {
          feenox_push_error_message("functions do not have the same number of arguments");
          return FEENOX_ERROR;
        }
        
      } else {
        feenox_call(feenox_expression_parse(&print_token->expression, token));
      }
    }
  }

  if (print_function->first_function == NULL) {
    feenox_push_error_message("at least one function expected");
    return FEENOX_ERROR;
  }
      
  if (print_function->first_function->type == function_type_algebraic) {
//      || print_function->first_function->type == function_type_routine) {
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
    feenox_check_alloc(print_function->separator = strdup(DEFAULT_PRINT_SEPARATOR));
  }

  if (print_function->format == NULL) {
    feenox_check_alloc(print_function->format = strdup(DEFAULT_PRINT_FORMAT));
  }

  if (print_function->file == NULL) {
    print_function->file = feenox.special_files.stdout_;
  }
  
  LL_APPEND(feenox.print_functions, print_function);
  feenox_call(feenox_add_instruction(feenox_instruction_print_function, print_function));
  
  return FEENOX_OK;
}

int feenox_parse_print_vector(void) {

  print_vector_t *print_vector = NULL;
  feenox_check_alloc(print_vector = calloc(1, sizeof(print_vector_t)));

  char *token = NULL;
  while ((token = feenox_get_next_token(NULL)) != NULL) {
///kw+PRINT_FUNCTION+usage <vector_1> [ { vector | expr } ... { vector | expr } ]

///kw+PRINT_VECTOR+detail Each argument should be either a vector or an expression of the integer\ `i`.
///kw+PRINT_VECTOR+usage @
///kw+PRINT_VECTOR+usage [ FILE { <file_path> | <file_id> } ]
///kw+PRINT_VECTOR+detail If the `FILE` keyword is not provided, default is to write to `stdout`.
    if (strcasecmp(token, "FILE") == 0 || strcasecmp(token, "FILE_PATH") == 0) {
      feenox_call(feenox_parser_file(&print_vector->file));
      if (print_vector->file->mode == NULL) {
        feenox_check_alloc(print_vector->file->mode = strdup("w"));
      }
      
///kw+PRINT_VECTOR+usage [ HEADER ]
///kw+PRINT_VECTOR+detail If `HEADER` is given, the output is prepended with a single line containing the
///kw+PRINT_VECTOR+detail names of the arguments and the names of the functions, separated by tabs.
///kw+PRINT_VECTOR+detail The header starts with a hash\ `#` that usually acts as a comment and is ignored
///kw+PRINT_VECTOR+detail by most plotting tools.
//    } else if (strcasecmp(token, "HEADER") == 0) {
//      print_vector->header = 1;
          
///kw+PRINT_VECTOR+usage @
///kw+PRINT_VECTOR+usage [ FORMAT <print_format> ]
    } else if (strcasecmp(token, "FORMAT") == 0) {
      // TODO: like in PRINT  
      if (feenox_parser_string(&print_vector->format) != FEENOX_OK) {
          return FEENOX_ERROR;
      }

    } else {

      // agregamos un eslabon a la lista de tokens
      print_token_t *print_token = calloc(1, sizeof(print_token_t));
      LL_APPEND(print_vector->tokens, print_token);

      if ((print_token->vector = feenox_get_vector_ptr(token)) != NULL) {
       if (print_vector->first_vector == NULL) {
         // es el primer vector
         print_vector->first_vector = print_token->vector;
        } 
            
      } else {
        feenox_call(feenox_expression_parse(&print_token->expression, token));
            
      }
    }
  }

  // llenamos defaults
  if (print_vector->format == NULL) {
    feenox_check_alloc(print_vector->format = strdup(DEFAULT_PRINT_FORMAT));
  }

  if (print_vector->separator == NULL) {
    print_vector->separator = strdup(DEFAULT_PRINT_SEPARATOR);
  }

  if (print_vector->file == NULL) {
    print_vector->file = feenox.special_files.stdout_;
  }

  LL_APPEND(feenox.print_vectors, print_vector);
  feenox_call(feenox_add_instruction(feenox_instruction_print_vector, print_vector));
      
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

  return FEENOX_OK;
}

int feenox_parse_phase_space(void) {
    
///kw_dae+PHASE_SPACE+usage PHASE_SPACE <vars> ... <vectors> ... <matrices> ... 

  if (feenox.dae.dimension != 0) {
    feenox_push_error_message("PHASE_SPACE keyword already given");
    return FEENOX_ERROR;
  }
#ifdef HAVE_SUNDIALS
  if (sizeof(realtype) != sizeof(double)) {
    feenox_push_error_message("\nSUNDIALS was compiled using a different word size than FeenoX, please recompile with double precision floating point arithmetic.");
    return FEENOX_ERROR;
  }
#endif

  int differential = 0;
  char *token = NULL;
  while ((token = feenox_get_next_token(NULL)) != NULL) {
    if (strcmp(token, "DIFFERENTIAL") == 0) {
      differential = 1;
    } else if (strcmp(token, "ALGEBRAIC") == 0) {
      differential = 0;
    } else {
      feenox_call(feenox_phase_space_add_object(token, differential));
    }
  }

  return FEENOX_OK;
}

int feenox_parse_read_mesh(void) {
  
  // we have at least one mesh so we need to define the whole set of special vars
  feenox_call(feenox_mesh_init_special_objects());
  
  // TODO: api?
  mesh_t *mesh = NULL;
  feenox_check_alloc(mesh = calloc(1, sizeof(mesh_t)));

///kw_pde+READ_MESH+usage { <file_path> | <file_id> }
///kw_pde+READ_MESH+detail Either a file identifier (defined previously with a `FILE` keyword) or a file path should be given.
///kw_pde+READ_MESH+detail The format is read from the extension, which should be either
///kw_pde+READ_MESH+detail @
///kw_pde+READ_MESH+detail  * `.msh`, `.msh2` or `.msh4` [Gmsh ASCII format](http:\/\/gmsh.info/doc/texinfo/gmsh.html#MSH-file-format), versions 2.2, 4.0 or 4.1
///kw_pde+READ_MESH+detail  * `.vtk` [ASCII legacy VTK](https:\/\/lorensen.github.io/VTKExamples/site/VTKFileFormats/)
///kw_pde+READ_MESH+detail  * `.frd` [CalculiX’s FRD ASCII output](https:\/\/web.mit.edu/calculix_v2.7/CalculiX/cgx_2.7/doc/cgx/node4.html))
///kw_pde+READ_MESH+detail @
///kw_pde+READ_MESH+detail Note than only MSH is suitable for defining PDE domains, as it is the only one that provides
///kw_pde+READ_MESH+detail physical groups (a.k.a labels) which are needed in order to define materials and boundary conditions.
///kw_pde+READ_MESH+detail The other formats are primarily supported to read function data contained in the file
///kw_pde+READ_MESH+detail and eventually, to operate over these functions (i.e. take differences with other functions contained
///kw_pde+READ_MESH+detail in other files to compare results).
///kw_pde+READ_MESH+detail The file path or file id can be used to refer to a particular mesh when reading more than one,
///kw_pde+READ_MESH+detail for instance in a `WRITE_MESH` or `INTEGRATE` keyword.
///kw_pde+READ_MESH+detail If a file path is given such as `cool_mesh.msh`, it can be later referred to as either
///kw_pde+READ_MESH+detail `cool_mesh.msh` or just `cool_mesh`.  
  feenox_call(feenox_parser_file(&mesh->file));
  if (mesh->file->mode == NULL) {
    feenox_check_alloc(mesh->file->mode = strdup("r"));
  }
    
  char *token = NULL;
  while ((token = feenox_get_next_token(NULL)) != NULL) {          
///kw_pde+READ_MESH+detail The spatial dimensions cab be given with `DIMENSION`.
///kw_pde+READ_MESH+detail If material properties are uniform and given with variables,
///kw_pde+READ_MESH+detail the number of dimensions are not needed and will be read from the file at runtime.
///kw_pde+READ_MESH+detail But if either properties are given by spatial functions or if functions
///kw_pde+READ_MESH+detail are to be read from the mesh with `READ_DATA` or `READ_FUNCTION`, then
///kw_pde+READ_MESH+detail the number of dimensions ought to be given explicitly because FeenoX needs to know
///kw_pde+READ_MESH+detail how many arguments these functions take. 
///kw_pde+READ_MESH+usage [ DIMENSIONS <num_expr> ]@
    if (strcasecmp(token, "DIMENSIONS") == 0) {
      double xi;
      feenox_call(feenox_parser_expression_in_string(&xi));
      mesh->dim = (int)(round(xi));
      if (mesh->dim < 1 || mesh->dim > 3) {
        feenox_push_error_message("mesh dimensions have to be either 1, 2 or 3, not '%g'", xi);
        return FEENOX_ERROR;
      }

///kw_pde+READ_MESH+detail If either `OFFSET` and/or `SCALE` are given, the node locations are first shifted and then scaled by the provided values.
///kw_pde+READ_MESH+usage [ SCALE <expr> ]
    } else if (strcasecmp(token, "SCALE") == 0) {
      feenox_call(feenox_parser_expression(&mesh->scale_factor));

///kw_pde+READ_MESH+usage [ OFFSET <expr_x> <expr_y> <expr_z> ]@
    } else if (strcasecmp(token, "OFFSET") == 0) {
      feenox_call(feenox_parser_expression(&mesh->offset_x));
      feenox_call(feenox_parser_expression(&mesh->offset_y));
      feenox_call(feenox_parser_expression(&mesh->offset_z));

///kw_pde+READ_MESH+usage [ INTEGRATION { full | reduced } ]@
    } else if (strcasecmp(token, "INTEGRATION") == 0) {
      char *keywords[] = {"full", "reduced", ""};
      int values[] = {integration_full, integration_reduced, 0};
      feenox_call(feenox_parser_keywords_ints(keywords, values, (int *)(&mesh->integration)));

///kw_pde+READ_MESH+detail When defining several meshes and solving a PDE problem, the mesh used
///kw_pde+READ_MESH+detail as the PDE domain is the one marked with `MAIN`.
///kw_pde+READ_MESH+detail If none of the meshes is explicitly marked as main, the first one is used.
///kw_pde+READ_MESH+usage [ MAIN ]
    } else if (strcasecmp(token, "MAIN") == 0) {
      feenox.mesh.mesh_main = mesh;

///kw_pde+READ_MESH+detail If `UPDATE_EACH_STEP` is given, then the mesh data is re-read from the file at
///kw_pde+READ_MESH+detail each time step. Default is to read the mesh once, except if the file path changes with time.
///kw_pde+READ_MESH+usage [ UPDATE_EACH_STEP ]@
    } else if (strcasecmp(token, "UPDATE_EACH_STEP") == 0 || strcasecmp(token, "RE_READ") == 0) {
      mesh->update_each_step = 1;


///kw_pde+READ_MESH+detail For each `READ_FIELD` keyword, a point-wise defined function of space named `<function_name>`
///kw_pde+READ_MESH+detail is defined and filled with the scalar data named `<name_in_mesh>`  contained in the mesh file.
///kw_pde+READ_MESH+usage [ READ_FIELD <name_in_mesh> AS <function_name> ] [ READ_FIELD ... ] @
///kw_pde+READ_MESH+detail The `READ_FUNCTION` keyword is a shortcut when the scalar name and the to-be-defined function are the same.
///kw_pde+READ_MESH+usage [ READ_FUNCTION <function_name> ] [READ_FUNCTION ...] @
    } else if (strcasecmp(token, "READ_FIELD") == 0 || strcasecmp(token, "READ_FUNCTION") == 0) {

      int custom_name = (strcasecmp(token, "READ_FIELD") == 0);

      if (mesh->dim == 0) {
        feenox_push_error_message("READ_FIELD needs DIMENSIONS to be set", token);
        return FEENOX_ERROR;
      }

      char *name_in_mesh = NULL;
      feenox_call(feenox_parser_string(&name_in_mesh));

      char *function_name = NULL;
      if (custom_name) {
        // the "AS"
        char *as = NULL;
        feenox_call(feenox_parser_string(&as));
        if (strcasecmp(as, "AS") != 0) {
          feenox_push_error_message("expected AS instead of '%s'", as);
          return FEENOX_ERROR;
        }
        feenox_free(as);

        feenox_call(feenox_parser_string(&function_name));
      } else {
        feenox_check_alloc(function_name = strdup(name_in_mesh));
      }

      node_data_t *node_data = NULL;
      feenox_check_alloc(node_data = calloc(1, sizeof(node_data_t)));
      feenox_check_alloc(node_data->name_in_mesh = strdup(name_in_mesh));
      node_data->function = feenox_define_function_get_ptr(function_name, mesh->dim);
      LL_APPEND(mesh->node_datas, node_data);
      feenox_free(name_in_mesh);
      feenox_free(function_name);

    } else {
      feenox_push_error_message("unknown keyword '%s'", token);
      return FEENOX_ERROR;
    }
  }

///kw_pde+READ_MESH+detail If no mesh is marked as `MAIN`, the first one is the main one.
  if (feenox.mesh.meshes == NULL) {
    feenox.mesh.mesh_main = mesh;
  }
  
  if (mesh->file == NULL) {
    feenox_push_error_message("no FILE given for READ_MESH");
    return FEENOX_ERROR;
  }

  char *ext = strrchr(mesh->file->format, '.');
  if (ext == NULL) {
    feenox_push_error_message("no file extension given", ext);
    return FEENOX_ERROR;
  }

  // TODO: pointer to reader function
  if (strncasecmp(ext, ".msh", 4) == 0 ||
      strncasecmp(ext, ".msh2", 5) == 0 ||
      strncasecmp(ext, ".msh4", 5) == 0) {
    mesh->reader = feenox_mesh_read_gmsh;
  } else if (strcasecmp(ext, ".vtk") == 0) {
    mesh->reader = feenox_mesh_read_vtk;
  } else if (strcasecmp(ext, ".frd") == 0) {
    mesh->reader = feenox_mesh_read_frd;
  } else {
    feenox_push_error_message("unknown extension '%s'", ext);
    return FEENOX_ERROR;
  }

  // if nobody told us the dimension, check if there is one in the PROBLEM keyword
  if (mesh->dim == 0 && feenox.pde.dim != 0) {
    mesh->dim = feenox.pde.dim;
  }
  
  // TODO: API?
  if (feenox_get_mesh_ptr(mesh->file->name) != NULL) {
    feenox_push_error_message("there already exists a mesh named '%s'", mesh->file->name);
    return FEENOX_ERROR;
  }
  HASH_ADD_KEYPTR(hh, feenox.mesh.meshes, mesh->file->name, strlen(mesh->file->name), mesh);
  feenox_call(feenox_add_instruction(feenox_instruction_mesh_read, mesh));
  
  return FEENOX_OK;
}


int feenox_parse_write_mesh(void) {
  
  mesh_write_t *mesh_write = NULL;
  feenox_check_alloc(mesh_write = calloc(1, sizeof(mesh_write_t)));

///kw_pde+WRITE_MESH+usage { <file_path> | <file_id> }
///kw_pde+WRITE_MESH+detail Either a file identifier (defined previously with a `FILE` keyword) or a file path should be given.
///kw_pde+WRITE_MESH+detail The format is automatically detected from the extension. Otherwise, the keyword `FILE_FORMAT` can
///kw_pde+WRITE_MESH+detail be use to give the format explicitly.

  feenox_call(feenox_parser_file(&mesh_write->file));
  if (mesh_write->file->mode == NULL) {
    feenox_check_alloc(mesh_write->file->mode = strdup("w"));
  }
  
  char *token = NULL;
  while ((token = feenox_get_next_token(NULL)) != NULL) {          
///kw_pde+WRITE_MESH+usage [ MESH <mesh_identifier> ]
///kw_pde+WRITE_MESH+detail If there are several meshes defined then which one should be used has to be
///kw_pde+WRITE_MESH+detail given explicitly with `MESH`.
    if (strcasecmp(token, "MESH") == 0) {
      
      char *mesh_name = NULL;
      feenox_call(feenox_parser_string(&mesh_name));
      if ((mesh_write->mesh = feenox_get_mesh_ptr(mesh_name)) == NULL) {
        feenox_push_error_message("unknown mesh '%s'", mesh_name);
        feenox_free(mesh_name);
        return FEENOX_ERROR;
      }
      feenox_free(mesh_name);
          
///kw_pde+WRITE_MESH+usage [ NO_MESH ]
///kw_pde+WRITE_MESH+detail If the `NO_MESH` keyword is given, only the results are written into the output file.
///kw_pde+WRITE_MESH+detail Depending on the output format, this can be used to avoid repeating data and/or
///kw_pde+WRITE_MESH+detail creating partial output files which can the be latter assembled by post-processing scripts.
    } else if (strcasecmp(token, "NOMESH") == 0 || strcasecmp(token, "NO_MESH") == 0) {
      mesh_write->no_mesh = 1;

///kw_pde+WRITE_MESH+usage [ FILE_FORMAT { gmsh | vtk } ]
    } else if (strcasecmp(token, "FILE_FORMAT") == 0) {
      char *keywords[] = {"gmsh", "vtk", ""};
      int values[] = {post_format_gmsh, post_format_vtk, 0};
      feenox_call(feenox_parser_keywords_ints(keywords, values, (int *)&mesh_write->post_format));

///kw_pde+WRITE_MESH+usage [ NO_PHYSICAL_NAMES ]
///kw_pde+WRITE_MESH+detail When targetting the `.msh` output format, if `NO_PHYSICAL_NAMES` is given then the
///kw_pde+WRITE_MESH+detail section that sets the actual names of the physical entities is not written.      
///kw_pde+WRITE_MESH+detail This can be needed to avoid name clashes when dealing with multiple `.msh` files.
    } else if (strcasecmp(token, "NO_PHYSICAL_NAMES") == 0) {
      mesh_write->no_physical_names = 1;
      
///kw_pde+WRITE_MESH+usage  [ NODE |
///kw_pde+WRITE_MESH+detail The output is node-based by default. This can be controlled with both the
///kw_pde+WRITE_MESH+detail `NODE` and `CELL` keywords. All fields that come after a `NODE` (`CELL`) keyword
///kw_pde+WRITE_MESH+detail will be written at the node (cells). These keywords can be used several times
///kw_pde+WRITE_MESH+detail and mixed with fields. For example `CELL k(x,y,z) NODE T sqrt(x^2+y^2) CELL 1+z` will
///kw_pde+WRITE_MESH+detail write the conductivity and the expression $1+z$ as cell-based and the temperature
///kw_pde+WRITE_MESH+detail $T(x,y,z)$ and the expression $\sqrt{x^2+y^2}$ as a node-based fields.
    } else if (strcasecmp(token, "NODE") == 0 || strcasecmp(token, "NODES") == 0) {
      mesh_write->field_location = field_location_nodes;

///kw_pde+WRITE_MESH+usage CELL ]
    } else if (strcasecmp(token, "CELL") == 0 || strcasecmp(token, "CELLS") == 0) {
      mesh_write->field_location = field_location_cells;
      feenox.mesh.need_cells = 1;

///kw_pde+WRITE_MESH+detail Also, the `SCALAR_FORMAT` keyword can be used to define the precision of the ASCII
///kw_pde+WRITE_MESH+detail representation of the fields that follow. Default is `%g`.
///kw_pde+WRITE_MESH+usage [ SCALAR_FORMAT <printf_specification>]
    } else if (strcasecmp(token, "SCALAR_FORMAT") == 0) {
      feenox_call(feenox_parser_string(&mesh_write->printf_format));
      
///kw_pde+WRITE_MESH+detail The data to be written has to be given as a list of fields,
///kw_pde+WRITE_MESH+detail i.e. distributions (such as `k` or `E`), functions of space (such as `T`)
///kw_pde+WRITE_MESH+detail and/or expressions (such as `x^2+y^2+z^2`).
///kw_pde+WRITE_MESH+detail Each field is written as a scalar, unless the keyword `VECTOR` is given.
///kw_pde+WRITE_MESH+detail In this case, there should be exactly three fields following `VECTOR`.
///kw_pde+WRITE_MESH+usage [ VECTOR <field_x> <field_y> <field_z> ] [...]
    } else if (strcasecmp(token, "VECTOR") == 0) {
      
      mesh_write_dist_t *mesh_write_dist = NULL;
      feenox_check_alloc(mesh_write_dist = calloc(1, sizeof(mesh_write_dist_t)));
      feenox_check_alloc(mesh_write_dist->vector = calloc(3, sizeof(function_t *)));
          
      unsigned int i = 0;
      for (i = 0; i < 3; i++) {
            
        if ((token = feenox_get_next_token(NULL)) == NULL) {
          feenox_push_error_message("expected function name");
          return FEENOX_ERROR;
        }
            
        if ((mesh_write_dist->vector[i] = feenox_get_function_ptr(token)) == NULL) {
          feenox_check_alloc(mesh_write_dist->vector[i] = calloc(1, sizeof(function_t)));
          feenox_check_alloc(mesh_write_dist->vector[i]->name = strdup(token));
          mesh_write_dist->vector[i]->type = function_type_algebraic;
          mesh_write_dist->vector[i]->n_arguments = 3;
          mesh_write_dist->vector[i]->n_arguments_given = 3;
          mesh_write_dist->vector[i]->var_argument = feenox.mesh.vars.arr_x;
          feenox_call(feenox_expression_parse(&mesh_write_dist->vector[i]->algebraic_expression, token)); 
        }
        mesh_write_dist->vector[i]->used = 1;
        
        mesh_write_dist->field_location = mesh_write->field_location;
        if (mesh_write->printf_format != NULL) {
          feenox_check_alloc(mesh_write_dist->printf_format = strdup(mesh_write->printf_format));
        }  
      }
          
      LL_APPEND(mesh_write->mesh_write_dists, mesh_write_dist);
          
          
    } else {
          
///kw_pde+WRITE_MESH+usage [ <field_1> ] [ <field_2> ] ...
      mesh_write_dist_t *mesh_write_dist = NULL;
      feenox_check_alloc(mesh_write_dist = calloc(1, sizeof(mesh_write_dist_t)));
          
      if ((mesh_write_dist->scalar = feenox_get_function_ptr(token)) == NULL) {
        feenox_check_alloc(mesh_write_dist->scalar = calloc(1, sizeof(function_t)));
        feenox_check_alloc(mesh_write_dist->scalar->name = strdup(token));
        mesh_write_dist->scalar->type = function_type_algebraic;
        mesh_write_dist->scalar->n_arguments = 3;
        mesh_write_dist->scalar->n_arguments_given = 3;
        mesh_write_dist->scalar->var_argument = feenox.mesh.vars.arr_x;
        feenox_call(feenox_expression_parse(&mesh_write_dist->scalar->algebraic_expression, token)); 
      }
      
      mesh_write_dist->scalar->used = 1;
      mesh_write_dist->field_location = mesh_write->field_location;
      if (mesh_write->printf_format != NULL) {
        feenox_check_alloc(mesh_write_dist->printf_format = strdup(mesh_write->printf_format));
      }  
      
      LL_APPEND(mesh_write->mesh_write_dists, mesh_write_dist);
    }
  }

  // if there's only one mesh, use that one, otherwise ask which one 
  if (mesh_write->mesh == NULL) {
    if (feenox.mesh.mesh_main == feenox.mesh.meshes) {
      mesh_write->mesh = feenox.mesh.mesh_main;
    } else {
      feenox_push_error_message("do not know what mesh should the post-processing be applied to");
      return FEENOX_ERROR;
    }
  }

  if (mesh_write->post_format == post_format_fromextension) {
    char *ext = mesh_write->file->format + strlen(mesh_write->file->format) - 4;

           if (strcasecmp(ext, ".pos") == 0 || strcasecmp(ext, ".msh") == 0) {
      mesh_write->post_format = post_format_gmsh;
    } else if (strcasecmp(ext, ".vtk") == 0) {
      mesh_write->post_format = post_format_vtk;
    } else {
      feenox_push_error_message("unknown extension '%s' and no FORMAT given", ext);
      return FEENOX_ERROR;
    }
  }

  
  // TODO
  switch (mesh_write->post_format) {
    case post_format_gmsh:
      mesh_write->write_header = feenox_mesh_write_header_gmsh;
      mesh_write->write_mesh   = feenox_mesh_write_mesh_gmsh;
      mesh_write->write_scalar = feenox_mesh_write_scalar_gmsh;
      mesh_write->write_vector = feenox_mesh_write_vector_gmsh;
    break;
    case post_format_vtk:
      mesh_write->write_header = feenox_mesh_write_header_vtk;
      mesh_write->write_mesh   = feenox_mesh_write_mesh_vtk;
      mesh_write->write_scalar = feenox_mesh_write_scalar_vtk;
      mesh_write->write_vector = feenox_mesh_write_vector_vtk;
    break;
    default:
      return FEENOX_ERROR;
    break;
  }

  LL_APPEND(feenox.mesh.mesh_writes, mesh_write);
  feenox_call(feenox_add_instruction(feenox_instruction_mesh_write, mesh_write));
  
  return FEENOX_OK;
}


int feenox_parse_physical_group(void) {
///kw_pde+PHYSICAL_GROUP+detail This keyword should seldom be needed. Most of the times,
///kw_pde+PHYSICAL_GROUP+detail  a combination of `MATERIAL` and `BC` ought to be enough for most purposes.
  
///kw_pde+PHYSICAL_GROUP+usage <name>
///kw_pde+PHYSICAL_GROUP+detail The name of the `PHYSICAL_GROUP` keyword should match the name of the physical group defined within the input file.
///kw_pde+PHYSICAL_GROUP+detail If there is no physical group with the provided name in the mesh, this instruction has no effect.
  char *name = NULL;
  feenox_call(feenox_parser_string(&name));

  char *token = NULL;
  char *mesh_name = NULL;
  char *material_name = NULL;
  char *bc_name = NULL;
  unsigned int dimension = 0;
  unsigned int id = 0;
  while ((token = feenox_get_next_token(NULL)) != NULL) {          
///kw_pde+PHYSICAL_GROUP+usage [ MESH <name> ]
///kw_pde+PHYSICAL_GROUP+detail If there are many meshes, an explicit mesh can be given with `MESH`.
///kw_pde+PHYSICAL_GROUP+detail Otherwise, the physical group is defined on the main mesh.
    if (strcasecmp(token, "MESH") == 0) {
      feenox_call(feenox_parser_string(&mesh_name));

///kw_pde+PHYSICAL_GROUP+usage [ DIMENSION <expr> ]
///kw_pde+PHYSICAL_GROUP+detail An explicit dimension of the physical group can be provided with `DIMENSION`.
    } else if (strcasecmp(token, "DIMENSION") == 0 || strcasecmp(token, "DIM") == 0) {
      double xi;
      feenox_call(feenox_parser_expression_in_string(&xi));
      dimension = (unsigned int)(round(xi));

///kw_pde+PHYSICAL_GROUP+usage [ ID <expr> ]@
///kw_pde+PHYSICAL_GROUP+detail An explicit id can be given with `ID`.
///kw_pde+PHYSICAL_GROUP+detail Both dimension and id should match the values in the mesh.
    } else if (strcasecmp(token, "ID") == 0 || strcasecmp(token, "DIM") == 0) {
      double xi;
      feenox_call(feenox_parser_expression_in_string(&xi));
      id = (unsigned int)(round(xi));
      
///kw_pde+PHYSICAL_GROUP+usage [ MATERIAL <name> |
///kw_pde+PHYSICAL_GROUP+detail For volumetric elements, physical groups can be linked to materials using `MATERIAL`.
///kw_pde+PHYSICAL_GROUP+detail Note that if a material is created with the same name as a physical group in the mesh,
///kw_pde+PHYSICAL_GROUP+detail they will be linked automatically, so there is no need to use `PHYSCAL_GROUP` for this.
///kw_pde+PHYSICAL_GROUP+detail The `MATERIAL` keyword in `PHYSICAL_GROUP` is used to link a physical group
///kw_pde+PHYSICAL_GROUP+detail in a mesh file and a material in the feenox input file with different names.
///kw_pde+PHYSICAL_GROUP+detail 
    } else if (strcasecmp(token, "MATERIAL") == 0) {
      feenox_call(feenox_parser_string(&material_name));
  
///kw_pde+PHYSICAL_GROUP+usage | BC <name> [ BC ... ] ]@
    } else if (strcasecmp(token, "BC") == 0 || strcasecmp(token, "BOUNDARY_CONDITION") == 0) {
///kw_pde+PHYSICAL_GROUP+detail Likewise, for non-volumetric elements, physical groups can be linked to boundary using `BC`.
///kw_pde+PHYSICAL_GROUP+detail As in the preceeding case, if a boundary condition is created with the same name as a physical group in the mesh,
///kw_pde+PHYSICAL_GROUP+detail they will be linked automatically, so there is no need to use `PHYSCAL_GROUP` for this.
///kw_pde+PHYSICAL_GROUP+detail The `BC` keyword in `PHYSICAL_GROUP` is used to link a physical group
///kw_pde+PHYSICAL_GROUP+detail in a mesh file and a boundary condition in the feenox input file with different names.
///kw_pde+PHYSICAL_GROUP+detail Note that while there can be only one `MATERIAL` associated to a physical group,
///kw_pde+PHYSICAL_GROUP+detail there can be many `BC`s associated to a physical group.
      feenox_call(feenox_parser_string(&bc_name));

    } else {
      feenox_push_error_message("undefined keyword '%s'", token);
      return FEENOX_ERROR;
    }
  }
  
  feenox_call(feenox_define_physical_group(name, mesh_name, dimension, id));
  // feenox_physical_group_set_material()
  // feenox_physical_group_add_bc()
  
      
  feenox_free(bc_name);
  feenox_free(material_name);
  feenox_free(mesh_name);
  feenox_free(name);
  
  return FEENOX_OK;
}


int feenox_parse_material(void) {

///kw_pde+MATERIAL+usage <name>
///kw_pde+MATERIAL+detail If the name of the material matches a physical group in the mesh, it is automatically linked to that physical group.
  
  char *material_name = NULL;
  feenox_call(feenox_parser_string(&material_name));
  
  // we first create the material with a null pointer for the mesh
  // and then add it if MESH is given, in the api the mesh should be given at creation time
  material_t *material = feenox_define_material_get_ptr(material_name, NULL);
  
  char *token = NULL;
  while ((token = feenox_get_next_token(NULL)) != NULL) {

///kw_pde+MATERIAL+usage [ MESH <name> ]
///kw_pde+MATERIAL+detail If there are many meshes, the mesh this keyword refers to has to be given with `MESH`.
    if (strcasecmp(token, "MESH") == 0) {
      char *mesh_name; 
      feenox_call(feenox_parser_string(&mesh_name));
      // we can directly change the mesh here, in the API the mesh would be passed when creating the material
      if ((material->mesh = feenox_get_mesh_ptr(mesh_name)) == NULL) {
        feenox_push_error_message("undefined mesh '%s'" , mesh_name);
        return FEENOX_ERROR;
      }
      feenox_free(mesh_name);
          
///kw_pde+MATERIAL+usage [ PHYSICAL_GROUP <name_1>  [ PHYSICAL_GROUP <name_2> [ ... ] ] ]
///kw_pde+MATERIAL+detail If the material applies to more than one physical group in the mesh, they can be
///kw_pde+MATERIAL+detail added using as many `PHYSICAL_GROUP` keywords as needed. 
    } else if (strcasecmp(token, "PHYSICAL_GROUP") == 0) {
      char *physical_group_name;
      feenox_call(feenox_parser_string(&physical_group_name));  

      physical_group_t *physical_group = NULL;
      if ((physical_group = feenox_get_physical_group_ptr(physical_group_name, material->mesh)) == NULL) {
        if ((physical_group = feenox_define_physical_group_get_ptr(physical_group_name, material->mesh, material->mesh->dim_topo, 0)) == NULL) {
          return FEENOX_ERROR;
        }
      }
          
      // TODO: api
      physical_group->material = material;
      feenox_free(physical_group_name);
        
    } else {
///kw_pde+MATERIAL+usage [ <property_name_1>=<expr_1> [ <property_name_2>=<expr_2> [ ... ] ] ]
///kw_pde+MATERIAL+detail The names of the properties in principle can be arbitrary, but each problem type
///kw_pde+MATERIAL+detail needs a minimum set of properties defined with particular names.
///kw_pde+MATERIAL+detail For example, steady-state thermal problems need at least the conductivity which 
///kw_pde+MATERIAL+detail should be named\ `k`. If the problem is transient, it will also need
///kw_pde+MATERIAL+detail heat capacity\ `rhocp` or diffusivity\ `alpha`. 
///kw_pde+MATERIAL+detail Mechanical problems need Young modulus\ `E` and Poisson’s ratio\ `nu`.
///kw_pde+MATERIAL+detail Modal also needs density\ `rho`. Check the particular documentation for each problem type.
///kw_pde+MATERIAL+detail Besides these mandatory properties, any other one can be defined.
///kw_pde+MATERIAL+detail For instance, if one mandatory property dependend on the concentration of boron in the material,
///kw_pde+MATERIAL+detail a new per-material property can be added named `boron` and then the function `boron(x,y,z)` can
///kw_pde+MATERIAL+detail be used in the expression that defines the mandatory property.
      
      char *property_name = NULL;
      char *expr_string = NULL;
      
      // now token has something like E=2.1e3 or nu=1/3
      char *equal_sign = strchr(token, '=');
      if (equal_sign == NULL) {
        feenox_push_error_message("expecting an equal sign in material data '%s' such as 'E=2.1e11'", token);
        return FEENOX_ERROR;
      }
      
      *equal_sign = '\0';
      feenox_check_alloc(property_name = strdup(token));
      feenox_check_alloc(expr_string = strdup(equal_sign+1));
      feenox_call(feenox_define_property_data(property_name, material_name, expr_string));
      feenox_free(expr_string);
      feenox_free(property_name);
    }
  }
  feenox_free(material_name);
      
  return FEENOX_OK;
}



int feenox_parse_bc(void) {

///kw_pde+BC+usage <name>
///kw_pde+BC+detail If the name of the boundary condition matches a physical group in the mesh, it is automatically linked to that physical group.

  if (feenox.mesh.mesh_main == NULL) {
    feenox_push_error_message("BC before giving a MESH");
    return FEENOX_ERROR;
  }
  
  if (feenox.pde.bc_parse == NULL) {
    feenox_push_error_message("BC before setting the PROBLEM type");
    return FEENOX_ERROR;
  }
  
  char *bc_name = NULL;
  feenox_call(feenox_parser_string(&bc_name));

  // as in material above, we first create the bc with a null pointer for the mesh
  // and then add it if MESH is given, in the api the mesh should be given at creation time
  bc_t *bc = feenox_define_bc_get_ptr(bc_name, NULL);
  
  char *token = NULL;
  while ((token = feenox_get_next_token(NULL)) != NULL) {

///kw_pde+BC+usage [ MESH <name> ]
///kw_pde+BC+detail If there are many meshes, the mesh this keyword refers to has to be given with `MESH`.
    if (strcasecmp(token, "MESH") == 0) {
      char *mesh_name; 
      feenox_call(feenox_parser_string(&mesh_name));
      // we can directly change the mesh here, in the API the mesh would be passed when creating the material
      if ((bc->mesh = feenox_get_mesh_ptr(mesh_name)) == NULL) {
        feenox_push_error_message("undefined mesh '%s'" , mesh_name);
        return FEENOX_ERROR;
      }
      feenox_free(mesh_name);
    
///kw_pde+BC+usage [ PHYSICAL_GROUP <name_1>  PHYSICAL_GROUP <name_2> ... ]
///kw_pde+BC+detail If the boundary condition applies to more than one physical group in the mesh,
///kw_pde+BC+detail they can be added using as many `PHYSICAL_GROUP` keywords as needed. 
///kw_pde+BC+detail If at least one `PHYSICAL_GROUP` is given explicitly, then the `BC` name
///kw_pde+BC+detail is not used to try to implicitly link it to a physical group in the mesh.
    } else if (strcasecmp(token, "PHYSICAL_GROUP") == 0) {
      
      bc->has_explicit_groups = 1;
      
      char *physical_group_name;
      feenox_call(feenox_parser_string(&physical_group_name));  

      physical_group_t *physical_group = NULL;
      if ((physical_group = feenox_get_physical_group_ptr(physical_group_name, bc->mesh)) == NULL) {
        if ((physical_group = feenox_define_physical_group_get_ptr(physical_group_name, bc->mesh, bc->mesh->dim_topo, 0)) == NULL) {
          return FEENOX_ERROR;
        }
      }
          
      // TODO: api
      LL_APPEND(physical_group->bcs, bc);
      feenox_free(physical_group_name);
        
    } else {
///kw_pde+BC+usage [ <bc_data1> <bc_data2> ... ]
///kw_pde+BC+detail Each `<bc_data>` argument is a single string whose meaning depends on the type
///kw_pde+BC+detail of problem being solved. For instance `T=150*sin(x/pi)` prescribes the
///kw_pde+BC+detail temperature to depend on space as the provided expression in a
///kw_pde+BC+detail thermal problem and `fixed` fixes the displacements in all the directions
///kw_pde+BC+detail in a mechanical or modal problem. 
///kw_pde+BC+detail See the particular section on boundary conditions for further details.
      
      if (feenox_add_bc_data_get_ptr(bc, token) == NULL) {
        return FEENOX_ERROR;
      }
    }
  }
  feenox_free(bc_name);
  
  return FEENOX_OK;
}

int feenox_parse_problem(void) {

#ifdef HAVE_PETSC
  int (*feenox_problem_parse_particular)(const char *) = NULL;
  int (*feenox_problem_init_parser_particular)(void) = NULL;

  char *token = NULL;
  while ((token = feenox_get_next_token(NULL)) != NULL) {

///kw_pde+PROBLEM+usage [
        
///kw_pde+PROBLEM+usage mechanical
///kw_pde+PROBLEM+usage |
///kw_pde+PROBLEM+detail @    
///kw_pde+PROBLEM+detail  * `laplace` (or `poisson`) solves the Laplace (or Poisson) equation.
    if (strcasecmp(token, "laplace") == 0 || strcasecmp(token, "poisson") == 0) {
      feenox_problem_parse_particular = feenox_problem_parse_problem_laplace;
      feenox_problem_init_parser_particular = feenox_problem_init_parser_laplace;
    
///kw_pde+PROBLEM+detail  * `mechanical` (or `elastic`) solves the mechanical elastic problem.
///kw_pde+PROBLEM+detail If the mesh is two-dimensional, and not `AXISYMMETRIC`, either
///kw_pde+PROBLEM+detail `plane_stress` or `plane_strain` has to be set instead.
    } else if (strcasecmp(token, "mechanical") == 0 || strcasecmp(token, "elastic") == 0) {
      feenox_problem_parse_particular = feenox_problem_parse_problem_mechanical;
      feenox_problem_init_parser_particular = feenox_problem_init_parser_mechanical;

///kw_pde+PROBLEM+usage thermal
///kw_pde+PROBLEM+usage |
///kw_pde+PROBLEM+detail  * `thermal` (or `heat` ) solves the heat conduction problem.
    } else if (strcasecmp(token, "thermal") == 0 || strcasecmp(token, "heat") == 0) {
      feenox_problem_parse_particular = feenox_problem_parse_problem_thermal;
      feenox_problem_init_parser_particular = feenox_problem_init_parser_thermal;

///kw_pde+PROBLEM+usage modal
///kw_pde+PROBLEM+usage |
///kw_pde+PROBLEM+detail  * `modal` computes the natural mechanical frequencies and oscillation modes.        
    } else if (strcasecmp(token, "modal") == 0) {
#ifndef HAVE_SLEPC
      feenox_push_error_message("modal problems need a FeenoX binary linked against SLEPc.");
      return FEENOX_ERROR;
#endif
      feenox_problem_parse_particular = feenox_problem_parse_problem_modal;
      feenox_problem_init_parser_particular = feenox_problem_init_parser_modal;
      
///kw_pde+PROBLEM+usage neutron_diffusion
///kw_pde+PROBLEM+usage ]@
///kw_pde+PROBLEM+detail  * `neutron_diffusion` multi-group core-level neutron diffusion with a FEM formulation 
    } else if (strcasecmp(token, "neutron_diffusion") == 0) {
      feenox_problem_parse_particular = feenox_problem_parse_problem_neutron_diffusion;
      feenox_problem_init_parser_particular = feenox_problem_init_parser_neutron_diffusion;

      
///kw_pde+PROBLEM+detail @    
///kw_pde+PROBLEM+detail If you are a programmer and want to contribute with another problem type, please do so!
///kw_pde+PROBLEM+detail Check out [FeenoX repository](https:\/\/github.com/seamplex/feenox/blob/main/doc/programming.md)
///kw_pde+PROBLEM+detail for licensing information, programming guides and code of conduct.
///kw_pde+PROBLEM+detail @    

///kw_pde+PROBLEM+usage @
///kw_pde+PROBLEM+detail The number of spatial dimensions of the problem needs to be given either
///kw_pde+PROBLEM+detail as `1d`, `2d`, `3d` or with the keyword `DIMENSIONS`.
///kw_pde+PROBLEM+detail Alternatively, one can define a `MESH` with an explicit `DIMENSIONS` keyword before `PROBLEM`.
///kw_pde+PROBLEM+usage [ 1D |
    } else if (strcasecmp(token, "1d") == 0) {
      feenox.pde.dim = 1;
///kw_pde+PROBLEM+usage   2D |
    } else if (strcasecmp(token, "2d") == 0) {
      feenox.pde.dim = 2;
///kw_pde+PROBLEM+usage   3D |
    } else if (strcasecmp(token, "3d") == 0) {
      feenox.pde.dim = 3;
///kw_pde+PROBLEM+usage   DIMENSIONS <expr> ]
    } else if (strcasecmp(token, "DIMENSIONS") == 0) {
      double xi = 0;
      feenox_call(feenox_parser_expression_in_string(&xi));
      feenox.pde.dim = (unsigned int)(xi);
      if (feenox.pde.dim < 1 || feenox.pde.dim > 3)  {
        feenox_push_error_message("either one, two or three dimensions should be selected instead of '%d'", feenox.pde.dim);
        return FEENOX_ERROR;
      }

///kw_pde+PROBLEM+usage [ MESH <identifier> ] @
///kw_pde+PROBLEM+detail If there are more than one `MESH`es define, the one over which the problem is to be solved
///kw_pde+PROBLEM+detail can be defined by giving the explicit mesh name with `MESH`. By default, the first mesh to be
///kw_pde+PROBLEM+detail defined in the input file is the one over which the problem is solved.
    } else if (strcasecmp(token, "MESH") == 0) {
      char *mesh_name;

      // TODO: function parse_mesh() or something of the like
      feenox_call(feenox_parser_string(&mesh_name));
      if ((feenox.pde.mesh = feenox_get_mesh_ptr(mesh_name)) == NULL) {
        feenox_push_error_message("unknown mesh '%s'", mesh_name);
        feenox_free(mesh_name);
        return FEENOX_ERROR;
      }
      feenox_free(mesh_name);
      
      // TODO: shouldn't this go in the MESH keyword?
///kw_pde+PROBLEM+usage @[
///kw_pde+PROBLEM+usage AXISYMMETRIC { x | y }
///kw_pde+PROBLEM+usage |
///kw_pde+PROBLEM+detail If the `AXISYMMETRIC` keyword is given, the mesh is expected 
///kw_pde+PROBLEM+detail to be two-dimensional in the $x$-$y$ plane and the problem 
///kw_pde+PROBLEM+detail is assumed to be axi-symmetric around the given axis. 
    } else if (strcasecmp(token, "AXISYMMETRIC") == 0) {
      char *keywords[] = { "x", "y" };
      int values[] = {symmetry_axis_x, symmetry_axis_y, 0};
      feenox_call(feenox_parser_keywords_ints(keywords, values, (int *)&feenox.pde.symmetry_axis));

///kw_pde+PROBLEM+usage [ PROGRESS ]@
///kw_pde+PROBLEM+detail If the keyword `PROGRESS` is given, three ASCII lines will show in the terminal the
///kw_pde+PROBLEM+detail progress of the ensamble of the stiffness matrix (or matrices),
///kw_pde+PROBLEM+detail the solution of the system of equations
///kw_pde+PROBLEM+detail and the computation of gradients (stresses).
    } else if (strcasecmp(token, "PROGRESS") == 0 || strcasecmp(token, "PROGRESS_ASCII") == 0) {
      feenox.pde.progress_ascii = PETSC_TRUE;
      
///kw_pde+PROBLEM+detail If the special variable `end_time` is zero, FeenoX solves a static
///kw_pde+PROBLEM+detail  problem---although the variable `static_steps` is still honored.
///kw_pde+PROBLEM+detail If `end_time` is non-zero, FeenoX solves a transient or quasistatic problem.
///kw_pde+PROBLEM+detail This can be controlled by `TRANSIENT` or `QUASISTATIC`.
///kw_pde+PROBLEM+usage [ TRANSIENT |
    } else if (strcasecmp(token, "TRANSIENT") == 0) {
      feenox.pde.transient_type = transient_type_transient;
///kw_pde+PROBLEM+usage QUASISTATIC]@
    } else if (strcasecmp(token, "QUASISTATIC") == 0) {
      feenox.pde.transient_type = transient_type_quasistatic;


///kw_pde+PROBLEM+detail By default FeenoX tries to detect wheter the computation should be linear or non-linear.
///kw_pde+PROBLEM+detail An explicit mode can be set with either `LINEAR` on `NON_LINEAR`.
///kw_pde+PROBLEM+usage [ LINEAR
    } else if (strcasecmp(token, "LINEAR") == 0) {
      feenox.pde.math_type = math_type_linear;

///kw_pde+PROBLEM+usage | NON_LINEAR ]
    } else if (strcasecmp(token, "NON_LINEAR") == 0) {
      feenox.pde.math_type = math_type_nonlinear;


///kw_pde+PROBLEM+usage [ MODES <expr> ] @
///kw_pde+PROBLEM+detail The number of modes to be computed when solving eigenvalue problems is given by `MODES`.
///kw_pde+PROBLEM+detail The default value is problem dependent.
    } else if (strcasecmp(token, "MODES") == 0) {
      double xi = 0;
      feenox_call(feenox_parser_expression_in_string(&xi));
      feenox.pde.nev = (int)(xi);
      if (feenox.pde.nev < 1)  {
        feenox_push_error_message("a positive number of modes should be given instead of '%d'", feenox.pde.nev);
        return FEENOX_ERROR;
      }
      
///kw_pde+PROBLEM+usage [ PRECONDITIONER { gamg | mumps | lu | hypre | sor | bjacobi | cholesky | ... } ]@
///kw_pde+PROBLEM+detail The preconditioner (`PC`), linear (`KSP`), non-linear (`SNES`) and time-stepper (`TS`)
///kw_pde+PROBLEM+detail solver types be any of those available in PETSc (first option is the default):
///kw_pde+PROBLEM+detail @          
///kw_pde+PROBLEM+detail  * List of `PRECONDITIONER`s <http:\/\/petsc.org/release/docs/manualpages/PC/PCType.html>.
    } else if (strcasecmp(token, "PRECONDITIONER") == 0 || strcasecmp(token, "PC") == 0 || strcasecmp(token, "PC_TYPE") == 0) {
      feenox_call(feenox_parser_string((char **)&feenox.pde.pc_type));

///kw_pde+PROBLEM+usage [ LINEAR_SOLVER { gmres | mumps | bcgs | bicg | richardson | chebyshev | ... } ]@
///kw_pde+PROBLEM+detail  * List of `LINEAR_SOLVER`s <http:\/\/petsc.org/release/docs/manualpages/KSP/KSPType.html>.
    } else if (strcasecmp(token, "LINEAR_SOLVER") == 0 || strcasecmp(token, "KSP") == 0 || strcasecmp(token, "KSP_TYPE") == 0) {
      feenox_call(feenox_parser_string((char **)&feenox.pde.ksp_type));
          
///kw_pde+PROBLEM+usage [ NONLINEAR_SOLVER { newtonls | newtontr | nrichardson | ngmres | qn | ngs | ... } ]@
///kw_pde+PROBLEM+detail  * List of `NONLINEAR_SOLVER`s <http:\/\/petsc.org/release/docs/manualpages/SNES/SNESType.html>.
///kw_pde+PROBLEM+detail @
        } else if (strcasecmp(token, "NONLINEAR_SOLVER") == 0 || strcasecmp(token, "NON_LINEAR_SOLVER") == 0 || strcasecmp(token, "SNES") == 0 || strcasecmp(token, "SNES_TYPE") == 0) {
          feenox_call(feenox_parser_string((char **)&feenox.pde.snes_type));

///kw_pde+PROBLEM+usage [ TRANSIENT_SOLVER { bdf | beuler | arkimex | rosw | glee | ... } ]@
///kw_pde+PROBLEM+detail  * List of `TRANSIENT_SOLVER`s <http:\/\/petsc.org/release/docs/manualpages/TS/TSType.html>.
    } else if (strcasecmp(token, "TRANSIENT_SOLVER") == 0 || strcasecmp(token, "TS") == 0 || strcasecmp(token, "TS_TYPE") == 0) {
      feenox_call(feenox_parser_string((char **)&feenox.pde.ts_type));

///kw_pde+PROBLEM+usage [ TIME_ADAPTATION { basic | none | dsp | cfl | glee | ... } ]@
///kw_pde+PROBLEM+detail  * List of `TIME_ADAPTATION`s <http:\/\/petsc.org/release/docs/manualpages/TS/TSAdaptType.html>.
    } else if (strcasecmp(token, "TIME_ADAPTATION") == 0 || strcasecmp(token, "TS_ADAPT") == 0 || strcasecmp(token, "TS_ADAPT_TYPE") == 0) {
      feenox_call(feenox_parser_string((char **)&feenox.pde.ts_adapt_type));

///kw_pde+PROBLEM+usage [ EIGEN_SOLVER { krylovschur | lanczos | arnoldi | power | gd | ... } ]@
///kw_pde+PROBLEM+detail  * List of `EIGEN_SOLVER`s <https:\/\/slepc.upv.es/documentation/current/docs/manualpages/EPS/EPSType.html>.
    } else if (strcasecmp(token, "EIGEN_SOLVER") == 0 || strcasecmp(token, "EPS") == 0 || strcasecmp(token, "EPS_TYPE") == 0) {
#ifdef HAVE_SLEPC
      feenox_call(feenox_parser_string((char **)&feenox.pde.eps_type));
#else
      feenox_push_error_message("EIGEN_SOLVER needs SLEPc");
      return FEENOX_ERROR;      
#endif
      
///kw_pde+PROBLEM+usage [ SPECTRAL_TRANSFORMATION { shift | sinvert | cayley | ... } ]@
///kw_pde+PROBLEM+detail  * List of `SPECTRAL_TRANSFORMATION`s <https:\/\/slepc.upv.es/documentation/current/docs/manualpages/ST/STType.html>.
    } else if (strcasecmp(token, "SPECTRAL_TRANSFORMATION") == 0 || strcasecmp(token, "ST") == 0 || strcasecmp(token, "ST_TYPE") == 0) {
#ifdef HAVE_SLEPC
      feenox_call(feenox_parser_string((char **)&feenox.pde.st_type));
#else
      feenox_push_error_message("SPECTRAL_TRANSFORMATION needs SLEPc");
      return FEENOX_ERROR;      
#endif

///kw_pde+PROBLEM+usage [ EIGEN_FORMULATION { omega | lambda } ]@
///kw_pde+PROBLEM+detail If the `EIGEN_FORMULATION` is `omega` then $K \phi = \omega^2 M \phi$,
///kw_pde+PROBLEM+detail and $M \phi = \lambda K \phi$$ if it is `lambda`.
    } else if (strcasecmp(token, "EIGEN_FORMULATION") == 0) {
      char *keywords[] = {"omega", "lambda", ""};
      int values[] = {eigen_formulation_omega, eigen_formulation_lambda, 0};
      feenox_call(feenox_parser_keywords_ints(keywords, values, (int *)&feenox.pde.eigen_formulation));
      
    } else if (feenox_problem_parse_particular != NULL) {
      feenox_call(feenox_problem_parse_particular(token));
      
    } else {
      feenox_push_error_message("undefined keyword '%s'", token);
      return FEENOX_ERROR;
      
    }
  } 

  // if there is already a mesh, use it
  if (feenox.pde.mesh == NULL && feenox.mesh.mesh_main != NULL) {
    feenox.pde.mesh = feenox.mesh.mesh_main;
  }
  
  if (feenox.pde.dim == 0 && feenox.pde.mesh != 0) {
    feenox.pde.dim = feenox.pde.mesh->dim;
  }
  
  if (feenox_problem_init_parser_particular == NULL) {
    feenox_push_error_message("undefined PROBLEM type");
    return FEENOX_ERROR;     
  }
  
  if (feenox.pde.petscinit_called == PETSC_FALSE) {
    feenox_call(feenox_problem_init_parser_general());
    feenox_call(feenox_problem_init_parser_particular());
  } else {
    feenox_push_error_message("PROBLEM already initialized");
    return FEENOX_ERROR;    
  } 
#endif  
      
  return FEENOX_OK;
  
}

int feenox_parse_solve_problem(void) {
 
///kw_pde+SOLVE_PROBLEM+detail Whenever the instruction `SOLVE_PROBLEM` is executed,
///kw_pde+SOLVE_PROBLEM+detail FeenoX solves the PDE problem.
///kw_pde+SOLVE_PROBLEM+detail For static problems, that means solving the equations
///kw_pde+SOLVE_PROBLEM+detail and filling in the result functions.
///kw_pde+SOLVE_PROBLEM+detail For transient or quasisstatic problems, that means
///kw_pde+SOLVE_PROBLEM+detail advancing one time step.
  
  if (feenox.pde.instruction != NULL) {
    feenox_push_error_message("there was already one SOLVE_PROBLEM instruction");
    return FEENOX_ERROR;
  }
  
  feenox_check_null(feenox.pde.instruction = feenox_add_instruction_and_get_ptr(&feenox_instruction_solve_problem, NULL));
  
  return FEENOX_OK;
}

int feenox_parse_integrate(void) {
  
  mesh_integrate_t *mesh_integrate = NULL;
  feenox_check_alloc(mesh_integrate = calloc(1, sizeof(mesh_integrate_t)));
  
///kw_pde+INTEGRATE+usage { <expression> | <function> }
///kw_pde+INTEGRATE+detail Either an expression or a function of space $x$, $y$ and/or $z$ should be given.
///kw_pde+INTEGRATE+detail If the integrand is a function, do not include the arguments, i.e. instead of `f(x,y,z)` just write `f`.
///kw_pde+INTEGRATE+detail The results should be the same but efficiency will be different (faster for pure functions).
  char *token = feenox_get_next_token(NULL);
  if ((mesh_integrate->function = feenox_get_function_ptr(token)) == NULL) {
    feenox_call(feenox_expression_parse(&mesh_integrate->expr, token));
  }
  
  char *name_mesh = NULL;
  char *name_physical_group = NULL;
  char *name_result = NULL;
  
  while ((token = feenox_get_next_token(NULL)) != NULL) {
///kw_pde+INTEGRATE+usage [ OVER <physical_group> ]
///kw_pde+INTEGRATE+detail By default the integration is performed over the highest-dimensional elements of the mesh,
///kw_pde+INTEGRATE+detail i.e. over the whole volume, area or length for three, two and one-dimensional meshes, respectively.
///kw_pde+INTEGRATE+detail If the integration is to be carried out over just a physical group, it has to be given in `OVER`.

    if (strcasecmp(token, "OVER") == 0) {
      feenox_call(feenox_parser_string(&name_physical_group));
          
///kw_pde+INTEGRATE+usage [ MESH <mesh_identifier> ]
///kw_pde+INTEGRATE+detail If there are more than one mesh defined, an explicit one has to be given with `MESH`.
    } else if (strcasecmp(token, "MESH") == 0) {
      feenox_call(feenox_parser_string(&name_mesh));
      
///kw_pde+INTEGRATE+detail Either `NODES` or `CELLS` define how the integration is to be performed.
///kw_pde+INTEGRATE+usage [ NODES
///kw_pde+INTEGRATE+detail With `NODES` the integration is performed using the Gauss points
///kw_pde+INTEGRATE+detail and weights associated to each element type.
    } else if (strcasecmp(token, "NODES") == 0) {
      mesh_integrate->field_location = field_location_nodes;
            
///kw_pde+INTEGRATE+usage | CELLS ]@
///kw_pde+INTEGRATE+detail With `CELLS` the integral is computed as the sum of the product of the
///kw_pde+INTEGRATE+detail integrand at the center of each cell (element) and the cell’s volume.
///kw_pde+INTEGRATE+detail Do expect differences in the results and efficiency between these two approaches
///kw_pde+INTEGRATE+detail depending on the nature of the integrand.
    } else if (strcasecmp(token, "CELLS") == 0) {
      mesh_integrate->field_location = field_location_cells;
      feenox.mesh.need_cells = 1;
          
///kw_pde+INTEGRATE+usage RESULT <variable>@
///kw_pde+INTEGRATE+detail The scalar result of the integration is stored in the variable given by
///kw_pde+INTEGRATE+detail the mandatory keyword `RESULT`.
///kw_pde+INTEGRATE+detail If the variable does not exist, it is created.
    } else if (strcasecmp(token, "RESULT") == 0) {
      feenox_call(feenox_parser_string(&name_result));
            
    } else {
      feenox_push_error_message("unknown keyword '%s'", token);
      return FEENOX_ERROR;
    }
  }

  if (name_mesh != NULL) {
    if ((mesh_integrate->mesh = feenox_get_mesh_ptr(name_mesh)) == NULL) {
      feenox_push_error_message("undefined mesh '%s'" , name_mesh);
      return FEENOX_ERROR;
    }
    feenox_free(name_mesh);
  } else if ((mesh_integrate->mesh = feenox.mesh.mesh_main) == NULL) {
    feenox_push_error_message("no mesh defined for INTEGRATE");
    return FEENOX_ERROR;
  }
    
  if (name_physical_group != NULL) {
    if ((mesh_integrate->physical_group = feenox_get_physical_group_ptr(name_physical_group, mesh_integrate->mesh)) == NULL) {
      if ((mesh_integrate->physical_group = feenox_define_physical_group_get_ptr(name_physical_group, mesh_integrate->mesh, 0, 0)) == NULL) {
        return FEENOX_ERROR;
      }
    }
    feenox_free(name_physical_group);
  }
  
  if (name_result != NULL) {
    mesh_integrate->result = feenox_get_or_define_variable_get_ptr(name_result);
  } else {
    feenox_push_error_message("RESULT is mandatory for INTEGRATE");
    return FEENOX_ERROR;
  }

  LL_APPEND(feenox.mesh.integrates, mesh_integrate);
  feenox_call(feenox_add_instruction(feenox_instruction_mesh_integrate, mesh_integrate));
  return FEENOX_OK;
}


int feenox_parse_find_extrema(void) {
  
  mesh_find_extrema_t *mesh_find_extrema = NULL;
  feenox_check_alloc(mesh_find_extrema = calloc(1, sizeof(mesh_find_extrema_t)));
  
///kw_pde+FIND_EXTREMA+usage { <expression> | <function> }
///kw_pde+FIND_EXTREMA+detail Either an expression or a function of space $x$, $y$ and/or $z$ should be given.
  char *token = feenox_get_next_token(NULL);
  if ((mesh_find_extrema->function = feenox_get_function_ptr(token)) == NULL) {
    feenox_call(feenox_expression_parse(&mesh_find_extrema->expr, token));
  }
  
  char *name_mesh = NULL;
  char *name_physical_group = NULL;
  char *name_min = NULL;
  char *name_max = NULL;
  char *name_x_min = NULL;
  char *name_x_max = NULL;
  char *name_y_min = NULL;
  char *name_y_max = NULL;
  char *name_z_min = NULL;
  char *name_z_max = NULL;
  char *name_i_min = NULL;
  char *name_i_max = NULL;
  
  while ((token = feenox_get_next_token(NULL)) != NULL) {
///kw_pde+FIND_EXTREMA+usage [ OVER <physical_group> ]
///kw_pde+FIND_EXTREMA+detail By default the search is performed over the highest-dimensional elements of the mesh,
///kw_pde+FIND_EXTREMA+detail i.e. over the whole volume, area or length for three, two and one-dimensional meshes, respectively.
///kw_pde+FIND_EXTREMA+detail If the search is to be carried out over just a physical group, it has to be given in `OVER`.

    if (strcasecmp(token, "OVER") == 0) {
      feenox_call(feenox_parser_string(&name_physical_group));
          
///kw_pde+FIND_EXTREMA+usage [ MESH <mesh_identifier> ]
///kw_pde+FIND_EXTREMA+detail If there are more than one mesh defined, an explicit one has to be given with `MESH`.
    } else if (strcasecmp(token, "MESH") == 0) {
      feenox_call(feenox_parser_string(&name_mesh));
      
///kw_pde+FIND_EXTREMA+detail If neither `NODES`, `CELLS` or `GAUSS` is given then the search is
///kw_pde+FIND_EXTREMA+detail performed over the three of them.
///kw_pde+FIND_EXTREMA+usage [ NODES
///kw_pde+FIND_EXTREMA+detail With `NODES` only the function or expression is evalauted at the mesh nodes.
    } else if (strcasecmp(token, "NODES") == 0) {
      mesh_find_extrema->field_location = field_location_nodes;
            
///kw_pde+FIND_EXTREMA+usage | CELLS
///kw_pde+FIND_EXTREMA+detail With `CELLS` only the function or expression is evalauted at the element centers.
    } else if (strcasecmp(token, "CELLS") == 0) {
      mesh_find_extrema->field_location = field_location_cells;
//      feenox.mesh.need_cells = 1;

///kw_pde+FIND_EXTREMA+usage | GAUSS ]@
///kw_pde+FIND_EXTREMA+detail With `GAUSS` only the function or expression is evalauted at the Gauss points.
    } else if (strcasecmp(token, "GAUSS") == 0) {
      mesh_find_extrema->field_location = field_location_gauss;
      
///kw_pde+FIND_EXTREMA+usage [ MIN <variable> ]
///kw_pde+FIND_EXTREMA+detail The value of the absolute minimum (maximum) is stored in the variable indicated by `MIN` (`MAX`).
///kw_pde+FIND_EXTREMA+detail If the variable does not exist, it is created.
    } else if (strcasecmp(token, "MIN") == 0) {
      feenox_call(feenox_parser_string(&name_min));

///kw_pde+FIND_EXTREMA+usage [ MAX <variable> ]
    } else if (strcasecmp(token, "MAX") == 0) {
      feenox_call(feenox_parser_string(&name_max));

///kw_pde+FIND_EXTREMA+usage [ X_MIN <variable> ]
///kw_pde+FIND_EXTREMA+detail The value of the $x$-$y$-$z$\ coordinate of the absolute minimum (maximum)
///kw_pde+FIND_EXTREMA+detail is stored in the variable indicated by `X_MIN`-`Y_MIN`-`Z_MIN` (`X_MAX`-`Y_MAX`-`Z_MAX`).
///kw_pde+FIND_EXTREMA+detail If the variable does not exist, it is created.
    } else if (strcasecmp(token, "X_MIN") == 0) {
      feenox_call(feenox_parser_string(&name_x_min));

///kw_pde+FIND_EXTREMA+usage [ X_MAX <variable> ]
    } else if (strcasecmp(token, "X_MAX") == 0) {
      feenox_call(feenox_parser_string(&name_x_max));

///kw_pde+FIND_EXTREMA+usage [ Y_MIN <variable> ]
    } else if (strcasecmp(token, "Y_MIN") == 0) {
      feenox_call(feenox_parser_string(&name_y_min));

///kw_pde+FIND_EXTREMA+usage [ Y_MAX <variable> ]
    } else if (strcasecmp(token, "Y_MAX") == 0) {
      feenox_call(feenox_parser_string(&name_y_max));
      
///kw_pde+FIND_EXTREMA+usage [ Z_MIN <variable> ]
    } else if (strcasecmp(token, "Z_MIN") == 0) {
      feenox_call(feenox_parser_string(&name_z_min));

///kw_pde+FIND_EXTREMA+usage [ Z_MAX <variable> ]
    } else if (strcasecmp(token, "Z_MAX") == 0) {
      feenox_call(feenox_parser_string(&name_z_max));

///kw_pde+FIND_EXTREMA+usage [ I_MIN <variable> ]
///kw_pde+FIND_EXTREMA+detail The index (either node or cell) where the absolute minimum (maximum) is found
///kw_pde+FIND_EXTREMA+detail is stored in the variable indicated by `I_MIN` (`I_MAX`).
    } else if (strcasecmp(token, "I_MIN") == 0) {
      feenox_call(feenox_parser_string(&name_i_min));
///kw_pde+FIND_EXTREMA+usage [ I_MAX <variable> ]
    } else if (strcasecmp(token, "I_MAX") == 0) {
      feenox_call(feenox_parser_string(&name_i_max));
      
    } else {
      feenox_push_error_message("unknown keyword '%s'", token);
      return FEENOX_ERROR;
    }
  }

  if (name_mesh != NULL) {
    if ((mesh_find_extrema->mesh = feenox_get_mesh_ptr(name_mesh)) == NULL) {
      feenox_push_error_message("undefined mesh '%s'" , name_mesh);
      return FEENOX_ERROR;
    }
    feenox_free(name_mesh);
  } else if ((mesh_find_extrema->mesh = feenox.mesh.mesh_main) == NULL) {
    feenox_push_error_message("no mesh defined for FIND_EXTREMA");
    return FEENOX_ERROR;
  }
    
  if (name_physical_group != NULL) {
    if ((mesh_find_extrema->physical_group = feenox_get_physical_group_ptr(name_physical_group, mesh_find_extrema->mesh)) == NULL) {
      if ((mesh_find_extrema->physical_group = feenox_define_physical_group_get_ptr(name_physical_group, mesh_find_extrema->mesh, 0, 0)) == NULL) {
        return FEENOX_ERROR;
      }
    }
    feenox_free(name_physical_group);
  }
  
  if (name_min != NULL) {
    mesh_find_extrema->min = feenox_get_or_define_variable_get_ptr(name_min);
  }
  if (name_max != NULL) {
    mesh_find_extrema->max = feenox_get_or_define_variable_get_ptr(name_max);
  }
  if (name_x_min != NULL) {
    mesh_find_extrema->min = feenox_get_or_define_variable_get_ptr(name_x_min);
  }
  if (name_x_max != NULL) {
    mesh_find_extrema->max = feenox_get_or_define_variable_get_ptr(name_x_max);
  }
  if (name_y_min != NULL) {
    mesh_find_extrema->min = feenox_get_or_define_variable_get_ptr(name_y_min);
  }
  if (name_y_max != NULL) {
    mesh_find_extrema->max = feenox_get_or_define_variable_get_ptr(name_y_max);
  }
  if (name_z_min != NULL) {
    mesh_find_extrema->min = feenox_get_or_define_variable_get_ptr(name_z_min);
  }
  if (name_z_max != NULL) {
    mesh_find_extrema->max = feenox_get_or_define_variable_get_ptr(name_z_max);
  }
  if (name_i_min != NULL) {
    mesh_find_extrema->min = feenox_get_or_define_variable_get_ptr(name_i_min);
  }
  if (name_i_max != NULL) {
    mesh_find_extrema->max = feenox_get_or_define_variable_get_ptr(name_i_max);
  }
  
  LL_APPEND(feenox.mesh.find_extremas, mesh_find_extrema);
  feenox_call(feenox_add_instruction(feenox_instruction_mesh_find_extrema, mesh_find_extrema));
  
  return FEENOX_OK;
}

int feenox_parse_fit(void) {

///kw+FIT+detail The function with the data has to be point-wise defined
///kw+FIT+detail (i.e. a `FUNCTION` read from a file, with inline `DATA` or defined over a mesh).
///kw+FIT+detail The function to be fitted has to be parametrized with at least one of
///kw+FIT+detail the variables provided after the `USING` keyword.
///kw+FIT+detail For example to fit\ $f(x,y)=a x^2 + b sqrt(y)$ to a pointwise-defined
///kw+FIT+detail function\ $g(x,y)$ one gives `FIT f TO g VIA a b`.
///kw+FIT+detail Only the names of the functions have to be given, not the arguments.
///kw+FIT+detail Both functions have to have the same number of arguments.
  
  fit_t *fit = NULL;
  feenox_check_alloc(fit = calloc(1, sizeof(fit_t)));
  
///kw+FIT+usage <function_to_be_fitted> 
  // the function whose parameters we are going to fit, i.e. f(x,y) above
  char *name_function = NULL;
  feenox_call(feenox_parser_string(&name_function));
  
///kw+FIT+usage TO
  char *dummy = NULL;
  feenox_call(feenox_parser_string(&dummy));
  if (strcasecmp(dummy, "TO") != 0) {
    feenox_push_error_message("expected keyword 'TO' instead of '%s'", dummy);
  }
  feenox_free(dummy);
  
///kw+FIT+usage <function_with_data>
  char *name_data = NULL;
  feenox_call(feenox_parser_string(&name_data));

  if ((fit->function = feenox_get_function_ptr(name_function)) == NULL) {
    if (strchr(name_function, '(') != NULL) {
      feenox_push_error_message("function '%s' undefined (only the function name is to be provided, do not include its arguments)", name_function);
    } else {
      feenox_push_error_message("function '%s' undefined", name_function);
    }
    return FEENOX_ERROR;
  }
  if (fit->function->algebraic_expression.items == NULL) {
    feenox_push_error_message("function '%s' has to be algebraically defined", fit->function->name);
    return FEENOX_ERROR;
  }

  if ((fit->data = feenox_get_function_ptr(name_data)) == NULL) {
    if (strchr(name_data, '(') != NULL) {
      feenox_push_error_message("function '%s' undefined (only the function name is to be provided, do not include its arguments)", name_data);
    } else {
      feenox_push_error_message("function '%s' undefined", name_data);
    }
    return FEENOX_ERROR;
  }

  if (fit->function->n_arguments != fit->data->n_arguments) {
    feenox_push_error_message("function '%s' has %d arguments and '%s' has %d", fit->function->name, fit->function->n_arguments, fit->data->name, fit->data->n_arguments);
    return FEENOX_ERROR;
  }

  if ((fit->n_data = fit->data->data_size) == 0) {
    if (fit->data->algebraic_expression.items != NULL) {
      feenox_push_error_message("function '%s' has to be point-wise defined", fit->data->name);
      return FEENOX_ERROR;
    }  
  }

///kw+FIT+usage VIA
///kw+FIT+detail The initial guess of the solution is given by the initial value of the variables after the `VIA` keyword.
  var_ll_t *var_list = NULL; 
  feenox_call(feenox_parser_string(&dummy));
  if (strcasecmp(dummy, "VIA") != 0) {
    feenox_push_error_message("expected keyword 'VIA' instead of '%s'", dummy);
  }
  feenox_free(dummy);
  
///kw+FIT+usage <var_1> <var_2> ... <var_n>@
  char *token = NULL;
  while ((token = feenox_get_next_token(NULL)) != NULL) {

    // TODO: convergencia por gradiente, incertezas, covarianza, incrementos para gradiente automatico
///kw+FIT+usage [ GRADIENT <expr_1> <expr_2> ... <expr_n> ]@
///kw+FIT+detail Analytical expressions for the gradient of the function to be fitted with respect
///kw+FIT+detail to the parameters to be fitted can be optionally given with the `GRADIENT` keyword.
///kw+FIT+detail If none is provided, the gradient will be computed numerically using finite differences.
    if (strcasecmp(token, "GRADIENT") == 0) {
 
      if (fit->n_via == 0) {
        feenox_push_error_message("GRADIENT keyword before parameters");
        return FEENOX_ERROR;
      }
          
      feenox_check_alloc(fit->gradient = calloc(fit->n_via, sizeof(expr_t)));
      unsigned int i = 0;
      for (i = 0; i < fit->n_via; i++) {
        if ((token = feenox_get_next_token(NULL)) == NULL) {
          feenox_push_error_message("expected an expression");
          return FEENOX_ERROR;
        }

        feenox_call(feenox_expression_parse(&fit->gradient[i], token));
      }

///kw+FIT+usage [ RANGE_MIN <expr_1> <expr_2> ... <expr_j> ]@
///kw+FIT+detail A range over which the residuals are to be minimized can be given with `RANGE_MIN` and `RANGE_MAX`.
///kw+FIT+detail The expressions give the range of the arguments of the functions, not of the parameters.
///kw+FIT+detail For multidimensional fits, the range is an hypercube.
///kw+FIT+detail If no range is given, all the definition points of the function with the data are used for the fit.
    } else if (strcasecmp(token, "RANGE_MIN") == 0) {
     feenox_call(feenox_parser_expressions(&fit->range.min, fit->data->n_arguments));

///kw+FIT+usage [ RANGE_MAX <expr_1> <expr_2> ... <expr_n> ]@
    } else if (strcasecmp(token, "RANGE_MAX") == 0) {
     feenox_call(feenox_parser_expressions(&fit->range.max, fit->data->n_arguments));
          
///kw+FIT+usage [ TOL_REL <expr> ]
///kw+FIT+detail Convergence can be controlled by giving the relative and absolute tolreances with
///kw+FIT+detail `TOL_REL` (default `DEFAULT_NLIN_FIT_EPSREL`) and `TOL_ABS` (default `DEFAULT_NLIN_FIT_EPSABS`),
///kw+FIT+detail and with the maximum number of iterations `MAX_ITER` (default DEFAULT_NLIN_FIT_MAX_ITER).
/*     
    } else if (strcasecmp(token, "TOL_REL") == 0) {
      feenox_call(feenox_parser_expression(&fit->tol_rel));
*/
///kw+FIT+usage [ TOL_ABS <expr> ]
/*     
    } else if (strcasecmp(token, "TOL_ABS") == 0) {
      feenox_call(feenox_parser_expression(&fit->tol_abs));
*/
///kw+FIT+usage [ MAX_ITER <expr> ]@
    } else if (strcasecmp(token, "MAX_ITER") == 0) {
      double xi = 0;
      feenox_call(feenox_parser_expression_in_string(&xi));
      if ((fit->max_iter = (int)xi) < 0) {
        feenox_push_error_message("expected a positive integer for MAX_ITER");
        return FEENOX_ERROR;
      }

///kw+FIT+usage [ VERBOSE ]
    } else if (strcasecmp(token, "VERBOSE") == 0) {
///kw+FIT+detail If the optional keyword `VERBOSE` is given, some data of the intermediate steps is written in the standard output.
      fit->verbose = 1;

    } else {

      var_ll_t *var_item =  NULL;
      feenox_check_alloc(var_item = calloc(1, sizeof(var_ll_t)));
      if ((var_item->var = feenox_get_variable_ptr(token)) == NULL) {
        feenox_push_error_message("unknown variable '%s'", token);
        return FEENOX_ERROR;
      } 
          
      LL_APPEND(var_list, var_item);
      fit->n_via++;

    }
  }

  if (fit->n_via == 0) {
    feenox_push_error_message("no fit parameters given");
    return FEENOX_ERROR;
  }

  feenox_check_alloc(fit->via = calloc(fit->n_via,  sizeof(var_t *)));
  feenox_check_alloc(fit->sigma = calloc(fit->n_via, sizeof(var_t *)));
  var_ll_t *var_item = var_list;
  unsigned int i = 0;
  for (i = 0; i < fit->n_via; i++) {
    fit->via[i] = var_item->var;

    char *name = NULL;
    feenox_check_minusone(asprintf(&name, "sigma_%s", var_item->var->name));
    feenox_check_alloc(fit->sigma[i] = feenox_define_variable_get_ptr(name));
        
    if ((var_item = var_item->next) == NULL && i != (fit->n_via-1)) {
      feenox_push_error_message("internal mismatch in number of fit parameter %d", i);
      return FEENOX_ERROR;
    }
  }

  // TODO: choose
  if (fit->max_iter == 0) {
    fit->max_iter = DEFAULT_FIT_MAX_ITER;
  }
  
  // TODO: cleanup var_list
  // TODO: add instruction

  LL_APPEND(feenox.fits, fit);
  feenox_call(feenox_add_instruction(feenox_instruction_fit, fit));
  
  return FEENOX_OK;
}

int feenox_parse_dump(void) {

  dump_t *dump = NULL;
  feenox_check_alloc(dump = calloc(1, sizeof(dump_t)));
  
  char *token = NULL;
  while ((token = feenox_get_next_token(NULL)) != NULL) {
///kw_pde+DUMP+usage [ FORMAT { binary | ascii | octave } ]
    if (strcasecmp(token, "FORMAT") == 0) {
      char *keywords[] = {           "default",           "binary",           "ascii",           "octave", ""};
      int values[] =     {dump_format_default, dump_format_binary, dump_format_ascii, dump_format_octave, 0};
      feenox_call(feenox_parser_keywords_ints(keywords, values, (int *)(&dump->format)));
      
///kw_pde+DUMP+usage [ K |
    } if (strcasecmp(token, "K") == 0) {
      dump->K = 1;
///kw_pde+DUMP+usage   K_bc |
    } else if (strcasecmp(token, "K_bc") == 0) {
      dump->K_bc = 1;
///kw_pde+DUMP+usage   b |
    } else if (strcasecmp(token, "b") == 0) {
      dump->b = 1;
///kw_pde+DUMP+usage   b_bc |
    } else if (strcasecmp(token, "b_bc") == 0) {
      dump->b_bc = 1;
///kw_pde+DUMP+usage   M |
    } else if (strcasecmp(token, "M") == 0) {
      dump->M = 1;
///kw_pde+DUMP+usage   M_bc |
    } else if (strcasecmp(token, "M_bc") == 0) {
      dump->M_bc = 1;
    }

  }
  
  LL_APPEND(feenox.dumps, dump);
  feenox_call(feenox_add_instruction(feenox_instruction_dump, dump));
  
  
  
  return FEENOX_OK;
}
