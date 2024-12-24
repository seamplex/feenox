/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for neutron diffusion FEM: global header
 *
 *  Copyright (C) 2021--2023 Jeremy Theler
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
#ifndef NEUTRON_DIFFUSION_H
#define NEUTRON_DIFFUSION_H

typedef struct neutron_diffusion_t neutron_diffusion_t;

struct neutron_diffusion_t {

  unsigned int groups;  
  
  distribution_t *D;
  distribution_t *Sigma_t;
  distribution_t *Sigma_a;
  distribution_t *nu_Sigma_f;
  distribution_t **Sigma_s;
  distribution_t *S;
  
  // diffusion coefficients, (groups * dim) x (groups * dim)
  gsl_matrix *D_G;
  
  // removal XSs: groups x groups
  gsl_matrix *R;

  // fission XSs: groups x groups
  gsl_matrix *X;
  
  // independent sources: groups
  gsl_vector *s;  
  
  unsigned int n_nodes;
  // elemental matrices (size J*G x J*G)
  gsl_matrix *Li;  // leakage
  gsl_matrix *Ai;  // absorption
  gsl_matrix *Fi;  // fission

  // intermediate matrices
  gsl_matrix *DB;
  gsl_matrix *RH;
  gsl_matrix *XH;
  
  
  int has_sources;
  int has_fission;
  int space_XS;

  // fission spectrum
  vector_t *chi;
  
  // effective multiplication factor
  var_t *keff;

  // neutron currents
  function_t **Jx;
  function_t **Jy;
  function_t **Jz;  
};  
extern neutron_diffusion_t neutron_diffusion;


#endif /* NEUTRON_DIFFUSION_FEM_H */

