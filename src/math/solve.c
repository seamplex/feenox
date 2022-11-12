/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox multidimensional root finding instruction
 *
 *  Copyright (C) 2022 jeremy theler
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
#include "feenox.h"

#ifdef HAVE_GSL
int feenox_solve_f(const gsl_vector *x, void *params, gsl_vector *f);
int feenox_solve_print_state (solve_t *solve, const size_t iter, gsl_multiroot_fsolver *s);
#endif

int feenox_instruction_solve(void *arg) {
#ifdef HAVE_GSL
  solve_t *solve = (solve_t *)arg;
  gsl_multiroot_function f = {&feenox_solve_f, solve->n_unknowns, solve};  
  
  double epsabs = (solve->epsabs.items != NULL) ? feenox_expression_eval(&solve->epsabs) : DEFAULT_SOLVE_EPSABS;
  double epsrel = (solve->epsrel.items != NULL) ? feenox_expression_eval(&solve->epsrel) : DEFAULT_SOLVE_EPSREL;
  int maxiter = (solve->max_iter != 0) ? solve->max_iter : DEFAULT_SOLVE_MAX_ITER;
  
  // solution vector
  gsl_vector *x = NULL;
  feenox_check_alloc(x = gsl_vector_alloc(solve->n_unknowns));
  
  // initial guess
  size_t i = 0;
  for (i = 0; i < solve->n_unknowns; i++) {
    gsl_vector_set(x, i, feenox_var_value(solve->unknown[i]));
  }
  
  gsl_multiroot_fsolver *s = NULL;
  feenox_check_alloc(s = gsl_multiroot_fsolver_alloc(solve->type, solve->n_unknowns));
  feenox_call(gsl_multiroot_fsolver_set(s, &f, x));
  
  // loop
  int status = 0;
  int iter = 0;
  if (solve->verbose) {
    feenox_solve_print_state(solve, iter, s);
  }  
  
  do {
    iter++;
    status = gsl_multiroot_fsolver_iterate(s);

    if (solve->verbose) {
      feenox_solve_print_state(solve, iter, s);
    }  

    if (status) {
      break;
    }

     status = (epsrel == 0) ? gsl_multiroot_test_residual(gsl_multiroot_fsolver_f(s), epsabs)
                            : gsl_multiroot_test_delta(gsl_multiroot_fsolver_dx(s), gsl_multiroot_fsolver_root(s), epsabs, epsrel);
  } while (status == GSL_CONTINUE && iter < maxiter);

  if (solve->verbose) {
    printf ("status = %s\n", gsl_strerror (status));
  }
  
  // bring the solution back to the variables
  gsl_vector *solution = gsl_multiroot_fsolver_root(s);
  for (i = 0; i < solve->n_unknowns; i++) {
    feenox_var_value(solve->unknown[i]) = gsl_vector_get(solution, i);
  }

  // cleanup
  gsl_multiroot_fsolver_free(s);
  gsl_vector_free(x);
#endif
  
  return FEENOX_OK;
}

#ifdef HAVE_GSL
int feenox_solve_f(const gsl_vector *x, void *params, gsl_vector *f) {
  solve_t *solve = (solve_t *)params;

  // pass vector x to unknown variables
  double xi = 0;
  size_t i = 0;
  for (i = 0; i < solve->n_unknowns; i++) {
    xi = gsl_vector_get(x, i);
    if (!isnan(xi)) {
      feenox_var_value(solve->unknown[i]) = xi;
    } else {
      return GSL_EDOM;
    }
  }
  
  // evaluate the residuals and set f
  for (i = 0; i < solve->n_unknowns; i++) {
    xi = feenox_expression_eval(&solve->residual[i]);
    if (!isnan(xi)) {
      gsl_vector_set(f, i, xi);
    } else {
      return GSL_ERANGE;
    }
  }
  return GSL_SUCCESS;
  
}

int feenox_solve_print_state(solve_t *solve, const size_t iter, gsl_multiroot_fsolver *s)
{
  printf ("iter = %3lu x = ", iter);
  for (size_t i = 0; i < solve->n_unknowns; i++) {
    printf("% .3e ", gsl_vector_get(s->x, i));
  }
  printf(" f(x) = ");
  for (size_t i = 0; i < solve->n_unknowns; i++) {
    printf("% .3e ", gsl_vector_get(s->f, i));
  }
  printf("\n");
  
  return FEENOX_OK;
}
#endif