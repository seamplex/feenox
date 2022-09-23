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

#include <stdlib.h>
#include <limits.h>
#ifdef HAVE_CLOCK_GETTIME
 #include <time.h>
#else
 #include <mach/clock.h>
 #include <mach/mach.h>
#endif

#ifdef HAVE_GETRUSAGE
 #include <sys/resource.h>
#endif


// plotx reference
//  1. min
//  2. max
//  3. step
//  4. minxtics
//  5. maxxtics
//  6. stepxtics
//  7. minytics
//  8. maxytics
//  9. stepytics


double feenox_builtin_abs(expr_item_t *);
double feenox_builtin_acos(expr_item_t *);
double feenox_builtin_asin(expr_item_t *);
double feenox_builtin_atan(expr_item_t *);
double feenox_builtin_atan2(expr_item_t *);
double feenox_builtin_ceil(expr_item_t *);
double feenox_builtin_clock(expr_item_t *);
double feenox_builtin_cos(expr_item_t *);
double feenox_builtin_cosh(expr_item_t *);
double feenox_builtin_cpu_time(expr_item_t *);
double feenox_builtin_d_dt(expr_item_t *);
double feenox_builtin_deadband(expr_item_t *);
double feenox_builtin_equal(expr_item_t *);
double feenox_builtin_exp(expr_item_t *);
double feenox_builtin_expint1(expr_item_t *);
double feenox_builtin_expint2(expr_item_t *);
double feenox_builtin_expint3(expr_item_t *);
double feenox_builtin_expintn(expr_item_t *);
double feenox_builtin_floor(expr_item_t *);
double feenox_builtin_heaviside(expr_item_t *);
double feenox_builtin_if(expr_item_t *);
double feenox_builtin_is_in_interval(expr_item_t *);
double feenox_builtin_integral_dt(expr_item_t *);
double feenox_builtin_integral_euler_dt(expr_item_t *);
double feenox_builtin_lag(expr_item_t *);
double feenox_builtin_lag_bilinear(expr_item_t *);
double feenox_builtin_lag_euler(expr_item_t *);
double feenox_builtin_last(expr_item_t *);
double feenox_builtin_limit(expr_item_t *);
double feenox_builtin_limit_dt(expr_item_t *);
double feenox_builtin_log(expr_item_t *);
double feenox_builtin_mark_max(expr_item_t *);
double feenox_builtin_mark_min(expr_item_t *);
double feenox_builtin_max(expr_item_t *);
double feenox_builtin_memory(expr_item_t *);
double feenox_builtin_min(expr_item_t *);
double feenox_builtin_mod(expr_item_t *);
double feenox_builtin_not(expr_item_t *);
double feenox_builtin_quasi_random(expr_item_t *);
double feenox_builtin_random(expr_item_t *);
double feenox_builtin_random_gauss(expr_item_t *);
double feenox_builtin_round(expr_item_t *);
double feenox_builtin_sawtooth_wave(expr_item_t *);
double feenox_builtin_sgn(expr_item_t *);
double feenox_builtin_is_even(expr_item_t *);
double feenox_builtin_is_odd(expr_item_t *);
double feenox_builtin_sin(expr_item_t *);
double feenox_builtin_j0(expr_item_t *);
double feenox_builtin_sinh(expr_item_t *);
double feenox_builtin_sqrt(expr_item_t *);
double feenox_builtin_square_wave(expr_item_t *);
double feenox_builtin_tan(expr_item_t *);
double feenox_builtin_tanh(expr_item_t *);
double feenox_builtin_threshold_max(expr_item_t *);
double feenox_builtin_threshold_min(expr_item_t *);
double feenox_builtin_triangular_wave(expr_item_t *);
double feenox_builtin_wall_time(expr_item_t *);

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
    {"cpu_time",            0, 1, &feenox_builtin_cpu_time},
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
    {"j0",                  1, 1, &feenox_builtin_j0},
    {"mark_max",            2, MINMAX_ARGS, &feenox_builtin_mark_max},
    {"mark_min",            2, MINMAX_ARGS, &feenox_builtin_mark_min},
    {"max",                 2, MINMAX_ARGS, &feenox_builtin_max},
    {"memory",              0, 1, &feenox_builtin_memory},
    {"min",                 2, MINMAX_ARGS, &feenox_builtin_min},
    {"mod",                 2, 2, &feenox_builtin_mod},
    {"not",                 1, 2, &feenox_builtin_not},
    {"quasi_random",        2, 2, &feenox_builtin_quasi_random},
    {"random",              2, 3, &feenox_builtin_random},
    {"random_gauss",        2, 3, &feenox_builtin_random_gauss},
    {"round",               1, 1, &feenox_builtin_round},
    {"sawtooth_wave",       1, 1, &feenox_builtin_sawtooth_wave},
    {"sgn",                 1, 2, &feenox_builtin_sgn},
    {"sin",                 1, 1, &feenox_builtin_sin},
    {"sinh",                1, 1, &feenox_builtin_sinh},
    {"sqrt",                1, 1, &feenox_builtin_sqrt},
    {"square_wave",         1, 1, &feenox_builtin_square_wave},
    {"tan",                 1, 1, &feenox_builtin_tan},
    {"tanh",                1, 1, &feenox_builtin_tanh},
    {"threshold_max",       2, 3, &feenox_builtin_threshold_max},
    {"threshold_min",       2, 3, &feenox_builtin_threshold_min},
    {"triangular_wave",     1, 1, &feenox_builtin_triangular_wave},
    {"wall_time",           0, 1, &feenox_builtin_wall_time},
};


///fn+clock+usage clock([f])
///fn+clock+desc Returns the value of a certain clock in seconds measured from a certain (but specific) milestone.
///fn+clock+desc The kind of clock and the initial milestone depend on the optional integer argument\ $f$.
///fn+clock+desc It defaults to one, meaning `CLOCK_MONOTONIC`.
///fn+clock+desc The list and the meanings of the other available values for\ $f$ can be checked
///fn+clock+desc in the `clock_gettime (2)` system call manual page.
double feenox_builtin_clock(expr_item_t *f) {

#ifdef HAVE_CLOCK_GETTIME
  struct timespec tp;

  clockid_t clk_id = (f->arg[0].items != NULL) ? (clockid_t)feenox_expression_eval(&f->arg[0]) : CLOCK_MONOTONIC;

  if (clock_gettime(clk_id, &tp) < 0) {
    feenox_runtime_error();
  }
  return (double)tp.tv_sec + 1e-9 * (double)tp.tv_nsec;
#else
  return 0;
#endif //  HAVE_CLOCK_GETTIME
}

///fn+wall_time+usage wall_time()
///fn+wall_time+desc Returns the time ellapsed since the invocation of FeenoX, in seconds.
double feenox_builtin_wall_time(expr_item_t *f) {

#ifdef HAVE_CLOCK_GETTIME
  struct timespec tp;

  if (clock_gettime(CLOCK_MONOTONIC, &tp) < 0) {
    feenox_runtime_error();
  }
  
  return (double)(tp.tv_sec - feenox.tp0.tv_sec) + 1e-9 * (double)(tp.tv_nsec - feenox.tp0.tv_sec);
#else
  return 0;
#endif //  HAVE_CLOCK_GETTIME
}


///fn+memory+usage memory()
///fn+memory+desc Returns the maximum memory (resident set size) used by FeenoX, in Gigabytes.
double feenox_builtin_memory(expr_item_t *f) {

  double memory_gb = 0;
#ifdef HAVE_GETRUSAGE
  struct rusage usage;
  getrusage(RUSAGE_SELF, &usage);
  memory_gb = (double)(usage.ru_maxrss/(1024.0*1024.0));
#endif //  HAVE_GETRUSAGE
  
  return memory_gb;
}

