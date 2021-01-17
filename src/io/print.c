/*------------ -------------- -------- --- ----- ---   --       -            -
 *  FeenoX text output routines
 *
 *  Copyright (C) 2009--2021 jeremy theler
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
extern feenox_t feenox;

#include <stdio.h>
#include <string.h>


int feenox_instruction_print(void *arg) {
  print_t *print = (print_t *)arg;
  print_token_t *print_token;
  char *current_format = (print->tokens != NULL) ? print->tokens->format : NULL;

  char default_print_format[] = DEFAULT_PRINT_FORMAT;
  int have_to_print = 1;
  int have_to_header = 0;
  int i, j;
  int flag = 1;  // flag to know if we already printed something or not (for matrices)
  
  if ((int)feenox_special_var_value(in_static) != 0) {
    if (print->skip_static_step.factors != NULL && print->last_static_step != 0 &&
        ((int)(feenox_special_var_value(step_static)) == 1 || ((int)(feenox_special_var_value(step_static)) - print->last_static_step) < feenox_evaluate_expression(&print->skip_static_step))) {
      have_to_print = 0;
    }
  } else {
    if (print->skip_step.factors != NULL && 
        ((int)(feenox_special_var_value(step_transient)) - print->last_step) < feenox_evaluate_expression(&print->skip_step)) {
      have_to_print = 0;
    }
    if (print->skip_time.factors != NULL && 
        (feenox_special_var_value(t) == 0 || (feenox_special_var_value(t) - print->last_time) < feenox_evaluate_expression(&print->skip_time))) {
      have_to_print = 0;
    }
  }
  
  // in parallel runs only print from first processor
  have_to_print &= (feenox.rank == 0);

  if (have_to_print == 0) {
    return FEENOX_OK;
  }

  if (print->file->pointer == NULL) {
    feenox_call(feenox_instruction_file_open(print->file));
  }

  if (print->tokens == NULL || print->tokens->format == NULL) {
    current_format = default_print_format;
  }

  print->last_step = (int)(feenox_special_var_value(step_transient));
  print->last_time = feenox_special_var_value(t);
  print->last_static_step = (int)(feenox_special_var_value(step_static));

  if (print->header && (feenox.mode != mode_parametric || (int)feenox_special_var_value(step_outer) == 1)) {
    if (print->skip_header_step.factors != NULL && print->header_already_printed == 0) {
      have_to_header = 1;
    } else if (((int)(feenox_special_var_value(step_transient)) - print->last_header_step) < feenox_evaluate_expression(&print->skip_header_step)) {
      have_to_header = 1;
    }
  }
  
  // write the header if we have to
  if (have_to_header) {
    fprintf(print->file->pointer, "# ");
    LL_FOREACH(print->tokens, print_token) {
      if (print_token->text != NULL) {
        fprintf(print->file->pointer, "%s", print_token->text);
        if (print_token->next != NULL) {
          fprintf(print->file->pointer, "%s", print->separator);
        }
      }
    }
    fprintf(print->file->pointer, "\n");
    print->header_already_printed = 1;
    print->last_header_step = (int)(feenox_special_var_value(step_transient));
  }


  // now print the tokens!
  LL_FOREACH(print->tokens, print_token) {

    if (print_token->expression.factors != NULL) {
      fprintf(print->file->pointer, current_format, feenox_evaluate_expression(&print_token->expression));
      // if there's not the last token or there's no newline print the separator
      if ((print_token->next != NULL) || (print->nonewline)) {
        fprintf(print->file->pointer, "%s", print->separator);
      }

      flag = 0;
      
    } else if (print_token->vector != NULL) {
      if (!print_token->vector->initialized) {
        feenox_call(feenox_vector_init(print_token->vector));
      }
      for (i = 0; i < print_token->vector->size; i++) {
        fprintf(print->file->pointer, current_format, gsl_vector_get(feenox_value_ptr(print_token->vector), i));
        if (i != print_token->vector->size-1) {
          fprintf(print->file->pointer, "%s", print->separator);
        }
      }
      if ((print_token->next != NULL) || (print->nonewline)) {
        fprintf(print->file->pointer, "%s", print->separator);
      }

      flag = 0;
      
    } else if (print_token->matrix != NULL) {
      if (!print_token->matrix->initialized) {
        feenox_call(feenox_matrix_init(print_token->matrix));
      }
      for (i = 0; i < print_token->matrix->rows; i++) {
        for (j = 0; j < print_token->matrix->cols; j++) {
          fprintf(print->file->pointer, current_format, gsl_matrix_get(feenox_value_ptr(print_token->matrix), i, j));
          if (j != print_token->matrix->cols-1) {
            fprintf(print->file->pointer, "%s", print->separator);
          }
        }
        if (flag == 1 && i != print_token->matrix->rows-1) {
          fprintf(print->file->pointer, "\n");
        } else {
          fprintf(print->file->pointer, "%s", print->separator);
        }
      }

      flag = 0;
      
    } else if (print_token->text != NULL) {
      fprintf(print->file->pointer, "%s", print_token->text);
      if (print_token->next != NULL) {
        fprintf(print->file->pointer, "%s", print->separator);
      }
      
      flag = 0;
      
    } else if (print_token->format != NULL) {
      current_format = print_token->format;
    }
    
  }

  if (!print->nonewline) {
    fprintf(print->file->pointer, "\n");
  }

  // siempre flusheamos
  fflush(print->file->pointer);
    

  return FEENOX_OK;

}

/*
int feenox_instruction_print_function(void *arg) {
  print_function_t *print_function = (print_function_t *)arg;
  print_token_t *print_token;
  
  int j, k;
  int flag = 0;
  double *x, *x_min, *x_max, *x_step;

  // in parallel runs only print from first processor
  if (feenox.rank != 0) {
    return WASORA_RUNTIME_OK;
  }
  
  if (print_function->file->pointer == NULL) {
    feenox_call(feenox_instruction_open_file(print_function->file));
  }
  
  if (!print_function->first_function->initialized) {
    feenox_call(feenox_function_init(print_function->first_function));
  }
  
  // vemos si hay que escribir un header
  if (print_function->header && (feenox.parametric_mode == 0 || (int)feenox_var(feenox.special_vars.step_outer) == 1)) {
    fprintf(print_function->file->pointer, "# ");
    
    // primero los argumentos de la primera funcion
    for (k = 0; k < print_function->first_function->n_arguments; k++) {
      fprintf(print_function->file->pointer, "%s", print_function->first_function->var_argument[k]->name);
      fprintf(print_function->file->pointer, "%s", print_function->separator);
    }
    
    LL_FOREACH(print_function->tokens, print_token) {
      if (print_token->text != NULL) {
        fprintf(print_function->file->pointer, "%s", print_token->text);
        if (print_token->next != NULL) {
          fprintf(print_function->file->pointer, "%s", print_function->separator);
        }
      }
    }
    fprintf(print_function->file->pointer, "\n");
  }
  

  if (print_function->range.min != NULL && print_function->range.max != NULL && 
      (print_function->range.step != NULL || print_function->range.nsteps != NULL)) {
    // nos dieron los puntos donde se quiere dibujar las funciones

    x = calloc(print_function->first_function->n_arguments, sizeof(double));
    x_min = calloc(print_function->first_function->n_arguments, sizeof(double));
    x_max = calloc(print_function->first_function->n_arguments, sizeof(double));
    x_step = calloc(print_function->first_function->n_arguments, sizeof(double));

    for (j = 0; j < print_function->first_function->n_arguments; j++) {
      x_min[j] = feenox_evaluate_expression(&print_function->range.min[j]);
      if (print_function->range.nsteps != NULL &&
          feenox_evaluate_expression(&print_function->range.nsteps[j]) != 1) {
        x_max[j] = feenox_evaluate_expression(&print_function->range.max[j]);
        x_step[j] = (print_function->range.step != NULL) ? feenox_evaluate_expression(&print_function->range.step[j]) :
                                     (x_max[j]-x_min[j])/feenox_evaluate_expression(&print_function->range.nsteps[j]);
      } else if (print_function->range.step != NULL) {
        x_max[j] = feenox_evaluate_expression(&print_function->range.max[j]);
        x_step[j] = feenox_evaluate_expression(&print_function->range.step[j]);
       
      } else {
        x_max[j] = x_min[j] + 0.1;
        x_step[j] = 1;
      }

      x[j] = x_min[j];
    }

    // hasta que el primer argumento llegue al maximo y se pase un
    // poquito para evitar que por el redondeo se nos escape el ultimo punto
    while (x[0] < x_max[0] * (1 + feenox_value(feenox_special_var(zero)))) {

      // imprimimos los argumentos
      for (j = 0; j < print_function->first_function->n_arguments; j++) {
        fprintf(print_function->file->pointer, print_function->format, x[j]);
        fprintf(print_function->file->pointer, "%s", print_function->separator);
      }

      LL_FOREACH (print_function->tokens, print_token) {
        // imprimimos lo que nos pidieron
        if (print_token->function != NULL) {
          fprintf(print_function->file->pointer, print_function->format, feenox_evaluate_function(print_token->function, x));
          
        } else if (print_token->expression.n_tokens != 0) {
          feenox_set_function_args(print_function->first_function, x);
          fprintf(print_function->file->pointer, print_function->format, feenox_evaluate_expression(&print_token->expression));
          
        }
        
        if (print_token->next != NULL) {
          fprintf(print_function->file->pointer, "%s", print_function->separator);
        } else {
          fprintf(print_function->file->pointer, "\n");
          // siempre flusheamos
          fflush(print_function->file->pointer);
        }
      }

      // incrementamos el ultimo argumento
      x[print_function->first_function->n_arguments-1] += x_step[print_function->first_function->n_arguments-1];
      // y vamos mirando si hay que reiniciarlos
      for (j = print_function->first_function->n_arguments-2; j >= 0; j--) {
        if (x[j+1] > (x_max[j+1] + 0.1*x_step[j+1])) {
          x[j+1] = x_min[j+1];
          x[j] += x_step[j];

          // si estamos en 2d y reiniciamos el primer argumento imprimimos una linea
          // en blanco para que plotear con gnuplot with lines salga lindo
          if (print_function->first_function->n_arguments == 2 && j == 0) {
            fprintf(print_function->file->pointer, "\n");
          }

        }
      }
    }

    free(x);
    free(x_min);
    free(x_max);
    free(x_step);

  } else if (print_function->first_function != NULL && print_function->first_function->data_size > 0) {


    // imprimimos en los puntos de definicion de la primera
    x = calloc(print_function->first_function->n_arguments, sizeof(double));

    for (j = 0; j < print_function->first_function->data_size; j++) {

      if (print_function->physical_entity != NULL) {
        element_list_item_t *element_list_item;
        flag = 0;
        LL_FOREACH(feenox_mesh.main_mesh->node[j].associated_elements, element_list_item) {
          if (element_list_item->element->physical_entity == print_function->physical_entity) {
            flag = 1;
          }
        }
      }
          
      if (print_function->physical_entity == NULL || flag) {
      
        // los argumentos de la primera funcion
        for (k = 0; k < print_function->first_function->n_arguments; k++) {
          // nos acordamos los argumentos para las otras funciones que vienen despues
          x[k] = print_function->first_function->data_argument[k][j];
          fprintf(print_function->file->pointer, print_function->format, x[k]);
          fprintf(print_function->file->pointer, "%s", print_function->separator);
        }

        // las cosas que nos pidieron
        LL_FOREACH(print_function->tokens, print_token) {

          // imprimimos lo que nos pidieron
          if (print_token->function != NULL) {
            // momento! hay que incializar, puede darse el caso de que no se haya inicializado todavia
            if (!print_token->function->initialized)  {
              feenox_call(feenox_function_init(print_token->function));
            }
            
            if (print_token->function == print_function->first_function || print_token->function->data_size == print_function->first_function->data_size) {
              // la primera funcion tiene los puntos posta asi que no hay que interpolar
              if (print_token->function->data_value != NULL) {
                fprintf(print_function->file->pointer, print_function->format, print_token->function->data_value[j]);
              } else {
                fprintf(print_function->file->pointer, print_function->format, 0.0);
              }
            } else {
              fprintf(print_function->file->pointer, print_function->format, feenox_evaluate_function(print_token->function, x));
            }

          } else if (print_token->expression.n_tokens != 0) {
            feenox_set_function_args(print_function->first_function, x);
            fprintf(print_function->file->pointer, print_function->format, feenox_evaluate_expression(&print_token->expression));

          }

          if (print_token->next != NULL) {
            fprintf(print_function->file->pointer, "%s", print_function->separator);
          } else {
            fprintf(print_function->file->pointer, "\n");
            // siempre flusheamos
            fflush(print_function->file->pointer);
          }
          
          // si estamos en 2d y cambiamos los dos primeros argumentos imprimimos una linea
          // en blanco para que plotear con gnuplot with lines salga lindo        
          if (print_token->next == NULL &&
              print_function->first_function->n_arguments == 2 &&
              print_function->first_function->rectangular_mesh &&
              j != print_function->first_function->data_size &&
              gsl_fcmp(print_function->first_function->data_argument[0][j], print_function->first_function->data_argument[0][j+1], print_function->first_function->multidim_threshold) != 0 &&
              gsl_fcmp(print_function->first_function->data_argument[1][j], print_function->first_function->data_argument[1][j+1], print_function->first_function->multidim_threshold) != 0) {
            fprintf(print_function->file->pointer, "\n");
          }
          
        }
      }
//      fprintf(print_function->file->pointer, "\n");

    }

    free(x);

  } else {
    feenox_push_error_message("mandatory range needed for PRINT_FUNCTION instruction (function %s is not point-wise defined)", print_function->first_function->name);
    return WASORA_RUNTIME_ERROR;
      
  }

  fflush(print_function->file->pointer);
  
  return 0;

}



int feenox_instruction_print_vector(void *arg) {
  print_vector_t *print_vector = (print_vector_t *)arg;
  print_token_t *print_token;

  int j, k;
  int n_elems_per_line;
  
  // in parallel runs only print from first processor
  if (feenox.rank != 0) {
    return WASORA_RUNTIME_OK;
  }

  if (print_vector->file->pointer == NULL) {
    feenox_call(feenox_instruction_open_file(print_vector->file));
  }

  if (print_vector->first_vector == NULL) {
    return WASORA_RUNTIME_OK;
  }
  
  if (!print_vector->first_vector->initialized) {
    feenox_call(feenox_vector_init(print_vector->first_vector));
  }
  
  if ((n_elems_per_line = (int)feenox_evaluate_expression(&print_vector->elems_per_line)) || print_vector->horizontal) {

    LL_FOREACH(print_vector->tokens, print_token) {
      
      if (!print_token->vector->initialized) {
        feenox_call(feenox_vector_init(print_token->vector));
      }
      
      if (print_token->vector->size != print_vector->first_vector->size) {
        feenox_push_error_message("vectors %s and %s do not have the same size", print_token->vector->name, print_vector->first_vector->name);
        return WASORA_RUNTIME_ERROR;
      }
      
      j = 0;
      for (k = 0; k < print_vector->first_vector->size; k++) {
        
        if (print_token->vector != NULL) {
          fprintf(print_vector->file->pointer, print_vector->format, gsl_vector_get(feenox_value_ptr(print_token->vector), k));

        } else if (print_token->expression.n_tokens != 0) {
          feenox_var(feenox.special_vars.i) = k+1;
          fprintf(print_vector->file->pointer, print_vector->format, feenox_evaluate_expression(&print_token->expression));
          
        }

        if (++j == n_elems_per_line) {
          j = 0;
          fprintf(print_vector->file->pointer, "\n");
        } else {
          fprintf(print_vector->file->pointer, "%s", print_vector->separator);
        }
      }
      fprintf(print_vector->file->pointer, "\n");
    }

  } else {

    for (k = 0; k < print_vector->first_vector->size; k++) {

      LL_FOREACH(print_vector->tokens, print_token) {
        
        if (print_token->vector != NULL) {
          if (print_token->vector->initialized == 0) {
            feenox_call(feenox_vector_init(print_token->vector));
          }
          if (print_token->vector->size != print_vector->first_vector->size) {
            feenox_push_error_message("vectors %s and %s do not have the same size", print_token->vector->name, print_vector->first_vector->name);
            return WASORA_RUNTIME_ERROR;
          }
          
          fprintf(print_vector->file->pointer, print_vector->format, gsl_vector_get(feenox_value_ptr(print_token->vector), k));

        } else if (print_token->expression.n_tokens != 0) {
          feenox_var(feenox.special_vars.i) = k+1;
          fprintf(print_vector->file->pointer, print_vector->format, feenox_evaluate_expression(&print_token->expression));
          
        }

        if (print_token->next != NULL) {
          fprintf(print_vector->file->pointer, "%s", print_vector->separator);
        } else {
          fprintf(print_vector->file->pointer, "\n");
        }
      }
    }
  }

  fflush(print_vector->file->pointer);
  
  return 0;

}

int feenox_instruction_print_matrix(void *arg) {
  print_matrix_t *print_matrix = (print_matrix_t *)arg;


  int j, k;

  int have_to_print = 1;

  if (have_to_print) {

    if (print_matrix->file->pointer == NULL) {
      feenox_instruction_open_file(print_matrix->file);
    }

    for (k = 0; k < print_matrix->matrix->rows; k++) {
      for (j = 0; j < print_matrix->matrix->cols; j++) {

//        if (print_matrix->matrix->is_crisp == 0) {
//          fprintf(print_matrix->file->pointer, print_matrix->format, print_matrix->matrix->element[k][j]->current[0]);
//        } else {
          fprintf(print_matrix->file->pointer, print_matrix->format, gsl_matrix_get(print_matrix->matrix->current, k, j));
//        }
        fprintf(print_matrix->file->pointer, "%s", print_matrix->separator);
      }
      fprintf(print_matrix->file->pointer, "\n");
    }
  }

  fflush(print_matrix->file->pointer);
  
  return 0;

}
*/
