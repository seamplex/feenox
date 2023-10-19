/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for neutron transport FEM: global header
 *
 *  Copyright (C) 2023 jeremy theler
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
#ifndef NEUTRON_SN_H
#define NEUTRON_SN_H

#define sn_dof_index(m,g) ((m)*neutron_sn.groups + (g))

typedef struct neutron_sn_t neutron_sn_t;



struct neutron_sn_t {

  unsigned int N;
  unsigned int directions;
  unsigned int groups;
  
  // directions and weights
  double **Omega;
  int **SN_triangle;
  double *w;

  // total XS
  distribution_t *Sigma_t;
  
  // absorption XS
  distribution_t *Sigma_a;
  
  // scattering XS
  distribution_t **Sigma_s0;
  distribution_t **Sigma_s1;
  
  // product of nu and fission XS
  distribution_t *nu_Sigma_f;

  // product of energy per fission and fission XS
//  distribution_t *e_Sigma_f;
  
  // independent neutron source
  distribution_t *S;
  
  
  // removal XSs: groups x groups
  gsl_matrix *R;

  // fission XSs: groups x groups
  gsl_matrix *X;
  
  // independent sources: groups
  gsl_vector *s;  
  
  // elemental matrices  
  unsigned int n_nodes;
  
  // Petrov-stabilized H_Gc
  gsl_matrix *P;
  
  // Direction matrix
  gsl_matrix *D;
  
  // intermediate
  gsl_matrix *DB;
  gsl_matrix *AH;
  gsl_matrix *XH;
  
  // elemental matrices (size J*M*G x J*M*G)
  
  gsl_matrix *Li;  // leakage
  gsl_matrix *Ai;  // absorption
  gsl_matrix *Fi;  // fission
  gsl_vector *Si;  // sources (vector)

  
  int has_sources;
  int has_fission;
  int space_XS;

  // fission spectrum
  vector_t *chi;
  
  // supg stabilization factor
  var_t *sn_alpha;
  
  // --- results ----------------------

  // effective multiplication factor
  var_t *keff;
  // scalar fluxes (array of G functions)
  function_t **phi;
  
  
};  
extern neutron_sn_t neutron_sn;


#endif /* NEUTRON_SN_H */

