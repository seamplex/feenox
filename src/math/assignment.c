/*------------ -------------- -------- --- ----- ---   --       -            -
 *  FeenoX assignment of variables/vectors/matrices
 *
 *  Copyright (C) 2009--2015 jeremy theler
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
extern feenox_t feenox;


int feenox_add_assignment(char *left_hand, char *right_hand) {
  
  char *dummy;
  char *dummy_time_range;
  char *dummy_index_range;
  char *dummy_par;
  char *dummy_init;
  char *dummy_0;
  
  assignment_t *assignment = calloc(1, sizeof(assignment));
  
  // let us start assuming the assignment is "plain"
  assignment->plain = 1;
    
  // first from right to left
  // range of indices
  if ((dummy_index_range = strchr(left_hand, '<')) != NULL) {
    *dummy_index_range = '\0';
  }
  
  // subindexes between parenthesis if it is a vector or matrix
  if ((dummy_par = strchr(left_hand, '(')) != NULL) {
    *dummy_par = '\0';
  }
  
  // initial values, either _init o _0
  if ((dummy_init = feenox_ends_in_init(left_hand)) != NULL) {
    *dummy_init = '\0';
    assignment->initial_static = 1;
  }
  
  if ((dummy_0 = feenox_ends_in_zero(left_hand)) != NULL) {
    *dummy_0 = '\0';
    assignment->initial_transient = 1;
  }
  
  // TODO: strip blanks?
  if ((assignment->matrix = feenox_get_matrix_ptr(left_hand)) == NULL &&
      (assignment->vector = feenox_get_vector_ptr(left_hand)) == NULL) {
    
    assignment->variable = feenox_get_or_define_variable_ptr(left_hand);
    assignment->plain = 0;
    assignment->scalar = 1;
  
  }
    
    // we are looking for a variable
    // TODO: honor IMPLICIT
 /*   
    if ((assignment->variable = feenox_get_variable_ptr(left_hand)) == NULL) {
      if (feenox.implicit_none) {
        feenox_push_error_message("undefined symbol '%s' and implicit definition is disabled", left_hand);
        return FEENOX_ERROR;
      } else {
        if ((assignment->variable = feenox_define_variable(left_hand)) == NULL) {
          return FEENOX_ERROR;
        } else {
          // las variables siempre son escalares
          assignment->plain = 0;
          assignment->scalar = 1;
        }
      }
    } else {
      // las variables siempre son escalares
      assignment->plain = 0;
      assignment->scalar = 1;
    }
  }
*/  
  // now from lef to right
  // re-build the string
  if (dummy_0 != NULL) {
    *dummy_0 = '_';
  }
  if (dummy_init != NULL) {
    *dummy_init = '_';
  }
  
  // if there are parenthesis, then there are subindexes
  if (dummy_par != NULL) {
    if (assignment->matrix == NULL && assignment->vector == NULL) {
      if (feenox_get_function_ptr(left_hand) != NULL) {
        feenox_push_error_message("functions are defined using ':=' instead of '='", left_hand);  
      } else {
        feenox_push_error_message("'%s' is neither a vector nor a matrix", left_hand);
      }
      return FEENOX_ERROR;
    }
    *dummy_par = '(';
    
    // TODO: check for dependence on i and j by looking at the list of items
    if (assignment->matrix != NULL) {
      // si aparecen o la letra "i" o la letra "j" entonces no es escalar
      assignment->scalar = (strchr(dummy_par, 'i') == NULL) && (strchr(dummy_par, 'j') == NULL);
      // si aparece la letra "i" y no la letra "j" entonces se varia solo las filas
      assignment->expression_only_of_i = (strchr(dummy_par, 'j') == NULL) && (strchr(dummy_par, 'i') != NULL);
      // si no aparece la letra "i" pero si la letra "j" entonces se varia solo las columnas
      assignment->expression_only_of_j = (strchr(dummy_par, 'i') == NULL) && (strchr(dummy_par, 'j') != NULL);
      // si no es es "i,j" entonces no es plain
      if (strcmp("(i,j)", dummy_par) != 0) {
        assignment->plain = 0;
      }
      feenox_call(feenox_parse_range(dummy_par, '(', ',', ')', &assignment->row, &assignment->col));
    } else if (assignment->vector != NULL) {
      if ((dummy = strrchr(dummy_par, ')')) == NULL) {
        feenox_push_error_message("unmatched parenthesis for '%s'", left_hand);
        return FEENOX_ERROR;
      }
      *dummy = '\0';
      
      // si aparece la letra "i" entonces no es escalar
      assignment->scalar = strchr(dummy_par+1, 'i') == NULL;
      
      // si no es "i" entonces no es plain
      if (strcmp("i", dummy_par+1) != 0) {
        assignment->plain = 0;
      }
      feenox_call(feenox_expression_parse(&assignment->row, dummy_par+1));
      *dummy = ')';
    }
  }
  
  // si hay rango de indices
  if (dummy_index_range != NULL) {
    if (assignment->matrix == NULL && assignment->vector == NULL) {
      if (feenox_get_function_ptr(left_hand) != NULL) {
        feenox_push_error_message("functions are defined using ':=' instead of '='", left_hand);  
      } else {
        feenox_push_error_message("'%s' is neither a vector nor a matrix", left_hand);
      }
      return FEENOX_ERROR;
    }
    *dummy_index_range = '<';
    
    if (assignment->matrix != NULL) {
      feenox_call(feenox_parse_range(dummy_index_range, '<', ':', ';', &assignment->i_min, &assignment->i_max));
      feenox_call(feenox_parse_range(strchr(dummy_index_range, ';'), ';', ':', '>', &assignment->j_min, &assignment->j_max));
    } else if (assignment->vector != NULL) {
      feenox_call(feenox_parse_range(dummy_index_range, '<', ':', '>', &assignment->i_min, &assignment->i_max));
    }
  }
  
  // the right-hand side is easy!
  feenox_call(feenox_expression_parse(&assignment->rhs, right_hand));
  
  return FEENOX_OK;
}

