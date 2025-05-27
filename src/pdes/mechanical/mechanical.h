/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox mechanical header
 *
 *  Copyright (C) 2021-2025 Jeremy Theler
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
typedef struct mechanical_material_ctx_t mechanical_material_ctx_t;
typedef struct feenox_linearize_t feenox_linearize_t;

struct mechanical_material_ctx_t {

  enum {
    material_model_unknown,
    material_model_elastic_isotropic,
    material_model_elastic_orthotropic,    
    material_model_hyperelastic_svk,
    material_model_hyperelastic_neohookean,
  } material_model;

  enum {
    thermal_expansion_model_none,
    thermal_expansion_model_isotropic,
    thermal_expansion_model_orthotropic,    
  } thermal_expansion_model;
  
  // TODO: union with pointers to specific material data
  
};


struct mechanical_t {
  
  int nonlinear_geom;
  int nonlinear_material;
  
  enum {
    variant_full,
    variant_plane_stress,
    variant_plane_strain,
    variant_axisymmetric,
  } variant;  
  
  // global models using the enums above
  // if different volumes have different models, these are unknown and each material uses its own context
  int material_model;
  int thermal_expansion_model;
  
  // isotropic properties  
  distribution_t E;           // Young's modulus
  distribution_t nu;          // Poisson's ratio
  distribution_t lambda;      // Lame 
  distribution_t mu;          // Lame
  distribution_t K;           // bulk modulus
  distribution_t alpha;       // (mean) thermal expansion coefficient
  
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

  
  // this one depends if we are 3D or 2D
  unsigned int stress_strain_size;
  // this one changes when changing the element type (even between BCs and bulk)
  unsigned int n_nodes;

  // holder for the rigid-body displacements
#ifdef HAVE_PETSC  
  MatNullSpace rigid_body_base;
#endif
  
  // C-like virtual methods (i.e. function pointers)
  int (*compute_material_tangent)(const double *x, material_t *material);
  int (*compute_stress_from_strain)(node_t *node, element_t *element, unsigned int j,
    double epsilonx, double epsilony, double epsilonz, double gammaxy, double gammayz, double gammazx,
    double *sigmax, double *sigmay, double *sigmaz, double *tauxy, double *tauyz, double *tauzx);

  int (*compute_thermal_strain)(const double *x, material_t *material);
  int (*compute_thermal_stress)(const double *x, material_t *material, double *sigmat_x, double *sigmat_y, double *sigmat_z);

  
  // auxiliary intermediate matrices
  gsl_matrix *C_tangent;    // stress-strain tangent matrix (i.e. fourth-order tensor in voigt projection 6x6 for 3d)
  gsl_matrix *B_shape;      // matrix with the derivatives of the shape functions 6x(3*n_nodes) for 3d
  gsl_matrix *CB;           // product of C_tangent times B_shape, 6x(3*n_nodes) for 3d
  gsl_vector *et;           // thermal strain in voigt, size 6 for 3d
  gsl_vector *Cet;          // product of C times et, size 6 for 3d
  
  
  // non-linear stuff
  gsl_matrix *eye;     // 3x3 identity (the symbol I is already taken by complex.h)
  gsl_matrix *grad_u;  // displacement gradient
  gsl_matrix *F;       // deformation gradient
  gsl_matrix *F_inv;   // inverse deformation gradient
  double J;            // determinant of F
  
  gsl_matrix *eps;       // green-lagrange strain tensor
  gsl_matrix *B;       // left green-lagrange strain tensor
  gsl_matrix *C;       // right green-lagrange strain tensor
  gsl_matrix *C_inv;   // inverse right green-lagrange strain tensor

  gsl_matrix *PK1;       // first piola-kirchoff stress
  gsl_matrix *PK2;       // second piola-kirchoff stress
  gsl_vector *PK2_voigt; // PK2 in voigt notation
  gsl_matrix *Sigma;   // 9x9 expansion of PK2
  
  gsl_matrix *S;       // cauchy stress tensor
  gsl_matrix *G;       // matrix with derivatives of shape functions
  gsl_matrix *SigmaG;  // temporary holder
  
  // temporary non-linear
  gsl_matrix *SF;         // temporary holder
/*
  gsl_matrix *tmp;        // temporary holder
  gsl_matrix *twomuE;     // temporary holder
  gsl_matrix *invC;
*/
  
//  double hourglass_epsilon;

  var_t *ldef;
//  var_t *ldef_check;

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
  
  // strains
  function_t *exx;
  function_t *eyy;
  function_t *ezz;
  function_t *exy;
  function_t *eyz;
  function_t *ezx;
  
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
extern mechanical_t mechanical;

#endif /* MECHANICAL_H */

