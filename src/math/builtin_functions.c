/*------------ -------------- -------- --- ----- ---   --       -            -
 *  FeenoX builtin functions
 *
 *  Copyright (C) 2009--2021 jeremy theler
 *
 *  This file is part of FeenoX.
 *
 *  FeenoX is free software: you can redistribute it and/or modify
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
extern feenox_t feenox;

#include <stdlib.h>
#include <limits.h>
#ifdef __MACH__
 #include <mach/clock.h>
 #include <mach/mach.h>
#endif
#include <time.h>


double feenox_builtin_abs(struct expr_factor_t *);
double feenox_builtin_asin(struct expr_factor_t *);
double feenox_builtin_acos(struct expr_factor_t *);
double feenox_builtin_atan(struct expr_factor_t *);
double feenox_builtin_atan2(struct expr_factor_t *);
double feenox_builtin_ceil(struct expr_factor_t *);
double feenox_builtin_clock(struct expr_factor_t *);
double feenox_builtin_cos(struct expr_factor_t *);
double feenox_builtin_cosh(struct expr_factor_t *);
double feenox_builtin_d_dt(struct expr_factor_t *);
double feenox_builtin_deadband(struct expr_factor_t *);
double feenox_builtin_equal(struct expr_factor_t *);
double feenox_builtin_exp(struct expr_factor_t *);
double feenox_builtin_expint1(struct expr_factor_t *);
double feenox_builtin_expint2(struct expr_factor_t *);
double feenox_builtin_expint3(struct expr_factor_t *);
double feenox_builtin_expintn(struct expr_factor_t *);
double feenox_builtin_floor(struct expr_factor_t *);
double feenox_builtin_heaviside(struct expr_factor_t *);
double feenox_builtin_if(struct expr_factor_t *);
double feenox_builtin_is_in_interval(struct expr_factor_t *);
double feenox_builtin_integral_dt(struct expr_factor_t *);
double feenox_builtin_integral_euler_dt(struct expr_factor_t *);
double feenox_builtin_lag(struct expr_factor_t *);
double feenox_builtin_lag_bilinear(struct expr_factor_t *);
double feenox_builtin_lag_euler(struct expr_factor_t *);
double feenox_builtin_last(struct expr_factor_t *);
double feenox_builtin_limit(struct expr_factor_t *);
double feenox_builtin_limit_dt(struct expr_factor_t *);
double feenox_builtin_log(struct expr_factor_t *);
double feenox_builtin_mark_max(struct expr_factor_t *);
double feenox_builtin_mark_min(struct expr_factor_t *);
double feenox_builtin_max(struct expr_factor_t *);
double feenox_builtin_min(struct expr_factor_t *);
double feenox_builtin_mod(struct expr_factor_t *);
double feenox_builtin_not(struct expr_factor_t *);
double feenox_builtin_random(struct expr_factor_t *);
double feenox_builtin_random_gauss(struct expr_factor_t *);
double feenox_builtin_round(struct expr_factor_t *);
double feenox_builtin_sawtooth_wave(struct expr_factor_t *);
double feenox_builtin_sgn(struct expr_factor_t *);
double feenox_builtin_is_even(struct expr_factor_t *);
double feenox_builtin_is_odd(struct expr_factor_t *);
double feenox_builtin_sin(struct expr_factor_t *);
double feenox_builtin_j0(struct expr_factor_t *);
double feenox_builtin_sinh(struct expr_factor_t *);
double feenox_builtin_sqrt(struct expr_factor_t *);
double feenox_builtin_square_wave(struct expr_factor_t *);
double feenox_builtin_tan(struct expr_factor_t *);
double feenox_builtin_tanh(struct expr_factor_t *);
double feenox_builtin_threshold_max(struct expr_factor_t *);
double feenox_builtin_threshold_min(struct expr_factor_t *);
double feenox_builtin_triangular_wave(struct expr_factor_t *);

struct builtin_function_t builtin_function[N_BUILTIN_FUNCTIONS] = {
    {"abs",                 1, 1, &feenox_builtin_abs},
    {"asin",                1, 1, &feenox_builtin_asin},
    {"acos",                1, 1, &feenox_builtin_acos},
    {"atan",                1, 1, &feenox_builtin_atan},
    {"atan2",               2, 2, &feenox_builtin_atan2},
    {"ceil",                1, 1, &feenox_builtin_ceil},
    {"clock",               0, 1, &feenox_builtin_clock},
    {"cos",                 1, 1, &feenox_builtin_cos},
    {"cosh",                1, 1, &feenox_builtin_cosh},
    {"d_dt",                1, 1, &feenox_builtin_d_dt},
    {"deadband",            2, 2, &feenox_builtin_deadband},
    {"equal",               2, 3, &feenox_builtin_equal},
    {"exp",                 1, 1, &feenox_builtin_exp},
    {"expint1",             1, 1, &feenox_builtin_expint1},
    {"expint2",             1, 1, &feenox_builtin_expint2},
    {"expint3",             1, 1, &feenox_builtin_expint3},
    {"expintn",             2, 2, &feenox_builtin_expintn},
    {"floor",               1, 1, &feenox_builtin_floor},
    {"heaviside",           1, 2, &feenox_builtin_heaviside},
    {"if",                  1, 4, &feenox_builtin_if},
    {"integral_dt",         1, 1, &feenox_builtin_integral_dt},
    {"integral_euler_dt",   1, 1, &feenox_builtin_integral_euler_dt},
    {"is_even",             1, 1, &feenox_builtin_is_even},
    {"is_in_interval",      3, 3, &feenox_builtin_is_in_interval},
    {"is_odd",              1, 1, &feenox_builtin_is_odd},
    {"lag",                 2, 2, &feenox_builtin_lag},
    {"lag_bilinear",        2, 2, &feenox_builtin_lag_bilinear},
    {"lag_euler",           2, 2, &feenox_builtin_lag_euler},
    {"last",                1, 2, &feenox_builtin_last},
    {"limit",               3, 3, &feenox_builtin_limit},
    {"limit_dt",            3, 3, &feenox_builtin_limit_dt},
    {"log",                 1, 1, &feenox_builtin_log},
    {"mark_max",            2, MINMAX_ARGS, &feenox_builtin_mark_max},
    {"mark_min",            2, MINMAX_ARGS, &feenox_builtin_mark_min},
    {"max",                 2, MINMAX_ARGS, &feenox_builtin_max},
    {"min",                 2, MINMAX_ARGS, &feenox_builtin_min},
    {"mod",                 2, 2, &feenox_builtin_mod},
    {"not",                 1, 2, &feenox_builtin_not},
    {"random",              2, 3, &feenox_builtin_random},
    {"random_gauss",        2, 3, &feenox_builtin_random_gauss},
    {"round",               1, 1, &feenox_builtin_round},
    {"sawtooth_wave",       1, 1, &feenox_builtin_sawtooth_wave},
    {"sgn",                 1, 2, &feenox_builtin_sgn},
    {"sin",                 1, 1, &feenox_builtin_sin},
    {"j0",                  1, 1, &feenox_builtin_j0},
    {"sinh",                1, 1, &feenox_builtin_sinh},
    {"sqrt",                1, 1, &feenox_builtin_sqrt},
    {"square_wave",         1, 1, &feenox_builtin_square_wave},
    {"tan",                 1, 1, &feenox_builtin_tan},
    {"tanh",                1, 1, &feenox_builtin_tanh},
    {"threshold_max",       2, 3, &feenox_builtin_threshold_max},
    {"threshold_min",       2, 3, &feenox_builtin_threshold_min},
    {"triangular_wave",     1, 1, &feenox_builtin_triangular_wave},
};


///fn+clock+name clock
///fn+clock+usage clock([f])
///fn+clock+desc Returns the value of a certain clock in seconds measured from a certain (but specific)
///fn+clock+desc milestone. The kind of clock and the initial milestone depends on the
///fn+clock+desc optional flag $f$. It defaults to zero, meaning wall time since the UNIX Epoch.
///fn+clock+desc The list and the meanings of the other available values for $f$ can be checked
///fn+clock+desc in the `clock_gettime (2)` system call manual page.
///fn+clock+example clock.was
double feenox_builtin_clock(expr_factor_t *expr) {

  struct timespec tp;

#if defined (__MACH__)
  // OS X does not have clock_gettime, use clock_get_time
  clock_id_t clk_id;

  if (expr->arg[0].factors != NULL) {
    clk_id = (int)feenox_expression_eval(&expr->arg[0]);
  } else {
    clk_id = SYSTEM_CLOCK; // Same meaning than monotonic in linux
  }

  clock_serv_t cclock;
  mach_timespec_t mts;
  host_get_clock_service(mach_host_self(), clk_id, &cclock);
  clock_get_time(cclock, &mts);
  mach_port_deallocate(mach_task_self(), cclock);
  tp.tv_sec = mts.tv_sec;
  tp.tv_nsec = mts.tv_nsec;

  return (float)tp.tv_sec + ((float)tp.tv_nsec * 1E-9);
#else
  clockid_t clk_id;

  if (expr->arg[0].factors != NULL) {
    clk_id = (int)feenox_expression_eval(&expr->arg[0]);
  } else {
    clk_id = CLOCK_MONOTONIC;
  }

  if (clock_gettime(clk_id, &tp) < 0) {
    feenox_runtime_error();
  }

  return (float)tp.tv_sec + ((float)tp.tv_nsec * 1E-9);
#endif
}

///fn+last+name last
///fn+last+usage last(x,[p])
///fn+last+math z^{-1}\left[ x \right] = x(t-\Delta t)
///fn+last+desc Returns the value the variable $x$ had in the previous time step.
///fn+last+desc This function is equivalent to the $Z$-transform operator "delay" denoted by $z^{-1}\left[ x\right]$.
///fn+last+desc For $t=0$ the function returns the actual value of $x$.
///fn+last+desc The optional flag $p$ should be set to one if the reference to `last`
///fn+last+desc is done in an assignment over a variable that already appears inside
///fn+last+desc expression $x$. See example number 2.
///fn+last+example last1.was last2.was
double feenox_builtin_last(expr_factor_t *expr) {

  double y;
  double x[2];
  x[0] = feenox_expression_eval(&expr->arg[0]);
  x[1] = feenox_expression_eval(&expr->arg[1]);

  if (expr->aux == NULL) {
    expr->aux = malloc(3*sizeof(double));
    expr->aux[0] = x[0];
    expr->aux[1] = x[0];
    expr->aux[2] = 0;
  } else {
    
    if ((int)feenox_special_var_value(in_static)) {
      if ((int)round(expr->aux[2]) != (int)(feenox_special_var_value(step_static))) {
        expr->aux[0] = expr->aux[1];
        expr->aux[1] = x[0];
        expr->aux[2] = (int)(feenox_special_var_value(step_static));
      }
      
    } else {
      if ((int)round(expr->aux[2]) != (int)(feenox_special_var_value(step_transient))) {
        expr->aux[0] = expr->aux[1];
        expr->aux[1] = x[0];
        expr->aux[2] = (int)(feenox_special_var_value(step_transient));
      }
    }
  }

  // esto es un maneje para poder hacer x = last(x), TODO: buscar una solucion mas elegante
  if (x[1] == 0) {
    y = expr->aux[0];
  } else {
    y = x[0];
  }

  if (feenox_special_var_value(done)) {
    free(expr->aux);
    expr->aux = NULL;
  }
  return y;
}

///fn+d_dt+name d_dt
///fn+d_dt+usage d_dt(x)
///fn+d_dt+math \frac{x(t) - x(t-\Delta t)}{\Delta t} \approx \frac{d}{dt} \Big( x (t) \Big)
///fn+d_dt+desc Computes the time derivative of the signal $x$ using the difference between the
///fn+d_dt+desc value of the signal in the previous time step and the actual value divided by the
///fn+d_dt+desc time step. For $t=0$, the return value is zero.
///fn+d_dt+desc Unlike the functional `derivative`, this function works with expressions and not
///fn+d_dt+desc with functions. Therefore the argument $x$ may be for example an expression
///fn+d_dt+desc involving a variable that may be read from a shared-memory object, whose
///fn+d_dt+desc time derivative cannot be computed with `derivative`.
///fn+d_dt+example d_dt.was
double feenox_builtin_d_dt(expr_factor_t *expr) {

  double y;
  double x[1];
  x[0] = feenox_expression_eval(&expr->arg[0]);

  if (feenox_special_var_value(dt) == 0) {
    feenox_nan_error();
    return 0;
  }

  if (expr->aux == NULL) {
    expr->aux = malloc(3*sizeof(double));
    expr->aux[0] = x[0];
    expr->aux[1] = x[0];
    expr->aux[2] = 0;
  } else if ((int)(feenox_special_var_value(in_static))) {
    expr->aux[0] = x[0];
    expr->aux[1] = x[0];
    expr->aux[2] = 0;
  } else if ((int)round(expr->aux[2]) != (int)(feenox_special_var_value(step_transient))) {
    expr->aux[0] = expr->aux[1];
    expr->aux[1] = x[0];
    expr->aux[2] = (int)(feenox_special_var_value(step_transient));
  }

  y = (x[0] - expr->aux[0])/feenox_special_var_value(dt);

  if (feenox_special_var_value(done)) {
    free(expr->aux);
    expr->aux = NULL;
  }
  return y;
}

///fn+integral_dt+name integral_dt
///fn+integral_dt+usage integral_dt(x)
///fn+integral_dt+math z^{-1}\left[ \int_0^t x(t') \, dt' \right] +  \frac{x(t) + x(t-\Delta t)}{2} \, \Delta t \approx \int_0^{t} x(t') \, dt'
///fn+integral_dt+desc Computes the time integral of the signal $x$ using the trapezoidal rule
///fn+integral_dt+desc using the value of the signal in the previous time step and the current value.
///fn+integral_dt+desc At $t = 0$ the integral is initialized to zero.
///fn+integral_dt+desc Unlike the functional `integral`, this function works with expressions and not
///fn+integral_dt+desc with functions. Therefore the argument $x$ may be for example an expression
///fn+integral_dt+desc involving a variable that may be read from a shared-memory object, whose
///fn+integral_dt+desc time integral cannot be computed with `integral`.
///fn+integral_dt+example integral_dt.was
double feenox_builtin_integral_dt(expr_factor_t *expr) {

  double y;
  double x[1];
  x[0] = feenox_expression_eval(&expr->arg[0]);

  if (expr->aux == NULL) {
    expr->aux = malloc(4*sizeof(double));
    expr->aux[0] = x[0];
    expr->aux[1] = x[0];
    expr->aux[2] = 0;
    expr->aux[3] = 0;
  } else if ((int)(feenox_special_var_value(in_static))) {
    expr->aux[0] = x[0];
    expr->aux[1] = x[0];
    expr->aux[2] = 0;
    expr->aux[3] = 0;
  } else if ((int)round(expr->aux[3]) != (int)(feenox_special_var_value(step_transient))) {
    expr->aux[0] = expr->aux[1];
    expr->aux[1] = x[0];
    expr->aux[2] += 0.5*(expr->aux[0]+x[0])*feenox_special_var_value(dt);
    expr->aux[3] = (int)(feenox_special_var_value(step_transient));
  }

  y = expr->aux[2];

  if (feenox_special_var_value(done)) {
    free(expr->aux);
    expr->aux = NULL;
  }
  return y;
}

///fn+integral_euler_dt+name integral_euler_dt
///fn+integral_euler_dt+math z^{-1}\left[ \int_0^t x(t') \, dt' \right] +   x(t) \, \Delta t \approx \int_0^{t} x(t') \, dt'
///fn+integral_euler_dt+usage integral_euler_dt(x)
///fn+integral_euler_dt+desc Idem as `integral_dt` but uses the backward Euler rule to update the
///fn+integral_euler_dt+desc integral value.
///fn+integral_euler_dt+desc This function is provided in case this particular way
///fn+integral_euler_dt+desc of approximating time integrals is needed.
double feenox_builtin_integral_euler_dt(expr_factor_t *expr) {

  double y;
  double x[1];
  x[0] = feenox_expression_eval(&expr->arg[0]);

  if (expr->aux == NULL) {
    expr->aux = malloc(2*sizeof(double));
    expr->aux[0] = 0;
    expr->aux[1] = 0;
  } else if ((int)(feenox_special_var_value(in_static))) {
    expr->aux[0] = 0;
    expr->aux[1] = 0;
  } else if ((int)round(expr->aux[1]) != (int)(feenox_special_var_value(step_transient))) {
    expr->aux[0] += x[0]*feenox_special_var_value(dt);
    expr->aux[1] = (int)(feenox_special_var_value(step_transient));
  }

  y = expr->aux[0];

  if (feenox_special_var_value(done)) {
    free(expr->aux);
    expr->aux = NULL;
  }
  return y;
}

///fn+square_wave+name square_wave
///fn+square_wave+usage square_wave(x)
///fn+square_wave+desc Computes a square function betwen zero and one with a period equal to one.
///fn+square_wave+desc The output is one for $0 < x < 1/2$ and goes to zero for $1/2 < x < 1$.
///fn+square_wave+desc As with the sine wave, a square wave can be generated by passing as the argument $x$
///fn+square_wave+desc a linear function of time such as $\omega t+\phi$, where $\omega$ controls the frequency of the wave
///fn+square_wave+desc and $\phi$ controls its phase.
///fn+square_wave+math \begin{cases} 1 & \text{if $x - \lfloor x \rfloor < 0.5$} \\ 0 & \text{otherwise} \end{cases}
///fn+square_wave+plotx 0 2.75 1e-2
///fn+square_wave+example square_wave.was
double feenox_builtin_square_wave(expr_factor_t *expr) {

  double x[1];
  x[0] = feenox_expression_eval(&expr->arg[0]);

  if ((x[0] - floor(x[0])) < 0.5) {
    return 1;
  }
  return 0;
}

///fn+triangular_wave+name triangular_wave
///fn+triangular_wave+usage triangular_wave(x)
///fn+triangular_wave+math \begin{cases} 2 (x - \lfloor x \rfloor) & \text{if $x - \lfloor x \rfloor < 0.5$} \\ 2 [1-(x - \lfloor x \rfloor)] & \text{otherwise} \end{cases}
///fn+triangular_wave+desc Computes a triangular wave betwen zero and one with a period equal to one.
///fn+triangular_wave+desc As with the sine wave, a triangular wave can be generated by passing as the argument $x$
///fn+triangular_wave+desc a linear function of time such as $\omega t+\phi$, where $\omega $ controls the frequency of the wave
///fn+triangular_wave+desc and $\phi$ controls its phase.
///fn+triangular_wave+plotx 0 2.75 0.1
double feenox_builtin_triangular_wave(expr_factor_t *expr) {

  double x[1];
  x[0] = feenox_expression_eval(&expr->arg[0]);

  if ((x[0] - floor(x[0])) < 0.5) {
    return 2*(x[0] - floor(x[0]));
  } else {
    return 2*(1 - (x[0] - floor(x[0])));
  }
}

///fn+sawtooth_wave+name sawtooth_wave
///fn+sawtooth_wave+usage sawtooth_wave(x)
///fn+sawtooth_wave+math x - \lfloor x \rfloor
///fn+sawtooth_wave+desc Computes a sawtooth wave betwen zero and one with a period equal to one.
///fn+sawtooth_wave+desc As with the sine wave, a sawtooh wave can be generated by passing as the argument $x$
///fn+sawtooth_wave+desc a linear function of time such as $\omega t+\phi$, where $\omega$ controls the frequency of the wave
///fn+sawtooth_wave+desc and $\phi$ controls its phase.
///fn+sawtooth_wave+plotx 0 2.75 1e-2
///fn+sawtooth_wave+example sawtooth_wave.was
double feenox_builtin_sawtooth_wave(expr_factor_t *expr) {

  double x[1];
  x[0] = feenox_expression_eval(&expr->arg[0]);

  return x[0] - floor(x[0]);
}

///fn+sin+name sin
///fn+sin+usage sin(x)
///fn+sin+math \sin(x)
///fn+sin+desc Computes the sine of the argument $x$, where $x$ is in radians.
///fn+sin+desc A sine wave can be generated by passing as the argument $x$
///fn+sin+desc a linear function of time such as $\omega t+\phi$, where $\omega$ controls the frequency of the wave
///fn+sin+desc and $\phi$ controls its phase.
///fn+sin+plotx -2*pi 2*pi pi/100
double feenox_builtin_sin(expr_factor_t *expr) {
  double x[1];
  x[0] = feenox_expression_eval(&expr->arg[0]);

  return sin(x[0]);
}

///fn+asin+name asin
///fn+asin+usage y = asin(x)
///fn+asin+math y = \arcsin(x)
///fn+asin+desc Computes arc in radians whose sine is equal to the argument $x$.
///fn+asin+desc A NaN error is raised if $|x|>1$.
///fn+asin+plotx -1 1 1/100 -1 1 0.5 -1 1 1
double feenox_builtin_asin(expr_factor_t *expr) {
  double x[1];
  x[0] = feenox_expression_eval(&expr->arg[0]);

  if (fabs(x[0]) > 1.0) {
    feenox_push_error_message("argument greater than one to function asin");
    feenox_nan_error();
    return 0;
  }

  return asin(x[0]);
}


///fn+acos+name acos
///fn+acos+usage y = acos(x)
///fn+acos+math y = \arccos(x)
///fn+acos+desc Computes arc in radians whose cosine is equal to the argument $x$.
///fn+acos+desc A NaN error is raised if $|x|>1$.
///fn+acos+plotx -1 1 1/100 -1 1 0.5 0 2.8 1
double feenox_builtin_acos(expr_factor_t *expr) {
  double x[1];
  x[0] = feenox_expression_eval(&expr->arg[0]);

  if (fabs(x[0]) > 1.0) {
    feenox_push_error_message("argument greater than one to function acos");
    feenox_nan_error();
    return 0;
  }

  return acos(x[0]);
}

///fn+j0+name j0
///fn+j0+usage j0(x)
///fn+j0+math J_0(x)
///fn+j0+desc Computes the regular cylindrical Bessel function of zeroth order evaluated at the argument $x$.
///fn+j0+plotx 0 10 0.05
double feenox_builtin_j0(expr_factor_t *expr) {
  double x[1];
  x[0] = feenox_expression_eval(&expr->arg[0]);

  return gsl_sf_bessel_J0(x[0]);
}

///fn+cos+name cos
///fn+cos+usage cos(x)
///fn+cos+math \cos(x)
///fn+cos+desc Computes the cosine of the argument $x$, where $x$ is in radians.
///fn+cos+desc A cosine wave can be generated by passing as the argument $x$
///fn+cos+desc a linear function of time such as $\omega t+\phi$, where $\omega$ controls the frequency of the wave
///fn+cos+desc and $\phi$ controls its phase.
///fn+cos+plotx -2*pi 2*pi pi/100
double feenox_builtin_cos(expr_factor_t *expr) {
  double x[1];
  x[0] = feenox_expression_eval(&expr->arg[0]);

  return cos(x[0]);
}

///fn+tan+name tan
///fn+tan+usage tan(x)
///fn+tan+math  \tan(x)
///fn+tan+desc Computes the tangent of the argument $x$, where $x$ is in radians.
///fn+tan+plotx -pi/3 pi/3 pi/100
double feenox_builtin_tan(expr_factor_t *expr) {
  double x[1];
  x[0] = feenox_expression_eval(&expr->arg[0]);

  return tan(x[0]);
}

///fn+sinh+name sinh
///fn+sinh+usage sinh(x)
///fn+sinh+math \sinh(x)
///fn+sinh+desc Computes the hyperbolic sine of the argument $x$, where $x$ is in radians.
///fn+sinh+plotx -pi/2 pi/2 pi/100
double feenox_builtin_sinh(expr_factor_t *expr) {
  double x[1];
  x[0] = feenox_expression_eval(&expr->arg[0]);

  return sinh(x[0]);
}

///fn+cosh+name cosh
///fn+cosh+usage cosh(x)
///fn+cosh+math \cosh(x)
///fn+cosh+desc Computes the hyperbolic cosine of the argument $x$, where $x$ is in radians.
///fn+cosh+plotx -2*pi 2*pi pi/100
double feenox_builtin_cosh(expr_factor_t *expr) {
  double x[1];
  x[0] = feenox_expression_eval(&expr->arg[0]);

  return cosh(x[0]);
}

///fn+tanh+name tanh
///fn+tanh+usage tanh(x)
///fn+tanh+math \tanh(x)
///fn+tanh+desc Computes the hyperbolic tangent of the argument $x$, where $x$ is in radians.
///fn+tanh+plotx -pi/2 pi/2 pi/100
double feenox_builtin_tanh(expr_factor_t *expr) {
  double x[1];
  x[0] = feenox_expression_eval(&expr->arg[0]);

  return tanh(x[0]);
}


///fn+atan+name atan
///fn+atan+usage atan(x)
///fn+atan+math \arctan(x)
///fn+atan+desc Computes, in radians, the arc tangent of the argument $x$.
///fn+atan+plotx -pi pi pi/100
///fn+atan2+example atan.was
double feenox_builtin_atan(expr_factor_t *expr) {
  double x[1];
  x[0] = feenox_expression_eval(&expr->arg[0]);

  return atan(x[0]);
}

///fn+atan2+name atan2
///fn+atan2+usage atan(y,x)
///fn+atan2+math \arctan(y/x) \in [-\pi,\pi]
///fn+atan2+desc Computes, in radians, the arc tangent of quotient $y/x$, using the signs of the two arguments
///fn+atan2+desc to determine the quadrant of the result, which is in the range $[-\pi,\pi]$.
///fn+atan2+example atan2.was
double feenox_builtin_atan2(expr_factor_t *expr) {
  double x[2];
  x[0] = feenox_expression_eval(&expr->arg[0]);
  x[1] = feenox_expression_eval(&expr->arg[1]);

  return atan2(x[0], x[1]);
}

///fn+exp+name exp
///fn+exp+desc Computes the exponential function the argument $x$, i.e. the base of the
///fn+exp+desc natural logarithms raised to the $x$-th power.
///fn+exp+usage exp(x)
///fn+exp+math e^x
///fn+exp+plotx -1 2.75 1e-2
///fn+exp+example exp.was
double feenox_builtin_exp(expr_factor_t *expr) {
  double x[1];
  x[0] = feenox_expression_eval(&expr->arg[0]);

  return (x[0] < GSL_LOG_DBL_MIN)?0:gsl_sf_exp(x[0]);
}

///fn+expint1+name expint1
///fn+expint1+desc Computes the first exponential integral function of the argument $x$.
///fn+expint1+desc If $x$ equals zero, a NaN error is issued.
///fn+expint1+usage expint1(x)
///fn+expint1+math \text{Re} \left[ \int_1^{\infty}\! \frac{\exp(-xt)}{t} \, dt \right]
///fn+expint1+plotx 1e-2 2.0 1e-2
///fn+expint1+example expint1.was
double feenox_builtin_expint1(expr_factor_t *expr) {
  double x[1];
  x[0] = feenox_expression_eval(&expr->arg[0]);

  if (x[0] == 0) {
    feenox_nan_error();
    return 0;
  }

  return gsl_sf_expint_E1(x[0]);
}

///fn+expint2+name expint2
///fn+expint2+desc Computes the second exponential integral function of the argument $x$.
///fn+expint2+usage expint2(x)
///fn+expint2+math \text{Re} \left[ \int_1^{\infty}\! \frac{\exp(-xt)}{t^2} \, dt \right]
///fn+expint2+plotx 0.0 2.0 1e-2
///fn+expint2+example expint2.was
double feenox_builtin_expint2(expr_factor_t *expr) {
  double x[1];
  x[0] = feenox_expression_eval(&expr->arg[0]);

  return gsl_sf_expint_E2(x[0]);
}

///fn+expint3+name expint3
///fn+expint3+desc Computes the third exponential integral function of the argument $x$.
///fn+expint3+usage expint3(x)
///fn+expint3+math \text{Re} \left[ \int_1^{\infty}\! \frac{\exp(-xt)}{t^3} \, dt \right]
///fn+expint3+plotx 0.0 2.0 1e-2
double feenox_builtin_expint3(expr_factor_t *expr) {
  double x[1];
  x[0] = feenox_expression_eval(&expr->arg[0]);

  return gsl_sf_expint_En(3, x[0]);
}

///fn+expintn+name expintn
///fn+expintn+desc Computes the $n$-th exponential integral function of the argument $x$.
///fn+expintn+desc If $n$ equals zero or one and $x$ zero, a NaN error is issued.
///fn+expintn+usage expintn(n,x)
///fn+expintn+math \text{Re} \left[ \int_1^{\infty}\! \frac{\exp(-xt)}{t^n} \, dt \right]
///fn+expintn+example expintn.was
double feenox_builtin_expintn(expr_factor_t *expr) {
  double x[1];
  int n;
  n = ((int)(round(feenox_expression_eval(&expr->arg[0]))));
  x[0] = feenox_expression_eval(&expr->arg[1]);

  if ((n == 1 || n == 0) && x[0] == 0) {
    feenox_nan_error();
    return 0;
  }

  return gsl_sf_expint_En(n, x[0]);
}

///fn+log+name log
///fn+log+desc Computes the natural logarithm of the argument $x$. If $x$ is zero or negative,
///fn+log+desc a NaN error is issued.
///fn+log+usage log(x)
///fn+log+math \ln(x)
///fn+log+plotx 1e-2 2.75 1e-2
///fn+log+example log.was
double feenox_builtin_log(expr_factor_t *expr) {
  double x[1];
  x[0] = feenox_expression_eval(&expr->arg[0]);

  if (x[0] <= 0) {
    feenox_nan_error();
    return 0;
  }

  return gsl_sf_log(x[0]);
}


///fn+abs+name abs
///fn+abs+desc Returns the absolute value of the argument $x$.
///fn+abs+usage y = abs(x)
///fn+abs+math y = |x|
///fn+abs+plotx -2.5 2.5 1e-2   -2 2 1   0 2 1
///fn+abs+example abs.was
double feenox_builtin_abs(expr_factor_t *expr) {
  double x[1];
  x[0] = feenox_expression_eval(&expr->arg[0]);

  return fabs(x[0]);
}


///fn+sqrt+name sqrt
///fn+sqrt+desc Computes the positive square root of the argument $x$. If $x$ is negative,
///fn+sqrt+desc a NaN error is issued.
///fn+sqrt+usage sqrt(x)
///fn+sqrt+math \sqrt{x}
///fn+sqrt+plotx 0 2.75 1e-2
double feenox_builtin_sqrt(expr_factor_t *expr) {
  double x[1];
  x[0] = feenox_expression_eval(&expr->arg[0]);

  if (x[0] < 0) {
    feenox_nan_error();
    return 0;
  }
  return sqrt(x[0]);
}


///fn+is_even+name is_even
///fn+is_even+desc Returns one if the argument $x$ rounded to the nearest integer is even.
///fn+is_even+usage y = is_even(x)
///fn+is_even+math  y = \begin{cases}1 &\text{if $x$ is even} \\ 0 &\text{if $x$} is odd} \end{cases}
double feenox_builtin_is_even(expr_factor_t *expr) {
  return GSL_IS_EVEN((int)(round(feenox_expression_eval(&expr->arg[0]))));
}

///fn+is_odd+name is_odd
///fn+is_odd+desc Returns one if the argument $x$ rounded to the nearest integer is odd.
///fn+is_odd+usage y = is_odd(x)
///fn+is_odd+math  y = \begin{cases}1 &\text{if $x$ is odd} \\ 0 &\text{if $x$} is even} \end{cases}
double feenox_builtin_is_odd(expr_factor_t *expr) {
  return GSL_IS_ODD((int)(round(feenox_expression_eval(&expr->arg[0]))));
}


///fn+heaviside+name heaviside
///fn+heaviside+desc Computes the zero-centered Heaviside step function of the argument $x$.
///fn+heaviside+desc If the optional second argument $\epsilon$ is provided, the discontinuous
///fn+heaviside+desc step at $x=0$ is replaced by a ramp starting at $x=0$ and finishing at $x=\epsilon$.
///fn+heaviside+usage heaviside(x, [eps])
///fn+heaviside+math \begin{cases} 0 & \text{if $x < 0$} \\ x / \epsilon & \text{if $0 < x < \epsilon$} \\ 1 & \text{if $x > \epsilon$} \end{cases}
///fn+heaviside+plotx -2.75 2.75 1e-2
///fn+heaviside+example heaviside.was
double feenox_builtin_heaviside(expr_factor_t *expr) {
  double x[2];

  x[0] = feenox_expression_eval(&expr->arg[0]);
  x[1] = feenox_expression_eval(&expr->arg[1]);

  if (x[0] <= 0) {
    return 0;
  } else if (x[0] >= x[1]) {
    return 1;
  }

  return x[0]/x[1];
}


///fn+sgn+name sgn
///fn+sgn+desc Returns minus one, zero or plus one depending on the sign of the first argument $x$.
///fn+sgn+desc The second optional argument $\epsilon$ gives the precision of the "zero"
///fn+sgn+desc evaluation. If not given, default is $\epsilon = 10^{-16}$.
///fn+sgn+usage sgn(x, [eps])
///fn+sgn+math  \begin{cases}-1 &\text{if $x \le -\epsilon$} \\ 0 &\text{if $|x| < \epsilon$} \\ +1 &\text{if $x \ge +\epsilon$} \end{cases}
///fn+sgn+plotx -2.75 2.75 1e-2
double feenox_builtin_sgn(expr_factor_t *expr) {
  double x[2];
  double eps = 1e-16;

  x[0] = feenox_expression_eval(&expr->arg[0]);
  x[1] = feenox_expression_eval(&expr->arg[1]);

  if (expr->arg[1].factors != NULL) {
    eps = x[1];
  }
  if (fabs(x[0]) < eps) {
    return 0;
  } else {
    return (x[0] > 0)?1:-1;
  }
}


///fn+not+name not
///fn+not+desc Returns one if the first argument $x$ is zero and zero otherwise.
///fn+not+desc The second optional argument $\epsilon$ gives the precision of the "zero"
///fn+not+desc evaluation. If not given, default is $\epsilon = 10^{-16}$.
///fn+not+usage not(x, [eps])
///fn+not+math  \begin{cases}0 &\text{if $|x| < \epsilon$} \\ 1 &\text{otherwise} \end{cases}
double feenox_builtin_not(expr_factor_t *expr) {
  double x[2];
  double eps = 1e-16;

  x[0] = feenox_expression_eval(&expr->arg[0]);
  x[1] = feenox_expression_eval(&expr->arg[1]);

  if (expr->arg[1].factors != NULL) {
    eps = x[1];
  }
  if (fabs(x[0]) < eps) {
    return 1;
  } else {
    return 0;
  }
}

///fn+mod+name mod
///fn+mod+desc Returns the remainder of the division between the first argument $a$ and the
///fn+mod+desc second $b$. Both arguments may be non-integral.
///fn+mod+usage mod(a, b)
///fn+mod+math \displaystyle y = a - \left\lfloor \frac{a}{b} \right\rfloor \cdot b
double feenox_builtin_mod(expr_factor_t *expr) {
  double x[2];

  x[0] = feenox_expression_eval(&expr->arg[0]);
  x[1] = feenox_expression_eval(&expr->arg[1]);

  if (x[1] == 0) {
    return 0;
  } else {
    return x[0] - floor(x[0]/x[1])*x[1];
  }
}


///fn+floor+name floor
///fn+floor+desc Returns the largest integral value not greater than the argument $x$.
///fn+floor+usage floor(x)
///fn+floor+math \lfloor x \rfloor
///fn+floor+plotx -2.75 2.75 1e-2
double feenox_builtin_floor(expr_factor_t *expr) {
  double x[1];
  x[0] = feenox_expression_eval(&expr->arg[0]);

  return floor(x[0]);
}


///fn+ceil+name ceil
///fn+ceil+desc Returns the smallest integral value not less than the argument $x$.
///fn+ceil+usage ceil(x)
///fn+ceil+math \lceil x \rceil
///fn+ceil+plotx -2.75 2.75 1e-2
double feenox_builtin_ceil(expr_factor_t *expr) {
  double x[1];
  x[0] = feenox_expression_eval(&expr->arg[0]);

  return ceil(x[0]);
}


///fn+round+name round
///fn+round+desc Rounds the argument $x$ to the nearest integer. Halfway cases are rounded away from zero.
///fn+round+usage round(x)
///fn+round+math \begin{cases} \lceil x \rceil & \text{if $\lceil x \rceil - x < 0.5$} \\ \lceil x \rceil & \text{if $\lceil x \rceil - x = 0.5 \wedge x > 0$} \\ \lfloor x \rfloor & \text{if $x-\lfloor x \rfloor < 0.5$} \\ \lfloor x \rfloor & \text{if $x-\lfloor x \rfloor = 0.5 \wedge x < 0$} \end{cases}
///fn+round+plotx -2.75 2.75 1e-2
double feenox_builtin_round(expr_factor_t *expr) {
  double x[1];
  x[0] = feenox_expression_eval(&expr->arg[0]);

  return round(x[0]);
}


///fn+deadband+name deadband
///fn+deadband+desc Filters the first argument $x$ with a deadband centered at zero with an amplitude
///fn+deadband+desc given by the second argument $a$.
///fn+deadband+usage deadband(x, a)
///fn+deadband+math \begin{cases} 0 & \text{if $| x | \leq a$} \\ x + a & \text{if $x < a$} \\ x - a & \text{if $x > a$} \end{cases}
double feenox_builtin_deadband(expr_factor_t *expr) {
  double x[2];
  x[0] = feenox_expression_eval(&expr->arg[0]);
  x[1] = feenox_expression_eval(&expr->arg[1]);

  return (fabs(x[0])<x[1])?0:(x[0]+((x[0]>0)?(-1):1)*x[1]);
}

///fn+lag+name lag
///fn+lag+desc Filters the first argument $x(t)$ with a first-order lag of characteristic time $\tau$,
///fn+lag+desc i.e. this function applies the transfer function
///fn+lag+desc !bt
///fn+lag+desc \[ G(s) = \frac{1}{1 + s\tau} \]
///fn+lag+desc !et
///fn+lag+desc to the time-dependent signal $x(t)$, by assuming that it is constant during the time interval
///fn+lag+desc $[t-\Delta t,t]$ and using the analytical solution of the differential equation for that case
///fn+lag+desc at $t = \Delta t$ with the initial condition $y(0) = y(t-\Delta t)$.
///fn+lag+usage lag(x, tau)
///fn+lag+math x(t) - \Big[ x(t) - y(t-\Delta t) \Big] \cdot \exp\left(-\frac{\Delta t}{\tau}\right)
double feenox_builtin_lag(expr_factor_t *expr) {

  double y;
  double x[2];
  x[0] = feenox_expression_eval(&expr->arg[0]);
  x[1] = feenox_expression_eval(&expr->arg[1]);

  if (expr->aux == NULL) {
    // si es la primera vez que se llama a este lag, allocamos
    // el apuntador interno e inicializamos al valor de entrada
    expr->aux = malloc(3*sizeof(double));
    expr->aux[0] = x[0];
    expr->aux[1] = x[0];
    expr->aux[2] = 0;
  } else if ((int)(feenox_special_var_value(in_static)) || x[1] < ZERO) {
    // si no es la primera vez que se llama a este lag pero estamos
    // en el paso estatico o el tau es muy chiquito, pasa de largo
    // el valor del primer argumento
    expr->aux[0] = x[0];
    expr->aux[1] = x[0];
    expr->aux[2] = 0;
  } else if ((int)round(expr->aux[2]) != (int)(feenox_special_var_value(step_transient))) {
    // si nos llamaron, nos aseguramos de que solamente hagamos el lag
    //   cuando corresponda para evitar problemas en cosas que se llaman
    //   iterativa o implicitamente
    expr->aux[0] = expr->aux[1];
    expr->aux[1] = x[0] - (x[0] - expr->aux[0])*exp(-feenox_special_var_value(dt)/x[1]);
    expr->aux[2] = (int)(feenox_special_var_value(step_transient));
  }

  // si termino la corrida rompemos todo para que si tenemos que volver
  // a arrcancar, empiece todo como si nada
  if (feenox_special_var_value(done)) {
    double dummy = expr->aux[1];
    free(expr->aux);
    expr->aux = NULL;
    return dummy;
  }

  y = expr->aux[1];

  if (feenox_special_var_value(done)) {
    free(expr->aux);
    expr->aux = NULL;
  }
  return y;
}

///fn+lag_euler+name lag_euler
///fn+lag_euler+desc Filters the first argument $x(t)$ with a first-order lag of characteristic time $\tau$,
///fn+lag_euler+desc i.e. this function applies the transfer function
///fn+lag_euler+desc !bt
///fn+lag_euler+desc \[ G(s) = \frac{1}{1 + s\tau} \]
///fn+lag_euler+desc !et
///fn+lag_euler+desc to the time-dependent signal $x(t)$ by using the Euler forward rule.
///fn+lag_euler+usage lag_euler(x, tau)
///fn+lag_euler+math y(t-\Delta t) + \Big[ x(t) - x(t - \Delta t) \Big] \cdot \frac{\Delta t}{\tau}
double feenox_builtin_lag_euler(expr_factor_t *expr) {

  double y;
  double x[2];
  x[0] = feenox_expression_eval(&expr->arg[0]);
  x[1] = feenox_expression_eval(&expr->arg[1]);

  if (expr->aux == NULL) {
    expr->aux = malloc(5*sizeof(double));
    expr->aux[0] = x[0];
    expr->aux[1] = x[0];
    expr->aux[2] = x[0];
    expr->aux[3] = x[0];
    expr->aux[4] = 0;
  } else if ((int)(feenox_special_var_value(in_static))) {
    expr->aux[0] = x[0];
    expr->aux[1] = x[0];
    expr->aux[2] = x[0];
    expr->aux[3] = x[0];
    expr->aux[4] = 0;
  } else if ((int)round(expr->aux[4]) != (int)(feenox_special_var_value(step_transient))) {
    expr->aux[0] = expr->aux[1];
    expr->aux[1] = x[0];
    expr->aux[2] = expr->aux[3];
    expr->aux[3] = expr->aux[2] + feenox_special_var_value(dt)/x[1]*(x[0]-expr->aux[2]);
    expr->aux[4] = (int)(feenox_special_var_value(step_transient));
  }

  y = expr->aux[3];

  if (feenox_special_var_value(done)) {
    free(expr->aux);
    expr->aux = NULL;
  }
  return y;
}

///fn+lag_bilinear+name lag_bilinear
///fn+lag_bilinear+desc Filters the first argument $x(t)$ with a first-order lag of characteristic time $\tau$,
///fn+lag_bilinear+desc i.e. this function applies the transfer function
///fn+lag_bilinear+desc !bt
///fn+lag_bilinear+desc \[ G(s) = \frac{1}{1 + s\tau} \]
///fn+lag_bilinear+desc !et
///fn+lag_bilinear+desc to the time-dependent signal $x(t)$ by using the bilinear transformation formula.
///fn+lag_bilinear+usage lag_bilinear(x, tau)
///fn+lag_bilinear+math y = y(t-\Delta t) \cdot \left[ 1 - \frac{\Delta t}{2\tau} \right] + \left[ \frac{x(t) + x(t - \Delta t)}{1 + \frac{\Delta t}{2\tau}}\right] \cdot \frac{\Delta t}{2\tau}
double feenox_builtin_lag_bilinear(expr_factor_t *expr) {

  double y;
  double x[2];
  x[0] = feenox_expression_eval(&expr->arg[0]);
  x[1] = feenox_expression_eval(&expr->arg[1]);

  if (expr->aux == NULL) {
    expr->aux = malloc(5*sizeof(double));
    expr->aux[0] = x[0];
    expr->aux[1] = x[0];
    expr->aux[2] = x[0];
    expr->aux[3] = x[0];
    expr->aux[4] = 0;
  } else if ((int)(feenox_special_var_value(in_static))) {
    expr->aux[0] = x[0];
    expr->aux[1] = x[0];
    expr->aux[2] = x[0];
    expr->aux[3] = x[0];
    expr->aux[4] = 0;
  } else if ((int)round(expr->aux[4]) != (int)(feenox_special_var_value(step_transient))) {
    expr->aux[0] = expr->aux[1];
    expr->aux[1] = x[0];
    expr->aux[2] = expr->aux[3];
    expr->aux[3] = (expr->aux[2] * (1 - 0.5*feenox_special_var_value(dt)/x[1]) + 0.5*feenox_special_var_value(dt)/x[1]*(x[0] + expr->aux[0]))/(1 + 0.5*feenox_special_var_value(dt)/x[1]);
    expr->aux[4] = (int)(feenox_special_var_value(step_transient));
  }

  y = expr->aux[3];

  if (feenox_special_var_value(done)) {
    free(expr->aux);
    expr->aux = NULL;
  }
  return y;
}


// lead(x, tau) implementa la fucion de transferencia s.tau/(1+s.tau)
/*
double feenox_builtin_lead(expr_factor_t *expr) {

  double y;
  double x[2];
  x[0] = feenox_expression_eval(&expr->arg[0]);
  x[1] = feenox_expression_eval(&expr->arg[1]);

  if (expr->aux == NULL) {
    expr->aux = malloc(3*sizeof(double));
    expr->aux[0] = x[0];
    expr->aux[1] = x[0];
    expr->aux[2] = 0;
  } else if ((int)(feenox_special_var_value(in_static)))) {
    expr->aux[0] = x[0];
    expr->aux[1] = x[0];
    expr->aux[2] = 0;
  } else if ((int)round(expr->aux[2]) != (int)(feenox_special_var_value(step_transient)))) {
    expr->aux[0] = expr->aux[1];
    expr->aux[1] = (x[0] - expr->aux[0])/(0.5*feenox_special_var_value(dt))/x[1] + 1) - (0.5*feenox_special_var_value(dt))/x[1] - 1)/(0.5*feenox_special_var_value(dt))/x[1] + 1) * (expr->aux[0]);
    expr->aux[2] = (int)(feenox_special_var_value(step_transient)));
  }

  y = expr->aux[1];

  if (feenox_special_var_value(done))) {
    free(expr->aux);
    expr->aux = NULL;
  }
  return y;
}
*/

