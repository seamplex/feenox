/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for one-dimensional thermal-hydraulics: solver
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

/*
 Staggered finite-volume scheme (a la RELAP5):
  - each mesh node is a control volume with pressure p and temperature T
  - each line element is a flow path with mass flow mdot (positive from node a to node b)
 
 Global vector layout:
   [ p_0 T_0 ... p_{N-1} T_{N-1} mdot_0 ... mdot_{K-1} solid blocks ... ]
   Each heat structure has its own block of N radial temperature columns,
   starting at hs->offset, with hs->nr fuel-gap-clad temperatures per column.
 
 Residuals:
   row 2j:   mass balance at node j (or p_j - p_bc if node j has a pressure BC)
   row 2j+1: energy balance at node j (non-conservative enthalpy form)
   row 2N+k: momentum balance along pipe k
*/

#define TH_P(j)    (2*(j))
#define TH_T(j)    (2*(j)+1)
#define TH_MDOT(k) (2*thermal_hydraulics.n_nodes + (k))
#define TH_SOLID(hs,j,i) ((hs)->offset + (j)*(hs)->nr + (i))

#ifdef HAVE_PETSC
static int thermal_hydraulics_update_properties(void);
static int thermal_hydraulics_update_junctions(void);
static double thermal_hydraulics_port_pressure(th_pipe_t *pipe, int side, const PetscScalar *x);
static double thermal_hydraulics_junction_kinetic_energy(size_t j, const PetscScalar *x, double rho);
static int thermal_hydraulics_initial_guess(void);
static int thermal_hydraulics_create_jacobian(void);
static int thermal_hydraulics_setup_snes(void);
static int thermal_hydraulics_store_old(void);
static PetscErrorCode thermal_hydraulics_residual(SNES snes, Vec X, Vec R, void *ctx);
static int thermal_hydraulics_property_defined(distribution_t *property, int group);
static double thermal_hydraulics_wall_coefficient(th_pipe_t *pipe, double p, double T, double mdot);
static int thermal_hydraulics_init_heat_structure(th_heat_structure_t *hs);
static int thermal_hydraulics_init_exchanger(th_heat_structure_t *hs);
static int thermal_hydraulics_update_heat_structure(th_heat_structure_t *hs);
static int thermal_hydraulics_set_initial_conditions(void);
static expr_t *thermal_hydraulics_material_expression(material_t *material, const char *name);
static double thermal_hydraulics_radial_weight(th_heat_structure_t *hs, size_t i, int side);
static double thermal_hydraulics_interface_heat(th_heat_structure_t *hs, size_t j, int side, const PetscScalar *x);
static int thermal_hydraulics_solid_residual(th_heat_structure_t *hs, const PetscScalar *x, PetscScalar *r);
#endif

int feenox_problem_solve_thermal_hydraulics(void) {

#ifdef HAVE_PETSC
  
  for (th_heat_structure_t *hs = thermal_hydraulics.heat_structures; hs != NULL; hs = hs->hh.next) {
    if (hs->nr == 0) {
      hs->offset = 2*thermal_hydraulics.n_nodes + thermal_hydraulics.n_pipes + thermal_hydraulics.n_solid_unknowns;
      feenox_call(thermal_hydraulics_init_heat_structure(hs));
      thermal_hydraulics.n_solid_unknowns += thermal_hydraulics.n_nodes*hs->nr;
    }
  }
  size_t size = 2*thermal_hydraulics.n_nodes + thermal_hydraulics.n_pipes +
                thermal_hydraulics.n_solid_unknowns;
  if (thermal_hydraulics.x == NULL) {
    petsc_call(VecCreateSeq(PETSC_COMM_SELF, size, &thermal_hydraulics.x));
    petsc_call(PetscObjectSetName((PetscObject)thermal_hydraulics.x, "x_th"));
    petsc_call(VecDuplicate(thermal_hydraulics.x, &thermal_hydraulics.r));
  }

  // first evaluate the properties and BCs (they might depend on time)
  thermal_hydraulics.transient = (feenox_special_var_value(in_static) == 0 && feenox_special_var_value(end_time) > 0);
  thermal_hydraulics.dt = feenox_special_var_value(dt);
  feenox_call(thermal_hydraulics_update_properties());
  for (th_heat_structure_t *hs = thermal_hydraulics.heat_structures; hs != NULL; hs = hs->hh.next) {
    feenox_call(thermal_hydraulics_update_heat_structure(hs));
  }
  int initial = feenox_special_var_value(end_time) > 0 &&
                (feenox_get_function_ptr("p_0") != NULL || feenox_get_function_ptr("T_0") != NULL ||
                 feenox_get_function_ptr("mdot_0") != NULL || feenox_get_function_ptr("v_0") != NULL ||
                 feenox_get_function_ptr("T_solid_0") != NULL);
  
  if (thermal_hydraulics.snes == NULL) {
    feenox_call(thermal_hydraulics_initial_guess());
    if (initial) {
      feenox_call(thermal_hydraulics_set_initial_conditions());
    }
    feenox_call(thermal_hydraulics_create_jacobian());
    feenox_call(thermal_hydraulics_setup_snes());
  }
  
  if (thermal_hydraulics.transient && thermal_hydraulics.x_old == NULL) {
    feenox_push_error_message("internal error, transient step without previous state");
    return FEENOX_ERROR;
  }
  
  if (thermal_hydraulics.transient && (!isfinite(thermal_hydraulics.dt) || thermal_hydraulics.dt <= 0)) {
    feenox_push_error_message("thermal_hydraulics transient steps need a positive finite dt");
    return FEENOX_ERROR;
  }
  if (feenox.pde.do_not_solve == 0 && !(initial && feenox_special_var_value(in_static))) {
    petsc_call(SNESSolve(thermal_hydraulics.snes, NULL, thermal_hydraulics.x));
    SNESConvergedReason reason;
    petsc_call(SNESGetConvergedReason(thermal_hydraulics.snes, &reason));
    if (reason < 0) {
      feenox_push_error_message("PETSc's non-linear solver did not converge with reason '%s' (%d)", SNESConvergedReasons[reason], reason);
      return FEENOX_ERROR;
    }
  }
  
  // keep the converged state as the initial condition for the next time step
  feenox_call(thermal_hydraulics_store_old());
  
  // copy p and T into the generic nodal solution vector phi so
  // feenox_problem_phi_to_solution() can fill the solution functions
  const PetscScalar *x = NULL;
  petsc_call(VecGetArrayRead(thermal_hydraulics.x, &x));
  double total_heat = 0, total_energy = 0;
  for (th_heat_structure_t *hs = thermal_hydraulics.heat_structures; hs != NULL; hs = hs->hh.next) {
    double heat = 0, energy = 0;
    double side_heat[2] = {0,0};
    for (size_t k = 0; !hs->exchanger && k < thermal_hydraulics.n_pipes; k++) {
      th_pipe_t *pipe = &thermal_hydraulics.pipes[k];
      if (pipe->element->physical_group != hs->group) continue;
      heat += pipe->h_wall*pipe->P_h*pipe->L/2*
              (x[TH_SOLID(hs,pipe->a,hs->nr-1)]-x[TH_T(pipe->a)]+
               x[TH_SOLID(hs,pipe->b,hs->nr-1)]-x[TH_T(pipe->b)]);
    }
    for (size_t j = 0; j < thermal_hydraulics.n_nodes; j++) {
      if (hs->exchanger && hs->node_length[j] > 0) {
        for (int side = 0; side < 2; side++) {
          side_heat[side] += thermal_hydraulics_interface_heat(hs, j, side, x);
        }
      }
      for (size_t i = 0; i+1 < hs->nr; i++) {
        int l = hs->layer[i];
        for (int side = 0; side < 2; side++) {
          energy += hs->rho[l]*hs->cp[l]*hs->node_length[j]*
                    thermal_hydraulics_radial_weight(hs, i, side)*x[TH_SOLID(hs,j,i+side)];
        }
        if (hs->exchanger) {
          heat = side_heat[0]+side_heat[1];
          for (int side = 0; side < 2; side++) feenox_var_value(hs->side_heat[side]) = side_heat[side];
        }
      }
    }
    feenox_var_value(hs->heat) = heat;
    feenox_var_value(hs->energy) = energy;
    total_heat += heat;
    total_energy += energy;
  }
  if (thermal_hydraulics.heat_structures != NULL) {
    feenox_var_value(thermal_hydraulics.hs_heat) = total_heat;
    feenox_var_value(thermal_hydraulics.hs_energy) = total_energy;
  }
  for (size_t j = 0; j < thermal_hydraulics.n_nodes; j++) {
    node_t *node = &feenox.pde.mesh->node[j];
    petsc_call(VecSetValue(feenox.pde.phi, node->index_dof[0], x[TH_P(j)], INSERT_VALUES));
    petsc_call(VecSetValue(feenox.pde.phi, node->index_dof[1], x[TH_T(j)], INSERT_VALUES));
  }
  petsc_call(VecRestoreArrayRead(thermal_hydraulics.x, &x));
  petsc_call(VecAssemblyBegin(feenox.pde.phi));
  petsc_call(VecAssemblyEnd(feenox.pde.phi));
  
#endif
  return FEENOX_OK;
}

int feenox_problem_solve_post_thermal_hydraulics(void) {
  
#ifdef HAVE_PETSC
  
  const PetscScalar *x = NULL;
  petsc_call(VecGetArrayRead(thermal_hydraulics.x, &x));
  
  int warned = 0;
  for (size_t j = 0; j < thermal_hydraulics.n_nodes; j++) {
    th_node_t *node = &thermal_hydraulics.nodes[j];
    double p = x[TH_P(j)];
    double T = x[TH_T(j)];
    double rho = thermal_hydraulics.fluid.rho(p, T);
    
    // the velocity and mass flow at the nodes are the average of the adjacent pipes
    double mdot = 0;
    double v = 0;
    for (size_t i = 0; i < node->n_pipes; i++) {
      th_pipe_t *pipe = &thermal_hydraulics.pipes[node->pipe[i]];
      double mdot_pipe = x[TH_MDOT(node->pipe[i])];
      mdot += mdot_pipe;
      v += mdot_pipe / (rho * pipe->A);
    }
    mdot /= node->n_pipes;
    v /= node->n_pipes;
    
    feenox_vector_set(thermal_hydraulics.rho_function->vector_value, j, rho);
    feenox_vector_set(thermal_hydraulics.h_function->vector_value, j, thermal_hydraulics.fluid.h(p, T));
    feenox_vector_set(thermal_hydraulics.v_function->vector_value, j, v);
    feenox_vector_set(thermal_hydraulics.mdot_function->vector_value, j, mdot);
    int first_rod = 1;
    for (th_heat_structure_t *hs = thermal_hydraulics.heat_structures; hs != NULL; hs = hs->hh.next) {
      size_t column = j;
      if (hs->exchanger && hs->node_length[j] == 0 && hs->partner[j] < thermal_hydraulics.n_nodes) column = hs->partner[j];
      int active = hs->node_length[column] > 0;
      double center = active ? x[TH_SOLID(hs,column,0)] : NAN;
      double fuel_surface = active ? x[TH_SOLID(hs,column,hs->fuel_surface)] : NAN;
      double clad_surface = active ? x[TH_SOLID(hs,column,hs->nr-1)] : NAN;
      feenox_vector_set(hs->center_function->vector_value, j, center);
      feenox_vector_set(hs->fuel_surface_function->vector_value, j, fuel_surface);
      feenox_vector_set(hs->clad_surface_function->vector_value, j, clad_surface);
      if (!hs->exchanger && first_rod) {
        feenox_vector_set(thermal_hydraulics.fuel_center_function->vector_value, j, center);
        feenox_vector_set(thermal_hydraulics.fuel_surface_function->vector_value, j, fuel_surface);
        feenox_vector_set(thermal_hydraulics.clad_surface_function->vector_value, j, clad_surface);
        first_rod = 0;
      }
    }
    
    if (warned == 0) {
      if (T < thermal_hydraulics.fluid.T_min || T > thermal_hydraulics.fluid.T_max) {
        fprintf(stderr, "warning: temperature %g K at node %ld is out of the range of validity of %s\n", T, feenox.pde.mesh->node[j].tag, thermal_hydraulics.fluid.name);
        warned = 1;
      } else if (thermal_hydraulics.fluid.psat != NULL && p < thermal_hydraulics.fluid.psat(T)) {
        fprintf(stderr, "warning: pressure %g Pa at node %ld is below saturation for T = %g K (two-phase flow is not supported yet)\n", p, feenox.pde.mesh->node[j].tag, T);
        warned = 1;
      }
    }
  }
  
  petsc_call(VecRestoreArrayRead(thermal_hydraulics.x, &x));
  
#endif
  return FEENOX_OK;
}

