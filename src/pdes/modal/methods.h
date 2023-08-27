/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for modal analysis: virtual methods
 *
 *  Copyright (C) 2021--2023 jeremy theler
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
#ifndef MODAL_METHODS_H
#define MODAL_METHODS_H

// modal/parser.c
extern int feenox_problem_parse_problem_modal(const char *token);
extern int feenox_problem_parse_write_post_modal(mesh_write_t *mesh_write, const char *token);

// modal/init.c
extern int feenox_problem_parse_time_init_modal(void);
extern int feenox_problem_init_runtime_modal(void);
#ifdef HAVE_PETSC
extern int feenox_problem_setup_pc_modal(PC pc);
extern int feenox_problem_setup_ksp_modal(KSP ksp);
#endif
#ifdef HAVE_SLEPC
extern int feenox_problem_setup_eps_modal(EPS eps);
#endif

// modal/bulk.c
extern int feenox_problem_build_allocate_aux_modal(unsigned int J);
extern int feenox_problem_build_volumetric_gauss_point_modal(element_t *element, unsigned int q);

// modal/bc.c
extern int feenox_problem_bc_parse_modal(bc_data_t *bc_data, const char *lhs, char *rhs);
extern int feenox_problem_bc_set_modal_displacement(bc_data_t *bc_data, element_t *e, size_t j_global);
extern int feenox_problem_bc_set_modal_symmetry(bc_data_t *bc_data, element_t *e, size_t j_global);
extern int feenox_problem_bc_set_modal_radial(bc_data_t *bc_data, element_t *e, size_t j_global);
extern int feenox_problem_bc_set_modal_multifreedom(bc_data_t *bc_data, element_t *e, size_t j_global);
extern double feenox_modal_gsl_function_of_uvw(double x, void *params);

// material models
extern int feenox_problem_build_compute_modal_C_elastic_isotropic(const double *x, material_t *material);
extern int feenox_problem_build_compute_modal_C_elastic_plane_stress(const double *x, material_t *material);
extern int feenox_problem_build_compute_modal_C_elastic_plane_strain(const double *x, material_t *material);
extern int feenox_problem_build_compute_modal_C_elastic_orthotropic(const double *x, material_t *material);

// modal/post.c
extern int feenox_problem_solve_post_modal(void);
#endif