// deriv_lag(x, tau) implementa la fucion de transferencia s.tau/(1+s.tau)
/*
double feenox_builtin_lead_euler(algebraic_token_t *expr) {
  if (expr->nofirst_time) {
    return (expr->arg_value[0] - expr->arg_last_value[0])/(0.5*feenox_special_var_value(dt))/expr->arg_value[1] + 1) - (0.5*feenox_special_var_value(dt))/expr->arg_value[1] - 1)/(0.5*feenox_special_var_value(dt))/expr->arg_value[1] + 1) * (expr->last_value);
  } else {
    expr->nofirst_time = 1;
    return 0;
  }
}


// deriv_lag(x, tau) implementa la fucion de transferencia s.tau/(1+s.tau)
double feenox_builtin_lead_bilinear(algebraic_token_t *expr) {
  if (expr->nofirst_time) {
    return (expr->arg_value[0] - expr->arg_last_value[0])/(0.5*feenox_special_var_value(dt))/expr->arg_value[1] + 1) - (0.5*feenox_special_var_value(dt))/expr->arg_value[1] - 1)/(0.5*feenox_special_var_value(dt))/expr->arg_value[1] + 1) * (expr->last_value);
  } else {
    expr->nofirst_time = 1;
    return 0;
  }
}
*/

