/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for one-dimensional thermal-hydraulics: initialization
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
thermal_hydraulics_t thermal_hydraulics;

int feenox_problem_parse_time_init_thermal_hydraulics(void) {

///kw_pde+PROBLEM+detail  * `thermal_hydraulics` one-dimensional single-phase liquid-water flow in networks of pipes

#ifdef HAVE_PETSC
  // virtual methods
  feenox.pde.parse_bc = feenox_problem_bc_parse_thermal_hydraulics;
  feenox.pde.parse_write_results = feenox_problem_parse_write_post_thermal_hydraulics;
  feenox.pde.init_before_run = feenox_problem_init_runtime_thermal_hydraulics;
  feenox.pde.solve_post = feenox_problem_solve_post_thermal_hydraulics;
  
  // the scalar unknowns live at the nodes
  feenox.mesh.default_field_location = field_location_nodes;
  
///pr_thermal_hydraulics+DIMENSIONS+detail Pipes are always topologically one-dimensional but they can live in a space
///pr_thermal_hydraulics+DIMENSIONS+detail of one, two or three dimensions. The default is one (i.e. pipes along the $x$ axis).
///pr_thermal_hydraulics+DIMENSIONS+detail For networks in the $x$-$y$ plane or in 3D space use `DIMENSIONS 2` or `DIMENSIONS 3` respectively.
  if (feenox.pde.dim_by_default) {
    feenox.pde.dim = 1;
  }
  
  // pressure and temperature at the nodes are the primary nodal unknowns
  // the mass flow at the elements is also an unknown but it is not a nodal dof
  feenox.pde.dofs = 2;
  feenox_check_alloc(feenox.pde.unknown_name = calloc(feenox.pde.dofs, sizeof(char *)));
///re_thermal_hydraulics+p+description The pressure field\ $p(x)$ in Pa. This is a primary unknown of the problem.
  feenox_check_alloc(feenox.pde.unknown_name[0] = strdup("p"));
///re_thermal_hydraulics+T+description The temperature field\ $T(x)$ in K. This is a primary unknown of the problem.
  feenox_check_alloc(feenox.pde.unknown_name[1] = strdup("T"));
  feenox_call(feenox_problem_define_solutions());

///re_thermal_hydraulics+rho+description The density field\ $\rho(x)$ in kg/m^3. This is a secondary unknown of the problem.
  feenox_call(feenox_problem_define_solution_function("rho", &thermal_hydraulics.rho_function, FEENOX_SOLUTION_NOT_GRADIENT));
///re_thermal_hydraulics+h+description The specific enthalpy field\ $h(x)$ in J/kg. This is a secondary unknown of the problem.
  feenox_call(feenox_problem_define_solution_function("h", &thermal_hydraulics.h_function, FEENOX_SOLUTION_NOT_GRADIENT));
///re_thermal_hydraulics+v+description The velocity field\ $v(x)$ in m/s along the direction of the pipe elements, averaged at the nodes.
  feenox_call(feenox_problem_define_solution_function("v", &thermal_hydraulics.v_function, FEENOX_SOLUTION_NOT_GRADIENT));
///re_thermal_hydraulics+mdot+description The mass flow field\ $\dot{m}(x)$ in kg/s along the direction of the pipe elements, averaged at the nodes.
  feenox_call(feenox_problem_define_solution_function("mdot", &thermal_hydraulics.mdot_function, FEENOX_SOLUTION_NOT_GRADIENT));
///pr_thermal_hydraulics+T_0+description Initial fluid temperature as a function of the spatial coordinates.
///pr_thermal_hydraulics+T_0+description For transients, any prescribed initial field skips the t=0 steady solve. Unspecified fluid fields use the boundary-based initial guess.
///pr_thermal_hydraulics+T_0+description A transient with T_0 need not prescribe a temperature or density boundary condition, allowing initially filled circulating loops.
///pr_thermal_hydraulics+p_0+description Initial pressure (Pa), as a function of the spatial coordinates.
///pr_thermal_hydraulics+v_0+description Initial signed pipe velocity (m/s), evaluated at element centers. Mutually exclusive with `mdot_0`.
///pr_thermal_hydraulics+mdot_0+description Initial signed mass flow (kg/s), evaluated at element centers.
///pr_thermal_hydraulics+T_solid_0+description Initial solid temperature (K), with radius as the first argument followed by the spatial coordinates.
///pr_thermal_hydraulics+T_solid_0+description This function initializes every heat structure on its own radial grid.
///pr_thermal_hydraulics+T_solid_0+description If absent with prescribed fluid initial conditions, the solid starts at the local initial fluid temperature.
///pr_thermal_hydraulics+T_solid_0+description Without any initial fields, a coupled steady state initializes the transient. Time integration uses backward Euler.

///va_thermal_hydraulics+g_x+detail The $x$ component of the gravity acceleration vector in m/s^2. Default is zero.
  feenox_check_alloc(thermal_hydraulics.g[0] = feenox_get_or_define_variable_get_ptr("g_x"));
///va_thermal_hydraulics+g_y+detail The $y$ component of the gravity acceleration vector in m/s^2. Default is zero.
  feenox_check_alloc(thermal_hydraulics.g[1] = feenox_get_or_define_variable_get_ptr("g_y"));
///va_thermal_hydraulics+g_z+detail The $z$ component of the gravity acceleration vector in m/s^2. Default is zero.
///va_thermal_hydraulics+g_z+detail Set `g_z = -9.81` for the usual case of having $z$ pointing upwards.
  feenox_check_alloc(thermal_hydraulics.g[2] = feenox_get_or_define_variable_get_ptr("g_z"));
  
  // so far, the only fluid is liquid water
  thermal_hydraulics.fluid.name = "water-if97";
  thermal_hydraulics.fluid.rho = thermal_hydraulics_if97_rho;
  thermal_hydraulics.fluid.h = thermal_hydraulics_if97_h;
  thermal_hydraulics.fluid.cp = thermal_hydraulics_if97_cp;
  thermal_hydraulics.fluid.mu = thermal_hydraulics_iapws_mu;
  thermal_hydraulics.fluid.k = thermal_hydraulics_if97_k;
  thermal_hydraulics.fluid.beta = thermal_hydraulics_if97_beta;
  thermal_hydraulics.fluid.T_from_p_rho = thermal_hydraulics_if97_T_from_p_rho;
  thermal_hydraulics.fluid.psat = thermal_hydraulics_if97_psat;
  thermal_hydraulics.fluid.T_min = 273.15;
  thermal_hydraulics.fluid.T_max = 623.15;
  
  
#endif
  return FEENOX_OK;
}

