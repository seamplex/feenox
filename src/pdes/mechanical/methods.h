/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox mechanical methods
 *
 *  Copyright (C) 2021-2025 Jeremy Theler
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
extern int feenox_problem_parse_write_post_mechanical(mesh_write_t *mesh_write, const char *);
extern int feenox_parse_linearize_stress();

// mechanical/init.c
extern int feenox_problem_parse_time_init_mechanical(void);
extern int feenox_problem_init_runtime_mechanical(void);
#ifdef HAVE_PETSC
extern int feenox_problem_setup_pc_mechanical(PC pc);
extern int feenox_problem_setup_ksp_mechanical(KSP ksp);
extern int feenox_problem_setup_snes_mechanical(SNES snes);
extern int feenox_problem_compute_rigid_nullspace(MatNullSpace *nullspace);
#endif
        
// mechanical/bc.c
extern int feenox_problem_bc_parse_mechanical(bc_data_t *bc_data, const char *lhs, char *rhs);

extern int feenox_problem_bc_set_mechanical_displacement(bc_data_t *bc_data, element_t *e, size_t j_global);
extern int feenox_problem_bc_set_mechanical_symmetry(bc_data_t *bc_data, element_t *e, size_t j_global);
extern int feenox_problem_bc_set_mechanical_radial(bc_data_t *bc_data, element_t *e, size_t j_global);
extern int feenox_problem_bc_set_mechanical_multifreedom(bc_data_t *bc_data, element_t *e, size_t j_global);
extern double feenox_mechanical_gsl_function_of_uvw(double x, void *params);

extern int feenox_problem_bc_set_mechanical_compression(bc_data_t *bc_data, element_t *e, unsigned int q);
extern int feenox_problem_bc_set_mechanical_tension(bc_data_t *bc_data, element_t *e, unsigned int q);
extern int feenox_problem_bc_set_mechanical_normal_stress(bc_data_t *bc_data, element_t *e, unsigned int q, signed int sign);
extern int feenox_problem_bc_set_mechanical_traction(bc_data_t *bc_data, element_t *e, unsigned int q);
extern int feenox_problem_bc_set_mechanical_force(bc_data_t *bc_data, element_t *e, unsigned int q);


// mechanical/bulk.c
extern int feenox_problem_build_allocate_aux_mechanical(unsigned int n_nodes);
extern int feenox_problem_build_volumetric_gauss_point_mechanical(element_t *, unsigned int q);
extern int feenox_problem_build_volumetric_gauss_point_mechanical_nonlinear(element_t *, unsigned int q);

// mechanical/strain.c
extern gsl_matrix *feenox_problem_mechanical_compute_gradient_displacement(const gsl_matrix *dhdx, const gsl_vector *u);
extern gsl_matrix *feenox_problem_mechanical_compute_gradient_deformation(const gsl_matrix *grad_u);
extern gsl_matrix *feenox_problem_mechanical_compute_strain_cauchy_green_left(const gsl_matrix *F);
extern gsl_matrix *feenox_problem_mechanical_compute_strain_green_lagrange(const gsl_matrix *C);

// ---- material models ---------------------------------

// -- linear elastic isotropic  ------
// init
extern int feenox_mechanical_material_init_linear_elastic(material_t *material, int i);
extern int feenox_mechanical_material_setup_linear_elastic(void);

// stress-strain 4th-order tensor
extern void feenox_problem_mechanical_compute_lambda_mu(const double *x, material_t *material, double *lambda, double *mu);
extern int feenox_problem_mechanical_compute_tangent_matrix_C_linear_elastic(const double *x, material_t *material);
extern int feenox_problem_mechanical_compute_tangent_matrix_C_elastic_plane_stress(const double *x, material_t *material);
extern int feenox_problem_mechanical_compute_tangent_matrix_C_elastic_plane_strain(const double *x, material_t *material);

// stress measure
extern gsl_matrix *feenox_problem_build_mechanical_stress_measure_linear_elastic(const double *x, material_t *material);

// -- linear elastic orthotropic  ------
// init
extern int feenox_mechanical_material_init_linear_elastic_orthotropic(material_t *material, int i);
extern int feenox_mechanical_material_setup_linear_elastic_orthotropic(void);

