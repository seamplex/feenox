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
#ifndef NEUTRON_TRANSPORT_H
#define NEUTRON_TRANSPORT_H

#define dof_index(n,g) ((n)*neutron_transport.groups + (g))

typedef struct neutron_transport_t neutron_transport_t;



struct neutron_transport_t {

  unsigned int N;
  unsigned int directions;
  unsigned int groups;
  
  // directions and weights
  double **Omega;
  double *w;

  // for SUPG: factor relating volume/area of an element depending on the dimension
  // if there were circles, spheres: 1 for 1D, 4 for 2D and 6 for 3D
  // if there were triang, tets:     1 for 1D, 6 for 2D and 12 for 3d
  double supg_dimension_factor;
  
  
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
  gsl_matrix *removal;

  // fission XSs: groups x groups
  gsl_matrix *nufission;
  
  // independent sources: groups
  gsl_vector *src;  
  
  // fission spectrum: groups
  double *chi;
  

  
  // elemental matrices  
  unsigned int n_nodes;
  
  gsl_matrix *P;
  gsl_matrix *OMEGA;
  gsl_matrix *OMEGAB;
  gsl_matrix *AH;
  gsl_matrix *XH;
  
  gsl_matrix *Ki;
  gsl_matrix *Ai;
  gsl_matrix *Xi;
  gsl_vector *Si;

  
  int has_sources;
  int has_fission;
  int space_XS;

  var_t *sn_alpha;
  
  // --- results ----------------------
  
  // effective multiplication factor
  var_t *keff;
  // scalar fluxes (array of G functions)
  function_t **phi;
  
  
};  
extern neutron_transport_t neutron_transport;


#endif /* NEUTRON_TRANSPORT_FEM_H */

