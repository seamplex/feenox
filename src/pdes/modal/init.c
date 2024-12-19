/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for modal analysis: initialization
 *
 *  Copyright (C) 2021--2023 jeremy theler
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
#include "modal.h"
modal_t modal;

int feenox_problem_parse_time_init_modal(void) {

#ifdef HAVE_SLEPC  
  // virtual methods
  feenox.pde.parse_bc = feenox_problem_bc_parse_modal;
  feenox.pde.parse_write_results = feenox_problem_parse_write_post_modal;
  
  feenox.pde.init_before_run = feenox_problem_init_runtime_modal;
  
  feenox.pde.setup_eps = feenox_problem_setup_eps_modal;
  feenox.pde.setup_ksp = feenox_problem_setup_ksp_modal;
  feenox.pde.setup_pc = feenox_problem_setup_pc_modal;
  
  feenox.pde.element_build_allocate_aux = feenox_problem_build_allocate_aux_modal;
  feenox.pde.element_build_volumetric_at_gauss = feenox_problem_build_volumetric_gauss_point_modal;
  
  feenox.pde.solve_post = feenox_problem_solve_post_modal;
  
  // we are FEM
  feenox.mesh.default_field_location = field_location_nodes;
  
  // move symmetry_axis which is a general PDE setting to
  // the mechanically-particular axisymmetric variant
  if (feenox.pde.symmetry_axis != symmetry_axis_none) {
      modal.variant = variant_axisymmetric;
  }
  
  // check consistency of problem type and dimensions
  if (modal.variant == variant_axisymmetric ||
      modal.variant == variant_plane_stress ||
      modal.variant == variant_plane_strain) {

    if (feenox.pde.dim != 0) {
      if (feenox.pde.dim != 2) {
        feenox_push_error_message("dimension inconsistency, expected 2 dimensions not %d", feenox.pde.dim);
        return FEENOX_ERROR;
      }
    } else {
      feenox.pde.dim = 2;
    }

    if (feenox.pde.dofs != 0) {
      if (feenox.pde.dofs != 2) {
        feenox_push_error_message("DOF inconsistency, expected DOFs per node = 2");
        return FEENOX_ERROR;
      }
    } else {
      feenox.pde.dofs = 2;
    }
    
  } else {
    if (feenox.pde.dim == 0) {
      // default is 3d
      feenox.pde.dim = 3;
    } else if (feenox.pde.dim == 1)   {
      feenox_push_error_message("cannot solve 1D modal problems");
      return FEENOX_ERROR;
    } else if (feenox.pde.dim == 2)   {
      feenox_push_error_message("to solve 2D problems give either plane_stress, plane_strain or axisymmetric");
      return FEENOX_ERROR;
    } else if (feenox.pde.dim != 3) {
      feenox_push_error_message("dimension inconsistency, expected DIM 3 instead of %d", feenox.pde.dim);
      return FEENOX_ERROR;
    }
    feenox.pde.dofs = feenox.pde.dim;
  }
  
  // TODO: custom names
  feenox_check_alloc(feenox.pde.unknown_name = calloc(feenox.pde.dofs, sizeof(char *)));
  feenox_check_alloc(feenox.pde.unknown_name[0] = strdup("u"));
  if (feenox.pde.dofs > 1) {
    feenox_check_alloc(feenox.pde.unknown_name[1] = strdup("v"));
    if (feenox.pde.dofs > 2) {
      feenox_check_alloc(feenox.pde.unknown_name[2] = strdup("w"));
    }  
  }
  
  // if there are no explicit number of eigenvalues we set a non-zero value here
  if (feenox.pde.nev == 0) {
    feenox.pde.nev = DEFAULT_MODAL_MODES;
  }

  // ------- modal-related vectors & outputs -----------------------------------  
///va+M_T+desc A scalar with the total mass\ $m$ computed from the mass matrix\ $M$ as
///va+M_T+desc 
///va+M_T+desc \[ M_T = \frac{1}{n_\text{DOFs}} \cdot \vec{1}^T \cdot M \cdot \vec{1} \]
///va+M_T+desc 
///va+M_T+desc where $n_\text{DOFs}$ is the number of degrees of freedoms per node.
///va+M_T+desc Note that this is only approximately equal to the actual mass which is
///va+M_T+desc the integral of the density $\rho(x,y,z)$ over the problem domain.
  feenox_check_alloc(modal.M_T = feenox_define_variable_get_ptr("M_T"));

///ve+f+desc _Size:_ number of requested modes.
///ve+f+desc _Elements:_ The frequency $f_i$ of the $i$-th mode, in cycles per unit of time.
  feenox_check_alloc(modal.f = feenox_define_vector_get_ptr("f", feenox.pde.nev));

///ve+omega+desc _Size:_ number of requested modes.
///ve+omega+desc _Elements:_ The angular frequency $\omega_i$ of the $i$-th mode, in radians per unit of time.
  feenox_check_alloc(modal.omega = feenox_define_vector_get_ptr("omega", feenox.pde.nev));


///ve+m+desc _Size:_ number of requested modes.
///ve+m+desc _Elements:_ The generalized modal mass $m_i$ of the $i$-th mode computed as
///ve+m+desc
///ve+m+desc \[ \text{m}_i = \frac{1}{n_\text{DOFs}} \vec{\phi}_i^T \cdot M \cdot \vec{\phi}_i \]
///va+m+desc 
///va+m+desc where $n_\text{DOFs}$ is the number of degrees of freedoms per node, $M$ is the mass matrix
///va+m+desc and $\vec{\phi}_i$ is the $i$-th eigenvector normalized such that the largest element is equal to one.

  feenox_check_alloc(modal.m = feenox_define_vector_get_ptr("m", feenox.pde.nev));

///ve+L+desc _Size:_ number of requested modes.
///ve+L+desc _Elements:_ The excitation factor $L_i$ of the $i$-th mode computed as
///ve+L+desc
///ve+L+desc \[ L_i = \frac{1}{n_\text{DOFs}} \cdot \vec{\phi}_i^T \cdot M \cdot \vec{1} \]
///va+L+desc 
///va+L+desc where $n_\text{DOFs}$ is the number of degrees of freedoms per node, $M$ is the mass matrix
///va+L+desc and $\vec{\phi}_i$ is the $i$-th eigenvector normalized such that the largest element is equal to one.
  feenox_check_alloc(modal.L = feenox_define_vector_get_ptr("L", feenox.pde.nev));

///ve+Gamma+desc _Size:_ number of requested modes.
///ve+Gamma+desc _Elements:_ The participation factor $\Gamma_i$ of the $i$-th mode computed as
///ve+Gamma+desc
///ve+Gamma+desc \[ \Gamma_i = \frac{ \vec{\phi}_i^T \cdot M \cdot \vec{1} }{ \vec{\phi}_i^T \cdot M \cdot \vec{\phi}} \]
  feenox_check_alloc(modal.Gamma = feenox_define_vector_get_ptr("Gamma", feenox.pde.nev));

///ve+mu+desc _Size:_ number of requested modes.
///ve+mu+desc _Elements:_ The relative effective modal mass $\mu_i$ of the $i$-th mode computed as
///ve+mu+desc
///ve+mu+desc \[ \mu_i = \frac{L_i^2}{M_t \cdot n_\text{DOFs} \cdot m_i} \]
///ve+mu+desc
///ve+mu+desc Note that $\sum_{i=1}^N m_i = 1$, where $N$ is total number of degrees of freedom ($n_\text{DOFs}$ times the number of nodes).
  feenox_check_alloc(modal.mu = feenox_define_vector_get_ptr("mu", feenox.pde.nev));

///ve+Mu+desc _Size:_ number of requested modes.
///ve+Mu+desc _Elements:_ The accumulated relative effective modal mass $\Mu_i$ up to the $i$-th mode computed as
///ve+Mu+desc
///ve+Mu+desc \[ \Mu_i = \sum_{j=1}^i \mu_i \]
///ve+Mu+desc
///ve+Mu+desc Note that $\Mu_N = 1$, where $N$ is total number of degrees of freedom ($n_\text{DOFs}$ times the number of nodes).
  feenox_check_alloc(modal.Mu = feenox_define_vector_get_ptr("Mu", feenox.pde.nev));

  // define eigenvectors (we don't know its size yet)
  feenox_check_alloc(feenox.pde.vectors.phi = calloc(feenox.pde.nev, sizeof(vector_t *)));;
  for (unsigned int i = 0; i < feenox.pde.nev; i++) {
    char *modename = NULL;
    feenox_check_minusone(asprintf(&modename, "phi%u", i+1));
    feenox_check_alloc(feenox.pde.vectors.phi[i] = feenox_define_vector_get_ptr(modename, 0));
    feenox_free(modename);
  }

  
// these are for the algebraic expressions in the  implicitly-defined BCs
// i.e. 0=u*nx+v*ny or 0=u*y-v*x
// here they are defined as uppercase because there already exist functions named u, v and w
// but the parser changes their case when an implicit BC is read
  modal.displ_for_bc[0]= feenox_define_variable_get_ptr("U");
  modal.displ_for_bc[1]= feenox_define_variable_get_ptr("V");
  modal.displ_for_bc[2]= feenox_define_variable_get_ptr("W");
  
#else
  feenox_push_error_message("modal problems need a FeenoX binary linked against SLEPc.");
  return FEENOX_ERROR;
#endif
  
  return FEENOX_OK;
}