///fn+equal+name equal
///fn+equal+desc Checks if the two first expressions $a$ and $b$ are equal, up to the tolerance
///fn+equal+desc given by the third optional argument $\epsilon$. If either $|a|>1$ or $|b|>1$,
///fn+equal+desc the arguments are compared using GSL's `gsl_fcmp`, otherwise the
///fn+equal+desc absolute value of their difference is compared against $\epsilon$. This function
///fn+equal+desc returns \textsl{exactly} zero if the arguments are not equal and one otherwise.
///fn+equal+desc Default value for $\epsilon = 10^{-16}$.
///fn+equal+usage equal(a, b, [eps])
///fn+equal+math \begin{cases} 1 & \text{if $a = b$} \\ 0 & \text{if $a \neq b$} \end{cases}
double feenox_builtin_equal(expr_factor_t *expr) {
  double eps = 1e-16;
  double x[3];
  x[0] = feenox_expression_eval(&expr->arg[0]);
  x[1] = feenox_expression_eval(&expr->arg[1]);
  x[2] = feenox_expression_eval(&expr->arg[2]);


  if (expr->arg[2].factors != NULL) {
    eps = x[2];
  }

  if (fabs(x[0]) < 1 || fabs(x[1]) < 1) {
    return (fabs(x[0]-x[1]) < eps)?1:0;
  } else {
    return (gsl_fcmp(x[0], x[1], eps) == 0)?1:0;
  }


}



