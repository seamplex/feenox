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
 *  feenox is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with feenox.  If not, see <http://www.gnu.org/licenses/>.
 *------------------- ------------  ----    --------  --     -       -         -
 */
#include "feenox.h"
extern feenox_t feenox;

int feenox_problem_gradient_compute(void) {
  
  // the mesh for "rough" mode is different
  mesh_t *mesh = (feenox.pde.rough == 0) ? feenox.pde.mesh : feenox.pde.mesh_rough;
  
  // number of steps we need to make for progress bar
  size_t step = ceil((double)(mesh->n_elements + mesh->n_nodes)*feenox.n_procs/100.0);
  if (step < 1) {
    step = 1;
  }
  
  unsigned int g = 0;
  unsigned int m = 0;
  if (feenox.pde.gradient[0][0]->data_value == NULL) {
    // gradient vector
    for (g = 0; g < feenox.pde.dofs; g++) {
      for (m = 0; m < feenox.pde.dim; m++) {
        // derivative of the degree of freedom g with respect to dimension m
        feenox_gradient_fill(feenox.pde, gradient[g][m]);
        feenox_gradient_fill(feenox.pde, delta_gradient[g][m]);
      }
    }
        
    // TODO: per problem
    feenox_call(feenox_problem_gradient_fill_thermal());
  }
  
  // step 1. sweep elements and compute tensors at each node of each element
  size_t progress = 0;
  unsigned int ascii_progress_chars = 0;
  size_t i = 0;
  for (i = 0; i < mesh->n_elements; i++) {
    if ((feenox.pde.progress_ascii == PETSC_TRUE) && (progress++ % step) == 0) {
      printf(CHAR_PROGRESS_GRADIENT);  
      fflush(stdout);
      ascii_progress_chars++;
    }
    
    if (mesh->element[i].type->dim == feenox.pde.dim) {
      feenox_call(feenox_problem_gradient_compute_at_element(&mesh->element[i], mesh));
      // TODO: per problem
      // TODO: this is only needed in rough
//      feenox_call(feenox_problem_gradient_properties_at_element_nodes_thermal(&mesh->element[i], mesh));
    }
    

  }
  
  // step 2. sweep nodes of the output mesh (same as input in smooth, rough in rough)
  size_t j = 0;
  for (j = 0; j < mesh->n_nodes; j++) {
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
        feenox.pde.gradient[g][m]->data_value[j] = gsl_matrix_get(mesh->node[j].dphidx, g, m);
        feenox.pde.delta_gradient[g][m]->data_value[j] = gsl_matrix_get(mesh->node[j].delta_dphidx, g, m);
      }
    }
    
    feenox_call(feenox_problem_fill_fluxes(mesh, j));
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
  
  
  return FEENOX_OK;
}



