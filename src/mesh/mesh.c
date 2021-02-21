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
#include "feenox.h"
extern feenox_t feenox;

int feenox_instruction_mesh_read(void *arg) {

  mesh_t *mesh = (mesh_t *)arg;
  
  function_t *function, *tmp_function;
  element_list_item_t *associated_element;
  element_t *element;
  node_data_t *node_data;
  int i, j, d, v;
  int first_neighbor_nodes;
  int bulk_dimensions = 0;
  double scale_factor;
  double offset[3];
  double vol;
  double cog[3];

  if (mesh->initialized) {
    if (mesh->update_each_step == 0) {
      // we are not supposed to read the mesh all over again
      return FEENOX_OK;
    } else {
      // we have to re-read the mesh, but we already have one
      // so we get need to get rid of the current one
      feenox_mesh_free(mesh);
    }
  }
  
  // TODO: virtual methods
  switch (mesh->format) {
    case mesh_format_gmsh:
      feenox_call(feenox_mesh_read_gmsh(mesh));
    break; 
    case mesh_format_vtk:
      feenox_call(feenox_mesh_read_vtk(mesh));
    break;
    case mesh_format_frd:
      feenox_call(feenox_mesh_read_frd(mesh));
    break;
    default:
      feenox_push_error_message("unsupported mesh format");
      return FEENOX_ERROR;
    break;
  }
  
  // sweep nodes ande define the bounding box
  // TODO: see if this can go inside one of the kd loops
  mesh->bounding_box_min.index_mesh = -1;
  mesh->bounding_box_max.index_mesh = -1;
  mesh->bounding_box_min.index_dof = NULL;
  mesh->bounding_box_max.index_dof = NULL;
  mesh->bounding_box_min.associated_elements = NULL;
  mesh->bounding_box_max.associated_elements = NULL;
  
  scale_factor = feenox_expression_eval(&mesh->scale_factor);
  offset[0] = feenox_expression_eval(&mesh->offset_x);
  offset[1] = feenox_expression_eval(&mesh->offset_y);
  offset[2] = feenox_expression_eval(&mesh->offset_z);

  double x_min[3];
  double x_max[3];
  for (d = 0; d < 3; d++) {
    x_min[d] = +MESH_INF;
    x_max[d] = -MESH_INF;
  }
  
  for (j = 0; j < mesh->n_nodes; j++) {
    for (d = 0; d < 3; d++) {
      if (scale_factor != 0 || offset[d] != 0) {
        mesh->node[j].x[d] -= offset[d];
        mesh->node[j].x[d] *= scale_factor;
      }
      
      if (mesh->dim < 1 && fabs(mesh->node[j].x[0]) > MESH_TOL) {
        mesh->dim = 1;
      }
      if (mesh->dim < 2 && fabs(mesh->node[j].x[1]) > MESH_TOL) {
        mesh->dim = 2;
      }
      if (mesh->dim < 3 && fabs(mesh->node[j].x[2]) > MESH_TOL) {
        mesh->dim = 3;
      }
      
      if (mesh->node[j].x[d] < x_min[d]) {
        x_min[d] = mesh->bounding_box_min.x[d] = mesh->node[j].x[d];
      }
      if (mesh->node[j].x[d] > x_max[d]) {
        x_max[d] = mesh->bounding_box_max.x[d] = mesh->node[j].x[d];
      }
    }
  }
  
  // TODO: what was this?
//  feenox_call(feenox_vector_init(feenox_mesh.vars.bbox_min));
//  feenox_call(feenox_vector_init(feenox_mesh.vars.bbox_max));
/*  
  gsl_vector_set(feenox_mesh.vars.bbox_min->value, 0, x_min[0]);
  gsl_vector_set(feenox_mesh.vars.bbox_min->value, 1, x_min[1]);
  gsl_vector_set(feenox_mesh.vars.bbox_min->value, 2, x_min[2]);
  gsl_vector_set(feenox_mesh.vars.bbox_max->value, 0, x_max[0]);
  gsl_vector_set(feenox_mesh.vars.bbox_max->value, 1, x_max[1]);
  gsl_vector_set(feenox_mesh.vars.bbox_max->value, 2, x_max[2]);
*/  
  
  // allocate arrays for the elements that belong to a physical group
  // (arrays are more efficient than a linked list)
  physical_group_t *physical_group;
  for (physical_group = mesh->physical_groups; physical_group != NULL; physical_group = physical_group->hh.next) {
    if (physical_group->n_elements != 0) {
      physical_group->element = malloc(physical_group->n_elements * sizeof(int));
    }
    // check out what the highest tag is to allocate temporary arrays
    if (physical_group->tag > mesh->physical_tag_max) {
      mesh->physical_tag_max = physical_group->tag;
    }
  }
  
  for (i = 0; i < mesh->n_elements; i++) {
    
    // check the dimension of the element, the higher is the topological dim of the mes
    if (mesh->element[i].type->dim > mesh->dim_topo) {
      mesh->dim_topo = mesh->element[i].type->dim;
    }

    // same for order
    if (mesh->element[i].type->order > mesh->order) {
      mesh->order = mesh->element[i].type->order;
    }

    // nos acordamos del elemento que tenga el mayor numero de nodos
    if (mesh->element[i].type->nodes > mesh->max_nodes_per_element) {
      mesh->max_nodes_per_element = mesh->element[i].type->nodes;
    }

    // y del que tenga mayor cantidad de vecinos
    if (mesh->element[i].type->faces > mesh->max_faces_per_element) {
      mesh->max_faces_per_element = mesh->element[i].type->faces;
    }

    // armamos la lista de elementos de cada entidad
    physical_group = mesh->element[i].physical_group;
    if (physical_group != NULL && physical_group->i_element < physical_group->n_elements) {
      physical_group->element[physical_group->i_element++] = i;
    }
  }
 
  // verificamos que la malla tenga la dimension esperada
  if (mesh->bulk_dimensions == 0) {
    mesh->bulk_dimensions = bulk_dimensions;
  } else if (mesh->bulk_dimensions != bulk_dimensions) {
    feenox_push_error_message("mesh '%s' is expected to have %d dimensions but it has %d", mesh->file->path, mesh->bulk_dimensions, bulk_dimensions);
    return WASORA_RUNTIME_ERROR;
  }
    
  // rellenamos un array de nodos que pueda ser usado como argumento de funciones
  // TODO: poner esto en el loop de arriba?
  mesh->nodes_argument = malloc(mesh->spatial_dimensions * sizeof(double *));
  for (d = 0; d < mesh->spatial_dimensions; d++) {
    mesh->nodes_argument[d] = malloc(mesh->n_nodes * sizeof(double));
    for (j = 0; j < mesh->n_nodes; j++) {
      mesh->nodes_argument[d][j] = mesh->node[j].x[d]; 
    }
  }
  
  // idem de celdas
  if (feenox_mesh.need_cells) {
    feenox_call(mesh_element2cell(mesh));
    mesh->cells_argument = malloc(mesh->spatial_dimensions * sizeof(double *));
    for (d = 0; d < mesh->spatial_dimensions; d++) {
      mesh->cells_argument[d] = malloc(mesh->n_cells * sizeof(double));
      for (i = 0; i < mesh->n_cells; i++) {
        mesh->cells_argument[d][i] = mesh->cell[i].x[d]; 
      }
    }
  }

  if (feenox_mesh.main_mesh == mesh) {
    feenox_var(feenox_mesh.vars.cells) = (double)mesh->n_cells;
    feenox_var(feenox_mesh.vars.nodes) = (double)mesh->n_nodes;
    feenox_var(feenox_mesh.vars.elements) = (double)mesh->n_elements;
  }
  
  // vemos si nos quedo alguna funcion sin leer
  LL_FOREACH(mesh->node_datas, node_data) {
    if (node_data->function->mesh == NULL) {
      feenox_push_error_message("cannot find data for function '%s' for t=0 in mesh '%s'", node_data->name_in_mesh, mesh->name);
      return WASORA_RUNTIME_ERROR;
    }
    
  }
  
  // barremos todas las funciones, si encontramos alguna que tenga una malla la linkeamos a esta
  HASH_ITER(hh, feenox.functions, function, tmp_function) {
    if (function->mesh != NULL && function->mesh == mesh) {

      function->initialized = 0;
      
      if (function->type == type_pointwise_mesh_node) {
        function->data_size = mesh->n_nodes;
        function->data_argument = mesh->nodes_argument;
      } else if (function->type == type_pointwise_mesh_cell) {
        function->data_size = mesh->n_cells;
        function->data_argument = mesh->cells_argument;
      }
      
      if (function->vector_value != NULL) {
        function->vector_value->size = function->data_size;
      }
    }
  }
  
  // calculamos el volumen (o superficie o longitud) y el centro de masa de las physical entities
  if (mesh->bulk_dimensions != 0) {
    for (physical_group = mesh->physical_entities; physical_group != NULL; physical_group = physical_group->hh.next) {
      vol = cog[0] = cog[1] = cog[2] = 0;
      for (i = 0; i < physical_group->n_elements; i++) {
        element = &mesh->element[physical_group->element[i]];
        for (v = 0; v < element->type->gauss[mesh->integration].V; v++) {
          mesh_compute_integration_weight_at_gauss(element, v, mesh->integration);

          for (j = 0; j < element->type->nodes; j++) {
            vol += element->w[v] * element->type->gauss[mesh->integration].h[v][j];
            cog[0] += element->w[v] * element->type->gauss[mesh->integration].h[v][j] * element->node[j]->x[0];
            cog[1] += element->w[v] * element->type->gauss[mesh->integration].h[v][j] * element->node[j]->x[1];
            cog[2] += element->w[v] * element->type->gauss[mesh->integration].h[v][j] * element->node[j]->x[2];
          }
        }
      }
      physical_group->volume = vol;
      physical_group->cog[0] = cog[0]/vol;
      physical_group->cog[1] = cog[1]/vol;
      physical_group->cog[2] = cog[2]/vol;

      // las pasamos a feenox para que esten disponibles en el input
      if (physical_group->var_vol != NULL) {
        feenox_var_value(physical_group->var_vol) = vol;
      }

      if (physical_group->vector_cog != NULL) {
        if (!physical_group->vector_cog->initialized) {
          feenox_call(feenox_vector_init(physical_group->vector_cog));
        }
        gsl_vector_set(physical_group->vector_cog->value, 0, physical_group->cog[0]);
        gsl_vector_set(physical_group->vector_cog->value, 1, physical_group->cog[1]);
        gsl_vector_set(physical_group->vector_cog->value, 2, physical_group->cog[2]);
      }
    }
  }

  // create a k-dimensional tree and try to figure out what the maximum number of neighbours each node has
  if (mesh->kd_nodes == NULL) {
    mesh->kd_nodes = kd_create(mesh->spatial_dimensions);
    for (j = 0; j < mesh->n_nodes; j++) {
      kd_insert(mesh->kd_nodes, mesh->node[j].x, &mesh->node[j]);
    
      first_neighbor_nodes = 1;  // el nodo mismo
      LL_FOREACH(mesh->node[j].associated_elements, associated_element) {
        if (associated_element->element->type->dim == mesh->bulk_dimensions) {
          if (associated_element->element->type->id == ELEMENT_TYPE_TETRAHEDRON4 ||
              associated_element->element->type->id == ELEMENT_TYPE_TETRAHEDRON10) {
            // los tetrahedros son "buenos" y con esta cuenta nos ahorramos memoria
            first_neighbor_nodes += (associated_element->element->type->nodes) - (associated_element->element->type->nodes_per_face);
          } else {
            // si tenemos elementos generales, hay que allocar mas memoria
            first_neighbor_nodes += (associated_element->element->type->nodes) - 1;
          }
        }
      }
      if (first_neighbor_nodes > mesh->max_first_neighbor_nodes) {
        mesh->max_first_neighbor_nodes = first_neighbor_nodes;
      }
    }
  }  
  
  // esto es todo amigos!
  mesh->initialized = 1;

  return FEENOX_OK;

}