///fn+random+name random
///fn+random+desc Returns a random real number uniformly distributed between the first
///fn+random+desc real argument $x_1$ and the second one $x_2$.
///fn+random+desc If the third integer argument $s$ is given, it is used as the seed and thus
///fn+random+desc repetitive sequences can be obtained. If no seed is provided, the current time
///fn+random+desc (in seconds) plus the internal address of the expression is used. Therefore,
///fn+random+desc two successive calls to the function without seed (hopefully) do not give the same result.
///fn+random+desc This function uses a second-order multiple recursive generator described by
///fn+random+desc Knuth in Seminumerical Algorithms, 3rd Ed., Section 3.6.
///fn+random+usage random(x1, x2, [s])
///fn+random+math  x_1 + r \cdot (x_2-x_1) \quad \quad 0 \leq r < 1
double feenox_builtin_random(expr_factor_t *expr) {

  double y;
  double x[3];
  x[0] = feenox_expression_eval(&expr->arg[0]);
  x[1] = feenox_expression_eval(&expr->arg[1]);
  x[2] = feenox_expression_eval(&expr->arg[2]);

  // si es la primera llamada inicializamos el generador
  if (expr->aux == NULL) {
    expr->aux = (double *)gsl_rng_alloc(DEFAULT_RANDOM_METHOD);
    // si nos dieron tercer argumento, lo usamos como semilla, sino usamos time()
    if (expr->arg[2].factors == NULL) {
      gsl_rng_set((gsl_rng *)expr->aux, (unsigned long int)(time(NULL)) + (unsigned long int)(&expr->aux));
    } else {
      gsl_rng_set((gsl_rng *)expr->aux, (unsigned long int)(x[2]));
    }
  }


  // TODO: memory leaks en fiteo, minimizacion, etc
  y = x[0] + gsl_rng_uniform((const gsl_rng *)expr->aux)*(x[1]-x[0]);

  if (feenox_special_var_value(done)) {
    gsl_rng_free((gsl_rng *)expr->aux);
    expr->aux = NULL;
  }

  return y;

  // TODO: no camina con seed y fit al mismo tiempo

}


