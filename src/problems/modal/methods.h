#ifndef MODAL_METHODS_H
#define MODAL_METHODS_H

// modal/init.c
extern int feenox_problem_parse_modal(const char *);
extern int feenox_problem_init_parser_modal(void);
extern int feenox_problem_init_runtime_modal(void);
#ifdef HAVE_PETSC
extern int feenox_problem_setup_pc_modal(PC pc);
extern int feenox_problem_setup_ksp_modal(KSP ksp);
#endif
#ifdef HAVE_SLEPC
extern int feenox_problem_setup_eps_modal(EPS eps);
#endif

// modal/bulk.c
extern int feenox_problem_build_volumetric_gauss_point_modal(element_t *element, unsigned int v);

// modal/bc.c
extern int feenox_problem_bc_parse_modal(bc_data_t *bc_data, const char *lhs, const char *rhs);
extern int feenox_problem_bc_set_dirichlet_modal(bc_data_t *bc_data, size_t node, size_t *index);

// modal/post.c
extern int feenox_problem_solve_post_modal(void);
#endif
