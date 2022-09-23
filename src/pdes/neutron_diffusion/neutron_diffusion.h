/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for neutron diffusion FEM: global header
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
#ifndef NEUTRON_DIFFUSION_H
#define NEUTRON_DIFFUSION_H

typedef struct neutron_diffusion_t neutron_diffusion_t;

struct neutron_diffusion_t {

  unsigned int groups;  
  
  distribution_t *D;
  distribution_t *sigma_t;
  distribution_t *sigma_a;
  distribution_t *nu_sigma_f;
  distribution_t **sigma_s;
  distribution_t *source;

//  int has_dirichlet_bcs;
  int has_sources;
  int has_fission;
  int space_XS;

  var_t *keff;
  
};  
extern neutron_diffusion_t neutron_diffusion;


#endif /* NEUTRON_DIFFUSION_FEM_H */