#ifdef HAVE_PETSC

static expr_t *thermal_hydraulics_material_expression(material_t *material, const char *name) {
  property_data_t *data = NULL;
  HASH_FIND_STR(material->property_datums, name, data);
  if (data == NULL) {
    feenox_push_error_message("material '%s' needs heat-structure property '%s'", material->name, name);
    return NULL;
  }
  return &data->expr;
}

static int thermal_hydraulics_init_solid_outputs(th_heat_structure_t *hs) {
  feenox_call(feenox_problem_fill_aux_solution(hs->center_function));
  feenox_call(feenox_problem_fill_aux_solution(hs->fuel_surface_function));
  feenox_call(feenox_problem_fill_aux_solution(hs->clad_surface_function));
  if (!hs->exchanger) {
    int first_rod = 1;
    for (th_heat_structure_t *other = thermal_hydraulics.heat_structures; other != hs; other = other->hh.next) {
      if (!other->exchanger) first_rod = 0;
    }
    if (first_rod) {
      feenox_call(feenox_problem_fill_aux_solution(thermal_hydraulics.fuel_center_function));
      feenox_call(feenox_problem_fill_aux_solution(thermal_hydraulics.fuel_surface_function));
      feenox_call(feenox_problem_fill_aux_solution(thermal_hydraulics.clad_surface_function));
    }
  }
  return FEENOX_OK;
}

static double thermal_hydraulics_axial_coordinate(th_heat_structure_t *hs, size_t j) {
  double s = 0;
  for (int d = 0; d < 3; d++) s += hs->axis[d]*feenox.pde.mesh->node[j].x[d];
  return s;
}

static int thermal_hydraulics_init_exchanger(th_heat_structure_t *hs) {
  hs->group = feenox_get_physical_group_ptr(hs->primary_name, feenox.pde.mesh);
  hs->secondary = feenox_get_physical_group_ptr(hs->secondary_name, feenox.pde.mesh);
  if (hs->group == NULL || hs->secondary == NULL || hs->group == hs->secondary ||
      hs->group->dimension != 1 || hs->secondary->dimension != 1) {
    feenox_push_error_message("HEAT_STRUCTURE '%s' needs distinct PRIMARY and SECONDARY physical line groups", hs->group_name);
    return FEENOX_ERROR;
  }
  expr_t *geometry[] = {&hs->thickness_expr, &hs->cells_expr, hs->plate ? &hs->depth_expr : &hs->inner_radius_expr,
                        &hs->axis_expr[0], &hs->axis_expr[1], &hs->axis_expr[2]};
  double value[6];
  for (int i = 0; i < 6; i++) {
    if (feenox_expression_depends_on_space(geometry[i]) || feenox_depends_on_time(geometry[i]->variables)) {
      feenox_push_error_message("HEAT_STRUCTURE '%s' geometry must not depend on space or time", hs->group_name);
      return FEENOX_ERROR;
    }
    value[i] = feenox_expression_eval(geometry[i]);
    if (!isfinite(value[i]) || (i < 3 && value[i] <= 0)) {
      feenox_push_error_message("HEAT_STRUCTURE '%s' needs positive finite thickness, cells, depth or inner radius, and finite AXIS", hs->group_name);
      return FEENOX_ERROR;
    }
  }
  if (value[1] != floor(value[1]) || value[1] > 10000) {
    feenox_push_error_message("HEAT_STRUCTURE '%s' CELLS must be an integer in [1,10000]", hs->group_name);
    return FEENOX_ERROR;
  }
  double norm = gsl_hypot3(value[3], value[4], value[5]);
  if (!isfinite(norm) || norm <= 0) {
    feenox_push_error_message("HEAT_STRUCTURE '%s' needs a nonzero finite AXIS", hs->group_name);
    return FEENOX_ERROR;
  }
  for (int d = 0; d < 3; d++) hs->axis[d] = value[3+d]/norm;
  hs->depth = hs->plate ? value[2] : 0;
  hs->nr = (size_t)value[1]+1;
  hs->fuel_surface = (hs->nr-1)/2;
  feenox_check_alloc(hs->radius = calloc(hs->nr, sizeof(double)));
  feenox_check_alloc(hs->layer = calloc(hs->nr-1, sizeof(int)));
  feenox_check_alloc(hs->node_length = calloc(thermal_hydraulics.n_nodes, sizeof(double)));
  feenox_check_alloc(hs->partner = malloc(thermal_hydraulics.n_nodes*sizeof(size_t)));
  for (size_t i = 0; i < hs->nr; i++) {
    hs->radius[i] = (hs->plate ? 0 : value[2])+value[0]*i/(hs->nr-1);
  }
  for (int side = 0; side < 2; side++) {
    if (feenox_expression_depends_on_space(&hs->h_expr[side])) {
      feenox_push_error_message("HEAT_STRUCTURE '%s' H_PRIMARY/H_SECONDARY must not depend on space", hs->group_name);
      return FEENOX_ERROR;
    }
    if (hs->has_perimeter[side]) {
      expr_t *expr = &hs->perimeter_expr[side];
      if (feenox_expression_depends_on_space(expr) || feenox_depends_on_time(expr->variables)) {
        feenox_push_error_message("HEAT_STRUCTURE '%s' interface perimeters must be constant in space and time", hs->group_name);
        return FEENOX_ERROR;
      }
      hs->perimeter[side] = feenox_expression_eval(expr);
    } else {
      hs->perimeter[side] = hs->plate ? hs->depth : 2*M_PI*hs->radius[side == 0 ? 0 : hs->nr-1];
    }
    if (!isfinite(hs->perimeter[side]) || hs->perimeter[side] <= 0) {
      feenox_push_error_message("HEAT_STRUCTURE '%s' needs positive finite interface perimeters", hs->group_name);
      return FEENOX_ERROR;
    }
  }
  material_t *material = feenox_get_material_ptr(hs->material_name[0]);
  if (material == NULL) {
    feenox_push_error_message("unknown HEAT_STRUCTURE material '%s'", hs->material_name[0]);
    return FEENOX_ERROR;
  }
  const char *properties[] = {"k", "rho_s", "cp"};
  for (int m = 0; m < 3; m++) {
    hs->properties[0][m] = thermal_hydraulics_material_expression(material, properties[m]);
    if (hs->properties[0][m] == NULL) return FEENOX_ERROR;
    if (hs->properties[0][m]->variables != NULL || hs->properties[0][m]->functions != NULL) {
      feenox_push_error_message("HEAT_STRUCTURE solid properties must be constant");
      return FEENOX_ERROR;
    }
  }
  size_t N = thermal_hydraulics.n_nodes;
  unsigned int *degree[2] = {NULL, NULL};
  for (int side = 0; side < 2; side++) {
    feenox_check_alloc(degree[side] = calloc(N, sizeof(unsigned int)));
  }
  for (size_t j = 0; j < N; j++) hs->partner[j] = N;
  double tolerance = 1e-8;
  for (size_t k = 0; k < thermal_hydraulics.n_pipes; k++) {
    th_pipe_t *pipe = &thermal_hydraulics.pipes[k];
    for (int side = 0; side < 2; side++) {
      if (pipe->element->physical_group != (side == 0 ? hs->group : hs->secondary)) continue;
      double projected = fabs(thermal_hydraulics_axial_coordinate(hs, pipe->b)-thermal_hydraulics_axial_coordinate(hs, pipe->a));
      if (fabs(projected-pipe->L) > tolerance*pipe->L || projected <= 0) {
        feenox_push_error_message("HEAT_STRUCTURE '%s' pipes must be straight and parallel to AXIS", hs->group_name);
        return FEENOX_ERROR;
      }
      degree[side][pipe->a]++;
      degree[side][pipe->b]++;
      if (side == 0) {
        hs->node_length[pipe->a] += pipe->L/2;
        hs->node_length[pipe->b] += pipe->L/2;
        hs->length += pipe->L;
      }
    }
  }
  if (hs->length <= 0) {
    feenox_push_error_message("HEAT_STRUCTURE '%s' PRIMARY has no pipe elements", hs->group_name);
    return FEENOX_ERROR;
  }
  tolerance *= hs->length;
  for (int side = 0; side < 2; side++) {
    int ends = 0;
    for (size_t j = 0; j < N; j++) {
      ends += degree[side][j] == 1;
      if (degree[side][j] > 2 || (degree[0][j] && degree[1][j])) {
        feenox_push_error_message("HEAT_STRUCTURE '%s' needs unbranched, hydraulically separate pipe groups", hs->group_name);
        return FEENOX_ERROR;
      }
    }
    if (ends != 2) {
      feenox_push_error_message("HEAT_STRUCTURE '%s' needs two nonempty unbranched pipe chains", hs->group_name);
      return FEENOX_ERROR;
    }
  }
  for (size_t j = 0; j < N; j++) {
    if (degree[0][j] == 0) continue;
    int matches = 0;
    for (size_t m = 0; m < N; m++) {
      if (degree[1][m] == 0) continue;
      if (fabs(thermal_hydraulics_axial_coordinate(hs,j)-thermal_hydraulics_axial_coordinate(hs,m)) <= tolerance) {
        if (hs->partner[m] != N) {
          feenox_push_error_message("HEAT_STRUCTURE '%s' has ambiguous axial stations", hs->group_name);
          return FEENOX_ERROR;
        }
        hs->partner[j] = m;
        hs->partner[m] = j;
        matches++;
      }
    }
    if (matches != 1) {
      feenox_push_error_message("HEAT_STRUCTURE '%s' requires matching unique axial stations", hs->group_name);
      return FEENOX_ERROR;
    }
  }
  for (size_t j = 0; j < N; j++) {
    if (degree[1][j] && hs->partner[j] == N) {
      feenox_push_error_message("HEAT_STRUCTURE '%s' requires matching axial meshes", hs->group_name);
      return FEENOX_ERROR;
    }
  }
  for (size_t k = 0; k < thermal_hydraulics.n_pipes; k++) {
    th_pipe_t *pipe = &thermal_hydraulics.pipes[k];
    if (pipe->element->physical_group != hs->group) continue;
    int matches = 0;
    for (size_t m = 0; m < thermal_hydraulics.n_pipes; m++) {
      th_pipe_t *other = &thermal_hydraulics.pipes[m];
      if (other->element->physical_group == hs->secondary &&
          ((other->a == hs->partner[pipe->a] && other->b == hs->partner[pipe->b]) ||
           (other->b == hs->partner[pipe->a] && other->a == hs->partner[pipe->b]))) matches++;
    }
    if (matches != 1) {
      feenox_push_error_message("HEAT_STRUCTURE '%s' requires matching axial elements", hs->group_name);
      return FEENOX_ERROR;
    }
  }
  feenox_free(degree[0]);
  feenox_free(degree[1]);
  feenox_call(thermal_hydraulics_init_solid_outputs(hs));
  return FEENOX_OK;
}

