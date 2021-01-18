/*------------ -------------- -------- --- ----- ---   --       -            -
 *  wasora function fitting functions
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

#include <stdio.h>

#ifndef _WASORA_H_
#include "wasora.h"
#endif

int wasora_fit_run(void) {
  
  int i;
  int iter = 0;
  double epsabs, epsrel;
  int gslstatus = GSL_SUCCESS;
  double c;
  
  gsl_multifit_fdfsolver *s = NULL;
  gsl_vector *param;
  gsl_multifit_function_fdf f;
  gsl_matrix *covar = gsl_matrix_alloc (wasora.fit.p, wasora.fit.p);
  gsl_matrix *J = gsl_matrix_alloc(wasora.fit.n, wasora.fit.p); 
    

  wasora_value(wasora_special_var(in_outer_initial)) = 1;
  wasora_value(wasora_special_var(step_outer)) = 0;
  wasora_value(wasora_special_var(step_inner)) = 0;
  wasora_call(wasora_standard_run());
  wasora_value(wasora_special_var(in_outer_initial)) = 0;
  
  // guess y step inicial
  param = gsl_vector_alloc(wasora.fit.p);
  for (i = 0; i < wasora.fit.p; i++) {
    gsl_vector_set(param, i, wasora_value(wasora.fit.param[i]));
  }
  
  f.f = &wasora_gsl_fit_f;
  f.df = &wasora_gsl_fit_df;
  f.fdf = &wasora_gsl_fit_fdf;
  f.n = wasora.fit.n;
  f.p = wasora.fit.p;
  f.params = NULL;
  
  s = gsl_multifit_fdfsolver_alloc(wasora.fit.algorithm, wasora.fit.n, wasora.fit.p);
  gsl_multifit_fdfsolver_set(s, &f, param);

  if (wasora.fit.verbose) {
    wasora_fit_print_state (iter, gslstatus, s);
  }

  do {
    wasora_value(wasora_special_var(step_inner)) = 0;  
    wasora_value(wasora_special_var(step_outer)) = (double)(++iter);
    
    gslstatus = gsl_multifit_fdfsolver_iterate(s);

    if (wasora.fit.verbose) {
      wasora_fit_print_state(iter, gslstatus, s);
    }

    if (gslstatus)
      break;

    // TODO: chequear convergencia por gradiente
    epsabs = (wasora.fit.deltaepsrel.n_tokens != 0)?wasora_evaluate_expression(&wasora.fit.deltaepsrel):DEFAULT_NLIN_FIT_EPSREL;
    epsrel = (wasora.fit.deltaepsabs.n_tokens != 0)?wasora_evaluate_expression(&wasora.fit.deltaepsabs):DEFAULT_NLIN_FIT_EPSABS;
    gslstatus = gsl_multifit_test_delta(s->dx, s->x, epsabs, epsrel);
  } while (gslstatus == GSL_CONTINUE && iter < wasora.fit.max_iter);  

 
// TODO: estudiar y entender que son estos errores
#if (GSL_MAJOR_VERSION < 2)  
  gsl_multifit_covar (s->J, 0.0, covar);
#else
  gsl_multifit_fdfsolver_jac(s, J);
  gsl_multifit_covar(J, 0.0, covar);
#endif
  
  // listo! ya obtuvimos el resultado, lo ponemos en los parametros
  // que nos pidieron y volvemos a ejecutar el main con done_outer = 1
  c = GSL_MAX_DBL(1, gsl_blas_dnrm2(s->f) / sqrt((double)(wasora.fit.n - wasora.fit.p))); 
  for (i = 0; i < wasora.fit.p; i++) {
    wasora_value(wasora.fit.param[i]) = gsl_vector_get(s->x, i);
    wasora_value(wasora.fit.sigma[i]) = c*sqrt(gsl_matrix_get(covar, i, i));
  }
  
  // volvemos a correr con done_outer = 1 y los parametros optimos por si queremos
  // imprimir algo o lo que sea
  wasora_value(wasora_special_var(step_inner)) = 0;  
  wasora_value(wasora_special_var(step_outer)) = (double)(++iter);
  wasora_value(wasora_special_var(done_outer)) = 1;
  wasora_call(wasora_standard_run());
  
  
  if (wasora.fit.verbose && wasora.rank == 0) {
    double chi = gsl_blas_dnrm2(s->f);
    double dof = wasora.fit.n - wasora.fit.p;
    printf("# chisq/dof = %g\n",  chi*chi / dof);

    for (i = 0; i < wasora.fit.p; i++) {
      printf ("# %s\t= %.5g # +/- %.5g\n", wasora.fit.param[i]->name, wasora_value(wasora.fit.param[i]),  wasora_value(wasora.fit.sigma[i]));
    }
  }

  gsl_multifit_fdfsolver_free(s);
  gsl_matrix_free(covar);
  gsl_matrix_free(J);
  gsl_vector_free(param);
  
  return WASORA_RUNTIME_OK;
  
}


void wasora_fit_read_params_from_solver(const gsl_vector *param) {

  int i;
  
  for (i = 0; i < wasora.fit.p; i++) {
    wasora_value(wasora.fit.param[i]) = gsl_vector_get(param, i);
  }
  
  return;
}


int wasora_fit_compute_f(gsl_vector *f) {

  int i, j, in_range;
  double *x;
  double *range_min = NULL;
  double *range_max = NULL;

  if (wasora.fit.norerun == 0) {
    // llamamos a la rutina principal
    wasora_value(wasora_special_var(step_inner)) += 1;    
    wasora_call(wasora_standard_run());
  }

  x = malloc(wasora.fit.data->n_arguments*sizeof(double));

  if (wasora.fit.range.min != NULL && wasora.fit.range.max != NULL) {
    range_min = malloc(wasora.fit.data->n_arguments*sizeof(double));
    range_max = malloc(wasora.fit.data->n_arguments*sizeof(double));
    
    for (i = 0; i < wasora.fit.data->n_arguments; i++) {
      range_min[i] = wasora_evaluate_expression(&wasora.fit.range.min[i]);
      range_max[i] = wasora_evaluate_expression(&wasora.fit.range.max[i]);
    }
  }
    
  for (j = 0; j < wasora.fit.n; j++) {
    in_range = 1;  // empezamos suponiendo que el punto esta dentro del rango
    for (i = 0; i < wasora.fit.data->n_arguments; i++) {
      x[i] = wasora.fit.data->data_argument[i][j];
      if (range_min != NULL && (x[i] < range_min[i] || x[i] > range_max[i])) {
        in_range = 0;
      }
    }
    if (in_range) {
      gsl_vector_set(f, j, wasora_evaluate_function(wasora.fit.function, x) - wasora.fit.data->data_value[j]);
    }
  }

  if (range_min != NULL) {
    free(range_min);
    free(range_max);
  }
    
  free(x);

  return WASORA_RUNTIME_OK;
}

void wasora_fit_compute_analytical_df(gsl_matrix *J) {

  int i, j, k, in_range;
  double *range_min = NULL;
  double *range_max = NULL;

  if (wasora.fit.range.min != NULL && wasora.fit.range.max != NULL) {
    range_min = malloc(wasora.fit.data->n_arguments*sizeof(double));
    range_max = malloc(wasora.fit.data->n_arguments*sizeof(double));
    
    for (i = 0; i < wasora.fit.data->n_arguments; i++) {
      range_min[i] = wasora_evaluate_expression(&wasora.fit.range.min[i]);
      range_max[i] = wasora_evaluate_expression(&wasora.fit.range.max[i]);
    }
  }

  for (j = 0; j < wasora.fit.n; j++) {
    in_range = 1; // suponemos que estamos en el rango
    for (i = 0; i < wasora.fit.data->n_arguments; i++) {
      wasora_value(wasora.fit.function->var_argument[i]) = wasora.fit.data->data_argument[i][j];
      if (range_min != NULL && (wasora.fit.data->data_argument[i][j] < range_min[i] || wasora.fit.data->data_argument[i][j] > range_max[i])) {
        in_range = 0;
      }
    }

    if (in_range) {
      for (k = 0; k < wasora.fit.p; k++) {
        gsl_matrix_set (J, j, k, wasora_evaluate_expression(&wasora.fit.gradient[k])); 
      }
    }      
  }
  
  return;
}

int wasora_fit_compute_numerical_df(gsl_matrix *J) {

  int i, j, k;
  int in_range;
  double *x;
  double *range_min = NULL;
  double *range_max = NULL;
  double yprime;
  double *y_plus, *y_minus;
  double orig_p;

  y_plus = calloc(wasora.fit.n, sizeof(double));
  y_minus = calloc(wasora.fit.n, sizeof(double));

  x = malloc(wasora.fit.data->n_arguments*sizeof(double));

  if (wasora.fit.range.min != NULL && wasora.fit.range.max != NULL) {
    range_min = malloc(wasora.fit.data->n_arguments*sizeof(double));
    range_max = malloc(wasora.fit.data->n_arguments*sizeof(double));
    
    for (i = 0; i < wasora.fit.data->n_arguments; i++) {
      range_min[i] = wasora_evaluate_expression(&wasora.fit.range.min[i]);
      range_max[i] = wasora_evaluate_expression(&wasora.fit.range.max[i]);
    }
  }
  
  for (k = 0; k < wasora.fit.p; k++) {
    
    // ponemos p + delta _p
    orig_p = wasora_value(wasora.fit.param[k]);
    if (fabs(orig_p) > wasora_var(wasora_special_var(zero))) {
      wasora_value(wasora.fit.param[k]) = (1+DEFAULT_NLIN_FIT_GRAD_H)*orig_p;
    } else {
      wasora_value(wasora.fit.param[k]) = +DEFAULT_NLIN_FIT_GRAD_H;
    }
    
    if (wasora.fit.norerun == 0) {
      wasora_value(wasora_special_var(step_inner)) += 1;      
      wasora_call(wasora_standard_run());
    }

    // y evaluamos para todos los tiempos
    for (j = 0; j < wasora.fit.n; j++) {
      in_range = 1;
      for (i = 0; i < wasora.fit.data->n_arguments; i++) {
        x[i] = wasora.fit.data->data_argument[i][j];
        if (range_min != NULL && (wasora.fit.data->data_argument[i][j] < range_min[i] || wasora.fit.data->data_argument[i][j] > range_max[i])) {
          in_range = 0;
        }
      }
      if (in_range) {
        y_plus[j] = wasora_evaluate_function(wasora.fit.function, x);
      }
    }

    // ponemos p - delta _p
    if (fabs(orig_p) > wasora_var(wasora_special_var(zero))) {
      wasora_value(wasora.fit.param[k]) = (1-DEFAULT_NLIN_FIT_GRAD_H)*orig_p;
    } else {
      wasora_value(wasora.fit.param[k]) = -DEFAULT_NLIN_FIT_GRAD_H;
    }
    
    if (wasora.fit.norerun == 0) {
      wasora_value(wasora_special_var(step_inner)) += 1;      
      wasora_call(wasora_standard_run());
    }

    for (j = 0; j < wasora.fit.n; j++) {
      in_range = 1;
      for (i = 0; i < wasora.fit.data->n_arguments; i++) {
        x[i] = wasora.fit.data->data_argument[i][j];
        if (range_min != NULL && (wasora.fit.data->data_argument[i][j] < range_min[i] || wasora.fit.data->data_argument[i][j] > range_max[i])) {
          in_range = 0;
        }
      }
      if (in_range) {
        y_minus[j] = wasora_evaluate_function(wasora.fit.function, x);
      }
    }

    // volvemosa poner p como estaba
    wasora_value(wasora.fit.param[k]) = orig_p;

    for (j = 0; j < wasora.fit.n; j++) {
      if (fabs(orig_p) > wasora_var(wasora_special_var(zero))) {
        yprime = (y_plus[j] - y_minus[j])/(2*DEFAULT_NLIN_FIT_GRAD_H*orig_p);
      } else {
        yprime = (y_plus[j] - y_minus[j])/(2*DEFAULT_NLIN_FIT_GRAD_H);
      }
  
      gsl_matrix_set (J, j, k, yprime); 
    }
      
  }

  if (range_min != NULL) {
    free(range_min);
    free(range_max);
  }
  
  free(x);
  free(y_minus);
  free(y_plus);

  return WASORA_RUNTIME_OK;
}


int wasora_gsl_fit_f(const gsl_vector *param, void *data, gsl_vector *f) {

  wasora_fit_read_params_from_solver(param);  
  wasora_fit_compute_f(f);

  return GSL_SUCCESS;
}


int wasora_gsl_fit_df(const gsl_vector *param, void *data, gsl_matrix *J) {

  wasora_fit_read_params_from_solver(param);  

  if (wasora.fit.gradient != NULL) {
    wasora_fit_compute_analytical_df(J);
  } else {
    wasora_fit_compute_numerical_df(J);
  }
  
  return GSL_SUCCESS;
}


int wasora_gsl_fit_fdf(const gsl_vector *param, void *data, gsl_vector *f, gsl_matrix *J) {

  wasora_fit_read_params_from_solver(param);  
  wasora_fit_compute_f(f);
  
  if (wasora.fit.gradient != NULL) {
    wasora_fit_compute_analytical_df(J);
  } else {
    wasora_fit_compute_numerical_df(J);
  }

  return GSL_SUCCESS;
}


void wasora_fit_print_state(int iter, int gslstatus, gsl_multifit_fdfsolver *s) {
  int i;
  
  printf ("fititeration: %d\t", iter);
  for (i = 0; i < wasora.fit.p; i++) {
    printf("% .3e ", gsl_vector_get(s->x, i));
  }
  printf("\tstatus = %s    |r|^2 = %g\n",  gsl_strerror(gslstatus), gsl_blas_dnrm2 (s->f));
  
  return;
}
