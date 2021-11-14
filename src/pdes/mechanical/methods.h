#ifndef MECHANICAL_METHODS_H
#define MECHANICAL_METHODS_H

// mechanical/init.c
extern int feenox_problem_parse_problem_mechanical(const char *);
extern int feenox_problem_init_parser_mechanical(void);
extern int feenox_problem_init_runtime_mechanical(void);
#ifdef HAVE_PETSC
extern int feenox_problem_setup_pc_mechanical(PC pc);
extern int feenox_problem_setup_ksp_mechanical(KSP ksp);
extern int feenox_problem_setup_snes_mechanical(SNES snes);
extern int feenox_problem_compute_rigid_nullspace(MatNullSpace *nullspace);
#endif
        
// mechanical/bc.c
extern int feenox_problem_bc_parse_mechanical(bc_data_t *bc_data, const char *lhs, const char *rhs);

extern int feenox_problem_bc_set_mechanical_displacement(bc_data_t *bc_data, size_t node_index);

extern int feenox_problem_bc_set_mechanical_compression(element_t *element, bc_data_t *bc_data, unsigned int v);
extern int feenox_problem_bc_set_mechanical_tension(element_t *element, bc_data_t *bc_data, unsigned int v);
extern int feenox_problem_bc_set_mechanical_normal_stress(element_t *element, bc_data_t *bc_data, unsigned int v, signed int sign);
extern int feenox_problem_bc_set_mechanical_traction(element_t *element, bc_data_t *bc_data, unsigned int v);

// mechanical/bulk.c
extern int feenox_problem_build_allocate_aux_mechanical(size_t n_nodes);
extern int feenox_problem_build_volumetric_gauss_point_mechanical(element_t *this, unsigned int v);

// material models
extern int feenox_problem_build_compute_mechanical_C_elastic_isotropic(const double *x, material_t *material);
extern int feenox_problem_build_compute_mechanical_C_elastic_plane_stress(const double *x, material_t *material);
extern int feenox_problem_build_compute_mechanical_C_elastic_plane_strain(const double *x, material_t *material);


// mechanical/post.c
extern int feenox_problem_solve_post_mechanical(void);

// mechanical/stress.c
extern int feenox_problem_gradient_fill_mechanical(void);
extern int feenox_problem_gradient_properties_at_element_nodes_mechanical(element_t *element, mesh_t *mesh);
extern int feenox_problem_gradient_fluxes_at_node_alloc_mechanical(node_t *node);
extern int feenox_problem_gradient_add_elemental_contribution_to_node_mechanical(node_t *node, element_t *element, unsigned int j, double rel_weight);
extern int feenox_problem_gradient_fill_fluxes_mechanical(mesh_t *mesh, size_t j);

extern int feenox_principal_stress_compute(double sigmax, double sigmay, double sigmaz, double tauxy, double tauyz, double tauzx, double *sigma1, double *sigma2, double *sigma3);
extern double feenox_vonmises_from_principal(double sigma1, double sigma2, double sigma3);
extern double feenox_vonmises_from_stress_tensor(double sigmax, double sigmay, double sigmaz, double tauxy, double tauyz, double tauzx);

#endif
