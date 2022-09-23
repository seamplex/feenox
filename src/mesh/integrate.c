/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's mesh-related integration routines
 *
 *  Copyright (C) 2016,2018,2021 jeremy theler
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

int feenox_instruction_mesh_integrate(void *arg) {

  mesh_integrate_t *mesh_integrate = (mesh_integrate_t *)arg;
  mesh_t *mesh = mesh_integrate->mesh;
  function_t *function = mesh_integrate->function;
  expr_t *expr = &mesh_integrate->expr;
  physical_group_t *physical_group = mesh_integrate->physical_group;
  var_t *result = mesh_integrate->result;
  element_t *element = NULL;
  double integral = 0;
  
  // TODO: take into account expressions of x and nx
  // TODO: make one actual function for each field location and function type, virtual?
  // TODO: choose dimension?
  if (function != NULL) {
    if (mesh_integrate->field_location == field_location_cells) {
      // cell-based functions
      if (function->type == function_type_pointwise_mesh_cell && function->mesh == mesh) {
        // function defined over a cell integrated over a cell, cool!
        size_t i = 0;
        for (i = 0; i < mesh->n_cells; i++) {
          element = mesh->cell[i].element;
          if ((physical_group == NULL && element->type->dim == mesh->dim) ||
              (physical_group != NULL && element->physical_group == physical_group)) {
            integral += function->data_value[i] * mesh->cell[i].element->type->element_volume(mesh->cell[i].element);
          }
        }
      } else {
        // location is cell but function is not cell
        size_t i = 0;
        for (i = 0; i < mesh->n_cells; i++) {
          element = mesh->cell[i].element;
          if ((physical_group == NULL && element->type->dim == mesh->dim) ||
              (physical_group != NULL && element->physical_group == physical_group)) {
            integral += feenox_function_eval(function, mesh->cell[i].x) * mesh->cell[i].element->type->element_volume(mesh->cell[i].element);
          }
        }
      }
    } else {
      // not cell-based functions
      if (function->type == function_type_pointwise_mesh_node && function->mesh == mesh) {
        // funcion mesh node, pretty cool
        
        // check if the time is the correct one
/*        
        if (function->name_in_mesh != NULL && function->mesh->format == mesh_format_gmsh &&
            function->mesh_time < feenox_var_value(feenox_special_var(t))-0.001*feenox_var_value(feenox_special_var(dt))) {
          feenox_call(mesh_gmsh_update_function(function, feenox_var_value(feenox_special_var(t)), feenox_var_value(feenox_special_var(dt))));
          function->mesh_time = feenox_var_value(feenox_special_var(t));
        }
*/      
        
        double xi = 0;
        unsigned int v = 0;
        unsigned int j = 0;
        size_t i = 0;
        for (i = 0; i < mesh->n_elements; i++) {
          element = &mesh->element[i];
          if ((physical_group == NULL && element->type->dim == mesh->dim) ||
              (physical_group != NULL && element->physical_group == physical_group)) {
            for (v = 0; v < element->type->gauss[mesh->integration].V; v++) {
              feenox_mesh_compute_w_at_gauss(element, v, mesh->integration);

              xi = 0;
              for (j = 0; j < element->type->nodes; j++) {
                xi += element->type->gauss[mesh->integration].h[v][j] * function->data_value[element->node[j]->tag - 1];
              }

              integral += element->w[v] * xi;
            }
          }
        }
      } else {
        // general function
        size_t i = 0;
        unsigned int v = 0;
        for (i = 0; i < mesh->n_elements; i++) {
          element = &mesh->element[i];
          if ((physical_group == NULL && element->type->dim == mesh->dim) ||
              (physical_group != NULL && element->physical_group == physical_group)) {
            for (v = 0; v < element->type->gauss[mesh->integration].V; v++) {
              feenox_mesh_compute_w_at_gauss(element, v, mesh->integration);
              // TODO: check if the integrand depends on space
              feenox_mesh_compute_x_at_gauss(element, v, mesh->integration);
              integral += element->w[v] * feenox_function_eval(mesh_integrate->function, element->x[v]);
            }  
          }
        }
      }
    }
  } else {
    if (mesh_integrate->field_location == field_location_cells) {
      // an expression integrated over cells
      size_t i;
      for (i = 0; i < mesh->n_cells; i++) {
        element = mesh->cell[i].element;
        if ((physical_group == NULL && element->type->dim == mesh->dim) ||
            (physical_group != NULL && element->physical_group == physical_group)) {
          // TODO: inlined function
          feenox_mesh_update_coord_vars(mesh->cell[i].x);
          integral += feenox_expression_eval(expr) * mesh->cell[i].element->type->element_volume(mesh->cell[i].element);
        }
      }
    } else {
      // an expression evaluated at the nodes
      unsigned int v = 0;
      size_t i = 0;
      for (i = 0; i < mesh->n_elements; i++) {
        element = &mesh->element[i];
        if ((physical_group == NULL && element->type->dim == mesh->dim) ||
            (physical_group != NULL && element->physical_group == physical_group)) {
          for (v = 0; v < element->type->gauss[mesh->integration].V; v++) {
            feenox_mesh_compute_w_at_gauss(element, v, mesh->integration);
            // TODO: check is the integrand depends on space
            feenox_mesh_compute_x_at_gauss(element, v, mesh->integration);
            feenox_mesh_update_coord_vars(element->x[v]);
            // si el elemento es de linea o de superficie calculamos la normal para tenerla en nx, ny y nz
            // TODO: nx
/*            
            if (mesh->dim - element->type->dim == 1) {
              feenox_call(mesh_compute_normal(element));
            }  
 */
            double xi = feenox_expression_eval(expr);
            integral += element->w[v] * xi;
          }
        }
      }        
    }
  }  
  
  if (result != NULL) {
    feenox_var_value(result) = integral;
  }  

  return FEENOX_OK;
}



double feenox_mesh_integral_over_element(element_t *this, mesh_t *mesh, function_t *function) {

  double integral = 0;
  double xi = 0;

  unsigned int v = 0;
  unsigned int j = 0;
  for (v = 0; v < this->type->gauss[mesh->integration].V; v++) {
    feenox_mesh_compute_w_at_gauss(this, v, mesh->integration);
 
    xi = 0;
    for (j = 0; j < this->type->nodes; j++) {
      xi += this->type->gauss[mesh->integration].h[v][j] * function->data_value[this->node[j]->index_mesh];
    }

    integral += this->w[v] * xi;
  }

  return integral;

}
