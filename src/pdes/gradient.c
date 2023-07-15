/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for computing the gradients of the PDE solutions
 *
 *  Copyright (C) 2021 jeremy theler
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

int feenox_problem_gradient_compute(void) {
  
#ifdef HAVE_PETSC
  
  if (feenox.pde.m2 == NULL) {
    feenox_check_alloc(feenox.pde.m2 = gsl_matrix_calloc(feenox.pde.dofs, feenox.pde.dim));
  }
  
  // the mesh for "rough" mode is different
  mesh_t *mesh = (feenox.pde.rough == 0) ? feenox.pde.mesh : feenox.pde.mesh_rough;
  
  // number of steps we need to make for progress bar
  size_t step = ceil((double)(mesh->n_elements + mesh->n_nodes)*feenox.n_procs/100.0);
  if (step < 1) {
    step = 1;
  }
  
  unsigned int g = 0;
  unsigned int m = 0;
  if (feenox.pde.gradient[0][0]->vector_value == NULL) {
    // gradient vector
    for (g = 0; g < feenox.pde.dofs; g++) {
      for (m = 0; m < feenox.pde.dim; m++) {
        // derivative of the degree of freedom g with respect to dimension m
        feenox_problem_fill_aux_solution(feenox.pde.gradient[g][m]);
//        feenox_gradient_fill(feenox.pde, delta_gradient[g][m]);
      }
    }
        
    if (feenox.pde.feenox_problem_gradient_fill != NULL) {
      feenox_call(feenox.pde.feenox_problem_gradient_fill());
    }  
  }
  
  // step 1. sweep elements and compute tensors at each node of each element
  size_t progress = 0;
  unsigned int ascii_progress_chars = 0;
  for (size_t i = 0; i < mesh->n_elements; i++) {
    if ((feenox.pde.progress_ascii == PETSC_TRUE) && (progress++ % step) == 0) {
      printf(CHAR_PROGRESS_GRADIENT);  
      fflush(stdout);
      ascii_progress_chars++;
    }
    
    if (mesh->element[i].type->dim == feenox.pde.dim) {
      feenox_call(feenox_problem_gradient_compute_at_element(&mesh->element[i], mesh));
    }
  }
  
  // step 2. sweep nodes of the output mesh (same as input in smooth, rough in rough)
  for (size_t j = 0; j < mesh->n_nodes; j++) {
    if ((feenox.pde.progress_ascii == PETSC_TRUE) && (progress++ % step) == 0) {
      printf(CHAR_PROGRESS_GRADIENT);  
      fflush(stdout);
      ascii_progress_chars++;
    }

    if (feenox.pde.rough == 0) {    
      feenox_call(feenox_problem_gradient_smooth_at_node(&mesh->node[j]));
    }  
    
    // we now have the averages, now fill in the functions
    for (g = 0; g < feenox.pde.dofs; g++) {
      for (m = 0; m < feenox.pde.dim; m++) {
        feenox_vector_set(feenox.pde.gradient[g][m]->vector_value, j, gsl_matrix_get(mesh->node[j].dphidx, g, m));
//        feenox.pde.delta_gradient[g][m]->data_value[j] = gsl_matrix_get(mesh->node[j].delta_dphidx, g, m);
      }
    }
    
    if (feenox.pde.feenox_problem_gradient_fill_fluxes != NULL) {
      feenox_call(feenox.pde.feenox_problem_gradient_fill_fluxes(mesh, j));
    }  
  }
  
  // TODO: put 100 as a define or as a variable
  if (feenox.pde.progress_ascii == PETSC_TRUE) {  
    if (feenox.n_procs == 1) {
      while (ascii_progress_chars++ < 100) {
        printf(CHAR_PROGRESS_GRADIENT);
      }
    }
    if (feenox.rank == 0) {
      printf("\n");  
      fflush(stdout);
    }  
  }  
  
#endif

  return FEENOX_OK;
}



