#ifndef LAPLACE_H
#define LAPLACE_H

#define BC_TYPE_LAPLACE_UNDEFINED             0
#define BC_TYPE_LAPLACE_DIRICHLET             1
#define BC_TYPE_LAPLACE_NEUMANN               2

typedef struct laplace_t laplace_t;

struct laplace_t {
  distribution_t f;
  distribution_t alpha;

#ifdef HAVE_PETSC  
  PetscBool space_mass;
  PetscBool phi_mass;
  
  PetscBool space_source;
  PetscBool phi_source;

  PetscBool space_bc;
  PetscBool phi_bc;
#endif
  
  // caches for uniform properties
  struct {
    double f;
    double alpha;
  } cache;
  
};  


#endif /* LAPLACE_H */

