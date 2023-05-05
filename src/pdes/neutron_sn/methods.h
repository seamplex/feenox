/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for neutron transport FEM: virtual methods
 *
 *  Copyright (C) 2023 jeremy theler
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
#ifndef NEUTRON_TRANSPORT_FEM_METHODS_H
#define NEUTRON_TRANSPORT_FEM_METHODS_H

// neutron_sn/parser.c
extern int feenox_problem_parse_problem_neutron_sn(const char *);
extern int feenox_problem_parse_post_neutron_sn(mesh_write_t *mesh_write, const char *token);

// neutron_sn/init.c
extern int feenox_problem_neutron_sn_init_cosines(double *mu, double mu1);
extern int feenox_problem_init_parser_neutron_sn(void);
extern int feenox_problem_init_runtime_neutron_sn(void);
#ifdef HAVE_PETSC
extern int feenox_problem_setup_pc_neutron_sn(PC pc);
extern int feenox_problem_setup_ksp_neutron_sn(KSP ksp);
#endif
#ifdef HAVE_SLEPC
extern int feenox_problem_setup_eps_neutron_sn(EPS eps);
#endif

// neutron_sn/bulk.c
extern int feenox_problem_build_allocate_aux_neutron_sn(unsigned int n_nodes);
extern int feenox_problem_build_volumetric_gauss_point_neutron_sn(element_t *element, unsigned int v);

// neutron_sn/bc.c
extern int feenox_problem_bc_parse_neutron_sn(bc_data_t *bc_data, const char *lhs, char *rhs);
extern int feenox_problem_bc_set_neutron_sn_vacuum(bc_data_t *this, element_t *e, size_t j_global);
extern int feenox_problem_bc_set_neutron_sn_mirror(bc_data_t *this, element_t *e, size_t j_global);

// neutron_sn/post.c
extern int feenox_problem_solve_post_neutron_sn(void);
#endif  // NEUTRON_TRANSPORT_FEM
