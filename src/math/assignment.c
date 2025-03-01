/*------------ -------------- -------- --- ----- ---   --       -            -
 *  FeenoX assignment of variables/vectors/matrices
 *
 *  Copyright (C) 2009--2021 Jeremy Theler
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

// API
int feenox_add_assignment(const char *left_hand, const char *right_hand) {
  
  assignment_t *assignment = NULL;
  feenox_check_alloc(assignment = calloc(1, sizeof(assignment_t)));
  
  // let us start assuming the assignment is "plain"
  assignment->plain = 1;
    
  // first from right to left
  // range of indices
  char *index_range = NULL;
  if ((index_range = strchr(left_hand, '<')) != NULL) {
    *index_range = '\0';
  }
  
  // subindexes between brackets if it is a vector or matrix
  char *bracket = NULL;
  if ((bracket = strchr(left_hand, '[')) != NULL) {
    *bracket = '\0';
  }
  
  // initial values, either _init o _0
  char *init = NULL;
  if ((init = feenox_ends_in_init(left_hand)) != NULL) {
    *init = '\0';
    assignment->initial_static = 1;
  }
  
  char *zero = NULL;
  if ((zero = feenox_ends_in_zero(left_hand)) != NULL) {
    *zero = '\0';
    assignment->initial_transient = 1;
  }
  
  char *sanitized_lhs = NULL;
  feenox_check_alloc(sanitized_lhs = strdup(left_hand));
  feenox_call(feenox_strip_blanks(sanitized_lhs));
  if ((assignment->matrix = feenox_get_matrix_ptr(sanitized_lhs)) == NULL &&
      (assignment->vector = feenox_get_vector_ptr(sanitized_lhs)) == NULL) {
    
    // TODO: honor IMPLICIT
    if ((assignment->variable = feenox_get_or_define_variable_get_ptr(sanitized_lhs)) == NULL) {
      return FEENOX_ERROR;  
    }
    assignment->plain = 0;
    assignment->scalar = 1;
  }
  feenox_free(sanitized_lhs);

  
  // now from lef to right
  // re-build the string
  if (zero != NULL) {
    *zero = '_';
    if (assignment->variable != NULL) {
      assignment->variable->assigned_zero = 1;
    } else if (assignment->vector != NULL) {
      assignment->vector->assigned_zero = 1;
    } else if (assignment->matrix != NULL) {
      assignment->matrix->assigned_zero = 1;
    }
  }
  if (init != NULL) {
    *init = '_';
    if (assignment->variable != NULL) {
      assignment->variable->assigned_init = 1;
    } else if (assignment->vector != NULL) {
      assignment->vector->assigned_init = 1;
    } else if (assignment->matrix != NULL) {
      assignment->matrix->assigned_init = 1;
    }
  }
  
  // if there are brackets, then there are subindexes
  if (bracket != NULL) {
    if (assignment->matrix == NULL && assignment->vector == NULL) {
      feenox_push_error_message("'%s' is neither a vector nor a matrix, functions are defined using ':=' instead of '='", left_hand);
      return FEENOX_ERROR;      
    }
    *bracket = '[';
    
    // TODO: check for dependence on i and j by looking at the list of items
    if (assignment->matrix != NULL) {
      // if either "i" or "j" appear then it is not a scalar
      assignment->scalar = (strchr(bracket, 'i') == NULL) && (strchr(bracket, 'j') == NULL);
      // if "i" appears but not "j" then only sweep over rows only
      assignment->expression_only_of_i = (strchr(bracket, 'j') == NULL) && (strchr(bracket, 'i') != NULL);
      // if "i" does not appear but "j" does the sweep over cols only
      assignment->expression_only_of_j = (strchr(bracket, 'i') == NULL) && (strchr(bracket, 'j') != NULL);
      // if it is not "i,j" then it is plain
      if (strcmp("(i,j)", bracket) != 0) {
        assignment->plain = 0;
      }
      feenox_call(feenox_parse_range(bracket, '[', ',', ']', &assignment->row, &assignment->col));
    } else if (assignment->vector != NULL) {
      char *dummy = NULL;
      if ((dummy = strrchr(bracket, ']')) == NULL) {
        feenox_push_error_message("unmatched bracket for '%s'", left_hand);
        return FEENOX_ERROR;
      }
      *dummy = '\0';
      
      // si aparece la letra "i" entonces no es escalar
      assignment->scalar = (strchr(bracket+1, 'i') == NULL);
      
      // si no es "i" entonces no es plain
      if (strcmp("i", bracket+1) != 0) {
        assignment->plain = 0;
      }
      feenox_call(feenox_expression_parse(&assignment->row, bracket+1));
      *dummy = ')';
    }
  }
  
  // if there is an index range
  if (index_range != NULL) {
    if (assignment->matrix == NULL && assignment->vector == NULL) {
      feenox_push_error_message("'%s' is neither a vector nor a matrix", left_hand);
      return FEENOX_ERROR;
    }
    *index_range = '<';
    
    if (assignment->matrix != NULL) {
      feenox_call(feenox_parse_range(index_range, '<', ':', ';', &assignment->i_min, &assignment->i_max));
      feenox_call(feenox_parse_range(strchr(index_range, ';'), ';', ':', '>', &assignment->j_min, &assignment->j_max));
    } else if (assignment->vector != NULL) {
      feenox_call(feenox_parse_range(index_range, '<', ':', '>', &assignment->i_min, &assignment->i_max));
    }
  }
  
  if (assignment->vector == NULL || bracket != NULL || index_range != NULL) {
    // the right-hand side is easy (
    feenox_call(feenox_expression_parse(&assignment->rhs, right_hand));
  
    LL_APPEND(feenox.assignments, assignment);
    if (assignment->variable != NULL) {
      feenox_call(feenox_add_instruction(feenox_instruction_assignment_scalar, assignment));
    } else if (assignment->vector != NULL) {
      feenox_call(feenox_add_instruction(feenox_instruction_assignment_vector, assignment));      
    } else if (assignment->matrix != NULL) {
      feenox_call(feenox_add_instruction(feenox_instruction_assignment_matrix, assignment));      
    } else {
      feenox_push_error_message("invalid assignment");
      return FEENOX_ERROR;
    }
  } else {
    // this block handles the vector initialization 
    vector_t *vector = assignment->vector;
    feenox_free(assignment);
    char *non_const_rhs = strdup(right_hand);
    char *rhs_starting_with_bracket = non_const_rhs;
    while (*rhs_starting_with_bracket != '(') {
      if (*rhs_starting_with_bracket == '\0') {
        feenox_push_error_message("right hand side for vector initialization has to be inside brackets '(' and ')'");
        return FEENOX_ERROR;
      }
      rhs_starting_with_bracket++;
    }
    size_t n_chars_count = 0;
    int n_expressions = 0;
    if ((n_expressions = feenox_count_arguments(rhs_starting_with_bracket, &n_chars_count)) <= 0) {
      feenox_push_error_message("invalid initialization expression");
      return FEENOX_ERROR;
    }
    
    if (vector->size != 0 && n_expressions > vector->size) {
      feenox_push_error_message("more expressions (%d) than vector size (%d)", n_expressions, vector->size);
      return FEENOX_ERROR;
    }

    char **expr = NULL;
    feenox_call(feenox_read_arguments(rhs_starting_with_bracket, n_expressions, &expr, &n_chars_count));
    for (int i = 0; i < n_expressions; i++) {
      feenox_check_alloc(assignment = calloc(1, sizeof(assignment_t)));
      assignment->vector = vector;
      assignment->scalar = 1;
      
      char *i_str = NULL;
      feenox_check_minusone(asprintf(&i_str, "%d", i+1));
      feenox_call(feenox_expression_parse(&assignment->row, i_str));
      feenox_call(feenox_expression_parse(&assignment->rhs, expr[i]));
      feenox_call(feenox_add_instruction(feenox_instruction_assignment_vector, assignment));      
      LL_APPEND(feenox.assignments, assignment);
      feenox_free(i_str);
      feenox_free(expr[i]);
    }
    feenox_free(expr);
    
    feenox_free(non_const_rhs);
  }
    
  return FEENOX_OK;
}

int feenox_instruction_assignment_scalar(void *arg) {
  assignment_t *assignment = (assignment_t *)arg;
  feenox_call(feenox_assign_single(assignment, 0, 0));
  return FEENOX_OK;  
}

int feenox_instruction_assignment_vector(void *arg) {
  assignment_t *assignment = (assignment_t *)arg;
  
  if (assignment->scalar) {
    unsigned int row = (unsigned int)(feenox_expression_eval(&assignment->row) - 1);
    feenox_call(feenox_assign_single(assignment, row, 0));
  } else {
    
    size_t i_min = 0;
    size_t i_max = 0;
    
    
    if (assignment->i_min.items != NULL) {
      // esto es C amigos!
      i_min = (size_t)(round(feenox_expression_eval(&assignment->i_min))) - 1;
      i_max = (size_t)(round(feenox_expression_eval(&assignment->i_max)));
    } else {
      if (!assignment->vector->initialized) {
        feenox_call(feenox_vector_init(assignment->vector, FEENOX_VECTOR_INITIAL));
      }
      i_max = assignment->vector->size;
    }
    
    size_t row = 0;
    size_t i = 0;
    for (i = i_min; i < i_max; i++) {
      feenox_call(feenox_get_assignment_rowcol(assignment, i, 0, &row, NULL));
      feenox_call(feenox_assign_single(assignment, row, 0));
    }
  }

  return FEENOX_OK;  
}

int feenox_instruction_assignment_matrix(void *arg) {
//  assignment_t *assignment = (assignment_t *)arg;
  return FEENOX_OK;  
}

/*  
int feenox_instruction_assignment(void *arg) {
  assignment_t *assignment = (assignment_t *)arg;

  int row, col;
  int i, j;
  int i_min, i_max, j_min, j_max;
  
  if (assignment->i_min.items == NULL) {
    feenox_call(feenox_assign_scalar(assignment, 0, 0));
  }

  feenox_call(feenox_get_assignment_array_boundaries(assignment, &i_min, &i_max, &j_min, &j_max));
 
  for (i = i_min; i < i_max; i++) {
    for (j = j_min; j < j_max; j++) {
      feenox_call(feenox_get_assignment_rowcol(assignment, i, j, &row, &col));
      feenox_call(feenox_assign_scalar(assignment, row, col));
    }
  }
  return FEENOX_OK;
}
*/  

