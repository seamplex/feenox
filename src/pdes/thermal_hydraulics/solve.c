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
   [ p_0 T_0 p_1 T_1 ... p_{N-1} T_{N-1} mdot_0 mdot_1 ... mdot_{K-1} ]
 
 Residuals:
   row 2j:   mass balance at node j (or p_j - p_bc if node j has a pressure BC)
   row 2j+1: energy balance at node j (non-conservative enthalpy form)
   row 2N+k: momentum balance along pipe k
*/

#define TH_P(j)    (2*(j))
#define TH_T(j)    (2*(j)+1)
#define TH_MDOT(k) (2*thermal_hydraulics.n_nodes + (k))

#ifdef HAVE_PETSC
static int thermal_hydraulics_update_properties(void);
static int thermal_hydraulics_initial_guess(void);
static int thermal_hydraulics_create_jacobian(void);
static int thermal_hydraulics_setup_snes(void);
static int thermal_hydraulics_store_old(void);
static PetscErrorCode thermal_hydraulics_residual(SNES snes, Vec X, Vec R, void *ctx);
#endif

int feenox_problem_solve_thermal_hydraulics(void) {

#ifdef HAVE_PETSC
  
  size_t size = 2*thermal_hydraulics.n_nodes + thermal_hydraulics.n_pipes;
  if (thermal_hydraulics.x == NULL) {
    petsc_call(VecCreateSeq(PETSC_COMM_SELF, size, &thermal_hydraulics.x));
    petsc_call(PetscObjectSetName((PetscObject)thermal_hydraulics.x, "x_th"));
    petsc_call(VecDuplicate(thermal_hydraulics.x, &thermal_hydraulics.r));
  }

  // first evaluate the properties and BCs (they might depend on time)
  thermal_hydraulics.transient = (feenox_special_var_value(in_static) == 0 && feenox_special_var_value(end_time) > 0);
  thermal_hydraulics.dt = feenox_special_var_value(dt);
  feenox_call(thermal_hydraulics_update_properties());
  
  if (thermal_hydraulics.snes == NULL) {
    feenox_call(thermal_hydraulics_initial_guess());
    feenox_call(thermal_hydraulics_create_jacobian());
    feenox_call(thermal_hydraulics_setup_snes());
  }
  
  if (thermal_hydraulics.transient && thermal_hydraulics.x_old == NULL) {
    feenox_push_error_message("internal error, transient step without previous state");
    return FEENOX_ERROR;
  }
  
  if (feenox.pde.do_not_solve == 0) {
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

// evaluate the pipe properties and the boundary conditions at the nodes
static int thermal_hydraulics_update_properties(void) {
  
  for (size_t k = 0; k < thermal_hydraulics.n_pipes; k++) {
    th_pipe_t *pipe = &thermal_hydraulics.pipes[k];
    element_t *e = pipe->element;
    material_t *material = feenox_fem_get_material(e);
    double x[3];
    for (int d = 0; d < 3; d++) {
      x[d] = 0.5*(e->node[0]->x[d] + e->node[1]->x[d]);
    }
    
    double D = (thermal_hydraulics.D.defined) ? thermal_hydraulics.D.eval(&thermal_hydraulics.D, x, material) : 0;
    pipe->A = (thermal_hydraulics.A.defined) ? thermal_hydraulics.A.eval(&thermal_hydraulics.A, x, material) : 0;
    if (pipe->A <= 0 && D > 0) {
      pipe->A = M_PI*D*D/4.0;
    }
    if (pipe->A <= 0) {
      feenox_push_error_message("element %ld has a non-positive flow area (give either 'A' or 'D')", e->tag);
      return FEENOX_ERROR;
    }
    
    pipe->D_h = (thermal_hydraulics.D_h.defined) ? thermal_hydraulics.D_h.eval(&thermal_hydraulics.D_h, x, material) : 0;
    if (pipe->D_h <= 0) {
      pipe->D_h = (D > 0) ? D : sqrt(4*pipe->A/M_PI);
    }
    
    pipe->roughness = (thermal_hydraulics.roughness.defined) ? thermal_hydraulics.roughness.eval(&thermal_hydraulics.roughness, x, material) : 0;
    pipe->f = (thermal_hydraulics.f.defined) ? thermal_hydraulics.f.eval(&thermal_hydraulics.f, x, material) : 0;
    pipe->f_given = (pipe->f > 0);
    pipe->q = (thermal_hydraulics.q.defined) ? thermal_hydraulics.q.eval(&thermal_hydraulics.q, x, material) : 0;
  }
  
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
    
    n_p += node->has_p;
    n_T += node->has_T || node->has_rho;
    
    if (node->has_mdot) {
      thermal_hydraulics.mdot_ref = GSL_MAX(thermal_hydraulics.mdot_ref, fabs(node->mdot_bc));
    } else if (node->has_v) {
      double rho = (node->has_rho) ? node->rho_bc : 1000.0;
      thermal_hydraulics.mdot_ref = GSL_MAX(thermal_hydraulics.mdot_ref, fabs(rho * node->v_bc * node->A));
    }
  }
  
  if (thermal_hydraulics.transient == 0 && n_p == 0) {
    feenox_push_error_message("steady-state thermal_hydraulics problems need at least one pressure boundary condition 'p'");
    return FEENOX_ERROR;
  }
  if (n_T == 0) {
    feenox_push_error_message("thermal_hydraulics problems need at least one boundary condition with either temperature 'T' or density 'rho'");
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
    }
    if (node->has_T) {
      T_guess += node->T_bc;
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
  double rho_guess = thermal_hydraulics.fluid.rho(p_guess, T_guess);
  for (size_t j = 0; j < thermal_hydraulics.n_nodes; j++) {
    th_node_t *node = &thermal_hydraulics.nodes[j];
    if (node->has_mdot && node->mdot_bc > 0) {
      mdot_guess += node->mdot_bc;
    } else if (node->has_v && node->v_bc > 0) {
      mdot_guess += ((node->has_rho) ? node->rho_bc : rho_guess) * node->v_bc * node->A;
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
  }
  for (size_t k = 0; k < thermal_hydraulics.n_pipes; k++) {
    x[TH_MDOT(k)] = mdot_guess;
  }
  petsc_call(VecRestoreArray(thermal_hydraulics.x, &x));
  
  return FEENOX_OK;
}

// the jacobian is computed by finite differences with coloring
// so we only need to provide the non-zero structure
static int thermal_hydraulics_create_jacobian(void) {

  size_t N = thermal_hydraulics.n_nodes;
  PetscInt size = 2*N + thermal_hydraulics.n_pipes;
  
  PetscInt *nnz = NULL;
  feenox_check_alloc(nnz = calloc(size, sizeof(PetscInt)));
  for (size_t j = 0; j < N; j++) {
    // own p & T plus mdot and p & T of the neighbor for each pipe
    nnz[TH_P(j)] = nnz[TH_T(j)] = 2 + 3*thermal_hydraulics.nodes[j].n_pipes;
  }
  for (size_t k = 0; k < thermal_hydraulics.n_pipes; k++) {
    nnz[TH_MDOT(k)] = 5;
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
  }
  for (size_t k = 0; k < thermal_hydraulics.n_pipes; k++) {
    thermal_hydraulics.mdot_old[k] = x[TH_MDOT(k)];
  }
  petsc_call(VecRestoreArrayRead(thermal_hydraulics.x, &x));
  
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
    }
    
    double mass_accumulation = 0;
    double energy_accumulation = 0;
    if (transient) {
      mass_accumulation = node->V * (rho[j] - thermal_hydraulics.rho_old[j]) / dt;
      energy_accumulation = node->V * ((rho[j]*h[j] - p) - thermal_hydraulics.rhou_old[j] - h[j]*(rho[j] - thermal_hydraulics.rho_old[j])) / dt;
    }
    
    // the state of the fluid that enters through the boundary (if any)
    double mdot_bc = 0;
    double h_bc = h[j];
    double rho_bc = rho[j];
    if (node->has_T) {
      h_bc = fluid->h(p, node->T_bc);
      rho_bc = fluid->rho(p, node->T_bc);
    } else if (node->has_rho) {
      double T_bc = fluid->T_from_p_rho(p, node->rho_bc);
      h_bc = fluid->h(p, T_bc);
      rho_bc = node->rho_bc;
    }
    
    if (node->has_p) {
      // the boundary provides whatever is needed to close the mass balance
      mdot_bc = mass_accumulation - net_in;
      r[TH_P(j)] = (p - node->p_bc) / thermal_hydraulics.p_ref;
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
    
    r[TH_MDOT(k)] = (x[TH_P(a)] - x[TH_P(b)] + gravity - friction - acceleration - inertia) / thermal_hydraulics.p_ref;
  }
  
  petsc_call(VecRestoreArray(R, &r));
  petsc_call(VecRestoreArrayRead(X, &x));
  
  return 0;
}

#endif
