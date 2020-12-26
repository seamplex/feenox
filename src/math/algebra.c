/*------------ -------------- -------- --- ----- ---   --       -            -
 *  FeenoX algebraic expressions evaluation routines
 *
 *  Copyright (C) 2009--2020 jeremy theler
 *
 *  This file is part of Feenox.
 *
 *  Feenox is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Feenox is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Feenox.  If not, see <http://www.gnu.org/licenses/>.
 *------------------- ------------  ----    --------  --     -       -         -
 */
#include <ctype.h>
#include <string.h>

#include "feenox.h"

extern const char operators[];
extern const char factorseparators[];


// evalua la expresion del argumento y devuelve su valor
double feenox_evaluate_expression(expr_t *this) {

  int level;
//  int n_tokens;
//  int index_i, index_j;
  char tmp_operator;
  expr_factor_t *factor;
  expr_factor_t *E,*P;

  if (this == NULL || this->factors == NULL) {
    return 0;
  }
  
  LL_FOREACH(this->factors, factor) {
    factor->tmp_level = factor->level;

    switch(factor->type & EXPR_BASICTYPE_MASK) {
      case EXPR_CONSTANT:
        factor->value = factor->constant;
      break;
        
      case EXPR_VARIABLE:
        switch (factor->type) {
          case EXPR_VARIABLE | EXPR_CURRENT:
            factor->value = feenox_var_value(factor->variable);
          break;
          case EXPR_VARIABLE | EXPR_INITIAL_TRANSIENT:
            factor->value = *(factor->variable->initial_transient);
          break;
          case EXPR_VARIABLE | EXPR_INITIAL_STATIC:
            factor->value = *(factor->variable->initial_static);
          break;
        }
      break;
      // TODO
/*      
      case EXPR_VECTOR:
    
        if (!token[i].vector->initialized) {
          if (feenox_vector_init(token[i].vector) != WASORA_RUNTIME_OK) {
            feenox_push_error_message("initialization of vector %s failed", token[i].vector->name);
            feenox_runtime_error();
          }
        }
        
        index_i = lrint(feenox_evaluate_expression(&token[i].arg[0]));
        if (index_i <= 0 || index_i > token[i].vector->size) {
          feenox_push_error_message("subindex %d out of range for vector %s", index_i, token[i].vector->name);
          feenox_runtime_error();
          return 0;
        }

        switch (token[i].type) {
          case EXPR_VECTOR | EXPR_CURRENT:
            token[i].value = feenox_vector_get(token[i].vector, index_i-1);
          break;
          case EXPR_VECTOR | EXPR_INITIAL_TRANSIENT:
            token[i].value = feenox_vector_get_initial_transient(token[i].vector, index_i-1);
          break;
          case EXPR_VECTOR | EXPR_INITIAL_STATIC:
            token[i].value = feenox_vector_get_initial_static(token[i].vector, index_i-1);
          break;
        }
      break;
      
      case EXPR_MATRIX:
          
        if (!token[i].matrix->initialized) {
          feenox_call(feenox_matrix_init(token[i].matrix));
        }
    
        index_i = lrint(feenox_evaluate_expression(&token[i].arg[0]));
        if (index_i <= 0 || index_i > token[i].matrix->rows) {
          feenox_push_error_message("row subindex %d out of range for matrix %s", index_i, token[i].matrix->name);
          feenox_runtime_error();
          return 0;
        }
        index_j = (int)(round(feenox_evaluate_expression(&token[i].arg[1])));
        if (index_j <= 0 || index_j > token[i].matrix->cols) {
          feenox_push_error_message("column subindex %d out of range for matrix %s", index_j, token[i].matrix->name);
          feenox_runtime_error();
        }

        switch (token[i].type) {
          case EXPR_MATRIX | EXPR_CURRENT:
            token[i].value = gsl_matrix_get(feenox_value_ptr(token[i].matrix), index_i-1, index_j-1);
          break;
          case EXPR_MATRIX | EXPR_INITIAL_TRANSIENT:
            token[i].value = gsl_matrix_get(token[i].matrix->initial_transient, index_i-1, index_j-1);
          break;
          case EXPR_MATRIX | EXPR_INITIAL_STATIC:
            token[i].value = gsl_matrix_get(token[i].matrix->initial_static, index_i-1, index_j-1);
          break;
        }
      break;

      case EXPR_BUILTIN_FUNCTION:
        token[i].value = token[i].builtin_function->routine(&token[i]);
      break;
      case EXPR_BUILTIN_VECTORFUNCTION:
        token[i].value = token[i].builtin_vectorfunction->routine(token[i].vector_arg);
      break;
      case EXPR_BUILTIN_FUNCTIONAL:
        token[i].value = token[i].builtin_functional->routine(&token[i], token[i].functional_var_arg);
      break;
      case EXPR_FUNCTION:
        token[i].value = feenox_evaluate_factor_function(&token[i]);
      break;
 */
    }
  }

  level = 0;
  LL_FOREACH(this->factors, factor) {
    if (factor->level > level) {
      level = factor->level;
    }
  }

  while (level > 0) {

    for (E = P = this->factors; E != NULL; E->tmp_level != 0 && !E->oper ? P=E : NULL, E = E->next) {
      
      if (E->tmp_level == level && E->oper != 0) {
        tmp_operator = operators[E->oper-1];
        E = E->next;
        switch(tmp_operator) {
          case '&':
            P->value = (int)P->value & (int)E->value;
          break;
          case '|':
            P->value = (int)P->value | (int)E->value;
          break;
          case '=':
           if (fabs(P->value) < 1 || fabs(E->value) < 1) {
             P->value = (fabs(P->value - E->value) < feenox_var_value(feenox_special_var(zero)))?1:0;
           } else {
             P->value = (gsl_fcmp(P->value, E->value, feenox_var_value(feenox_special_var(zero))) == 0)?1:0;
           }
          break;
          case '!':
           if (fabs(P->value) < 1 || fabs(E->value) < 1) {
             P->value = (fabs(P->value - E->value) < feenox_var_value(feenox_special_var(zero)))?0:1;
           } else {
             P->value = (gsl_fcmp(P->value, E->value, feenox_var_value(feenox_special_var(zero))) == 0)?0:1;
           }
          break;
          case '<':
            P->value = P->value < E->value;
          break;
          case '>':
            P->value = P->value > E->value;
          break;
          case '+':
            P->value = P->value + E->value;
          break;
          case '-':
            P->value = P->value - E->value;
          break;
          case '*':
            P->value = P->value * E->value;
          break;
          case '/':
            if (E->value == 0) {
              feenox_nan_error();
            }
            P->value = P->value / E->value;
          break;
          case '^':
            if (P->value == 0 && E->value == 0) {
              feenox_nan_error();
            }
            P->value = pow(P->value, E->value);
          break;
        }
        E->tmp_level = 0;
      }
    }

    level--;

  }

  if (gsl_isnan(this->factors->value) || gsl_isinf(this->factors->value)) {
    feenox_push_error_message("in '%s'", this->string);
    feenox_nan_error();
  }


  return this->factors->value;

}