int feenox_problem_init_runtime_modal(void) {

#ifdef HAVE_PETSC  
  feenox.pde.mesh->data_type = data_type_node;
  feenox.pde.spatial_unknowns = feenox.pde.mesh->n_nodes;
  
  // initialize distributions
  feenox_distribution_define_mandatory(modal, rho, "rho", "density");
//  modal.rho.non_uniform = feenox_expression_depends_on_space(modal.rho.dependency_variables);
  
  
  // initialize distributions
  
  // first see if we have linear elastic
  feenox_call(feenox_distribution_init(&modal.E, "E"));  
//  modal.E.non_uniform = feenox_expression_depends_on_space(modal.E.dependency_variables);
  feenox_call(feenox_distribution_init(&modal.nu, "nu"));  
//  modal.nu.non_uniform = feenox_expression_depends_on_space(modal.nu.dependency_variables);
  
  // TODO: allow different volumes to have different material models
  if (modal.E.defined && modal.nu.defined) {
    if (modal.E.full == 0) {
      feenox_push_error_message("Young modulus 'E' is not defined over all volumes");
      return FEENOX_ERROR;
    }
    if (modal.nu.full == 0) {
      feenox_push_error_message("Poisson’s ratio 'nu' is not defined over all volumes");
      return FEENOX_ERROR;
    }
    modal.material_model = material_model_elastic_isotropic;
  } else if (modal.E.defined) {
    feenox_push_error_message("Young modulus 'E' defined but Poisson’s ratio 'nu' not defined");
    return FEENOX_ERROR;
  } else if (modal.nu.defined) {
    feenox_push_error_message("Poisson’s ratio 'nu' defined but Young modulus 'E' not defined");
    return FEENOX_ERROR;
  }
  
  // see if there are orthotropic properties
  feenox_call(feenox_distribution_init(&modal.E_x, "Ex"));
  if (modal.E_x.defined == 0) {
    feenox_call(feenox_distribution_init(&modal.E_x, "E_x"));
  }
  feenox_call(feenox_distribution_init(&modal.E_y, "Ey"));
  if (modal.E_y.defined == 0) {
    feenox_call(feenox_distribution_init(&modal.E_y, "E_y"));
  }
  feenox_call(feenox_distribution_init(&modal.E_z, "Ez"));
  if (modal.E_z.defined == 0) {
    feenox_call(feenox_distribution_init(&modal.E_z, "E_z"));
  }

  feenox_call(feenox_distribution_init(&modal.nu_xy, "nuxy"));
  if (modal.nu_xy.defined == 0) {
    feenox_call(feenox_distribution_init(&modal.nu_xy, "nu_xy"));
  }
  feenox_call(feenox_distribution_init(&modal.nu_yz, "nuyz"));
  if (modal.nu_yz.defined == 0) {
    feenox_call(feenox_distribution_init(&modal.nu_yz, "nu_yz"));
  }
  feenox_call(feenox_distribution_init(&modal.nu_zx, "nuzx"));
  if (modal.nu_zx.defined == 0) {
    feenox_call(feenox_distribution_init(&modal.nu_zx, "nu_zx"));
  }
  
  feenox_call(feenox_distribution_init(&modal.G_xy, "Gxy"));
  if (modal.G_xy.defined == 0) {
    feenox_call(feenox_distribution_init(&modal.G_xy, "G_xy"));
  }
  feenox_call(feenox_distribution_init(&modal.G_yz, "Gyz"));
  if (modal.G_yz.defined == 0) {
    feenox_call(feenox_distribution_init(&modal.G_yz, "G_yz"));
  }
  feenox_call(feenox_distribution_init(&modal.G_zx, "Gzx"));
  if (modal.G_zx.defined == 0) {
    feenox_call(feenox_distribution_init(&modal.G_zx, "G_zx"));
  }
  
  // check for consistency
  int n_ortho = modal.E_x.defined   + modal.E_y.defined   + modal.E_z.defined   +
                modal.nu_xy.defined + modal.nu_yz.defined + modal.nu_zx.defined +
                modal.G_xy.defined  + modal.G_yz.defined  + modal.G_zx.defined;
  
  if (n_ortho > 0) {
    if (modal.material_model == material_model_elastic_isotropic) {
      feenox_push_error_message("both isotropic and orthotropic properties given, choose one");
      return FEENOX_ERROR;
    } else if (n_ortho < 9) {
      feenox_push_error_message("%d orthotropic properties missing", 9-n_ortho);
      return FEENOX_ERROR;
    } else if (modal.material_model == material_model_unknown) {
      modal.material_model = material_model_elastic_orthotropic;
    }
  }  


  // set material model virtual methods
  switch (modal.material_model) {
  
    case material_model_elastic_isotropic:
      modal.uniform_C = ((modal.E.non_uniform == 0) && (modal.nu.non_uniform == 0));
      if (modal.variant == variant_full) {
        modal.compute_C = feenox_problem_build_compute_modal_C_elastic_isotropic;
      } else if (modal.variant == variant_plane_stress) {      
        modal.compute_C = feenox_problem_build_compute_modal_C_elastic_plane_stress;  
      } else if (modal.variant == variant_plane_strain) {  
        modal.compute_C = feenox_problem_build_compute_modal_C_elastic_plane_strain;
      }  
    break;
    
    case material_model_elastic_orthotropic:
      
      if (modal.variant != variant_full) {
        feenox_push_error_message("elastic orthotropic materials cannot be used in plane stress/strain");
        return FEENOX_ERROR;
      }

      modal.compute_C = feenox_problem_build_compute_modal_C_elastic_orthotropic;
    
    break;
    
    default:
      feenox_push_error_message("unknown material model, usual way to go is to define E and nu");
      return FEENOX_ERROR;
    break;
  }

  // size of stress-strain matrix
  if (modal.variant == variant_full) {
    modal.stress_strain_size = 6;
  } else if (modal.variant == variant_axisymmetric) {
    modal.stress_strain_size = 4;
  } else if (modal.variant == variant_plane_stress || modal.variant == variant_plane_strain) {
    modal.stress_strain_size = 3;
  } else {
    feenox_push_error_message("internal mismatch, unknown variant");
    return FEENOX_ERROR;
  }
  
  // allocate stress-strain objects
  feenox_check_alloc(modal.C = gsl_matrix_calloc(modal.stress_strain_size, modal.stress_strain_size));
  if (modal.uniform_C) {
    // cache properties
    feenox_call(modal.compute_C(NULL, NULL));
  } 

  feenox.pde.math_type = math_type_eigen;
  feenox.pde.solve = feenox_problem_solve_slepc_eigen;
  
  feenox.pde.has_stiffness = 1;
  feenox.pde.has_mass = 1;
  feenox.pde.has_rhs = 0;
  
  feenox.pde.has_jacobian_K = 0;
  feenox.pde.has_jacobian_M = 0;
  feenox.pde.has_jacobian_b = 0;
  feenox.pde.has_jacobian = feenox.pde.has_jacobian_K || feenox.pde.has_jacobian_M || feenox.pde.has_jacobian_b;
  
  feenox.pde.symmetric_K = 1;
  feenox.pde.symmetric_M = 1;
#endif
  return FEENOX_OK;
}

