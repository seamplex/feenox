/*------------ -------------- -------- --- ----- ---   --       -            -
 *  wasora builtin functions and functionals arrays
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

#include "builtindecl.h"

struct builtin_function_t builtin_function[N_BUILTIN_FUNCTIONS] = {
    {"abs",                 1, 1, &builtin_abs},
    {"asin",                1, 1, &builtin_asin},
    {"acos",                1, 1, &builtin_acos},
    {"atan",                1, 1, &builtin_atan},
    {"atan2",               2, 2, &builtin_atan2},
    {"ceil",                1, 1, &builtin_ceil},
    {"clock",               0, 1, &builtin_clock},
    {"cos",                 1, 1, &builtin_cos},
    {"cosh",                1, 1, &builtin_cosh},
    {"d_dt",                1, 1, &builtin_d_dt},
    {"deadband",            2, 2, &builtin_deadband},
    {"equal",               2, 3, &builtin_equal},
    {"exp",                 1, 1, &builtin_exp},
    {"expint1",             1, 1, &builtin_expint1},
    {"expint2",             1, 1, &builtin_expint2},
    {"expint3",             1, 1, &builtin_expint3},
    {"expintn",             2, 2, &builtin_expintn},
    {"floor",               1, 1, &builtin_floor},
    {"heaviside",           1, 2, &builtin_heaviside},
    {"if",                  1, 4, &builtin_if},
    {"integral_dt",         1, 1, &builtin_integral_dt},
    {"integral_euler_dt",   1, 1, &builtin_integral_euler_dt},
    {"is_even",             1, 1, &builtin_is_even},
    {"is_in_interval",      3, 3, &builtin_is_in_interval},
    {"is_odd",              1, 1, &builtin_is_odd},
    {"lag",                 2, 2, &builtin_lag},
    {"lag_bilinear",        2, 2, &builtin_lag_bilinear},
    {"lag_euler",           2, 2, &builtin_lag_euler},
    {"last",                1, 2, &builtin_last},
    {"limit",               3, 3, &builtin_limit},
    {"limit_dt",            3, 3, &builtin_limit_dt},
    {"log",                 1, 1, &builtin_log},
    {"mark_max",            2, MINMAX_ARGS, &builtin_mark_max},
    {"mark_min",            2, MINMAX_ARGS, &builtin_mark_min},
    {"max",                 2, MINMAX_ARGS, &builtin_max},
    {"min",                 2, MINMAX_ARGS, &builtin_min},
    {"mod",                 2, 2, &builtin_mod},
    {"not",                 1, 2, &builtin_not},
    {"random",              2, 3, &builtin_random},
    {"random_gauss",        2, 3, &builtin_random_gauss},
    {"round",               1, 1, &builtin_round},
    {"sawtooth_wave",       1, 1, &builtin_sawtooth_wave},
    {"sgn",                 1, 2, &builtin_sgn},
    {"sin",                 1, 1, &builtin_sin},
    {"j0",                  1, 1, &builtin_j0},
    {"sinh",                1, 1, &builtin_sinh},
    {"sqrt",                1, 1, &builtin_sqrt},
    {"square_wave",         1, 1, &builtin_square_wave},
    {"tan",                 1, 1, &builtin_tan},
    {"tanh",                1, 1, &builtin_tanh},
    {"threshold_max",       2, 3, &builtin_threshold_max},
    {"threshold_min",       2, 3, &builtin_threshold_min},
    {"triangular_wave",     1, 1, &builtin_triangular_wave},
};



struct builtin_functional_t builtin_functional[N_BUILTIN_FUNCTIONALS] = {
    {"derivative",          3, 5, &builtin_derivative},
    {"integral",            4, 7, &builtin_integral},
    {"gauss_kronrod",       4, 5, &builtin_gauss_kronrod},
    {"gauss_legendre",      4, 5, &builtin_gauss_legendre},
    {"prod",                4, 4, &builtin_prod},
    {"sum",                 4, 4, &builtin_sum},
    {"root",                4, 7, &builtin_root},
    {"func_min",            4, 8, &builtin_func_min},
};


struct builtin_vectorfunction_t builtin_vectorfunction[N_BUILTIN_VECTOR_FUNCTIONS] = {
    {"vecdot",              2, 2, &builtin_vecdot},
    {"vecnorm",             1, 1, &builtin_vecnorm},
    {"vecsize",             1, 1, &builtin_vecsize},
    {"vecmin",              1, 1, &builtin_vecmin},
    {"vecmax",              1, 1, &builtin_vecmax},
    {"vecminindex",         1, 1, &builtin_vecminindex},
    {"vecmaxindex",         1, 1, &builtin_vecmaxindex},
    {"vecsum",              1, 1, &builtin_vecsum},
};

