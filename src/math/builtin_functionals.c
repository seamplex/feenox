/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox builtin functionals
 *
 *  Copyright (C) 2009--2020 jeremy theler
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

// from gsl/roots/root.h
#define SAFE_FUNC_CALL(f, x, yp) \
do { \
  *yp = GSL_FN_EVAL(f,x); \
  if (!gsl_finite(*yp)) \
    GSL_ERROR("function value is not finite", GSL_EBADFUNC); \
} while (0)


double feenox_builtin_derivative(struct expr_item_t *, struct var_t *);
double feenox_builtin_integral(struct expr_item_t *, struct var_t *);
double feenox_builtin_simpson(struct expr_item_t *, struct var_t *);
double feenox_builtin_gauss_kronrod(struct expr_item_t *, struct var_t *);
double feenox_builtin_gauss_legendre(struct expr_item_t *, struct var_t *);
double feenox_builtin_prod(struct expr_item_t *, struct var_t *);
double feenox_builtin_sum(struct expr_item_t *, struct var_t *);
double feenox_builtin_root(struct expr_item_t *, struct var_t *);
double feenox_builtin_func_min(struct expr_item_t *, struct var_t *);


struct builtin_functional_t builtin_functional[N_BUILTIN_FUNCTIONALS] = {
    {"derivative",          3, 5, &feenox_builtin_derivative},
    {"integral",            4, 7, &feenox_builtin_integral},
    {"gauss_kronrod",       4, 5, &feenox_builtin_gauss_kronrod},
    {"gauss_legendre",      4, 5, &feenox_builtin_gauss_legendre},
    {"prod",                4, 4, &feenox_builtin_prod},
    {"sum",                 4, 4, &feenox_builtin_sum},
    {"root",                4, 7, &feenox_builtin_root},
    {"func_min",            4, 8, &feenox_builtin_func_min},
};


typedef struct {
  expr_item_t *function;
  var_t *variable;
} gsl_function_arguments_t;


///fu+derivative+name derivative
///fu+derivative+usage derivative(f(x), x, a, [h], [p])
///fu+derivative+desc Computes the derivative of the expression $f(x)$ 
///fu+derivative+desc given in the first argument with respect to the variable $x$
///fu+derivative+desc given in the second argument at the point $x=a$ given in
///fu+derivative+desc the third argument using an adaptive scheme.
///fu+derivative+desc The fourth optional argument $h$ is the initial width
///fu+derivative+desc of the range the adaptive derivation method starts with. 
///fu+derivative+desc The fifth optional argument $p$ is a flag that indicates
///fu+derivative+desc whether a backward ($p < 0$), centered ($p = 0$) or forward ($p > 0$)
///fu+derivative+desc stencil is to be used.
///fu+derivative+desc This functional calls the GSL functions
///fu+derivative+desc`gsl_deriv_backward`, `gsl_deriv_central` or `gsl_deriv_forward`
///fu+derivative+desc according to the indicated flag $p$.
///fu+derivative+desc Defaults are $h = (1/2)^{-10} \approx 9.8 \times 10^{-4}$ and $p = 0$.
///fu+derivative+math \left. \frac{d}{dx} \Big[ f(x) \Big] \right|_{x = a} 
///fu+derivative+example derivative.was
double feenox_builtin_derivative(expr_item_t *a, var_t *var_x) {

  double error;

  double x, x_old;
  double h;
  double result;

  gsl_function function_to_derive;
  gsl_function_arguments_t function_arguments;

  // nos acordamos cuanto vale x para despues volver a dejarle ese valor
  x_old = feenox_var_value(var_x);

  x = feenox_expression_eval(&a->arg[2]);
  if ((h = feenox_expression_eval(&a->arg[3])) == 0) {
    h = DEFAULT_DERIVATIVE_STEP;
  }

  function_arguments.function = a;
  function_arguments.variable = var_x;

  function_to_derive.function = feenox_gsl_function;
  function_to_derive.params = (void *)(&function_arguments);


  if (feenox_expression_eval(&a->arg[4]) == 0) {
    gsl_deriv_central(&function_to_derive, x, h, &result, &error);
  } else if (feenox_expression_eval(&a->arg[4]) > 0) {
    gsl_deriv_forward(&function_to_derive, x, h, &result, &error);
  } else {
    gsl_deriv_backward(&function_to_derive, x, h, &result, &error);
  }


  // le volvemos a poner el valor de x que tenia antes
  feenox_var_value(var_x) = x_old;


  return result;
}

