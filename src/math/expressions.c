/*------------ -------------- -------- --- ----- ---   --       -            -
 *  FeenoX algebraic expressions evaluation routines
 *
 *  Copyright (C) 2009--2021 jeremy theler
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

const char operators[]        = "&|=!<>+-*/^,()[]";
const char factorseparators[] = "&|=!<>+-*/^,()[] \t\n";

//extern const char operators[];
//extern const char factorseparators[];

char *feenox_ends_in_zero(const char *name) {

  char *dummy;

  if (((dummy = strstr(name, "_0")) != 0) && (isblank(dummy[2]) || dummy[2] == '\0')) {
    return dummy;
  } else {
    return NULL;
  }
  
}

char *feenox_ends_in_init(const char *name) {

  char *dummy;

  if (((dummy = strstr(name, "_init")) != 0) && (dummy[5] == '\0' || dummy[5] == '(')) {
    return dummy;
  } else {
    return NULL;
  }
}


char *feenox_ends_in_dot(const char *name) {

  char *dummy;

  if (((dummy = strstr(name, "_dot")) != 0) && (dummy[4] == '\0' || dummy[4] == '(')) {
    return dummy;
  } else {
    return NULL;
  }
}

int feenox_count_arguments(char *string, size_t *n_chars) {

  // arguments have to be inside parenthesis or brackets
  if (string[0] != '(' && string[0] != '[') {
    feenox_push_error_message("argument list needs to start with a parenthesis");
    return -1;
  }

  // count how many arguments are there (take into account nested parenthesis)
  char *s = string+1;
  size_t n = 1;
  size_t level = 1;
  int n_arguments = 1;
  while (level != 0) {
    if (*s == '(' || *s == '[') {
      level++;
    } else if (*s == ')' || *s == ']') {
      level--;
    } else if (*s == '\0') {
      feenox_push_error_message("argument list needs to be closed with ')'");
      return -1;
    }
    if (*s == ',' && level == 1) {
      n_arguments++;
    }
    s++;
    n++;
  }
  *s = '\0';

  if (n_chars != NULL) {
    *n_chars = n;
  }
  
  return n_arguments;
}


int feenox_read_arguments(char *string, unsigned int n_arguments, char ***arg, size_t *n_chars) {

  if (string[0] != '(' && string[0] != '[') {
    feenox_push_error_message("arguments must start with a parenthesis");
    return FEENOX_ERROR;
  }
  
  feenox_check_alloc((*arg) = calloc(n_arguments, sizeof(char *)));

  int i;
  size_t n = 0;
  char *dummy = string;
  char char_backup;
  for (i = 0; i < n_arguments; i++) {
    size_t level = 1;
    dummy++;
    n++;
    char *argument = dummy;
    while (1) {
      // if level is 1 and next char is ',' or ')' and we are on the last argument, we are done
      if (level == 1 && ((i != n_arguments-1 && *dummy == ',') || (i == n_arguments-1 && (*dummy == ')' || *dummy == ']')))) {
        break;
      }
          
      if (*dummy == '(' || *dummy == '[') {
        level++;
      } else if (*dummy == ')' || *dummy == ']') {
        level--;
      } else if (*dummy == '\0') {
        feenox_push_error_message("when parsing arguments");
        return FEENOX_ERROR;
      }
      dummy++;
      n++;
    }

    // put a '\0' after dummy but make a backup of what there was there
    char_backup = *dummy;
    *dummy = '\0';
    // in argument we have the i-th argument
    feenox_check_alloc((*arg)[i] = strdup(argument));
    *dummy = char_backup;
  }
  
  if (n_chars != NULL) {
    // the +1 is because of the final closing parenthesis
    *n_chars = n+1;
  }
  
  return FEENOX_OK;
}


