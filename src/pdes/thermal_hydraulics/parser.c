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

static int thermal_hydraulics_parse_point(int is_pump) {
  const char *keyword = is_pump ? "PUMP" : "JUNCTION";
  if (feenox.pde.parse_problem != feenox_problem_parse_problem_thermal_hydraulics) {
    feenox_push_error_message("%s requires PROBLEM thermal_hydraulics first", keyword);
    return FEENOX_ERROR;
  }
  th_point_t *point = NULL;
  feenox_check_alloc(point = calloc(1, sizeof(th_point_t)));
  point->is_pump = is_pump;
  int head_given = 0, direction_given = 0, reverse_given = 0;
  char *token = NULL;
  while ((token = feenox_get_next_token(NULL)) != NULL) {
    if (strcasecmp(token, "POINT") == 0) {
      if (point->group_name != NULL) {
        feenox_push_error_message("duplicate POINT for %s", keyword);
        return FEENOX_ERROR;
      }
      feenox_call(feenox_parser_string(&point->group_name));
    } else if (strcasecmp(token, "MERGE") == 0 && !point->merge) {
      point->merge = 1;
    } else if (is_pump && strcasecmp(token, "HEAD") == 0 && !head_given) {
      head_given = 1;
      feenox_call(feenox_parser_expression(&point->head));
    } else if (is_pump && strcasecmp(token, "DIRECTION") == 0 && !direction_given) {
      direction_given = 1;
      for (int d = 0; d < 3; d++) {
        feenox_call(feenox_parser_expression(&point->direction[d]));
      }
    } else if (is_pump && strcasecmp(token, "K_REVERSE") == 0 && !reverse_given) {
      reverse_given = 1;
      for (int side = 0; side < 2; side++) {
        feenox_call(feenox_parser_expression(&point->reverse_loss[side]));
      }
    } else if (is_pump && strcasecmp(token, "A_REF") == 0 && !point->has_area) {
      point->has_area = 1;
      feenox_call(feenox_parser_expression(&point->area));
    } else if (!is_pump && strchr(token, '=') != NULL) {
      char *equal = strchr(token, '=');
      *equal = '\0';
      if (equal[1] == '\0') {
        feenox_push_error_message("JUNCTION property '%s' needs an expression", token);
        return FEENOX_ERROR;
      }
      if (strcmp(token, "K") != 0 && strcmp(token, "A_ref") != 0 &&
          !(strncmp(token, "K_", 2) == 0 && token[2] != '\0')) {
        feenox_push_error_message("undefined JUNCTION property '%s'", token);
        return FEENOX_ERROR;
      }
      th_point_property_t *property = NULL;
      HASH_FIND_STR(point->properties, token, property);
      if (property != NULL) {
        feenox_push_error_message("duplicate JUNCTION property '%s'", token);
        return FEENOX_ERROR;
      }
      feenox_check_alloc(property = calloc(1, sizeof(th_point_property_t)));
      feenox_check_alloc(property->name = strdup(token));
      feenox_call(feenox_expression_parse(&property->expr, equal+1));
      HASH_ADD_KEYPTR(hh, point->properties, property->name, strlen(property->name), property);
    } else {
      feenox_push_error_message("undefined or duplicate keyword '%s' for %s", token, keyword);
      return FEENOX_ERROR;
    }
  }
  if (point->group_name == NULL || point->group_name[0] == '\0') {
    feenox_push_error_message("%s requires keyword POINT", keyword);
    return FEENOX_ERROR;
  }
  if (is_pump && (!head_given || !direction_given)) {
    feenox_push_error_message("PUMP requires keyword %s", !head_given ? "HEAD" : "DIRECTION");
    return FEENOX_ERROR;
  }
  th_point_t *existing = NULL;
  HASH_FIND_STR(thermal_hydraulics.points, point->group_name, existing);
  if (existing != NULL) {
    feenox_push_error_message("point group '%s' already has a PUMP or JUNCTION declaration", point->group_name);
    return FEENOX_ERROR;
  }
  HASH_ADD_KEYPTR(hh, thermal_hydraulics.points, point->group_name, strlen(point->group_name), point);
  return FEENOX_OK;
}