// stress-strain 4th-order tensor
extern int feenox_problem_mechanical_compute_tangent_matrix_C_elastic_orthotropic(const double *x, material_t *material);

// stress measure

// -- saint venant-kirchoff
// init
extern int feenox_mechanical_material_init_svk(material_t *material, int i);
extern int feenox_mechanical_material_setup_svk(void);


// -- neo-hookean ------
// init
extern int feenox_mechanical_material_init_neohookean(material_t *material, int i);
extern int feenox_mechanical_material_setup_neohookean(void);

// stress measure
extern gsl_matrix *feenox_problem_build_mechanical_stress_measure_neohookean(const double *x, material_t *material);
extern gsl_matrix *feenox_problem_mechanical_compute_stress_PK2_neohookean(const double *x, material_t *material);
extern int feenox_problem_mechanical_compute_tangent_matrix_C_neohookean(const double *x, material_t *material);

// TODO: this one is general
extern int feenox_problem_mechanical_compute_stress_cauchy_from_PK2(const gsl_matrix *F, const gsl_matrix *PK2);



// stress.c
extern int feenox_problem_mechanical_compute_stress_first_piola_kirchoff(void);
extern gsl_matrix *feenox_problem_mechanical_compute_stress_PK2_elastic(const double *x, material_t *material);
extern gsl_matrix *feenox_problem_mechanical_compute_stress_cauchy_neohookean(const double *x, material_t *material);
 
extern int feenox_stress_from_strain_linear_elastic(node_t *node, element_t *element, unsigned int j,
    double epsilonx, double epsilony, double epsilonz, double gammaxy, double gammayz, double gammazx,
    double *sigmax, double *sigmay, double *sigmaz, double *tauxy, double *tauyz, double *tauzx);


// thermal effects
extern gsl_vector *feenox_problem_mechanical_compute_thermal_strain_isotropic(const double *x, material_t *material);
extern int feenox_problem_mechanical_compute_thermal_stress_isotropic (const double *x, material_t *material, double *sigmat_x, double *sigmat_y, double *sigmat_z);

extern gsl_vector *feenox_problem_mechanical_compute_thermal_strain_orthotropic(const double *x, material_t *material);
extern int feenox_problem_mechanical_compute_thermal_stress_orthotropic (const double *x, material_t *material, double *sigmat_x, double *sigmat_y, double *sigmat_z);

// mechanical/post.c
extern int feenox_problem_solve_post_mechanical(void);

// mechanical/gradient.c
extern int feenox_problem_gradient_fill_mechanical(void);
extern int feenox_problem_gradient_properties_at_element_nodes_mechanical(element_t *element, mesh_t *mesh);
extern int feenox_problem_gradient_fluxes_at_node_alloc_mechanical(node_t *node);
extern int feenox_problem_gradient_add_elemental_contribution_to_node_mechanical(node_t *node, element_t *element, unsigned int j, double rel_weight);
extern int feenox_problem_gradient_fill_fluxes_mechanical(mesh_t *mesh, size_t j);

// mechanical/stress.c
extern int feenox_stress_from_strain_linear(node_t *node, element_t *element, unsigned int j, double epsilonx, double epsilony, double epsilonz, double gammaxy, double gammayz, double gammazx, double *sigmax, double *sigmay, double *sigmaz, double *tauxy, double *tauyx, double *tauzx);
extern int feenox_principal_stress_from_cauchy(double sigmax, double sigmay, double sigmaz, double tauxy, double tauyz, double tauzx, double *sigma1, double *sigma2, double *sigma3);
extern double feenox_vonmises_from_principal(double sigma1, double sigma2, double sigma3);
extern double feenox_vonmises_from_stress_tensor(double sigmax, double sigmay, double sigmaz, double tauxy, double tauyz, double tauzx);
extern double feenox_tresca_from_stress_tensor(double sigmax, double sigmay, double sigmaz, double tauxy, double tauyz, double tauzx);
extern int feenox_strain_energy(void);

// mechanical/linearize.c
extern int feenox_instruction_linearize(void *arg);


#endif