// evalua la expresion que hay en la cadena
// para eso genera una expresion temporal, la rellena y la evalua
double feenox_evaluate_expression_in_string(const char *string) {
  
  expr_t expr = {NULL, 0, NULL, NULL};
  double val;
  
  if (feenox_parse_expression(string, &expr) != 0) {
    return 0;
  }
  val = feenox_evaluate_expression(&expr);
  // TODO
  // feenox_destroy_expression(&expr);
  
  return val;
}




/*

int feenox_count_divisions(expr_t *expr) {
  
  int i, j;
  int n = 0;
  int m = 0;
  
  for (i = 0; i < expr->n_tokens; i++) {
    
    if (expr->token[i].type == EXPR_OPERATOR && operators[expr->token[i].oper-1] == '/') {
      m++;
    }
    
    if (expr->token[i].arg != NULL) {
      switch (expr->token[i].type) {
        case EXPR_BUILTIN_FUNCTION:
          n = expr->token[i].builtin_function->max_arguments;
        break;
        case EXPR_BUILTIN_FUNCTIONAL: 
          n = expr->token[i].builtin_functional->max_arguments;
        break;
        case EXPR_FUNCTION:
          n = expr->token[i].function->n_arguments;
          if (expr->token[i].function->type == type_algebraic) {
            m += feenox_count_divisions(&expr->token[i].function->algebraic_expression);
          }
        break;
        case EXPR_BUILTIN_VECTORFUNCTION:
          n = expr->token[i].builtin_vectorfunction->max_arguments;
        break;
      }
      for (j = 0; j < n; j++) {
        m += feenox_count_divisions(&expr->token[i].arg[j]);
      }
    }
    
    
  }
  
  return m;
  
}
*/