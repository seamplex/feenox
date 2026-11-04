/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for one-dimensional thermal-hydraulics: boundary conditions
 *
 *  Copyright (C) 2026 Jeremy Theler
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
#include "feenox.h"
#include "thermal_hydraulics.h"

int feenox_problem_bc_parse_thermal_hydraulics(bc_data_t *bc_data, const char *lhs, char *rhs) {

  // boundary conditions are applied over points (i.e. nodes) at the end of pipes
  // a single BC can have several data, i.e. BC inlet rho=753.632 v=4.43
  
///bc_thermal_hydraulics+p+usage p=<expr>
///bc_thermal_hydraulics+p+description Fixed pressure (in Pa) at the node, usually an outlet.
///bc_thermal_hydraulics+p+description If fluid enters through this node, it does so with the temperature given by `T`
///bc_thermal_hydraulics+p+description (or the node's temperature if `T` is not given).
  if (strcmp(lhs, "p") == 0) {
    bc_data->type_phys = BC_TYPE_TH_PRESSURE;
    
///bc_thermal_hydraulics+T+usage T=<expr>
///bc_thermal_hydraulics+T+description Temperature (in K) of the fluid that enters the domain through the node.
  } else if (strcmp(lhs, "T") == 0) {
    bc_data->type_phys = BC_TYPE_TH_TEMPERATURE;
    
///bc_thermal_hydraulics+rho+usage rho=<expr>
///bc_thermal_hydraulics+rho+description Density (in kg/m^3) of the fluid that enters the domain through the node.
///bc_thermal_hydraulics+rho+description The temperature is computed from the density and the node's pressure.
  } else if (strcmp(lhs, "rho") == 0) {
    bc_data->type_phys = BC_TYPE_TH_DENSITY;

///bc_thermal_hydraulics+v+usage v=<expr>
///bc_thermal_hydraulics+v+description Velocity (in m/s) of the fluid entering the domain through the node.
///bc_thermal_hydraulics+v+description The mass flow is $\dot{m} = \rho \cdot v \cdot A$, where $A$ is the area of the adjacent pipe.
///bc_thermal_hydraulics+v+description Negative values mean the fluid leaves the domain.
  } else if (strcmp(lhs, "v") == 0) {
    bc_data->type_phys = BC_TYPE_TH_VELOCITY;

///bc_thermal_hydraulics+mdot+usage mdot=<expr>
///bc_thermal_hydraulics+mdot+description Mass flow (in kg/s) entering the domain through the node.
///bc_thermal_hydraulics+mdot+description Negative values mean the fluid leaves the domain.
  } else if (strcmp(lhs, "mdot") == 0) {
    bc_data->type_phys = BC_TYPE_TH_MASS_FLOW;
    
  } else {
    feenox_push_error_message("unknown thermal_hydraulics boundary condition '%s'", lhs);
    return FEENOX_ERROR;
  }
  
  if (rhs == NULL) {
    feenox_push_error_message("boundary condition '%s' needs an expression", lhs);
    return FEENOX_ERROR;
  }

  // the BC is evaluated at the beginning of each solve, so we do not use
  // neither set_essential nor set_natural, we just need the expression
  feenox_call(feenox_expression_parse(&bc_data->expr, rhs));
  bc_data->space_dependent = feenox_depends_on_space(bc_data->expr.variables);
  
  return FEENOX_OK;
}
