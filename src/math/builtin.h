/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox builtin functions and functionals arrays
 *
 *  Copyright (C) 2009--2021 jeremy theler
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

#include "builtin_decl.h"

struct feenox_builtin_function_t feenox_builtin_function[N_BUILTIN_FUNCTIONS] = {
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


/*
struct feenox_builtin_functional_t feenox_builtin_functional[N_BUILTIN_FUNCTIONALS] = {
    {"derivative",          3, 5, &feenox_builtin_derivative},
    {"integral",            4, 7, &feenox_builtin_integral},
    {"gauss_kronrod",       4, 5, &feenox_builtin_gauss_kronrod},
    {"gauss_legendre",      4, 5, &feenox_builtin_gauss_legendre},
    {"prod",                4, 4, &feenox_builtin_prod},
    {"sum",                 4, 4, &feenox_builtin_sum},
    {"root",                4, 7, &feenox_builtin_root},
    {"func_min",            4, 8, &feenox_builtin_func_min},
};


struct feenox_builtin_vectorfunction_t feenox_builtin_vectorfunction[N_BUILTIN_VECTOR_FUNCTIONS] = {
    {"vecdot",              2, 2, &feenox_builtin_vecdot},
    {"vecnorm",             1, 1, &feenox_builtin_vecnorm},
    {"vecsize",             1, 1, &feenox_builtin_vecsize},
    {"vecmin",              1, 1, &feenox_builtin_vecmin},
    {"vecmax",              1, 1, &feenox_builtin_vecmax},
    {"vecminindex",         1, 1, &feenox_builtin_vecminindex},
    {"vecmaxindex",         1, 1, &feenox_builtin_vecmaxindex},
    {"vecsum",              1, 1, &feenox_builtin_vecsum},
};

*/