static int thermal_hydraulics_init_heat_structure(th_heat_structure_t *hs) {
  if (hs->exchanger) return thermal_hydraulics_init_exchanger(hs);
  HASH_FIND_STR(feenox.pde.mesh->physical_groups, hs->group_name, hs->group);
  if (hs->group == NULL || hs->group->dimension != 1 || hs->group->material == NULL) {
    feenox_push_error_message("HEAT_STRUCTURE needs a physical line group '%s' with a channel material", hs->group_name);
    return FEENOX_ERROR;
  }
  material_t *channel = hs->group->material;
  const char *geometry[] = {"fuel_radius", "gap_width", "clad_width", "fuel_cells", "gap_cells", "clad_cells"};
  double value[6];
  for (int i = 0; i < 6; i++) {
    property_data_t *data = NULL;
    HASH_FIND_STR(channel->property_datums, geometry[i], data);
    if (data == NULL && i != 0 && i != 3) {
      value[i] = i < 3 ? 0 : (value[i-3] > 0 ? 1 : 0);
      continue;
    }
    expr_t *expr = thermal_hydraulics_material_expression(channel, geometry[i]);
    if (expr == NULL) return FEENOX_ERROR;
    if (feenox_expression_depends_on_space(expr) || feenox_depends_on_time(expr->variables)) {
      feenox_push_error_message("heat-structure geometry '%s' must not depend on space or time", geometry[i]);
      return FEENOX_ERROR;
    }
    value[i] = feenox_expression_eval(expr);
    if (!isfinite(value[i]) || value[i] < 0 || ((i == 0 || i == 3) && value[i] == 0) ||
        (i >= 3 && (value[i] != floor(value[i]) || value[i] > 10000 || ((value[i-3] > 0) != (value[i] > 0))))) {
      feenox_push_error_message("heat-structure '%s' must be positive for active layers; absent layers need zero width and zero cells; cell counts must be integers <= 10000", geometry[i]);
      return FEENOX_ERROR;
    }
  }
  property_data_t *count = NULL;
  HASH_FIND_STR(channel->property_datums, "rod_count", count);
  hs->rod_count = 1;
  if (count != NULL) {
    if (feenox_expression_depends_on_space(&count->expr) || feenox_depends_on_time(count->expr.variables)) {
      feenox_push_error_message("rod_count must not depend on space or time");
      return FEENOX_ERROR;
    }
    hs->rod_count = feenox_expression_eval(&count->expr);
    if (!isfinite(hs->rod_count) || hs->rod_count <= 0) {
      feenox_push_error_message("rod_count must be positive and finite");
      return FEENOX_ERROR;
    }
  }
  hs->nr = 1+(size_t)value[3]+(size_t)value[4]+(size_t)value[5];
  hs->fuel_surface = (size_t)value[3];
  feenox_check_alloc(hs->radius = calloc(hs->nr, sizeof(double)));
  feenox_check_alloc(hs->layer = calloc(hs->nr-1, sizeof(int)));
  feenox_check_alloc(hs->node_length = calloc(thermal_hydraulics.n_nodes, sizeof(double)));
  size_t n = 0;
  double radius = 0;
  const char *properties[] = {"k", "rho_s", "cp"};
  for (int l = 0; l < 3; l++) {
    if (value[l+3] == 0) continue;
    material_t *material = feenox_get_material_ptr(hs->material_name[l]);
    if (material == NULL) {
      feenox_push_error_message("unknown heat-structure material '%s'", hs->material_name[l]);
      return FEENOX_ERROR;
    }
    for (int m = 0; m < 3; m++) {
      if ((hs->properties[l][m] = thermal_hydraulics_material_expression(material, properties[m])) == NULL) {
        return FEENOX_ERROR;
      }
      if (hs->properties[l][m]->variables != NULL || hs->properties[l][m]->functions != NULL) {
        feenox_push_error_message("heat-structure material '%s' property '%s' must be constant", material->name, properties[m]);
        return FEENOX_ERROR;
      }
    }
    size_t cells = (size_t)value[l+3];
    for (size_t i = 0; i < cells; i++) {
      hs->layer[n] = l;
      hs->radius[++n] = radius+value[l]*(i+1)/cells;
    }
    radius += value[l];
  }
  for (size_t k = 0; k < thermal_hydraulics.n_pipes; k++) {
    th_pipe_t *pipe = &thermal_hydraulics.pipes[k];
    if (pipe->element->physical_group == hs->group) {
      pipe->heat_structure = hs;
      hs->length += pipe->L;
      hs->node_length[pipe->a] += pipe->L/2;
      hs->node_length[pipe->b] += pipe->L/2;
    }
  }
  if (hs->length <= 0) {
    feenox_push_error_message("HEAT_STRUCTURE group '%s' has no pipe elements", hs->group_name);
    return FEENOX_ERROR;
  }
  if ((hs->power_expr = thermal_hydraulics_material_expression(channel, "hs_power")) == NULL) {
    return FEENOX_ERROR;
  }
  if (feenox_expression_depends_on_space(hs->power_expr)) {
    feenox_push_error_message("hs_power is total channel power and must not depend on space");
    return FEENOX_ERROR;
  }
  feenox_call(thermal_hydraulics_init_solid_outputs(hs));
  return FEENOX_OK;
}

static int thermal_hydraulics_update_heat_structure(th_heat_structure_t *hs) {
  hs->power = hs->exchanger ? 0 : feenox_expression_eval(hs->power_expr);
  if (!isfinite(hs->power) || hs->power < 0) {
    feenox_push_error_message("hs_power for heat structure '%s' must be finite and non-negative", hs->group_name);
    return FEENOX_ERROR;
  }
  for (int l = 0; l < (hs->exchanger ? 1 : 3); l++) {
    if (hs->properties[l][0] == NULL) continue;
    hs->k[l] = feenox_expression_eval(hs->properties[l][0]);
    hs->rho[l] = feenox_expression_eval(hs->properties[l][1]);
    hs->cp[l] = feenox_expression_eval(hs->properties[l][2]);
    if (!isfinite(hs->k[l]) || hs->k[l] <= 0 || !isfinite(hs->rho[l]) || hs->rho[l] <= 0 ||
        !isfinite(hs->cp[l]) || hs->cp[l] <= 0) {
      feenox_push_error_message("heat-structure material '%s' needs positive finite k, rho_s, and cp", hs->material_name[l]);
      return FEENOX_ERROR;
    }
    if (hs->exchanger) {
      for (int side = 0; side < 2; side++) {
        hs->coefficient[side] = feenox_expression_eval(&hs->h_expr[side]);
        if (!isfinite(hs->coefficient[side]) || hs->coefficient[side] <= 0) {
          feenox_push_error_message("HEAT_STRUCTURE '%s' needs positive finite H_PRIMARY/H_SECONDARY", hs->group_name);
          return FEENOX_ERROR;
        }
      }
    }
  }
  return FEENOX_OK;
}

// Axisymmetric linear-element lumped area: integral of 2*pi*r*N_side dr.
static double thermal_hydraulics_radial_weight(th_heat_structure_t *hs, size_t i, int side) {
  double a = hs->radius[i];
  double dr = hs->radius[i+1]-a;
  if (hs->exchanger && hs->plate) return hs->depth*dr/2;
  return (hs->exchanger ? 1 : hs->rod_count)*2*M_PI*dr*(a/2+dr*(side == 0 ? 1.0/6 : 1.0/3));
}

static double thermal_hydraulics_interface_heat(th_heat_structure_t *hs, size_t j, int side, const PetscScalar *x) {
  size_t fluid_node = side == 0 ? j : hs->partner[j];
  size_t surface = side == 0 ? 0 : hs->nr-1;
  return hs->coefficient[side]*hs->perimeter[side]*hs->node_length[j]*
         (x[TH_SOLID(hs,j,surface)]-x[TH_T(fluid_node)]);
}

static int thermal_hydraulics_set_initial_conditions(void) {
  const char *names[] = {"p_0", "T_0", "mdot_0", "v_0", "T_solid_0"};
  function_t *f[5];
  for (int i = 0; i < 5; i++) {
    f[i] = feenox_get_function_ptr(names[i]);
    int dim = feenox.pde.dim+(i == 4);
    if (f[i] != NULL && f[i]->n_arguments != dim) {
      feenox_push_error_message("initial condition '%s' needs %d arguments", names[i], dim);
      return FEENOX_ERROR;
    }
  }
  if (f[2] != NULL && f[3] != NULL) {
    feenox_push_error_message("give either mdot_0 or v_0, not both");
    return FEENOX_ERROR;
  }
  if (f[4] != NULL && thermal_hydraulics.heat_structures == NULL) {
    feenox_push_error_message("T_solid_0 requires a HEAT_STRUCTURE");
    return FEENOX_ERROR;
  }
  PetscScalar *x = NULL;
  petsc_call(VecGetArray(thermal_hydraulics.x, &x));
  for (size_t j = 0; j < thermal_hydraulics.n_nodes; j++) {
    double *xx = feenox.pde.mesh->node[j].x;
    if (f[0] != NULL) x[TH_P(j)] = feenox_function_eval(f[0], xx);
    if (f[1] != NULL) x[TH_T(j)] = feenox_function_eval(f[1], xx);
    if (!isfinite(x[TH_P(j)]) || x[TH_P(j)] <= 0 || !isfinite(x[TH_T(j)]) ||
        x[TH_T(j)] < thermal_hydraulics.fluid.T_min || x[TH_T(j)] > thermal_hydraulics.fluid.T_max) {
      petsc_call(VecRestoreArray(thermal_hydraulics.x, &x));
      feenox_push_error_message("invalid initial fluid pressure or temperature at node %ld", feenox.pde.mesh->node[j].tag);
      return FEENOX_ERROR;
    }
    for (th_heat_structure_t *hs = thermal_hydraulics.heat_structures; hs != NULL; hs = hs->hh.next) {
      for (size_t i = 0; i < hs->nr; i++) {
        double args[4] = {hs->radius[i], xx[0], xx[1], xx[2]};
        x[TH_SOLID(hs,j,i)] = f[4] != NULL ? feenox_function_eval(f[4], args) : x[TH_T(j)];
        if (!isfinite(x[TH_SOLID(hs,j,i)]) || x[TH_SOLID(hs,j,i)] <= 0) {
          petsc_call(VecRestoreArray(thermal_hydraulics.x, &x));
          feenox_push_error_message("invalid initial solid temperature in '%s' at node %ld", hs->group_name, feenox.pde.mesh->node[j].tag);
          return FEENOX_ERROR;
        }
      }
    }
  }
  for (size_t k = 0; k < thermal_hydraulics.n_pipes; k++) {
    th_pipe_t *pipe = &thermal_hydraulics.pipes[k];
    double xx[3];
    for (int d = 0; d < 3; d++) xx[d] = 0.5*(pipe->element->node[0]->x[d]+pipe->element->node[1]->x[d]);
    if (f[2] != NULL) x[TH_MDOT(k)] = feenox_function_eval(f[2], xx);
    if (f[3] != NULL) {
      double p = 0.5*(x[TH_P(pipe->a)]+x[TH_P(pipe->b)]);
      double T = 0.5*(x[TH_T(pipe->a)]+x[TH_T(pipe->b)]);
      x[TH_MDOT(k)] = thermal_hydraulics.fluid.rho(p, T)*pipe->A*feenox_function_eval(f[3], xx);
    }
    if (!isfinite(x[TH_MDOT(k)])) {
      petsc_call(VecRestoreArray(thermal_hydraulics.x, &x));
      feenox_push_error_message("invalid initial pipe flow at element %ld", pipe->element->tag);
      return FEENOX_ERROR;
    }
  }
  petsc_call(VecRestoreArray(thermal_hydraulics.x, &x));
  return FEENOX_OK;
}

