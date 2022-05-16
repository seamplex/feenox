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
 *  feenox is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with feenox.  If not, see <http://www.gnu.org/licenses/>.
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
    variant_plane_strain
  } variant;  
  
  int has_dirichlet_bcs;
  
  distribution_t E;     // Young's modulus
  distribution_t nu;    // Poisson's ratio
  distribution_t rho;   // density

  int space_E;
  int space_nu;
  int space_rho;
  
#ifdef HAVE_PETSC  
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

