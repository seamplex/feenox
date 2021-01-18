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
#include "feenox.h"
extern feenox_t feenox;

#include <math.h>
#include <ctype.h>
#include <string.h>

extern const char operators[];
extern const char factorseparators[];

extern char *feenox_ends_in_init(char *name);
extern char *feenox_ends_in_zero(char *name);
extern int feenox_count_arguments(char *string);

//  parse a string with an algebraic expression and fills in the struct expr
int feenox_expression_parse(expr_t *this, const char *orig_string) {

  if (orig_string == NULL || strcmp(orig_string, "") == 0) {
    return FEENOX_OK;
  }

  // let's make a copy so the parser can break it up as it wants
  char *string_copy = strdup(orig_string);

  // the expr structure contains another copy of the original string for debugging purposes
  this->string = strdup(string_copy);
  
  char *string = string_copy;
  char *oper = NULL;
  int last_was_an_op = 0;
  size_t level = 1;
  expr_factor_t *factor;
  size_t delta_level = strlen(operators);

  while (*string != '\0') {

    if (isblank(*string)) {
      string++;
    } else if ((oper = strchr(operators, *string)) != NULL) {
      if (*string == '(') {
        level += delta_level;
        string++;
        last_was_an_op = 1;
      } else if (*string == ')') {
        level -= delta_level;
        string++;
        last_was_an_op = 0;
      } else {       // +-*/^.
        if (last_was_an_op == 1) {
          if ((*oper == '+' || *oper == '-')) {
            if ((factor = feenox_expression_parse_factor(string)) == NULL) {
              return FEENOX_ERROR;
            }
            factor->level = level;
            LL_APPEND(this->factors, factor);
            string += factor->n_chars;
            last_was_an_op = 0;
          } else {
            feenox_push_error_message("two adjacent operators");
            return FEENOX_ERROR;
          }
        } else { 
          factor = calloc(1, sizeof(expr_factor_t));
          factor->type = EXPR_OPERATOR;
          factor->oper = oper-operators+1;
          // precedence two by two from left to right
          factor->level = level+(oper-operators)/2*2; 
          LL_APPEND(this->factors, factor);
          string++;
          last_was_an_op = 1;
        }
      }
    } else {
      // a constant, variable or function
      if ((factor = feenox_expression_parse_factor(string)) == NULL) {
        return FEENOX_ERROR;
      }
      LL_APPEND(this->factors, factor);
      factor->level = level;
      if (factor->n_chars <= 0) {
        return FEENOX_ERROR;
      }
      string += factor->n_chars;
      last_was_an_op = 0;
    }
  }

  if (level != 1) {
    feenox_push_error_message("unmatched opening bracket in algebraic expression");
    return FEENOX_ERROR;
  } else if (oper != NULL && *oper != ')') {
    feenox_push_error_message("missing argument for operator '%c'", *oper);
    return FEENOX_ERROR;
  }

  free(string_copy);
  return FEENOX_OK;

}