static int thermal_hydraulics_property_defined(distribution_t *property, int group) {
  return property->defined && (property->full || (group >= 0 && property->defined_per_group[group]));
}

static expr_t *thermal_hydraulics_point_expression(material_t *material, th_point_t *point, const char *name) {
  if (point != NULL) {
    th_point_property_t *property = NULL;
    HASH_FIND_STR(point->properties, name, property);
    return property != NULL ? &property->expr : NULL;
  }
  property_data_t *data = NULL;
  HASH_FIND_STR(material->property_datums, name, data);
  return data != NULL ? &data->expr : NULL;
}

static int thermal_hydraulics_port_loss(physical_group_t *group, size_t j, const char *name, expr_t *expr) {
  if (strncmp(name, "K_", 2) != 0) return FEENOX_OK;
  double value = feenox_expression_eval(expr);
  if (!isfinite(value) || value < 0) {
    feenox_push_error_message("junction '%s' needs finite %s >= 0", group->name, name);
    return FEENOX_ERROR;
  }
  th_node_t *node = &thermal_hydraulics.nodes[j];
  int matched = 0;
  for (size_t i = 0; i < node->n_pipes; i++) {
    th_pipe_t *pipe = &thermal_hydraulics.pipes[node->pipe[i]];
    physical_group_t *pipe_group = pipe->element->physical_group;
    if (pipe_group != NULL && strcmp(name+2, pipe_group->name) == 0) {
      pipe->K[pipe->a == j ? 0 : 1] = value;
      matched = 1;
    }
  }
  if (!matched) {
    feenox_push_error_message("junction '%s' property '%s' does not match an attached pipe group", group->name, name);
    return FEENOX_ERROR;
  }
  return FEENOX_OK;
}

static int thermal_hydraulics_update_pump(th_point_t *pump, size_t j) {
  th_node_t *node = &thermal_hydraulics.nodes[j];
  if (node->n_pipes != 2) {
    feenox_push_error_message("PUMP '%s' needs exactly two incident pipe elements", pump->group_name);
    return FEENOX_ERROR;
  }
  double direction[3];
  for (int d = 0; d < 3; d++) {
    direction[d] = feenox_expression_eval(&pump->direction[d]);
  }
  double norm = gsl_hypot3(direction[0], direction[1], direction[2]);
  if (!isfinite(norm) || norm <= 0) {
    feenox_push_error_message("PUMP '%s' needs a finite nonzero DIRECTION", pump->group_name);
    return FEENOX_ERROR;
  }
  int suction_found = 0, discharge_found = 0;
  for (size_t i = 0; i < 2; i++) {
    th_pipe_t *pipe = &thermal_hydraulics.pipes[node->pipe[i]];
    double projection = 0;
    for (int d = 0; d < 3; d++) {
      projection += (pipe->a == j ? 1 : -1)*(pipe->dx[d]/pipe->L)*(direction[d]/norm);
    }
    if (projection < -1e-10) {
      pump->suction = node->pipe[i];
      suction_found++;
    } else if (projection > 1e-10) {
      pump->discharge = node->pipe[i];
      discharge_found++;
    }
  }
  if (suction_found != 1 || discharge_found != 1) {
    feenox_push_error_message("PUMP '%s' DIRECTION must have opposite nonzero projections on the two outward pipe tangents", pump->group_name);
    return FEENOX_ERROR;
  }
  double head = feenox_expression_eval(&pump->head);
  pump->A_ref = pump->has_area ? feenox_expression_eval(&pump->area) : 0;
  for (int side = 0; side < 2; side++) {
    pump->K_reverse[side] = feenox_expression_eval(&pump->reverse_loss[side]);
  }
  if (!isfinite(head) || head < 0 ||
      !isfinite(pump->K_reverse[0]) || pump->K_reverse[0] < 0 ||
      !isfinite(pump->K_reverse[1]) || pump->K_reverse[1] < 0 ||
      (pump->has_area && (!isfinite(pump->A_ref) || pump->A_ref <= 0))) {
    feenox_push_error_message("PUMP '%s' needs finite HEAD >= 0, K_REVERSE >= 0 and, if specified, A_REF > 0", pump->group_name);
    return FEENOX_ERROR;
  }
  pump->specific_work = 9.80665*head;
  node->pump = pump;
  return FEENOX_OK;
}

static int thermal_hydraulics_update_junctions(void) {
  mesh_t *mesh = feenox.pde.mesh;
  for (size_t j = 0; j < thermal_hydraulics.n_nodes; j++) {
    thermal_hydraulics.nodes[j].junction_group = NULL;
    thermal_hydraulics.nodes[j].pump = NULL;
  }
  for (size_t k = 0; k < thermal_hydraulics.n_pipes; k++) {
    for (int side = 0; side < 2; side++) {
      thermal_hydraulics.pipes[k].K[side] = 0;
      thermal_hydraulics.pipes[k].loss_area[side] = thermal_hydraulics.pipes[k].A;
    }
  }
  for (th_point_t *point = thermal_hydraulics.points; point != NULL; point = point->hh.next) {
    if (feenox_get_physical_group_ptr(point->group_name, mesh) == NULL) {
      feenox_push_error_message("undefined physical point group '%s' for %s", point->group_name, point->is_pump ? "PUMP" : "JUNCTION");
      return FEENOX_ERROR;
    }
  }
  for (physical_group_t *group = mesh->physical_groups; group != NULL; group = group->hh.next) {
    material_t *material = group->material;
    th_point_t *point = NULL;
    HASH_FIND_STR(thermal_hydraulics.points, group->name, point);
    if (material == NULL && point == NULL) continue;
    if (material != NULL && point != NULL) {
      feenox_push_error_message("point group '%s' has both a MATERIAL and a PUMP/JUNCTION declaration", group->name);
      return FEENOX_ERROR;
    }
    int configured = point != NULL;
    for (property_data_t *data = material != NULL ? material->property_datums : NULL; data != NULL; data = data->hh.next) {
      const char *name = data->property->name;
      configured |= strcmp(name, "K") == 0 || strcmp(name, "A_ref") == 0 || strncmp(name, "K_", 2) == 0;
    }
    if (!configured) continue;
    if (group->dimension != 0 || group->n_elements == 0) {
      feenox_push_error_message("point component needs a non-empty physical point group, not '%s'", group->name);
      return FEENOX_ERROR;
    }
    if (point != NULL && point->is_pump && group->n_elements != 1) {
      feenox_push_error_message("PUMP '%s' needs exactly one physical point", group->name);
      return FEENOX_ERROR;
    }
    for (size_t e = 0; e < group->n_elements; e++) {
      element_t *element = &mesh->element[group->element[e]];
      size_t j = element->node[0]->index_mesh;
      th_node_t *node = &thermal_hydraulics.nodes[j];
      if (node->n_pipes < 2) {
        feenox_push_error_message("junction point '%s' at node %ld needs at least two attached pipes", group->name, mesh->node[j].tag);
        return FEENOX_ERROR;
      }
      if (node->junction_group != NULL) {
        if (node->junction_group == group) continue;
        feenox_push_error_message("node %ld has overlapping junction point groups '%s' and '%s'", mesh->node[j].tag, node->junction_group->name, group->name);
        return FEENOX_ERROR;
      }
      node->junction_group = group;
      for (int d = 0; d < 3; d++) {
        feenox_var_value(feenox.mesh.vars.arr_x[d]) = mesh->node[j].x[d];
      }
      if (point != NULL && point->is_pump) {
        feenox_call(thermal_hydraulics_update_pump(point, j));
      }
      expr_t *expr = thermal_hydraulics_point_expression(material, point, "K");
      double K = expr != NULL ? feenox_expression_eval(expr) : 0;
      expr = thermal_hydraulics_point_expression(material, point, "A_ref");
      int has_area = expr != NULL || (node->pump != NULL && node->pump->has_area);
      double A_ref = expr != NULL ? feenox_expression_eval(expr) : node->pump != NULL ? node->pump->A_ref : 0;
      if (!isfinite(K) || K < 0 || (has_area && (!isfinite(A_ref) || A_ref <= 0))) {
        feenox_push_error_message("junction '%s' needs finite K >= 0 and, if specified, finite A_ref > 0", group->name);
        return FEENOX_ERROR;
      }
      node->junction_area = thermal_hydraulics.pipes[node->pipe[0]].A;
      for (size_t i = 0; i < node->n_pipes; i++) {
        th_pipe_t *pipe = &thermal_hydraulics.pipes[node->pipe[i]];
        int side = pipe->a == j ? 0 : 1;
        pipe->K[side] = K;
        pipe->loss_area[side] = has_area ? A_ref : pipe->A;
        node->junction_area = GSL_MIN(node->junction_area, pipe->A);
      }
      if (has_area) node->junction_area = A_ref;
      if (point != NULL) {
        for (th_point_property_t *property = point->properties; property != NULL; property = property->hh.next) {
          feenox_call(thermal_hydraulics_port_loss(group, j, property->name, &property->expr));
        }
      } else {
        for (property_data_t *data = material->property_datums; data != NULL; data = data->hh.next) {
          feenox_call(thermal_hydraulics_port_loss(group, j, data->property->name, &data->expr));
        }
      }
    }
  }
  return FEENOX_OK;
}

// Low-Mach port pressure: common reference head, reversible area change,
// and a dissipative loss whose direction follows the actual flow.
// Unlike RELAP-7 Theory Manual rev. 3, section 9.6, this does not solve
// a compressible entropy-based junction Riemann problem.
static double thermal_hydraulics_port_pressure(th_pipe_t *pipe, int side, const PetscScalar *x) {
  size_t j = side == 0 ? pipe->a : pipe->b;
  th_node_t *node = &thermal_hydraulics.nodes[j];
  if (node->junction_group == NULL) return x[TH_P(j)];
  double throughflow = 0;
  double mdot = x[TH_MDOT(pipe-thermal_hydraulics.pipes)];
  for (size_t i = 0; i < node->n_pipes; i++) {
    throughflow += 0.5*fabs(x[TH_MDOT(node->pipe[i])]);
  }
  double rho = thermal_hydraulics.rho[j];
  double reference_head = gsl_pow_2(throughflow/node->junction_area)/(2*rho);
  double port_head = gsl_pow_2(mdot/pipe->A)/(2*rho);
  double K = pipe->K[side];
  double pump_head = 0;
  if (node->pump != NULL) {
    th_point_t *pump = node->pump;
    int suction = (size_t)(pipe-thermal_hydraulics.pipes) == pump->suction;
    th_pipe_t *in = &thermal_hydraulics.pipes[pump->suction];
    double pump_flow = (in->b == j ? 1 : -1)*x[TH_MDOT(pump->suction)];
    if (pump_flow < 0) K += pump->K_reverse[suction ? 0 : 1];
    pump_head = (suction ? -0.5 : 0.5)*rho*pump->specific_work;
  }
  double loss = K*mdot*fabs(mdot)/(2*rho*gsl_pow_2(pipe->loss_area[side]));
  return x[TH_P(j)]+reference_head-port_head+pump_head+(side == 0 ? -loss : loss);
}