///fn+cpu_time+usage cpu_time([f])
///fn+cpu_time+desc Returns the CPU time used by FeenoX, in seconds.
///fn+cpu_time+desc If the optional argument `f` is not provided  or it is zero (default),
///fn+cpu_time+desc the sum of times for both user-space and kernel-space usage is returned.
///fn+cpu_time+desc For `f=1` only user time is returned.
///fn+cpu_time+desc For `f=2` only system time is returned.

double feenox_builtin_cpu_time(expr_item_t *f) {

  double cpu_time_seconds = 0;
#ifdef HAVE_GETRUSAGE
  struct rusage usage;
  getrusage(RUSAGE_SELF, &usage);
  double user   = (double)usage.ru_utime.tv_sec + 1e-6 * (double)usage.ru_utime.tv_usec;
  double kernel = (double)usage.ru_stime.tv_sec + 1e-6 * (double)usage.ru_stime.tv_usec;

  switch ((f->arg[0].items != NULL) ? (int)feenox_expression_eval(&f->arg[0]) : 0) {
    case 0:
      cpu_time_seconds = user + kernel;
    break;
    case 1:
      cpu_time_seconds = user;
    break;
    case 2:
      cpu_time_seconds = kernel;
    break;
  }
#endif //  HAVE_GETRUSAGE
  
  return cpu_time_seconds;
}

///fn+last+usage last(x,[p])
///fn+last+math z^{-1}\left[ x \right] = x(t-\Delta t)
///fn+last+desc Returns the value the variable\ $x$ had in the previous time step.
///fn+last+desc This function is equivalent to the\ $Z$-transform operator "delay" denoted by\ $z^{-1}\left[ x\right]$.
///fn+last+desc For\ $t=0$ the function returns the actual value of\ $x$.
///fn+last+desc The optional flag\ $p$ should be set to one if the reference to `last`
///fn+last+desc is done in an assignment over a variable that already appears inside
///fn+last+desc expression\ $x$ such as `x = last(x)`. See example number 2.
double feenox_builtin_last(expr_item_t *f) {

  double y = 0;
  double x = feenox_expression_eval(&f->arg[0]);
  double p = feenox_expression_eval(&f->arg[1]);

  if (f->aux == NULL) {
    f->aux = malloc(3*sizeof(double));
    f->aux[0] = x;
    f->aux[1] = x;
    f->aux[2] = 0;
  } else {
    
    if ((int)feenox_special_var_value(in_static)) {
      if ((int)round(f->aux[2]) != (int)(feenox_special_var_value(step_static))) {
        f->aux[0] = f->aux[1];
        f->aux[1] = x;
        f->aux[2] = (int)(feenox_special_var_value(step_static));
      }
      
    } else {
      if ((int)round(f->aux[2]) != (int)(feenox_special_var_value(step_transient))) {
        f->aux[0] = f->aux[1];
        f->aux[1] = x;
        f->aux[2] = (int)(feenox_special_var_value(step_transient));
      }
    }
  }

  // this is to be able to do x = last(x)
  // TODO: see if there's a better way
  if (p == 0) {
    y = f->aux[0];
  } else {
    y = x;
  }

  if (feenox_special_var_value(done)) {
    feenox_free(f->aux);
  }
  return y;
}

///fn+d_dt+usage d_dt(x)
///fn+d_dt+math \frac{x(t) - x(t-\Delta t)}{\Delta t} \approx \frac{d}{dt} \Big( x (t) \Big)
///fn+d_dt+desc Computes the time derivative of the expression given in the argument\ $x$
///fn+d_dt+desc during a transient problem
///fn+d_dt+desc using the difference between the value of the signal in the previous time step
///fn+d_dt+desc and the actual value divided by the time step\ $\delta t$ stored in `dt`.
///fn+d_dt+desc The argument\ $x$ does not neet to be a variable, it can be an expression
///fn+d_dt+desc involving one or more variables that change in time.
///fn+d_dt+desc For $t=0$, the return value is zero.
///fn+d_dt+desc Unlike the functional `derivative`, the full dependence of these variables with time
///fn+d_dt+desc does not need to be known beforehand, i.e. the expression `x` might involve variables
///fn+d_dt+desc read from a shared-memory object at each time step.
double feenox_builtin_d_dt(expr_item_t *f) {

  double y = 0;
  double x = feenox_expression_eval(&f->arg[0]);

  if (feenox_special_var_value(dt) == 0) {
    feenox_nan_error();
    return 0;
  }

  if (f->aux == NULL) {
    f->aux = malloc(3*sizeof(double));
    f->aux[0] = x;
    f->aux[1] = x;
    f->aux[2] = 0;
  } else if ((int)(feenox_special_var_value(in_static))) {
    f->aux[0] = x;
    f->aux[1] = x;
    f->aux[2] = 0;
  } else if ((int)round(f->aux[2]) != (int)(feenox_special_var_value(step_transient))) {
    f->aux[0] = f->aux[1];
    f->aux[1] = x;
    f->aux[2] = (int)(feenox_special_var_value(step_transient));
  }

  y = (x - f->aux[0])/feenox_special_var_value(dt);

  if (feenox_special_var_value(done)) {
    feenox_free(f->aux);
  }
  return y;
}

///fn+integral_dt+usage integral_dt(x)
///fn+integral_dt+math z^{-1}\left[ \int_0^{t-\Delta t} x(t') \, dt' \right] +  \frac{x(t) + x(t-\Delta t)}{2} \, \Delta t \approx \int_0^{t} x(t') \, dt'
///fn+integral_dt+desc Computes the time integral of the expression given in the argument\ $x$
///fn+integral_dt+desc during a transient problem with the trapezoidal rule
///fn+integral_dt+desc using the value of the signal in the previous time step and the current value.
///fn+integral_dt+desc At $t = 0$ the integral is initialized to zero.
///fn+integral_dt+desc Unlike the functional `integral`, the full dependence of these variables with time
///fn+integral_dt+desc does not need to be known beforehand, i.e. the expression `x` might involve variables
///fn+integral_dt+desc read from a shared-memory object at each time step.
double feenox_builtin_integral_dt(expr_item_t *f) {

  double y;
  double x = feenox_expression_eval(&f->arg[0]);

  if (f->aux == NULL) {
    f->aux = malloc(4*sizeof(double));
    f->aux[0] = x;
    f->aux[1] = x;
    f->aux[2] = 0;
    f->aux[3] = 0;
  } else if ((int)(feenox_special_var_value(in_static))) {
    f->aux[0] = x;
    f->aux[1] = x;
    f->aux[2] = 0;
    f->aux[3] = 0;
  } else if ((int)round(f->aux[3]) != (int)(feenox_special_var_value(step_transient))) {
    f->aux[0] = f->aux[1];
    f->aux[1] = x;
    f->aux[2] += 0.5*(f->aux[0]+x)*feenox_special_var_value(dt);
    f->aux[3] = (int)(feenox_special_var_value(step_transient));
  }

  y = f->aux[2];

  if (feenox_special_var_value(done)) {
    feenox_free(f->aux);
  }
  return y;
}

