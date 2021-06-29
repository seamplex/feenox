/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox function fitting functions
 *
 *  Copyright (C) 2009--2015,2021 jeremy theler
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
extern feenox_t feenox;

int feenox_instruction_fit(void *arg) {
  fit_t *fit = (fit_t *)arg;
  
  gsl_multifit_function_fdf f;
  f.f = &feenox_gsl_fit_f;
  f.df = &feenox_gsl_fit_df;
  f.fdf = &feenox_gsl_fit_fdf;
  
  if (fit->n_data == 0) {
    feenox_call(feenox_function_init(fit->data));
    fit->n_data = fit->data->data_size;
  }
  f.n = fit->n_data;
  f.p = fit->n_via;
  f.params = fit;

  // initial guess
  gsl_vector *via = NULL;
  feenox_check_alloc(via = gsl_vector_alloc(fit->n_via));
  unsigned int i = 0;
  for (i = 0; i < fit->n_via; i++) {
    gsl_vector_set(via, i, feenox_var_value(fit->via[i]));
  }
  
  feenox_check_alloc(fit->x = calloc(fit->data->n_arguments, sizeof(double)));
  
  if (fit->range.min != NULL && fit->range.max != NULL) {
    feenox_check_alloc(fit->range_min = calloc(fit->data->n_arguments, sizeof(double)));
    feenox_check_alloc(fit->range_max = calloc(fit->data->n_arguments, sizeof(double)));
    
    for (i = 0; i < fit->data->n_arguments; i++) {
      fit->range_min[i] = feenox_expression_eval(&fit->range.min[i]);
      fit->range_max[i] = feenox_expression_eval(&fit->range.max[i]);
    }
  }
  

  gsl_multifit_fdfsolver *s = NULL;
  feenox_check_alloc(s = gsl_multifit_fdfsolver_alloc(fit->algorithm, fit->n_data, fit->n_via));
  feenox_call(gsl_multifit_fdfsolver_set(s, &f, via));

  unsigned int iter = 0;
  int gsl_status = GSL_SUCCESS;
  if (fit->verbose) {
    feenox_fit_print_state (fit, iter, gsl_status, s);
  }

  double tol_abs = 0;
  double tol_rel = 0;
  do {
    gsl_status = gsl_multifit_fdfsolver_iterate(s);

    if (fit->verbose) {
      feenox_fit_print_state(fit, iter, gsl_status, s);
    }

    if (gsl_status)
      break;

    // TODO: check convergence due to gradient
    tol_abs = (fit->tol_rel.items != NULL) ? feenox_expression_eval(&fit->tol_rel) : DEFAULT_FIT_EPSREL;
    tol_rel = (fit->tol_abs.items != NULL) ? feenox_expression_eval(&fit->tol_rel) : DEFAULT_FIT_EPSABS;
    gsl_status = gsl_multifit_test_delta(s->dx, s->x, tol_abs, tol_rel);
    
  } while (gsl_status == GSL_CONTINUE && iter < fit->max_iter);  


  gsl_matrix *covar = NULL;
  feenox_check_alloc(covar = gsl_matrix_alloc (fit->n_via, fit->n_via));
  gsl_matrix *J = NULL;
  feenox_check_alloc(J = gsl_matrix_alloc(fit->n_data, fit->n_via)); 
  
// TODO: estudiar y entender que son estos errores
#if (GSL_MAJOR_VERSION < 2)  
  gsl_multifit_covar (s->J, 0.0, covar);
#else
  gsl_multifit_fdfsolver_jac(s, J);
  gsl_multifit_covar(J, 0.0, covar);
#endif
  
  // got it, fill the variables with the results
//  c = GSL_MAX_DBL(1, gsl_blas_dnrm2(s->f) / sqrt((double)(fit->n - fit->n_via))); 
  for (i = 0; i < fit->n_via; i++) {
    feenox_var_value(fit->via[i]) = gsl_vector_get(s->x, i);
//    feenox_var_value(fit->sigma[i]) = c*sqrt(gsl_matrix_get(covar, i, i));
  }
  
  if (fit->verbose && feenox.rank == 0) {
    double chi = gsl_blas_dnrm2(s->f);
    double dof = fit->n_data - fit->n_via;
    printf("# chisq/dof = %g\n",  chi*chi / dof);

    for (i = 0; i < fit->n_via; i++) {
//      printf ("# %s\t= %.5g # +/- %.5g\n", fit->via[i]->name, feenox_var_value(fit->via[i]),  feenox_var_value(fit->sigma[i]));
      printf ("# %s\t= %+e # +/- 0\n", fit->via[i]->name, feenox_var_value(fit->via[i]));
    }
  }

  gsl_matrix_free(covar);
  gsl_matrix_free(J);
  gsl_multifit_fdfsolver_free(s);
  if (fit->range_max == NULL) {
    feenox_free(fit->range_max)
  }      
  if (fit->range_min == NULL) {
    feenox_free(fit->range_min)
  }      
  feenox_free(fit->x)
  gsl_vector_free(via);
  
  return FEENOX_OK;
  
}

