#ifndef THERMAL_METHODS_H
#define THERMAL_METHODS_H

// thermal/init.c
extern int feenox_problem_parse_problem_thermal(const char *);
extern int feenox_problem_init_parser_thermal(void);
extern int feenox_problem_init_runtime_thermal(void);
#ifdef HAVE_PETSC
extern int feenox_problem_setup_pc_thermal(PC pc);
extern int feenox_problem_setup_ksp_thermal(KSP ksp);
#endif

// thermal/bulk.c
extern int feenox_problem_build_volumetric_gauss_point_thermal(element_t *element, unsigned int v);

// thermal/bc.c
extern int feenox_problem_bc_parse_thermal(bc_data_t *bc_data, const char *lhs, const char *rhs);

extern int feenox_problem_bc_set_thermal_temperature(bc_data_t *bc_data, size_t node_index);

extern int feenox_problem_bc_set_thermal_heatflux(element_t *element, bc_data_t *bc_data, unsigned int v);
extern int feenox_problem_bc_set_thermal_convection(element_t *element, bc_data_t *bc_data, unsigned int v);

// thermal/heatflux.c
extern int feenox_problem_gradient_fill_thermal(void);
extern int feenox_problem_gradient_properties_at_element_nodes_thermal(element_t *element, mesh_t *mesh);
extern int feenox_problem_gradient_fluxes_at_node_alloc_thermal(node_t *node);
extern int feenox_problem_gradient_add_elemental_contribution_to_node_thermal(node_t *node, element_t *element, unsigned int j, double rel_weight);
extern int feenox_problem_fill_fluxes(mesh_t *mesh, size_t j);

// thermal/post.c
extern int feenox_problem_solve_post_thermal(void);

#endif