static double thermal_hydraulics_junction_kinetic_energy(size_t j, const PetscScalar *x, double rho) {
  th_node_t *node = &thermal_hydraulics.nodes[j];
  if (node->junction_group == NULL) return 0;
  double energy = 0;
  for (size_t i = 0; i < node->n_pipes; i++) {
    size_t k = node->pipe[i];
    th_pipe_t *pipe = &thermal_hydraulics.pipes[k];
    energy += 0.5*pipe->L*pipe->A*gsl_pow_2(x[TH_MDOT(k)]/pipe->A)/(2*rho);
  }
  return energy;
}

// RELAP-7 Theory Manual, INL/EXT-14-31366 rev. 3, eqs. (739)-(750).
// https://www.osti.gov/servlets/purl/1498781
static double thermal_hydraulics_wall_coefficient(th_pipe_t *pipe, double p, double T, double mdot) {
  if (pipe->has_h_wall) {
    return pipe->h_wall;
  }
  th_fluid_t *fluid = &thermal_hydraulics.fluid;
  if (!isfinite(p) || p <= 0 || !isfinite(T) || T < fluid->T_min || T > fluid->T_max) {
    return NAN;
  }
  double rho = fluid->rho(p, T);
  double mu = fluid->mu(rho, T);
  double k = fluid->k(p, T);
  double Pr = fluid->cp(p, T)*mu/k;
  double Re = GSL_MAX(1000, fabs(mdot)*pipe->D_h/(pipe->A*mu));
  double fw = 1/gsl_pow_2(1.58*log(Re)-3.28);
  double Nu = (fw/2)*(Re-1000)*Pr/(1+12.7*sqrt(fw/2)*(pow(Pr, 2.0/3)-1));
  double rho_wall = fluid->rho(p, pipe->T_wall);
  double Pr_wall = fluid->cp(p, pipe->T_wall)*fluid->mu(rho_wall, pipe->T_wall)/fluid->k(p, pipe->T_wall);
  if (!isfinite(Pr) || Pr <= 0 || !isfinite(Pr_wall) || Pr_wall <= 0 || !isfinite(k) || k <= 0) {
    return NAN;
  }
  double ratio = GSL_MIN(20, GSL_MAX(0.05, Pr/Pr_wall));
  Nu *= pow(ratio, 0.11);

  double g = gsl_hypot3(feenox_var_value(thermal_hydraulics.g[0]),
                       feenox_var_value(thermal_hydraulics.g[1]),
                       feenox_var_value(thermal_hydraulics.g[2]));
  double rho_film = fluid->rho(p, 0.5*(T+pipe->T_wall));
  double Gr = g*fabs(fluid->beta(p, T)*(pipe->T_wall-T))*gsl_pow_3(pipe->D_h)*gsl_pow_2(rho_film/mu);
  double Ra = Gr*Pr;
  double Nu_nc = GSL_MAX(0.59*pow(Ra, 0.25), 0.13*cbrt(Ra));
  return GSL_MAX(4.36, GSL_MAX(Nu, Nu_nc))*k/pipe->D_h;
}

// evaluate the pipe properties and the boundary conditions at the nodes
static int thermal_hydraulics_update_properties(void) {
  
  for (size_t k = 0; k < thermal_hydraulics.n_pipes; k++) {
    th_pipe_t *pipe = &thermal_hydraulics.pipes[k];
    element_t *e = pipe->element;
    material_t *material = feenox_fem_get_material(e);
    int group = -1;
    int i = 0;
    physical_group_t *physical_group = NULL;
    for (physical_group = feenox.pde.mesh->physical_groups; physical_group != NULL; physical_group = physical_group->hh.next) {
      if (physical_group == e->physical_group) {
        group = i;
        break;
      }
      i++;
    }
    
    double x[3] = {
      0.5*(e->node[0]->x[0] + e->node[1]->x[0]),
      0.5*(e->node[0]->x[1] + e->node[1]->x[1]),
      0.5*(e->node[0]->x[2] + e->node[1]->x[2])
    };

    // diameter
    double D = (thermal_hydraulics.D.defined) ? thermal_hydraulics.D.eval(&thermal_hydraulics.D, x, material) : 0;
    // area
    pipe->A = (thermal_hydraulics.A.defined) ? thermal_hydraulics.A.eval(&thermal_hydraulics.A, x, material) : 0;
    if (pipe->A <= 0 && D > 0) {
      pipe->A = M_PI*D*D/4.0;
    }
    if (pipe->A <= 0) {
      feenox_push_error_message("element %ld has a non-positive flow area (give either 'A' or 'D')", e->tag);
      return FEENOX_ERROR;
    }
    
    // hydraulic diameter
    pipe->D_h = (thermal_hydraulics.D_h.defined) ? thermal_hydraulics.D_h.eval(&thermal_hydraulics.D_h, x, material) : 0;
    if (pipe->D_h <= 0) {
      pipe->D_h = (D > 0) ? D : sqrt(4*pipe->A/M_PI);
    }
    
    pipe->roughness = (thermal_hydraulics.roughness.defined) ? thermal_hydraulics.roughness.eval(&thermal_hydraulics.roughness, x, material) : 0;
    pipe->f = (thermal_hydraulics.f.defined) ? thermal_hydraulics.f.eval(&thermal_hydraulics.f, x, material) : 0;
    pipe->f_given = (pipe->f > 0);
    pipe->q = (thermal_hydraulics.q.defined) ? thermal_hydraulics.q.eval(&thermal_hydraulics.q, x, material) : 0;
    pipe->P_h = thermal_hydraulics_property_defined(&thermal_hydraulics.P_h, group) ?
                thermal_hydraulics.P_h.eval(&thermal_hydraulics.P_h, x, material) : 4*pipe->A/pipe->D_h;
    th_heat_structure_t *hs = pipe->heat_structure;
    if (hs != NULL && !thermal_hydraulics_property_defined(&thermal_hydraulics.P_h, group)) {
      pipe->P_h = hs->rod_count*2*M_PI*hs->radius[hs->nr-1];
    }
    pipe->has_T_wall = thermal_hydraulics_property_defined(&thermal_hydraulics.T_wall, group);
    pipe->has_q_wall = thermal_hydraulics_property_defined(&thermal_hydraulics.q_wall, group);
    pipe->has_h_wall = thermal_hydraulics_property_defined(&thermal_hydraulics.h_wall, group);
    
    // TODO: can we check for this at parse time?
    if (pipe->has_T_wall && pipe->has_q_wall) {
      feenox_push_error_message("element %ld has both 'T_wall' and 'q_wall'; specify only one wall heating model", e->tag);
      return FEENOX_ERROR;
    }
    if (hs != NULL && (pipe->has_T_wall || pipe->has_q_wall)) {
      feenox_push_error_message("HEAT_STRUCTURE pipe cannot also have T_wall or q_wall");
      return FEENOX_ERROR;
    }
    if (pipe->has_h_wall && !pipe->has_T_wall && hs == NULL) {
      feenox_push_error_message("element %ld has 'h_wall' without 'T_wall'", e->tag);
      return FEENOX_ERROR;
    }
    pipe->T_wall = pipe->has_T_wall ? thermal_hydraulics.T_wall.eval(&thermal_hydraulics.T_wall, x, material) : 0;
    pipe->q_wall = pipe->has_q_wall ? thermal_hydraulics.q_wall.eval(&thermal_hydraulics.q_wall, x, material) : 0;
    pipe->h_wall = pipe->has_h_wall ? thermal_hydraulics.h_wall.eval(&thermal_hydraulics.h_wall, x, material) : 0;
    if (hs != NULL && (!pipe->has_h_wall || pipe->h_wall <= 0 || pipe->P_h <= 0)) {
      feenox_push_error_message("HEAT_STRUCTURE pipe needs positive h_wall and P_h");
      return FEENOX_ERROR;
    }
    if (!isfinite(pipe->P_h) || pipe->P_h < 0 ||
        !isfinite(pipe->q_wall) || !isfinite(pipe->h_wall) || pipe->h_wall < 0) {
      feenox_push_error_message("element %ld needs finite wall properties with P_h >= 0 and h_wall >= 0", e->tag);
      return FEENOX_ERROR;
    }
    if (pipe->has_T_wall && (!isfinite(pipe->T_wall) ||
        pipe->T_wall < thermal_hydraulics.fluid.T_min || pipe->T_wall > thermal_hydraulics.fluid.T_max)) {
      feenox_push_error_message("element %ld has T_wall outside the liquid-water range [%g, %g] K",
                                e->tag, thermal_hydraulics.fluid.T_min, thermal_hydraulics.fluid.T_max);
      return FEENOX_ERROR;
    }
  }
  
  feenox_call(thermal_hydraulics_update_junctions());

  // node volumes, heat and reference areas
  for (size_t j = 0; j < thermal_hydraulics.n_nodes; j++) {
    th_node_t *node = &thermal_hydraulics.nodes[j];
    node->V = 0;
    node->Q = 0;
    for (size_t i = 0; i < node->n_pipes; i++) {
      th_pipe_t *pipe = &thermal_hydraulics.pipes[node->pipe[i]];
      node->V += 0.5 * pipe->A * pipe->L;
      node->Q += 0.5 * pipe->q * pipe->A * pipe->L;
    }
    node->A = thermal_hydraulics.pipes[node->pipe[0]].A;
    
    node->has_p = node->has_T = node->has_rho = node->has_v = node->has_mdot = 0;
    node->has_p0 = node->has_T0 = 0;
  }
  
  // boundary conditions
  mesh_t *mesh = feenox.pde.mesh;
  for (physical_group_t *physical_group = mesh->physical_groups; physical_group != NULL; physical_group = physical_group->hh.next) {
    for (size_t l = 0; l < physical_group->n_bcs; l++) {
      bc_data_t *bc_data = NULL;
      DL_FOREACH(physical_group->bc[l]->bc_datums, bc_data) {
        for (size_t i = 0; i < physical_group->n_elements; i++) {
          element_t *e = &mesh->element[physical_group->element[i]];
          for (unsigned int m = 0; m < e->type->nodes; m++) {
            size_t j = e->node[m]->index_mesh;
            th_node_t *node = &thermal_hydraulics.nodes[j];
            feenox_var_value(feenox.mesh.vars.x) = e->node[m]->x[0];
            feenox_var_value(feenox.mesh.vars.y) = e->node[m]->x[1];
            feenox_var_value(feenox.mesh.vars.z) = e->node[m]->x[2];
            
            if (bc_data->condition.items != NULL && fabs(feenox_expression_eval(&bc_data->condition)) < DEFAULT_CONDITION_THRESHOLD) {
              continue;
            }
            
            double value = feenox_expression_eval(&bc_data->expr);
            switch (bc_data->type_phys) {
              case BC_TYPE_TH_PRESSURE:
                node->has_p = 1;
                node->p_bc = value;
              break;
              case BC_TYPE_TH_STAGNATION_PRESSURE:
                node->has_p0 = 1;
                node->p0_bc = value;
              break;
              case BC_TYPE_TH_STAGNATION_TEMPERATURE:
                node->has_T0 = 1;
                node->T0_bc = value;
              break;
              case BC_TYPE_TH_TEMPERATURE:
                node->has_T = 1;
                node->T_bc = value;
              break;
              case BC_TYPE_TH_DENSITY:
                node->has_rho = 1;
                node->rho_bc = value;
              break;
              case BC_TYPE_TH_VELOCITY:
                node->has_v = 1;
                node->v_bc = value;
              break;
              case BC_TYPE_TH_MASS_FLOW:
                node->has_mdot = 1;
                node->mdot_bc = value;
              break;
            }
          }
        }
      }
    }
  }
  
  // consistency checks and reference values
  int n_p = 0;
  int n_T = 0;
  thermal_hydraulics.mdot_ref = 0;
  for (size_t j = 0; j < thermal_hydraulics.n_nodes; j++) {
    th_node_t *node = &thermal_hydraulics.nodes[j];
    if (node->has_p0 != node->has_T0) {
      feenox_push_error_message("node %ld requires both 'p0' and 'T0' boundary conditions", mesh->node[j].tag);
      return FEENOX_ERROR;
    }
    if (node->has_p0) {
      if (node->n_pipes != 1) {
        feenox_push_error_message("node %ld: 'p0' and 'T0' boundary conditions require a pipe endpoint", mesh->node[j].tag);
        return FEENOX_ERROR;
      }
      if (node->has_p || node->has_T || node->has_rho || node->has_v || node->has_mdot) {
        feenox_push_error_message("node %ld: 'p0' and 'T0' conflict with p, T, rho, v, or mdot boundary conditions", mesh->node[j].tag);
        return FEENOX_ERROR;
      }
      if (!isfinite(node->p0_bc) || node->p0_bc <= 0) {
        feenox_push_error_message("node %ld needs finite stagnation pressure 'p0' > 0", mesh->node[j].tag);
        return FEENOX_ERROR;
      }
      if (!isfinite(node->T0_bc) || node->T0_bc < thermal_hydraulics.fluid.T_min ||
          node->T0_bc > thermal_hydraulics.fluid.T_max) {
        feenox_push_error_message("node %ld has T0 outside the liquid-water range [%g, %g] K",
                                  mesh->node[j].tag, thermal_hydraulics.fluid.T_min, thermal_hydraulics.fluid.T_max);
        return FEENOX_ERROR;
      }
      double rho0 = thermal_hydraulics.fluid.rho(node->p0_bc, node->T0_bc);
      double h0 = thermal_hydraulics.fluid.h(node->p0_bc, node->T0_bc);
      if (!isfinite(rho0) || rho0 <= 0 || !isfinite(h0)) {
        feenox_push_error_message("node %ld has an invalid reservoir state at (p0, T0)", mesh->node[j].tag);
        return FEENOX_ERROR;
      }
    }
    if (node->has_v && node->has_mdot) {
      feenox_push_error_message("node %ld has both 'v' and 'mdot' boundary conditions", mesh->node[j].tag);
      return FEENOX_ERROR;
    }
    if (node->has_p && (node->has_v || node->has_mdot)) {
      feenox_push_error_message("node %ld has both a pressure and a flow boundary condition", mesh->node[j].tag);
      return FEENOX_ERROR;
    }
    if (node->has_T && node->has_rho) {
      feenox_push_error_message("node %ld has both 'T' and 'rho' boundary conditions", mesh->node[j].tag);
      return FEENOX_ERROR;
    }
    if (node->junction_group != NULL && (node->has_p || node->has_v || node->has_mdot)) {
      feenox_push_error_message("junction point '%s' cannot also have p, v, or mdot boundary conditions", node->junction_group->name);
      return FEENOX_ERROR;
    }
    
    n_p += node->has_p || node->has_p0;
    n_T += node->has_T || node->has_rho || node->has_T0;
    
    if (node->has_mdot) {
      thermal_hydraulics.mdot_ref = GSL_MAX(thermal_hydraulics.mdot_ref, fabs(node->mdot_bc));
    } else if (node->has_v) {
      double rho = (node->has_rho) ? node->rho_bc : 1000.0;
      thermal_hydraulics.mdot_ref = GSL_MAX(thermal_hydraulics.mdot_ref, fabs(rho * node->v_bc * node->A));
    }
  }
  
  if (thermal_hydraulics.transient == 0 && n_p == 0) {
    feenox_push_error_message("steady-state thermal_hydraulics problems need at least one pressure boundary condition 'p' or 'p0'");
    return FEENOX_ERROR;
  }
  if (n_T == 0 && !(feenox_special_var_value(end_time) > 0 && feenox_get_function_ptr("T_0") != NULL)) {
    feenox_push_error_message("thermal_hydraulics problems need at least one boundary condition with temperature 'T', density 'rho', or stagnation temperature 'T0'");
    return FEENOX_ERROR;
  }
  
  if (thermal_hydraulics.mdot_ref == 0) {
    thermal_hydraulics.mdot_ref = 1.0;
  }
  thermal_hydraulics.p_ref = 1e3;
  thermal_hydraulics.h_ref = 1e3;
  
  return FEENOX_OK;
}

