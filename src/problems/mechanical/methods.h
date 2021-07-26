#ifndef MECHANICAL_METHODS_H
#define MECHANICAL_METHODS_H

// mechanical/init.c
extern int feenox_problem_parse_mechanical(const char *);
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
  

// mechanical/bulk.c
extern int feenox_problem_build_volumetric_gauss_point_mechanical(element_t *this, unsigned int v);

// mechanical/post.c
extern int feenox_problem_solve_post_mechanical(void);

#endif