///fn+random_gauss+name random_gauss
///fn+random_gauss+desc Returns a random real number with a Gaussian distribution with a mean
///fn+random_gauss+desc equal to the first argument $x_1$ and a standard deviation equatl to the second one $x_2$.
///fn+random_gauss+desc If the third integer argument $s$ is given, it is used as the seed and thus
///fn+random_gauss+desc repetitive sequences can be obtained. If no seed is provided, the current time
///fn+random_gauss+desc (in seconds) plus the internal address of the expression is used. Therefore,
///fn+random_gauss+desc two successive calls to the function without seed (hopefully) do not give the same result.
///fn+random_gauss+desc This function uses a second-order multiple recursive generator described by
///fn+random_gauss+desc Knuth in Seminumerical Algorithms, 3rd Ed., Section 3.6.
///fn+random_gauss+usage random_gauss(x1, x2, [s])
double feenox_builtin_random_gauss(expr_factor_t *expr) {

  double x[3];
  x[0] = feenox_expression_eval(&expr->arg[0]);
  x[1] = feenox_expression_eval(&expr->arg[1]);
  x[2] = feenox_expression_eval(&expr->arg[2]);

  // si es la primera llamada inicializamos el generador
  if (expr->aux == NULL) {
    expr->aux = (double *)gsl_rng_alloc(DEFAULT_RANDOM_METHOD);
    // si nos dieron tercer argumento, lo usamos como semilla, sino usamos time()
    if (expr->arg[2].factors == NULL) {
      gsl_rng_set((const gsl_rng *)expr->aux, (unsigned long int)(time(NULL)) + (unsigned long int)(&expr->aux));
//      gsl_rng_set((const gsl_rng *)expr->aux, (unsigned long int)(time(NULL)));
    } else {
      gsl_rng_set((const gsl_rng *)expr->aux, (unsigned long int)(x[2]));
    }
  }

  return x[0] + gsl_ran_gaussian((const gsl_rng *)expr->aux, x[1]);

  // TODO: no camina con seed y fit al mismo tiempo

}

