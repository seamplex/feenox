/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox mechanical header
 *
 *  Copyright (C) 2021-2022 jeremy theler
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
#define BC_TYPE_MECHANICAL_TANGENTIAL_SYMMETRY  2
#define BC_TYPE_MECHANICAL_RADIAL_SYMMETRY      3
#define BC_TYPE_MECHANICAL_MULTIDOF_EXPRESSION  4
#define BC_TYPE_MECHANICAL_PRESSURE_TENSION     16
#define BC_TYPE_MECHANICAL_PRESSURE_COMPRESSION 17
#define BC_TYPE_MECHANICAL_TRACTION             18
#define BC_TYPE_MECHANICAL_FORCE                19

typedef struct mechanical_t mechanical_t;
typedef struct feenox_linearize_t feenox_linearize_t;

struct mechanical_t {
  
  enum {
    variant_full,
    variant_plane_stress,
    variant_plane_strain,
    variant_axisymmetric,
  } variant;  
  
  // TODO: have a "mixed" material model where each volume has its own model
  enum {
    material_model_unknown,
    material_model_elastic_isotropic,
    material_model_elastic_orthotropic,    
  } material_model;

  enum {
    thermal_expansion_model_none,
    thermal_expansion_model_isotropic,
    thermal_expansion_model_orthotropic,    
  } thermal_expansion_model;
  
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
  distribution_t T_ref; // reference temperature (has to be a constant)
  double T0;            // evaluated T_ref

  // volumetric force densityies
  distribution_t f_x;
  distribution_t f_y;
  distribution_t f_z;
  
  // flags to speed up things
  int uniform_C;
  int constant_C;
  int uniform_expansion;
  int constant_expansion;
  
  int n_nodes;
  int stress_strain_size;

  // holder for the rigid-body displacements
#ifdef HAVE_PETSC  
  MatNullSpace rigid_body_base;
#endif
  
  // C-like virtual methods (i.e. function pointers)
  int (*compute_C)(const double *x, material_t *material);
  int (*compute_stress_from_strain)(node_t *node, element_t *element, unsigned int j,
    double epsilonx, double epsilony, double epsilonz, double gammaxy, double gammayz, double gammazx,
    double *sigmax, double *sigmay, double *sigmaz, double *tauxy, double *tauyz, double *tauzx);

  int (*compute_thermal_strain)(const double *x, material_t *material);
  int (*compute_thermal_stress)(const double *x, material_t *material, double *sigmat_x, double *sigmat_y, double *sigmat_z);

  
  // auxiliary intermediate matrices
  gsl_matrix *C;    // stress-strain matrix, 6x6 for 3d
  gsl_matrix *B;    // strain-displacement matrix, 6x(3*n_nodes) for 3d
  gsl_matrix *CB;   // product of C times B, 6x(3*n_nodes) for 3d
  gsl_vector *et;   // thermal strain vector, size 6 for 3d
  gsl_vector *Cet;  // product of C times et, size 6 for 3d
  
//  double hourglass_epsilon;
  
  // for implicit multi-dof BCs
  var_t *displ_for_bc[3];

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
  
  feenox_linearize_t *linearizes;
};


struct feenox_linearize_t {
  expr_t x1;
  expr_t y1;
  expr_t z1;
  expr_t x2;
  expr_t y2;
  expr_t z2;

// TODO: ignore through thickness
//  int ignore_through_thickness;
  file_t *file;
  
  // von mises
  var_t *M;     // membrane
  var_t *MB;    // membrane plus bending
  var_t *P;     // peak
  
  // tresca
  var_t *Mt;
  var_t *MBt;
  var_t *Pt;
  
  // principal1
  var_t *M1;
  var_t *MB1;
  var_t *P1;

  // principal2
  var_t *M2;
  var_t *MB2;
  var_t *P2;
  
  // principal3
  var_t *M3;
  var_t *MB3;
  var_t *P3;
  
  
  feenox_linearize_t *next;
};

#endif /* MECHANICAL_H */