///fn+integral_euler_dt+math z^{-1}\left[ \int_0^{t-\Delta t} x(t') \, dt' \right] +   x(t) \, \Delta t \approx \int_0^{t} x(t') \, dt'
///fn+integral_euler_dt+usage integral_euler_dt(x)
///fn+integral_euler_dt+desc Idem as `integral_dt` but uses the backward Euler rule to update the instantaenous integral value.
///fn+integral_euler_dt+desc This function is provided in case this particular way
///fn+integral_euler_dt+desc of approximating time integrals is needed,
///fn+integral_euler_dt+desc for instance to compare FeenoX solutions with other computer codes.
///fn+integral_euler_dt+desc In general, it is recommended to use `integral_dt`.
double feenox_builtin_integral_euler_dt(expr_item_t *f) {

  double y;
  double x = feenox_expression_eval(&f->arg[0]);

  if (f->aux == NULL) {
    f->aux = malloc(2*sizeof(double));
    f->aux[0] = 0;
    f->aux[1] = 0;
  } else if ((int)(feenox_special_var_value(in_static))) {
    f->aux[0] = 0;
    f->aux[1] = 0;
  } else if ((int)round(f->aux[1]) != (int)(feenox_special_var_value(step_transient))) {
    f->aux[0] += x*feenox_special_var_value(dt);
    f->aux[1] = (int)(feenox_special_var_value(step_transient));
  }

  y = f->aux[0];

  if (feenox_special_var_value(done)) {
    feenox_free(f->aux);
  }
  return y;
}

///fn+square_wave+usage square_wave(x)
///fn+square_wave+desc Computes a square function between zero and one with a period equal to one.
///fn+square_wave+desc The output is one for $0 < x < 1/2$ and zero for $1/2 \leq x < 1$.
///fn+square_wave+desc As with the sine wave, a square wave can be generated by passing as the argument\ $x$
///fn+square_wave+desc a linear function of time such as\ $\omega t+\phi$, where\ $\omega$ 
///fn+square_wave+desc controls the frequency of the wave and\ $\phi$ controls its phase.
///fn+square_wave+math \begin{cases} 1 & \text{if $x - \lfloor x \rfloor < 0.5$} \\ 0 & \text{otherwise} \end{cases}
///fn+square_wave+plotx 0 2.75 1e-3    0 3 1     0 1 0.5    0.25  0.25
double feenox_builtin_square_wave(expr_item_t *f) {
  double x = feenox_expression_eval(&f->arg[0]);
  return ((x - floor(x)) < 0.5);
}

///fn+triangular_wave+usage triangular_wave(x)
///fn+triangular_wave+math \begin{cases} 2 (x - \lfloor x \rfloor) & \text{if $x - \lfloor x \rfloor < 0.5$} \\ 2 [1-(x - \lfloor x \rfloor)] & \text{otherwise} \end{cases}
///fn+triangular_wave+desc Computes a triangular wave between zero and one with a period equal to one.
///fn+triangular_wave+desc As with the sine wave, a triangular wave can be generated by passing as the argument\ $x$
///fn+triangular_wave+desc a linear function of time such as\ $\omega t+\phi$, where\ $\omega$
///fn+triangular_wave+desc controls the frequency of the wave and\ $\phi$ controls its phase.
///fn+triangular_wave+plotx 0 2.75 1e-2    0 3 1     0 1 0.5    0.25  0.25
double feenox_builtin_triangular_wave(expr_item_t *f) {
  double x = feenox_expression_eval(&f->arg[0]);

  if ((x - floor(x)) < 0.5) {
    return 2*(x - floor(x));
  } else {
    return 2*(1 - (x - floor(x)));
  }
}

///fn+sawtooth_wave+usage sawtooth_wave(x)
///fn+sawtooth_wave+math x - \lfloor x \rfloor
///fn+sawtooth_wave+desc Computes a sawtooth wave between zero and one with a period equal to one.
///fn+sawtooth_wave+desc As with the sine wave, a sawtooh wave can be generated by passing as the argument\ $x$
///fn+sawtooth_wave+desc a linear function of time such as\ $\omega t+\phi$, where\ $\omega$ controls 
///fn+sawtooth_wave+desc the frequency of the wave and $\phi$ controls its phase.
///fn+sawtooth_wave+plotx 0 2.75 1e-3    0 3 1     0 1 0.5    0.25  0.25
double feenox_builtin_sawtooth_wave(expr_item_t *f) {
  double x = feenox_expression_eval(&f->arg[0]);
  return x - floor(x);
}

///fn+sin+usage sin(x)
///fn+sin+math \sin(x)
///fn+sin+desc Computes the sine of the argument\ $x$, where\ $x$ is in radians.
///fn+sin+desc A sine wave can be generated by passing as the argument\ $x$
///fn+sin+desc a linear function of time such as\ $\omega t+\phi$, where\ $\omega$ controls the frequency of the wave
///fn+sin+desc and\ $\phi$ controls its phase.
///fn+sin+plotx -2*pi 2*pi pi/100     -6 +6 2     -1 +1 0.5   1  0.25
double feenox_builtin_sin(expr_item_t *f) {
  return sin(feenox_expression_eval(&f->arg[0]));
}

///fn+asin+usage asin(x)
///fn+asin+math \arcsin(x)
///fn+asin+desc Computes the arc in radians whose sine is equal to the argument\ $x$.
///fn+asin+desc A NaN error is raised if\ $|x|>1$.
///fn+asin+plotx -1 1 1/100      -1 1 0.5     -1.5 1.5 1    0.25 0.5
double feenox_builtin_asin(expr_item_t *f) {
  double x = feenox_expression_eval(&f->arg[0]);

  if (fabs(x) > 1.0) {
    feenox_push_error_message("argument greater than one to function asin");
    feenox_nan_error();
    return 0;
  }

  return asin(x);
}


///fn+acos+usage acos(x)
///fn+acos+math \arccos(x)
///fn+acos+desc Computes the arc in radians whose cosine is equal to the argument\ $x$.
///fn+acos+desc A NaN error is raised if\ $|x|>1$.
///fn+acos+plotx -1 1 1/100   -1 1 0.5    0 3.5 1   0.25 0.5
double feenox_builtin_acos(expr_item_t *f) {
  double x = feenox_expression_eval(&f->arg[0]);

  if (fabs(x) > 1.0) {
    feenox_push_error_message("argument greater than one to function acos");
    feenox_nan_error();
    return 0;
  }

  return acos(x);
}

///fn+j0+usage j0(x)
///fn+j0+math J_0(x)
///fn+j0+desc Computes the regular cylindrical Bessel function of zeroth order evaluated at the argument\ $x$.
///fn+j0+plotx 0 10 0.05
double feenox_builtin_j0(expr_item_t *f) {
  return gsl_sf_bessel_J0(feenox_expression_eval(&f->arg[0]));
}

///fn+cos+usage cos(x)
///fn+cos+math \cos(x)
///fn+cos+desc Computes the cosine of the argument\ $x$, where\ $x$ is in radians.
///fn+cos+desc A cosine wave can be generated by passing as the argument\ $x$
///fn+cos+desc a linear function of time such as\ $\omega t+\phi$, where $\omega$ controls 
///fn+cos+desc the frequency of the wave and $\phi$ controls its phase.
///fn+cos+plotx -2*pi 2*pi pi/100     -6 +6 2     -1 +1 0.5   1  0.25
double feenox_builtin_cos(expr_item_t *f) {
  return cos(feenox_expression_eval(&f->arg[0]));
}

///fn+tan+usage tan(x)
///fn+tan+math  \tan(x)
///fn+tan+desc Computes the tangent of the argument\ $x$, where\ $x$ is in radians.
///fn+tan+plotx -1.3 +1.3 1e-2    -1 +1 0.5    -4 +4 2   0.25 0.5
double feenox_builtin_tan(expr_item_t *f) {
  return tan(feenox_expression_eval(&f->arg[0]));
}

///fn+sinh+usage sinh(x)
///fn+sinh+math \sinh(x)
///fn+sinh+desc Computes the hyperbolic sine of the argument\ $x$, where\ $x$ is in radians.
///fn+sinh+plotx -2.5 2.5 1e-2    -3 +3 1    -6 +6 2   0.25 1
double feenox_builtin_sinh(expr_item_t *f) {
  return sinh(feenox_expression_eval(&f->arg[0]));
}

