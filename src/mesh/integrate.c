/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's mesh-related integration routines
 *
 *  Copyright (C) 2016,2018,2022--2023 Jeremy Theler
 *
 *  This file is part of feenox.
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
 *  along with feenox.  If not, see <http://www.gnu.org/licenses/>.
 *------------------- ------------  ----    --------  --     -       -         -
 */
#include <feenox.h>

#define is_physical_group_right(elm, p_grp) \
        (((p_grp) == NULL && (elm)->type->dim == mesh->dim) || \
         ((p_grp) != NULL && (elm)->physical_group == p_grp))

int feenox_instruction_mesh_integrate(void *arg) {

  mesh_integrate_t *mesh_integrate = (mesh_integrate_t *)arg;
  mesh_t *mesh = mesh_integrate->mesh;
  function_t *function = mesh_integrate->function;
  expr_t *expr = &mesh_integrate->expr;
  physical_group_t *physical_group = mesh_integrate->physical_group;
  var_t *result = mesh_integrate->result;
  double integral_local = 0;
  double integral_global = 0;

#ifdef HAVE_PETSC
  if (feenox.pde.petscinit_called == PETSC_FALSE) {
    feenox_problem_parse_time_init();
  }
#endif
  
  if (mesh->mpi_matches_partitions == mpi_matches_partitions_no) {
    feenox_call(feenox_compute_first_last_element(mesh));
  }
  
  // TODO: virtuals!
  
  // TODO: take into account expressions of x and nx
  // TODO: choose dimension?
  if (function != NULL) {
    if (mesh_integrate->field_location == field_location_cells) {
      // cell-based integration of functions
      if (function->type == function_type_pointwise_mesh_cell && function->mesh == mesh) {
        integral_local = feenox_mesh_integral_function_cell_cell(function, mesh, physical_group);
      } else {
        integral_local = feenox_mesh_integral_function_general_cell(function, mesh, physical_group);
      }
    } else {
      // gauss-based integration of functions
      if (function->type == function_type_pointwise_mesh_node && function->mesh == mesh) {
        // check if the time is the correct one
/*        
        if (function->name_in_mesh != NULL && function->mesh->format == mesh_format_gmsh &&
            function->mesh_time < feenox_var_value(feenox_special_var(t))-0.001*feenox_var_value(feenox_special_var(dt))) {
          feenox_call(mesh_gmsh_update_function(function, feenox_var_value(feenox_special_var(t)), feenox_var_value(feenox_special_var(dt))));
          function->mesh_time = feenox_var_value(feenox_special_var(t));
        }
*/      
        integral_local = feenox_mesh_integral_function_node_gauss(function, mesh, physical_group);
      } else {
        integral_local = feenox_mesh_integral_function_general_gauss(function, mesh, physical_group);;
      }
    }
  } else {
    if (mesh_integrate->field_location == field_location_cells) {
      integral_local = feenox_mesh_integral_expression_cell(expr, mesh, physical_group);
    } else {
      integral_local = feenox_mesh_integral_expression_gauss(expr, mesh, physical_group);
    }
  }  


#ifdef HAVE_PETSC  
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-value"
  MPI_Allreduce(&integral_local, &integral_global, 1, MPIU_SCALAR, MPIU_SUM, PETSC_COMM_WORLD);
#pragma GCC diagnostic pop
#else
  integral_global = integral_local;    
#endif
  
  if (result != NULL) {
    feenox_var_value(result) = integral_global;
  }  

  return FEENOX_OK;
}



double feenox_mesh_integral_over_element(element_t *this, mesh_t *mesh, function_t *function) {

  double integral = 0;

  for (unsigned int q = 0; q < this->type->gauss[mesh->integration].Q; q++) {
    double wdet = feenox_fem_compute_w_det_at_gauss_integration(this, q, mesh->integration);
    double val = 0;
    for (unsigned int j = 0; j < this->type->nodes; j++) {
      val += gsl_matrix_get(this->type->gauss[mesh->integration].H_c[q], 0, j) * feenox_vector_get(function->vector_value, this->node[j]->index_mesh);
    }

    integral += wdet * val;
  }

  return integral;

}


double feenox_mesh_integral_function_cell_cell(function_t *function, mesh_t *mesh, physical_group_t *physical_group) {
  
  // function defined over a cell integrated over a cell, cool!
  double integral = 0;

  for (size_t i = 0; i < mesh->n_cells; i++) {
    element_t *e = mesh->cell[i].element;
    if (is_element_local(mesh, e) && is_physical_group_right(e, physical_group)) {
      integral += feenox_vector_get(function->vector_value, i) * mesh->cell[i].element->type->volume(mesh->cell[i].element);
    }
  }

  return integral;
}

