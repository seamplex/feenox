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
 *  feenox is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with feenox.  If not, see <http://www.gnu.org/licenses/>.
 *------------------- ------------  ----    --------  --     -       -         -
 */
#ifndef NEUTRON_TRANSPORT_H
#define NEUTRON_TRANSPORT_H

typedef struct neutron_transport_t neutron_transport_t;



struct neutron_transport_t {

  unsigned int N;
  unsigned int directions;
  unsigned int groups;
  
  // directions and weights
  double **Omega;
  double *w;
  
  
  // total XS
  distribution_t *sigma_t;
  
  // absorption XS
  distribution_t *sigma_a;
  
  // scattering XS
  distribution_t **sigma_s0;
  distribution_t **sigma_s1;
  
  // product of nu and fission XS
  distribution_t *nu_sigma_f;

  // product of energy per fission and fission XS
  distribution_t *e_sigma_f;
  
  // independent neutron source
  distribution_t *source;

//  int has_dirichlet_bcs;
  int has_sources;
  int has_fission;
  int space_XS;

  var_t *keff;
  
};  
extern neutron_transport_t neutron_transport;


#endif /* NEUTRON_TRANSPORT_FEM_H */

