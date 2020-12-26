/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox cleanup routines
 *
 *  Copyright (C) 2009--2018,2020 jeremy theler
 *
 *  This file is part of feenox.
 *
 *  feenox is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  feenox is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with feenox.  If not, see <http://www.gnu.org/licenses/>.
 *------------------- ------------  ----    --------  --     -       -         -
 */
#include <stdio.h>
#include <stdlib.h>
#include "feenox.h"

void feenox_polite_exit(int error) {

  feenox_var_value(feenox_special_var(done)) = 1;
  feenox_finalize();
  exit(error);

  return;
}

/*
void feenox_free_shm(void) {
  io_t *io;
  struct semaphore_t *semaphore;
  
  LL_FOREACH(feenox.ios, io) {
    if (io->shm_pointer_double != NULL) {
      feenox_free_shared_pointer(io->shm_pointer_double, io->shm_name, io->size*sizeof(double));
    }
    if (io->shm_pointer_float != NULL) {
      feenox_free_shared_pointer(io->shm_pointer_float, io->shm_name, io->size*sizeof(float));
    }
    if (io->shm_pointer_char != NULL) {
      feenox_free_shared_pointer(io->shm_pointer_char, io->shm_name, io->size*sizeof(char));
    }
    
    if (io->shm_name != NULL) {
      free(io->shm_name);
    }
  }
  
  LL_FOREACH(feenox.semaphores, semaphore) {
    feenox_free_semaphore(semaphore->pointer, semaphore->name);
  }
  
  return;
}

void feenox_free_files(void) {
  file_t *file, *tmp;
  int i;
  
  HASH_ITER(hh, feenox.files, file, tmp) {
    if (file->pointer != stdin && file->pointer != stdout) {
      feenox_instruction_close_file(file);
    }
    free(file->name);
    free(file->format);
    free(file->path);
    free(file->mode);
    for (i = 0; i < file->n_args; i++) {
      feenox_destroy_expression(&file->arg[i]);  
    }
    free(file->arg);
    
    HASH_DEL(feenox.files, file);
    free(file);
  }
  
  return;
}

void feenox_free_functions(void) {
  function_t *function = NULL;
  function_t *tmp = NULL;

  // no vale ir hacia adelante porque las funciones siguientes
  // pueden tener referencias a funciones anteriores, hay que
  // ir al vesre de atras para adelante  
  if ((function = feenox.functions) != NULL) {
    while (function->hh.next != NULL) {
      function = function->hh.next;
    }
  
    for (; function != NULL; function = tmp) {
      tmp = function->hh.prev;
      feenox_free_function(function);    
    }
  }

  return;
}

void feenox_free_function(function_t *function) {
  
  int i;
  
  if (function->algebraic_expression.n_tokens != 0) {
    feenox_destroy_expression(&function->algebraic_expression);
  }
  
  if (function->data_argument_alloced) {
    for (i = 0; i < function->n_arguments; i++) {
      free(function->data_argument[i]);
      function->data_argument[i] = NULL;
    }
    free(function->data_argument);
    function->data_argument = NULL;
  }
  
  if (function->data_value != NULL) {
    free(function->data_value);
    function->data_value = NULL;
  }
  
  free(function->name_in_mesh);
  free(function->data_file);
  free(function->column);
  
// este free es problematico
  if (function->var_argument_alloced) {
    free(function->var_argument);
  }
  
  if (function->interp != NULL) {
    gsl_interp_free(function->interp);
  }
  if (function->interp_accel != NULL) {
    gsl_interp_accel_free(function->interp_accel);    
  }
  free(function->name);
  
  HASH_DEL(feenox.functions, function);
  free(function);
 
  return;
}


void feenox_free_var(var_t *var) {
  
  alias_t *alias;
  int is_alias = 0;
  
  // si somos un alias no tenemos que desalocar nada
  LL_FOREACH(feenox.aliases, alias) {
    if (alias->new_variable == var) {
      is_alias = 1;
    }
  }
  
  if (is_alias == 0) {
    if (var->realloced == 0) {
      free(feenox_value_ptr(var));
    }
    free(var->initial_transient);
    free(var->initial_static);
    free(var->name);
  }

  HASH_DEL(feenox.vars, var);
  free(var);

  return;
}

void feenox_free_vars(void) {
  var_t *var, *tmp;
  
  HASH_ITER(hh, feenox.vars, var, tmp) {
    feenox_free_var(var);
  }

  return;
}

void feenox_free_vector(vector_t *vector) {
  
  if (vector->realloced == 0 && vector->initialized) {
    if (feenox_value_ptr(vector) != NULL) gsl_vector_free(feenox_value_ptr(vector));
    if (vector->initial_static != NULL) gsl_vector_free(vector->initial_static);
    if (vector->initial_transient != NULL) gsl_vector_free(vector->initial_transient);
  }
  
  if (vector->size_expr != NULL) {
    feenox_destroy_expression(vector->size_expr);  
  }
  free(vector->size_expr);
  vector->size_expr = NULL;
  free(vector->name);
  HASH_DEL(feenox.vectors, vector);
  free(vector);
  
  return;
}

void feenox_free_matrices(void) {
  matrix_t *matrix, *tmp;
  
  HASH_ITER(hh, feenox.matrices, matrix, tmp) {
    feenox_free_matrix(matrix);
  }
  
  return;
}


void feenox_free_matrix(matrix_t *matrix) {
  
  free(matrix->name);
  if (matrix->realloced == 0) {
    gsl_matrix_free(feenox_value_ptr(matrix));
    gsl_matrix_free(matrix->initial_static);
    gsl_matrix_free(matrix->initial_transient);
  }
  HASH_DEL(feenox.matrices, matrix);
  free(matrix);
  
  return;
}

void feenox_free_vectors(void) {
  vector_t *vector, *tmp;
  
  HASH_ITER(hh, feenox.vectors, vector, tmp) {
    feenox_free_vector(vector);
  }
  
  return;
}

void feenox_free_print_vectors(void) {
  print_vector_t *print_vector, *tmp;
  print_token_t *print_token, *tmp2;

  LL_FOREACH_SAFE(feenox.print_vectors, print_vector, tmp) {
    free(print_vector->format);
    free(print_vector->separator);
    LL_FOREACH_SAFE(print_vector->tokens, print_token, tmp2) {
      feenox_destroy_expression(&print_vector->tokens->expression);
      LL_DELETE(print_vector->tokens, print_token);
      free(print_token);
    }

    LL_DELETE(feenox.print_vectors, print_vector);
    free(print_vector);
  }
  
  return;
}

void feenox_free_print_functions(void) {
  print_function_t *print_function, *tmp;
  print_token_t *print_token, *tmp2;
  int i;

  LL_FOREACH_SAFE(feenox.print_functions, print_function, tmp) {
    free(print_function->format);
    free(print_function->separator);
    if (print_function->first_function != NULL) {
      for (i = 0; i < print_function->first_function->n_arguments; i++) {
        if (print_function->range.min != NULL) {
          feenox_destroy_expression(&print_function->range.min[i]);
        }
        if (print_function->range.max != NULL) {
          feenox_destroy_expression(&print_function->range.max[i]);
        }
        if (print_function->range.step != NULL) {
          feenox_destroy_expression(&print_function->range.step[i]);
        }
        if (print_function->range.nsteps != NULL) {
          feenox_destroy_expression(&print_function->range.nsteps[i]);
        }
      }
    }
    LL_FOREACH_SAFE(print_function->tokens, print_token, tmp2) {
      feenox_destroy_expression(&print_token->expression);
      free(print_token->text);
      LL_DELETE(print_function->tokens, print_token);
      free(print_token);
    }

    LL_DELETE(feenox.print_functions, print_function);
    free(print_function);
  }
  
  return;
}


void feenox_free_prints(void) {
  print_t *print, *tmp;
  print_token_t *print_token, *tmp2;

  LL_FOREACH_SAFE(feenox.prints, print, tmp) {
    free(print->separator);
    LL_FOREACH_SAFE(print->tokens, print_token, tmp2) {
			feenox_destroy_expression(&print_token->expression);
			free(print_token->text);
			free(print_token->format);
      LL_DELETE(print->tokens, print_token);
      free(print_token);
    }

    LL_DELETE(feenox.prints, print);
    free(print);
  }
  
  return;
}

void feenox_free_solves(void) {
  solve_t *solve, *tmp;
  int i;

  LL_FOREACH_SAFE(feenox.solves, solve, tmp) {
    free(solve->unknown);
    if (solve->residual != NULL) {
      for (i = 0; i < solve->n; i++) {
        feenox_destroy_expression(&solve->residual[i]);
      }
      free(solve->residual);
    }
    
    if (solve->guess != NULL) {
      for (i = 0; i < solve->n; i++) {
        feenox_destroy_expression(&solve->guess[i]);
      }
      free(solve->guess);
    }
    
    feenox_destroy_expression(&solve->epsabs);
    feenox_destroy_expression(&solve->epsrel);

    LL_DELETE(feenox.solves, solve);
    free(solve);
  }
  
  return;
}

void feenox_free_m4(void) {
  m4_t *m4, *tmp;
  m4_macro_t *m4_macro, *tmp2;

  LL_FOREACH_SAFE(feenox.m4s, m4, tmp) {
    LL_FOREACH_SAFE(m4->macros, m4_macro, tmp2) {
      free(m4_macro->name);
      free(m4_macro->print_token.format);
      feenox_destroy_expression(&m4_macro->print_token.expression);
      LL_DELETE(m4->macros, m4_macro);
      free(m4_macro);
    }
    
    LL_DELETE(feenox.m4s, m4);
    free(m4);
  }
  
  return;
}
void feenox_free_dae(void) {

#ifdef HAVE_IDA
  dae_t *dae, *tmp;
  
  if (feenox_dae.daes != NULL) {
    LL_FOREACH_SAFE(feenox_dae.daes, dae, tmp) {
      feenox_destroy_expression(&dae->residual);
      feenox_destroy_expression(&dae->expr_i_min);
      feenox_destroy_expression(&dae->expr_i_max);
      feenox_destroy_expression(&dae->expr_j_min);
      feenox_destroy_expression(&dae->expr_j_max);
      LL_DELETE(feenox_dae.daes, dae);
      free(dae);
    }
  }

  if (feenox_dae.system != NULL) {
    IDAFree(&feenox_dae.system);
    feenox_dae.system = NULL;
  }
    
  if (feenox_dae.x != NULL) {
    N_VDestroy_Serial(feenox_dae.x);
    feenox_dae.x = NULL;
  }
  if (feenox_dae.dxdt != NULL) {
    N_VDestroy_Serial(feenox_dae.dxdt);
    feenox_dae.dxdt = NULL;
  }
  if (feenox_dae.id != NULL) {
    N_VDestroy_Serial(feenox_dae.id);
    feenox_dae.id = NULL;
  }
  
#endif  
  return;
}

void feenox_free_assignments(void) {
  assignment_t *assignment, *tmp;

  LL_FOREACH_SAFE(feenox.assignments, assignment, tmp) {
    feenox_destroy_expression(&assignment->rhs);
    feenox_destroy_expression(&assignment->t_min);
    feenox_destroy_expression(&assignment->t_max);
    feenox_destroy_expression(&assignment->i_min);
    feenox_destroy_expression(&assignment->i_max);
    feenox_destroy_expression(&assignment->j_min);
    feenox_destroy_expression(&assignment->j_max);
    feenox_destroy_expression(&assignment->col);
    feenox_destroy_expression(&assignment->row);
    
    LL_DELETE(feenox.assignments, assignment);
    free(assignment);
  }
  
  return;
}

void feenox_free_instructions(void) {
  instruction_t *instruction, *tmp;

  LL_FOREACH_SAFE(feenox.instructions, instruction, tmp) {
    LL_DELETE(feenox.instructions, instruction);
    if (instruction->argument_alloced) {
      free(instruction->argument);
    }
    free(instruction);
  }
  
  return;
}

void feenox_destroy_expression(expr_t *expr) {
  
  int i, j;
  int nallocs = 0;
  
  if (expr == NULL) {
    return;
  }
  
  for (i = 0; i < expr->n_tokens; i++) {
    if (expr->token[i].arg != NULL) {
      switch (expr->token[i].type) {
        case EXPR_VECTOR:
          nallocs = 1;
        break;
        case EXPR_MATRIX:
          nallocs = 2;
        break;
        case EXPR_BUILTIN_FUNCTION:
          nallocs = expr->token[i].builtin_function->max_arguments;
        break;
        case EXPR_BUILTIN_FUNCTIONAL: 
          nallocs = expr->token[i].builtin_functional->max_arguments;
        break;
        case EXPR_FUNCTION:
          nallocs = expr->token[i].function->n_arguments;
        break;
        case EXPR_BUILTIN_VECTORFUNCTION:
          nallocs = expr->token[i].builtin_vectorfunction->max_arguments;
        break;
        default:
          nallocs = 0;
        break;
      }
      for (j = nallocs-1; j >= 0; j--) {
        feenox_destroy_expression(&expr->token[i].arg[j]);
      }
      free(expr->token[i].arg);
    }
    if (expr->token[i].vector != NULL || expr->token[i].builtin_vectorfunction != NULL) {
      free(expr->token[i].vector_arg);
    }
    if (expr->token[i].aux != NULL) {
      free(expr->token[i].aux);
    }
  }
  
  if (expr->n_tokens != 0) {
    free(expr->token);
  }
  
  if (expr->string != NULL) {
    free(expr->string);
    expr->string = NULL;
  }
  expr = NULL;  
  
  return;
  
}
*/