// parse a factor (which means a constant, a variable or a function)
expr_factor_t *feenox_expression_parse_factor(const char *string) {

  char *backup;
  char *argument;
  double constant;
  int i;
  int n;
  int n_arguments;
  int n_allocs;
  char *token;
  
  char *dummy_argument;

  var_t *dummy_var = NULL;
  vector_t *dummy_vector = NULL;
  matrix_t *dummy_matrix = NULL;

  builtin_function_t *dummy_builtin_function = NULL;
  builtin_vectorfunction_t *dummy_builtin_vectorfunction = NULL;
  builtin_functional_t *dummy_builtin_functional = NULL;

  function_t *dummy_function = NULL;

  char *dummy = NULL;
  char char_backup = ' ';
  
  int wants_initial_static = 0;
  int wants_initial_transient = 0;
  int got_it = 0;

  backup = strdup(string);
  
  expr_factor_t *factor = calloc(1, sizeof(expr_factor_t));
  

  // either an explicit number or an explicit sign or a dot for gringos that write ".1" instead of "0.1"  
  if (isdigit((int)(*string)) || *string == '-' || *string == '+' || *string == '.') {
    // a number
    if (sscanf(string, "%lf%n", &constant, &n) == 0) {
      return NULL;
    }
    factor->type = EXPR_CONSTANT;
    factor->constant = constant;

  } else {
    // we got letters
    if ((token = strtok(backup, factorseparators)) == NULL || strlen(token) == 0) {
      return NULL;
    }

    // if the name ends in _0 it means the user wants the initial value
    if (feenox_ends_in_zero(token) != NULL) {
      dummy = feenox_ends_in_zero(token);
      wants_initial_transient = 1;
      *dummy = '\0';
    }

    // same for _init
    // if we put if (dummy = feenox_ends_in_init) and the guy returns null we break
    // the dummy the the feenox_ends_in_zero() got us
    if (feenox_ends_in_init(token) != NULL) {
      dummy = feenox_ends_in_init(token);
      wants_initial_static = 1;
      *dummy = '\0';
    }

    // cannot ask for both
    if (wants_initial_transient && wants_initial_static) {
      feenox_push_error_message("cannot ask for both _0 and _init");
      return NULL;
    }
    
    // matrix, vector or variable
    if ((dummy_matrix = feenox_get_matrix_ptr(token)) != NULL) {
      got_it = 1;
      factor->matrix = dummy_matrix;
      factor->type = EXPR_MATRIX;
    } else if ((dummy_vector = feenox_get_vector_ptr(token)) != NULL) {
      got_it = 1;
      factor->vector = dummy_vector;
      factor->type = EXPR_VECTOR;
    } else if ((dummy_var = feenox_get_variable_ptr(token)) != NULL) {
      // check that variables don't need arguments
      if (string[strlen(token)] == '(') {
        feenox_push_error_message("variable '%s' does not take arguments (it is a variable)", token);
        free(backup);
        return NULL;
      }
      got_it = 1;
      factor->variable = dummy_var;
      factor->type = EXPR_VARIABLE;
    }

    if (got_it) {
      n = strlen(token);
      if (wants_initial_transient) {
        n += 2;
        factor->type |= EXPR_INITIAL_TRANSIENT;
      } else if (wants_initial_static) {
        n += 5;
        factor->type |= EXPR_INITIAL_STATIC;
      }
      // put the underscore back
      if (wants_initial_transient || wants_initial_static)  {
        *dummy = '_';
      }
    }

    if ( dummy_matrix != NULL ||
         dummy_vector != NULL ||
        (dummy_function = feenox_get_function_ptr(token)) != NULL ||
        (dummy_builtin_function = feenox_get_builtin_function_ptr(token)) != NULL ||
        (dummy_builtin_vectorfunction = feenox_get_builtin_vectorfunction_ptr(token)) != NULL ||
        (dummy_builtin_functional = feenox_get_builtin_functional_ptr(token)) != NULL) {

      int level;
      
      got_it = 1;

      // copy into argument whatever is after the name
      argument = strdup(string+strlen(token));

      // arguments have to be in parenthesis
      if (argument[0] != '(') {
        feenox_push_error_message("expected parenthesis after '%s'", token);
        free(argument);
        free(backup);
        return NULL;
      }

      // count how many arguments are there
      if ((n_arguments = feenox_count_arguments(argument)) <= 0) {
        free(argument);
        free(backup);
        return NULL;
      }

      // n is the number of characters to parse
      n = strlen(token)+strlen(argument);

      if (factor != NULL) {

        if (dummy_vector != NULL) {
          if (n_arguments < 1 || n_arguments > 1) {
            feenox_push_error_message("vector '%s' takes exactly one subindex expression", dummy_vector->name);
            free(argument);
            free(backup);
            return NULL;
          }
          n_allocs = 1;
        } else if (dummy_matrix != NULL) {
          if (n_arguments < 2 || n_arguments > 2) {
            feenox_push_error_message("matrix '%s' takes exactly two subindex expressions", dummy_matrix->name);
            free(argument);
            free(backup);
            return NULL;
          }
          n_allocs = 2;
/*          
        } else if (dummy_builtin_function != NULL) {

          // tenemos una funcion interna
          factor->type = EXPR_BUILTIN_FUNCTION;
          factor->builtin_function = dummy_builtin_function;

          if (n_arguments < factor->builtin_function->min_arguments) {
            feenox_push_error_message("function '%s' takes at least %d argument%s instead of %d", token, factor->builtin_function->min_arguments, (factor->builtin_function->min_arguments==1)?"":"s", n_arguments);
            free(argument);
            free(backup);
            return FEENOX_ERROR;
          }
          if (n_arguments > factor->builtin_function->max_arguments) {
            feenox_push_error_message("function '%s' takes at most %d argument%s instead of %d", token, factor->builtin_function->max_arguments, (factor->builtin_function->max_arguments==1)?"":"s", n_arguments);
            free(argument);
            free(backup);
            return FEENOX_ERROR;
          }

          n_allocs = factor->builtin_function->max_arguments;

        } else if (dummy_builtin_vectorfunction != NULL) {
          
          // tenemos una funcion sobre vectores interna
          factor->type = EXPR_BUILTIN_VECTORFUNCTION;
          factor->builtin_vectorfunction = dummy_builtin_vectorfunction;
          
          if (n_arguments < factor->builtin_vectorfunction->min_arguments) {
            feenox_push_error_message("function '%s' takes at least %d argument%s instead of %d", token, factor->builtin_vectorfunction->min_arguments, (factor->builtin_vectorfunction->min_arguments==1)?"":"s", n_arguments);
            free(argument);
            free(backup);
            return FEENOX_ERROR;
          }
          if (n_arguments > factor->builtin_vectorfunction->max_arguments) {
            feenox_push_error_message("function '%s' takes at most %d argument%s instead of %d", token, factor->builtin_vectorfunction->max_arguments, (factor->builtin_vectorfunction->max_arguments==1)?"":"s", n_arguments);
            free(argument);
            free(backup);
            return FEENOX_ERROR;
          }

          n_allocs = factor->builtin_vectorfunction->max_arguments;
          
          
        } else if (dummy_builtin_functional != NULL) {

          // tenemos un funcional interno
          factor->type = EXPR_BUILTIN_FUNCTIONAL;
          factor->builtin_functional = dummy_builtin_functional;

          if (n_arguments < factor->builtin_functional->min_arguments) {
            feenox_push_error_message("functional '%s' takes at least %d argument%s instead of %d", token, factor->builtin_functional->min_arguments, (factor->builtin_functional->min_arguments==1)?"":"s", n_arguments);
            free(argument);
            free(backup);
            return FEENOX_ERROR;
          }
          if (n_arguments > factor->builtin_functional->max_arguments) {
            feenox_push_error_message("functional '%s' takes at most %d argument%s instead of %d", token, factor->builtin_functional->max_arguments, (factor->builtin_functional->max_arguments==1)?"":"s", n_arguments);
            free(argument);
            free(backup);
            return FEENOX_ERROR;
          }

          n_allocs = factor->builtin_functional->max_arguments;
*/
        } else if (dummy_function != NULL) {

          // tenemos una funcion del input
          factor->type = EXPR_FUNCTION;
          factor->function = dummy_function;

          if (n_arguments != factor->function->n_arguments) {
            feenox_push_error_message("function '%s' takes exactly %d argument%s instead of %d", token, factor->function->n_arguments, (factor->function->n_arguments==1)?"":"s", n_arguments);
            free(argument);
            free(backup);
            return NULL;
          }

          n_allocs = factor->function->n_arguments;

        } else {
          free(backup);
          free(argument);
          return NULL;
        }

        if (factor->type != EXPR_BUILTIN_VECTORFUNCTION) {
          factor->arg = calloc(n_allocs, sizeof(expr_t));
        } else if (factor->type == EXPR_BUILTIN_VECTORFUNCTION) {
          factor->vector_arg = calloc(n_allocs, sizeof(vector_t *));
        }


        if (n_arguments == 1) {

          // if the functions takes only one argument this is easy
          if (factor->type != EXPR_BUILTIN_VECTORFUNCTION) {
            if (feenox_expression_parse(&factor->arg[0], argument) != 0) {
              free(argument);
              free(backup);
              return NULL;
            }
          } else if (factor->type == EXPR_BUILTIN_VECTORFUNCTION) {
            argument[strlen(argument)-1] = '\0';
            if ((factor->vector_arg[0] = feenox_get_vector_ptr(&argument[1])) == NULL) {
              feenox_push_error_message("undefined vector '%s'", &argument[1]);
              free(argument);
              free(backup);
              return NULL;
            }
          }

        } else {

          // if the function takes more arguments we need to read them one by one

          // if we directly increment argument then we cannot free() it
          dummy_argument = argument;
          for (i = 0; i < n_arguments; i++) {
            if (i != n_arguments-1) {

              level = 1;
              dummy = dummy_argument+1;
              while (1) {
                if (*dummy == ',' && level == 1) {
                  break;
                }
                if (*dummy == '(') {
                  level++;
                } else if (*dummy == ')') {
                  level--;
                } else if (*dummy == '\0') {
                  free(argument);
                  free(backup);
                  return NULL;
                }
                dummy++;
              }

              *dummy = ')';
              char_backup = *(dummy+1);
              *(dummy+1) = '\0';
            }


            // en dummy_argument tenemos el argumento i-esimo
            // entre parentesis
            if (factor->type == EXPR_BUILTIN_VECTORFUNCTION) {
              
              // le sacamos los parentesis
              dummy_argument[strlen(dummy_argument)-1] = '\0';
              if ((factor->vector_arg[i] = feenox_get_vector_ptr(&dummy_argument[1])) == NULL) {
                feenox_push_error_message("undefined vector '%s'", &dummy_argument[1]);
                free(argument);
                free(backup);
                return NULL;
              }
              
            } else if (factor->type == EXPR_BUILTIN_FUNCTIONAL && i == 1) {
              // si es un funcional, el segundo argumento es una variable
              // pero tenemos que sacarle los parentesis
              *(dummy) = '\0';
              if ((factor->functional_var_arg = feenox_get_variable_ptr(dummy_argument+1)) == NULL) {
                free(argument);
                free(backup);
                return NULL;
              }
            } else {
              if (feenox_expression_parse(&factor->arg[i], dummy_argument) != 0) {
                free(argument);
                free(backup);
                return NULL;
              }
            }

            if (i != n_arguments-1) {
              *dummy = '(';
              *(dummy+1) = char_backup;
              dummy_argument = dummy;
            }
          }
        }
      }
      
      free(argument);
      
    } 

    if (got_it == 0) {
      feenox_push_error_message("unknown symbol '%s'", token);
      free(backup);
      return NULL;
    }
  }

  free(backup);
  factor->n_chars = n;

  return factor;

}