void feenox_fit_update_x(fit_t *this, size_t j) {
  unsigned int i = 0;
  for (i = 0; i < this->data->n_arguments; i++) {
    this->x[i] = this->data->data_argument[i][j];
    feenox_var_value(this->function->var_argument[i]) = this->x[i];
  }
  return;
}


void feenox_fit_update_vias(fit_t *this, const gsl_vector *via) {
  unsigned int i = 0;
  for (i = 0; i < this->n_via; i++) {
    feenox_var_value(this->via[i]) = gsl_vector_get(via, i);
  }
  return;
}

int feenox_fit_compute_f(fit_t *this, gsl_vector *f) {

  size_t j = 0;
  for (j = 0; j < this->n_data; j++) {
    feenox_fit_update_x(this, j);
    gsl_vector_set(f, j, feenox_fit_in_range(this) ? (feenox_function_eval(this->function, this->x) - this->data->data_value[j]) : 0);
  }

  return FEENOX_OK;
}

int feenox_fit_compute_df(fit_t *this, gsl_matrix *J) {

  unsigned int i = 0;
  size_t j = 0;  
  int in_range = 0;
  for (j = 0; j < this->n_data; j++) {
    feenox_fit_update_x(this, j);
    in_range = feenox_fit_in_range(this);
    
    for (i = 0; i < this->n_via; i++) {
      gsl_matrix_set (J, j, i, (in_range) ? ((this->gradient != NULL) ? feenox_expression_eval(&this->gradient[i]) : feenox_expression_derivative_wrt_variable(&this->function->algebraic_expression, this->via[i], feenox_var_value(this->via[i]))) : 0); 
    }      
  }
  
  return FEENOX_OK;
}

int feenox_fit_in_range(fit_t *this) {

  if (this->range_min == NULL || this->range_max == NULL) {
    return 1;
  }
  unsigned int i = 0;
  for (i = 0; i < this->data->n_arguments; i++) {
    if (this->x[i] < this->range_min[i] || this->x[i] > this->range_max[i]) {
      return 0;
    }
  }
  
  return 1;
}


int feenox_gsl_fit_f(const gsl_vector *via, void *arg, gsl_vector *f) {
  fit_t *this = (fit_t *)arg;
  
  feenox_fit_update_vias(this, via);  
  feenox_fit_compute_f(this, f);
//  feenox_debug_print_gsl_vector(f, stdout);

  return GSL_SUCCESS;
}


int feenox_gsl_fit_df(const gsl_vector *via, void *arg, gsl_matrix *J) {
  fit_t *this = (fit_t *)arg;

  feenox_fit_update_vias(this, via);  
  feenox_call(feenox_fit_compute_df(this, J));
//  feenox_debug_print_gsl_matrix(J, stdout);
  
  return GSL_SUCCESS;
}


int feenox_gsl_fit_fdf(const gsl_vector *param, void *arg, gsl_vector *f, gsl_matrix *J) {
  fit_t *this = (fit_t *)arg;

  feenox_fit_update_vias(this, param);  
  feenox_fit_compute_f(this, f);
  feenox_call(feenox_fit_compute_df(this, J));
  
  return GSL_SUCCESS;
}


void feenox_fit_print_state(fit_t *this, unsigned int iter, int gsl_status, gsl_multifit_fdfsolver *s) {
  printf ("ititeration: %d\t", iter);
  unsigned int i = 0;
  for (i = 0; i < this->n_via; i++) {
    printf("%+e ", gsl_vector_get(s->x, i));
  }
  printf("\tstatus = %s\t|r|^2 = %g\n",  gsl_strerror(gsl_status), gsl_blas_dnrm2 (s->f));
  
  return;
}