void feenox_finalize(void) {

  int i;

/*  
  if (feenox.min.n != 0) {
    if (feenox.min.guess != NULL) {
      for (j = feenox.min.n-1; j >= 0; j--) {
        feenox_destroy_expression(&feenox.min.guess[j]);
      }
      free(feenox.min.guess);
    }
    if (feenox.min.gradient != NULL) {
      for (j = feenox.min.n-1; j >= 0; j--) {
        feenox_destroy_expression(&feenox.min.gradient[j]);
      }
      free(feenox.min.gradient);
    }
    
    feenox_destroy_expression(&feenox.min.gradtol);
    feenox_destroy_expression(&feenox.min.tol);
    
    free(feenox.min.x);
  }
  
  if (feenox.fit.p != 0) {
    if (feenox.fit.guess != NULL) {
      for (j = feenox.fit.p-1; j >= 0; j--) {
        feenox_destroy_expression(&feenox.fit.guess[j]);
      }
      free(feenox.fit.guess);
    }

    feenox_destroy_expression(&feenox.fit.deltaepsabs);
    feenox_destroy_expression(&feenox.fit.deltaepsrel);

    free(feenox.fit.param);
    free(feenox.fit.sigma);
  }

  
  feenox_free_assignments();
  feenox_free_dae();
  feenox_free_prints();
  feenox_free_print_vectors();
  feenox_free_print_functions();
  feenox_free_solves();
  feenox_free_shm();
  feenox_free_instructions();
  feenox_free_functions();
  feenox_free_files();
  feenox_free_vars();
  feenox_free_vectors();
  feenox_free_matrices();  

  feenox_free_m4();
  
  free(feenox.error);
  free(feenox.line);
*/
  for (i = 0; i < feenox.argc_orig; i++) {
    free(feenox.argv_orig[i]);
  }  
  free(feenox.argv_orig);
//  free(feenox.main_input_dirname_aux);
  
  return;

}