static int thermal_hydraulics_initial_guess(void) {
  
  // average pressure and temperature from the BCs
  double p_guess = 0;
  double T_guess = 0;
  int n_p = 0;
  int n_T = 0;
  for (size_t j = 0; j < thermal_hydraulics.n_nodes; j++) {
    th_node_t *node = &thermal_hydraulics.nodes[j];
    if (node->has_p) {
      p_guess += node->p_bc;
      n_p++;
    } else if (node->has_p0) {
      p_guess += node->p0_bc;
      n_p++;
    }
    if (node->has_T) {
      T_guess += node->T_bc;
      n_T++;
    } else if (node->has_T0) {
      T_guess += node->T0_bc;
      n_T++;
    }
  }
  p_guess = (n_p > 0) ? p_guess/n_p : 1e5;
  
  if (n_T == 0) {
    for (size_t j = 0; j < thermal_hydraulics.n_nodes; j++) {
      th_node_t *node = &thermal_hydraulics.nodes[j];
      if (node->has_rho) {
        T_guess += thermal_hydraulics.fluid.T_from_p_rho(p_guess, node->rho_bc);
        n_T++;
      }
    }
  }
  T_guess = (n_T > 0) ? T_guess/n_T : 300;
  
  // total mass flow entering through flow BCs
  double mdot_guess = 0;
  int mdot_sign = 1;
  double rho_guess = thermal_hydraulics.fluid.rho(p_guess, T_guess);
  for (size_t j = 0; j < thermal_hydraulics.n_nodes; j++) {
    th_node_t *node = &thermal_hydraulics.nodes[j];
    double inflow = node->has_mdot ? node->mdot_bc :
                    node->has_v ? ((node->has_rho) ? node->rho_bc : rho_guess)*node->v_bc*node->A : 0;
    if (inflow > 0) {
      if (mdot_guess == 0 && node->n_pipes == 1) {
        mdot_sign = -node->sign[0];
      }
      mdot_guess += inflow;
    }
  }
  
  // optional user-provided initial guesses
///pr_thermal_hydraulics+p_guess+description Initial guess for the pressure as a function of space (optional).
  function_t *p_guess_function = feenox_get_function_ptr("p_guess");
///pr_thermal_hydraulics+T_guess+description Initial guess for the temperature as a function of space (optional).
  function_t *T_guess_function = feenox_get_function_ptr("T_guess");
  
  PetscScalar *x = NULL;
  petsc_call(VecGetArray(thermal_hydraulics.x, &x));
  for (size_t j = 0; j < thermal_hydraulics.n_nodes; j++) {
    double *xx = feenox.pde.mesh->node[j].x;
    x[TH_P(j)] = (p_guess_function != NULL) ? feenox_function_eval(p_guess_function, xx) : p_guess;
    x[TH_T(j)] = (T_guess_function != NULL) ? feenox_function_eval(T_guess_function, xx) : T_guess;
    for (th_heat_structure_t *hs = thermal_hydraulics.heat_structures; hs != NULL; hs = hs->hh.next) {
      for (size_t i = 0; i < hs->nr; i++) {
        x[TH_SOLID(hs,j,i)] = x[TH_T(j)];
      }
    }
  }
  for (size_t k = 0; k < thermal_hydraulics.n_pipes; k++) {
    x[TH_MDOT(k)] = mdot_sign*mdot_guess;
  }
  if (mdot_guess == 0) {
    // At rest the quadratic pressure-loss and stagnation terms have zero
    // derivative. Seed pressure-driven components to avoid a singular first
    // Newton step, independently of pipe element orientations.
    size_t *queue = NULL, *distance = NULL;
    feenox_check_alloc(queue = calloc(thermal_hydraulics.n_nodes, sizeof(size_t)));
    feenox_check_alloc(distance = calloc(thermal_hydraulics.n_nodes, sizeof(size_t)));
    for (size_t root = 0; root < thermal_hydraulics.n_nodes; root++) {
      th_node_t *inlet = &thermal_hydraulics.nodes[root];
      if (!inlet->has_p0 || distance[root] != 0) continue;
      size_t count = 1;
      queue[0] = root;
      distance[root] = 1;
      double outlet_pressure = 0;
      size_t n_outlets = 0;
      for (size_t head = 0; head < count; head++) {
        size_t j = queue[head];
        th_node_t *node = &thermal_hydraulics.nodes[j];
        if (node->has_p) {
          outlet_pressure += node->p_bc;
          n_outlets++;
        }
        for (size_t i = 0; i < node->n_pipes; i++) {
          th_pipe_t *pipe = &thermal_hydraulics.pipes[node->pipe[i]];
          size_t other = pipe->a == j ? pipe->b : pipe->a;
          if (distance[other] == 0) {
            distance[other] = distance[j]+1;
            queue[count++] = other;
          }
        }
      }
      if (n_outlets == 0) continue;
      double dp = inlet->p0_bc-outlet_pressure/n_outlets;
      double rho0 = thermal_hydraulics.fluid.rho(inlet->p0_bc, inlet->T0_bc);
      double seed = inlet->A*sqrt(2*rho0*fabs(dp));
      if (dp < 0) seed = -seed;
      for (size_t head = 0; head < count; head++) {
        th_node_t *node = &thermal_hydraulics.nodes[queue[head]];
        for (size_t i = 0; i < node->n_pipes; i++) {
          size_t k = node->pipe[i];
          th_pipe_t *pipe = &thermal_hydraulics.pipes[k];
          x[TH_MDOT(k)] = distance[pipe->b] > distance[pipe->a] ? seed :
                          distance[pipe->b] < distance[pipe->a] ? -seed : 0;
        }
      }
    }
    feenox_free(queue);
    feenox_free(distance);
  }
  petsc_call(VecRestoreArray(thermal_hydraulics.x, &x));
  
  return FEENOX_OK;
}