///fu+integral+name integral
///fu+integral+usage integral(f(x), x, a, b, [eps], [k], [max_subdivisions])
///fu+integral+desc Computes the integral of the expression $f(x)$
///fu+integral+desc given in the first argument with respect to variable $x$
///fu+integral+desc given in the second argument over the interval $[a,b]$ given
///fu+integral+desc in the third and fourth arguments respectively using an adaptive
///fu+integral+desc scheme, in which the domain is divided into a number of maximum number
///fu+integral+desc of subintervals
///fu+integral+desc and a fixed-point Gauss-Kronrod-Patterson scheme is applied to each
///fu+integral+desc quadrature subinterval. Based on an estimation of the error commited,
///fu+integral+desc one or more of these subintervals may be split to repeat
///fu+integral+desc the numerical integration alogorithm with a refined division.
///fu+integral+desc The fifth optional argument $\epsilon$ is is a relative tolerance
///fu+integral+desc used to check for convergence. It correspondes to GSL's `epsrel` 
///fu+integral+desc parameter (`epsabs` is set to zero). 
///fu+integral+desc The sixth optional argument $1\leq k \le 6$ is an integer key that
///fu+integral+desc indicates the integration rule to apply in each interval.
///fu+integral+desc It corresponds to GSL's parameter `key`. 
///fu+integral+desc The seventh optional argument gives the maximum number of subdivisions,
///fu+integral+desc which defaults to 1024.
///fu+integral+desc If the integration interval $[a,b]$ if finite, this functional calls
///fu+integral+desc the GSL function `gsl_integration_qag`. If $a$ is less that minus the
///fu+integral+desc internal variable `infinite`, $b$ is greater that `infinite` or both
///fu+integral+desc conditions hold, GSL functions `gsl_integration_qagil`, 
///fu+integral+desc `gsl_integration_qagiu` or `gsl_integration_qagi` are called.
///fu+integral+desc The condition of finiteness of a fixed range $[a,b]$ can thus be 
///fu+integral+desc changed by modifying the internal variable `infinite`.
///fu+integral+desc Defaults are $\epsilon = (1/2)^{-10} \approx 10^{-3}$ and $k=3$.
///fu+integral+desc The maximum numbers of subintervals is limited to 1024.
///fu+integral+desc Due to the adaptivity nature of the integration method, this function
///fu+integral+desc gives good results with arbitrary integrands, even for
///fu+integral+desc infinite and semi-infinite integration ranges. However, for certain
///fu+integral+desc integrands, the adaptive algorithm may be too expensive or even fail
///fu+integral+desc to converge. In these cases, non-adaptive quadrature functionals ought to
///fu+integral+desc be used instead.
///fu+integral+desc See GSL reference for further information.
///fu+integral+math \int_a^b f(x) \, dx  
///fu+integral+example integral1.was integral2.was integral3.was
double feenox_builtin_integral(expr_item_t *a, var_t *var_x) {
  double x_old;
  double x_lower;
  double x_upper;
  double epsrel;
  double result;

  // TODO: ver como informar al usuario estos valores
  double error;
  int intervals;
  int pointskey;

  gsl_integration_workspace *w;
  gsl_function function_to_integrate;
  gsl_function_arguments_t function_arguments;

  
  // nos acordamos cuanto vale x para despues volver a dejarle ese valor
  x_old = feenox_var_value(var_x);

  x_lower = feenox_expression_eval(&a->arg[2]);
  x_upper = feenox_expression_eval(&a->arg[3]);
  
  if ((epsrel = feenox_expression_eval(&a->arg[4])) == 0) {
    epsrel = DEFAULT_INTEGRATION_TOLERANCE;
  }

  if ((pointskey = (int)feenox_expression_eval(&a->arg[5])) == 0) {
    pointskey = DEFAULT_INTEGRATION_KEY;
  }
  
  if ((intervals = (int)feenox_expression_eval(&a->arg[6])) == 0) {
    intervals = DEFAULT_INTEGRATION_INTERVALS;
  }
  
  
  function_arguments.function = a;
  function_arguments.variable = var_x;

  function_to_integrate.function = feenox_gsl_function;
  function_to_integrate.params = (void *)(&function_arguments);

  // TODO: poner en aux para no tener que hacer alloc cada vez
  w = gsl_integration_workspace_alloc(intervals);

  if (x_lower < -0.9*feenox_special_var_value(infinite) && x_upper > +0.9*feenox_special_var_value(infinite)) {
    gsl_integration_qagi(&function_to_integrate, 1e-8, epsrel, intervals, w, &result, &error);
  } else if (x_lower < -0.9*feenox_special_var_value(infinite)) {
    gsl_integration_qagil(&function_to_integrate, x_upper, 1e-8, epsrel, intervals, w, &result, &error);
  } else if (x_upper > +0.9*feenox_special_var_value(infinite)) {
    gsl_integration_qagiu(&function_to_integrate, x_lower, 1e-8, epsrel, intervals, w, &result, &error);
  } else {
    gsl_integration_qag(&function_to_integrate, x_lower, x_upper, 1e-8, epsrel, intervals, pointskey, w, &result, &error);
  }


  // le volvemos a poner el valor de x que tenia antes
  feenox_var_value(var_x) = x_old;

  gsl_integration_workspace_free(w);

  return result;
}