double feenox_mesh_integral_function_general_cell(function_t *function, mesh_t *mesh, physical_group_t *physical_group) {

  // location is cell but function is not cell
  double integral = 0;
  double n[3] = {0, 0, 0};
  int depends_on_normal = feenox_expression_depends_on_normal(&function->algebraic_expression);

  for (size_t i = 0; i < mesh->n_cells; i++) {
    element_t *e = mesh->cell[i].element;
    if (is_element_local(mesh, e) && is_physical_group_right(e, physical_group)) {
      if (depends_on_normal) {
        feenox_call(feenox_mesh_compute_outward_normal(e, n));
      }  
      integral += feenox_function_eval(function, mesh->cell[i].x) * mesh->cell[i].element->type->volume(mesh->cell[i].element);
    }
  }

  return integral;
}


double feenox_mesh_integral_function_node_gauss(function_t *function, mesh_t *mesh, physical_group_t *physical_group) {
  
  // funcion mesh node, pretty cool
  double integral = 0;

  for (size_t i = 0; i < mesh->n_elements; i++) {
    element_t *e = &mesh->element[i];
    if (is_element_local(mesh, e) && is_physical_group_right(e, physical_group)) {
      for (unsigned int q = 0; q < e->type->gauss[mesh->integration].Q; q++) {
        double wdet = feenox_fem_compute_w_det_at_gauss_integration(e, q, mesh->integration);

        double xi = 0;
        for (unsigned int j = 0; j < e->type->nodes; j++) {
          xi += gsl_matrix_get(e->type->gauss[mesh->integration].H_c[q], 0, j) * feenox_vector_get(function->vector_value, e->node[j]->index_mesh);
        }
        integral += wdet * xi;
      }
    }
  }

  return integral;
}


double feenox_mesh_integral_function_general_gauss(function_t *function, mesh_t *mesh, physical_group_t *physical_group) {

  // general function
  double n[3] = {0, 0, 0};
  double integral = 0;
  int depends_on_normal = feenox_expression_depends_on_normal(&function->algebraic_expression);

  for (size_t i = 0; i < mesh->n_elements; i++) {
    element_t *e = &mesh->element[i];
    if (is_element_local(mesh, e) && is_physical_group_right(e, physical_group)) {
      for (unsigned int q = 0; q < e->type->gauss[mesh->integration].Q; q++) {
        double *x = feenox_fem_compute_x_at_gauss(e, q, mesh->integration);
        if (depends_on_normal) {
          feenox_call(feenox_mesh_compute_outward_normal(e, n));
        }  
        double wdet = feenox_fem_compute_w_det_at_gauss_integration(e, q, mesh->integration);
        integral += wdet * feenox_function_eval(function, x);
      }  
    }
  }

  return integral;
}


double feenox_mesh_integral_expression_cell(expr_t *expr, mesh_t *mesh, physical_group_t *physical_group) {

  double integral = 0;
  
  // an expression integrated over cells
  for (size_t i = 0; i < mesh->n_cells; i++) {
    element_t *e = mesh->cell[i].element;
    if (is_element_local(mesh, e) && is_physical_group_right(e, physical_group)) {
      feenox_fem_update_coord_vars(mesh->cell[i].x);
      integral += feenox_expression_eval(expr) * mesh->cell[i].element->type->volume(mesh->cell[i].element);
    }
  }
  return integral;
}

double feenox_mesh_integral_expression_gauss(expr_t *expr, mesh_t *mesh, physical_group_t *physical_group) {

  double integral = 0;
  double n[3] = {0,0,0};
  
  int depends_on_normal = feenox_expression_depends_on_normal(expr);
  
  // an expression evaluated at the gauss points
  for (size_t i = 0; i < mesh->n_elements; i++) {
    element_t *e = &mesh->element[i];
    if (is_element_local(mesh, e) && is_physical_group_right(e, physical_group)) {
      
      for (unsigned int q = 0; q < e->type->gauss[mesh->integration].Q; q++) {
        feenox_fem_compute_x_at_gauss_and_update_var(e, q, mesh->integration);
        if (depends_on_normal) {
          feenox_call(feenox_mesh_compute_outward_normal(e, n));
        }  
        integral += feenox_fem_compute_w_det_at_gauss_integration(e, q, mesh->integration) * feenox_expression_eval(expr);
      }
    }
  }
 
// small debug-session for my phd thesis  
//  petsc_call(PetscSynchronizedPrintf(PETSC_COMM_WORLD, "[proceso %d] mi integral parcial es %g\n", feenox.mpi_rank, integral));
//  petsc_call(PetscSynchronizedFlush(PETSC_COMM_WORLD, PETSC_STDOUT));    
  
  return integral;
}
