/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox mechanical header
 *
 *  Copyright (C) 2015-2022 jeremy theler
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

#ifndef MECHANICAL_H
#define MECHANICAL_H

#define BC_TYPE_MECHANICAL_UNDEFINED            0
#define BC_TYPE_MECHANICAL_DISPLACEMENT         1
#define BC_TYPE_MECHANICAL_PRESSURE_TENSION     2
#define BC_TYPE_MECHANICAL_PRESSURE_COMPRESSION 3
#define BC_TYPE_MECHANICAL_TRACTION             4
#define BC_TYPE_MECHANICAL_FORCE                5

typedef struct mechanical_t mechanical_t;

struct mechanical_t {
  
  enum {
    variant_full,
    variant_plane_stress,
    variant_plane_strain,
    variant_axisymmetric,
  } variant;  
  
  enum {
    material_model_unknown,
    material_model_elastic_isotropic,
    material_model_elastic_orthotropic,    
  } material_model;

  // isotropic properties  
  distribution_t E;     // Young's modulus
  distribution_t nu;    // Poisson's ratio
  distribution_t alpha; // (mean) thermal expansion coefficient
  
  // orthotropic properties
  distribution_t E_x, E_y, E_z;             // Young's moduli
  distribution_t nu_xy, nu_yz, nu_zx;       // Poisson's ratios
  distribution_t G_xy, G_yz, G_zx;          // Shear moduli
  distribution_t alpha_x, alpha_y, alpha_z; // (mean) thermal expansion coefficient
  
  // temperature field
  distribution_t T;     // temperature distribution
  double T0;            // reference temperature

#ifdef HAVE_PETSC  
  PetscBool space_dependent_properties;
  MatNullSpace rigid_body_base;
#endif
  
  size_t n_nodes;
  size_t stress_strain_size;

  int (*compute_C)(const double *x, material_t *material);
  
  
  // auxiliary intermediate matrices
  gsl_matrix *C;    // stress-strain matrix, 6x6 for 3d
  gsl_matrix *B;    // strain-displacement matrix, 6x(3*n_nodes) for 3d
  gsl_matrix *CB;   // product of C times B, 6x(3*n_nodes) for 3d
  gsl_vector *et;   // thermal strain vector, size 6 for 3d
  gsl_vector *Cet;  // product of C times et, size 6 for 3d
  
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