///fn+limit+name limit
///fn+limit+desc Limits the first argument $x$ to the interval $[a,b]$. The second argument $a$ should
///fn+limit+desc be less than the third argument $b$.
///fn+limit+usage limit(x, a, b)
///fn+limit+math \begin{cases} a & \text{if $x < a$} \\ x & \text{if $a \leq x \leq b$} \\ b & \text{if $x > b$} \end{cases}
double feenox_builtin_limit(expr_factor_t *expr) {
  double x[3];
  x[0] = feenox_expression_eval(&expr->arg[0]);
  x[1] = feenox_expression_eval(&expr->arg[1]);
  x[2] = feenox_expression_eval(&expr->arg[2]);

  if (x[0] < x[1]) {
    return x[1];
  } else if (x[0] > x[2]) {
    return x[2];
  }

  return x[0];
}

///fn+limit_dt+name limit_dt
///fn+limit_dt+desc Limits the value of the first argument $x(t)$ so to that its time derivative
///fn+limit_dt+desc is bounded to the interval $[a,b]$. The second argument $a$ should
///fn+limit_dt+desc be less than the third argument $b$.
///fn+limit_dt+usage limit_dt(x, a, b)
///fn+limit_dt+math \begin{cases} x(t) & \text{if $a \leq dx/dt \leq b$} \\ x(t-\Delta t) + a \cdot \Delta t & \text{if $dx/dt < a$} \\ x(t-\Delta t) + b \cdot \Delta t & \text{if $dx/dt > b$} \end{cases}
double feenox_builtin_limit_dt(expr_factor_t *expr) {

  double y;
  double derivative;
  double x[3];
  x[0] = feenox_expression_eval(&expr->arg[0]);
  x[1] = feenox_expression_eval(&expr->arg[1]);
  x[2] = feenox_expression_eval(&expr->arg[2]);

  if (feenox_special_var_value(dt) == 0) {
    feenox_nan_error();
    return 0;
  }

  if (expr->aux == NULL) {
    expr->aux = malloc(3*sizeof(double));
    expr->aux[0] = x[0];
    expr->aux[1] = x[0];
    expr->aux[2] = 0;
  } else if ((int)(feenox_special_var_value(in_static))) {
    expr->aux[0] = x[0];
    expr->aux[1] = x[0];
    expr->aux[2] = 0;
  } else if ((int)round(expr->aux[2]) != (int)(feenox_special_var_value(step_transient))) {
    expr->aux[0] = expr->aux[1];
    expr->aux[1] = x[0];
    expr->aux[2] = (int)(feenox_special_var_value(step_transient));
  }

  derivative = (x[0] - expr->aux[0])/feenox_special_var_value(dt);


//  if (!expr->nofirst_time) {
//    return x[0];
//  }

  if (derivative < x[1]) {
    y = expr->aux[0] + x[1]*feenox_special_var_value(dt);
  } else if (derivative > x[2]) {
    y = expr->aux[0] + x[2]*feenox_special_var_value(dt);
  } else {
    y = x[0];
  }

  if (feenox_special_var_value(done)) {
    free(expr->aux);
    expr->aux = NULL;
  }
  return y;
}