node_t *mesh_find_nearest_node(mesh_t *mesh, const double *x) {
  
  node_t *node;
  void *res_item;  
  
  res_item = kd_nearest(mesh->kd_nodes, x);
  node = (node_t *)(kd_res_item(res_item, NULL));
  kd_res_free(res_item);    

  return node;
}

element_t *mesh_find_element(mesh_t *mesh, node_t *nearest_node, const double *x) {

  double x_nearest[3] = {0, 0, 0};
  double dist2;
  element_t *element = NULL;
  element_list_item_t *associated_element;
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
    
    LL_FOREACH(nearest_node->associated_elements, associated_element) {
      if (associated_element->element->type->dim == mesh->bulk_dimensions && associated_element->element->type->point_in_element(associated_element->element, x)) {
        element = associated_element->element;
        break;
      }  
    }
  }
  

  if (element == NULL && feenox_var(feenox_mesh.vars.mesh_failed_interpolation_factor) > 0) {
    // if we do not find any then the mesh might be deformed or the point might be outside the domain
    // so we see if we can find another one

    switch (mesh->spatial_dimensions) {
      case 1:
        dist2 = gsl_pow_2(fabs(x[0] - x_nearest[0]));
      break;
      case 2:
        dist2 = mesh_subtract_squared_module2d(x, x_nearest);
      break;
      case 3:
        dist2 = mesh_subtract_squared_module(x, x_nearest);
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
    struct kdres *presults = kd_nearest_range(mesh->kd_nodes, x, feenox_var(feenox_mesh.vars.mesh_failed_interpolation_factor)*sqrt(dist2));
      
    while(element == NULL && kd_res_end(presults) == 0) {
      second_nearest_node = (node_t *)(kd_res_item(presults, x_nearest));
      LL_FOREACH(second_nearest_node->associated_elements, associated_element) {
          
        cached_element = NULL;
        HASH_FIND_INT(cache, &associated_element->element->tag, cached_element);
        if (cached_element == NULL) {
          struct cached_element *cached_element = malloc(sizeof(struct cached_element));
          cached_element->id = associated_element->element->tag;
          HASH_ADD_INT(cache, id, cached_element);
        
          if (associated_element->element->type->dim == mesh->bulk_dimensions && associated_element->element->type->point_in_element(associated_element->element, x)) {
            element = associated_element->element;
            break;
          }
        }  
      }
      kd_res_next(presults);
    }
    kd_res_free(presults);

    HASH_ITER(hh, cache, cached_element, tmp) {
      HASH_DEL(cache, cached_element);
      free(cached_element);
    }  
  }  
  
  if (element == NULL) {
    // if still we did not find anything, 
    switch (mesh->spatial_dimensions) {
      case 1:
        dist2 = gsl_pow_2(fabs(x[0] - x_nearest[0]));
      break;
      case 2:
        dist2 = mesh_subtract_squared_module2d(x, x_nearest);
      break;
      case 3:
        dist2 = mesh_subtract_squared_module(x, x_nearest);
      break;
    }

    // just what is close
    if (dist2 < DEFAULT_MULTIDIM_INTERPOLATION_THRESHOLD) {
      LL_FOREACH(nearest_node->associated_elements, associated_element) {
        if (associated_element->element->type->dim == mesh->bulk_dimensions) {
          element = associated_element->element;
          break;
        }
      }
    }
  }
  
  // update cache
  mesh->last_chosen_element = element;

  return element;
}

