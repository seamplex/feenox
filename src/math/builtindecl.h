/*------------ -------------- -------- --- ----- ---   --       -            -
 *  wasora builtin functions and functionals declarations
 *
 *  Copyright (C) 2009--2013 jeremy theler
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

// funciones internas

extern double builtin_abs(struct factor_t *);
extern double builtin_asin(struct factor_t *);
extern double builtin_acos(struct factor_t *);
extern double builtin_atan(struct factor_t *);
extern double builtin_atan2(struct factor_t *);
extern double builtin_ceil(struct factor_t *);
extern double builtin_clock(struct factor_t *);
extern double builtin_cos(struct factor_t *);
extern double builtin_cosh(struct factor_t *);
extern double builtin_d_dt(struct factor_t *);
extern double builtin_deadband(struct factor_t *);
extern double builtin_equal(struct factor_t *);
extern double builtin_exp(struct factor_t *);
extern double builtin_expint1(struct factor_t *);
extern double builtin_expint2(struct factor_t *);
extern double builtin_expint3(struct factor_t *);
extern double builtin_expintn(struct factor_t *);
extern double builtin_floor(struct factor_t *);
extern double builtin_heaviside(struct factor_t *);
extern double builtin_if(struct factor_t *);
extern double builtin_is_in_interval(struct factor_t *);
extern double builtin_integral_dt(struct factor_t *);
extern double builtin_integral_euler_dt(struct factor_t *);
extern double builtin_lag(struct factor_t *);
extern double builtin_lag_bilinear(struct factor_t *);
extern double builtin_lag_euler(struct factor_t *);
extern double builtin_last(struct factor_t *);
extern double builtin_limit(struct factor_t *);
extern double builtin_limit_dt(struct factor_t *);
extern double builtin_log(struct factor_t *);
extern double builtin_mark_max(struct factor_t *);
extern double builtin_mark_min(struct factor_t *);
extern double builtin_max(struct factor_t *);
extern double builtin_min(struct factor_t *);
extern double builtin_mod(struct factor_t *);
extern double builtin_not(struct factor_t *);
extern double builtin_random(struct factor_t *);
extern double builtin_random_gauss(struct factor_t *);
extern double builtin_round(struct factor_t *);
extern double builtin_sawtooth_wave(struct factor_t *);
extern double builtin_sgn(struct factor_t *);
extern double builtin_is_even(struct factor_t *);
extern double builtin_is_odd(struct factor_t *);
extern double builtin_sin(struct factor_t *);
extern double builtin_j0(struct factor_t *);
extern double builtin_sinh(struct factor_t *);
extern double builtin_sqrt(struct factor_t *);
extern double builtin_square_wave(struct factor_t *);
extern double builtin_tan(struct factor_t *);
extern double builtin_tanh(struct factor_t *);
extern double builtin_threshold_max(struct factor_t *);
extern double builtin_threshold_min(struct factor_t *);
extern double builtin_triangular_wave(struct factor_t *);

// funcionales
extern double builtin_derivative(struct factor_t *, struct var_t *);
extern double builtin_integral(struct factor_t *, struct var_t *);
extern double builtin_simpson(struct factor_t *, struct var_t *);
extern double builtin_gauss_kronrod(struct factor_t *, struct var_t *);
extern double builtin_gauss_legendre(struct factor_t *, struct var_t *);
extern double builtin_prod(struct factor_t *, struct var_t *);
extern double builtin_sum(struct factor_t *, struct var_t *);
extern double builtin_root(struct factor_t *, struct var_t *);
extern double builtin_func_min(struct factor_t *, struct var_t *);

// funciones sobre vectores
extern double builtin_vecdot(struct vector_t **);
extern double builtin_vecnorm(struct vector_t **);
extern double builtin_vecmin(struct vector_t **);
extern double builtin_vecmax(struct vector_t **);
extern double builtin_vecminindex(struct vector_t **);
extern double builtin_vecmaxindex(struct vector_t **);
extern double builtin_vecsum(struct vector_t **);
extern double builtin_vecsize(struct vector_t **);