///fu+gauss_kronrod+name gauss_kronrod
///fu+gauss_kronrod+usage gauss_kronrod(f(x), x, a, b, [eps])
///fu+gauss_kronrod+desc Computes the integral of the expression $f(x)$
///fu+gauss_kronrod+desc given in the first argument with respect to variable $x$
///fu+gauss_kronrod+desc given in the second argument over the interval $[a,b]$ given
///fu+gauss_kronrod+desc in the third and fourth arguments respectively using a
///fu+gauss_kronrod+desc non-adaptive procedure which uses fixed Gauss-Kronrod-Patterson
///fu+gauss_kronrod+desc abscissae to sample the integrand at a maximum of 87 points.
///fu+gauss_kronrod+desc It is provided for fast integration of smooth functions.
///fu+gauss_kronrod+desc The algorithm applies the Gauss-Kronrod 10-point, 21-point,
///fu+gauss_kronrod+desc 43-point and 87-point integration rules in succession until an
///fu+gauss_kronrod+desc estimate of the integral is achieved within the relative tolerance
///fu+gauss_kronrod+desc given in the fifth optional argument $\epsilon$
///fu+gauss_kronrod+desc It correspondes to GSL's `epsrel` parameter (`epsabs` is set to zero).  
///fu+gauss_kronrod+desc The rules are designed in such a way that each rule uses all
///fu+gauss_kronrod+desc the results of its predecessors, in order to minimize the total
///fu+gauss_kronrod+desc number of function evaluations.
///fu+gauss_kronrod+desc Defaults are $\epsilon = (1/2)^{-10} \approx 10^{-3}$.
///fu+gauss_kronrod+desc See GSL reference for further information.
///fu+gauss_kronrod+math \int_a^b f(x) \, dx  
///fu+gauss_kronrod+examples integral3.was

double feenox_builtin_gauss_kronrod(expr_item_t *a, var_t *var_x) {
  double error;

  double x_old;
  double x_lower;
  double x_upper;
  double epsrel;
  double result;

  size_t neval;

  gsl_function function_to_integrate;
  gsl_function_arguments_t function_arguments;

  
  // nos acordamos cuanto vale x para despues volver a dejarle ese valor
  x_old = feenox_var_value(var_x);

  x_lower = feenox_expression_eval(&a->arg[2]);
  x_upper = feenox_expression_eval(&a->arg[3]);
  if ((epsrel = feenox_expression_eval(&a->arg[4])) == 0) {
    epsrel = DEFAULT_INTEGRATION_TOLERANCE;
  }

  function_arguments.function = a;
  function_arguments.variable = var_x;

  function_to_integrate.function = feenox_gsl_function;
  function_to_integrate.params = (void *)(&function_arguments);

  gsl_integration_qng(&function_to_integrate, x_lower, x_upper, 0, epsrel, &result, &error, &neval);
  
  // le volvemos a poner el valor de x que tenia antes
  feenox_var_value(var_x) = x_old;


  return result;
}

