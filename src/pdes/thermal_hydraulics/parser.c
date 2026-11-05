/*------------ -------------- -------- --- ----- ---   --       -            -
 *  FeenoX parser for thermal_hydraulics-specific keywords
 *
 *  Copyright (C) 2026 Jeremy Theler
 *
 *  This file is part of FeenoX.
 *
 *  FeenoX is free software: you can redistribute it and/or modify
 *  it under the terms "of the GNU General Public License as published by
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
#include "../../parser/parser.h"
#include "thermal_hydraulics.h"

///pb_thermal_hydraulics+NONE+description The `thermal_hydraulics` problem does not need any extra keyword to `PROBLEM`.
int feenox_problem_parse_problem_thermal_hydraulics(const char *token) {

  // TODO: FLUID keyword to choose the equation of state
  // TODO: one or two phases
  if (token != NULL) {
    feenox_push_error_message("undefined keyword '%s' for thermal_hydraulics PROBLEM", token);
    return FEENOX_ERROR;
  } else {
    // if token is NULL we have to do the parse-time initialization
    feenox_call(feenox_problem_parse_time_init_thermal_hydraulics());
  }

  return FEENOX_OK;
}

int feenox_parse_heat_structure(void) {

///kw_thermal_hydraulics+HEAT_STRUCTURE+desc Couple a cylindrical fuel-gap-clad heat structure to a physical pipe group.
///kw_thermal_hydraulics+HEAT_STRUCTURE+usage HEAT_STRUCTURE PIPE <pipe_group> FUEL <fuel_material> [ GAP <gap_material> ] [ CLAD <clad_material> ] [ AXIAL_CONDUCTION | NO_AXIAL_CONDUCTION ]
///kw_thermal_hydraulics+HEAT_STRUCTURE+detail `AXIAL_CONDUCTION` (default) includes radial and axial conduction in this structure. `NO_AXIAL_CONDUCTION` uses independent radial columns. These flags take no parameters and are mutually exclusive.
///kw_thermal_hydraulics+HEAT_STRUCTURE+detail Secondary keywords can appear in any order. `PIPE` and `FUEL` are mandatory; omitted `GAP` and `CLAD` materials each default to `FUEL`. This does not remove either geometric layer.
///kw_thermal_hydraulics+HEAT_STRUCTURE+detail Declare after `PROBLEM thermal_hydraulics`, before using solid output functions or writing results.
///kw_thermal_hydraulics+HEAT_STRUCTURE+detail Any number of heat structures can be declared, keyed by distinct pipe-group names. Named solid materials need constant `k`, `rho_s`, and `cp`.
///kw_thermal_hydraulics+HEAT_STRUCTURE+detail Each structure defines `T_fuel_center_<pipe_group>`, `T_fuel_surface_<pipe_group>`, `T_clad_surface_<pipe_group>`, `hs_heat_<pipe_group>`, and `hs_energy_<pipe_group>`.
///kw_thermal_hydraulics+HEAT_STRUCTURE+detail `WRITE_RESULTS all` includes every structure's temperature fields; individual suffixed temperature names can also be selected.
///kw_thermal_hydraulics+HEAT_STRUCTURE+detail Unsuffixed temperature functions refer to the first declared structure for compatibility; `hs_heat` and `hs_energy` sum all structures. At shared fluid nodes each solid retains its own temperature; distinct structures are not conductively connected.
///kw_thermal_hydraulics+HEAT_STRUCTURE+detail Solid density uses `rho_s` to avoid colliding with the fluid solution function `rho`.
///kw_thermal_hydraulics+HEAT_STRUCTURE+detail The channel material defines `fuel_radius`, `gap_width`, `clad_width`, `fuel_cells`, `gap_cells`, `clad_cells`, and total `hs_power` in W.
///kw_thermal_hydraulics+HEAT_STRUCTURE+detail Radius and layer widths are positive lengths in m; radial cell counts are positive integers. Geometry is evaluated once and must not depend on space or time.
///kw_thermal_hydraulics+HEAT_STRUCTURE+detail Solid properties must be constant expressions without variable or user-function dependencies, in W/(m K), kg/m^3, and J/(kg K), respectively.
///kw_thermal_hydraulics+HEAT_STRUCTURE+detail Power is uniform in the fuel and along the channel; the centerline and axial ends are insulated.
///kw_thermal_hydraulics+HEAT_STRUCTURE+detail `hs_power` can vary with time but must not depend on space. A positive prescribed `h_wall` in W/(m^2 K) couples cladding to coolant; automatic convection is not yet supported for heat structures.
///kw_thermal_hydraulics+HEAT_STRUCTURE+detail The heated perimeter defaults to the outer cladding circumference; an explicit `P_h` must be positive. Do not combine a heat structure with `T_wall` or `q_wall`.
///kw_thermal_hydraulics+HEAT_STRUCTURE+detail The coolant remains single-phase IF97 region-1 water; boiling and two-phase heat transfer are not modeled.
  if (feenox.pde.parse_problem != feenox_problem_parse_problem_thermal_hydraulics) {
    feenox_push_error_message("HEAT_STRUCTURE requires PROBLEM thermal_hydraulics first");
    return FEENOX_ERROR;
  }
  const char *keywords[] = {"PIPE", "FUEL", "GAP", "CLAD"};
  char *values[4] = {NULL};
  int axial_conduction = 1;
  int axial_given = 0;
  char *token = NULL;
  while ((token = feenox_get_next_token(NULL)) != NULL) {
    if (strcasecmp(token, "AXIAL_CONDUCTION") == 0 || strcasecmp(token, "NO_AXIAL_CONDUCTION") == 0) {
      if (axial_given) {
        feenox_push_error_message("HEAT_STRUCTURE allows only one AXIAL_CONDUCTION or NO_AXIAL_CONDUCTION flag");
        return FEENOX_ERROR;
      }
      axial_conduction = (strcasecmp(token, "AXIAL_CONDUCTION") == 0);
      axial_given = 1;
      continue;
    }
    int i = 0;
    while (i < 4 && strcasecmp(token, keywords[i]) != 0) i++;
    if (i == 4) {
      feenox_push_error_message("undefined keyword '%s' for HEAT_STRUCTURE", token);
      return FEENOX_ERROR;
    }
    if (values[i] != NULL) {
      feenox_push_error_message("duplicate keyword '%s' for HEAT_STRUCTURE", keywords[i]);
      return FEENOX_ERROR;
    }
    if ((token = feenox_get_next_token(NULL)) == NULL || token[0] == '\0') {
      feenox_push_error_message("HEAT_STRUCTURE keyword '%s' needs a parameter", keywords[i]);
      return FEENOX_ERROR;
    }
    feenox_check_alloc(values[i] = strdup(token));
  }
  for (int i = 0; i < 2; i++) {
    if (values[i] == NULL) {
      feenox_push_error_message("HEAT_STRUCTURE requires keyword '%s'", keywords[i]);
      return FEENOX_ERROR;
    }
  }
  for (int i = 2; i < 4; i++) {
    if (values[i] == NULL) {
      feenox_check_alloc(values[i] = strdup(values[1]));
    }
  }
  char *group_name = values[0];
  th_heat_structure_t *hs = NULL;
  HASH_FIND_STR(thermal_hydraulics.heat_structures, group_name, hs);
  if (hs != NULL) {
    feenox_push_error_message("HEAT_STRUCTURE for pipe group '%s' already defined", group_name);
    feenox_free(group_name);
    return FEENOX_ERROR;
  }
  feenox_check_alloc(hs = calloc(1, sizeof(th_heat_structure_t)));
  hs->group_name = group_name;
  hs->axial_conduction = axial_conduction;
  for (int i = 0; i < 3; i++) {
    hs->material_name[i] = values[i+1];
  }
  const char *names[] = {"T_fuel_center", "T_fuel_surface", "T_clad_surface", "hs_heat", "hs_energy"};
  function_t **functions[] = {&hs->center_function, &hs->fuel_surface_function, &hs->clad_surface_function};
  var_t **variables[] = {&hs->heat, &hs->energy};
  for (int i = 0; i < 5; i++) {
    char *name = NULL;
    feenox_check_minusone(asprintf(&name, "%s_%s", names[i], group_name));
    if (i < 3) {
      feenox_call(feenox_problem_define_solution_function(name, functions[i], FEENOX_SOLUTION_NOT_GRADIENT));
    } else {
      feenox_check_alloc(*variables[i-3] = feenox_get_or_define_variable_get_ptr(name));
    }
    feenox_free(name);
  }
  if (thermal_hydraulics.heat_structures == NULL) {
///re_thermal_hydraulics+T_fuel_center+description First heat structure's fuel centerline temperature (K), NaN outside its pipe group. Use `T_fuel_center_<pipe_group>` for each structure.
    feenox_call(feenox_problem_define_solution_function("T_fuel_center", &thermal_hydraulics.fuel_center_function, FEENOX_SOLUTION_NOT_GRADIENT));
///re_thermal_hydraulics+T_fuel_surface+description First heat structure's fuel outer-surface temperature (K). Use `T_fuel_surface_<pipe_group>` for each structure.
    feenox_call(feenox_problem_define_solution_function("T_fuel_surface", &thermal_hydraulics.fuel_surface_function, FEENOX_SOLUTION_NOT_GRADIENT));
///re_thermal_hydraulics+T_clad_surface+description First heat structure's cladding outer-surface temperature (K). Use `T_clad_surface_<pipe_group>` for each structure.
    feenox_call(feenox_problem_define_solution_function("T_clad_surface", &thermal_hydraulics.clad_surface_function, FEENOX_SOLUTION_NOT_GRADIENT));
///va_thermal_hydraulics+hs_heat+detail Total heat transferred from all solids to the fluid in W, positive into the fluid.
    feenox_check_alloc(thermal_hydraulics.hs_heat = feenox_get_or_define_variable_get_ptr("hs_heat"));
///va_thermal_hydraulics+hs_energy+detail Total solid sensible energy in J, with zero reference temperature and constant solid properties.
    feenox_check_alloc(thermal_hydraulics.hs_energy = feenox_get_or_define_variable_get_ptr("hs_energy"));
  }
  HASH_ADD_STR(thermal_hydraulics.heat_structures, group_name, hs);

  return FEENOX_OK;
}


int feenox_problem_parse_write_post_thermal_hydraulics(mesh_write_t *mesh_write, const char *token) {

  if (strcmp(token, "all") == 0) {
    
    feenox_call(feenox_problem_parse_write_post_thermal_hydraulics(mesh_write, "p"));
    feenox_call(feenox_problem_parse_write_post_thermal_hydraulics(mesh_write, "T"));
    feenox_call(feenox_problem_parse_write_post_thermal_hydraulics(mesh_write, "rho"));
    feenox_call(feenox_problem_parse_write_post_thermal_hydraulics(mesh_write, "h"));
    feenox_call(feenox_problem_parse_write_post_thermal_hydraulics(mesh_write, "v"));
    feenox_call(feenox_problem_parse_write_post_thermal_hydraulics(mesh_write, "mdot"));
    if (thermal_hydraulics.heat_structures != NULL) {
      feenox_call(feenox_problem_parse_write_post_thermal_hydraulics(mesh_write, "T_fuel_center"));
      feenox_call(feenox_problem_parse_write_post_thermal_hydraulics(mesh_write, "T_fuel_surface"));
      feenox_call(feenox_problem_parse_write_post_thermal_hydraulics(mesh_write, "T_clad_surface"));
      for (th_heat_structure_t *hs = thermal_hydraulics.heat_structures; hs != NULL; hs = hs->hh.next) {
        feenox_call(feenox_add_post_field(mesh_write, 1, &hs->center_function->name, NULL, field_location_nodes));
        feenox_call(feenox_add_post_field(mesh_write, 1, &hs->fuel_surface_function->name, NULL, field_location_nodes));
        feenox_call(feenox_add_post_field(mesh_write, 1, &hs->clad_surface_function->name, NULL, field_location_nodes));
      }
    }
    
  } else if (strcmp(token, "p") == 0 || strcmp(token, "pressure") == 0) {
    feenox_call(feenox_add_post_field(mesh_write, 1, &feenox.pde.solution[0]->name, NULL, field_location_nodes));
  } else if (strcmp(token, "T") == 0 || strcmp(token, "temperature") == 0) {
    feenox_call(feenox_add_post_field(mesh_write, 1, &feenox.pde.solution[1]->name, NULL, field_location_nodes));
  } else if (strcmp(token, "rho") == 0 || strcmp(token, "density") == 0) {
    feenox_call(feenox_add_post_field(mesh_write, 1, &thermal_hydraulics.rho_function->name, NULL, field_location_nodes));
  } else if (strcmp(token, "h") == 0 || strcmp(token, "enthalpy") == 0) {
    feenox_call(feenox_add_post_field(mesh_write, 1, &thermal_hydraulics.h_function->name, NULL, field_location_nodes));
  } else if (strcmp(token, "v") == 0 || strcmp(token, "velocity") == 0) {
    feenox_call(feenox_add_post_field(mesh_write, 1, &thermal_hydraulics.v_function->name, NULL, field_location_nodes));
  } else if (strcmp(token, "mdot") == 0 || strcmp(token, "mass_flow") == 0) {
    feenox_call(feenox_add_post_field(mesh_write, 1, &thermal_hydraulics.mdot_function->name, NULL, field_location_nodes));
  } else if (thermal_hydraulics.heat_structures != NULL && strcmp(token, "T_fuel_center") == 0) {
    feenox_call(feenox_add_post_field(mesh_write, 1, &thermal_hydraulics.fuel_center_function->name, NULL, field_location_nodes));
  } else if (thermal_hydraulics.heat_structures != NULL && strcmp(token, "T_fuel_surface") == 0) {
    feenox_call(feenox_add_post_field(mesh_write, 1, &thermal_hydraulics.fuel_surface_function->name, NULL, field_location_nodes));
  } else if (thermal_hydraulics.heat_structures != NULL && strcmp(token, "T_clad_surface") == 0) {
    feenox_call(feenox_add_post_field(mesh_write, 1, &thermal_hydraulics.clad_surface_function->name, NULL, field_location_nodes));
  } else {
    for (th_heat_structure_t *hs = thermal_hydraulics.heat_structures; hs != NULL; hs = hs->hh.next) {
      function_t *functions[] = {hs->center_function, hs->fuel_surface_function, hs->clad_surface_function};
      for (int i = 0; i < 3; i++) {
        if (strcmp(token, functions[i]->name) == 0) {
          feenox_call(feenox_add_post_field(mesh_write, 1, &functions[i]->name, NULL, field_location_nodes));
          return FEENOX_OK;
        }
      }
    }
    feenox_push_error_message("undefined keyword '%s' for thermal_hydraulics WRITE_RESULTS", token);
    return FEENOX_ERROR;
  }
 
  return FEENOX_OK;
}
