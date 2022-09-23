/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's mesh-related routines for finding extrema
 *
 *  Copyright (C) 2016--2021 jeremy theler
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
#include <feenox.h>

#define FEENOX_EXTREMA_MIN 0
#define FEENOX_EXTREMA_MAX 1

#define feenox_store_extrema_function(minmax, ind, val) \
  value[minmax] = val;  \
  index[minmax] = ind;  \
  x[minmax] = function->data_argument[0][ind];  \
  y[minmax] = (function->n_arguments > 1)?function->data_argument[1][ind] : 0;  \
  z[minmax] = (function->n_arguments > 2)?function->data_argument[2][ind] : 0;

#define feenox_store_extrema_mesh(minmax, ind, val, where) \
  value[minmax] = val;  \
  index[minmax] = ind;  \
  x[minmax] = where[0];  \
  y[minmax] = where[1]; \
  z[minmax] = where[2];


int feenox_instruction_mesh_find_extrema(void *arg) {

  double value[2] = {+INFTY, -INFTY};
  double x[2] = {0, 0};
  double y[2] = {0, 0};
  double z[2] = {0, 0};
  size_t index[2] = {0, 0};
  
  mesh_find_extrema_t *mesh_find_extrema = (mesh_find_extrema_t *)arg;
  mesh_t *mesh = mesh_find_extrema->mesh;
  physical_group_t *physical_group = mesh_find_extrema->physical_group;
  function_t *function = mesh_find_extrema->function;
  expr_t *expr = &mesh_find_extrema->expr;
  element_t *element = NULL;
  
  double val = 0;
  if (physical_group == NULL) {
    if (function != NULL) {
      if (function->mesh == mesh && ((mesh_find_extrema->field_location == field_location_cells && function->type == function_type_pointwise_mesh_cell) ||
                                     (mesh_find_extrema->field_location == field_location_nodes && function->type == function_type_pointwise_mesh_node))) {
        size_t i = 0;
        for (i = 0; i < function->data_size; i++) {
          val = function->data_value[i];
          if (val > value[FEENOX_EXTREMA_MAX]) {
            feenox_store_extrema_function(FEENOX_EXTREMA_MAX, i, val);
          }
          if (val < value[FEENOX_EXTREMA_MIN]) {
            feenox_store_extrema_function(FEENOX_EXTREMA_MIN, i, val);
          }
        }
      } else {
        
        // unless explcitily asked, check for all nodes, cells and gauss
        if (mesh_find_extrema->field_location == field_location_default || mesh_find_extrema->field_location == field_location_cells) {
          
          if (mesh->cell == NULL) {
            feenox_call(feenox_mesh_element2cell(mesh));
          }
          
          size_t i = 0;
          for (i = 0; i < mesh->n_cells; i++) {
            val = feenox_function_eval(function, mesh->cell[i].x);
            if (val > value[FEENOX_EXTREMA_MAX]) {
              feenox_store_extrema_mesh(FEENOX_EXTREMA_MAX, i, val, mesh->cell[i].x);
            }
            if (val < value[FEENOX_EXTREMA_MIN]) {
              feenox_store_extrema_mesh(FEENOX_EXTREMA_MIN, i, val, mesh->cell[i].x);
            }  
          }
          
        }
        
        if (mesh_find_extrema->field_location == field_location_default || mesh_find_extrema->field_location == field_location_nodes) {
          
          size_t i = 0;
          for (i = 0; i < mesh->n_nodes; i++) {
            val = feenox_function_eval(function, mesh->node[i].x);
            if (val > value[FEENOX_EXTREMA_MAX]) {
              feenox_store_extrema_mesh(FEENOX_EXTREMA_MAX, i, val, mesh->node[i].x);
            }
            if (val < value[FEENOX_EXTREMA_MIN]) {
              feenox_store_extrema_mesh(FEENOX_EXTREMA_MIN, i, val, mesh->node[i].x);
            }
          }
          
        }

        if (mesh_find_extrema->field_location == field_location_default || mesh_find_extrema->field_location == field_location_gauss) {
          
          unsigned int v = 0;
          size_t i = 0;
          for (i = 0; i < mesh->n_elements; i++) {
            element = &mesh->element[i];
            for (v = 0; v < element->type->gauss[mesh->integration].V; v++) {
              feenox_mesh_compute_x_at_gauss(element, v, mesh->integration);
              
              val = feenox_function_eval(function, element->x[v]);
              if (val > value[FEENOX_EXTREMA_MAX]) {
                feenox_store_extrema_mesh(FEENOX_EXTREMA_MAX, i, val, element->x[v]);
              }
              if (val < value[FEENOX_EXTREMA_MIN]) {
                feenox_store_extrema_mesh(FEENOX_EXTREMA_MIN, i, val, element->x[v]);
              }
            }  
          }
          
        }
        
      }
      
    } else {
      
      // if we are here we are trying to find the extrema of an expression
      // we cannot know whether they are going to be
      if (mesh_find_extrema->field_location == field_location_default || mesh_find_extrema->field_location == field_location_cells) {
        if (mesh->cell == NULL) {
          feenox_call(feenox_mesh_element2cell(mesh));
        }

        size_t i = 0;
        for (i = 0; i < mesh->n_cells; i++) {
          feenox_mesh_update_coord_vars(mesh->cell[i].x);
          val = feenox_expression_eval(expr);
          
          if (val > value[FEENOX_EXTREMA_MAX]) {
            feenox_store_extrema_mesh(FEENOX_EXTREMA_MAX, i, val, mesh->cell[i].x);
          }
          if (val < value[FEENOX_EXTREMA_MIN]) {
            feenox_store_extrema_mesh(FEENOX_EXTREMA_MIN, i, val, mesh->cell[i].x);
          }
        }
      }
      
      if (mesh_find_extrema->field_location == field_location_default || mesh_find_extrema->field_location == field_location_nodes) {
        size_t i = 0;
        for (i = 0; i < mesh->n_nodes; i++) {
          feenox_mesh_update_coord_vars(mesh->node[i].x);
          val = feenox_expression_eval(expr);
          
          if (val > value[FEENOX_EXTREMA_MAX]) {
            feenox_store_extrema_mesh(FEENOX_EXTREMA_MAX, i, val, mesh->node[i].x);
          }
          if (val < value[FEENOX_EXTREMA_MIN]) {
            feenox_store_extrema_mesh(FEENOX_EXTREMA_MIN, i, val, mesh->node[i].x);
          }
        }
      }
      
      if (mesh_find_extrema->field_location == field_location_default || mesh_find_extrema->field_location == field_location_gauss) {
        unsigned int v = 0;
        size_t i = 0;
        for (i = 0; i < mesh->n_elements; i++) {
          element = &mesh->element[i];
          for (v = 0; v < element->type->gauss[mesh->integration].V; v++) {
            feenox_mesh_compute_x_at_gauss(element, v, mesh->integration);
            feenox_mesh_update_coord_vars(element->x[v]);
            val = feenox_expression_eval(expr);
            
            if (val > value[FEENOX_EXTREMA_MAX]) {
              feenox_store_extrema_mesh(FEENOX_EXTREMA_MAX, i, val, element->x[v]);
            }
            if (val < value[FEENOX_EXTREMA_MIN]) {
              feenox_store_extrema_mesh(FEENOX_EXTREMA_MIN, i, val, element->x[v]);
            }
          }  
        }
      }
      
    }
    
  } else {
    
    if (function != NULL) {
      // TODO: these
      if (mesh_find_extrema->field_location == field_location_cells) {
        if (function->type == function_type_pointwise_mesh_cell && function->mesh == mesh) {
          feenox_push_error_message("FIND_EXTREMA with OVER on a cell-centered function not implemented yet.");
          return FEENOX_ERROR;
        } else {
          feenox_push_error_message("FIND_EXTREMA with OVER on a cell-centered generic function not implemented yet.");
          return FEENOX_ERROR;
        }
      } else {
        if (function->type == function_type_pointwise_mesh_node && function->mesh == mesh) {
          
          size_t i = 0;
          unsigned int j = 0;
          for (i = 0; i < physical_group->n_elements; i++) {
            element = &mesh->element[physical_group->element[i]];
            for (j = 0; j < element->type->nodes; j++) {
              val = function->data_value[element->node[j]->index_mesh];
              if (val > value[FEENOX_EXTREMA_MAX]) {
                feenox_store_extrema_mesh(FEENOX_EXTREMA_MAX, element->node[j]->index_mesh, val, element->node[j]->x);
              }
              if (val < value[FEENOX_EXTREMA_MIN]) {
                feenox_store_extrema_mesh(FEENOX_EXTREMA_MIN, element->node[j]->index_mesh, val, element->node[j]->x);
              }  
            }
          }
          
        } else {
          feenox_push_error_message("FIND_EXTREMA with OVER on a node-centered generic function not implemented yet.");
          return FEENOX_ERROR;
        }
      }
    } else {
      if (mesh_find_extrema->field_location == field_location_cells) {
        feenox_push_error_message("FIND_EXTREMA with OVER on a cell-centered expression not implemented yet.");
        return FEENOX_ERROR;
      } else {
        size_t i = 0;
        unsigned int j = 0;
        for (i = 0; i < physical_group->n_elements; i++) {
          element = &mesh->element[physical_group->element[i]];
          for (j = 0; j < element->type->nodes; j++) {
            feenox_mesh_update_coord_vars(element->node[j]->x);
            val = feenox_expression_eval(expr);
            if (val > value[FEENOX_EXTREMA_MAX]) {
              feenox_store_extrema_mesh(FEENOX_EXTREMA_MAX, element->node[j]->index_mesh, val, element->node[j]->x);
            }
            if (val < value[FEENOX_EXTREMA_MIN]) {
              feenox_store_extrema_mesh(FEENOX_EXTREMA_MIN, element->node[j]->index_mesh, val, element->node[j]->x);
            }
          }
        }
      }
    }
  }  

  if (mesh_find_extrema->min != NULL) {
    feenox_var_value(mesh_find_extrema->min) = value[FEENOX_EXTREMA_MIN];
  }
  if (mesh_find_extrema->i_min != NULL) {
    feenox_var_value(mesh_find_extrema->i_min) = (double)index[FEENOX_EXTREMA_MIN];
  }
  if (mesh_find_extrema->x_min != NULL) {
    feenox_var_value(mesh_find_extrema->x_min) = x[FEENOX_EXTREMA_MIN];
  }
  if (mesh_find_extrema->y_min != NULL) {
    feenox_var_value(mesh_find_extrema->y_min) = y[FEENOX_EXTREMA_MIN];
  }
  if (mesh_find_extrema->z_min != NULL) {
    feenox_var_value(mesh_find_extrema->z_min) = z[FEENOX_EXTREMA_MIN];
  }
  
  if (mesh_find_extrema->max != NULL) {
    feenox_var_value(mesh_find_extrema->max) = value[FEENOX_EXTREMA_MAX];
  }
  if (mesh_find_extrema->i_max != NULL) {
    feenox_var_value(mesh_find_extrema->i_max) = (double)index[FEENOX_EXTREMA_MAX];
  }
  if (mesh_find_extrema->x_max != NULL) {
    feenox_var_value(mesh_find_extrema->x_max) = x[FEENOX_EXTREMA_MAX];
  }
  if (mesh_find_extrema->y_max != NULL) {
    feenox_var_value(mesh_find_extrema->y_max) = y[FEENOX_EXTREMA_MAX];
  }
  if (mesh_find_extrema->z_max != NULL) {
    feenox_var_value(mesh_find_extrema->z_max) = z[FEENOX_EXTREMA_MAX];
  }
  
  return FEENOX_OK;
}

