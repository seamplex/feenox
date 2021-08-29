/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for the heat equation: global header
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
#ifndef THERMAL_H
#define THERMAL_H

#define BC_TYPE_THERMAL_UNDEFINED               0
#define BC_TYPE_THERMAL_TEMPERATURE             1
#define BC_TYPE_THERMAL_HEATFLUX                2
#define BC_TYPE_THERMAL_CONVECTION_COEFFICIENT  3
#define BC_TYPE_THERMAL_CONVECTION_TEMPERATURE  4

typedef struct thermal_t thermal_t;

struct thermal_t {
  distribution_t k;           // isotropic conductivity 
  distribution_t kx, ky, kz;  // orthotropic conductivity 
  distribution_t q;           // volumetric heat source
  distribution_t kappa;       // thermal diffusivity = k/(rho cp)
  distribution_t rho;         // density
  distribution_t cp;          // heat capacity
  distribution_t rhocp;       // density times heat capacity

#ifdef HAVE_PETSC  
  PetscBool space_stiffness;
  PetscBool temperature_stiffness;

  PetscBool space_mass;
  PetscBool temperature_mass;
  
  PetscBool space_source;
  PetscBool temperature_source;

  PetscBool space_bc;
  PetscBool temperature_bc;
#endif
  
  // heat fluxes
  function_t *qx;
  function_t *qy;
  function_t *qz;
  
  // extrema
  var_t *T_max;
  var_t *T_min;
  
  // caches for uniform properties
  struct {
    double k;
    double kx, ky, kz;
    double q;
    double kappa;
    double rho;
    double cp;
    double rhocp;
  } cache;
  
};  


#endif /* THERMAL_H */