int feenox_get_assignment_array_boundaries(assignment_t *assignment, int *i_min, int *i_max, int *j_min, int *j_max) {
 
  *i_min = 0;
  *i_max = 1;
  *j_min = 0;
  *j_max = 1;
  
  if (!assignment->scalar) {
    if (assignment->i_min.items != NULL) {
      // esto es C amigos!
      *i_min = (int)(round(feenox_expression_eval(&assignment->i_min))) - 1;
      *i_max = (int)(round(feenox_expression_eval(&assignment->i_max)));
    } else {
      
      if (assignment->vector != NULL) {
        
        if (!assignment->vector->initialized) {
          feenox_call(feenox_vector_init(assignment->vector, FEENOX_VECTOR_INITIAL));
        }
        
        *i_max = assignment->vector->size;
        
      } else if (assignment->matrix != NULL) {
        
        if (!assignment->matrix->initialized) {
          feenox_call(feenox_matrix_init(assignment->matrix));
        }
        
        if (assignment->expression_only_of_j == 0) {
          *i_max = assignment->matrix->rows;
        } else {
          *i_min = (int)(round(feenox_expression_eval(&assignment->row))) - 1;
          *i_max = *i_min + 1;
        }
      }
    }
    
    if (assignment->j_min.items != NULL) {
      // esto es C amigos!
      *j_min = (int)(round(feenox_expression_eval(&assignment->j_min))) - 1;
      *j_max = (int)(round(feenox_expression_eval(&assignment->j_max)));
      
    } else {
      if (assignment->matrix != NULL) {
        
        if (!assignment->matrix->initialized) {
          feenox_call(feenox_matrix_init(assignment->matrix));
        }
        
        if (assignment->expression_only_of_i == 0) {
          *j_max = assignment->matrix->cols;
        } else {
          *j_min = (int)(round(feenox_expression_eval(&assignment->col))) - 1;
          *j_max = *j_min + 1;
        }
      }
    }
  }
  
  return FEENOX_OK;
  
}