//  parse a string with an algebraic expression and fill in the struct expr
int feenox_expression_parse(expr_t *this, const char *orig_string) {

  if (orig_string == NULL || strcmp(orig_string, "") == 0) {
    return FEENOX_OK;
  }

  // let's make a copy so the parser can break it up as it wants
  char *string_copy = NULL;
  feenox_check_alloc(string_copy = strdup(orig_string));

  // the expr structure contains another copy of the original string for debugging purposes
  feenox_check_alloc(this->string = strdup(string_copy));
  
  char *string = string_copy;
  char *oper = NULL;
  char last_op = '(';  // initially it is like we start with an opening parenthesis
  size_t level = 1;
  expr_item_t *item;
  // TODO: maybe we can get away wih half delta_level?
  size_t delta_level = strlen(operators);

  while (*string != '\0') {

    if (isblank(*string)) {
      // blanks are ignored
      string++;
    } else if ((oper = strchr(operators, *string)) != NULL) {
      // handle one of the operatos
      if (*string == '(') {
        
        level += delta_level;
        last_op = '(';
        string++;
        
      } else if (*string == ')') {
        
        level -= delta_level;
        last_op = ')';
        string++;

      // TODO: to avoid problems with spaces a get_next_char() method is needed
      } else if (last_op == '(' && (string[0] == '-' || string[0] == '+') && (isalpha(string[1]) || string[1] == '(')) {
        
        // having "(-xxx..." is like having "(0-xxx..."
        feenox_check_alloc(item = calloc(1, sizeof(expr_item_t)));
        item->type = EXPR_CONSTANT;
        item->constant = 0;
        item->level = level; 
        LL_APPEND(this->items, item);
          
        feenox_check_alloc(item = calloc(1, sizeof(expr_item_t)));
        item->type = EXPR_OPERATOR;
        item->oper = (string[0] == '+') ? 7 : 8; // hard-coded location of '+'/'-' within operators
        item->level = level+((item->oper-1)/2)*2;
        LL_APPEND(this->items, item);
        
        string++;
        
      } else if ((last_op != '\0' && last_op != ')') && (*oper == '+' || *oper == '-')) {
        
        if ((item = feenox_expression_parse_item(string)) == NULL) {
          return FEENOX_ERROR;
        }
        item->level = level;
        LL_APPEND(this->items, item);
        
        string += item->n_chars;
        last_op = '\0';
        oper = NULL;  // reset the operator because it was not an actual operator
            
        feenox_pull_dependencies_variables(&this->variables, item->variables);
        feenox_pull_dependencies_functions(&this->functions, item->functions);
            
      } else if (last_op == '\0' || last_op == ')') {
        
        feenox_check_alloc(item = calloc(1, sizeof(expr_item_t)));
        item->type = EXPR_OPERATOR;
        // precedence two by two from left to right
        size_t delta = oper - operators;
        size_t incr = (delta - (delta % 2));
        item->oper = delta + 1;
        item->level = level + incr;
        LL_APPEND(this->items, item);
        
        string++;
        last_op = *oper;
        
      } else {
        
        feenox_push_error_message("two adjacent operators '%c' and '%c'", last_op, *string);
        return FEENOX_ERROR;
        
      }
    } else {
      // a constant, variable or function
      if ((item = feenox_expression_parse_item(string)) == NULL) {
        return FEENOX_ERROR;
      }
      LL_APPEND(this->items, item);
      item->level = level;
      if (item->n_chars <= 0) {
        return FEENOX_ERROR;
      }
      string += item->n_chars;
      last_op = '\0';
      
      feenox_pull_dependencies_variables(&this->variables, item->variables);
      feenox_pull_dependencies_functions(&this->functions, item->functions);
    }
  }

  if (level != 1) {
    feenox_push_error_message("unmatched opening bracket in algebraic expression");
    return FEENOX_ERROR;
  } else if (oper != NULL && *oper != ')') {
    feenox_push_error_message("missing argument for operator '%c'", *oper);
    return FEENOX_ERROR;
  }

  feenox_free(string_copy);
  return FEENOX_OK;

}