double feenox_expression_eval(expr_t *this) {

  size_t i, j;
  int level;
  char tmp_operator;
  expr_factor_t *factor;
  expr_factor_t *E,*P;

  if (this == NULL || this->factors == NULL) {
    return 0;
  }
  
  LL_FOREACH(this->factors, factor) {
    factor->tmp_level = factor->level;

    // TODO: replace the switch by pointer to functions (i.e. virtual methods in C++ slang)?
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
      case EXPR_VECTOR:
        
        // we need to initialize here so we have the size for the check that follows
        if (factor->vector->initialized == 0) {
          if (feenox_vector_init(factor->vector) != FEENOX_OK) {
            feenox_push_error_message("initialization of vector '%s' failed", factor->vector->name);
            feenox_runtime_error();
          }
        }
    
        i = lrint(feenox_expression_eval(&(factor->arg[0])));
        if (i <= 0 || i > factor->vector->size) {
          feenox_push_error_message("subindex %d out of range for vector %s", i, factor->vector->name);
          feenox_runtime_error();
          return 0;
        }

        switch (factor->type) {
          case EXPR_VECTOR | EXPR_CURRENT:
            factor->value = gsl_vector_get(factor->vector->value, i-1);
          break;
          case EXPR_VECTOR | EXPR_INITIAL_TRANSIENT:
            factor->value = gsl_vector_get(factor->vector->initial_transient, i-1);
          break;
          case EXPR_VECTOR | EXPR_INITIAL_STATIC:
            factor->value = gsl_vector_get(factor->vector->initial_static, i-1);
          break;
        }
      break;
      
      case EXPR_MATRIX:
        
        if (factor->matrix->initialized == 0) {
          if (feenox_matrix_init(factor->matrix) != FEENOX_OK) {
            feenox_push_error_message("initialization of vector '%s' failed", factor->vector->name);
            feenox_runtime_error();
          }
        }
        
        i = lrint(feenox_expression_eval(&(factor->arg[0])));
        if (i <= 0 || i > factor->matrix->rows) {
          feenox_push_error_message("row subindex %d out of range for matrix %s", i, factor->matrix->name);
          feenox_runtime_error();
          return 0;
        }
        j = lrint(feenox_expression_eval(&(factor->arg[1])));
        if (j <= 0 || j > factor->matrix->cols) {
          feenox_push_error_message("column subindex %d out of range for matrix %s", j, factor->matrix->name);
          feenox_runtime_error();
          return 0;
        }

        switch (factor->type) {
          case EXPR_MATRIX | EXPR_CURRENT:
            factor->value = gsl_matrix_get(factor->matrix->value, i-1, j-1);
          break;
          case EXPR_MATRIX | EXPR_INITIAL_TRANSIENT:
            factor->value = gsl_matrix_get(factor->matrix->initial_transient, i-1, j-1);
          break;
          case EXPR_MATRIX | EXPR_INITIAL_STATIC:
            factor->value = gsl_matrix_get(factor->matrix->initial_static, i-1, j-1);
          break;
        }
      break;

/*      
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
             P->value = (fabs(P->value - E->value) < feenox_special_var_value(zero))?1:0;
           } else {
             P->value = (gsl_fcmp(P->value, E->value, feenox_special_var_value(zero)) == 0)?1:0;
           }
          break;
          case '!':
           if (fabs(P->value) < 1 || fabs(E->value) < 1) {
             P->value = (fabs(P->value - E->value) < feenox_special_var_value(zero))?0:1;
           } else {
             P->value = (gsl_fcmp(P->value, E->value, feenox_special_var_value(zero)) == 0)?0:1;
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



// evaluates the expression contained in the string
double feenox_evaluate_expression_in_string(const char *string) {
  
  expr_t expr = {NULL, 0, NULL, NULL};
  double val;
  
  if (feenox_expression_parse(&expr, string) != FEENOX_OK) {
    return 0;
  }
  val = feenox_expression_eval(&expr);
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