// the jacobian is computed by finite differences with coloring
// so we only need to provide the non-zero structure
static int thermal_hydraulics_create_jacobian(void) {

  size_t N = thermal_hydraulics.n_nodes;
  PetscInt size = 2*N + thermal_hydraulics.n_pipes + thermal_hydraulics.n_solid_unknowns;
  
  PetscInt *nnz = NULL;
  feenox_check_alloc(nnz = calloc(size, sizeof(PetscInt)));
  for (size_t j = 0; j < N; j++) {
    // own p & T plus mdot and p & T of the neighbor for each pipe
    nnz[TH_P(j)] = nnz[TH_T(j)] = 2 + 3*thermal_hydraulics.nodes[j].n_pipes;
    for (th_heat_structure_t *hs = thermal_hydraulics.heat_structures; hs != NULL; hs = hs->hh.next) {
      nnz[TH_T(j)]++;
      for (size_t i = 0; i < hs->nr; i++) {
        nnz[TH_SOLID(hs,j,i)] = 5+thermal_hydraulics.nodes[j].n_pipes;
      }
    }
  }
  for (size_t k = 0; k < thermal_hydraulics.n_pipes; k++) {
    th_pipe_t *pipe = &thermal_hydraulics.pipes[k];
    nnz[TH_MDOT(k)] = GSL_MIN(size, 5+thermal_hydraulics.nodes[pipe->a].n_pipes+thermal_hydraulics.nodes[pipe->b].n_pipes);
  }
  petsc_call(MatCreateSeqAIJ(PETSC_COMM_SELF, size, size, 0, nnz, &thermal_hydraulics.J));
  petsc_call(PetscObjectSetName((PetscObject)thermal_hydraulics.J, "J_th"));
  feenox_free(nnz);

  for (size_t j = 0; j < N; j++) {
    th_node_t *node = &thermal_hydraulics.nodes[j];
    for (int row_offset = 0; row_offset < 2; row_offset++) {
      PetscInt row = 2*j + row_offset;
      PetscInt cols[2];
      cols[0] = TH_P(j);
      cols[1] = TH_T(j);
      petsc_call(MatSetValue(thermal_hydraulics.J, row, cols[0], 0, INSERT_VALUES));
      petsc_call(MatSetValue(thermal_hydraulics.J, row, cols[1], 0, INSERT_VALUES));
      for (size_t i = 0; i < node->n_pipes; i++) {
        th_pipe_t *pipe = &thermal_hydraulics.pipes[node->pipe[i]];
        size_t other = (pipe->a == j) ? pipe->b : pipe->a;
        petsc_call(MatSetValue(thermal_hydraulics.J, row, TH_MDOT(node->pipe[i]), 0, INSERT_VALUES));
        petsc_call(MatSetValue(thermal_hydraulics.J, row, TH_P(other), 0, INSERT_VALUES));
        petsc_call(MatSetValue(thermal_hydraulics.J, row, TH_T(other), 0, INSERT_VALUES));
      }
    }
  }
  for (size_t k = 0; k < thermal_hydraulics.n_pipes; k++) {
    th_pipe_t *pipe = &thermal_hydraulics.pipes[k];
    PetscInt row = TH_MDOT(k);
    PetscInt cols[5] = {TH_P(pipe->a), TH_T(pipe->a), TH_P(pipe->b), TH_T(pipe->b), TH_MDOT(k)};
    PetscScalar zeros[5] = {0, 0, 0, 0, 0};
    petsc_call(MatSetValues(thermal_hydraulics.J, 1, &row, 5, cols, zeros, INSERT_VALUES));
    for (int side = 0; side < 2; side++) {
      th_node_t *node = &thermal_hydraulics.nodes[side == 0 ? pipe->a : pipe->b];
      for (size_t i = 0; i < node->n_pipes; i++) {
        petsc_call(MatSetValue(thermal_hydraulics.J, row, TH_MDOT(node->pipe[i]), 0, INSERT_VALUES));
      }
    }
  }
  for (th_heat_structure_t *hs = thermal_hydraulics.heat_structures; hs != NULL; hs = hs->hh.next) {
    size_t nr = hs->nr;
    for (size_t j = 0; j < N; j++) {
      if (!hs->exchanger) {
        petsc_call(MatSetValue(thermal_hydraulics.J, TH_T(j), TH_SOLID(hs,j,nr-1), 0, INSERT_VALUES));
      }
      for (size_t i = 0; i < nr; i++) {
        PetscInt row = TH_SOLID(hs,j,i);
        petsc_call(MatSetValue(thermal_hydraulics.J, row, row, 0, INSERT_VALUES));
        petsc_call(MatSetValue(thermal_hydraulics.J, row, TH_T(j), 0, INSERT_VALUES));
        if (i > 0) petsc_call(MatSetValue(thermal_hydraulics.J, row, TH_SOLID(hs,j,i-1), 0, INSERT_VALUES));
        if (i+1 < nr) petsc_call(MatSetValue(thermal_hydraulics.J, row, TH_SOLID(hs,j,i+1), 0, INSERT_VALUES));
        th_node_t *node = &thermal_hydraulics.nodes[j];
        for (size_t l = 0; l < node->n_pipes; l++) {
          th_pipe_t *pipe = &thermal_hydraulics.pipes[node->pipe[l]];
          size_t other = pipe->a == j ? pipe->b : pipe->a;
          petsc_call(MatSetValue(thermal_hydraulics.J, row, TH_SOLID(hs,other,i), 0, INSERT_VALUES));
        }
      }
      if (hs->exchanger) {
        for (size_t j = 0; j < N; j++) {
          if (hs->node_length[j] == 0) continue;
          size_t m = hs->partner[j];
          petsc_call(MatSetValue(thermal_hydraulics.J, TH_T(j), TH_SOLID(hs,j,0), 0, INSERT_VALUES));
          petsc_call(MatSetValue(thermal_hydraulics.J, TH_T(m), TH_SOLID(hs,j,nr-1), 0, INSERT_VALUES));
          petsc_call(MatSetValue(thermal_hydraulics.J, TH_SOLID(hs,j,nr-1), TH_T(m), 0, INSERT_VALUES));
        }
      }
    }
  }
  petsc_call(MatAssemblyBegin(thermal_hydraulics.J, MAT_FINAL_ASSEMBLY));
  petsc_call(MatAssemblyEnd(thermal_hydraulics.J, MAT_FINAL_ASSEMBLY));
  
  return FEENOX_OK;
}

static int thermal_hydraulics_setup_snes(void) {
  
  petsc_call(SNESCreate(PETSC_COMM_SELF, &thermal_hydraulics.snes));
  petsc_call(SNESSetFunction(thermal_hydraulics.snes, thermal_hydraulics.r, thermal_hydraulics_residual, NULL));
  petsc_call(SNESSetJacobian(thermal_hydraulics.snes, thermal_hydraulics.J, thermal_hydraulics.J, SNESComputeJacobianDefaultColor, NULL));
  
  if (feenox.pde.snes_type != NULL) {
    petsc_call(SNESSetType(thermal_hydraulics.snes, feenox.pde.snes_type));
  }
  if (feenox.pde.ls_type != NULL) {
    SNESLineSearch ls;
    petsc_call(SNESGetLineSearch(thermal_hydraulics.snes, &ls));
    petsc_call(SNESLineSearchSetType(ls, feenox.pde.ls_type));
  }
  
  petsc_call(SNESSetTolerances(thermal_hydraulics.snes, feenox_var_value(feenox.pde.vars.snes_atol),
                                                        feenox_var_value(feenox.pde.vars.snes_rtol),
                                                        feenox_var_value(feenox.pde.vars.snes_stol),
                                                        (PetscInt)feenox_var_value(feenox.pde.vars.snes_max_it),
                                                        PETSC_DEFAULT));
  
  // the system is small and the diagonal has zeros (the mass equations do not
  // depend on the nodal pressure) so we go for a direct solver
  KSP ksp;
  PC pc;
  petsc_call(SNESGetKSP(thermal_hydraulics.snes, &ksp));
  petsc_call(KSPSetType(ksp, KSPPREONLY));
  petsc_call(KSPGetPC(ksp, &pc));
  petsc_call(PCSetType(pc, PCLU));
#ifdef PETSC_HAVE_MUMPS
  petsc_call(PCFactorSetMatSolverType(pc, MATSOLVERMUMPS));
#else
  petsc_call(PCFactorReorderForNonzeroDiagonal(pc, 1e-12));
#endif
  
  petsc_call(SNESSetFromOptions(thermal_hydraulics.snes));
  
  return FEENOX_OK;
}

static int thermal_hydraulics_store_old(void) {
  
  if (feenox_special_var_value(end_time) == 0) {
    return FEENOX_OK;
  }
  
  if (thermal_hydraulics.x_old == NULL) {
    petsc_call(VecDuplicate(thermal_hydraulics.x, &thermal_hydraulics.x_old));
    feenox_check_alloc(thermal_hydraulics.rho_old = calloc(thermal_hydraulics.n_nodes, sizeof(double)));
    feenox_check_alloc(thermal_hydraulics.rhou_old = calloc(thermal_hydraulics.n_nodes, sizeof(double)));
    feenox_check_alloc(thermal_hydraulics.mdot_old = calloc(thermal_hydraulics.n_pipes, sizeof(double)));
  }
  petsc_call(VecCopy(thermal_hydraulics.x, thermal_hydraulics.x_old));
  
  const PetscScalar *x = NULL;
  petsc_call(VecGetArrayRead(thermal_hydraulics.x, &x));
  for (size_t j = 0; j < thermal_hydraulics.n_nodes; j++) {
    double p = x[TH_P(j)];
    double T = x[TH_T(j)];
    thermal_hydraulics.rho_old[j] = thermal_hydraulics.fluid.rho(p, T);
    thermal_hydraulics.rhou_old[j] = thermal_hydraulics.rho_old[j] * thermal_hydraulics.fluid.h(p, T) - p;
    thermal_hydraulics.nodes[j].kinetic_energy_old =
        thermal_hydraulics_junction_kinetic_energy(j, x, thermal_hydraulics.rho_old[j]);
  }
  for (size_t k = 0; k < thermal_hydraulics.n_pipes; k++) {
    thermal_hydraulics.mdot_old[k] = x[TH_MDOT(k)];
  }
  petsc_call(VecRestoreArrayRead(thermal_hydraulics.x, &x));
  
  return FEENOX_OK;
}

