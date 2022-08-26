/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox mechanical methods
 *
 *  Copyright (C) 2021-2022 jeremy theler
 *
 *  This file is part of Feenox <https://www.seamplex.com/feenox>.
 *
 *  feenox is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Feenox is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Feenox.  If not, see <http://www.gnu.org/licenses/>.
 *------------------- ------------  ----    --------  --     -       -         -
 */

#ifndef MECHANICAL_METHODS_H
#define MECHANICAL_METHODS_H

// mechanical/parse.c
extern int feenox_problem_parse_problem_mechanical(const char *);
extern int feenox_parse_linearize_stress();

// mechanical/init.c
extern int feenox_problem_init_parser_mechanical(void);
extern int feenox_problem_init_runtime_mechanical(void);
#ifdef HAVE_PETSC
extern int feenox_problem_setup_pc_mechanical(PC pc);
extern int feenox_problem_setup_ksp_mechanical(KSP ksp);
extern int feenox_problem_setup_snes_mechanical(SNES snes);
extern int feenox_problem_compute_rigid_nullspace(MatNullSpace *nullspace);
#endif
        
// mechanical/bc.c
extern int feenox_problem_bc_parse_mechanical(bc_data_t *bc_data, const char *lhs, char *rhs);

extern int feenox_problem_bc_set_mechanical_displacement(element_t *element, bc_data_t *bc_data, size_t node_global_index);
extern int feenox_problem_bc_set_mechanical_multifreedom(element_t *element, bc_data_t *bc_data, size_t node_global_index);

extern int feenox_problem_bc_set_mechanical_compression(element_t *element, bc_data_t *bc_data, unsigned int v);
extern int feenox_problem_bc_set_mechanical_tension(element_t *element, bc_data_t *bc_data, unsigned int v);
extern int feenox_problem_bc_set_mechanical_normal_stress(element_t *element, bc_data_t *bc_data, unsigned int v, signed int sign);
extern int feenox_problem_bc_set_mechanical_traction(element_t *element, bc_data_t *bc_data, unsigned int v);
extern int feenox_problem_bc_set_mechanical_force(element_t *element, bc_data_t *bc_data, unsigned int v);

// mechanical/bulk.c
extern int feenox_problem_build_allocate_aux_mechanical(size_t n_nodes);
extern int feenox_problem_build_volumetric_gauss_point_mechanical(element_t *, unsigned int v);

// material models
extern int feenox_problem_build_compute_mechanical_C_elastic_isotropic(const double *x, material_t *material);
extern int feenox_problem_build_compute_mechanical_C_elastic_plane_stress(const double *x, material_t *material);
extern int feenox_problem_build_compute_mechanical_C_elastic_plane_strain(const double *x, material_t *material);
extern int feenox_problem_build_compute_mechanical_C_elastic_orthotropic(const double *x, material_t *material);

extern int feenox_problem_build_compute_mechanical_strain_isotropic (const double *x, material_t *material);
extern int feenox_problem_build_compute_mechanical_strain_orthotropic (const double *x, material_t *material);

extern int feenox_stress_from_strain_elastic_isotropic(node_t *node, element_t *element, unsigned int j, double epsilonx, double epsilony, double epsilonz, double gammaxy, double gammayz, double gammazx, double *sigmax, double *sigmay, double *sigmaz, double *tauxy, double *tauyx, double *tauzx);
extern int feenox_problem_build_compute_mechanical_stress_isotropic (const double *x, material_t *material, double *sigmat_x, double *sigmat_y, double *sigmat_z);
extern int feenox_problem_build_compute_mechanical_stress_orthotropic (const double *x, material_t *material, double *sigmat_x, double *sigmat_y, double *sigmat_z);

// mechanical/post.c
extern int feenox_problem_solve_post_mechanical(void);

// mechanical/stress.c
extern int feenox_problem_gradient_fill_mechanical(void);
extern int feenox_problem_gradient_properties_at_element_nodes_mechanical(element_t *element, mesh_t *mesh);
extern int feenox_problem_gradient_fluxes_at_node_alloc_mechanical(node_t *node);
extern int feenox_problem_gradient_add_elemental_contribution_to_node_mechanical(node_t *node, element_t *element, unsigned int j, double rel_weight);
extern int feenox_problem_gradient_fill_fluxes_mechanical(mesh_t *mesh, size_t j);

extern int feenox_stress_from_strain(node_t *node, element_t *element, unsigned int j, double epsilonx, double epsilony, double epsilonz, double gammaxy, double gammayz, double gammazx, double *sigmax, double *sigmay, double *sigmaz, double *tauxy, double *tauyx, double *tauzx);
extern int feenox_principal_stress_from_cauchy(double sigmax, double sigmay, double sigmaz, double tauxy, double tauyz, double tauzx, double *sigma1, double *sigma2, double *sigma3);
extern double feenox_vonmises_from_principal(double sigma1, double sigma2, double sigma3);
extern double feenox_vonmises_from_stress_tensor(double sigmax, double sigmay, double sigmaz, double tauxy, double tauyz, double tauzx);
extern double feenox_tresca_from_stress_tensor(double sigmax, double sigmay, double sigmaz, double tauxy, double tauyz, double tauzx);

// mechanical/linearize.c
extern int feenox_instruction_linearize(void *arg);


#endif