///fu+gauss_legendre+name gauss_legendre
///fu+gauss_legendre+usage gauss_legendre(f(x), x, a, b, [n])
///fu+gauss_legendre+desc Computes the integral of the expression $f(x)$
///fu+gauss_legendre+desc given in the first argument with respect to variable $x$
///fu+gauss_legendre+desc given in the second argument over the interval $[a,b]$ given
///fu+gauss_legendre+desc in the third and fourth arguments respectively using the
///fu+gauss_legendre+desc $n$-point Gauss-Legendre rule, where $n$ is given in the
///fu+gauss_legendre+desc optional fourth argument. 
///fu+gauss_legendre+desc It is provided for fast integration of smooth functions with
///fu+gauss_legendre+desc known polynomic order (it is exact for polynomials of order
///fu+gauss_legendre+desc $2n-1$).
///fu+gauss_legendre+desc This functional calls GSL function `gsl_integration_glfixedp`.
///fu+gauss_legendre+desc Default is $n = 12$.
///fu+gauss_legendre+desc See GSL reference for further information.
///fu+gauss_legendre+math \int_a^b f(x) \, dx  
///fu+gauss_legendre+examples integral3.was
double feenox_builtin_gauss_legendre(expr_item_t *a, var_t *var_x) {

#ifdef HAVE_GLFIXED_TABLE
  double x_old;
  double x_lower;
  double x_upper;
  double result;

  size_t n;

  gsl_function function_to_integrate;
  gsl_function_arguments_t function_arguments;

  gsl_integration_glfixed_table *points;

  // nos acordamos cuanto vale x para despues volver a dejarle ese valor
  x_old = feenox_var_value(var_x);

  x_lower = feenox_expression_eval(&a->arg[2]);
  x_upper = feenox_expression_eval(&a->arg[3]);
  if ((n = (int)feenox_expression_eval(&a->arg[4])) == 0) {
    n = 12;
  }

  function_arguments.function = a;
  function_arguments.variable = var_x;

  function_to_integrate.function = feenox_gsl_function;
  function_to_integrate.params = (void *)(&function_arguments);

  // TODO: poner en aux para no tener que hacer alloc cada vez
  points = gsl_integration_glfixed_table_alloc(n);
  result = gsl_integration_glfixed(&function_to_integrate, x_lower, x_upper, points);
  gsl_integration_glfixed_table_free(points);
  
  // le volvemos a poner el valor de x que tenia antes
  feenox_var_value(var_x) = x_old;

  return result;
  
#else

  feenox_push_error_message("the GSL version used to compile feenox does not have gsl_integration_glfixed_table");
  feenox_runtime_error();
  
  return 0;
  
#endif

}


// TODO: doble adaptivo

///fu+prod+name prod
///fu+prod+usage prod(f(i), i, a, b)
///fu+prod+desc Computes product of the $N=b-a$ expressions $f(i)$
///fu+prod+desc given in the first argument by varying the variable~$i$
///fu+prod+desc given in the second argument between~$a$
///fu+prod+desc given in the third argument and~$b$
///fu+prod+desc given in the fourth argument,~$i = a, a+1, \dots ,b-1,b$. 
///fu+prod+math \prod_{i=a}^b f_i  
double feenox_builtin_prod(expr_item_t *a, var_t *var_x) {

  int i;
  int i_lower, i_upper;
  
  double x_old;
  double S;
  
  // nos acordamos cuanto vale x para despues volver a dejarle ese valor
  x_old = feenox_var_value(var_x);

  i_lower = (int)(round(feenox_expression_eval(&a->arg[2])));
  i_upper = (int)(round(feenox_expression_eval(&a->arg[3])));
  
  S = 1;
  for (i = i_lower; i <= i_upper; i++) {
    feenox_var_value(var_x) = (double)i;
    S *= feenox_expression_eval(&a->arg[0]);
  }
  
  feenox_var_value(var_x) = x_old;
  
  return S;
  
}