static int thermal_hydraulics_solid_residual(th_heat_structure_t *hs, const PetscScalar *x, PetscScalar *r) {
  double scale = thermal_hydraulics.mdot_ref*thermal_hydraulics.h_ref;
  double q = hs->exchanger ? 0 : hs->power/(hs->rod_count*hs->length*M_PI*gsl_pow_2(hs->radius[hs->fuel_surface]));
  const PetscScalar *old = NULL;
  if (thermal_hydraulics.transient) {
    petsc_call(VecGetArrayRead(thermal_hydraulics.x_old, &old));
  }
  for (size_t j = 0; j < thermal_hydraulics.n_nodes; j++) {
    double L = hs->node_length[j];
    for (size_t i = 0; i < hs->nr; i++) {
      r[TH_SOLID(hs,j,i)] = L > 0 ? 0 : (x[TH_SOLID(hs,j,i)]-x[TH_T(j)])/1000;
    }
    if (L == 0) continue;
    for (size_t i = 0; i+1 < hs->nr; i++) {
      int l = hs->layer[i];
      double dr = hs->radius[i+1]-hs->radius[i];
      double conductance = hs->exchanger && hs->plate ? hs->depth*hs->k[l]/dr :
                           (hs->exchanger ? 1 : hs->rod_count)*M_PI*hs->k[l]*(hs->radius[i]+hs->radius[i+1])/dr;
      double heat = conductance*L*(x[TH_SOLID(hs,j,i+1)]-x[TH_SOLID(hs,j,i)]);
      r[TH_SOLID(hs,j,i)] += heat/scale;
      r[TH_SOLID(hs,j,i+1)] -= heat/scale;
      for (int side = 0; side < 2; side++) {
        size_t row = TH_SOLID(hs,j,i+side);
        double V = thermal_hydraulics_radial_weight(hs, i, side)*L;
        if (l == 0) r[row] += q*V/scale;
        if (old != NULL) {
          r[row] -= hs->rho[l]*hs->cp[l]*V*(x[row]-old[row])/(thermal_hydraulics.dt*scale);
        }
      }
    }
  }
  if (hs->axial_conduction) {
    for (size_t k = 0; k < thermal_hydraulics.n_pipes; k++) {
      th_pipe_t *pipe = &thermal_hydraulics.pipes[k];
      if (pipe->element->physical_group != hs->group) continue;
      for (size_t i = 0; i+1 < hs->nr; i++) {
        for (int side = 0; side < 2; side++) {
          double conductance = hs->k[hs->layer[i]]*thermal_hydraulics_radial_weight(hs, i, side)/pipe->L;
          size_t a = TH_SOLID(hs,pipe->a,i+side), b = TH_SOLID(hs,pipe->b,i+side);
          double heat = conductance*(x[b]-x[a]);
          r[a] += heat/scale;
          r[b] -= heat/scale;
        }
      }
    }
  }
  if (old != NULL) {
    petsc_call(VecRestoreArrayRead(thermal_hydraulics.x_old, &old));
  }
  return FEENOX_OK;
}

static PetscErrorCode thermal_hydraulics_residual(SNES snes, Vec X, Vec R, void *ctx) {
  
  const PetscScalar *x = NULL;
  PetscScalar *r = NULL;
  petsc_call(VecGetArrayRead(X, &x));
  petsc_call(VecGetArray(R, &r));
  
  th_fluid_t *fluid = &thermal_hydraulics.fluid;
  double *rho = thermal_hydraulics.rho;
  double *h = thermal_hydraulics.h;
  int transient = thermal_hydraulics.transient;
  double dt = thermal_hydraulics.dt;
  for (th_heat_structure_t *hs = thermal_hydraulics.heat_structures; hs != NULL; hs = hs->hh.next) {
    petsc_call(thermal_hydraulics_solid_residual(hs, x, r));
  }
  
  for (size_t j = 0; j < thermal_hydraulics.n_nodes; j++) {
    rho[j] = fluid->rho(x[TH_P(j)], x[TH_T(j)]);
    h[j] = fluid->h(x[TH_P(j)], x[TH_T(j)]);
  }
  
  // mass and energy balances at the nodes
  for (size_t j = 0; j < thermal_hydraulics.n_nodes; j++) {
    th_node_t *node = &thermal_hydraulics.nodes[j];
    double p = x[TH_P(j)];
    
    double net_in = 0;          // net mass flow entering the node through the pipes
    double energy = node->Q;    // energy balance in W
    if (node->pump != NULL) {
      th_point_t *pump = node->pump;
      th_pipe_t *in = &thermal_hydraulics.pipes[pump->suction];
      double pump_flow = (in->b == j ? 1 : -1)*x[TH_MDOT(pump->suction)];
      energy += pump_flow*pump->specific_work;
    }
    for (size_t i = 0; i < node->n_pipes; i++) {
      size_t k = node->pipe[i];
      th_pipe_t *pipe = &thermal_hydraulics.pipes[k];
      size_t other = (pipe->a == j) ? pipe->b : pipe->a;
      double mdot_in = node->sign[i] * x[TH_MDOT(k)];
      net_in += mdot_in;
      // upwind: only incoming flows bring a different enthalpy
      if (mdot_in > 0) {
        energy += mdot_in * (h[other] - h[j]);
      }
      energy += TH_EPSILON_MDOT * (h[other] - h[j]);
      if (node->junction_group != NULL) {
        // Convert the difference in port kinetic-energy flux to enthalpy.
        energy += mdot_in*gsl_pow_2(x[TH_MDOT(k)]/(rho[j]*pipe->A))/2;
      }
      if (pipe->P_h > 0) {
        double q_wall = pipe->q_wall;
        th_heat_structure_t *hs = pipe->heat_structure;
        if (hs != NULL) {
          q_wall = pipe->h_wall*(x[TH_SOLID(hs,j,hs->nr-1)]-x[TH_T(j)]);
          double heat = 0.5*pipe->P_h*pipe->L*q_wall;
          r[TH_SOLID(hs,j,hs->nr-1)] -= heat/(thermal_hydraulics.mdot_ref*thermal_hydraulics.h_ref);
        } else if (pipe->has_T_wall) {
          double h_wall = thermal_hydraulics_wall_coefficient(pipe, p, x[TH_T(j)], x[TH_MDOT(k)]);
          if (!isfinite(h_wall) || h_wall < 0) {
            petsc_call(VecRestoreArray(R, &r));
            petsc_call(VecRestoreArrayRead(X, &x));
            petsc_call(SNESSetFunctionDomainError(snes));
            return 0;
          }
          q_wall = h_wall*(pipe->T_wall-x[TH_T(j)]);
        }
        energy += 0.5*pipe->P_h*pipe->L*q_wall;
      }
    }
    
    double mass_accumulation = 0;
    double energy_accumulation = 0;
    if (transient) {
      mass_accumulation = node->V * (rho[j] - thermal_hydraulics.rho_old[j]) / dt;
      energy_accumulation = node->V * ((rho[j]*h[j] - p) - thermal_hydraulics.rhou_old[j] - h[j]*(rho[j] - thermal_hydraulics.rho_old[j])) / dt;
      if (node->junction_group != NULL) {
        energy_accumulation += (thermal_hydraulics_junction_kinetic_energy(j, x, rho[j])-node->kinetic_energy_old)/dt;
      }
    }
    
    // the state of the fluid that enters through the boundary (if any)
    double mdot_bc = 0;
    double h_bc = h[j];
    double rho_bc = rho[j];
    if (node->has_p0) {
      h_bc = fluid->h(node->p0_bc, node->T0_bc);
      rho_bc = fluid->rho(node->p0_bc, node->T0_bc);
      if (!isfinite(h_bc) || !isfinite(rho_bc) || rho_bc <= 0 ||
          !isfinite(rho[j]) || rho[j] <= 0) {
        petsc_call(VecRestoreArray(R, &r));
        petsc_call(VecRestoreArrayRead(X, &x));
        petsc_call(SNESSetFunctionDomainError(snes));
        return 0;
      }
    } else if (node->has_T) {
      h_bc = fluid->h(p, node->T_bc);
      rho_bc = fluid->rho(p, node->T_bc);
    } else if (node->has_rho) {
      double T_bc = fluid->T_from_p_rho(p, node->rho_bc);
      h_bc = fluid->h(p, T_bc);
      rho_bc = node->rho_bc;
    }
    
    if (node->has_p || node->has_p0) {
      // the boundary provides whatever is needed to close the mass balance
      mdot_bc = mass_accumulation - net_in;
      if (node->has_p0) {
        // Low-Mach stagnation pressure uses the adjacent pipe's local velocity;
        // Convert reservoir total enthalpy to static enthalpy only for inflow.
        double v = x[TH_MDOT(node->pipe[0])] / (rho[j]*node->A);
        double dynamic_pressure = 0.5*rho[j]*v*v;
        if (!isfinite(dynamic_pressure)) {
          petsc_call(VecRestoreArray(R, &r));
          petsc_call(VecRestoreArrayRead(X, &x));
          petsc_call(SNESSetFunctionDomainError(snes));
          return 0;
        }
        r[TH_P(j)] = (p + dynamic_pressure - node->p0_bc) / thermal_hydraulics.p_ref;
        h_bc -= 0.5*v*v;
      } else {
        r[TH_P(j)] = (p - node->p_bc) / thermal_hydraulics.p_ref;
      }
    } else {
      if (node->has_mdot) {
        mdot_bc = node->mdot_bc;
      } else if (node->has_v) {
        mdot_bc = rho_bc * node->v_bc * node->A;
      }
      r[TH_P(j)] = (net_in + mdot_bc - mass_accumulation) / thermal_hydraulics.mdot_ref;
    }
    
    if (mdot_bc > 0) {
      energy += mdot_bc * (h_bc - h[j]);
    }
    r[TH_T(j)] = (energy - energy_accumulation) / (thermal_hydraulics.mdot_ref * thermal_hydraulics.h_ref);
  }
  
  for (th_heat_structure_t *hs = thermal_hydraulics.heat_structures; hs != NULL; hs = hs->hh.next) {
    if (!hs->exchanger) continue;
    for (size_t j = 0; j < thermal_hydraulics.n_nodes; j++) {
      if (hs->node_length[j] == 0) continue;
      for (int side = 0; side < 2; side++) {
        double heat = thermal_hydraulics_interface_heat(hs, j, side, x) /
                      (thermal_hydraulics.mdot_ref*thermal_hydraulics.h_ref);
        size_t fluid_node = side == 0 ? j : hs->partner[j];
        r[TH_T(fluid_node)] += heat;
        r[TH_SOLID(hs,j,side == 0 ? 0 : hs->nr-1)] -= heat;
      }
    }
  }

  // momentum along the pipes
  double g[3];
  for (int d = 0; d < 3; d++) {
    g[d] = feenox_var_value(thermal_hydraulics.g[d]);
  }
  for (size_t k = 0; k < thermal_hydraulics.n_pipes; k++) {
    th_pipe_t *pipe = &thermal_hydraulics.pipes[k];
    size_t a = pipe->a;
    size_t b = pipe->b;
    double mdot = x[TH_MDOT(k)];
    double rho_e = 0.5*(rho[a] + rho[b]);
    double A = pipe->A;
    
    double f = pipe->f;
    if (pipe->f_given == 0) {
      double T_e = 0.5*(x[TH_T(a)] + x[TH_T(b)]);
      double Re = fabs(mdot) * pipe->D_h / (A * fluid->mu(rho_e, T_e));
      f = thermal_hydraulics_friction_churchill(Re, pipe->roughness/pipe->D_h);
    }
    
    double friction = f * pipe->L/pipe->D_h * mdot*fabs(mdot) / (2*rho_e*A*A);
    double gravity = rho_e * (g[0]*pipe->dx[0] + g[1]*pipe->dx[1] + g[2]*pipe->dx[2]);
    double acceleration = mdot*mdot/(A*A) * (1.0/rho[b] - 1.0/rho[a]);
    double inertia = (transient) ? pipe->L/A * (mdot - thermal_hydraulics.mdot_old[k]) / dt : 0;
    
    double p_a = thermal_hydraulics_port_pressure(pipe, 0, x);
    double p_b = thermal_hydraulics_port_pressure(pipe, 1, x);
    r[TH_MDOT(k)] = (p_a - p_b + gravity - friction - acceleration - inertia) / thermal_hydraulics.p_ref;
  }
  
  petsc_call(VecRestoreArray(R, &r));
  petsc_call(VecRestoreArrayRead(X, &x));
  
  return 0;
}

#endif