// parse a item within an expression (which means a constant, a variable or a function)
expr_item_t *feenox_expression_parse_item(const char *string) {

  // number of characters read, we put this into the allocated item at the end of this routine
  size_t n = 0;
  int n_int = 0; // sscanf can only return ints, not size_t
  char *backup ;
  feenox_check_alloc_null(backup = strdup(string));
  expr_item_t *item = NULL;
  feenox_check_alloc_null(item = calloc(1, sizeof(expr_item_t)));

  // either an explicit number or an explicit sign or a dot for gringos that write ".1" instead of "0.1"  
  if (isdigit((int)(*string)) || ((*string == '-' || *string == '+' || *string == '.') && isdigit((int)string[1]))) {
    // a number
    double constant;
    if (sscanf(string, "%lf%n", &constant, &n_int) == 0) {
      return NULL;
    }
    n += n_int;
    item->type = EXPR_CONSTANT;
    item->constant = constant;
  } else {
    // we got letters
    char *token = strtok(backup, factorseparators);
    if (token == NULL || strlen(token) == 0) {
      return NULL;
    }

    char *dummy = NULL;
    int wants_initial_transient = 0;
    int wants_initial_static = 0;
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
    item->type = EXPR_UNDEFINED;
    var_t *var = NULL;
    vector_t *vector = NULL;
    matrix_t *matrix = NULL;
    builtin_function_t *builtin_function = NULL;
    builtin_vectorfunction_t *builtin_vectorfunction = NULL;
    builtin_functional_t *builtin_functional = NULL;
    function_t *function = NULL;
    
    if ((matrix = feenox_get_matrix_ptr(token)) != NULL) {
      item->type = EXPR_MATRIX;
      item->matrix = matrix;
    } else if ((vector = feenox_get_vector_ptr(token)) != NULL) {
      item->type = EXPR_VECTOR;
      item->vector = vector;
    } else if ((var = feenox_get_variable_ptr(token)) != NULL) {
      // check that variables don't need arguments
      if (string[strlen(token)] == '(' || string[strlen(token)] == '[') {
        feenox_push_error_message("variable '%s' does not take arguments (it is a variable)", token);
        return NULL;
      }
      item->type = EXPR_VARIABLE;
      item->variable = var;
      
      // mark that this item depends on var
      var_ll_t *var_item = NULL;
      feenox_check_alloc_null(var_item = calloc(1, sizeof(var_ll_t)));
      var_item->var = var;
      LL_APPEND(item->variables, var_item);
    }

    if (matrix != NULL || vector != NULL || var != NULL) {
      n += strlen(token);
      if (wants_initial_transient) {
        n += 2;
        item->type |= EXPR_INITIAL_TRANSIENT;
      } else if (wants_initial_static) {
        n += 5;
        item->type |= EXPR_INITIAL_STATIC;
      }
      // put the underscore back
      if (wants_initial_transient || wants_initial_static)  {
        *dummy = '_';
      }
    }

    if ( matrix != NULL ||
         vector != NULL ||
        (function = feenox_get_function_ptr(token)) != NULL ||
        (builtin_function = feenox_get_builtin_function_ptr(token)) != NULL ||
        (builtin_vectorfunction = feenox_get_builtin_vectorfunction_ptr(token)) != NULL ||
        (builtin_functional = feenox_get_builtin_functional_ptr(token)) != NULL) {

      // copy into argument whatever is after the nam
      char *argument;
      feenox_check_alloc_null(argument = strdup(string+strlen(token)));

      // TODO: differentiate between functions and vectors/matrices
      // arguments have to be in parenthesis
      if (*argument != '[' && *argument != '(') {
        feenox_push_error_message("expected parenthesis after '%s'", token);
        return NULL;
      }

      // read the arguments and keep them as an array of strings
      size_t n_chars_count;
      int n_arguments = feenox_count_arguments(argument, &n_chars_count);
      if (n_arguments <= 0) {
        return NULL;
      }

      char **arg = NULL;
      size_t n_chars_parse;
      if (feenox_read_arguments(argument, n_arguments, &arg, &n_chars_parse) == FEENOX_ERROR) {
        return NULL;
      }
      
      if (n_chars_count != n_chars_parse) {
        feenox_push_error_message("internal parser mismatch");
        return NULL;
      }

      // n is the number of characters to parse (it will get copied into the output item_t)
      // if it is a matrix, vector or variable we already summed the length
      n += ((matrix == NULL && vector == NULL && var == NULL) ? strlen(token) : 0) + n_chars_count;
      
      int n_arguments_max;

      if (vector != NULL) {

        if (n_arguments < 1 || n_arguments > 1) {
          feenox_push_error_message("vector '%s' takes exactly one subindex expression", vector->name);
          return NULL;
        }
        n_arguments_max = 1;

      } else if (matrix != NULL) {

        if (n_arguments < 2 || n_arguments > 2) {
          feenox_push_error_message("matrix '%s' takes exactly two subindex expressions", matrix->name);
          return NULL;
        }
        n_arguments_max = 2;

      } else if (builtin_function != NULL) {

        item->type = EXPR_BUILTIN_FUNCTION;
        item->builtin_function = builtin_function;

        if (n_arguments < item->builtin_function->min_arguments) {
          feenox_push_error_message("function '%s' takes at least %d argument%s instead of %d", token, item->builtin_function->min_arguments, (item->builtin_function->min_arguments==1)?"":"s", n_arguments);
          return NULL;
        }
        if (n_arguments > item->builtin_function->max_arguments) {
          feenox_push_error_message("function '%s' takes at most %d argument%s instead of %d", token, item->builtin_function->max_arguments, (item->builtin_function->max_arguments==1)?"":"s", n_arguments);
          return NULL;
        }

        n_arguments_max = item->builtin_function->max_arguments;
        
      } else if (builtin_vectorfunction != NULL) {

        // tenemos una funcion sobre vectores interna
        item->type = EXPR_BUILTIN_VECTORFUNCTION;
        item->builtin_vectorfunction = builtin_vectorfunction;

        if (n_arguments < item->builtin_vectorfunction->min_arguments) {
          feenox_push_error_message("function '%s' takes at least %d argument%s instead of %d", token, item->builtin_vectorfunction->min_arguments, (item->builtin_vectorfunction->min_arguments==1)?"":"s", n_arguments);
          return NULL;
        }
        if (n_arguments > item->builtin_vectorfunction->max_arguments) {
          feenox_push_error_message("function '%s' takes at most %d argument%s instead of %d", token, item->builtin_vectorfunction->max_arguments, (item->builtin_vectorfunction->max_arguments==1)?"":"s", n_arguments);
          return NULL;
        }

        n_arguments_max = item->builtin_vectorfunction->max_arguments;

      } else if (builtin_functional != NULL) {

        item->type = EXPR_BUILTIN_FUNCTIONAL;
        item->builtin_functional = builtin_functional;

        if (n_arguments < item->builtin_functional->min_arguments) {
          feenox_push_error_message("functional '%s' takes at least %d argument%s instead of %d", token, item->builtin_functional->min_arguments, (item->builtin_functional->min_arguments==1)?"":"s", n_arguments);
          return NULL;
        }
        if (n_arguments > item->builtin_functional->max_arguments) {
          feenox_push_error_message("functional '%s' takes at most %d argument%s instead of %d", token, item->builtin_functional->max_arguments, (item->builtin_functional->max_arguments==1)?"":"s", n_arguments);
          return NULL;
        }

        n_arguments_max = item->builtin_functional->max_arguments;

      } else if (function != NULL) {

        item->type = EXPR_FUNCTION;
        item->function = function;

        if (n_arguments != item->function->n_arguments) {
          feenox_push_error_message("function '%s' takes exactly %d argument%s instead of %d", token, item->function->n_arguments, (item->function->n_arguments==1)?"":"s", n_arguments);
          return NULL;
        }

        n_arguments_max = item->function->n_arguments;

        // mark that this item depends on function
        function_ll_t *function_item = NULL;
        feenox_check_alloc_null(function_item = calloc(1, sizeof(function_ll_t)));
        function_item->function = function;
        LL_APPEND(item->functions, function_item);
        
        if (function->algebraic_expression.items != NULL) {
          feenox_pull_dependencies_variables(&item->variables, function->algebraic_expression.variables);
          feenox_pull_dependencies_functions(&item->functions, function->algebraic_expression.functions);
        }  
        
        // if it is a gradient of something, tell the PDE that it has to compute them
        feenox.pde.compute_gradients |= function->is_gradient;

      } else {
        return NULL;
      }

      if (item->type != EXPR_BUILTIN_VECTORFUNCTION) {
        feenox_check_alloc_null(item->arg = calloc(n_arguments_max, sizeof(expr_t)));
      } else {
        feenox_check_alloc_null(item->vector_arg = calloc(n_arguments_max, sizeof(vector_t *)));
      }


      unsigned int i = 0;
      for (i = 0; i < n_arguments; i++) {
        if (item->type == EXPR_BUILTIN_VECTORFUNCTION) {
          feenox_call_null(feenox_strip_blanks(arg[i]));
          if ((item->vector_arg[i] = feenox_get_vector_ptr(arg[i])) == NULL) {
            feenox_push_error_message("undefined vector '%s'", arg[i]);
            return NULL;
          }
        } else if (item->type == EXPR_BUILTIN_FUNCTIONAL && i == 1) {
          // if it is a functional the second argument is a variable
          feenox_call_null(feenox_strip_blanks(arg[i]));
          if ((item->functional_var_arg = feenox_get_variable_ptr(arg[i])) == NULL) {
            return NULL;
          }
        } else {
          if (feenox_expression_parse(&item->arg[i], arg[i]) != FEENOX_OK) {
            return NULL;
          }
          
          feenox_pull_dependencies_variables(&item->variables, item->arg[i].variables);
          feenox_pull_dependencies_functions(&item->functions, item->arg[i].functions);
        }
      }
      
      feenox_free(argument);
      
    } 

    if (item->type == EXPR_UNDEFINED) {
      feenox_push_error_message("unknown symbol '%s'", token);
      return NULL;
    }
  }

  feenox_free(backup);
  item->n_chars = n;

  return item;

}