int feenox_problem_gradient_compute_at_element(element_t *e, mesh_t *mesh) {
  
  unsigned int Q = e->type->gauss[mesh->integration].Q;
  unsigned int J = e->type->nodes;
      
  // if already alloced, no need to realloc
  
  if (e->dphidx_node == NULL) {
    feenox_check_alloc(e->dphidx_node = calloc(J, sizeof(gsl_matrix *)));
    unsigned int j = 0;
    for (j = 0; j < J; j++) {
      feenox_check_alloc(e->dphidx_node[j] = gsl_matrix_calloc(feenox.pde.dofs, feenox.pde.dim));
    }  
  }

  if (feenox.pde.rough == 0) {
    // TODO: virtual methods
/*    
    if (feenox.pde.gradient_element_weight == gradient_weight_volume) {
      this->type->element_volume(this);
      this->weight = this->volume;
    } else if (fino.gradient_element_weight == gradient_weight_quality) {
      mesh_compute_quality(mesh, this);
      this->weight = this->quality;
    } else if (fino.gradient_element_weight == gradient_weight_volume_times_quality) {
      this->type->element_volume(this);
      mesh_compute_quality(mesh, this);
      this->weight = this->volume*GSL_MAX(this->quality, 1);
    } else if (fino.gradient_element_weight == gradient_weight_flat) {
      element->weight = 1;
    }
*/
    e->gradient_weight = 1;
  }
        
  // if we were asked to extrapolate from gauss, we compute all the nodal values
  // at once by left-multiplying the gauss values by the (possibly-rectangular) 
  size_t j_global = 0;
  
//  feenox.pde.gradient_evaluation = gradient_at_nodes;
  
  if (feenox.pde.gradient_evaluation == gradient_gauss_extrapolated && e->type->gauss[mesh->integration].extrap != NULL) {
    
    // extrapolation matrix to get the nodal values
    gsl_vector *at_gauss = NULL;
    feenox_check_alloc(at_gauss = gsl_vector_alloc(Q));
    gsl_vector *at_nodes = NULL;
    feenox_check_alloc(at_nodes = gsl_vector_alloc(J));
    if (e->dphidx_gauss == NULL) {
      feenox_check_alloc(e->dphidx_gauss = calloc(Q, sizeof(gsl_matrix *)));
    }  
    
    for (unsigned int q = 0; q < Q; q++) {
    
      if (e->dphidx_gauss[q] == NULL) {
        feenox_check_alloc(e->dphidx_gauss[q] = gsl_matrix_calloc(feenox.pde.dofs, feenox.pde.dim));
      } else {
        gsl_matrix_set_zero(e->dphidx_gauss[q]);
      }
      feenox_call(feenox_mesh_compute_B_at_gauss(e, q, mesh->integration));

      // aca habria que hacer una matriz con los phi globales
      // (de j y g, que de paso no depende de q asi que se podria hacer afuera del for de q)
      // y ver como calcular la matriz dphidx como producto de dhdx y esta matriz
      for (unsigned int g = 0; g < feenox.pde.dofs; g++) {
        for (unsigned int d = 0; d < feenox.pde.dim; d++) {
          for (unsigned int j = 0; j < e->type->nodes; j++) {
            j_global = e->node[j]->index_mesh;
            gsl_matrix_add_to_element(e->dphidx_gauss[q], g, d, gsl_matrix_get(e->B[q], d, j) * mesh->node[j_global].phi[g]);
          }
        }
      }
    }
    
    // take the product of the extrapolation matrix times the values at the gauss points
    for (unsigned int g = 0; g < feenox.pde.dofs; g++) {
      for (unsigned int d = 0; d < feenox.pde.dim; d++) {
        for (unsigned int q = 0; q < Q; q++) {
          gsl_vector_set(at_gauss, q, gsl_matrix_get(e->dphidx_gauss[q], g, d));
        }  
        
        gsl_blas_dgemv(CblasNoTrans, 1.0, e->type->gauss[mesh->integration].extrap, at_gauss, 0, at_nodes);
        for (unsigned int j = 0; j < J; j++) {
          gsl_matrix_set(e->dphidx_node[j], g, d, gsl_vector_get(at_nodes, j));
        }
      }
    }
    gsl_vector_free(at_gauss);
    gsl_vector_free(at_nodes);
    
  } else {

    
    for (unsigned int j = 0; j < J; j++) {
      j_global = e->node[j]->index_mesh;
    
      if (e->type->node_parents != NULL && e->type->node_parents[j] != NULL && feenox.pde.gradient_highorder_nodes == gradient_average) {
        // average of parents
        node_relative_t *parent = NULL;
        double den = 0;
        LL_FOREACH(e->type->node_parents[j], parent) {
          den += 1.0;
          for (unsigned int g = 0; g < feenox.pde.dofs; g++) {
            for (unsigned int d = 0; d < feenox.pde.dim; d++) {
              gsl_matrix_add_to_element(e->dphidx_node[j], g, d, gsl_matrix_get(e->dphidx_node[parent->index], g, d));
            }  
          }
        }  
        gsl_matrix_scale(e->dphidx_node[j], 1.0/den);          
      
      } else {
        
        // direct evalution at the nodes
        gsl_matrix *dhdx = NULL;
        feenox_check_alloc(dhdx = gsl_matrix_calloc(J, feenox.pde.dim));
        feenox_call(feenox_mesh_compute_B(e, e->type->node_coords[j], NULL, dhdx));
      
        // the derivatives of each dof g with respect to the coordinate mas nueve derivadas (o menos)
        // TODO: como arriba, aunque hay que pelar ojo si hay menos DOFs
        size_t j_global_prime = 0;
        for (unsigned int g = 0; g < feenox.pde.dofs; g++) {
          for (unsigned int d = 0; d < feenox.pde.dim; d++) {
            for (unsigned int j_local_prime = 0; j_local_prime < J; j_local_prime++) {
              j_global_prime = e->node[j_local_prime]->index_mesh;
              gsl_matrix_add_to_element(e->dphidx_node[j], g, d, gsl_matrix_get(dhdx, j_local_prime, d) * mesh->node[j_global_prime].phi[g]);
            }
          }
        }
        gsl_matrix_free(dhdx);
      }
    }
  }
  
  return FEENOX_OK;
  
}



