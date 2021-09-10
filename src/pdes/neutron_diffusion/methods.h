/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for neutron diffusion FEM: virtual methods
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
 *  feenox is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with feenox.  If not, see <http://www.gnu.org/licenses/>.
 *------------------- ------------  ----    --------  --     -       -         -
 */
#ifndef NEUTRON_DIFFUSION_FEM_METHODS_H
#define NEUTRON_DIFFUSION_FEM_METHODS_H

// neutron_diffusion/init.c
extern int feenox_problem_parse_problem_neutron_diffusion(const char *);
extern int feenox_problem_init_parser_neutron_diffusion(void);
extern int feenox_problem_init_runtime_neutron_diffusion(void);
#ifdef HAVE_PETSC
extern int feenox_problem_setup_pc_neutron_diffusion(PC pc);
extern int feenox_problem_setup_ksp_neutron_diffusion(KSP ksp);
#endif
#ifdef HAVE_SLEPC
extern int feenox_problem_setup_eps_neutron_diffusion(EPS eps);
#endif

// neutron_diffusion/bulk.c
extern int feenox_problem_build_volumetric_gauss_point_neutron_diffusion(element_t *element, unsigned int v);

// neutron_diffusion/bc.c
extern int feenox_problem_bc_parse_neutron_diffusion(bc_data_t *bc_data, const char *lhs, const char *rhs);
extern int feenox_problem_bc_set_neutron_diffusion_null(bc_data_t *bc_data, size_t node_index);
extern int feenox_problem_bc_set_neutron_diffusion_vacuum(element_t *element, bc_data_t *bc_data, unsigned int v);

// neutron_diffusion/post.c
extern int feenox_problem_solve_post_neutron_diffusion(void);
#endif  // NEUTRON_DIFFUSION_FEM