// libera lo que allocamos al leerla, pero no lo que
// esta en el input (dimensiones, grados de libertad, entidades fisicas, etc)
int mesh_free(mesh_t *mesh) {

  physical_group_t *physical_group;
  element_list_item_t *element_item, *element_tmp;
  int i, j, d, v;
  
  if (mesh->cell != NULL) {
    for (i = 0; i < mesh->n_cells; i++) {
      if (mesh->cell[i].index != NULL) {
        free(mesh->cell[i].index);
      }
      if (mesh->cell[i].neighbor != NULL) {
	for(int k = 0; k<mesh->cell[i].n_neighbors; k++) {
	  if(mesh->cell[i].neighbor[k].face_coord != NULL)
	    free(mesh->cell[i].neighbor[k].face_coord);
	}
	free(mesh->cell[i].neighbor);
      }
      if (mesh->cell[i].ifaces != NULL) {
        for (j = 0; j < mesh->cell[i].element->type->faces; j++) {
          free(mesh->cell[i].ifaces[j]);
        }
        free(mesh->cell[i].ifaces);
      }
      if (mesh->cell[i].ineighbor != NULL) {
        free(mesh->cell[i].ineighbor);
      }
    }
    for (d = 0; d < mesh->spatial_dimensions; d++) {
      free(mesh->cells_argument[d]);
    }
    free(mesh->cells_argument);
    free(mesh->cell);
  }
  mesh->cell = NULL;
  mesh->n_cells = 0;
  mesh->max_faces_per_element = 0;

  // elements  
  if (mesh->element != NULL) {
    for (i = 0; i < mesh->n_elements; i++) {
      if (mesh->element[i].node != NULL) {
        for (j = 0; j < mesh->element[i].type->nodes; j++) {
          LL_FOREACH_SAFE(mesh->element[i].node[j]->associated_elements, element_item, element_tmp) {
            LL_DELETE(mesh->element[i].node[j]->associated_elements, element_item);
            free(element_item);
          }
          
          if (mesh->element[i].dphidx_node != NULL && mesh->element[i].dphidx_node[j] != NULL) {
            gsl_matrix_free(mesh->element[i].dphidx_node[j]);
          }
          
          if (mesh->element[i].property_node != NULL && mesh->element[i].dphidx_node != NULL) {
            free(mesh->element[i].property_node[j]);
          }
          
          
        }
        free(mesh->element[i].node);
        
        if (mesh->element[i].dphidx_node != NULL) {
          free(mesh->element[i].dphidx_node);
        }
        
        if (mesh->element[i].property_node != NULL) {
          free(mesh->element[i].property_node);
        }
        
      }
      
      if (mesh->element[i].type != NULL && mesh->element[i].type->gauss != NULL) {
        for (v = 0; v < mesh->element[i].type->gauss[mesh->integration].V; v++) {

          if (mesh->element[i].x != NULL && mesh->element[i].x[v] != NULL) {
            free(mesh->element[i].x[v]);
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
        free(mesh->element[i].w);
      }  
      if (mesh->element[i].x != NULL) {
        free(mesh->element[i].x);
      }  
      if (mesh->element[i].H != NULL) {
        free(mesh->element[i].H);
      }  
      if (mesh->element[i].B != NULL) {
        free(mesh->element[i].B);
      }  
      if (mesh->element[i].dxdr != NULL) {
        free(mesh->element[i].dxdr);
      }  
      if (mesh->element[i].drdx != NULL) {
        free(mesh->element[i].drdx);
      }  
      if (mesh->element[i].dhdx != NULL) {
        free(mesh->element[i].dhdx);
      }  
      if (mesh->element[i].dphidx_gauss != NULL) {
        free(mesh->element[i].dphidx_gauss);
      }  
      if (mesh->element[i].l != NULL) {
        free(mesh->element[i].l);
      }  
    }
    free(mesh->element);
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
    for (d = 0; d < mesh->spatial_dimensions; d++) {
      free(mesh->nodes_argument[d]);
    }
    free(mesh->nodes_argument);
    mesh->nodes_argument = NULL;
  }
  
  if (mesh->tag2index != NULL) {
    free(mesh->tag2index);
    mesh->tag2index = NULL;
  }
  
  if (mesh->node != NULL) {
    for (j = 0; j < mesh->n_nodes; j++) {
      if (mesh->node[j].index_dof != NULL) {
        free (mesh->node[j].index_dof);
      }
      if (mesh->node[j].phi != NULL) {
        free(mesh->node[j].phi);
      }
      if (mesh->node[j].dphidx != NULL) {
        gsl_matrix_free(mesh->node[j].dphidx);
      }
      if (mesh->node[j].delta_dphidx != NULL) {
        gsl_matrix_free(mesh->node[j].delta_dphidx);
      }
      if (mesh->node[j].f != NULL) {
        free(mesh->node[j].f);
      }
    }
    free(mesh->node);
  }
  mesh->node = NULL;
  mesh->n_nodes = 0;
  mesh->max_first_neighbor_nodes = 1;
  mesh->last_chosen_element = NULL;

  for (physical_group = mesh->physical_entities; physical_group != NULL; physical_group = physical_group->hh.next) {
    physical_group->n_elements = 0;
    physical_group->i_element = 0;
    free(physical_group->element);
    physical_group->element = NULL;
  }
/*
  for (d = 0; d < 4; d++) {
    HASH_ITER(hh_tag[d], mesh->physical_entities_by_tag[d], physical_group, physical_group_tmp) {
      HASH_DELETE(hh_tag[d], mesh->physical_entities_by_tag[d], physical_group);
    }
  }
  HASH_ITER(hh, mesh->physical_entities, physical_group, physical_group_tmp) {
    HASH_DEL(mesh->physical_entities, physical_group);
    // si hacemos free de la entidad en si entonces perdemos la informacion sobre BCs
    // TODO: pensar!    
    // free(physical_group->name);
    // free(physical_group->element);
    // free(physical_group);
  }
*/    
  
  mesh->initialized = 0;

  return FEENOX_OK;
}


// devuelve la direccion de la estructura de la variable que se llama name
mesh_t *feenox_get_mesh_ptr(const char *name) {
  mesh_t *mesh;
  HASH_FIND_STR(feenox_mesh.meshes, name, mesh);
  return mesh;
}