///fn+if+name if
///fn+if+desc Performs a conditional testing of the first argument $a$, and returns either the
///fn+if+desc second optional argument $b$ if $a$ is different from zero or the third optional argument $c$
///fn+if+desc if $a$ evaluates to zero. The comparison of the condition $a$ with zero is performed
///fn+if+desc within the precision given by the optional fourth argument $\epsilon$.
///fn+if+desc If the second argument $c$ is not given and $a$ is not zero, the function returns one.
///fn+if+desc If the third argument $c$ is not given and $a$ is zero, the function returns zero.
///fn+if+desc The default precision is $\epsilon = 10^{-16}$.
///fn+if+desc Even though `if` is a logical operation, all the arguments and the returned value
///fn+if+desc are double-precision floating point numbers.
///fn+if+usage if(a, [b], [c], [eps])
///fn+if+math \begin{cases} b & \text{if $a \neq 0$} \\ c & \text{if $a = b$} \end{cases}

double feenox_builtin_if(expr_factor_t *expr) {
  double eps = 1e-16;
  double x[4];
  x[0] = feenox_expression_eval(&expr->arg[0]);

  if (expr->arg[3].factors != NULL) {
    eps = feenox_expression_eval(&expr->arg[3]);
  }

  if (fabs(x[0]) > eps) {
    return (expr->arg[1].factors != NULL) ? feenox_expression_eval(&expr->arg[1]) : 1.0;
  } else {
    return (expr->arg[2].factors != NULL) ? feenox_expression_eval(&expr->arg[2]) : 0.0;
  }

}


