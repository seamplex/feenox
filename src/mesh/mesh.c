/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's mesh-related routines
 *
 *  Copyright (C) 2014--2021 jeremy theler
 *
 *  This file is part of feenox.
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
#include "../feenox.h"
extern feenox_t feenox;

int feenox_instruction_mesh_read(void *arg) {

  mesh_t *this = (mesh_t *)arg;
  
  if (this->initialized) {
    if (this->update_each_step == 0) {
      // we are not supposed to read the mesh all over again
      return FEENOX_OK;
    } else {
      // we have to re-read the mesh, but we already have one
      // so we get need to get rid of the current one
      feenox_mesh_free(this);
    }
  }
  
  // read the actual mesh with a format-dependent reader (who needs C++?)
  feenox_call(this->reader(this));
  
  // sweep nodes ande define the bounding box
  // TODO: see if this can go inside one of the kd loops
  this->bounding_box_min.index_mesh = SIZE_MAX;
  this->bounding_box_max.index_mesh = SIZE_MAX;
  this->bounding_box_min.index_dof = NULL;
  this->bounding_box_max.index_dof = NULL;
  this->bounding_box_min.associated_elements = NULL;
  this->bounding_box_max.associated_elements = NULL;
  
  double scale_factor = feenox_expression_eval(&this->scale_factor);
  double offset[3];
  offset[0] = feenox_expression_eval(&this->offset_x);
  offset[1] = feenox_expression_eval(&this->offset_y);
  offset[2] = feenox_expression_eval(&this->offset_z);

  double x_max[3] = {-MESH_INF, -MESH_INF, -MESH_INF};
  double x_min[3] = {+MESH_INF, +MESH_INF, +MESH_INF};

  unsigned int dim = 0;
  for (size_t j = 0; j < this->n_nodes; j++) {
    for (unsigned int m = 0; m < 3; m++) {
      if (scale_factor != 0 || offset[m] != 0) {
        this->node[j].x[m] -= offset[m];
        this->node[j].x[m] *= scale_factor;
      }
      
      if (dim < 1 && fabs(this->node[j].x[0]) > MESH_TOL) {
        dim = 1;
      }
      if (dim < 2 && fabs(this->node[j].x[1]) > MESH_TOL) {
        dim = 2;
      }
      if (dim < 3 && fabs(this->node[j].x[2]) > MESH_TOL) {
        dim = 3;
      }
      
      if (this->node[j].x[m] < x_min[m]) {
        x_min[m] = this->bounding_box_min.x[m] = this->node[j].x[m];
      }
      if (this->node[j].x[m] > x_max[m]) {
        x_max[m] = this->bounding_box_max.x[m] = this->node[j].x[m];
      }
    }
  }
  
  feenox_call(feenox_vector_init(feenox.mesh.vars.bbox_min, 1));
  gsl_vector_set(feenox.mesh.vars.bbox_min->value, 0, x_min[0]);
  gsl_vector_set(feenox.mesh.vars.bbox_min->value, 1, x_min[1]);
  gsl_vector_set(feenox.mesh.vars.bbox_min->value, 2, x_min[2]);

  feenox_call(feenox_vector_init(feenox.mesh.vars.bbox_max, 1));
  gsl_vector_set(feenox.mesh.vars.bbox_max->value, 0, x_max[0]);
  gsl_vector_set(feenox.mesh.vars.bbox_max->value, 1, x_max[1]);
  gsl_vector_set(feenox.mesh.vars.bbox_max->value, 2, x_max[2]);
  
  // allocate arrays for the elements that belong to a physical group
  // (arrays are more efficient than a linked list)
  physical_group_t *physical_group;
  for (physical_group = this->physical_groups; physical_group != NULL; physical_group = physical_group->hh.next) {
    if (physical_group->n_elements != 0) {
      feenox_check_alloc(physical_group->element = calloc(physical_group->n_elements, sizeof(size_t)));
    }
    // check out what the highest tag is to allocate temporary arrays
    if (physical_group->tag > this->physical_tag_max) {
      this->physical_tag_max = physical_group->tag;
    }
  }
  
  for (size_t i = 0; i < this->n_elements; i++) {
    
    // check the dimension of the element, the higher is the topological dim of the mes
    if (this->element[i].type->dim > this->dim_topo) {
      this->dim_topo = this->element[i].type->dim;
    }

    // same for order
    if (this->element[i].type->order > this->order) {
      this->order = this->element[i].type->order;
    }

    // nos acordamos del elemento que tenga el mayor numero de nodos
    if (this->element[i].type->nodes > this->max_nodes_per_element) {
      this->max_nodes_per_element = this->element[i].type->nodes;
    }

    // y del que tenga mayor cantidad de vecinos
    if (this->element[i].type->faces > this->max_faces_per_element) {
      this->max_faces_per_element = this->element[i].type->faces;
    }

    // armamos la lista de elementos de cada entidad
    physical_group = this->element[i].physical_group;
    if (physical_group != NULL && physical_group->i_element < physical_group->n_elements) {
      physical_group->element[physical_group->i_element++] = i;
    }
  }
 
  // check the dimensions match
  if (this->dim == 0) {
    this->dim = dim;
  } else if (this->dim != dim) {
    feenox_push_error_message("mesh '%s' has DIMENSION %d but the highest-dimensional element has %d", this->file->name, this->dim, dim);
    return FEENOX_ERROR;
  }
    
  // fill an array of nodes that can be used as arguments of functions
  if (this->nodes_argument == NULL) {
    feenox_call(feenox_mesh_create_nodes_argument(this));
  }
  
  // see if we need to create cells and allocate space for arguments
  if (feenox.mesh.need_cells) {
    feenox_call(feenox_mesh_element2cell(this));
    feenox_check_alloc(this->cells_argument = calloc(this->dim, sizeof(double *)));
    for (unsigned int m = 0; m < this->dim; m++) {
      feenox_check_alloc(this->cells_argument[m] = calloc(this->n_cells, sizeof(double)));
      for (size_t i = 0; i < this->n_cells; i++) {
        this->cells_argument[m][i] = this->cell[i].x[m]; 
      }
    }
  }

  if (this->n_cells == 0) {
    this->n_cells = this->n_elements_per_dim[this->dim];
  }
  
  if (this == feenox.mesh.mesh_main) {
    feenox_var_value(feenox.mesh.vars.cells) = (double)this->n_cells;
    feenox_var_value(feenox.mesh.vars.nodes) = (double)this->n_nodes;
    feenox_var_value(feenox.mesh.vars.elements) = (double)this->n_elements;
  }
  
  // see if there was any un-read function
  node_data_t *node_data;
  LL_FOREACH(this->node_datas, node_data) {
    if (node_data->function->mesh == NULL) {
      feenox_push_error_message("cannot find data for function '%s' in mesh '%s'", node_data->name_in_mesh, this->file->name);
      return FEENOX_ERROR;
    }
  }
  
  // compute the volume (or area or length) and center of gravity of the groups
  // but only if the variable groupname_vol or the vector groupname_cog 
  // are used in one of the expressions
  if (this->dim != 0) {
    for (physical_group = this->physical_groups; physical_group != NULL; physical_group = physical_group->hh.next) {
      
      if ((physical_group->var_volume != NULL && physical_group->var_volume->used) ||
          (physical_group->vector_cog != NULL && physical_group->vector_cog->used)) {
        physical_group->volume = 0;
        physical_group->cog[0] = 0;
        physical_group->cog[1] = 0;
        physical_group->cog[2] = 0;
        
        for (size_t i = 0; i < physical_group->n_elements; i++) {
          element_t *element = &this->element[physical_group->element[i]];
          for (unsigned int v = 0; v < element->type->gauss[this->integration].V; v++) {
            feenox_call(feenox_mesh_compute_w_at_gauss(element, v, this->integration));

            for (size_t j = 0; j < element->type->nodes; j++) {
              double wh = element->w[v] * element->type->gauss[this->integration].h[v][j];
              physical_group->volume += wh ;
              for (size_t m = 0; m < 3; m++) {
                physical_group->cog[m] += wh * element->node[j]->x[m];
              }
            }
          }
        }
        
        feenox_var_value(physical_group->var_volume) = physical_group->volume;
        
        if (physical_group->vector_cog->initialized == 0) {
          feenox_call(feenox_vector_init(physical_group->vector_cog, 0));
        }
        gsl_vector_set(physical_group->vector_cog->value, 0, physical_group->cog[0]);
        gsl_vector_set(physical_group->vector_cog->value, 1, physical_group->cog[1]);
        gsl_vector_set(physical_group->vector_cog->value, 2, physical_group->cog[2]);
      }
    }
  }

  // create a k-dimensional tree and try to figure out what the maximum number of neighbours each node has
  if (this->kd_nodes == NULL) {
    this->kd_nodes = kd_create(this->dim);
    for (size_t j = 0; j < this->n_nodes; j++) {
      kd_insert(this->kd_nodes, this->node[j].x, &this->node[j]);
    
      size_t first_neighbor_nodes = 1;  // el nodo mismo
      element_ll_t *element_item;
      LL_FOREACH(this->node[j].associated_elements, element_item) {
        if (element_item->element->type->dim == this->dim) {
          if (element_item->element->type->id == ELEMENT_TYPE_TETRAHEDRON4 ||
              element_item->element->type->id == ELEMENT_TYPE_TETRAHEDRON10) {
            // los tetrahedros son "buenos" y con esta cuenta nos ahorramos memoria
            first_neighbor_nodes += (element_item->element->type->nodes) - (element_item->element->type->nodes_per_face);
          } else {
            // si tenemos elementos generales, hay que allocar mas memoria
            first_neighbor_nodes += (element_item->element->type->nodes) - 1;
          }
        }
      }
      if (first_neighbor_nodes > this->max_first_neighbor_nodes) {
        this->max_first_neighbor_nodes = first_neighbor_nodes;
      }
    }
  }  
  
  this->initialized = 1;

  return FEENOX_OK;

}