int feenox_problem_gradient_smooth_at_node(node_t *node) {
  
  double *mean = NULL;
  double *current = NULL;
  double delta = 0;
  double sum_weight = 0;
  double rel_weight = 0;
  int found = 0;
      
  if (node->dphidx == NULL) {
    feenox_check_alloc(node->dphidx = gsl_matrix_calloc(feenox.pde.dofs, feenox.pde.dim));
  } else {
    gsl_matrix_set_zero(node->dphidx);
  }  
/*  
  if (node->delta_dphidx == NULL) {
    node->delta_dphidx = gsl_matrix_calloc(feenox.pde.dofs, feenox.pde.dim);
  } else {
    gsl_matrix_set_zero(node->delta_dphidx);
  }
*/
  
  if (feenox.pde.feenox_problem_gradient_fluxes_at_node_alloc != NULL) {
    feenox_call(feenox.pde.feenox_problem_gradient_fluxes_at_node_alloc(node));
  }  
      
  size_t j = 0;
  unsigned int g = 0;
  unsigned int m = 0;
  unsigned int n = 0;
  element_t *element = NULL;
  element_ll_t *associated_element = NULL;
  gsl_matrix_set_zero(feenox.pde.m2);
  LL_FOREACH(node->element_list, associated_element) {
    element = associated_element->element;
    if (element->dphidx_node != NULL) {
      found = 0;
      for (j = 0; !found && j < element->type->nodes; j++) {
        if (element->node[j] == node) {

          n++;
          found = 1;
          sum_weight += element->gradient_weight;
          rel_weight = element->gradient_weight / sum_weight;
              
          // derivative and uncertainties according to weldford
          for (g = 0; g < feenox.pde.dofs; g++) {
            for (m = 0; m < feenox.pde.dim; m++) {
              mean = gsl_matrix_ptr(node->dphidx, g, m);
              current = gsl_matrix_ptr(element->dphidx_node[j], g, m);
              delta = *current - *mean;
              *mean += rel_weight * delta;
              gsl_matrix_add_to_element(feenox.pde.m2, g, m, element->gradient_weight * delta * ((*current)-(*mean)));
//              gsl_matrix_set(node->delta_dphidx, g, m, sqrt(gsl_matrix_get(feenox.pde.m2, g, m)/sum_weight));
            }
          }

          if (feenox.pde.feenox_problem_gradient_add_elemental_contribution_to_node != NULL) {
            feenox_call(feenox.pde.feenox_problem_gradient_add_elemental_contribution_to_node(node, element, j, rel_weight));
          }  
        }
      }
    }
  }
  
  return FEENOX_OK;
      
}
