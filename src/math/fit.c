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
 *  FeenoX is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with FeenoX.  If not, see <http://www.gnu.org/licenses/>.
 *------------------- ------------  ----    --------  --     -       -         -
 */
#include "feenox.h"

int feenox_fit_f(const gsl_vector *via, void *arg, gsl_vector *f);
int feenox_fit_df(const gsl_vector *via, void *arg, gsl_matrix *J);
int feenox_fit_in_range(fit_t *this);
void feenox_fit_update_x(fit_t *this, size_t j);
void feenox_fit_update_vias(fit_t *this, const gsl_vector *via);
void feenox_fit_print_state(const size_t iter, void *arg, const gsl_multifit_nlinear_workspace *w);


int feenox_instruction_fit(void *arg) {
  fit_t *fit = (fit_t *)arg;
  
  if (fit->n_data == 0) {
    feenox_call(feenox_function_init(fit->data));
    fit->n_data = fit->data->data_size;
  }
  if (fit->n_data < fit->n_via) {
    feenox_push_error_message("less data points than free paramters for the fit");
    return FEENOX_ERROR;
  }
  
  gsl_multifit_nlinear_fdf fdf;
  fdf.f = feenox_fit_f;
  fdf.df = feenox_fit_df;
  fdf.fvv = NULL;        // not using geodesic acceleration
  fdf.n = fit->n_data;
  fdf.p = fit->n_via;
  fdf.params = fit;

  // initial guess
  gsl_vector *via = NULL;
  feenox_check_alloc(via = gsl_vector_calloc(fit->n_via));
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
  
  // initialization
  const gsl_multifit_nlinear_type *T = gsl_multifit_nlinear_trust;
  gsl_multifit_nlinear_parameters fdf_params = gsl_multifit_nlinear_default_parameters();
  gsl_multifit_nlinear_workspace *w = NULL;
  feenox_check_alloc(w = gsl_multifit_nlinear_alloc(T, &fdf_params, fit->n_data, fit->n_via));
  gsl_multifit_nlinear_winit(via, NULL, &fdf, w);

  // compute initial cost function
  gsl_vector *f = gsl_multifit_nlinear_residual(w);
  double chisq0 = 0;
  gsl_blas_ddot(f, f, &chisq0);

  // solve the system
  int info = 0;
  int gsl_status = GSL_SUCCESS;
  gsl_status = gsl_multifit_nlinear_driver(fit->max_iter, DEFAULT_FIT_XTOL, DEFAULT_FIT_GTOL, DEFAULT_FIT_FTOL, feenox_fit_print_state, fit, &info, w);
  
  // compute covariance of best fit parameters
  gsl_matrix *J = gsl_multifit_nlinear_jac(w);
  gsl_matrix *covar = gsl_matrix_alloc(fit->n_via, fit->n_via);
  gsl_multifit_nlinear_covar(J, 0.0, covar);

  // compute final cost
  double chisq = 0;
  gsl_blas_ddot(f, f, &chisq);
  
  double dof = fit->n_data - fit->n_via;
//  double c = GSL_MAX_DBL(1, sqrt(chisq / dof));
  double c = (dof > 0) ? sqrt(chisq / dof) : 0;
  
  // got it, fill the variables with the results
  for (i = 0; i < fit->n_via; i++) {
    feenox_var_value(fit->via[i]) = gsl_vector_get(w->x, i);
    feenox_var_value(fit->sigma[i]) = c*sqrt(gsl_matrix_get(covar, i, i));
  }
  
  if (fit->verbose && feenox.rank == 0) {
    double dof = fit->n_data - fit->n_via;
    printf("# status = %s\n", gsl_strerror (gsl_status));  
    printf("# summary from method '%s/%s'\n", gsl_multifit_nlinear_name(w), gsl_multifit_nlinear_trs_name(w));
    printf("# number of iterations: %zu\n", gsl_multifit_nlinear_niter(w));
    printf("# reason for stopping: %s\n", (info == 1) ? "small step size" : "small gradient");
    printf("# variance of residuals  chisq/dof = %g\n",  chisq / dof);
    printf("# initial |f(x)| = %f\n", sqrt(chisq0));
    printf("# final   |f(x)| = %f\n", sqrt(chisq));

    for (i = 0; i < fit->n_via; i++) {
      printf ("# %s\t= %.5g # +/- %.5g\n", fit->via[i]->name, feenox_var_value(fit->via[i]), feenox_var_value(fit->sigma[i]));
    }
  }

  gsl_matrix_free(covar);
  gsl_multifit_nlinear_free(w);
  
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
    this->x[i] = feenox_vector_get(this->data->vector_argument[i], j);
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


int feenox_fit_f(const gsl_vector *via, void *arg, gsl_vector *f) {
  fit_t *this = (fit_t *)arg;
  
  feenox_fit_update_vias(this, via);  
  
  size_t j = 0;
  for (j = 0; j < this->n_data; j++) {
    feenox_fit_update_x(this, j);
    gsl_vector_set(f, j, feenox_fit_in_range(this) ? (feenox_function_eval(this->function, this->x) - feenox_vector_get(this->data->vector_value, j)) : 0);
  }
//  feenox_debug_print_gsl_vector(f, stdout);

  return GSL_SUCCESS;
}


int feenox_fit_df(const gsl_vector *via, void *arg, gsl_matrix *J) {
  fit_t *this = (fit_t *)arg;

  feenox_fit_update_vias(this, via);  
  
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
//  feenox_debug_print_gsl_matrix(J, stdout);
  
  return GSL_SUCCESS;
}

void feenox_fit_print_state(const size_t iter, void *arg, const gsl_multifit_nlinear_workspace *w) {
  fit_t *this = (fit_t *)arg;

  if (this->verbose) {
    gsl_vector *via = gsl_multifit_nlinear_position(w);
    gsl_vector *f = gsl_multifit_nlinear_residual(w);

    printf ("# iter %2zu\t", iter);
    unsigned int i = 0;
    for (i = 0; i < this->n_via; i++) {
      printf("%+e ", gsl_vector_get(via, i));
    }
    printf("\tres = %.4g\n", gsl_blas_dnrm2(f));
  }  
  
  return;
}
