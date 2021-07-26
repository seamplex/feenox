#ifndef MECHANICAL_H
#define MECHANICAL_H

#define BC_TYPE_MECHANICAL_UNDEFINED            0
#define BC_TYPE_MECHANICAL_DISPLACEMENT         1
#define BC_TYPE_MECHANICAL_PRESSURE_TENSION     2
#define BC_TYPE_MECHANICAL_PRESSURE_COMPRESSION 3

typedef struct mechanical_t mechanical_t;

struct mechanical_t {
  
  enum {
    variant_full,
    variant_plane_stress,
    variant_plane_strain
  } variant;  
  
  distribution_t E;     // Young's modulus
  distribution_t nu;    // Poisson's ratio
  distribution_t alpha; // (mean) thermal expansion coefficient
  distribution_t T;     // temperture distribution
  double T0;            // reference temperature

#ifdef HAVE_PETSC  
/*  
  PetscBool space_E;
  PetscBool space_nu;
  PetscBool space_alpha;
  PetscBool space_T;
 */
  
  MatNullSpace rigid_body_base;
#endif
  
  double hourglass_epsilon;
  
  var_t *U[3];

  var_t *strain_energy;

  var_t *displ_max;
  var_t *displ_max_x;
  var_t *displ_max_y;
  var_t *displ_max_z;

  var_t *u_at_displ_max;
  var_t *v_at_displ_max;
  var_t *w_at_displ_max;
    
  var_t *sigma_max;
  var_t *sigma_max_x;
  var_t *sigma_max_y;
  var_t *sigma_max_z;
  var_t *delta_sigma_max;

  var_t *u_at_sigma_max;
  var_t *v_at_sigma_max;
  var_t *w_at_sigma_max;
  
  // cauchy stresses
  function_t *sigmax;
  function_t *sigmay;
  function_t *sigmaz;
  function_t *tauxy;
  function_t *tauyz;
  function_t *tauzx;

  function_t *sigma1;      // principal stresses
  function_t *sigma2;
  function_t *sigma3;
  function_t *sigma;       // von mises
  function_t *delta_sigma; // uncertainty
  function_t *tresca;

};

#endif /* MECHANICAL_H */

