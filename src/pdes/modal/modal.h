#ifndef MODAL_H
#define MODAL_H

#define DEFAULT_MODAL_MODES        10

typedef struct modal_t modal_t;

struct modal_t {

  enum {
    variant_full,
    variant_plane_stress,
    variant_plane_strain
  } variant;  
  
  int has_dirichlet_bcs;
  
  distribution_t E;     // Young's modulus
  distribution_t nu;    // Poisson's ratio
  distribution_t rho;   // density

#ifdef HAVE_PETSC  
  PetscBool space_E;
  PetscBool space_nu;
  PetscBool space_rho;
  
  MatNullSpace rigid_body_base;
  
#endif
  
  var_t *M_T;
  vector_t *f;
  vector_t *omega;
  vector_t *m;
  vector_t *L;
  vector_t *Gamma;
  vector_t *mu;
  vector_t *Mu;
};  


#endif /* MODAL_H */