///fn+cosh+usage cosh(x)
///fn+cosh+math \cosh(x)
///fn+cosh+desc Computes the hyperbolic cosine of the argument\ $x$, where\ $x$ is in radians.
///fn+cosh+plotx -1.5 1.5 1e-2     -1.5 +1.5 0.5     1 3 0.5   0.25 0.25
double feenox_builtin_cosh(expr_item_t *f) {
  return cosh(feenox_expression_eval(&f->arg[0]));
}

///fn+tanh+usage tanh(x)
///fn+tanh+math \tanh(x)
///fn+tanh+desc Computes the hyperbolic tangent of the argument\ $x$, where\ $x$ is in radians.
///fn+tanh+plotx -2.5 +2.5 1e-2    -3 +3 1    -1 +1 0.5    0.5 0.25
double feenox_builtin_tanh(expr_item_t *f) {
  return tanh(feenox_expression_eval(&f->arg[0]));
}


///fn+atan+usage atan(x)
///fn+atan+math \arctan(x)
///fn+atan+desc Computes, in radians, the arc tangent of the argument\ $x$.
///fn+atan+plotx -6 6 pi/100    -6 6 2     -1 1 1     1 0.5
double feenox_builtin_atan(expr_item_t *f) {
  return atan(feenox_expression_eval(&f->arg[0]));
}

///fn+atan2+usage atan2(y,x)
///fn+atan2+math \arctan(y/x)
///fn+atan2+desc Computes, in radians, the arc tangent of quotient\ $y/x$, using the signs of the two arguments
///fn+atan2+desc to determine the quadrant of the result, which is in the range $[-\pi,\pi]$.
double feenox_builtin_atan2(expr_item_t *f) {
  return atan2(feenox_expression_eval(&f->arg[0]), feenox_expression_eval(&f->arg[1]));
}

///fn+exp+desc Computes the exponential function the argument\ $x$, i.e. the base of the
///fn+exp+desc natural logarithm\ $e$ raised to the\ $x$-th power.
///fn+exp+usage exp(x)
///fn+exp+math e^x
///fn+exp+plotx -2 2 1e-2    -2 +2 1     0  8  2    0.25  1
double feenox_builtin_exp(expr_item_t *f) {
  double x = feenox_expression_eval(&f->arg[0]);

  return (x < GSL_LOG_DBL_MIN) ? 0 : gsl_sf_exp(x);
}

///fn+expint1+desc Computes the first exponential integral function of the argument\ $x$.
///fn+expint1+desc If\ $x$ is zero, a NaN error is issued.
///fn+expint1+usage expint1(x)
///fn+expint1+math \text{Re} \left[ \int_1^{\infty}\! \frac{\exp(-xt)}{t} \, dt \right]
///fn+expint1+plotx 1e-2 2.0 1e-2
double feenox_builtin_expint1(expr_item_t *f) {
  double x = feenox_expression_eval(&f->arg[0]);

  if (x == 0) {
    feenox_nan_error();
    return 0;
  }

  return gsl_sf_expint_E1(x);
}

///fn+expint2+desc Computes the second exponential integral function of the argument\ $x$.
///fn+expint2+usage expint2(x)
///fn+expint2+math \text{Re} \left[ \int_1^{\infty}\! \frac{\exp(-xt)}{t^2} \, dt \right]
///fn+expint2+plotx 0.0 2.0 1e-2
double feenox_builtin_expint2(expr_item_t *f) {
  return gsl_sf_expint_E2(feenox_expression_eval(&f->arg[0]));
}

///fn+expint3+desc Computes the third exponential integral function of the argument\ $x$.
///fn+expint3+usage expint3(x)
///fn+expint3+math \text{Re} \left[ \int_1^{\infty}\! \frac{\exp(-xt)}{t^3} \, dt \right]
///fn+expint3+plotx 0.0 2.0 1e-2
double feenox_builtin_expint3(expr_item_t *f) {
  return gsl_sf_expint_En(3, feenox_expression_eval(&f->arg[0]));
}

///fn+expintn+desc Computes the $n$-th exponential integral function of the argument\ $x$.
///fn+expintn+desc If\ $n$ is zero or one and\ $x$ is zero, a NaN error is issued.
///fn+expintn+usage expintn(n,x)
///fn+expintn+math \text{Re} \left[ \int_1^{\infty}\! \frac{\exp(-xt)}{t^n} \, dt \right]
double feenox_builtin_expintn(expr_item_t *f) {
  int n;
  n = ((int)(round(feenox_expression_eval(&f->arg[0]))));
  double x = feenox_expression_eval(&f->arg[1]);

  if ((n == 1 || n == 0) && x == 0) {
    feenox_nan_error();
    return 0;
  }

  return gsl_sf_expint_En(n, x);
}

///fn+log+desc Computes the natural logarithm of the argument\ $x$. If\ $x$ is zero or negative,
///fn+log+desc a NaN error is issued.
///fn+log+usage log(x)
///fn+log+math \ln(x)
///fn+log+plotx 0.1 3.75 1e-2    0  4  0.5   -2 1 1     0.25  0.25
double feenox_builtin_log(expr_item_t *f) {
  double x = feenox_expression_eval(&f->arg[0]);

  if (x <= 0) {
    feenox_nan_error();
    return 0;
  }

  return gsl_sf_log(x);
}


///fn+abs+desc Returns the absolute value of the argument\ $x$.
///fn+abs+usage abs(x)
///fn+abs+math |x|
///fn+abs+plotx   -2.5 +2.5 1e-2   -2 2 1   0 2 1   0.5 0.5
double feenox_builtin_abs(expr_item_t *f) {
  return fabs(feenox_expression_eval(&f->arg[0]));
}


///fn+sqrt+desc Computes the positive square root of the argument\ $x$.
///fn+sqrt+desc If\ $x$ is negative, a NaN error is issued.
///fn+sqrt+usage sqrt(x)
///fn+sqrt+math +\sqrt{x}
///fn+sqrt+plotx 0 5 1e-2    0 6 1    0 2.5 0.5    0.5  0.25
double feenox_builtin_sqrt(expr_item_t *f) {
  double x = feenox_expression_eval(&f->arg[0]);

  if (x < 0) {
    feenox_nan_error();
    return 0;
  }
  return sqrt(x);
}


///fn+is_even+desc Returns one if the argument\ $x$ rounded to the nearest integer is even.
///fn+is_even+usage is_even(x)
///fn+is_even+math  \begin{cases}1 &\text{if $x$ is even} \\ 0 &\text{if $x$ is odd} \end{cases}
double feenox_builtin_is_even(expr_item_t *f) {
  return GSL_IS_EVEN((int)(round(feenox_expression_eval(&f->arg[0]))));
}

///fn+is_odd+desc Returns one if the argument\ $x$ rounded to the nearest integer is odd.
///fn+is_odd+usage is_odd(x)
///fn+is_odd+math  \begin{cases}1 &\text{if $x$ is odd} \\ 0 &\text{if $x$ is even} \end{cases}
double feenox_builtin_is_odd(expr_item_t *f) {
  return GSL_IS_ODD((int)(round(feenox_expression_eval(&f->arg[0]))));
}


///fn+heaviside+desc Computes the zero-centered Heaviside step function of the argument\ $x$.
///fn+heaviside+desc If the optional second argument $\delta$ is provided, the discontinuous
///fn+heaviside+desc step at\ $x=0$ is replaced by a ramp starting at\ $x=0$ and finishing at\ $x=\delta$.
///fn+heaviside+usage heaviside(x, [delta])
///fn+heaviside+math \begin{cases} 0 & \text{if $x < 0$} \\ x / \delta & \text{if $0 < x < \delta$} \\ 1 & \text{if $x > \delta$} \end{cases}
///fn+heaviside+plotx -2.5 2.5 5e-3    -2 +2 1   0 1 0.5   0.5 0.25
double feenox_builtin_heaviside(expr_item_t *f) {
  double x = feenox_expression_eval(&f->arg[0]);
  double delta = feenox_expression_eval(&f->arg[1]);

  if (x <= 0) {
    return 0;
  } else if (x < delta) {
    return x/delta;
  }
  
  return 1;
}