#ifdef HAVE_PETSC
int feenox_problem_setup_pc_modal(PC pc) {

  PCType pc_type = NULL;
  petsc_call(PCGetType(pc, &pc_type));
  if (pc_type == NULL) {
    // if we don't set the pc type here then we PCFactorSetMatSolverType does not work
// xxx LU?
    petsc_call(PCSetType(pc, feenox.pde.symmetric_K ? PCCHOLESKY : PCLU));
#ifdef PETSC_HAVE_MUMPS
    petsc_call(PCFactorSetMatSolverType(pc, MATSOLVERMUMPS));
#endif
  }
  
  petsc_call(PCGetType(pc, &pc_type));
  if (strcmp(pc_type, PCGAMG) == 0) {

    if (modal.rigid_body_base == NULL) {
      feenox_problem_compute_rigid_nullspace(&modal.rigid_body_base);
    }  
    
    petsc_call(MatSetNearNullSpace(feenox.pde.K, modal.rigid_body_base));
    petsc_call(MatSetNearNullSpace(feenox.pde.M, modal.rigid_body_base));
  }
  
  return FEENOX_OK;
}

int feenox_problem_setup_ksp_modal(KSP ksp ) {

  KSPType ksp_type = NULL;
  petsc_call(KSPGetType(ksp, &ksp_type));
  if (ksp_type == NULL) {
    // if the user did not choose anything, we default to preonly + direct solver
    petsc_call(KSPSetType(ksp, KSPPREONLY));
  }  

  return FEENOX_OK;
}
#endif