int feenox_get_assignment_rowcol(assignment_t *assignment, size_t i, size_t j, size_t *row, size_t *col) {
  feenox_special_var_value(i) = (double)(i+1);
  feenox_special_var_value(j) = (double)(j+1);

  if (row != NULL) {
    if (assignment->plain) {
      *row = i;
    } else if (assignment->row.items != NULL) {
      *row = (int)(feenox_expression_eval(&assignment->row))-1;
    } else {
      *row = 0;
    }
  }  

  if (col != NULL) {
    if (assignment->plain) {
      *col = j;
    } else if (assignment->col.items != NULL) {
      *col = (int)(feenox_expression_eval(&assignment->col))-1;
    } else {
      *col = 0;
    }  
  }

  return FEENOX_OK;  
}

int feenox_assign_single(assignment_t *assignment, unsigned int row, unsigned int col) {

  double *current = NULL;
  double *initial_static = NULL;
  double *initial_transient = NULL;
  int assigned_zero = 0;
  int assigned_init = 0;
  
  double value = feenox_expression_eval(&assignment->rhs);

  if (assignment->variable != NULL) {
    current = feenox_value_ptr(assignment->variable);
    initial_static = assignment->variable->initial_static;
    initial_transient = assignment->variable->initial_transient;
    assigned_zero = assignment->variable->assigned_zero;
    assigned_init = assignment->variable->assigned_init;
    
  } else if (assignment->vector != NULL) {
    
    if (!assignment->vector->initialized) {
      feenox_vector_init(assignment->vector, FEENOX_VECTOR_INITIAL);
    }

    if (row >= assignment->vector->size) {
      feenox_push_error_message("out-of-bound assignment, vector '%s' has size %d but element %d was requested", assignment->vector->name, assignment->vector->size, row+1);
      return FEENOX_ERROR;
    }
    
    
    current = gsl_vector_ptr(feenox_value_ptr(assignment->vector), row);
    if (assignment->vector->initial_static != NULL) {
      initial_static = gsl_vector_ptr(assignment->vector->initial_static, row);
    }
    if (assignment->vector->initial_transient != NULL) {
      initial_transient = gsl_vector_ptr(assignment->vector->initial_transient, row);
    } 
    
    assigned_zero = assignment->vector->assigned_zero;
    assigned_init = assignment->vector->assigned_init;
    
    
  } else if (assignment->matrix != NULL) {
    
    if (!assignment->matrix->initialized) {
      feenox_matrix_init(assignment->matrix);
    }
    
    if (col >= assignment->matrix->cols) {
      feenox_push_error_message("out-of-bound assignment, matrix '%s' has %d cols but column %d was requested", assignment->matrix->name, assignment->matrix->cols, col+1);
      return FEENOX_ERROR;
    }
    if (row >= assignment->matrix->rows) {
      feenox_push_error_message("out-of-bound assignment, matrix '%s' has %d rows but row %d was requested", assignment->matrix->name, assignment->matrix->rows, row+1);
      return FEENOX_ERROR;
    }
    
    
    current = gsl_matrix_ptr(feenox_value_ptr(assignment->matrix), row, col);
    if (assignment->matrix->initial_static != NULL) {
      initial_static = gsl_matrix_ptr(assignment->matrix->initial_static, row, col);
    }
    if (assignment->matrix->initial_transient != NULL) {
      initial_transient = gsl_matrix_ptr(assignment->matrix->initial_transient, row, col);
    }

    assigned_zero = assignment->matrix->assigned_zero;
    assigned_init = assignment->matrix->assigned_init;
    
  } else {
    feenox_push_error_message("internal mismatch, neither variable nor vector nor matrix found for assignment");
    return FEENOX_OK;
  }
  
  int in_static = (int)(feenox_special_var_value(in_static));
  int in_static_initial = in_static && (int)(feenox_special_var_value(step_static)) == 1;
  if (assignment->initial_static) {
    if (in_static_initial) {
      *current = value;
      if (initial_static != NULL) {
        *initial_static = value;
      }
      if (initial_transient != NULL) {
        *initial_transient = value;
      }  
    }
    
    return FEENOX_OK;
    
  } else if (assignment->initial_transient) {
    if (in_static) {
      *current = value;
      if (initial_transient != NULL) {
        *initial_transient = value;
      }  
    }

    return FEENOX_OK;
  }
    
  // general assignment
  if (current != NULL && (assigned_init == 0 || in_static_initial == 0) && (assigned_zero == 0 || in_static == 0)) {
    *current = value;
  }

  return FEENOX_OK;

}