///fn+sgn+desc Returns minus one, zero or plus one depending on the sign of the first argument\ $x$.
///fn+sgn+desc The second optional argument $\epsilon$ gives the precision of the "zero"
///fn+sgn+desc evaluation. If not given, default is $\epsilon = 10^{-9}$.
///fn+sgn+usage sgn(x, [eps])
///fn+sgn+math  \begin{cases}-1 &\text{if $x \le -\epsilon$} \\ 0 &\text{if $|x| < \epsilon$} \\ +1 &\text{if $x \ge +\epsilon$} \end{cases}
///fn+sgn+plotx -2.5 2.5 1e-3    -2 +2 1     -1 +1 1   0.5 0.5
double feenox_builtin_sgn(expr_item_t *f) {
  double eps = 1e-9;
  double x = feenox_expression_eval(&f->arg[0]);
  
  if (f->arg[1].items != NULL) {
    eps = feenox_expression_eval(&f->arg[1]);
  }
  if (fabs(x) < eps) {
    return 0;
  }

  return (x > 0) ? (+1) : (-1);

}


///fn+not+desc Returns one if the first argument\ $x$ is zero and zero otherwise.
///fn+not+desc The second optional argument $\epsilon$ gives the precision of the 
///fn+not+desc "zero" evaluation. If not given, default is $\epsilon = 10^{-9}$.
///fn+not+usage not(x, [eps])
///fn+not+math  \begin{cases}1 &\text{if $|x| < \epsilon$} \\ 0 &\text{otherwise} \end{cases}
double feenox_builtin_not(expr_item_t *f) {
  double eps = 1e-9;
  double x = feenox_expression_eval(&f->arg[0]);

  if (f->arg[1].items != NULL) {
    eps = feenox_expression_eval(&f->arg[1]);
  }
  return (fabs(x) < eps);
}

///fn+mod+desc Returns the remainder of the division between the first argument\ $a$ and the
///fn+mod+desc second one\ $b$. Both arguments may be non-integral.
///fn+mod+usage mod(a, b)
///fn+mod+math a - \left\lfloor \frac{a}{b} \right\rfloor \cdot b
double feenox_builtin_mod(expr_item_t *f) {
  double a = feenox_expression_eval(&f->arg[0]);
  double b = feenox_expression_eval(&f->arg[1]);

  if (a == 0) {
    return 0;
  }
  
  return a - floor(a/b)*b;
}


///fn+floor+desc Returns the largest integral value not greater than the argument\ $x$.
///fn+floor+usage floor(x)
///fn+floor+math \lfloor x \rfloor
///fn+floor+plotx  -2.5 2.5 5e-3   -2 2 1   -3 2 1  0.5 0.5   

double feenox_builtin_floor(expr_item_t *f) {
  return floor(feenox_expression_eval(&f->arg[0]));
}


///fn+ceil+desc Returns the smallest integral value not less than the argument\ $x$.
///fn+ceil+usage ceil(x)
///fn+ceil+math \lceil x \rceil
///fn+ceil+plotx -2.5 2.5 5e-3   -2 2 1   -2 3 1  0.5 0.5   
double feenox_builtin_ceil(expr_item_t *f) {
  return ceil(feenox_expression_eval(&f->arg[0]));
}


///fn+round+desc Rounds the argument\ $x$ to the nearest integer. Halfway cases are rounded away from zero.
///fn+round+usage round(x)
///fn+round+math \begin{cases} \lceil x \rceil & \text{if $\lceil x \rceil - x < 0.5$} \\ \lceil x \rceil & \text{if $\lceil x \rceil - x = 0.5 \wedge x > 0$} \\ \lfloor x \rfloor & \text{if $x-\lfloor x \rfloor < 0.5$} \\ \lfloor x \rfloor & \text{if $x-\lfloor x \rfloor = 0.5 \wedge x < 0$} \end{cases}
///fn+round+plotx -2.7 2.7 1e-3   -2 +2 1   -3 +3 1    0.5 0.5
double feenox_builtin_round(expr_item_t *f) {
  return round(feenox_expression_eval(&f->arg[0]));
}


///fn+deadband+desc Filters the first argument\ $x$ with a deadband centered at zero with an amplitude
///fn+deadband+desc given by the second argument $a$.
///fn+deadband+usage deadband(x, a)
///fn+deadband+math \begin{cases} 0 & \text{if $| x | \leq a$} \\ x + a & \text{if $x < a$} \\ x - a & \text{if $x > a$} \end{cases}
double feenox_builtin_deadband(expr_item_t *f) {
  double x[2];
  x[0] = feenox_expression_eval(&f->arg[0]);
  x[1] = feenox_expression_eval(&f->arg[1]);

  return (fabs(x[0])<x[1])?0:(x[0]+((x[0]>0)?(-1):1)*x[1]);
}

///fn+lag+desc Filters the first argument\ $x(t)$ with a first-order lag of characteristic time $\tau$,
///fn+lag+desc i.e. this function applies the transfer function\ $G(s) = \frac{1}{1 + s\tau}$
///fn+lag+desc to the time-dependent signal\ $x(t)$ to obtain a filtered signal\ $y(t)$, 
///fn+lag+desc by assuming that it is constant during the time 
///fn+lag+desc interval\ $[t-\Delta t,t]$ and using the analytical solution of the differential equation
///fn+lag+desc  for that case at\ $t = \Delta t$ with the initial condition\ $y(0) = y(t-\Delta t)$.
///fn+lag+usage lag(x, tau)
///fn+lag+math x(t) - \Big[ x(t) - y(t-\Delta t) \Big] \cdot \exp\left(-\frac{\Delta t}{\tau}\right)
double feenox_builtin_lag(expr_item_t *f) {

  double y;
  double x[2];
  x[0] = feenox_expression_eval(&f->arg[0]);
  x[1] = feenox_expression_eval(&f->arg[1]);

  if (f->aux == NULL) {
    // si es la primera vez que se llama a este lag, allocamos
    // el apuntador interno e inicializamos al valor de entrada
    f->aux = malloc(3*sizeof(double));
    f->aux[0] = x[0];
    f->aux[1] = x[0];
    f->aux[2] = 0;
  } else if ((int)(feenox_special_var_value(in_static)) || x[1] < ZERO) {
    // si no es la primera vez que se llama a este lag pero estamos
    // en el paso estatico o el tau es muy chiquito, pasa de largo
    // el valor del primer argumento
    f->aux[0] = x[0];
    f->aux[1] = x[0];
    f->aux[2] = 0;
  } else if ((int)round(f->aux[2]) != (int)(feenox_special_var_value(step_transient))) {
    // si nos llamaron, nos aseguramos de que solamente hagamos el lag
    //   cuando corresponda para evitar problemas en cosas que se llaman
    //   iterativa o implicitamente
    f->aux[0] = f->aux[1];
    f->aux[1] = x[0] - (x[0] - f->aux[0])*exp(-feenox_special_var_value(dt)/x[1]);
    f->aux[2] = (int)(feenox_special_var_value(step_transient));
  }

  // si termino la corrida rompemos todo para que si tenemos que volver
  // a arrcancar, empiece todo como si nada
  if (feenox_special_var_value(done)) {
    double dummy = f->aux[1];
    feenox_free(f->aux);
    return dummy;
  }

  y = f->aux[1];

  if (feenox_special_var_value(done)) {
    feenox_free(f->aux);
  }
  return y;
}

