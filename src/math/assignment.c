/*------------ -------------- -------- --- ----- ---   --       -            -
 *  wasora assignment processing
 *
 *  Copyright (C) 2009--2015 jeremy theler
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

#ifndef _WASORA_H_
#include "wasora.h"
#endif

// un assignment puede ser tan sencillo como una variable solita o algo
// mas complejo como la asignacion de ciertos elementos de una matrix/vector
int wasora_instruction_assignment(void *arg) {
  assignment_t *assignment = (assignment_t *)arg;

  int row, col;
  int i, j;
  int i_min, i_max, j_min, j_max;
  
  wasora_call(wasora_get_assignment_array_boundaries(assignment, &i_min, &i_max, &j_min, &j_max));
 
  for (i = i_min; i < i_max; i++) {
    for (j = j_min; j < j_max; j++) {
      wasora_call(wasora_get_assignment_rowcol(assignment, i, j, &row, &col));
      wasora_call(wasora_assign_scalar(assignment, row, col));
    }
  }
  
  return WASORA_RUNTIME_OK;

}

int wasora_get_assignment_array_boundaries(assignment_t *assignment, int *i_min, int *i_max, int *j_min, int *j_max) {
 
  *i_min = 0;
  *i_max = 1;
  *j_min = 0;
  *j_max = 1;
  
  if (!assignment->scalar) {
    if (assignment->i_min.n_tokens != 0) {
      // esto es C amigos!
      *i_min = (int)(round(wasora_evaluate_expression(&assignment->i_min)))-1;
      *i_max = (int)(round(wasora_evaluate_expression(&assignment->i_max)));
    } else {
      
      if (assignment->vector != NULL) {
        
        if (!assignment->vector->initialized) {
          wasora_call(wasora_vector_init(assignment->vector));
        }
        
        *i_max = assignment->vector->size;
        
      } else if (assignment->matrix != NULL) {
        
        if (!assignment->matrix->initialized) {
          wasora_call(wasora_matrix_init(assignment->matrix));
        }
        
        if (assignment->expression_only_of_j == 0) {
          *i_max = assignment->matrix->rows;
        } else {
          *i_min = (int)(round(wasora_evaluate_expression(&assignment->row)))-1;
          *i_max = *i_min + 1;
        }
      }
    }
    
    if (assignment->j_min.n_tokens != 0) {
      // esto es C amigos!
      *j_min = (int)(round(wasora_evaluate_expression(&assignment->j_min)))-1;
      *j_max = (int)(round(wasora_evaluate_expression(&assignment->j_max)));
      
    } else {
      if (assignment->matrix != NULL) {
        
        if (!assignment->matrix->initialized) {
          wasora_call(wasora_matrix_init(assignment->matrix));
        }
        
        if (assignment->expression_only_of_i == 0) {
          *j_max = assignment->matrix->cols;
        } else {
          *j_min = (int)(round(wasora_evaluate_expression(&assignment->col)))-1;
          *j_max = *j_min + 1;
        }
      }
    }
  }
  
  return WASORA_RUNTIME_OK;
  
}

int wasora_get_assignment_rowcol(assignment_t *assignment, int i, int j, int *row, int *col) {
  wasora_var(wasora_special_var(i)) = (double)(i+1);
  wasora_var(wasora_special_var(j)) = (double)(j+1);

  if (assignment->plain) {
    *row = i;
  } else if (assignment->row.n_tokens != 0) {
    *row = (int)(wasora_evaluate_expression(&assignment->row))-1;
  } else {
    *row = 0;
  }

  if (assignment->plain) {
    *col = j;
  } else if (assignment->col.n_tokens != 0) {
    *col = (int)(wasora_evaluate_expression(&assignment->col))-1;
  } else {
    *col = 0;
  }

  return WASORA_RUNTIME_OK;  
}

// verifica si tenemos que poner los valores iniciales de una variable
void wasora_check_initial_variable(var_t *var) {
  
  if (var != NULL) {
    if (wasora_var(wasora_special_var(in_static))) {
      *var->initial_transient = wasora_value(var);
      if ((int)(wasora_var(wasora_special_var(step_static))) == 1) {
        *var->initial_static = wasora_value(var);
      }
    }
  }
  return;  
}

// verifica si tenemos que poner los valores iniciales de una variable
void wasora_check_initial_vector(vector_t *vector) {
  
  if (vector != NULL) {
    if (!vector->initialized) {
      wasora_vector_init(vector);
    }
    if (wasora_var(wasora_special_var(in_static))) {
      gsl_vector_memcpy(vector->initial_transient, wasora_value_ptr(vector));
      if ((int)(wasora_var(wasora_special_var(step_static))) == 1) {
        gsl_vector_memcpy(vector->initial_static, wasora_value_ptr(vector));
      }
    }
  }
  return;  
}

// verifica si tenemos que poner los valores iniciales de una variable
void wasora_check_initial_matrix(matrix_t *matrix) {
  
  if (matrix != NULL) {
    if (wasora_var(wasora_special_var(in_static))) {
      gsl_matrix_memcpy(matrix->initial_transient, wasora_value_ptr(matrix));
      if ((int)(wasora_var(wasora_special_var(step_static))) == 1) {
        gsl_matrix_memcpy(matrix->initial_static, wasora_value_ptr(matrix));
      }
    }
  }
  return;  
}


// ejecuta una asignacion escalar 
int wasora_assign_scalar(assignment_t *assignment, int row, int col) {

  int i;
  double t_min, t_max;
  int constant;
  double *current;
  double *initial_static;
  double *initial_transient;

  assignment_t *other;
  
  if (assignment->variable != NULL) {
    // si es un assignment sobre una variable parametrica, a comerla 
    for (i = 0; i < wasora.parametric.dimensions; i++) {
      if (assignment->variable == wasora.parametric.variable[i]) {
        return WASORA_RUNTIME_OK;
      }
    }
    // idem sobre un parametro a optimizar, aunque dejamos que en la primera vuelta entre
    if ((int)(wasora_var(wasora_special_var(in_outer_initial))) == 0) {
      for (i = 0; i < wasora.fit.p; i++) {
        if (assignment->variable == wasora.fit.param[i]) {
          return WASORA_RUNTIME_OK;
        }
      }
      for (i = 0; i < wasora.min.n; i++) {
        if (assignment->variable == wasora.min.x[i]) {
          return WASORA_RUNTIME_OK;
        }
      }
    }
    
    constant = assignment->variable->constant;
    current = wasora_value_ptr(assignment->variable);
    initial_static = assignment->variable->initial_static;
    initial_transient = assignment->variable->initial_transient;
    
  // TODO: estos punteros se podrian pedir solo si son necesarios, es decir
  // si estamos en init o cosa por el estilo
  } else if (assignment->vector != NULL) {
    
    if (!assignment->vector->initialized) {
      wasora_vector_init(assignment->vector);
    }
    
    constant = assignment->vector->constant;
    current = gsl_vector_ptr(wasora_value_ptr(assignment->vector), row);
    initial_static = gsl_vector_ptr(assignment->vector->initial_static, row);
    initial_transient = gsl_vector_ptr(assignment->vector->initial_transient, row);
    
  } else if (assignment->matrix != NULL) {
    
    if (!assignment->matrix->initialized) {
      wasora_matrix_init(assignment->matrix);
    }
    
    constant = assignment->matrix->constant;
    current = gsl_matrix_ptr(wasora_value_ptr(assignment->matrix), row, col);
    initial_static = gsl_matrix_ptr(assignment->matrix->initial_static, row, col);
    initial_transient = gsl_matrix_ptr(assignment->matrix->initial_transient, row, col);
  } else {
    return WASORA_RUNTIME_OK;
  }
  

  // si tenemos constant y no estamos el primer paso, a comerla
  if (constant && ((int)(wasora_var(wasora_special_var(step_static))) != 1 || (int)(wasora_var(wasora_special_var(step_transient))) != 0)) {
    return WASORA_RUNTIME_OK;
  }
  
  if (assignment->initial_static) {
    // si pide _init solo asignamos si estamos en static_step y en el paso uno
    if ((int)(wasora_var(wasora_special_var(in_static))) && (int)(wasora_var(wasora_special_var(step_static))) == 1) {
      *current = wasora_evaluate_expression(&assignment->rhs);
      *initial_static = *current;
      *initial_transient = *current;
    }
    
    return WASORA_RUNTIME_OK;
    
  } else if (assignment->initial_transient) {
    // si pide _0 solo asignamos si estamos en static_step    
    if ((int)(wasora_var(wasora_special_var(in_static)))) {
      *current = wasora_evaluate_expression(&assignment->rhs);
      *initial_transient = *current;
    }

    return WASORA_RUNTIME_OK;
      
  }
    
  // si el assignment pedido tiene argumento, vemos si tenemos que igualar
  if (assignment->t_min.n_tokens != 0) {
    t_min = wasora_evaluate_expression(&assignment->t_min);
    t_max = wasora_evaluate_expression(&assignment->t_max);
    if (wasora_var(wasora_special_var(time)) >= t_min && wasora_var(wasora_special_var(time)) < t_max) {
      *current = wasora_evaluate_expression(&assignment->rhs);
    }
    return WASORA_RUNTIME_OK;
  }

  // si no tiene argumento y resulta que hay algun assignment que si lo cumple,
  // o alguno tiene _0 o _init entonces no hacemos nada porque en algun momento va a ganar ese
  LL_FOREACH(wasora.assignments, other) {
    if (other != assignment  &&
           ( (other->variable != NULL && other->variable == assignment->variable)
           ||(other->vector   != NULL && other->vector   == assignment->vector)
           ||(other->matrix   != NULL && other->matrix   == assignment->matrix) )) {
      
      if (other->t_min.n_tokens != 0) {
        t_min = wasora_evaluate_expression(&other->t_min);
        t_max = wasora_evaluate_expression(&other->t_max);

        if (wasora_var(wasora_special_var(time)) >= t_min && wasora_var(wasora_special_var(time)) < t_max) {
          return WASORA_RUNTIME_OK;
        }
      }
  
      // si alguno tiene _0, estamos en static y el argumento es el mismo no hacemos nada
      if ((int)(wasora_var(wasora_special_var(in_static))) && other->initial_transient) {
        int i, i_min, i_max, j, j_min, j_max;
        int other_row, other_col;
        wasora_call(wasora_get_assignment_array_boundaries(other, &i_min, &i_max, &j_min, &j_max));
        for (i = i_min; i < i_max; i++) {
          for (j = j_min; j < j_max; j++) {
            wasora_call(wasora_get_assignment_rowcol(other, i, j, &other_row, &other_col));
            if (other_row == row && other_col == col) {
              return WASORA_RUNTIME_OK;
            }
          }
        }
      }

      // idem para _init
      if ((int)(wasora_var(wasora_special_var(in_static))) && (int)(wasora_var(wasora_special_var(step_static))) == 1 && other->initial_static) {
        return WASORA_RUNTIME_OK;
      }
    }
    
  }

  // no gano ningun otro asi que hacemos la asignacion que nos pidieron
  if (current != NULL) {
    *current = wasora_evaluate_expression(&assignment->rhs);

    if (wasora_var(wasora_special_var(in_static))) {
      *initial_transient = *current;
      if ((int)(wasora_var(wasora_special_var(step_static))) == 1) {
        *initial_static = *current;
      }
    }
  }

  return WASORA_RUNTIME_OK;

}


