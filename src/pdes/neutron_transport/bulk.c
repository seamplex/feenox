/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for neutron transport FEM: bulk elements
 *
 *  Copyright (C) 2023 jeremy theler
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
#include "neutron_transport.h"

int feenox_problem_build_volumetric_gauss_point_neutron_transport(element_t *this, unsigned int v) {

#ifdef HAVE_PETSC
  
/*  

  if (Source == NULL) {
    Source = malloc(milonga.groups * sizeof(double));
    SigmaA = malloc(milonga.groups * sizeof(double));
    SigmaT = malloc(milonga.groups * sizeof(double));
    SigmaS0 = malloc(milonga.groups * sizeof(double *));
    SigmaS1 = malloc(milonga.groups * sizeof(double *));
    chinuSigmaF = malloc(milonga.groups * sizeof(double *));
    for (g = 0; g < milonga.groups; g++) {
      SigmaS0[g] = malloc(milonga.groups * sizeof(double));
      SigmaS1[g] = malloc(milonga.groups * sizeof(double));
      chinuSigmaF[g] = malloc(milonga.groups * sizeof(double));
    }
  }
  

  if (element->physical_entity == NULL) {
    wasora_push_error_message("element %d needs a physical entity", element->tag);
    PetscFunctionReturn(WASORA_RUNTIME_ERROR);
  } else if (element->physical_entity->material == NULL) {
    wasora_push_error_message("physical entity '%s' needs a material", element->physical_entity->material);
    PetscFunctionReturn(WASORA_RUNTIME_ERROR);
  }

  material_xs = (xs_t *)(element->physical_entity->material->ext);

  if (J != element->type->nodes) {
    wasora_call(sn_elements_allocate_particular_elemental_objects(element));
  }

  // inicializar Ki Ai Xi Si <- 0
  gsl_matrix_set_zero(Ki);
  gsl_matrix_set_zero(Ai);
  gsl_matrix_set_zero(Xi);
  gsl_vector_set_zero(Si);

  // factor de estabilizacion
  tau = wasora_var(milonga.vars.sn_alpha) * 0.5 * gsl_hypot3(element->node[1]->x[0]-element->node[0]->x[0],
                                                             element->node[1]->x[1]-element->node[0]->x[1],
                                                             element->node[1]->x[2]-element->node[0]->x[2]);
  
  // para cada punto de gauss
  for (v = 0; v < element->type->gauss[GAUSS_POINTS_CANONICAL].V; v++) {

    // para este punto de gauss, calculamos las matrices H y B
    w_gauss = mesh_compute_fem_objects_at_gauss(wasora_mesh.main_mesh, element, v);
    
    // la estabilizacion de petrov
    for (j = 0; j < element->type->nodes; j++) {
      xi = element->type->h(j, wasora_mesh.main_mesh->fem.r);
      for (m = 0; m < milonga.directions; m++) {
        for (g = 0; g < milonga.groups; g++) {
          // parte base, igual a las h
          gsl_matrix_set(P, dof_index(m,g), milonga.directions*milonga.groups*j + dof_index(m,g), xi);
          // correccion
          for (n = 0; n < milonga.dimensions; n++) {
            gsl_matrix_add_to_element(P, dof_index(m,g), milonga.directions*milonga.groups*j + dof_index(m,g), 
                               tau * Omega[m][n] * gsl_matrix_get(wasora_mesh.main_mesh->fem.dhdx, j, n));
          }
        }
      }
    }
    
    
    // inicializamos las matrices con las XS (estas si dependen de la formulacion)
    gsl_matrix_set_zero(A);
    gsl_matrix_set_zero(X);
    gsl_vector_set_zero(S);

    for (g = 0; g < milonga.groups; g++) {
     
      if (material_xs->S[g]->n_tokens != 0) {
        if ((Source[g] = wasora_evaluate_expression(material_xs->S[g])) != 0) {
          milonga.has_sources = 1;
          this_element_has_sources = 1;
        } else {
          Source[g] = 0;
        }
      }
      if (material_xs->SigmaA[g]->n_tokens != 0) {
        SigmaA[g] = wasora_evaluate_expression(material_xs->SigmaA[g]);
      } else {
        SigmaA[g] = 0;
      }
      if (material_xs->SigmaT[g]->n_tokens != 0) {
        SigmaT[g] = wasora_evaluate_expression(material_xs->SigmaT[g]);
      } else {
        SigmaT[g] = 0;
      }
      
      for (g_prime = 0; g_prime < milonga.groups; g_prime++) {
        if (material_xs->SigmaS0[g][g_prime]->n_tokens != 0) {
          SigmaS0[g][g_prime] = wasora_evaluate_expression(material_xs->SigmaS0[g][g_prime]);
        } else {
          SigmaS0[g][g_prime] = 0;
        }
        if (material_xs->SigmaS1[g][g_prime]->n_tokens != 0) {
          SigmaS1[g][g_prime] = wasora_evaluate_expression(material_xs->SigmaS1[g][g_prime]);
        } else {
          SigmaS1[g][g_prime] = 0;
        }
        if (material_xs->nuSigmaF[g]->n_tokens != 0 && gsl_vector_get(wasora_value_ptr(milonga.vectors.chi), g_prime) != 0) {
          chinuSigmaF[g][g_prime] = gsl_vector_get(wasora_value_ptr(milonga.vectors.chi), g_prime) * wasora_evaluate_expression(material_xs->nuSigmaF[g]);
          this_element_has_fission = 1;
          milonga.has_fission = 1;
        } else {
          chinuSigmaF[g][g_prime] = 0;
        }
      }
    }
       
      
    for (m = 0; m < milonga.directions; m++) {
      for (g = 0; g < milonga.groups; g++) {
        gsl_vector_set(S, dof_index(m,g), Source[g]);
        
        // scattering y fision
        for (g_prime = 0; g_prime < milonga.groups; g_prime++) {
          for (m_prime = 0; m_prime < milonga.directions; m_prime++) {
            // scattering
            xi = -w[m_prime] * SigmaS0[g_prime][g];
            // si tenemos scattering anisotropico, l = 1
            if (material_xs->SigmaS1[g_prime][g]->n_tokens != 0) {
              xi -= w[m_prime] * SigmaS1[g_prime][g] * 3.0 * mesh_dot(Omega[m], Omega[m_prime]);
            }
            gsl_matrix_set(A, dof_index(m,g), dof_index(m_prime,g_prime), xi);

            // fision
            gsl_matrix_set(X, dof_index(m,g), dof_index(m_prime,g_prime), +w[m_prime] * chinuSigmaF[g_prime][g]);
          }
        }

        // absorcion
        xi = gsl_matrix_get(A, dof_index(m,g), dof_index(m,g));
        if (SigmaT[g] != 0) {
          xi += SigmaT[g];
        } else {
          xi += SigmaA[g];
          for (g_prime = 0; g_prime < milonga.groups; g_prime++) {
            xi += SigmaS0[g][g_prime];
          }
        }
        gsl_matrix_set(A, dof_index(m,g), dof_index(m,g), xi);
      }
    }
    
    // armamos la matriz elemental del termino de fugas (estabilizada con petrov)
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1, OMEGA, wasora_mesh.main_mesh->fem.B, 0, OMEGAB);
    gsl_blas_dgemm(CblasTrans, CblasNoTrans, w_gauss, P, OMEGAB, 1, Ki);    

    // la matriz elemental de scattering (estabilizada con petrov)
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1, A, wasora_mesh.main_mesh->fem.H, 0, AH);
    gsl_blas_dgemm(CblasTrans, CblasNoTrans, w_gauss, P, AH, 1, Ai);
    
    // la matriz elemental de fision (estabilizada con petrov)
    if (this_element_has_fission) {
      gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1, X, wasora_mesh.main_mesh->fem.H, 0, XH);
      gsl_blas_dgemm(CblasTrans, CblasNoTrans, w_gauss, P, XH, 1, Xi);
    }
    // el vector elemental de fuentes (estabilizado con petrov)
    if (this_element_has_sources) {
      gsl_blas_dgemv(CblasTrans, w_gauss, P, S, 1, Si);
    }
  }
  
  petsc_call(MatSetValues(milonga.R, L, wasora_mesh.main_mesh->fem.l, L, wasora_mesh.main_mesh->fem.l, gsl_matrix_ptr(Ki, 0, 0), ADD_VALUES));
  petsc_call(MatSetValues(milonga.R, L, wasora_mesh.main_mesh->fem.l, L, wasora_mesh.main_mesh->fem.l, gsl_matrix_ptr(Ai, 0, 0), ADD_VALUES));  
  if (this_element_has_fission) {
    petsc_call(MatSetValues(milonga.F, L, wasora_mesh.main_mesh->fem.l, L, wasora_mesh.main_mesh->fem.l, gsl_matrix_ptr(Xi, 0, 0), ADD_VALUES));
  }
  if (this_element_has_sources) {
    petsc_call(VecSetValues(milonga.S, L, wasora_mesh.main_mesh->fem.l, gsl_vector_ptr(Si, 0), ADD_VALUES));
  }
*/  
#endif
  
  return FEENOX_OK;
  
}

