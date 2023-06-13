/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for Laplace's equation: virtual methods
 *
 *  Copyright (C) 2021 jeremy theler
 *
 *  This file is part of FeenoX <https://www.seamplex.com/feenox>.
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
#ifndef LAPLACE_METHODS_H
#define LAPLACE_METHODS_H

// laplace/init.c
extern int feenox_problem_parse_problem_laplace(const char *token);
extern int feenox_problem_parse_post_laplace(mesh_write_t *mesh_write, const char *token);

extern int feenox_problem_init_parser_laplace(void);
extern int feenox_problem_init_runtime_laplace(void);
#ifdef HAVE_PETSC
extern int feenox_problem_setup_pc_laplace(PC pc);
extern int feenox_problem_setup_ksp_laplace(KSP ksp);
#endif

// laplace/bulk.c
extern int feenox_problem_build_volumetric_gauss_point_laplace(element_t *element, unsigned int v);

// laplace/bc.c
extern int feenox_problem_bc_parse_laplace(bc_data_t *bc_data, const char *lhs, char *rhs);
extern int feenox_problem_bc_set_laplace_phi(bc_data_t *bc_data, element_t *e, size_t j_global);
extern int feenox_problem_bc_set_laplace_derivative(bc_data_t *bc_data, element_t *e, unsigned int v);


#endif