///fu+sum+name sum
///fu+sum+usage sum(f_i, i, a, b)
///fu+sum+desc Computes sum of the $N=b-a$ expressions $f_i$
///fu+sum+desc given in the first argument by varying the variable $i$
///fu+sum+desc given in the second argument between $a$
///fu+sum+desc given in the third argument and $b$
///fu+sum+desc given in the fourth argument, $i=a,a+1,\dots,b-1,b$. 
///fu+sum+math \sum_{i=a}^b f_i  
///fu+sum+example sum1.was sum2.was sum3.was
double feenox_builtin_sum(expr_item_t *a, var_t *var_x) {

  int i;
  int i_lower, i_upper;
  
  double x_old;
  double S;
  
  // nos acordamos cuanto vale x para despues volver a dejarle ese valor
  x_old = feenox_var_value(var_x);

  i_lower = (int)(round(feenox_expression_eval(&a->arg[2])));
  i_upper = (int)(round(feenox_expression_eval(&a->arg[3])));
  
  S = 0;
  for (i = i_lower; i <= i_upper; i++) {
    feenox_var_value(var_x) = (double)i;
    S += feenox_expression_eval(&a->arg[0]);
  }
  
  feenox_var_value(var_x) = x_old;
  
  return S;
  
}


///fu+root+function_name root
///fu+root+usage root(f(x), x, a, b, [eps], [alg], [p])
///fu+root+math \left\{ x \in [a,b] / f(x) = 0 \right \} 
///fu+root+desc Computes the value of the variable $x$ given in the second argument
///fu+root+desc which makes the expression $f(x)$ given in the first argument to
///fu+root+desc be equal to zero by using a root bracketing algorithm.
///fu+root+desc The root should be in the range $[a,b]$ given by the third and fourth arguments.
///fu+root+desc The optional fifth argument $\epsilon$ gives a relative tolerance
///fu+root+desc for testing convergence, corresponding to GSL `epsrel` (note that
///fu+root+desc `epsabs` is set also to $\epsilon)$.
///fu+root+desc The sixth optional argument is an integer which indicates the
///fu+root+desc algorithm to use:
///fu+root+desc 0 (default) is `brent`,
///fu+root+desc 1 is `falsepos` and
///fu+root+desc 2 is `bisection`.
///fu+root+desc See the GSL documentation for further information on the algorithms.
///fu+root+desc The seventh optional argument $p$ is a flag that indicates how to proceed
///fu+root+desc if the sign of $f(a)$ is equal to the sign of $f(b)$.
///fu+root+desc If $p=0$ (default) an error is raised, otherwise it is not.
///fu+root+desc If more than one root is contained in the specified range, the first
///fu+root+desc one to be found is returned. The initial guess is $x_0 = (a+b)/2$.
///fu+root+desc If no roots are contained in the range and
///fu+root+desc $p \neq 0$, the returned value can be any value.
///fu+root+desc Default is $\epsilon = (1/2)^{-10} \approx 10^{3}$.
///fu+root+example root1.was root2.was root3.was root4.was
double feenox_builtin_root(expr_item_t *a, var_t *var_x) {
  int iter;
  int gsl_status;
  double x, x_old;
  double x_lower, x_upper;
  double f_lower, f_upper;
  double tmp;
  double epsrel;

  gsl_root_fsolver *s;
  gsl_function function_to_solve;
  gsl_function_arguments_t function_arguments;
  
  int nocomplain;
  int max_iter = DEFAULT_ROOT_MAX_TER;

  // evaluamos el intervalo donde sospechamos esta la solucion
  // nos acordamos cuanto vale x para despues volver a dejarle ese valor
  x_old = feenox_var_value(var_x);

  x_lower = feenox_expression_eval(&a->arg[2]);
  x_upper = feenox_expression_eval(&a->arg[3]);
  // si estan al reves los damos vuelta para evitar GSL_EINVAL
  if (x_lower > x_upper) {
    // manual de la CZ1000 de 1981
    tmp = x_lower;
    x_lower = x_upper;
    x_upper = tmp;
  }
  
  // arrancamos desde la mitad
  x = 0.5*(x_lower + x_upper);

  // evaluamos la precision buscada (no necesariamente es una constante)
  if ((epsrel = feenox_expression_eval(&a->arg[4])) == 0) {
    epsrel = DEFAULT_ROOT_TOLERANCE;
  }

  // el algoritmo
  // TODO: poner en aux para no tener que hacer alloc cada vez
  switch((int)feenox_expression_eval(&a->arg[5])) {
    case 0:
      s = gsl_root_fsolver_alloc(gsl_root_fsolver_brent);
    break;
    case 1:
      s = gsl_root_fsolver_alloc(gsl_root_fsolver_falsepos);
    break;
    case 2:
      s = gsl_root_fsolver_alloc(gsl_root_fsolver_bisection);
    break;
  }
  
  // un flag que nos indica si tenemos que ser quejosos o no
  nocomplain = feenox_expression_eval(&a->arg[6]);


  function_arguments.function = a;
  function_arguments.variable = var_x;

  function_to_solve.function = feenox_gsl_function;
  function_to_solve.params = (void *)(&function_arguments);

  // apagamos el handler para mirar el do not straddle y=0 nosotros
  gsl_set_error_handler_off();

  // miramos nosotros si el rango atrapa una raiz o no
  SAFE_FUNC_CALL (&function_to_solve, x_lower, &f_lower);
  SAFE_FUNC_CALL (&function_to_solve, x_upper, &f_upper);  
  
  if ((f_lower < 0.0 && f_upper < 0.0) || (f_lower > 0.0 && f_upper > 0.0)) {
    feenox_var_value(var_x) = x_old;
    if (nocomplain) {
      return 0;
    } else {
      
      int on_gsl_error = (int)(feenox_var_value(feenox_special_var(on_gsl_error)));

      if (!(on_gsl_error & ON_ERROR_NO_REPORT)) {
        feenox_push_error_message("range does not contain a root, f(%g) = %g and f(%g) = %g", x_lower, f_lower, x_upper, f_upper);
        feenox_pop_errors();
      }

      if (!(on_gsl_error & ON_ERROR_NO_QUIT)) {
        feenox_polite_exit(FEENOX_ERROR);
      }
    }
  }

  gsl_root_fsolver_set(s, &function_to_solve, x_lower, x_upper);
  
  // volvemos al poner el handler para lo que sigue
  gsl_set_error_handler(feenox_gsl_handler);
  
  // si no tenemos que ser quejosos entonces sacamos temporalmente
  // el handler de errores de la GSL
  if (nocomplain) {
  } else {
    gsl_root_fsolver_set(s, &function_to_solve, x_lower, x_upper);
  }

  iter = 0;
  do {
    iter++;
    gsl_root_fsolver_iterate(s);
    x = gsl_root_fsolver_root(s);
    x_lower = gsl_root_fsolver_x_lower(s);
    x_upper = gsl_root_fsolver_x_upper(s);
    gsl_status= gsl_root_test_interval(x_lower, x_upper, epsrel, epsrel);
  } while (gsl_status == GSL_CONTINUE && iter < max_iter);

  feenox_var_value(var_x) = x_old;

  gsl_root_fsolver_free(s);

  return x;

}