#ifdef HAVE_SLEPC
// these two are not 
int feenox_problem_setup_eps_modal(EPS eps) {

  // to be able to solve free-body vibrations we have to set a deflation space
  if (modal.has_dirichlet_bcs == 0) {
    
    if (feenox.pde.eigen_formulation == eigen_formulation_undefined) {
      feenox.pde.eigen_formulation = eigen_formulation_omega;
    } else if (feenox.pde.eigen_formulation == eigen_formulation_lambda) {
      feenox_push_error_message("free-free modal problems do not work with the lambda formulation");
      return FEENOX_ERROR;
    }
    
    if (modal.rigid_body_base == NULL) {
      feenox_problem_compute_rigid_nullspace(&modal.rigid_body_base);
    }  
  
    // getvecs below needs a const vec pointer
    PetscBool has_const;
    PetscInt n;
    const Vec *vecs;
    Vec rigid[6];
    petsc_call(MatNullSpaceGetVecs(modal.rigid_body_base, &has_const, &n, &vecs));
  
    // but eps needs modifiable vectors so we copy them
    for (unsigned int k = 0; k < n; k++) {
      petsc_call(VecDuplicate(vecs[k], &rigid[k]));
      petsc_call(VecCopy(vecs[k], rigid[k]));
    }
    
    petsc_call(EPSSetDeflationSpace(eps, n, rigid));
  }   
  
  return FEENOX_OK;
}
#endif
