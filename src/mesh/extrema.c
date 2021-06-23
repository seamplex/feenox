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
extern feenox_t feenox;

int feenox_instruction_mesh_find_extrema(void *arg) {

  double min = +INFTY;
  double x_min = 0;
  double y_min = 0;
  double z_min = 0;
  
  double max = -INFTY;
  double x_max = 0;
  double y_max = 0;
  double z_max = 0;

  size_t i_max = 0;
  size_t i_min = 0;
  
  mesh_find_extrema_t *mesh_find_extrema = (mesh_find_extrema_t *)arg;
  mesh_t *mesh = mesh_find_extrema->mesh;
  physical_group_t *physical_group = mesh_find_extrema->physical_group;
  function_t *function = mesh_find_extrema->function;
  expr_t *expr = &mesh_find_extrema->expr;
  element_t *element = NULL;
  
  // TODO: is this the best way to go?
  double y = 0;
  if (physical_group == NULL) {
    if (function != NULL) {
      if (mesh_find_extrema->field_location == field_location_cells) {
        if (function->type == function_type_pointwise_mesh_cell && function->mesh == mesh) {
          size_t i = 0;
          for (i = 0; i < function->data_size; i++) {
            y = function->data_value[i];
            if (y > max) {
              // TODO: SPOT!
              max = y;
              i_max = i;
              x_max = function->data_argument[0][i];
              y_max = (function->n_arguments > 1)?function->data_argument[1][i] : 0;
              z_max = (function->n_arguments > 2)?function->data_argument[2][i] : 0;
            }
            if (y < min) {
              // TODO: SPOT!
              min = y;
              i_min = i;
              x_min = function->data_argument[0][i];
              y_min = (function->n_arguments > 1)?function->data_argument[1][i] : 0;
              z_min = (function->n_arguments > 2)?function->data_argument[2][i] : 0;
            }
          }
        } else {
          size_t i = 0;
          for (i = 0; i < mesh->n_cells; i++) {
            y = feenox_function_eval(function, mesh->cell[i].x);
            if (y > max) {
              max = y;
              i_max = i;
              x_max = mesh->cell[i].x[0];
              y_max = mesh->cell[i].x[1];
              z_max = mesh->cell[i].x[2];
            }
            if (y < min) {
              min = y;
              i_min = i;
              x_min = mesh->cell[i].x[0];
              y_min = mesh->cell[i].x[1];
              z_min = mesh->cell[i].x[2];
            }
          }
        }
      } else {
        if (function->type == function_type_pointwise_mesh_node && function->mesh == mesh) {
          size_t i = 0;
          for (i = 0; i < function->data_size; i++) {
            y = function->data_value[i];
            if (y > max) {
              max = y;
              i_max = i;
              x_max = function->data_argument[0][i];
              y_max = (function->n_arguments > 1)?function->data_argument[1][i] : 0;
              z_max = (function->n_arguments > 2)?function->data_argument[2][i] : 0;
            }
            if (y < min) {
              min = y;
              i_min = i;
              x_min = function->data_argument[0][i];
              y_min = (function->n_arguments > 1)?function->data_argument[1][i] : 0;
              z_min = (function->n_arguments > 2)?function->data_argument[2][i] : 0;
            }
          }
        } else {
          size_t i = 0;
          for (i = 0; i < mesh->n_nodes; i++) {
            y = feenox_function_eval(function, mesh->node[i].x);
            if (y > max) {
              max = y;
              i_max = i;
              x_max = mesh->node[i].x[0];
              y_max = mesh->node[i].x[1];
              z_max = mesh->node[i].x[2];
            }
            if (y < min) {
              min = y;
              i_min = i;
              x_min = mesh->node[i].x[0];
              y_min = mesh->node[i].x[1];
              z_min = mesh->node[i].x[2];
            }
          }
        }
      }
    } else {
      if (mesh_find_extrema->field_location == field_location_cells) {
        size_t i = 0;
        for (i = 0; i < mesh->n_cells; i++) {
          feenox_mesh_update_coord_vars(mesh->cell[i].x);
          y = feenox_expression_eval(expr);
          if (y > max) {
            max = y;
            i_max = i;
            x_max = mesh->cell[i].x[0];
            y_max = mesh->cell[i].x[1];
            z_max = mesh->cell[i].x[2];
          }
          if (y < min) {
            min = y;
            i_min = i;
            x_min = mesh->cell[i].x[0];
            y_min = mesh->cell[i].x[1];
            z_min = mesh->cell[i].x[2];
          }
        }
      } else {
        size_t i = 0;
        for (i = 0; i < mesh->n_nodes; i++) {
          feenox_mesh_update_coord_vars(mesh->node[i].x);
          y = feenox_expression_eval(expr);
          if (y > max) {
            max = y;
            i_max = i;
            x_max = mesh->node[i].x[0];
            y_max = mesh->node[i].x[1];
            z_max = mesh->node[i].x[2];
          }
          if (y < min) {
            min = y;
            i_min = i;
            x_min = mesh->node[i].x[0];
            y_min = mesh->node[i].x[1];
            z_min = mesh->node[i].x[2];
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
          feenox_push_error_message("FIND_EXTREMA with OVER on a node-centered function not implemented yet.");
          return FEENOX_ERROR;
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
          
            feenox_mesh_update_coord_vars(mesh->node[j].x);
            y = feenox_expression_eval(expr);
            if (y > max) {
              max = y;
              i_max = element->node[j]->index_mesh;
              x_max = element->node[j]->x[0];
              y_max = element->node[j]->x[1];
              z_max = element->node[j]->x[2];
            }
            if (y < min) {
              min = y;
              i_min = element->node[j]->index_mesh;
              x_min = element->node[j]->x[0];
              y_min = element->node[j]->x[1];
              z_min = element->node[j]->x[2];
            }
        
          }
        }
      }
    }
  }  

  if (mesh_find_extrema->min != NULL) {
    feenox_var_value(mesh_find_extrema->min) = min;
  }
  if (mesh_find_extrema->i_min != NULL) {
    feenox_var_value(mesh_find_extrema->i_min) = (double)i_min;
  }
  if (mesh_find_extrema->x_min != NULL) {
    feenox_var_value(mesh_find_extrema->x_min) = x_min;
  }
  if (mesh_find_extrema->y_min != NULL) {
    feenox_var_value(mesh_find_extrema->y_min) = y_min;
  }
  if (mesh_find_extrema->z_min != NULL) {
    feenox_var_value(mesh_find_extrema->z_min) = z_min;
  }
  
  if (mesh_find_extrema->max != NULL) {
    feenox_var_value(mesh_find_extrema->max) = max;
  }
  if (mesh_find_extrema->i_max != NULL) {
    feenox_var_value(mesh_find_extrema->i_max) = (double)i_max;
  }
  if (mesh_find_extrema->x_max != NULL) {
    feenox_var_value(mesh_find_extrema->x_max) = x_max;
  }
  if (mesh_find_extrema->y_max != NULL) {
    feenox_var_value(mesh_find_extrema->y_max) = y_max;
  }
  if (mesh_find_extrema->z_max != NULL) {
    feenox_var_value(mesh_find_extrema->z_max) = z_max;
  }
  
  return FEENOX_OK;
}

