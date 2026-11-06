/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for one-dimensional thermal-hydraulics: global header
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
#ifndef THERMAL_HYDRAULICS_H
#define THERMAL_HYDRAULICS_H

// physical types of boundary conditions
#define BC_TYPE_TH_UNDEFINED     0
#define BC_TYPE_TH_PRESSURE      1
#define BC_TYPE_TH_TEMPERATURE   2
#define BC_TYPE_TH_DENSITY       3
#define BC_TYPE_TH_VELOCITY      4
#define BC_TYPE_TH_MASS_FLOW     5

// regularization flow (kg/s) used in the energy equation of stagnant nodes
#define TH_EPSILON_MDOT          1e-10

typedef struct th_fluid_t th_fluid_t;
typedef struct th_pipe_t th_pipe_t;
typedef struct th_node_t th_node_t;
typedef struct th_heat_structure_t th_heat_structure_t;
typedef struct thermal_hydraulics_t thermal_hydraulics_t;

// equation of state and transport properties of the working fluid
struct th_fluid_t {
  char *name;
  double (*rho)(double p, double T);
  double (*h)(double p, double T);
  double (*cp)(double p, double T);
  double (*mu)(double rho, double T);
  double (*k)(double p, double T);
  double (*beta)(double p, double T);
  double (*T_from_p_rho)(double p, double rho);
  double (*psat)(double T);      // NULL if not applicable
  double T_min;                  // validity range (only used for warnings)
  double T_max;
};

// a pipe segment is a 1D element (a "junction" or "flow path" in RELAP's jargon)
struct th_pipe_t {
  element_t *element;
  th_heat_structure_t *heat_structure;
  size_t a;                  // index of the start node
  size_t b;                  // index of the end node
  
  double L;                  // length
  double dx[3];              // x_b - x_a
  double A;                  // flow area
  double D_h;                // hydraulic diameter
  double roughness;          // absolute roughness
  double f;                  // fixed darcy friction factor (if f_given)
  int f_given;
  double q;                  // volumetric power (W/m^3)
  double P_h;                // heated perimeter (m)
  double T_wall;
  double q_wall;             // inward wall heat flux (W/m^2)
  double h_wall;             // prescribed heat transfer coefficient (W/m^2/K)
  int has_T_wall, has_q_wall, has_h_wall;
  double K[2];               // point-material loss coefficients at a and b
  double loss_area[2];
};

// a node is a control volume holding the scalar unknowns
struct th_node_t {
  double V;                  // volume (half of the adjacent pipes)
  double Q;                  // heat (W) deposited into the control volume
  double A;                  // area of the first adjacent pipe (used in velocity BCs)
  physical_group_t *junction_group;
  double junction_area;      // area defining the junction reference pressure
  double kinetic_energy_old;
  
  // adjacency (CSR-like, indexes into the pipe array)
  size_t n_pipes;
  size_t *pipe;
  int *sign;                 // +1 if this node is the end node (flow positive = into the node), -1 otherwise
  
  // boundary conditions evaluated at the start of each solve
  int has_p, has_T, has_rho, has_v, has_mdot;
  double p_bc, T_bc, rho_bc, v_bc, mdot_bc;
};

struct th_heat_structure_t {
  char *group_name;
  char *material_name[3];
  physical_group_t *group;
  expr_t *power_expr;
  expr_t *properties[3][3];   // k, rho, cp
  double k[3], rho[3], cp[3];
  double power, length;
  int axial_conduction;
  size_t nr;
  size_t offset;
  double *radius;
  int *layer;
  double *node_length;
  var_t *heat, *energy;
  function_t *center_function, *fuel_surface_function, *clad_surface_function;
  size_t fuel_surface;
  UT_hash_handle hh;
};

struct thermal_hydraulics_t {
  th_fluid_t fluid;
  th_heat_structure_t *heat_structures;
  size_t n_solid_unknowns;
  var_t *hs_heat, *hs_energy;
  function_t *fuel_center_function, *fuel_surface_function, *clad_surface_function;
  
  // properties
  distribution_t A;
  distribution_t D;
  distribution_t D_h;
  distribution_t roughness;
  distribution_t f;
  distribution_t q;
  distribution_t P_h;
  distribution_t T_wall;
  distribution_t q_wall;
  distribution_t h_wall;
  
  // gravity
  var_t *g[3];
  
  // mesh-based data
  size_t n_nodes;
  size_t n_pipes;
  th_pipe_t *pipes;
  th_node_t *nodes;
  
  // reference scales for the residuals
  double mdot_ref;
  double p_ref;
  double h_ref;
  
  // flag for transient steps (backward euler)
  int transient;
  double dt;
  
  // caches of nodal properties evaluated within the residual
  double *rho;
  double *h;
  double *rho_old;
  double *rhou_old;          // rho*h - p
  double *mdot_old;
  
  // secondary fields
  function_t *rho_function;
  function_t *h_function;
  function_t *v_function;
  function_t *mdot_function;
  
#ifdef HAVE_PETSC
  SNES snes;
  Vec x;
  Vec r;
  Vec x_old;
  Mat J;
#endif
};  
extern thermal_hydraulics_t thermal_hydraulics;

// if97.c
extern double thermal_hydraulics_if97_rho(double p, double T);
extern double thermal_hydraulics_if97_h(double p, double T);
extern double thermal_hydraulics_if97_cp(double p, double T);
extern double thermal_hydraulics_if97_T_from_p_rho(double p, double rho);
extern double thermal_hydraulics_iapws_mu(double rho, double T);
extern double thermal_hydraulics_if97_k(double p, double T);
extern double thermal_hydraulics_if97_beta(double p, double T);
extern double thermal_hydraulics_if97_psat(double T);

// friction.c
extern double thermal_hydraulics_friction_churchill(double Re, double relative_roughness);

#endif /* THERMAL_HYDRAULICS_H */