///fn+lag_euler+desc Filters the first argument\ $x(t)$ with a first-order lag of characteristic time $\tau$
///fn+lag_euler+desc to the time-dependent signal\ $x(t)$ by using the Euler forward rule.
///fn+lag_euler+usage lag_euler(x, tau)
///fn+lag_euler+math x(t-\Delta t) + \Big[ x(t) - x(t - \Delta t) \Big] \cdot \frac{\Delta t}{\tau}
double feenox_builtin_lag_euler(expr_item_t *f) {

  double y;
  double x[2];
  x[0] = feenox_expression_eval(&f->arg[0]);
  x[1] = feenox_expression_eval(&f->arg[1]);

  if (f->aux == NULL) {
    f->aux = malloc(5*sizeof(double));
    f->aux[0] = x[0];
    f->aux[1] = x[0];
    f->aux[2] = x[0];
    f->aux[3] = x[0];
    f->aux[4] = 0;
  } else if ((int)(feenox_special_var_value(in_static))) {
    f->aux[0] = x[0];
    f->aux[1] = x[0];
    f->aux[2] = x[0];
    f->aux[3] = x[0];
    f->aux[4] = 0;
  } else if ((int)round(f->aux[4]) != (int)(feenox_special_var_value(step_transient))) {
    f->aux[0] = f->aux[1];
    f->aux[1] = x[0];
    f->aux[2] = f->aux[3];
    f->aux[3] = f->aux[2] + feenox_special_var_value(dt)/x[1]*(x[0]-f->aux[2]);
    f->aux[4] = (int)(feenox_special_var_value(step_transient));
  }

  y = f->aux[3];

  if (feenox_special_var_value(done)) {
    feenox_free(f->aux);
  }
  return y;
}

///fn+lag_bilinear+desc Filters the first argument\ $x(t)$ with a first-order lag of characteristic time $\tau$
///fn+lag_bilinear+desc to the time-dependent signal\ $x(t)$ by using the bilinear transformation formula.
///fn+lag_bilinear+usage lag_bilinear(x, tau)
///fn+lag_bilinear+math x(t-\Delta t) \cdot \left[ 1 - \frac{\Delta t}{2\tau} \right] + \left[ \frac{x(t) + x(t - \Delta t)}{1 + \frac{\Delta t}{2\tau}}\right] \cdot \frac{\Delta t}{2\tau}
double feenox_builtin_lag_bilinear(expr_item_t *f) {

  double y;
  double x[2];
  x[0] = feenox_expression_eval(&f->arg[0]);
  x[1] = feenox_expression_eval(&f->arg[1]);

  if (f->aux == NULL) {
    f->aux = malloc(5*sizeof(double));
    f->aux[0] = x[0];
    f->aux[1] = x[0];
    f->aux[2] = x[0];
    f->aux[3] = x[0];
    f->aux[4] = 0;
  } else if ((int)(feenox_special_var_value(in_static))) {
    f->aux[0] = x[0];
    f->aux[1] = x[0];
    f->aux[2] = x[0];
    f->aux[3] = x[0];
    f->aux[4] = 0;
  } else if ((int)round(f->aux[4]) != (int)(feenox_special_var_value(step_transient))) {
    f->aux[0] = f->aux[1];
    f->aux[1] = x[0];
    f->aux[2] = f->aux[3];
    f->aux[3] = (f->aux[2] * (1 - 0.5*feenox_special_var_value(dt)/x[1]) + 0.5*feenox_special_var_value(dt)/x[1]*(x[0] + f->aux[0]))/(1 + 0.5*feenox_special_var_value(dt)/x[1]);
    f->aux[4] = (int)(feenox_special_var_value(step_transient));
  }

  y = f->aux[3];

  if (feenox_special_var_value(done)) {
    feenox_free(f->aux);
  }
  return y;
}


// lead(x, tau) implementa la fucion de transferencia s.tau/(1+s.tau)
/*
double feenox_builtin_lead(expr_factor_t *expr) {

  double y;
  double x[2];
  x[0] = feenox_expression_eval(&f->arg[0]);
  x[1] = feenox_expression_eval(&f->arg[1]);

  if (f->aux == NULL) {
    f->aux = malloc(3*sizeof(double));
    f->aux[0] = x[0];
    f->aux[1] = x[0];
    f->aux[2] = 0;
  } else if ((int)(feenox_special_var_value(in_static)))) {
    f->aux[0] = x[0];
    f->aux[1] = x[0];
    f->aux[2] = 0;
  } else if ((int)round(f->aux[2]) != (int)(feenox_special_var_value(step_transient)))) {
    f->aux[0] = f->aux[1];
    f->aux[1] = (x[0] - f->aux[0])/(0.5*feenox_special_var_value(dt))/x[1] + 1) - (0.5*feenox_special_var_value(dt))/x[1] - 1)/(0.5*feenox_special_var_value(dt))/x[1] + 1) * (f->aux[0]);
    f->aux[2] = (int)(feenox_special_var_value(step_transient)));
  }

  y = f->aux[1];

  if (feenox_special_var_value(done))) {
    feenox_free(f->aux);
  }
  return y;
}
*/

// deriv_lag(x, tau) implementa la fucion de transferencia s.tau/(1+s.tau)
/*
double feenox_builtin_lead_euler(algebraic_token_t *expr) {
  if (expr->nofirst_time) {
    return (f->arg_value[0] - f->arg_last_value[0])/(0.5*feenox_special_var_value(dt))/f->arg_value[1] + 1) - (0.5*feenox_special_var_value(dt))/f->arg_value[1] - 1)/(0.5*feenox_special_var_value(dt))/f->arg_value[1] + 1) * (expr->last_value);
  } else {
    expr->nofirst_time = 1;
    return 0;
  }
}


// deriv_lag(x, tau) implementa la fucion de transferencia s.tau/(1+s.tau)
double feenox_builtin_lead_bilinear(algebraic_token_t *expr) {
  if (expr->nofirst_time) {
    return (f->arg_value[0] - f->arg_last_value[0])/(0.5*feenox_special_var_value(dt))/f->arg_value[1] + 1) - (0.5*feenox_special_var_value(dt))/f->arg_value[1] - 1)/(0.5*feenox_special_var_value(dt))/f->arg_value[1] + 1) * (expr->last_value);
  } else {
    expr->nofirst_time = 1;
    return 0;
  }
}
*/

///fn+equal+desc Checks if the two first expressions $a$ and $b$ are equal, up to the tolerance
///fn+equal+desc given by the third optional argument $\epsilon$. If either $|a|>1$ or $|b|>1$,
///fn+equal+desc the arguments are compared using GSL's `gsl_fcmp`, otherwise the
///fn+equal+desc absolute value of their difference is compared against $\epsilon$. This function
///fn+equal+desc returns zero if the arguments are not equal and one otherwise.
///fn+equal+desc Default value for $\epsilon = 10^{-9}$.
///fn+equal+usage equal(a, b, [eps])
///fn+equal+math \begin{cases} 1 & \text{if $a = b$} \\ 0 & \text{if $a \neq b$} \end{cases}
double feenox_builtin_equal(expr_item_t *f) {
  double eps = 1e-9;
  double a = feenox_expression_eval(&f->arg[0]);
  double b = feenox_expression_eval(&f->arg[1]);

  if (f->arg[2].items != NULL) {
    eps = feenox_expression_eval(&f->arg[2]);
  }

  if (fabs(a) < 1 || fabs(b) < 1) {
    return (fabs(a-b) < eps)?1:0;
  } else {
    return (gsl_fcmp(a, b, eps) == 0) ? 1 : 0;
  }


}

