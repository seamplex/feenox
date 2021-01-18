/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox builtin functions and functionals declarations
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

// funciones internas



// funcionales
extern double feenox_builtin_derivative(struct expr_factor_t *, struct var_t *);
extern double feenox_builtin_integral(struct expr_factor_t *, struct var_t *);
extern double feenox_builtin_simpson(struct expr_factor_t *, struct var_t *);
extern double feenox_builtin_gauss_kronrod(struct expr_factor_t *, struct var_t *);
extern double feenox_builtin_gauss_legendre(struct expr_factor_t *, struct var_t *);
extern double feenox_builtin_prod(struct expr_factor_t *, struct var_t *);
extern double feenox_builtin_sum(struct expr_factor_t *, struct var_t *);
extern double feenox_builtin_root(struct expr_factor_t *, struct var_t *);
extern double feenox_builtin_func_min(struct expr_factor_t *, struct var_t *);

// funciones sobre vectores
extern double feenox_builtin_vecdot(struct vector_t **);
extern double feenox_builtin_vecnorm(struct vector_t **);
extern double feenox_builtin_vecmin(struct vector_t **);
extern double feenox_builtin_vecmax(struct vector_t **);
extern double feenox_builtin_vecminindex(struct vector_t **);
extern double feenox_builtin_vecmaxindex(struct vector_t **);
extern double feenox_builtin_vecsum(struct vector_t **);
extern double feenox_builtin_vecsize(struct vector_t **);

