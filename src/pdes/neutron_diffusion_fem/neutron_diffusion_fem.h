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
#ifndef NEUTRON_DIFFUSION_FEM_H
#define NEUTRON_DIFFUSION_FEM_H

#define DEFAULT_NEUTRON_DIFFUSION_FEM_MODES        10

typedef struct neutron_diffusion_fem_t neutron_diffusion_fem_t;

struct neutron_diffusion_fem_t {

  unsigned int groups;  
  
  distribution_t *D;
  distribution_t *sigma_t;
  distribution_t *sigma_a;
  distribution_t *nu_sigma_f;
  distribution_t **sigma_s;
  distribution_t *source;

#ifdef HAVE_PETSC  
//  PetscBool has_dirichlet_bcs;
  PetscBool has_sources;
  PetscBool has_fission;
  PetscBool space_XS;
#endif

  var_t *keff;
  
};  


#endif /* NEUTRON_DIFFUSION_FEM_H */