int feenox_parse_pump(void) {
///kw_thermal_hydraulics+PUMP+desc Attach an ideal prescribed-head pump to a physical point.
///kw_thermal_hydraulics+PUMP+usage PUMP POINT <group> HEAD <expr> DIRECTION <dx> <dy> <dz> [ K_REVERSE <suction_K> <discharge_K> ] [ A_REF <expr> ] [ MERGE ]
///kw_thermal_hydraulics+PUMP+detail Secondary keywords can appear in any order. POINT, HEAD and DIRECTION are mandatory. The group must contain exactly one point with exactly two incident first-order pipe elements.
///kw_thermal_hydraulics+PUMP+detail With MERGE, the group may contain several points (typically the suction end and the discharge end of two pipes that do not touch). They are joined into one control volume, as described for JUNCTION.
///kw_thermal_hydraulics+PUMP+detail Outward element tangents must have opposite nonzero projections on DIRECTION: negative identifies suction, positive discharge. Direction expressions are evaluated at each solve and need not form a unit vector. Mesh element orientation and pipe materials do not define pump direction.
///kw_thermal_hydraulics+PUMP+detail HEAD is finite and nonnegative, in m, and may vary with time. Total-pressure addition is rho*9.80665*HEAD, independently of the problem gravity vector. Hydraulic power is signed suction mass flow times 9.80665*HEAD; reverse flow does not reverse the imposed head.
///kw_thermal_hydraulics+PUMP+detail K_REVERSE gives nonnegative suction and discharge port losses active only during reverse flow, defaulting to zero. A_REF must be positive; if omitted each port uses its own flow area for losses. The pump includes reversible area-change and port kinetic-energy terms like a junction.
///kw_thermal_hydraulics+PUMP+detail The nodal pressure is the midpoint reference, not either port pressure. No extra storage volume, efficiency, shaft dynamics or check valve is modeled. Do not combine PUMP with another point component, point material, or p/v/mdot boundary conditions at that node.
  return thermal_hydraulics_parse_point(1);
}

int feenox_parse_junction(void) {
///kw_thermal_hydraulics+JUNCTION+desc Configure junction losses at shared mesh nodes without a point MATERIAL.
///kw_thermal_hydraulics+JUNCTION+usage JUNCTION POINT <group> [ K=<expr> ] [ K_<pipe_group>=<expr> ... ] [ A_ref=<expr> ] [ MERGE ]
///kw_thermal_hydraulics+JUNCTION+detail Uses the same low-Mach point-junction model as the legacy MATERIAL syntax, which remains supported. Omitted K is zero, enabling ideal area transitions. Each point needs at least two incident pipe elements. Multiple points in one group are supported. Do not combine with a material or another point-component declaration on the same node.
///kw_thermal_hydraulics+JUNCTION+detail Without MERGE, each point of the group is an independent junction. With MERGE, all the points of the group are joined into a single control volume regardless of their coordinates, so pipe ends connect by topology as in RELAP-7 (e.g. a plenum fed by pipes that end at different locations). Then the merged points together need at least two incident pipe elements, the junction has no elevation (no gravity acts between its points) and the fields at every merged point show the junction state. Boundary conditions cannot be set on merged points.
  return thermal_hydraulics_parse_point(0);
}