///fn+is_in_interval+name is_in_interval
///fn+is_in_interval+desc Returns true if the argument~$x$ is in the interval~$[a,b)$, i.e. including~$a$
///fn+is_in_interval+desc but excluding~$b$.
///fn+is_in_interval+usage is_in_interval(x, a, b)
///fn+is_in_interval+math \begin{cases} 1 & \text{if $a \leq x < b$} \\ 0 & \text{otherwise} \end{cases}
double feenox_builtin_is_in_interval(expr_factor_t *expr) {

  double y;
  double x[3];
  x[0] = feenox_expression_eval(&expr->arg[0]);
  x[1] = feenox_expression_eval(&expr->arg[1]);
  x[2] = feenox_expression_eval(&expr->arg[2]);

  y = (x[0] >= x[1] && x[0] < x[2]);

  return y;
}



///fn+threshold_max+name threshold_max
///fn+threshold_max+desc Returns one if the first argument $x$ is greater than the threshold given by
///fn+threshold_max+desc the second argument $a$, and \textit{exactly} zero otherwise. If the optional
///fn+threshold_max+desc third argument $b$ is provided, an hysteresis of width $b$ is needed in order
///fn+threshold_max+desc to reset the function value. Default is no hysteresis, i.e. $b=0$.
///fn+threshold_max+usage threshold_max(x, a, [b])
///fn+threshold_max+math \begin{cases} 1 & \text{if $x > a$} \\ 0 & \text{if $x < a-b$} \\ \text{last value of $y$} & \text{otherwise} \end{cases}

double feenox_builtin_threshold_max(expr_factor_t *expr) {

  double y;
  double x[3];
  x[0] = feenox_expression_eval(&expr->arg[0]);
  x[1] = feenox_expression_eval(&expr->arg[1]);
  x[2] = feenox_expression_eval(&expr->arg[2]);

  if (expr->aux == NULL) {
    expr->aux = malloc(1*sizeof(double));
    expr->aux[0] = x[0] > x[1];
  }

  if (x[0] > x[1]) {
    expr->aux[0] = 1;
  } else if (x[0] < (x[1]-x[2])) {
    expr->aux[0] = 0;
  }
  y = expr->aux[0];

  if (feenox_special_var_value(done)) {
    free(expr->aux);
    expr->aux = NULL;
  }
  return y;

}


///fn+threshold_min+name threshold_min
///fn+threshold_min+desc Returns one if the first argument $x$ is less than the threshold given by
///fn+threshold_min+desc the second argument $a$, and \textit{exactly} zero otherwise. If the optional
///fn+threshold_min+desc third argument $b$ is provided, an hysteresis of width $b$ is needed in order
///fn+threshold_min+desc to reset the function value. Default is no hysteresis, i.e. $b=0$.
///fn+threshold_min+usage threshold_min(x, a, [b])
///fn+threshold_min+math \begin{cases} 1 & \text{if $x < a$} \\ 0 & \text{if $x > a+b$} \\ \text{last value of $y$} & \text{otherwise} \end{cases}

double feenox_builtin_threshold_min(expr_factor_t *expr) {

  double y;
  double x[3];
  x[0] = feenox_expression_eval(&expr->arg[0]);
  x[1] = feenox_expression_eval(&expr->arg[1]);
  x[2] = feenox_expression_eval(&expr->arg[2]);

  if (expr->aux == NULL) {
    expr->aux = malloc(1*sizeof(double));
    expr->aux[0] = x[0] > x[1];
  }

  if (x[0] < x[1]) {
    expr->aux[0] = 1;
  } else if (x[0] > (x[1]+x[2])) {
    expr->aux[0] = 0;
  }

  y = expr->aux[0];

  if (feenox_special_var_value(done)) {
    free(expr->aux);
    expr->aux = NULL;
  }
  return y;
}


///fn+min+name min
///fn+min+desc Returns the minimum of the $n$ arguments $x_i$ provided.
///fn+min+usage min(x1, x2, [...], [x10])
///fn+min+math  \min \Big (x_1, x_2, \dots, x_{10} \Big)

double feenox_builtin_min(expr_factor_t *expr) {
  int i;
  double min = feenox_expression_eval(&expr->arg[0]);

  for (i = 1; i < MINMAX_ARGS; i++) {
    if (expr->arg[i].factors != NULL && feenox_expression_eval(&expr->arg[i]) < min) {
      min = feenox_expression_eval(&expr->arg[i]);
    }
  }
  return min;
}

///fn+max+name max
///fn+max+desc Returns the maximum of the arguments $x_i$ provided. Currently only maximum of ten arguments can be provided.
///fn+max+usage max(x1, x2, [...], [x10])
///fn+max+math  \max \Big (x_1, x_2, \dots, x_{10} \Big)

double feenox_builtin_max(expr_factor_t *expr) {
  int i;
  double max = feenox_expression_eval(&expr->arg[0]);

  for (i = 1; i < MINMAX_ARGS; i++) {
    if (expr->arg[i].factors != NULL && feenox_expression_eval(&expr->arg[i]) > max) {
      max = feenox_expression_eval(&expr->arg[i]);
    }
  }
  return max;
}


///fn+mark_min+name mark_min
///fn+mark_min+desc Returns the integer index $i$ of the minimum of the arguments $x_i$ provided. Currently only maximum of ten arguments can be provided.
///fn+mark_min+usage mark_max(x1, x2, [...], [x10])
///fn+mark_min+math  i / \min \Big (x_1, x_2, \dots, x_{10} \Big) = x_i

double feenox_builtin_mark_min(expr_factor_t *expr) {
  int i;
  int i_min = 0;
  double min = feenox_expression_eval(&expr->arg[0]);

  for (i = 1; i < MINMAX_ARGS; i++) {
    if (expr->arg[i].factors != NULL && feenox_expression_eval(&expr->arg[i]) < min) {
      min = feenox_expression_eval(&expr->arg[i]);
      i_min = i;
    }
  }
  return i_min+1;
}

///fn+mark_max+name mark_max
///fn+mark_max+desc Returns the integer index $i$ of the maximum of the arguments $x_i$ provided. Currently only maximum of ten arguments can be provided.
///fn+mark_max+usage mark_max(x1, x2, [...], [x10])
///fn+mark_max+math  i / \max \Big (x_1, x_2, \dots, x_{10} \Big) = x_i

double feenox_builtin_mark_max(expr_factor_t *expr) {
  int i;
  int i_max = 0;
  double max = feenox_expression_eval(&expr->arg[0]);

  for (i = 1; i < MINMAX_ARGS; i++) {
    if (expr->arg[i].factors != NULL && feenox_expression_eval(&expr->arg[i]) > max) {
      max = feenox_expression_eval(&expr->arg[i]);
      i_max = i;
    }
  }
  return i_max+1;
}