double feenox_builtin_quasi_random(expr_item_t *f) {

  double y = 0;
  double r = 0;
  double x1 = feenox_expression_eval(&f->arg[0]);
  double x2 = feenox_expression_eval(&f->arg[1]);

  // si es la primera llamada inicializamos el generador
  if (f->aux == NULL) {
    f->aux = (double *)gsl_qrng_alloc(gsl_qrng_sobol, 1);
  }

  // TODO: memory leaks en fiteo, minimizacion, etc
  gsl_qrng_get((const gsl_qrng *)f->aux, &r);
  y = x1 + r*(x2-x1);

  if (feenox_special_var_value(done)) {
    gsl_qrng_free((gsl_qrng *)f->aux);
    f->aux = NULL;
  }

  return y;
}



///fn+random+desc Returns a random real number uniformly distributed between the first
///fn+random+desc real argument\ $x_1$ and the second one\ $x_2$.
///fn+random+desc If the third integer argument $s$ is given, it is used as the seed and thus
///fn+random+desc repetitive sequences can be obtained. If no seed is provided, the current time
///fn+random+desc (in seconds) plus the internal address of the expression is used. Therefore,
///fn+random+desc two successive calls to the function without seed (hopefully) do not give the same result.
///fn+random+desc This function uses a second-order multiple recursive generator described by
///fn+random+desc Knuth in Seminumerical Algorithms, 3rd Ed., Section 3.6.
///fn+random+usage random(x1, x2, [s])
///fn+random+math  x_1 + r \cdot (x_2-x_1) \quad \quad 0 \leq r < 1
double feenox_builtin_random(expr_item_t *f) {

  double y = 0;
  double x1 = feenox_expression_eval(&f->arg[0]);
  double x2 = feenox_expression_eval(&f->arg[1]);

  // si es la primera llamada inicializamos el generador
  if (f->aux == NULL) {
    f->aux = (double *)gsl_rng_alloc(DEFAULT_RANDOM_METHOD);
    // si nos dieron tercer argumento, lo usamos como semilla, sino usamos time()
    if (f->arg[2].items == NULL) {
      gsl_rng_set((gsl_rng *)f->aux, (unsigned long int)(time(NULL)) + (unsigned long int)(&f->aux));
    } else {
      gsl_rng_set((gsl_rng *)f->aux, (unsigned long int)(feenox_expression_eval(&f->arg[2])));
    }
  }

  // TODO: memory leaks en fiteo, minimizacion, etc
  y = x1 + gsl_rng_uniform((const gsl_rng *)f->aux)*(x2-x1);

  if (feenox_special_var_value(done)) {
    gsl_rng_free((gsl_rng *)f->aux);
    f->aux = NULL;
  }

  return y;
}


///fn+random_gauss+desc Returns a random real number with a Gaussian distribution with a mean
///fn+random_gauss+desc equal to the first argument\ $x_1$ and a standard deviation equatl to the second one\ $x_2$.
///fn+random_gauss+desc If the third integer argument $s$ is given, it is used as the seed and thus
///fn+random_gauss+desc repetitive sequences can be obtained. If no seed is provided, the current time
///fn+random_gauss+desc (in seconds) plus the internal address of the expression is used. Therefore,
///fn+random_gauss+desc two successive calls to the function without seed (hopefully) do not give the same result.
///fn+random_gauss+desc This function uses a second-order multiple recursive generator described by
///fn+random_gauss+desc Knuth in Seminumerical Algorithms, 3rd Ed., Section 3.6.
///fn+random_gauss+usage random_gauss(x1, x2, [s])
double feenox_builtin_random_gauss(expr_item_t *f) {

  double x[3];
  x[0] = feenox_expression_eval(&f->arg[0]);
  x[1] = feenox_expression_eval(&f->arg[1]);
  x[2] = feenox_expression_eval(&f->arg[2]);

  // si es la primera llamada inicializamos el generador
  if (f->aux == NULL) {
    f->aux = (double *)gsl_rng_alloc(DEFAULT_RANDOM_METHOD);
    // si nos dieron tercer argumento, lo usamos como semilla, sino usamos time()
    if (f->arg[2].items == NULL) {
      gsl_rng_set((const gsl_rng *)f->aux, (unsigned long int)(time(NULL)) + (unsigned long int)(&f->aux));
//      gsl_rng_set((const gsl_rng *)f->aux, (unsigned long int)(time(NULL)));
    } else {
      gsl_rng_set((const gsl_rng *)f->aux, (unsigned long int)(x[2]));
    }
  }

  return x[0] + gsl_ran_gaussian((const gsl_rng *)f->aux, x[1]);

  // TODO: no camina con seed y fit al mismo tiempo

}

///fn+limit+desc Limits the first argument\ $x$ to the interval $[a,b]$. The second argument $a$ should
///fn+limit+desc be less than the third argument $b$.
///fn+limit+usage limit(x, a, b)
///fn+limit+math \begin{cases} a & \text{if $x < a$} \\ x & \text{if $a \leq x \leq b$} \\ b & \text{if $x > b$} \end{cases}
double feenox_builtin_limit(expr_item_t *f) {
  double x[3];
  x[0] = feenox_expression_eval(&f->arg[0]);
  x[1] = feenox_expression_eval(&f->arg[1]);
  x[2] = feenox_expression_eval(&f->arg[2]);

  if (x[0] < x[1]) {
    return x[1];
  } else if (x[0] > x[2]) {
    return x[2];
  }

  return x[0];
}

///fn+limit_dt+desc Limits the value of the first argument\ $x(t)$ so to that its time derivative
///fn+limit_dt+desc is bounded to the interval $[a,b]$. The second argument $a$ should
///fn+limit_dt+desc be less than the third argument $b$.
///fn+limit_dt+usage limit_dt(x, a, b)
///fn+limit_dt+math \begin{cases} x(t) & \text{if $a \leq dx/dt \leq b$} \\ x(t-\Delta t) + a \cdot \Delta t & \text{if $dx/dt < a$} \\ x(t-\Delta t) + b \cdot \Delta t & \text{if $dx/dt > b$} \end{cases}
double feenox_builtin_limit_dt(expr_item_t *f) {

  double y;
  double derivative;
  double x[3];
  x[0] = feenox_expression_eval(&f->arg[0]);
  x[1] = feenox_expression_eval(&f->arg[1]);
  x[2] = feenox_expression_eval(&f->arg[2]);

  if (feenox_special_var_value(dt) == 0) {
    feenox_nan_error();
    return 0;
  }

  if (f->aux == NULL) {
    f->aux = malloc(3*sizeof(double));
    f->aux[0] = x[0];
    f->aux[1] = x[0];
    f->aux[2] = 0;
  } else if ((int)(feenox_special_var_value(in_static))) {
    f->aux[0] = x[0];
    f->aux[1] = x[0];
    f->aux[2] = 0;
  } else if ((int)round(f->aux[2]) != (int)(feenox_special_var_value(step_transient))) {
    f->aux[0] = f->aux[1];
    f->aux[1] = x[0];
    f->aux[2] = (int)(feenox_special_var_value(step_transient));
  }

  derivative = (x[0] - f->aux[0])/feenox_special_var_value(dt);


//  if (!expr->nofirst_time) {
//    return x[0];
//  }

  if (derivative < x[1]) {
    y = f->aux[0] + x[1]*feenox_special_var_value(dt);
  } else if (derivative > x[2]) {
    y = f->aux[0] + x[2]*feenox_special_var_value(dt);
  } else {
    y = x[0];
  }

  if (feenox_special_var_value(done)) {
    feenox_free(f->aux);
  }
  return y;
}

