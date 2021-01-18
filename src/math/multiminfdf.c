/*------------ -------------- -------- --- ----- ---   --       -            -
 *  wasora minimization functions using GSL's methods with derivatives
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


void wasora_min_multiminfdf(gsl_vector *x) {

  int max_iters;
  int iter = 0;
  int gslstatus = GSL_SUCCESS;
  double tol, gradtol;

  gsl_multimin_fdfminimizer *fdf_s;
  gsl_multimin_function_fdf f;
  double ss;

  ss = (wasora.min.range.step != NULL) ? wasora_evaluate_expression(&wasora.min.range.step[0]) : DEFAULT_MINIMIZER_FDF_STEP;
     
  f.n = wasora.min.n;
  f.f = wasora_gsl_min_f;
  f.df = wasora_gsl_min_df;
  f.fdf = wasora_gsl_min_fdf;
  f.params = NULL;

  fdf_s = gsl_multimin_fdfminimizer_alloc(wasora.min.fdf_type, wasora.min.n);
  tol = (wasora.min.tol.n_tokens != 0) ? wasora_evaluate_expression(&wasora.min.tol) : DEFAULT_MINIMIZER_TOL;
  gsl_multimin_fdfminimizer_set(fdf_s, &f, x, ss, tol);
  wasora.min.solver_status = fdf_s->x;
  
  if (wasora.min.verbose && wasora.rank == 0) {
    wasora_multiminfdf_print_state(iter, fdf_s);
  }
  
  do {
    wasora_value(wasora_special_var(step_outer)) = (double)(++iter);
    wasora_value(wasora_special_var(step_inner)) = 0;  
    

    if ((gslstatus = gsl_multimin_fdfminimizer_iterate(fdf_s))) {
      break;
    }

    gradtol = (wasora.min.gradtol.n_tokens != 0) ? wasora_evaluate_expression(&wasora.min.gradtol) : DEFAULT_MINIMIZER_GRADTOL;
    max_iters = (wasora.min.max_iter.n_tokens != 0) ? wasora_evaluate_expression(&wasora.min.max_iter) : DEFAULT_MINIMIZER_MAX_ITER;
    gslstatus = gsl_multimin_test_gradient(fdf_s->gradient, gradtol);

    if (wasora.min.verbose) {
      wasora_multiminfdf_print_state(iter, fdf_s);
    }

  } while (gslstatus == GSL_CONTINUE && iter < max_iters);

  gsl_vector_memcpy(x, fdf_s->x);
  gsl_multimin_fdfminimizer_free(fdf_s);
     
  return;
  
}


void wasora_multiminfdf_print_state(int iter, gsl_multimin_fdfminimizer *fdf_s) {
  int i;


  printf ("# %d\t", iter);
  for (i = 0; i < wasora.min.n; i++) {
    printf("% .3e ", gsl_vector_get(wasora.min.solver_status, i));
  }
  
  printf("\t%s = %g\tgrad = %g\n", wasora.min.function->name, fdf_s->f, gsl_blas_dnrm2(fdf_s->gradient));
  
  fflush(stdout);

  return;
}