node_t *feenox_mesh_find_nearest_node(mesh_t *this, const double *x) {
  
  node_t *node;
  void *res_item;  
  
  // TODO: if kd_nodes is null, initialize it here
  res_item = kd_nearest(this->kd_nodes, x);
  node = (node_t *)(kd_res_item(res_item, NULL));
  kd_res_free(res_item);    

  return node;
}

element_t *feenox_mesh_find_element(mesh_t *mesh, node_t *nearest_node, const double *x) {

  double x_nearest[3] = {0, 0, 0};
  double dist2;
  element_t *element = NULL;
  element_ll_t *element_item;
  node_t *second_nearest_node;
  void *res_item;   

  // try the last chosen element
  element = mesh->last_chosen_element;
  
  // test if the last (cached) chosen_element is the one
  if (element == NULL || element->type->point_in_element(element, x) == 0) {
    element = NULL;
    
    // find the nearest node if not provided
    if (nearest_node == NULL) {
      res_item = kd_nearest(mesh->kd_nodes, x);
      nearest_node = (node_t *)(kd_res_item(res_item, x_nearest));
      kd_res_free(res_item);    
    }  
    
    LL_FOREACH(nearest_node->associated_elements, element_item) {
      if (element_item->element->type->dim == mesh->dim && element_item->element->type->point_in_element(element_item->element, x)) {
        element = element_item->element;
        break;
      }  
    }
  }
  

  if (element == NULL && feenox_var_value(feenox.mesh.vars.mesh_failed_interpolation_factor) > 0) {
    // if we do not find any then the mesh might be deformed or the point might be outside the domain
    // so we see if we can find another one

    switch (mesh->dim) {
      case 1:
        dist2 = gsl_pow_2(fabs(x[0] - x_nearest[0]));
      break;
      case 2:
        dist2 = feenox_mesh_subtract_squared_module2d(x, x_nearest);
      break;
      case 3:
        dist2 = feenox_mesh_subtract_squared_module(x, x_nearest);
      break;
    }
    
    // this is a hash of elements we already saw so we do not need to check for them many times
    struct cached_element {
      int id;
      UT_hash_handle hh;
    };
      
    struct cached_element *cache = NULL;
    struct cached_element *tmp = NULL;
    struct cached_element *cached_element = NULL;

    // we ask for the nodes which are within a radius mesh_failed_interpolation_factor times the last one
    struct kdres *presults = kd_nearest_range(mesh->kd_nodes, x, feenox_var_value(feenox.mesh.vars.mesh_failed_interpolation_factor)*sqrt(dist2));
      
    while(element == NULL && kd_res_end(presults) == 0) {
      second_nearest_node = (node_t *)(kd_res_item(presults, x_nearest));
      LL_FOREACH(second_nearest_node->associated_elements, element_item) {
          
        cached_element = NULL;
        HASH_FIND_INT(cache, &element_item->element->tag, cached_element);
        if (cached_element == NULL) {
          struct cached_element *cached_element = malloc(sizeof(struct cached_element));
          cached_element->id = element_item->element->tag;
          HASH_ADD_INT(cache, id, cached_element);
        
          if (element_item->element->type->dim == mesh->dim && element_item->element->type->point_in_element(element_item->element, x)) {
            element = element_item->element;
            break;
          }
        }  
      }
      kd_res_next(presults);
    }
    kd_res_free(presults);

    HASH_ITER(hh, cache, cached_element, tmp) {
      HASH_DEL(cache, cached_element);
      feenox_free(cached_element);
    }  
  }  
  
  if (element == NULL) {
    // if still we did not find anything, 
    switch (mesh->dim) {
      case 1:
        dist2 = gsl_pow_2(fabs(x[0] - x_nearest[0]));
      break;
      case 2:
        dist2 = feenox_mesh_subtract_squared_module2d(x, x_nearest);
      break;
      case 3:
        dist2 = feenox_mesh_subtract_squared_module(x, x_nearest);
      break;
    }

    // just what is close
    if (dist2 < DEFAULT_MULTIDIM_INTERPOLATION_THRESHOLD) {
      LL_FOREACH(nearest_node->associated_elements, element_item) {
        if (element_item->element->type->dim == mesh->dim) {
          element = element_item->element;
          break;
        }
      }
    }
  }
  
  // update cache
  mesh->last_chosen_element = element;

  return element;
}


