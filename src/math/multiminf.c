/*------------ -------------- -------- --- ----- ---   --       -            -
 *  wasora minimization functions using GSL's methods without derivatives
 *
 *  Copyright (C) 2013 jeremy theler
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


void wasora_min_multiminf(gsl_vector *x) {
  
  int i;
  int max_iters;
  int iter = 0;
  int gslstatus = GSL_SUCCESS;
  double size = 0;
  double tol;

  gsl_multimin_fminimizer *f_s;
  gsl_multimin_function f;
  gsl_vector *ss;

  ss = gsl_vector_alloc(wasora.min.n);
  for (i = 0; i < wasora.min.n; i++) {
    gsl_vector_set(ss, i, (wasora.min.range.step != NULL) ? wasora_evaluate_expression(&wasora.min.range.step[i]) : DEFAULT_MINIMIZER_F_STEP);
  }
     
  f.n = wasora.min.n;
  f.f = wasora_gsl_min_f;
  f.params = NULL;

  f_s = gsl_multimin_fminimizer_alloc(wasora.min.f_type, wasora.min.n);
  gsl_multimin_fminimizer_set(f_s, &f, x, ss);
  wasora.min.solver_status = f_s->x;
    
  if (wasora.min.verbose && wasora.rank == 0) {
    wasora_multiminf_print_state(iter, f_s);
  }
  
  do {
    wasora_value(wasora_special_var(step_outer)) = (double)(++iter);
    wasora_value(wasora_special_var(step_inner)) = 0;    

    if ((gslstatus = gsl_multimin_fminimizer_iterate(f_s))) {
      break;
    }

    size = gsl_multimin_fminimizer_size(f_s);
    tol = (wasora.min.tol.n_tokens != 0) ? wasora_evaluate_expression(&wasora.min.tol) : DEFAULT_MINIMIZER_TOL;
    max_iters = (wasora.min.max_iter.n_tokens != 0) ? wasora_evaluate_expression(&wasora.min.max_iter) : DEFAULT_MINIMIZER_MAX_ITER;
    gslstatus = gsl_multimin_test_size(size, tol);

    if (wasora.min.verbose) {
      wasora_multiminf_print_state(iter, f_s);
    }
  } while (gslstatus == GSL_CONTINUE && iter < max_iters);

  gsl_vector_memcpy(x, f_s->x);
  
  gsl_vector_free(ss);
  gsl_multimin_fminimizer_free(f_s);
     
  return;
  
}


void wasora_multiminf_print_state(int iter, gsl_multimin_fminimizer *f_s) {
  int i;

  printf ("# %d\t", iter);
  for (i = 0; i < wasora.min.n; i++) {
    printf("% .3e ", gsl_vector_get(wasora.min.solver_status, i));
  }
  
  printf("\t%s = %g\tsize = %g\n", wasora.min.function->name, f_s->fval, gsl_multimin_fminimizer_size(f_s));
  
  fflush(stdout);

  return;
}
