#ifndef LAPLACE_METHODS_H
#define LAPLACE_METHODS_H

// laplace/init.c
extern int feenox_problem_parse_problem_laplace(const char *);
extern int feenox_problem_init_parser_laplace(void);
extern int feenox_problem_init_runtime_laplace(void);
#ifdef HAVE_PETSC
extern int feenox_problem_setup_pc_laplace(PC pc);
extern int feenox_problem_setup_ksp_laplace(KSP ksp);
#endif

// laplace/bulk.c
extern int feenox_problem_build_volumetric_gauss_point_laplace(element_t *element, unsigned int v);

// laplace/bc.c
extern int feenox_problem_bc_parse_laplace(bc_data_t *bc_data, const char *lhs, const char *rhs);
extern int feenox_problem_bc_set_laplace_phi(bc_data_t *bc_data, size_t node_index);
extern int feenox_problem_bc_set_laplace_derivative(element_t *element, bc_data_t *bc_data, unsigned int v);


#endif