///fn+if+desc Performs a conditional testing of the first argument $a$, and returns either the
///fn+if+desc second optional argument $b$ if $a$ is different from zero or the third optional argument $c$
///fn+if+desc if $a$ evaluates to zero. The comparison of the condition $a$ with zero is performed
///fn+if+desc within the precision given by the optional fourth argument $\epsilon$.
///fn+if+desc If the second argument $c$ is not given and $a$ is not zero, the function returns one.
///fn+if+desc If the third argument $c$ is not given and $a$ is zero, the function returns zero.
///fn+if+desc The default precision is $\epsilon = 10^{-9}$.
///fn+if+desc Even though `if` is a logical operation, all the arguments and the returned value
///fn+if+desc are double-precision floating point numbers.
///fn+if+usage if(a, [b], [c], [eps])
///fn+if+math \begin{cases} b & \text{if $|a|<\epsilon$} \\ c & \text{otherwise} \end{cases}

double feenox_builtin_if(expr_item_t *f) {
  double eps = 1e-9;
  double a = feenox_expression_eval(&f->arg[0]);

  if (f->arg[3].items != NULL) {
    eps = fabs(feenox_expression_eval(&f->arg[3]));
  }

  if (fabs(a) > eps) {
    return (f->arg[1].items != NULL) ? feenox_expression_eval(&f->arg[1]) : 1.0;
  } else {
    return (f->arg[2].items != NULL) ? feenox_expression_eval(&f->arg[2]) : 0.0;
  }

}


///fn+is_in_interval+desc Returns true if the argument\ $x$ is in the interval\ $[a,b)$, 
///fn+is_in_interval+desc i.e. including\ $a$ but excluding\ $b$.
///fn+is_in_interval+usage is_in_interval(x, a, b)
///fn+is_in_interval+math \begin{cases} 1 & \text{if $a \leq x < b$} \\ 0 & \text{otherwise} \end{cases}
double feenox_builtin_is_in_interval(expr_item_t *f) {

  double y;
  double x[3];
  x[0] = feenox_expression_eval(&f->arg[0]);
  x[1] = feenox_expression_eval(&f->arg[1]);
  x[2] = feenox_expression_eval(&f->arg[2]);

  y = (x[0] >= x[1] && x[0] < x[2]);

  return y;
}



///fn+threshold_max+desc Returns one if the first argument\ $x$ is greater than the threshold given by
///fn+threshold_max+desc the second argument $a$, and \textit{exactly} zero otherwise. If the optional
///fn+threshold_max+desc third argument $b$ is provided, an hysteresis of width $b$ is needed in order
///fn+threshold_max+desc to reset the function value. Default is no hysteresis, i.e. $b=0$.
///fn+threshold_max+usage threshold_max(x, a, [b])
///fn+threshold_max+math \begin{cases} 1 & \text{if $x > a$} \\ 0 & \text{if $x < a-b$} \\ \text{last value of $y$} & \text{otherwise} \end{cases}

double feenox_builtin_threshold_max(expr_item_t *f) {

  double y;
  double x[3];
  x[0] = feenox_expression_eval(&f->arg[0]);
  x[1] = feenox_expression_eval(&f->arg[1]);
  x[2] = feenox_expression_eval(&f->arg[2]);

  if (f->aux == NULL) {
    f->aux = malloc(1*sizeof(double));
    f->aux[0] = x[0] > x[1];
  }

  if (x[0] > x[1]) {
    f->aux[0] = 1;
  } else if (x[0] < (x[1]-x[2])) {
    f->aux[0] = 0;
  }
  y = f->aux[0];

  if (feenox_special_var_value(done)) {
    feenox_free(f->aux);
  }
  return y;

}


///fn+threshold_min+desc Returns one if the first argument\ $x$ is less than the threshold given by
///fn+threshold_min+desc the second argument $a$, and \textit{exactly} zero otherwise. If the optional
///fn+threshold_min+desc third argument $b$ is provided, an hysteresis of width $b$ is needed in order
///fn+threshold_min+desc to reset the function value. Default is no hysteresis, i.e. $b=0$.
///fn+threshold_min+usage threshold_min(x, a, [b])
///fn+threshold_min+math \begin{cases} 1 & \text{if $x < a$} \\ 0 & \text{if $x > a+b$} \\ \text{last value of $y$} & \text{otherwise} \end{cases}

double feenox_builtin_threshold_min(expr_item_t *f) {

  double y;
  double x[3];
  x[0] = feenox_expression_eval(&f->arg[0]);
  x[1] = feenox_expression_eval(&f->arg[1]);
  x[2] = feenox_expression_eval(&f->arg[2]);

  if (f->aux == NULL) {
    f->aux = malloc(1*sizeof(double));
    f->aux[0] = x[0] > x[1];
  }

  if (x[0] < x[1]) {
    f->aux[0] = 1;
  } else if (x[0] > (x[1]+x[2])) {
    f->aux[0] = 0;
  }

  y = f->aux[0];

  if (feenox_special_var_value(done)) {
    feenox_free(f->aux);
  }
  return y;
}


///fn+min+desc Returns the minimum of the arguments\ $x_i$ provided. Currently only maximum of ten arguments can be given.
///fn+min+usage min(x1, x2, [...], [x10])
///fn+min+math  \min \Big (x_1, x_2, \dots, x_{10} \Big)

double feenox_builtin_min(expr_item_t *f) {
  int i;
  double min = feenox_expression_eval(&f->arg[0]);

  for (i = 1; i < MINMAX_ARGS; i++) {
    if (f->arg[i].items != NULL && feenox_expression_eval(&f->arg[i]) < min) {
      min = feenox_expression_eval(&f->arg[i]);
    }
  }
  return min;
}

///fn+max+desc Returns the maximum of the arguments\ $x_i$ provided. Currently only maximum of ten arguments can be given.
///fn+max+usage max(x1, x2, [...], [x10])
///fn+max+math  \max \Big (x_1, x_2, \dots, x_{10} \Big)

double feenox_builtin_max(expr_item_t *f) {
  int i;
  double max = feenox_expression_eval(&f->arg[0]);
  double tmp;

  for (i = 1; i < MINMAX_ARGS; i++) {
    if (f->arg[i].items != NULL && (tmp = feenox_expression_eval(&f->arg[i])) > max) {
      max = tmp;
    }
  }
  return max;
}


///fn+mark_min+desc Returns the integer index $i$ of the minimum of the arguments\ $x_i$ provided. Currently only maximum of ten arguments can be provided.
///fn+mark_min+usage mark_max(x1, x2, [...], [x10])
///fn+mark_min+math  i / \min \Big (x_1, x_2, \dots, x_{10} \Big) = x_i

double feenox_builtin_mark_min(expr_item_t *f) {
  int i;
  int i_min = 0;
  double min = feenox_expression_eval(&f->arg[0]);

  for (i = 1; i < MINMAX_ARGS; i++) {
    if (f->arg[i].items != NULL && feenox_expression_eval(&f->arg[i]) < min) {
      min = feenox_expression_eval(&f->arg[i]);
      i_min = i;
    }
  }
  return i_min+1;
}

///fn+mark_max+desc Returns the integer index $i$ of the maximum of the arguments\ $x_i$ provided. Currently only maximum of ten arguments can be provided.
///fn+mark_max+usage mark_max(x1, x2, [...], [x10])
///fn+mark_max+math  i / \max \Big (x_1, x_2, \dots, x_{10} \Big) = x_i

double feenox_builtin_mark_max(expr_item_t *f) {
  int i;
  int i_max = 0;
  double max = feenox_expression_eval(&f->arg[0]);

  for (i = 1; i < MINMAX_ARGS; i++) {
    if (f->arg[i].items != NULL && feenox_expression_eval(&f->arg[i]) > max) {
      max = feenox_expression_eval(&f->arg[i]);
      i_max = i;
    }
  }
  return i_max+1;
}