///fu+func_min+function_name func_min
///fu+func_min+escaped_name func\_min
///fu+func_min+desc Finds the value of the variable $x$ given in the second argument
///fu+func_min+desc which makes the expression $f(x)$ given in the first argument to
///fu+func_min+desc take local a minimum in the in the range $[a,b]$ given by
///fu+func_min+desc the third and fourth arguments. If there are many local minima,
///fu+func_min+desc the one that is closest to $(a+b)/2$ is returned.
///fu+func_min+desc The optional fifth argument $\epsilon$ gives a relative tolerance
///fu+func_min+desc for testing convergence, corresponding to GSL `epsrel` (note that
///fu+func_min+desc `epsabs` is set also to $\epsilon)$.
///fu+func_min+desc The sixth optional argument is an integer which indicates the
///fu+func_min+desc algorithm to use:
///fu+func_min+desc 0 (default) is `quad_golden`,
///fu+func_min+desc 1 is `brent` and
///fu+func_min+desc 2 is `goldensection`.
///fu+func_min+desc See the GSL documentation for further information on the algorithms.
///fu+func_min+desc The seventh optional argument $p$ is a flag that indicates how to proceed
///fu+func_min+desc if there is no local minimum in the range $[a,b]$.
///fu+func_min+desc If $p = 0$ (default), $a$ is returned if $f(a) < f(b)$ and $b$ otherwise.
///fu+func_min+desc If $p = 1$ then the local minimum algorimth is tried nevertheless.
///fu+func_min+desc Default is $\epsilon = (1/2)^{-20} \approx 9.6\times 10^{-7}$.
///fu+func_min+usage y = func_min(f(x), x, a, b, [eps], [alg], [p])
///fu+func_min+math y = \left\{ x \in [a,b] / f(x) = \min_{[a,b]} f(x) \right \} 
///fu+func_min+example func_min.was

