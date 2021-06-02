#ifndef MODAL_H
#define MODAL_H

typedef struct modal_t modal_t;

struct modal_t {
  distribution_t E;     // Young's modulus
  distribution_t nu;    // Poisson's ratio
  distribution_t rho;   // density

#ifdef HAVE_PETSC  
  PetscBool space_E;
  PetscBool space_nu;
  PetscBool space_rho;
#endif
  
};  


#endif /* MODAL_H */

