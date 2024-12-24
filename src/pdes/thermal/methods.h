/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for the heat equation: virtual methods
 *
 *  Copyright (C) 2021--2023 Jeremy Theler
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
#ifndef THERMAL_METHODS_H
#define THERMAL_METHODS_H

// thermal/parser.c
extern int feenox_problem_parse_problem_thermal(const char *token);
extern int feenox_problem_parse_write_post_thermal(mesh_write_t *mesh_write, const char *token);

// thermal/init.c
extern int feenox_problem_parse_time_init_thermal(void);
extern int feenox_problem_init_runtime_thermal(void);
#ifdef HAVE_PETSC
extern int feenox_problem_setup_pc_thermal(PC pc);
extern int feenox_problem_setup_ksp_thermal(KSP ksp);
#endif

// thermal/bulk.c
extern int feenox_problem_build_volumetric_gauss_point_thermal(element_t *element, unsigned int q);

// thermal/bc.c
extern int feenox_problem_bc_parse_thermal(bc_data_t *bc_data, const char *lhs, char *rhs);

extern int feenox_problem_bc_set_thermal_temperature(bc_data_t *bc_data, element_t *e, size_t j_global);

extern int feenox_problem_bc_set_thermal_heatflux(bc_data_t *bc_data, element_t *e,  unsigned int q);
extern int feenox_problem_bc_set_thermal_convection(bc_data_t *bc_data, element_t *e,  unsigned int q);

// thermal/heatflux.c
extern int feenox_problem_gradient_fill_thermal(void);
extern int feenox_problem_gradient_properties_at_element_nodes_thermal(element_t *element, mesh_t *mesh);
extern int feenox_problem_gradient_fluxes_at_node_alloc_thermal(node_t *node);
extern int feenox_problem_gradient_add_elemental_contribution_to_node_thermal(node_t *node, element_t *element, unsigned int j, double rel_weight);
extern int feenox_problem_gradient_fill_fluxes_thermal(mesh_t *mesh, size_t j);

// thermal/post.c
extern int feenox_problem_solve_post_thermal(void);

#endif
