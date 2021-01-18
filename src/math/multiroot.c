/*------------ -------------- -------- --- ----- ---   --       -            -
 *  wasora multidimensional roout finding GSL's methods
 *
 *  Copyright (C) 2015 jeremy theler
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


int wasora_instruction_solve(void *arg) {
  solve_t *solve = (solve_t *)arg;
  
  gsl_multiroot_fsolver *s;  
  
  int status;
  size_t i, iter = 0, maxiter;
  gsl_multiroot_function f = {&wasora_gsl_solve_f, solve->n, solve};  
  gsl_vector *x;
  double epsabs, epsrel;

  // leemos los deltas (epsrel = 0 indica que no hay que mirar deltas solo absoluto)
  epsabs = (solve->epsabs.n_tokens != 0) ? wasora_evaluate_expression(&solve->epsabs) : DEFAULT_SOLVE_EPSABS;
  epsrel = (solve->epsrel.n_tokens != 0) ? wasora_evaluate_expression(&solve->epsrel) : DEFAULT_SOLVE_EPSREL;
  maxiter = (solve->max_iter != 0) ? solve->max_iter : DEFAULT_SOLVE_MAX_ITER;
  
  // alocamos el vector solucion
  x = gsl_vector_alloc(solve->n);
  
  // le damos el guess inicial
  if (solve->guess != NULL) {
    // o bien explicitamente con GUESS
    for (i = 0; i < solve->n; i++) {
      gsl_vector_set(x, i, wasora_evaluate_expression(&solve->guess[i]));
    }
  } else {
    // o bien lo que tengan las incognitas al momento de ejecutar la instruccion
    for (i = 0; i < solve->n; i++) {
      gsl_vector_set(x, i, wasora_var_value(solve->unknown[i]));
    }
  }
  
  // seteamos cosas
  s = gsl_multiroot_fsolver_alloc(solve->type, solve->n);
  gsl_multiroot_fsolver_set(s, &f, x);  
  
  // loopeamos
  do {
    iter++;
    status = gsl_multiroot_fsolver_iterate(s);

    // TODO: verbose
//    print_state (iter, s);

    if (status) {
      break;
    }

     status = (epsrel == 0) ? gsl_multiroot_test_residual(gsl_multiroot_fsolver_f(s), epsabs)
                            : gsl_multiroot_test_delta(gsl_multiroot_fsolver_dx(s), gsl_multiroot_fsolver_root(s), epsabs, epsrel);
  } while (status == GSL_CONTINUE && iter < maxiter);

  
  // traemos la solucion desde x a las incognitas
  for (i = 0; i < solve->n; i++) {
    wasora_var_value(solve->unknown[i]) = gsl_vector_get(gsl_multiroot_fsolver_root(s), i);
  }

  // limpiamos
  gsl_multiroot_fsolver_free(s);
  gsl_vector_free(x);
  
  return status;
}

int wasora_gsl_solve_f(const gsl_vector *x, void *params, gsl_vector *f) {
  solve_t *solve = (solve_t *)params;
  double xi;
  int i;
  

  // pasamos el vector x a las incognitas
  for (i = 0; i < solve->n; i++) {
    xi = gsl_vector_get(x, i);
    if (!isnan(xi)) {
      wasora_var_value(solve->unknown[i]) = xi;
    } else {
      return GSL_EDOM;
    }
  }
  
  // evaluamos los residuos y los ponemos en f
  for (i = 0; i < solve->n; i++) {
    xi = wasora_evaluate_expression(&solve->residual[i]);
    if (!isnan(xi)) {
      gsl_vector_set(f, i, xi);
    } else {
      return GSL_ERANGE;
    }
  }
  return GSL_SUCCESS;
  
}