/*
// un assignment puede ser tan sencillo como una variable solita o algo
// mas complejo como la asignacion de ciertos elementos de una matrix/vector
int feenox_instruction_assignment(void *arg) {
  assignment_t *assignment = (assignment_t *)arg;

  int row, col;
  int i, j;
  int i_min, i_max, j_min, j_max;
  
  feenox_call(feenox_get_assignment_array_boundaries(assignment, &i_min, &i_max, &j_min, &j_max));
 
  for (i = i_min; i < i_max; i++) {
    for (j = j_min; j < j_max; j++) {
      feenox_call(feenox_get_assignment_rowcol(assignment, i, j, &row, &col));
      feenox_call(feenox_assign_scalar(assignment, row, col));
    }
  }
  
  return WASORA_RUNTIME_OK;

}

int feenox_get_assignment_array_boundaries(assignment_t *assignment, int *i_min, int *i_max, int *j_min, int *j_max) {
 
  *i_min = 0;
  *i_max = 1;
  *j_min = 0;
  *j_max = 1;
  
  if (!assignment->scalar) {
    if (assignment->i_min.n_tokens != 0) {
      // esto es C amigos!
      *i_min = (int)(round(feenox_evaluate_expression(&assignment->i_min)))-1;
      *i_max = (int)(round(feenox_evaluate_expression(&assignment->i_max)));
    } else {
      
      if (assignment->vector != NULL) {
        
        if (!assignment->vector->initialized) {
          feenox_call(feenox_vector_init(assignment->vector));
        }
        
        *i_max = assignment->vector->size;
        
      } else if (assignment->matrix != NULL) {
        
        if (!assignment->matrix->initialized) {
          feenox_call(feenox_matrix_init(assignment->matrix));
        }
        
        if (assignment->expression_only_of_j == 0) {
          *i_max = assignment->matrix->rows;
        } else {
          *i_min = (int)(round(feenox_evaluate_expression(&assignment->row)))-1;
          *i_max = *i_min + 1;
        }
      }
    }
    
    if (assignment->j_min.n_tokens != 0) {
      // esto es C amigos!
      *j_min = (int)(round(feenox_evaluate_expression(&assignment->j_min)))-1;
      *j_max = (int)(round(feenox_evaluate_expression(&assignment->j_max)));
      
    } else {
      if (assignment->matrix != NULL) {
        
        if (!assignment->matrix->initialized) {
          feenox_call(feenox_matrix_init(assignment->matrix));
        }
        
        if (assignment->expression_only_of_i == 0) {
          *j_max = assignment->matrix->cols;
        } else {
          *j_min = (int)(round(feenox_evaluate_expression(&assignment->col)))-1;
          *j_max = *j_min + 1;
        }
      }
    }
  }
  
  return WASORA_RUNTIME_OK;
  
}

int feenox_get_assignment_rowcol(assignment_t *assignment, int i, int j, int *row, int *col) {
  feenox_var(feenox_special_var(i)) = (double)(i+1);
  feenox_var(feenox_special_var(j)) = (double)(j+1);

  if (assignment->plain) {
    *row = i;
  } else if (assignment->row.n_tokens != 0) {
    *row = (int)(feenox_evaluate_expression(&assignment->row))-1;
  } else {
    *row = 0;
  }

  if (assignment->plain) {
    *col = j;
  } else if (assignment->col.n_tokens != 0) {
    *col = (int)(feenox_evaluate_expression(&assignment->col))-1;
  } else {
    *col = 0;
  }

  return WASORA_RUNTIME_OK;  
}

// verifica si tenemos que poner los valores iniciales de una variable
void feenox_check_initial_variable(var_t *var) {
  
  if (var != NULL) {
    if (feenox_var(feenox_special_var(in_static))) {
      *var->initial_transient = feenox_value(var);
      if ((int)(feenox_var(feenox_special_var(step_static))) == 1) {
        *var->initial_static = feenox_value(var);
      }
    }
  }
  return;  
}

// verifica si tenemos que poner los valores iniciales de una variable
void feenox_check_initial_vector(vector_t *vector) {
  
  if (vector != NULL) {
    if (!vector->initialized) {
      feenox_vector_init(vector);
    }
    if (feenox_var(feenox_special_var(in_static))) {
      gsl_vector_memcpy(vector->initial_transient, feenox_value_ptr(vector));
      if ((int)(feenox_var(feenox_special_var(step_static))) == 1) {
        gsl_vector_memcpy(vector->initial_static, feenox_value_ptr(vector));
      }
    }
  }
  return;  
}

// verifica si tenemos que poner los valores iniciales de una variable
void feenox_check_initial_matrix(matrix_t *matrix) {
  
  if (matrix != NULL) {
    if (feenox_var(feenox_special_var(in_static))) {
      gsl_matrix_memcpy(matrix->initial_transient, feenox_value_ptr(matrix));
      if ((int)(feenox_var(feenox_special_var(step_static))) == 1) {
        gsl_matrix_memcpy(matrix->initial_static, feenox_value_ptr(matrix));
      }
    }
  }
  return;  
}


// ejecuta una asignacion escalar 
int feenox_assign_scalar(assignment_t *assignment, int row, int col) {

  int i;
  double t_min, t_max;
  int constant;
  double *current;
  double *initial_static;
  double *initial_transient;

  assignment_t *other;
  
  if (assignment->variable != NULL) {
    // si es un assignment sobre una variable parametrica, a comerla 
    for (i = 0; i < feenox.parametric.dimensions; i++) {
      if (assignment->variable == feenox.parametric.variable[i]) {
        return WASORA_RUNTIME_OK;
      }
    }
    // idem sobre un parametro a optimizar, aunque dejamos que en la primera vuelta entre
    if ((int)(feenox_var(feenox_special_var(in_outer_initial))) == 0) {
      for (i = 0; i < feenox.fit.p; i++) {
        if (assignment->variable == feenox.fit.param[i]) {
          return WASORA_RUNTIME_OK;
        }
      }
      for (i = 0; i < feenox.min.n; i++) {
        if (assignment->variable == feenox.min.x[i]) {
          return WASORA_RUNTIME_OK;
        }
      }
    }
    
    constant = assignment->variable->constant;
    current = feenox_value_ptr(assignment->variable);
    initial_static = assignment->variable->initial_static;
    initial_transient = assignment->variable->initial_transient;
    
  // TODO: estos punteros se podrian pedir solo si son necesarios, es decir
  // si estamos en init o cosa por el estilo
  } else if (assignment->vector != NULL) {
    
    if (!assignment->vector->initialized) {
      feenox_vector_init(assignment->vector);
    }
    
    constant = assignment->vector->constant;
    current = gsl_vector_ptr(feenox_value_ptr(assignment->vector), row);
    initial_static = gsl_vector_ptr(assignment->vector->initial_static, row);
    initial_transient = gsl_vector_ptr(assignment->vector->initial_transient, row);
    
  } else if (assignment->matrix != NULL) {
    
    if (!assignment->matrix->initialized) {
      feenox_matrix_init(assignment->matrix);
    }
    
    constant = assignment->matrix->constant;
    current = gsl_matrix_ptr(feenox_value_ptr(assignment->matrix), row, col);
    initial_static = gsl_matrix_ptr(assignment->matrix->initial_static, row, col);
    initial_transient = gsl_matrix_ptr(assignment->matrix->initial_transient, row, col);
  } else {
    return WASORA_RUNTIME_OK;
  }
  

  // si tenemos constant y no estamos el primer paso, a comerla
  if (constant && ((int)(feenox_var(feenox_special_var(step_static))) != 1 || (int)(feenox_var(feenox_special_var(step_transient))) != 0)) {
    return WASORA_RUNTIME_OK;
  }
  
  if (assignment->initial_static) {
    // si pide _init solo asignamos si estamos en static_step y en el paso uno
    if ((int)(feenox_var(feenox_special_var(in_static))) && (int)(feenox_var(feenox_special_var(step_static))) == 1) {
      *current = feenox_evaluate_expression(&assignment->rhs);
      *initial_static = *current;
      *initial_transient = *current;
    }
    
    return WASORA_RUNTIME_OK;
    
  } else if (assignment->initial_transient) {
    // si pide _0 solo asignamos si estamos en static_step    
    if ((int)(feenox_var(feenox_special_var(in_static)))) {
      *current = feenox_evaluate_expression(&assignment->rhs);
      *initial_transient = *current;
    }

    return WASORA_RUNTIME_OK;
      
  }
    
  // si el assignment pedido tiene argumento, vemos si tenemos que igualar
  if (assignment->t_min.n_tokens != 0) {
    t_min = feenox_evaluate_expression(&assignment->t_min);
    t_max = feenox_evaluate_expression(&assignment->t_max);
    if (feenox_var(feenox_special_var(time)) >= t_min && feenox_var(feenox_special_var(time)) < t_max) {
      *current = feenox_evaluate_expression(&assignment->rhs);
    }
    return WASORA_RUNTIME_OK;
  }

  // si no tiene argumento y resulta que hay algun assignment que si lo cumple,
  // o alguno tiene _0 o _init entonces no hacemos nada porque en algun momento va a ganar ese
  LL_FOREACH(feenox.assignments, other) {
    if (other != assignment  &&
           ( (other->variable != NULL && other->variable == assignment->variable)
           ||(other->vector   != NULL && other->vector   == assignment->vector)
           ||(other->matrix   != NULL && other->matrix   == assignment->matrix) )) {
      
      if (other->t_min.n_tokens != 0) {
        t_min = feenox_evaluate_expression(&other->t_min);
        t_max = feenox_evaluate_expression(&other->t_max);

        if (feenox_var(feenox_special_var(time)) >= t_min && feenox_var(feenox_special_var(time)) < t_max) {
          return WASORA_RUNTIME_OK;
        }
      }
  
      // si alguno tiene _0, estamos en static y el argumento es el mismo no hacemos nada
      if ((int)(feenox_var(feenox_special_var(in_static))) && other->initial_transient) {
        int i, i_min, i_max, j, j_min, j_max;
        int other_row, other_col;
        feenox_call(feenox_get_assignment_array_boundaries(other, &i_min, &i_max, &j_min, &j_max));
        for (i = i_min; i < i_max; i++) {
          for (j = j_min; j < j_max; j++) {
            feenox_call(feenox_get_assignment_rowcol(other, i, j, &other_row, &other_col));
            if (other_row == row && other_col == col) {
              return WASORA_RUNTIME_OK;
            }
          }
        }
      }

      // idem para _init
      if ((int)(feenox_var(feenox_special_var(in_static))) && (int)(feenox_var(feenox_special_var(step_static))) == 1 && other->initial_static) {
        return WASORA_RUNTIME_OK;
      }
    }
    
  }

  // no gano ningun otro asi que hacemos la asignacion que nos pidieron
  if (current != NULL) {
    *current = feenox_evaluate_expression(&assignment->rhs);

    if (feenox_var(feenox_special_var(in_static))) {
      *initial_transient = *current;
      if ((int)(feenox_var(feenox_special_var(step_static))) == 1) {
        *initial_static = *current;
      }
    }
  }

  return WASORA_RUNTIME_OK;

}


*/