// free all resources allocated when reading a mesh but not the rest
// of the things that are in the input files (physical groups, etc)
int feenox_mesh_free(mesh_t *mesh) {

  physical_group_t *physical_group;
  element_ll_t *element_item, *element_tmp;
  int i, j, d, v;
/*  
  if (mesh->cell != NULL) {
    for (i = 0; i < mesh->n_cells; i++) {
      if (mesh->cell[i].index != NULL) {
        feenox_free(mesh->cell[i].index);
      }
      if (mesh->cell[i].neighbor != NULL) {
	      for(int k = 0; k<mesh->cell[i].n_neighbors; k++) {
	        if(mesh->cell[i].neighbor[k].face_coord != NULL) {
	          feenox_free(mesh->cell[i].neighbor[k].face_coord);
          }  
	      }
	      feenox_free(mesh->cell[i].neighbor);
      }
      if (mesh->cell[i].ifaces != NULL) {
        for (j = 0; j < mesh->cell[i].element->type->faces; j++) {
          feenox_free(mesh->cell[i].ifaces[j]);
        }
        feenox_free(mesh->cell[i].ifaces);
      }
      if (mesh->cell[i].ineighbor != NULL) {
        feenox_free(mesh->cell[i].ineighbor);
      }
    }
    for (d = 0; d < mesh->spatial_dimensions; d++) {
      feenox_free(mesh->cells_argument[d]);
    }
    feenox_free(mesh->cells_argument);
    feenox_free(mesh->cell);
  }
  mesh->cell = NULL;
  mesh->n_cells = 0;
 */ 
  mesh->max_faces_per_element = 0;

  // elements  
  if (mesh->element != NULL) {
    for (i = 0; i < mesh->n_elements; i++) {
      if (mesh->element[i].node != NULL) {
        for (j = 0; j < mesh->element[i].type->nodes; j++) {
          LL_FOREACH_SAFE(mesh->element[i].node[j]->associated_elements, element_item, element_tmp) {
            LL_DELETE(mesh->element[i].node[j]->associated_elements, element_item);
            feenox_free(element_item);
          }
          
          if (mesh->element[i].dphidx_node != NULL && mesh->element[i].dphidx_node[j] != NULL) {
            gsl_matrix_free(mesh->element[i].dphidx_node[j]);
          }

          if (mesh->element[i].property_at_node != NULL && mesh->element[i].property_at_node[j] != NULL) {
            feenox_free(mesh->element[i].property_at_node[j]);
          }
        }
        feenox_free(mesh->element[i].node);
        
        if (mesh->element[i].dphidx_node != NULL) {
          feenox_free(mesh->element[i].dphidx_node);
        }

        if (mesh->element[i].property_at_node != NULL) {
          feenox_free(mesh->element[i].property_at_node);
        }
      }
      
      if (mesh->element[i].type != NULL) {
        for (v = 0; v < mesh->element[i].type->gauss[mesh->integration].V; v++) {

          if (mesh->element[i].x != NULL && mesh->element[i].x[v] != NULL) {
            feenox_free(mesh->element[i].x[v]);
          }
          if (mesh->element[i].H != NULL && mesh->element[i].H[v] != NULL) {
            gsl_matrix_free(mesh->element[i].H[v]);
          }  
          if (mesh->element[i].B != NULL && mesh->element[i].B[v] != NULL) {
            gsl_matrix_free(mesh->element[i].B[v]);
          }  
          if (mesh->element[i].dxdr != NULL && mesh->element[i].dxdr[v] != NULL) {
            gsl_matrix_free(mesh->element[i].dxdr[v]);
          }
          if (mesh->element[i].drdx != NULL && mesh->element[i].drdx[v] != NULL) {
            gsl_matrix_free(mesh->element[i].drdx[v]);
          }  
          if (mesh->element[i].dhdx != NULL && mesh->element[i].dhdx[v] != NULL) {
            gsl_matrix_free(mesh->element[i].dhdx[v]);
          }  
          if (mesh->element[i].dphidx_gauss != NULL && mesh->element[i].dphidx_gauss[v] != NULL) {
            gsl_matrix_free(mesh->element[i].dphidx_gauss[v]);
          }  
        }
      }  
      
      if (mesh->element[i].w != NULL) {
        feenox_free(mesh->element[i].w);
      }  
      if (mesh->element[i].x != NULL) {
        feenox_free(mesh->element[i].x);
      }  
      if (mesh->element[i].H != NULL) {
        feenox_free(mesh->element[i].H);
      }  
      if (mesh->element[i].B != NULL) {
        feenox_free(mesh->element[i].B);
      }  
      if (mesh->element[i].dxdr != NULL) {
        feenox_free(mesh->element[i].dxdr);
      }  
      if (mesh->element[i].drdx != NULL) {
        feenox_free(mesh->element[i].drdx);
      }  
      if (mesh->element[i].dhdx != NULL) {
        feenox_free(mesh->element[i].dhdx);
      }  
      if (mesh->element[i].dphidx_gauss != NULL) {
        feenox_free(mesh->element[i].dphidx_gauss);
      }  
#ifdef HAVE_PETSC      
      if (mesh->element[i].l != NULL) {
        feenox_free(mesh->element[i].l);
      }  
#endif 
    }
    feenox_free(mesh->element);
  }
  mesh->element = NULL;
  mesh->n_elements = 0;
  mesh->max_nodes_per_element = 0;

  if (mesh->kd_nodes != NULL) {
    kd_free(mesh->kd_nodes);
  }
  mesh->kd_nodes = NULL;

  // nodes
  if (mesh->nodes_argument != NULL) {
    for (d = 0; d < mesh->dim; d++) {
      feenox_free(mesh->nodes_argument[d]);
    }
    feenox_free(mesh->nodes_argument);
  }
  
  if (mesh->tag2index != NULL) {
    free(mesh->tag2index);
  }
  
  if (mesh->node != NULL) {
    for (j = 0; j < mesh->n_nodes; j++) {
      if (mesh->node[j].index_dof != NULL) {
        free (mesh->node[j].index_dof);
      }
      if (mesh->node[j].phi != NULL) {
        feenox_free(mesh->node[j].phi);
      }
      if (mesh->node[j].dphidx != NULL) {
        gsl_matrix_free(mesh->node[j].dphidx);
      }
      if (mesh->node[j].delta_dphidx != NULL) {
        gsl_matrix_free(mesh->node[j].delta_dphidx);
      }
      if (mesh->node[j].flux != NULL) {
        feenox_free(mesh->node[j].flux);
      }
    }
    feenox_free(mesh->node);
  }
  mesh->node = NULL;
  mesh->n_nodes = 0;
  mesh->max_first_neighbor_nodes = 1;
  mesh->last_chosen_element = NULL;

  for (physical_group = mesh->physical_groups; physical_group != NULL; physical_group = physical_group->hh.next) {
    physical_group->n_elements = 0;
    physical_group->i_element = 0;
    feenox_free(physical_group->element);
  }
/*
  for (d = 0; d < 4; d++) {
    HASH_ITER(hh_tag[d], mesh->physical_groups_by_tag[d], physical_group, physical_group_tmp) {
      HASH_DELETE(hh_tag[d], mesh->physical_groups_by_tag[d], physical_group);
    }
  }
  HASH_ITER(hh, mesh->physical_groups, physical_group, physical_group_tmp) {
    HASH_DEL(mesh->physical_groups, physical_group);
    // si hacemos free de la entidad en si entonces perdemos la informacion sobre BCs
    // TODO: pensar!    
    // feenox_free(physical_group->name);
    // feenox_free(physical_group->element);
    // feenox_free(physical_group);
  }
*/    
  
  mesh->initialized = 0;

  return FEENOX_OK;
}


mesh_t *feenox_get_mesh_ptr(const char *name) {
  mesh_t *mesh = NULL;
  if (name != NULL) {
    HASH_FIND_STR(feenox.mesh.meshes, name, mesh);
  }  
  return mesh;
}


int feenox_mesh_create_nodes_argument(mesh_t *this) {
  
  feenox_check_alloc(this->nodes_argument = calloc(this->dim, sizeof(double *)));
  size_t j = 0;
  unsigned int m = 0;
  for (m = 0; m < this->dim; m++) {
    feenox_check_alloc(this->nodes_argument[m] = calloc(this->n_nodes, sizeof(double)));
    for (j = 0; j < this->n_nodes; j++) {
      this->nodes_argument[m][j] = this->node[j].x[m]; 
    }  
  }
  
  return FEENOX_OK;
}
