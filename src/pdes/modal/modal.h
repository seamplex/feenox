/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for modal analysis: global header
 *
 *  Copyright (C) 2021 jeremy theler
 *
 *  This file is part of FeenoX <https://www.seamplex.com/feenox>.
 *
 *  feenox is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  FeenoX is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with FeenoX.  If not, see <http://www.gnu.org/licenses/>.
 *------------------- ------------  ----    --------  --     -       -         -
 */
#ifndef MODAL_H
#define MODAL_H

#define DEFAULT_MODAL_MODES        10

#define BC_TYPE_MECHANICAL_UNDEFINED            0
#define BC_TYPE_MECHANICAL_DISPLACEMENT         1
#define BC_TYPE_MECHANICAL_TANGENTIAL_SYMMETRY  2
#define BC_TYPE_MECHANICAL_RADIAL_SYMMETRY      3
#define BC_TYPE_MECHANICAL_MULTIDOF_EXPRESSION  4


typedef struct modal_t modal_t;

struct modal_t {

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
  
  int has_dirichlet_bcs;

  distribution_t rho;   // density
  
  // isotropic properties    
  distribution_t E;     // Young's modulus
  distribution_t nu;    // Poisson's ratio
  
  // orthotropic properties
  distribution_t E_x, E_y, E_z;             // Young's moduli
  distribution_t nu_xy, nu_yz, nu_zx;       // Poisson's ratios
  distribution_t G_xy, G_yz, G_zx;          // Shear moduli
  distribution_t alpha_x, alpha_y, alpha_z; // (mean) thermal expansion coefficient  

  int space_E;
  int space_nu;
  int space_rho;

  // flags to speed up things
  int uniform_C;
  int constant_C;
  int uniform_expansion;
  int constant_expansion;
  
  unsigned int n_nodes;
  unsigned int stress_strain_size;

  // holder for the rigid-body displacements
#ifdef HAVE_PETSC  
  MatNullSpace rigid_body_base;
#endif
  
  int (*compute_C)(const double *x, material_t *material);
  
  // auxiliary intermediate matrices
  gsl_matrix *C;    // stress-strain matrix, 6x6 for 3d
  gsl_matrix *B;    // strain-displacement matrix, 6x(3*n_nodes) for 3d
  gsl_matrix *CB;   // product of C times B, 6x(3*n_nodes) for 3d
  gsl_vector *et;   // thermal strain vector, size 6 for 3d
  gsl_vector *Cet;  // product of C times et, size 6 for 3d
  
  var_t *M_T;
  vector_t *f;
  vector_t *omega;
  vector_t *m;
  vector_t *L;
  vector_t *Gamma;
  vector_t *mu;
  vector_t *Mu;
};  
extern modal_t modal;

#endif /* MODAL_H */

