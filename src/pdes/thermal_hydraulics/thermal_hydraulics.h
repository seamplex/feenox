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
typedef struct thermal_hydraulics_t thermal_hydraulics_t;

// equation of state and transport properties of the working fluid
struct th_fluid_t {
  char *name;
  double (*rho)(double p, double T);
  double (*h)(double p, double T);
  double (*cp)(double p, double T);
  double (*mu)(double rho, double T);
  double (*T_from_p_rho)(double p, double rho);
  double (*psat)(double T);      // NULL if not applicable
  double T_min;                  // validity range (only used for warnings)
  double T_max;
};

// a pipe segment is a 1D element (a "junction" or "flow path" in RELAP's jargon)
struct th_pipe_t {
  element_t *element;
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
};

// a node is a control volume holding the scalar unknowns
struct th_node_t {
  double V;                  // volume (half of the adjacent pipes)
  double Q;                  // heat (W) deposited into the control volume
  double A;                  // area of the first adjacent pipe (used in velocity BCs)
  
  // adjacency (CSR-like, indexes into the pipe array)
  size_t n_pipes;
  size_t *pipe;
  int *sign;                 // +1 if this node is the end node (flow positive = into the node), -1 otherwise
  
  // boundary conditions evaluated at the start of each solve
  int has_p, has_T, has_rho, has_v, has_mdot;
  double p_bc, T_bc, rho_bc, v_bc, mdot_bc;
};

struct thermal_hydraulics_t {
  th_fluid_t fluid;
  
  // properties
  distribution_t A;
  distribution_t D;
  distribution_t D_h;
  distribution_t roughness;
  distribution_t f;
  distribution_t q;
  
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
extern double thermal_hydraulics_if97_psat(double T);

// friction.c
extern double thermal_hydraulics_friction_churchill(double Re, double relative_roughness);

#endif /* THERMAL_HYDRAULICS_H */