int feenox_problem_gradient_compute_at_element(element_t *this, mesh_t *mesh) {
  
  unsigned int V = this->type->gauss[mesh->integration].V;
  unsigned int J = this->type->nodes;
      
  // if already alloced, no need to realloc
  
  if (this->dphidx_node == NULL) {
    feenox_check_alloc(this->dphidx_node = calloc(J, sizeof(gsl_matrix *)));
    unsigned int j = 0;
    for (j = 0; j < J; j++) {
      feenox_check_alloc(this->dphidx_node[j] = gsl_matrix_calloc(feenox.pde.dofs, feenox.pde.dim));
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
    this->gradient_weight = 1;
  }
        
  // if we were asked to extrapolate from gauss, we compute all the nodal values
  // at once by left-multiplying the gauss values by the (possibly-rectangular) 

  unsigned int v = 0;
  unsigned int g = 0;
  unsigned int m = 0;
  unsigned int j = 0;
  size_t j_global = 0;
  
  if (feenox.pde.gradient_evaluation == gradient_gauss_extrapolated && this->type->gauss[mesh->integration].extrap != NULL) {
    
    // extrapolation matrix to get the nodal values
    gsl_vector *at_gauss = NULL;
    feenox_check_alloc(at_gauss = gsl_vector_alloc(V));
    gsl_vector *at_nodes = NULL;
    feenox_check_alloc(at_nodes = gsl_vector_alloc(J));
    if (this->dphidx_gauss == NULL) {
      feenox_check_alloc(this->dphidx_gauss = calloc(V, sizeof(gsl_matrix *)));
    }  
    
    for (v = 0; v < V; v++) {
    
      if (this->dphidx_gauss[v] == NULL) {
        feenox_check_alloc(this->dphidx_gauss[v] = gsl_matrix_calloc(feenox.pde.dofs, feenox.pde.dim));
      } else {
        gsl_matrix_set_zero(this->dphidx_gauss[v]);
      }
      feenox_call(feenox_mesh_compute_dhdx_at_gauss(this, v, mesh->integration));

      // aca habria que hacer una matriz con los phi globales
      // (de j y g, que de paso no depende de v asi que se podria hacer afuera del for de v)
      // y ver como calcular la matriz dphidx como producto de dhdx y esta matriz
      for (g = 0; g < feenox.pde.dofs; g++) {
        for (m = 0; m < feenox.pde.dim; m++) {
          for (j = 0; j < this->type->nodes; j++) {
            j_global = this->node[j]->index_mesh;
            gsl_matrix_add_to_element(this->dphidx_gauss[v], g, m, gsl_matrix_get(this->dhdx[v], j, m) * mesh->node[j_global].phi[g]);
          }
        }
      }
    }
    
    // take the product of the extrapolation matrix times the values at the gauss points
    for (g = 0; g < feenox.pde.dofs; g++) {
      for (m = 0; m < feenox.pde.dim; m++) {
        for (v = 0; v < V; v++) {
          gsl_vector_set(at_gauss, v, gsl_matrix_get(this->dphidx_gauss[v], g, m));
        }  
        
        gsl_blas_dgemv(CblasNoTrans, 1.0, this->type->gauss[mesh->integration].extrap, at_gauss, 0, at_nodes);
        for (j = 0; j < J; j++) {
          gsl_matrix_set(this->dphidx_node[j], g, m, gsl_vector_get(at_nodes, j));
        }
      }
    }
    gsl_vector_free(at_gauss);
    gsl_vector_free(at_nodes);
    
  } else {

    
    for (j = 0; j < J; j++) {
      j_global = this->node[j]->index_mesh;
    
      if (this->type->node_parents != NULL && this->type->node_parents[j] != NULL && feenox.pde.gradient_highorder_nodes == gradient_average) {
        // average of parents
        node_relative_t *parent = NULL;
        double den = 0;
        LL_FOREACH(this->type->node_parents[j], parent) {
          den += 1.0;
          for (g = 0; g < feenox.pde.dofs; g++) {
            for (m = 0; m < feenox.pde.dim; m++) {
              gsl_matrix_add_to_element(this->dphidx_node[j], g, m, gsl_matrix_get(this->dphidx_node[parent->index], g, m));
            }  
          }
        }  
        gsl_matrix_scale(this->dphidx_node[j], 1.0/den);          
      
      } else {
        
        // direct evalution at the nodes
        gsl_matrix *dhdx = NULL;
        feenox_check_alloc(dhdx = gsl_matrix_calloc(J, feenox.pde.dim));
        feenox_call(feenox_mesh_compute_dhdx(this, this->type->node_coords[j], NULL, dhdx));
      
        // the derivatives of each dof g with respect to the coordinate mas nueve derivadas (o menos)
        // TODO: como arriba, aunque hay que pelar ojo si hay menos DOFs
        unsigned int j_local_prime = 0;
        size_t j_global_prime = 0;
        for (g = 0; g < feenox.pde.dofs; g++) {
          for (m = 0; m < feenox.pde.dim; m++) {
            for (j_local_prime = 0; j_local_prime < J; j_local_prime++) {
              j_global_prime = this->node[j_local_prime]->index_mesh;
              gsl_matrix_add_to_element(this->dphidx_node[j], g, m, gsl_matrix_get(dhdx, j_local_prime, m) * mesh->node[j_global_prime].phi[g]);
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
  gsl_matrix *m2 = gsl_matrix_calloc(feenox.pde.dofs, feenox.pde.dim);
  int found = 0;
      
  if (node->dphidx == NULL) {
    feenox_check_alloc(node->dphidx = gsl_matrix_calloc(feenox.pde.dofs, feenox.pde.dim));
  } else {
    gsl_matrix_set_zero(node->dphidx);
  }  
  if (node->delta_dphidx == NULL) {
    node->delta_dphidx = gsl_matrix_calloc(feenox.pde.dofs, feenox.pde.dim);
  } else {
    gsl_matrix_set_zero(node->delta_dphidx);
  }
  // TODO: per-problem
  feenox_call(feenox_problem_gradient_fluxes_at_node_alloc_thermal(node));
      
  size_t j = 0;
  unsigned int g = 0;
  unsigned int m = 0;
  unsigned int n = 0;
  element_t *element = NULL;
  element_ll_t *associated_element = NULL;
  LL_FOREACH(node->associated_elements, associated_element) {
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
              gsl_matrix_add_to_element(m2, g, m, element->gradient_weight * delta * ((*current)-(*mean)));
              gsl_matrix_set(node->delta_dphidx, g, m, sqrt(gsl_matrix_get(m2, g, m)/sum_weight));
            }
          }

          // TODO: per-problem stuff
          feenox_call(feenox_problem_gradient_add_elemental_contribution_to_node_thermal(node, element, j, rel_weight));
        }
      }
    }
  }
      
  gsl_matrix_free(m2);
  
  return FEENOX_OK;
      
}
