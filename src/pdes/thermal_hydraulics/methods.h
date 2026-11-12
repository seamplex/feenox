/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for one-dimensional thermal-hydraulics: virtual methods
 *
 *  Copyright (C) 2026 Jeremy Theler
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
#ifndef THERMAL_HYDRAULICS_METHODS_H
#define THERMAL_HYDRAULICS_METHODS_H

// thermal_hydraulics/parser.c
extern int feenox_parse_heat_structure(void);
extern int thermal_hydraulics_parse_point(int);
extern int feenox_problem_parse_problem_thermal_hydraulics(const char *token);
extern int feenox_problem_parse_write_post_thermal_hydraulics(mesh_write_t *mesh_write, const char *token);

// thermal_hydraulics/init.c
extern int feenox_problem_parse_time_init_thermal_hydraulics(void);
extern int feenox_problem_init_runtime_thermal_hydraulics(void);

// thermal_hydraulics/bc.c
extern int feenox_problem_bc_parse_thermal_hydraulics(bc_data_t *bc_data, const char *lhs, char *rhs);

// thermal_hydraulics/solve.c
extern int feenox_problem_solve_pre_thermal_hydraulics(void);
extern int feenox_problem_solve_post_thermal_hydraulics(void);
#ifdef HAVE_PETSC
extern int feenox_problem_residual_thermal_hydraulics(double t, Vec phi, Vec phi_dot, Vec r, int *domain_error);
extern Mat feenox_problem_create_jacobian_thermal_hydraulics(const char *name);
extern int feenox_problem_initial_condition_thermal_hydraulics(Vec phi, int *given);
extern int feenox_problem_setup_ksp_thermal_hydraulics(KSP ksp);
extern int feenox_problem_setup_pc_thermal_hydraulics(PC pc);
extern int feenox_problem_setup_ts_thermal_hydraulics(TS ts);
#endif

#endif