int feenox_parse_heat_structure(void) {

///kw_thermal_hydraulics+HEAT_STRUCTURE+desc Couple a cylindrical fuel-gap-clad heat structure to a physical pipe group.
///kw_thermal_hydraulics+HEAT_STRUCTURE+usage HEAT_STRUCTURE PIPE <pipe_group> FUEL <fuel_material> [ GAP <gap_material> ] [ CLAD <clad_material> ] [ AXIAL_CONDUCTION | NO_AXIAL_CONDUCTION ]
///kw_thermal_hydraulics+HEAT_STRUCTURE+detail `AXIAL_CONDUCTION` (default) includes radial and axial conduction in this structure. `NO_AXIAL_CONDUCTION` uses independent radial columns. These flags take no parameters and are mutually exclusive.
///kw_thermal_hydraulics+HEAT_STRUCTURE+detail Secondary keywords can appear in any order. `PIPE` and `FUEL` are mandatory; omitted `GAP` and `CLAD` materials each default to `FUEL`. Layer geometry determines whether a gap or clad is present.
///kw_thermal_hydraulics+HEAT_STRUCTURE+detail Declare after `PROBLEM thermal_hydraulics`, before using solid output functions or writing results.
///kw_thermal_hydraulics+HEAT_STRUCTURE+detail Any number of heat structures can be declared, keyed by distinct pipe-group names. Named solid materials need constant `k`, `rho_s`, and `cp`.
///kw_thermal_hydraulics+HEAT_STRUCTURE+detail Each structure defines `T_fuel_center_<pipe_group>`, `T_fuel_surface_<pipe_group>`, `T_clad_surface_<pipe_group>`, `hs_heat_<pipe_group>`, and `hs_energy_<pipe_group>`.
///kw_thermal_hydraulics+HEAT_STRUCTURE+detail `WRITE_RESULTS all` includes every structure's temperature fields; individual suffixed temperature names can also be selected.
///kw_thermal_hydraulics+HEAT_STRUCTURE+detail Unsuffixed fuel/cladding temperature functions refer to the first declared fuel-rod structure for compatibility; `hs_heat` and `hs_energy` sum all structures. At shared fluid nodes each solid retains its own temperature; distinct structures are not conductively connected.
///kw_thermal_hydraulics+HEAT_STRUCTURE+detail Solid density uses `rho_s` to avoid colliding with the fluid solution function `rho`.
///kw_thermal_hydraulics+HEAT_STRUCTURE+detail The channel material defines `fuel_radius`, `gap_width`, `clad_width`, `fuel_cells`, `gap_cells`, `clad_cells`, and total `hs_power` in W.
///kw_thermal_hydraulics+HEAT_STRUCTURE+detail Fuel radius and cell count are mandatory and positive. Omitted gap/clad widths default to zero, removing that layer. Omitted cell counts default to one for positive widths or zero for absent layers. Explicit zero width needs zero cells. Geometry is evaluated once and must not depend on space or time.
///kw_thermal_hydraulics+HEAT_STRUCTURE+detail The channel's optional positive rod_count defaults to one and scales radial/axial conduction, solid volume/storage and default heated perimeter consistently. hs_power is total bundle power, not power per rod. A positive noninteger rod_count can represent an equivalent bundle.
///kw_thermal_hydraulics+HEAT_STRUCTURE+detail Solid properties must be constant expressions without variable or user-function dependencies, in W/(m K), kg/m^3, and J/(kg K), respectively.
///kw_thermal_hydraulics+HEAT_STRUCTURE+detail Power is uniform in the fuel and along the channel; the centerline and axial ends are insulated.
///kw_thermal_hydraulics+HEAT_STRUCTURE+detail `hs_power` can vary with time but must not depend on space. A positive prescribed `h_wall` in W/(m^2 K) couples cladding to coolant; automatic convection is not yet supported for heat structures.
///kw_thermal_hydraulics+HEAT_STRUCTURE+detail The heated perimeter defaults to the outer cladding circumference; an explicit `P_h` must be positive. Do not combine a heat structure with `T_wall` or `q_wall`.
///kw_thermal_hydraulics+HEAT_STRUCTURE+detail The coolant remains single-phase IF97 region-1 water; boiling and two-phase heat transfer are not modeled.
///kw_thermal_hydraulics+HEAT_STRUCTURE+usage HEAT_STRUCTURE NAME <name> TYPE <PLATE|TUBE> MATERIAL <solid> THICKNESS <expr> CELLS <n> PRIMARY <group> SECONDARY <group> H_PRIMARY <expr> H_SECONDARY <expr> AXIS <dx> <dy> <dz> [ DEPTH <expr> | INNER_RADIUS <expr> ] [ PERIMETER_PRIMARY <expr> ] [ PERIMETER_SECONDARY <expr> ] [ AXIAL_CONDUCTION | NO_AXIAL_CONDUCTION ]
///kw_thermal_hydraulics+HEAT_STRUCTURE+detail Named two-sided structures couple hydraulically disconnected straight pipe groups at matching projected stations along AXIS, independent of flow or mesh orientation. Groups must span the same interval with matching axial elements; branched, shared-node and nonmatching meshes are rejected. AXIS need not be normalized.
///kw_thermal_hydraulics+HEAT_STRUCTURE+detail PLATE requires positive DEPTH in m (wall width); TUBE requires positive INNER_RADIUS in m. PRIMARY couples to the first plate surface or inner tube surface; SECONDARY couples to the opposite or outer surface. THICKNESS and CELLS define the transverse wall grid. MATERIAL needs constant positive k, rho_s, cp.
///kw_thermal_hydraulics+HEAT_STRUCTURE+detail The wall has insulated axial ends and no internal source. Axial conduction is enabled by default. Geometry is constant. Positive H_PRIMARY/H_SECONDARY in W/(m^2 K) may depend on time, not space. Interface perimeters in m default to DEPTH for plates and the inner/outer circumferences for tubes; explicit overrides do not change wall conduction or storage geometry.
///kw_thermal_hydraulics+HEAT_STRUCTURE+detail Outputs T_wall_primary_<name>, T_wall_middle_<name>, T_wall_secondary_<name> are defined on both coupled pipe groups; middle uses the lower central grid node for odd CELLS. hs_heat_primary_<name> and hs_heat_secondary_<name> are signed heat rates into each fluid. hs_heat_<name> is their sum, hs_energy_<name> is wall energy. T_solid_0 takes transverse distance (plate) or radius (tube), then the primary pipe coordinates.
  if (feenox.pde.parse_problem != feenox_problem_parse_problem_thermal_hydraulics) {
    feenox_push_error_message("HEAT_STRUCTURE requires PROBLEM thermal_hydraulics first");
    return FEENOX_ERROR;
  }
  const char *keywords[] = {"PIPE", "FUEL", "GAP", "CLAD", "NAME", "TYPE", "MATERIAL", "THICKNESS", "CELLS", "DEPTH", "INNER_RADIUS", "PRIMARY", "SECONDARY", "H_PRIMARY", "H_SECONDARY", "PERIMETER_PRIMARY", "PERIMETER_SECONDARY"};
  char *values[17] = {NULL};
  expr_t axis[3] = {0};
  int axis_given = 0;
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
    if (strcasecmp(token, "AXIS") == 0) {
      if (axis_given) {
        feenox_push_error_message("duplicate keyword AXIS for HEAT_STRUCTURE");
        return FEENOX_ERROR;
      }
      axis_given = 1;
      for (int d = 0; d < 3; d++) {
        feenox_call(feenox_parser_expression(&axis[d]));
      }
      continue;
    }
    
    // TODO: traditional parsing with split documentation
    int i = 0;
    while (i < 17 && strcasecmp(token, keywords[i]) != 0) i++;
    if (i == 17) {
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
  if (values[4] != NULL) {
    const int mandatory[] = {5,6,7,8,11,12,13,14};
    for (size_t i = 0; i < sizeof(mandatory)/sizeof(mandatory[0]); i++) {
      if (values[mandatory[i]] == NULL) {
        feenox_push_error_message("named HEAT_STRUCTURE requires keyword '%s'", keywords[mandatory[i]]);
        return FEENOX_ERROR;
      }
    }
    if (!axis_given) {
      feenox_push_error_message("named HEAT_STRUCTURE requires keyword AXIS");
      return FEENOX_ERROR;
    }
    int plate = strcasecmp(values[5], "PLATE") == 0;
    if (!plate && strcasecmp(values[5], "TUBE") != 0) {
      feenox_push_error_message("HEAT_STRUCTURE TYPE must be PLATE or TUBE");
      return FEENOX_ERROR;
    }
    if (values[plate ? 9 : 10] == NULL) {
      feenox_push_error_message("HEAT_STRUCTURE TYPE %s requires keyword %s", values[5], plate ? "DEPTH" : "INNER_RADIUS");
      return FEENOX_ERROR;
    }
    if (values[plate ? 10 : 9] != NULL || values[0] != NULL || values[1] != NULL || values[2] != NULL || values[3] != NULL) {
      feenox_push_error_message("incompatible geometry or fuel-rod keywords for named HEAT_STRUCTURE");
      return FEENOX_ERROR;
    }
    th_heat_structure_t *hs = NULL;
    HASH_FIND_STR(thermal_hydraulics.heat_structures, values[4], hs);
    if (hs != NULL) {
      feenox_push_error_message("HEAT_STRUCTURE name '%s' already defined", values[4]);
      return FEENOX_ERROR;
    }
    feenox_check_alloc(hs = calloc(1, sizeof(th_heat_structure_t)));
    hs->group_name = values[4];
    hs->exchanger = 1;
    hs->plate = plate;
    hs->axial_conduction = axial_conduction;
    hs->material_name[0] = values[6];
    hs->primary_name = values[11];
    hs->secondary_name = values[12];
    feenox_call(feenox_expression_parse(&hs->thickness_expr, values[7]));
    feenox_call(feenox_expression_parse(&hs->cells_expr, values[8]));
    feenox_call(feenox_expression_parse(plate ? &hs->depth_expr : &hs->inner_radius_expr, values[plate ? 9 : 10]));
    for (int d = 0; d < 3; d++) hs->axis_expr[d] = axis[d];
    for (int side = 0; side < 2; side++) {
      feenox_call(feenox_expression_parse(&hs->h_expr[side], values[13+side]));
      if (values[15+side] != NULL) {
        hs->has_perimeter[side] = 1;
        feenox_call(feenox_expression_parse(&hs->perimeter_expr[side], values[15+side]));
      }
    }
    const char *names[] = {"T_wall_primary", "T_wall_middle", "T_wall_secondary", "hs_heat", "hs_energy", "hs_heat_primary", "hs_heat_secondary"};
    function_t **functions[] = {&hs->center_function, &hs->fuel_surface_function, &hs->clad_surface_function};
    var_t **variables[] = {&hs->heat, &hs->energy, &hs->side_heat[0], &hs->side_heat[1]};
    for (int i = 0; i < 7; i++) {
      char *name = NULL;
      feenox_check_minusone(asprintf(&name, "%s_%s", names[i], hs->group_name));
      if (i < 3) {
        feenox_call(feenox_problem_define_solution_function(name, functions[i], FEENOX_SOLUTION_NOT_GRADIENT));
      } else {
        feenox_check_alloc(*variables[i-3] = feenox_get_or_define_variable_get_ptr(name));
      }
      feenox_free(name);
    }
    feenox_check_alloc(thermal_hydraulics.hs_heat = feenox_get_or_define_variable_get_ptr("hs_heat"));
    feenox_check_alloc(thermal_hydraulics.hs_energy = feenox_get_or_define_variable_get_ptr("hs_energy"));
    HASH_ADD_STR(thermal_hydraulics.heat_structures, group_name, hs);
    for (int i = 0; i < 17; i++) {
      if (i != 4 && i != 6 && i != 11 && i != 12) feenox_free(values[i]);
    }
    return FEENOX_OK;
  }
  for (int i = 5; i < 17; i++) {
    if (values[i] != NULL) {
      feenox_push_error_message("HEAT_STRUCTURE keyword '%s' requires NAME", keywords[i]);
      return FEENOX_ERROR;
    }
  }
  if (axis_given) {
    feenox_push_error_message("HEAT_STRUCTURE keyword AXIS requires NAME");
    return FEENOX_ERROR;
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
  
  // we need one for each, not only one for the first one
  if (thermal_hydraulics.fuel_center_function == NULL) {
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
      if (thermal_hydraulics.fuel_center_function != NULL) {
        feenox_call(feenox_problem_parse_write_post_thermal_hydraulics(mesh_write, "T_fuel_center"));
        feenox_call(feenox_problem_parse_write_post_thermal_hydraulics(mesh_write, "T_fuel_surface"));
        feenox_call(feenox_problem_parse_write_post_thermal_hydraulics(mesh_write, "T_clad_surface"));
      }
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
  } else if (thermal_hydraulics.fuel_center_function != NULL && strcmp(token, "T_fuel_center") == 0) {
    feenox_call(feenox_add_post_field(mesh_write, 1, &thermal_hydraulics.fuel_center_function->name, NULL, field_location_nodes));
  } else if (thermal_hydraulics.fuel_surface_function != NULL && strcmp(token, "T_fuel_surface") == 0) {
    feenox_call(feenox_add_post_field(mesh_write, 1, &thermal_hydraulics.fuel_surface_function->name, NULL, field_location_nodes));
  } else if (thermal_hydraulics.clad_surface_function != NULL && strcmp(token, "T_clad_surface") == 0) {
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