double feenox_expression_eval(expr_t *this) {

  if (this == NULL || this->items == NULL) {
    return 0;
  }

  size_t i = 0;
  size_t j = 0;
  expr_item_t *item = NULL;
  
//#define DEBUG
#ifdef DEBUG
  printf("\n------ %s -----------\n", this->string);
#endif  
  LL_FOREACH(this->items, item) {
#ifdef DEBUG
    printf("factor level %ld\n", item->level);
#endif  
    item->tmp_level = item->level;

    // TODO: replace the switch by pointer to functions (i.e. virtual methods in C++ slang)?
    switch(item->type & EXPR_BASICTYPE_MASK) {
      case EXPR_CONSTANT:
        item->value = item->constant;
#ifdef DEBUG
        printf("constant = %g\n", item->value);
#endif  
      break;
        
      case EXPR_VARIABLE:
        switch (item->type) {
          case EXPR_VARIABLE | EXPR_CURRENT:
            item->value = feenox_var_value(item->variable);
          break;
          case EXPR_VARIABLE | EXPR_INITIAL_TRANSIENT:
            item->value = *(item->variable->initial_transient);
          break;
          case EXPR_VARIABLE | EXPR_INITIAL_STATIC:
            item->value = *(item->variable->initial_static);
          break;
        }
#ifdef DEBUG
        printf("variable %s = %g\n", item->variable->name, item->value);
#endif  

      break;
      case EXPR_VECTOR:
        
        // we need to initialize here so we have the size for the check that follows
        if (item->vector->initialized == 0) {
          if (feenox_vector_init(item->vector, 0) != FEENOX_OK) {
            feenox_push_error_message("initialization of vector '%s' failed", item->vector->name);
            feenox_runtime_error();
          }
        }
    
        i = lrint(feenox_expression_eval(&(item->arg[0])));
        if (i <= 0 || i > item->vector->size) {
          feenox_push_error_message("subindex %d out of range for vector %s", i, item->vector->name);
          feenox_runtime_error();
          return 0;
        }

        switch (item->type) {
          case EXPR_VECTOR | EXPR_CURRENT:
            item->value = gsl_vector_get(item->vector->value, i-1);
          break;
          case EXPR_VECTOR | EXPR_INITIAL_TRANSIENT:
            item->value = gsl_vector_get(item->vector->initial_transient, i-1);
          break;
          case EXPR_VECTOR | EXPR_INITIAL_STATIC:
            item->value = gsl_vector_get(item->vector->initial_static, i-1);
          break;
        }
      break;
      
      case EXPR_MATRIX:
        
        if (item->matrix->initialized == 0) {
          if (feenox_matrix_init(item->matrix) != FEENOX_OK) {
            feenox_push_error_message("initialization of vector '%s' failed", item->vector->name);
            feenox_runtime_error();
          }
        }
        
        i = lrint(feenox_expression_eval(&(item->arg[0])));
        if (i <= 0 || i > item->matrix->rows) {
          feenox_push_error_message("row subindex %d out of range for matrix %s", i, item->matrix->name);
          feenox_runtime_error();
          return 0;
        }
        j = lrint(feenox_expression_eval(&(item->arg[1])));
        if (j <= 0 || j > item->matrix->cols) {
          feenox_push_error_message("column subindex %d out of range for matrix %s", j, item->matrix->name);
          feenox_runtime_error();
          return 0;
        }

        switch (item->type) {
          case EXPR_MATRIX | EXPR_CURRENT:
            item->value = gsl_matrix_get(item->matrix->value, i-1, j-1);
          break;
          case EXPR_MATRIX | EXPR_INITIAL_TRANSIENT:
            item->value = gsl_matrix_get(item->matrix->initial_transient, i-1, j-1);
          break;
          case EXPR_MATRIX | EXPR_INITIAL_STATIC:
            item->value = gsl_matrix_get(item->matrix->initial_static, i-1, j-1);
          break;
        }
      break;

      case EXPR_BUILTIN_FUNCTION:
        item->value = item->builtin_function->routine(item);
#ifdef DEBUG
        printf("builtin function %s = %g\n", item->builtin_function->name, item->value);
#endif  
      break;
      case EXPR_BUILTIN_VECTORFUNCTION:
        item->value = item->builtin_vectorfunction->routine(item->vector_arg);
      break;
      case EXPR_BUILTIN_FUNCTIONAL:
        item->value = item->builtin_functional->routine(item, item->functional_var_arg);
      break;
      case EXPR_FUNCTION:
        item->value = feenox_factor_function_eval(item);
#ifdef DEBUG
        printf("user function %s = %g\n", item->function->name, item->value);
#endif  
      break;
 
#ifdef DEBUG
      default:
        printf("operator %ld\n", item->oper);
#endif  
    }
#ifdef DEBUG
    printf("\n");
#endif  
  }

  // get the highest level
  size_t level = 0;
  LL_FOREACH(this->items, item) {
    if (item->level > level) {
      level = item->level;
    }
  }

  char tmp_operator = 0;
  expr_item_t *E = NULL;
  expr_item_t *P = NULL;
  
  while (level > 0) {

    for (E = P = this->items; E != NULL; E->tmp_level != 0 && !E->oper ? P=E : NULL, E = E->next) {
      
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
            P->value += E->value;
          break;
          case '-':
            P->value -= E->value;
          break;
          case '*':
            P->value *= E->value;
          break;
          case '/':
            if (E->value == 0) {
              feenox_nan_error();
            }
            P->value /= E->value;
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

  if (gsl_isnan(this->items->value) || gsl_isinf(this->items->value)) {
//    feenox_push_error_message("in '%s'", this->string);
    feenox_nan_error();
  }


  return this->items->value;

}



// evaluates the expression contained in the string
double feenox_expression_evaluate_in_string(const char *string) {
  
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


// this is a wrapper to compute the derivative of an expression with respect to a function
double feenox_expression_derivative_wrt_function_gsl_function(double x, void *params) {
  struct feenox_expression_derivative_params *p = (struct feenox_expression_derivative_params *)params;
  
  p->function->dummy_for_derivatives = 1;
  p->function->dummy_for_derivatives_value = x;
  
  double y = feenox_expression_eval(p->expr);
  
  p->function->dummy_for_derivatives_value = 0;
  p->function->dummy_for_derivatives = 0;
  
  return y;
}

double feenox_expression_derivative_wrt_function(expr_t *expr, function_t *function, double x) {
  gsl_function F;
  struct feenox_expression_derivative_params p;
  F.function = &feenox_expression_derivative_wrt_function_gsl_function;
  p.expr = expr;
  p.function = function;
  F.params = &p;
  
  double result, abserr;
  gsl_deriv_central(&F, x, DEFAULT_DERIVATIVE_STEP, &result, &abserr); 
  
  return result;
}

// this is a wrapper to compute the derivative of an expression with respect to a function
double feenox_expression_derivative_wrt_variable_gsl_function(double x, void *params) {
  struct feenox_expression_derivative_params *p = (struct feenox_expression_derivative_params *)params;
  
  // we need to restore the previous value otherwise
  // if this is time we screw everything up
  double x_old = feenox_var_value(p->variable);
  feenox_var_value(p->variable) = x;
  
  double y = feenox_expression_eval(p->expr);
  
  feenox_var_value(p->variable) = x_old;
  
  return y;
}

double feenox_expression_derivative_wrt_variable(expr_t *expr, var_t *variable, double x) {
  gsl_function F;
  struct feenox_expression_derivative_params p;
  F.function = &feenox_expression_derivative_wrt_variable_gsl_function;
  p.expr = expr;
  p.variable = variable;
  F.params = &p;
  
  double result, abserr;
  gsl_deriv_central(&F, x, DEFAULT_DERIVATIVE_STEP, &result, &abserr); 
  
  return result;
}

// parsea el rango de indices 
int feenox_parse_range(char *string, const char left_delim, const char middle_delim, const char right_delim, expr_t *a, expr_t *b) {
  char *first_bracket;
  char *second_bracket;
  char *colon;

  if ((first_bracket = strchr(string, left_delim)) == NULL) {
    feenox_push_error_message("range '%s' does not start with '%c'", string, left_delim);
    return FEENOX_ERROR;
  }

  if ((second_bracket = strrchr(string, right_delim)) == NULL) {
    feenox_push_error_message("unmatched '%c' for range in '%s'", left_delim, string);
    return FEENOX_ERROR;
  }

  *second_bracket = '\0';

  if ((colon = strchr(string, middle_delim)) == NULL) {
    feenox_push_error_message("delimiter '%c' not found when giving range", middle_delim);
    return FEENOX_ERROR;
  }
  *colon = '\0';

  if (feenox_expression_parse(a, first_bracket+1) != 0) {
    feenox_push_error_message("in min range expression");
    return FEENOX_ERROR;
  }
  if (feenox_expression_parse(b, colon+1) != 0) {
    feenox_push_error_message("in max range expression");
    return FEENOX_ERROR;
  }
  
  *second_bracket = right_delim;;
  *colon = middle_delim;
          
  return FEENOX_OK;
}


int feenox_pull_dependencies_variables(var_ll_t **to, var_ll_t *from) {
  // pull down all the dependencies of the arguments
  var_ll_t *var_item_old = NULL;
  var_ll_t *var_item_new = NULL;
  LL_FOREACH(from, var_item_old) {
    feenox_check_alloc(var_item_new = calloc(1, sizeof(var_ll_t)));
    var_item_new->var = var_item_old->var;
    LL_APPEND((*to), var_item_new);
  }
  return FEENOX_OK;
}

int feenox_pull_dependencies_functions(function_ll_t **to, function_ll_t *from) {
  // pull down all the dependencies of the arguments
  function_ll_t *function_item_old = NULL;
  function_ll_t *function_item_new = NULL;
  LL_FOREACH(from, function_item_old) {
    feenox_check_alloc(function_item_new = calloc(1, sizeof(function_ll_t)));
    function_item_new->function = function_item_old->function;
    LL_APPEND((*to), function_item_new);
  }
  return FEENOX_OK;
}

int feenox_expression_depends_on_space(var_ll_t *variables) {
  int depends = 0;
  var_ll_t *item = NULL;
  LL_FOREACH(variables, item) {
    if (item->var != NULL) {
      depends |= (item->var == feenox.mesh.vars.x) ||
                 (item->var == feenox.mesh.vars.y) ||
                 (item->var == feenox.mesh.vars.z);
    }  
  }
      
  return depends;
}

int feenox_expression_depends_on_function(function_ll_t *functions, function_t *function) {
  int depends = 0;
  function_ll_t *item = NULL;
  LL_FOREACH(functions, item) {
    if (item->function != NULL) {
      depends |= (item->function == function);
    }  
  }
      
  return depends;
}