int feenox_problem_init_runtime_thermal_hydraulics(void) {
  
#ifdef HAVE_PETSC
  
  if (feenox.mpi_size > 1) {
    feenox_push_error_message("thermal_hydraulics cannot be run in parallel (yet)");
    return FEENOX_ERROR;
  }
  
  mesh_t *mesh = feenox.pde.mesh;
  mesh->data_type = data_type_node;
  feenox.pde.spatial_unknowns = mesh->n_nodes;
  feenox.pde.math_type = math_type_nonlinear;
  feenox.pde.solve = feenox_problem_solve_thermal_hydraulics;
  
  feenox.pde.has_stiffness = 0;
  feenox.pde.has_mass = 0;
  feenox.pde.has_rhs = 0;
  feenox.pde.has_jacobian = 0;
  
  // initialize distributions
///pr_thermal_hydraulics+A+description Flow area\ $A$ of the pipe (in m^2).
///pr_thermal_hydraulics+A+description If not given, it is computed from the diameter\ $D$ as $\pi D^2/4$.
  feenox_call(feenox_distribution_init(&thermal_hydraulics.A, "A"));
///pr_thermal_hydraulics+D+description Inner diameter\ $D$ of a circular pipe (in m).
///pr_thermal_hydraulics+D+description It is used to compute the flow area and the hydraulic diameter if they are not explicitly given.
  feenox_call(feenox_distribution_init(&thermal_hydraulics.D, "D"));
///pr_thermal_hydraulics+D_h+description Hydraulic diameter\ $D_h$ of the pipe (in m).
///pr_thermal_hydraulics+D_h+description If not given, it is equal to\ $D$, or to $\sqrt{4A/\pi}$ if\ $D$ is not given either.
  feenox_call(feenox_distribution_init(&thermal_hydraulics.D_h, "D_h"));
///pr_thermal_hydraulics+roughness+description Absolute roughness\ $\varepsilon$ of the pipe wall (in m).
///pr_thermal_hydraulics+roughness+description Default is zero (i.e. smooth pipe).
  feenox_call(feenox_distribution_init(&thermal_hydraulics.roughness, "roughness"));
///pr_thermal_hydraulics+f+description Darcy friction factor\ $f$.
///pr_thermal_hydraulics+f+description If not given, it is computed with Churchill's correlation as a function of the
///pr_thermal_hydraulics+f+description Reynolds number and the relative roughness\ $\varepsilon/D_h$.
  feenox_call(feenox_distribution_init(&thermal_hydraulics.f, "f"));
///pr_thermal_hydraulics+K+description Non-negative local loss coefficient on a MATERIAL attached to a physical point shared by at least two pipes. Default is zero.
///pr_thermal_hydraulics+K+description `K_<pipe_group>` overrides `K` for each port belonging to that physical line group. Coefficients follow the actual flow direction, independently of element orientation.
///pr_thermal_hydraulics+K+description Each port contributes a loss `K_port*mdot*abs(mdot)/(2*rho*A_loss^2)`. These losses add across a two-pipe connection; `K=1` applies one coefficient to each port, not one coefficient to the whole connection.
///pr_thermal_hydraulics+A_ref+description Optional positive reference area in m^2 on a physical-point MATERIAL. If absent, local losses use the respective pipe area.
///pr_thermal_hydraulics+A_ref+description A point material containing `K`, `K_<pipe_group>`, or `A_ref` enables the low-Mach junction model. Use `K=0` for an ideal area transition. Unconfigured shared nodes retain the original common-static-pressure model.
///pr_thermal_hydraulics+A_ref+description Junction pressure is a reference static pressure: the common total pressure is `p+M^2/(2*rho*A_j^2)`, with `M=sum(abs(mdot_port))/2` and `A_j=A_ref`, or the smallest attached pipe area if absent.
///pr_thermal_hydraulics+A_ref+description Pipe-port pressures subtract the port velocity head and add an incoming or subtract an outgoing local loss. Static pressures on either side need not equal the point pressure.
///pr_thermal_hydraulics+A_ref+description The junction energy balance includes signed port kinetic-energy fluxes and backward-Euler kinetic-energy storage in its adjacent half elements. The point uses the existing fluid volumes; no extra junction volume, pump, or compressible entropy/Riemann junction model is added.
///pr_thermal_hydraulics+A_ref+description Point properties are evaluated at the point coordinates before each solve and can depend on time. Do not prescribe p, v, or mdot at an internal junction point. Overlapping configured point groups are rejected.
///pr_thermal_hydraulics+A_ref+description The JUNCTION primary keyword configures this same model without a point MATERIAL. A PUMP adds directed head and signed hydraulic power to a two-port point; see those keywords.
///pr_thermal_hydraulics+q+description Volumetric power\ $q'''$ (in W/m^3) deposited into the fluid.
///pr_thermal_hydraulics+q+description Default is zero.
  feenox_call(feenox_distribution_init(&thermal_hydraulics.q, "q"));
///pr_thermal_hydraulics+P_h+description Heated perimeter (in m). Default is $4A/D_h$.
///pr_thermal_hydraulics+P_h+description For a circular pipe this is $\pi D$. Set zero for an insulated pipe.
///pr_thermal_hydraulics+P_h+description For a heat structure, the default is the outer cladding circumference and an explicit value must be positive.
///pr_thermal_hydraulics+P_h+description The default applies separately to each material where `P_h` is absent; an explicit zero is preserved.
  feenox_call(feenox_distribution_init(&thermal_hydraulics.P_h, "P_h"));
///pr_thermal_hydraulics+T_wall+description Specified pipe wall temperature (in K).
///pr_thermal_hydraulics+T_wall+description Adds $h_{wall} P_h (T_{wall}-T)$ per unit length to the energy equation.
///pr_thermal_hydraulics+T_wall+description By default, the coefficient follows the TRACE single-phase tube model:
///pr_thermal_hydraulics+T_wall+description the maximum of laminar (Nu=4.36), corrected Gnielinski, and natural convection.
///pr_thermal_hydraulics+T_wall+description Water conductivity uses IAPWS-2011 with its industrial critical enhancement.
///pr_thermal_hydraulics+T_wall+description Wall boiling and wall thermal inertia are not modeled.
///pr_thermal_hydraulics+T_wall+description Mutually exclusive with `q_wall` on a given pipe; either can be combined with volumetric `q`.
///pr_thermal_hydraulics+T_wall+description Wall properties are evaluated at element centers before each solve (including time-dependent inputs).
///pr_thermal_hydraulics+T_wall+description Each endpoint control volume receives half the element's wall heat, evaluated at that node's fluid state.
///pr_thermal_hydraulics+T_wall+description Inlet `T` or `rho` specifies the entering stream state; the heated inlet control volume can have a different temperature.
  feenox_call(feenox_distribution_init(&thermal_hydraulics.T_wall, "T_wall"));
///pr_thermal_hydraulics+q_wall+description Specified wall heat flux (in W/m^2), positive into the fluid.
///pr_thermal_hydraulics+q_wall+description Adds $q_{wall}P_h$ per unit length. Negative values cool the fluid.
///pr_thermal_hydraulics+q_wall+description Mutually exclusive with `T_wall` on a given pipe. Default is an adiabatic wall.
  feenox_call(feenox_distribution_init(&thermal_hydraulics.q_wall, "q_wall"));
///pr_thermal_hydraulics+h_wall+description Optional prescribed wall heat transfer coefficient (in W/m^2/K).
///pr_thermal_hydraulics+h_wall+description Requires `T_wall` or a `HEAT_STRUCTURE` on the same pipe. For a heat structure it must be positive.
///pr_thermal_hydraulics+h_wall+description Heat structures currently require a prescribed coefficient rather than automatic TRACE convection.
///pr_thermal_hydraulics+h_wall+description Zero disables prescribed-temperature wall heat exchange; heat structures require a positive value. Negative values are invalid.
  feenox_call(feenox_distribution_init(&thermal_hydraulics.h_wall, "h_wall"));
  
  if (thermal_hydraulics.A.defined == 0 && thermal_hydraulics.D.defined == 0) {
    feenox_push_error_message("either the flow area 'A' or the diameter 'D' have to be given");
    return FEENOX_ERROR;
  }
  
  // count the pipes (i.e. one-dimensional elements)
  thermal_hydraulics.n_nodes = mesh->n_nodes;
  thermal_hydraulics.n_pipes = 0;
  for (size_t i = 0; i < mesh->n_elements; i++) {
    if (mesh->element[i].type->dim == 1) {
      if (mesh->element[i].type->nodes != 2) {
        feenox_push_error_message("thermal_hydraulics only supports first-order line elements, element %ld has %d nodes", mesh->element[i].tag, mesh->element[i].type->nodes);
        return FEENOX_ERROR;
      }
      thermal_hydraulics.n_pipes++;
    }
  }
  if (thermal_hydraulics.n_pipes == 0) {
    feenox_push_error_message("mesh does not have any line elements");
    return FEENOX_ERROR;
  }
  
  feenox_check_alloc(thermal_hydraulics.pipes = calloc(thermal_hydraulics.n_pipes, sizeof(th_pipe_t)));
  feenox_check_alloc(thermal_hydraulics.nodes = calloc(thermal_hydraulics.n_nodes, sizeof(th_node_t)));
  
  // MERGE point groups: every point of the group is represented by the
  // first one, so the pipes that end at any of them share a single node
  for (size_t j = 0; j < thermal_hydraulics.n_nodes; j++) {
    thermal_hydraulics.nodes[j].alias = j;
  }
  for (th_point_t *point = thermal_hydraulics.points; point != NULL; point = point->hh.next) {
    if (!point->merge) continue;
    physical_group_t *group = feenox_get_physical_group_ptr(point->group_name, mesh);
    if (group == NULL || group->dimension != 0 || group->n_elements == 0) {
      feenox_push_error_message("%s MERGE needs a non-empty physical point group, not '%s'", point->is_pump ? "PUMP" : "JUNCTION", point->group_name);
      return FEENOX_ERROR;
    }
    size_t representative = mesh->element[group->element[0]].node[0]->index_mesh;
    for (size_t e = 0; e < group->n_elements; e++) {
      size_t j = mesh->element[group->element[e]].node[0]->index_mesh;
      th_node_t *node = &thermal_hydraulics.nodes[j];
      if (node->alias != j) {
        feenox_push_error_message("node %ld belongs to more than one MERGE point group", mesh->node[j].tag);
        return FEENOX_ERROR;
      }
      node->alias = representative;
    }
  }

  // fill the pipe geometry and count the number of pipes per node
  size_t k = 0;
  for (size_t i = 0; i < mesh->n_elements; i++) {
    element_t *e = &mesh->element[i];
    if (e->type->dim == 1) {
      th_pipe_t *pipe = &thermal_hydraulics.pipes[k++];
      pipe->element = e;
      pipe->a = thermal_hydraulics.nodes[e->node[0]->index_mesh].alias;
      pipe->b = thermal_hydraulics.nodes[e->node[1]->index_mesh].alias;
      if (pipe->a == pipe->b) {
        feenox_push_error_message("element %ld has both ends in the same MERGE point group", e->tag);
        return FEENOX_ERROR;
      }
      for (int d = 0; d < 3; d++) {
        pipe->dx[d] = e->node[1]->x[d] - e->node[0]->x[d];
      }
      pipe->L = gsl_hypot3(pipe->dx[0], pipe->dx[1], pipe->dx[2]);
      if (pipe->L <= 0) {
        feenox_push_error_message("element %ld has zero length", e->tag);
        return FEENOX_ERROR;
      }
      thermal_hydraulics.nodes[pipe->a].n_pipes++;
      thermal_hydraulics.nodes[pipe->b].n_pipes++;
    }
  }
  
  // build the node-to-pipe adjacency
  for (size_t j = 0; j < thermal_hydraulics.n_nodes; j++) {
    th_node_t *node = &thermal_hydraulics.nodes[j];
    if (node->alias != j) {
      // merged into another node, it only mirrors its state
      continue;
    }
    if (node->n_pipes == 0) {
      feenox_push_error_message("node %ld does not belong to any pipe", mesh->node[j].tag);
      return FEENOX_ERROR;
    }
    feenox_check_alloc(node->pipe = calloc(node->n_pipes, sizeof(size_t)));
    feenox_check_alloc(node->sign = calloc(node->n_pipes, sizeof(int)));
    node->n_pipes = 0;
  }
  for (k = 0; k < thermal_hydraulics.n_pipes; k++) {
    th_node_t *node_a = &thermal_hydraulics.nodes[thermal_hydraulics.pipes[k].a];
    node_a->pipe[node_a->n_pipes] = k;
    node_a->sign[node_a->n_pipes++] = -1;
    
    th_node_t *node_b = &thermal_hydraulics.nodes[thermal_hydraulics.pipes[k].b];
    node_b->pipe[node_b->n_pipes] = k;
    node_b->sign[node_b->n_pipes++] = +1;
  }
  
  // caches
  feenox_check_alloc(thermal_hydraulics.rho = calloc(thermal_hydraulics.n_nodes, sizeof(double)));
  feenox_check_alloc(thermal_hydraulics.h = calloc(thermal_hydraulics.n_nodes, sizeof(double)));
  
  // secondary fields
  feenox_call(feenox_problem_fill_aux_solution(thermal_hydraulics.rho_function));
  feenox_call(feenox_problem_fill_aux_solution(thermal_hydraulics.h_function));
  feenox_call(feenox_problem_fill_aux_solution(thermal_hydraulics.v_function));
  feenox_call(feenox_problem_fill_aux_solution(thermal_hydraulics.mdot_function));
  
#endif  
  return FEENOX_OK;
}