double feenox_builtin_func_min(expr_item_t *a, var_t *var_x) {
  
  int iter, gsl_status;
  double x, x_old;
  double x_lower, x_upper;
  double epsrel;

  
  const gsl_min_fminimizer_type *T;
  gsl_min_fminimizer *s;
  gsl_function function_to_solve;
  gsl_function_arguments_t function_arguments;

  int max_iter;
  int nocomplain;
  max_iter = DEFAULT_ROOT_MAX_TER;
  
  // evaluamos el intervalo donde sospechamos esta la solucion
  // nos acordamos cuanto vale x para despues volver a dejarle ese valor
  x_old = feenox_var_value(var_x);

  x_lower = feenox_expression_eval(&a->arg[2]);
  x_upper = feenox_expression_eval(&a->arg[3]);

  // arrancamos desde la mitad
  x = 0.5*(x_lower + x_upper);
  
  // la precision
  if ((epsrel = feenox_expression_eval(&a->arg[4])) == 0) {
    epsrel = DEFAULT_ROOT_TOLERANCE;
  }


  // el algoritmo
  switch((int)feenox_expression_eval(&a->arg[5])) {
    case 0:
      T = gsl_min_fminimizer_quad_golden;
    break;
    case 1:
      T = gsl_min_fminimizer_brent;
    break;
    case 2:
      T = gsl_min_fminimizer_goldensection;
    break;
  }
  
  // un flag que nos indica si tenemos que ser quejosos o no
  nocomplain = feenox_expression_eval(&a->arg[6]);  


  function_arguments.function = a;
  function_arguments.variable = var_x;

  function_to_solve.function = feenox_gsl_function;
  function_to_solve.params = (void *)(&function_arguments);

  // TODO: poner en aux para no tener que hacer alloc cada vez
  s = gsl_min_fminimizer_alloc(T);

  // si no tenemos que ser quejosos entonces sacamos temporalmente
  // el handler de errores de la GSL 
  if (nocomplain) {
    gsl_set_error_handler_off();
    gsl_min_fminimizer_set(s, &function_to_solve, x, x_lower, x_upper);
    gsl_set_error_handler(feenox_gsl_handler);
  } else {
    gsl_set_error_handler_off();
    if (gsl_min_fminimizer_set(s, &function_to_solve, x, x_lower, x_upper) != GSL_SUCCESS) {
      gsl_set_error_handler(feenox_gsl_handler);
      
      gsl_min_fminimizer_free(s);
      if (feenox_gsl_function(x_lower, function_to_solve.params) < feenox_gsl_function(x_upper, function_to_solve.params)) {
        return x_lower;
      } else {
        return x_upper;
      }
    }
    gsl_set_error_handler(feenox_gsl_handler);
  }
  
  iter = 0;
  do {
    iter++;
    gsl_min_fminimizer_iterate(s);
    x = gsl_min_fminimizer_x_minimum(s);
    x_lower = gsl_min_fminimizer_x_lower(s);
    x_upper = gsl_min_fminimizer_x_upper(s);
    gsl_status = gsl_min_test_interval(x_lower, x_upper, epsrel, epsrel);
  } while (gsl_status == GSL_CONTINUE && iter < max_iter);

  feenox_var_value(var_x) = x_old;

  gsl_min_fminimizer_free(s);

  return x;

}


double feenox_gsl_function(double x, void *params) {

  double y;

  gsl_function_arguments_t *dummy = (gsl_function_arguments_t *)params;
  feenox_var_value(dummy->variable) = x;

  y = feenox_expression_eval(&dummy->function->arg[0]);

  if (gsl_isnan(y) || gsl_isinf(y)) {
    feenox_nan_error();
  }

  return y;

}
