/*------------ -------------- -------- --- ----- ---   --       -            -
 *  wasora minimization functions
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

#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#ifndef _WASORA_H_
#include "wasora.h"
#endif


int wasora_min_run(void) {
  
  int i;

  gsl_vector *x = NULL;

  wasora_value(wasora_special_var(in_outer_initial)) = 1;
  wasora_value(wasora_special_var(step_inner)) = 1;
  wasora_call(wasora_standard_run());
  wasora_value(wasora_special_var(in_outer_initial)) = 0;

  // guess y step inicial
//  if (wasora.min.siman_Efunc == NULL) {
    x = gsl_vector_alloc(wasora.min.n);
    for (i = 0; i < wasora.min.n; i++) {
      gsl_vector_set(x, i, (wasora.min.guess != NULL) ? wasora_evaluate_expression(&wasora.min.guess[i]) : wasora_value(wasora.min.x[i]));
    }
//  }
 
  // llamamos a quien corresponda
  if (wasora.min.f_type != NULL) {
    wasora_min_multiminf(x);
  } else if (wasora.min.fdf_type != NULL) {
    wasora_min_multiminfdf(x);
//  } else if (wasora.min.siman_type != NULL) {
//    wasora_min_siman(x);
  }
  
  // listo! ya obtuvimos el resultado, lo ponemos en las variables
  // que nos pidieron
  for (i = 0; i < wasora.min.n; i++) {
    wasora_var(wasora.min.x[i]) = gsl_vector_get(x, i);
  }
  
  // volvemos a correr con done_opt = 1 y los parametros optimos por si queremos
  // imprimir algo o lo que sea
  wasora_var(wasora.special_vars.done_outer) = 1;
  wasora_value(wasora_special_var(step_inner)) += 1;
  wasora_call(wasora_standard_run());

/*  
  if (wasora.min.verbose) {
    printf ("# %s = %.5g\n", wasora.min.function->name, (wasora.min.f_type != NULL) ? f_s->fval : fdf_s->f);  
    for (i = 0; i < wasora.min.n; i++) {
      printf ("# %s\t= %.5g\n", wasora.min.x[i]->name, wasora.min.x[i]->current[0]);  
    }
  }  
 */
  
  if (x != NULL) {
    gsl_vector_free(x);
  }

  return WASORA_RUNTIME_OK;
  
}


void wasora_min_read_params_from_solver(const gsl_vector *x) {

  int i;
  
  for (i = 0; i < wasora.min.n; i++) {
    wasora_var(wasora.min.x[i]) = gsl_vector_get(x, i);
  }
  
  return;
}


double wasora_min_compute_f(const double *x) {

  double y;
 
  if (wasora.min.norerun == 0) {
    // llamamos a la rutina principal
    wasora_value(wasora_special_var(step_inner)) += 1;
    if (wasora_standard_run() != WASORA_RUNTIME_OK) {
      wasora_runtime_error();
      return 0;
    }
  }

  y = wasora_evaluate_function(wasora.min.function, x);

  return y;
}

void wasora_min_compute_analytical_df(const double *x, gsl_vector *g) {

  int i;
  double y;

  // igualamos los argumentos de la funcion al vector x
  for (i = 0; i < wasora.min.n; i++) {
    wasora_value(wasora.min.function->var_argument[i]) = x[i];
  }

  // y calculamos las n expresiones analiticas que nos dieron
  for (i = 0; i < wasora.min.n; i++) {
    y = wasora_evaluate_expression(&wasora.min.gradient[i]);
    gsl_vector_set (g, i, y); 
  }
  
  return;
}

int wasora_min_compute_numerical_df(const double *x, gsl_vector *g) {
  
  int i;
  double yprime;
  double y_plus, y_minus;
  double *x_pert;
  
  x_pert = calloc(wasora.min.n, sizeof(double));


  for (i = 0; i < wasora.min.n; i++) {

    memcpy(x_pert, x, wasora.min.n*sizeof(double));

    // ponemos x+delta_x
    x_pert[i] = (1+DEFAULT_MINIMIZER_GRAD_H)*x[i];
    wasora_value(wasora.min.x[i]) = x_pert[i];

    if (wasora.min.norerun == 0) {
      wasora_value(wasora_special_var(step_inner)) += 1;
      wasora_call(wasora_standard_run());
    }

    y_plus = wasora_evaluate_function(wasora.min.function, x_pert);

    // ponemos x-delta_x
    x_pert[i] = (1-DEFAULT_MINIMIZER_GRAD_H)*x[i];
    wasora_value(wasora.min.x[i]) = x_pert[i];
    
    if (wasora.min.norerun == 0) {
      wasora_value(wasora_special_var(step_inner)) += 1;
      wasora_call(wasora_standard_run());
    }
    
    y_minus = wasora_evaluate_function(wasora.min.function, x_pert);

    yprime = (y_plus - y_minus)/(2*DEFAULT_MINIMIZER_GRAD_H*x[i]);
    gsl_vector_set(g, i, yprime); 
      
  }
  
  free(x_pert);

  return WASORA_RUNTIME_OK;
}



double wasora_gsl_min_f(const gsl_vector *x, void *params) {

  wasora_min_read_params_from_solver(x);
  return wasora_min_compute_f(gsl_vector_const_ptr(x, 0));
  
}


void wasora_gsl_min_df(const gsl_vector *x, void *params, gsl_vector *g) {
  
  wasora_min_read_params_from_solver(x);  

  if (wasora.min.gradient != NULL) {
    wasora_min_compute_analytical_df(gsl_vector_const_ptr(x, 0), g);
  } else {
    wasora_min_compute_numerical_df(gsl_vector_const_ptr(x, 0), g);
  }
  
  return;
}


void wasora_gsl_min_fdf(const gsl_vector *x, void *params, double *f, gsl_vector *g) {

  wasora_min_read_params_from_solver(x);  
  *f = wasora_min_compute_f(gsl_vector_const_ptr(x, 0));
  
  if (wasora.min.gradient != NULL) {
    wasora_min_compute_analytical_df(gsl_vector_const_ptr(x, 0), g);
  } else {
    wasora_min_compute_numerical_df(gsl_vector_const_ptr(x, 0), g);
  }

  return;
}


