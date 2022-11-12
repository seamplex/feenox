/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's mesh-related function interpolation routines
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

struct mesh_interp_params {
  element_t *element;
  const double *x;
};


double feenox_mesh_interpolate_function_node(struct function_t *function, const double *x) {
  
  double r[3] = {0, 0, 0};    // vector with the local coordinates within the element
  double y, dist2;
  static element_t *element;
  node_t *nearest_node;
  mesh_t *mesh = function->mesh;  
  int j;

  
  if (function->data_value == NULL) {
    return 0;
  }

  // find the nearest node
  nearest_node = feenox_mesh_find_nearest_node(mesh, x);

  // check is it is close enough to a node
  switch (mesh->dim) {
    case 1:
      if ((dist2 = gsl_pow_2(fabs(x[0]-nearest_node->x[0]))) < gsl_pow_2(function->multidim_threshold)) {
        return function->data_value[nearest_node->index_mesh];
      }
    break;
    case 2:
      if ((dist2 = (feenox_mesh_subtract_squared_module2d(x, nearest_node->x))) < gsl_pow_2(function->multidim_threshold)) {
        return function->data_value[nearest_node->index_mesh];
      }
    break;
    case 3:
      if ((dist2 = (feenox_mesh_subtract_squared_module(x, nearest_node->x))) < gsl_pow_2(function->multidim_threshold)) {
        return function->data_value[nearest_node->index_mesh];
      }
    break;
  }

  if ((element = feenox_mesh_find_element(mesh, nearest_node, x)) != NULL) {
    if (feenox_mesh_interp_solve_for_r(element, x, r) != FEENOX_OK) {
      return 0;
    }
  } else {
    // should we return the nearest node value?
    return function->data_value[nearest_node->index_mesh];
  }
  
  // compute the interpolation
  y = 0;
  if (function->spatial_derivative_of == NULL) {
    
    for (j = 0; j < element->type->nodes; j++) {
      y += element->type->h(j, r) * function->data_value[element->node[j]->index_mesh];
    }
    
  } else {
    
    lowlevel_matrix_t *dhdx = feenox_lowlevel_matrix_calloc(element->type->nodes, element->type->dim);
    
    feenox_mesh_compute_dhdx(element, r, NULL, dhdx);
      
    for (j = 0; j < element->type->nodes; j++) {
      y += feenox_lowlevel_matrix_get(dhdx, j, function->spatial_derivative_with_respect_to)
            * function->spatial_derivative_of->data_value[element->node[j]->index_mesh];
    }
    
    feenox_lowlevel_matrix_free(&dhdx);
    
  }  
  
  return y;
 
}


int feenox_mesh_interp_solve_for_r(element_t *this, const double *x, double *r) {

  if (this->type->id == ELEMENT_TYPE_TETRAHEDRON4 || this->type->id == ELEMENT_TYPE_TETRAHEDRON10) {
    
    // the tetrahedron is an easy one (it's linear)
    // usually using this for tet10 is good enough
    // TODO: check error
    feenox_mesh_compute_r_tetrahedron(this, x, r);
    
  } else {
#ifdef HAVE_GSL    
    int gsl_status;
    int m;
    size_t iter = 0;  
    struct mesh_interp_params p;
    gsl_vector *test;
    gsl_multiroot_fdfsolver *s;  
    const gsl_multiroot_fdfsolver_type *T;
    gsl_multiroot_function_fdf fun = {&feenox_mesh_interp_residual,
                                    &feenox_mesh_interp_jacob,
                                    &feenox_mesh_interp_residual_jacob,
                                    this->type->dim, &p};
  
    p.element = this;
    p.x = x;
  
    test = gsl_vector_calloc(this->type->dim);    // guess inicial cero

//    T = gsl_multiroot_fsolver_hybrids;
//    T = gsl_multiroot_fsolver_hybrid;      
//    T = gsl_multiroot_fsolver_dnewton;
//    T = gsl_multiroot_fsolver_broyden;
    T = gsl_multiroot_fdfsolver_gnewton;
      
    s = gsl_multiroot_fdfsolver_alloc (T, this->type->dim);
    gsl_multiroot_fdfsolver_set (s, &fun, test);

    do {
      iter++;
      if ((gsl_status = gsl_multiroot_fdfsolver_iterate(s)) != GSL_SUCCESS) {
        return FEENOX_ERROR;
      }
      gsl_status = gsl_multiroot_test_residual(s->f, feenox_var_value(feenox.mesh.vars.eps));
    } while (gsl_status == GSL_CONTINUE && iter < 10);

    for (m = 0; m < this->type->dim; m++) {
      r[m] = gsl_vector_get(gsl_multiroot_fdfsolver_root(s), m);
    }  

    gsl_vector_free(test);
    gsl_multiroot_fdfsolver_free(s);
  
#else
  feenox_push_error_message("non-tet4 interpolation needs GSL");
  return FEENOX_ERROR;
#endif
  }
  
  return FEENOX_OK;
}




// vemos que r hace que las x se interpolen exactamente (isoparametricos)
int feenox_mesh_interp_residual(const lowlevel_vector_t *test, void *params, lowlevel_vector_t *residual) {
#ifdef HAVE_GSL
  int i, j;
  double xi;
  
  element_t *element = ((struct mesh_interp_params *)params)->element;
  const double *x = ((struct mesh_interp_params *)params)->x;

  for (i = 0; i < element->type->dim; i++) {
    xi = x[i];
    for (j = 0; j< element->type->nodes; j++) {
      xi -= element->type->h(j, (double *)gsl_vector_const_ptr(test, 0)) * element->node[j]->x[i];
    }
    gsl_vector_set(residual, i, xi);
  }
  
  return GSL_SUCCESS;
#else
  return FEENOX_OK;
#endif  
}


int feenox_mesh_interp_jacob(const lowlevel_vector_t *test, void *params, lowlevel_matrix_t *J) {
#ifdef HAVE_GSL
  int i, j, k;
  double xi;
  
  element_t *element = ((struct mesh_interp_params *)params)->element;

  for (i = 0; i < element->type->dim; i++) {
    for (j = 0; j < element->type->dim; j++) {
      xi = 0;
      for (k = 0; k < element->type->nodes; k++) {
        // es negativo por como definimos el residuo
        // el cast explicito es para sacarnos de encima el const
        xi -= element->type->dhdr(k, j, (double *)gsl_vector_const_ptr(test, 0)) * element->node[k]->x[i];
      }
      gsl_matrix_set(J, i, j, xi);
    }
  }
     
  return GSL_SUCCESS;
#else
  return FEENOX_OK;
#endif  
}


int feenox_mesh_interp_residual_jacob(const lowlevel_vector_t *test, void *params, lowlevel_vector_t *residual, lowlevel_matrix_t * J) {
#ifdef HAVE_GSL
  // can this be improved?
  feenox_mesh_interp_residual(test, params, residual);
  feenox_mesh_interp_jacob(test, params, J);
  
  return GSL_SUCCESS;
#else
  return FEENOX_OK;
#endif  
}





int feenox_mesh_compute_r_tetrahedron(element_t *this, const double *x, double *r) {

  int j, j_prime;
  
  
  if (this->type->id == ELEMENT_TYPE_TETRAHEDRON4 || this->type->id == ELEMENT_TYPE_TETRAHEDRON10) {
    
    // reference: eq (9.11) from AFEM.Ch09
    
//    double xi0, one;
    double sixV;
//    double sixV01;
    double sixV02, sixV03, sixV04;

    // porque ya teniamos todo desde 1    
    double dx[5][5];
    double dy[5][5];
    double dz[5][5];
    for (j = 0; j < 4; j++) {
      for (j_prime = 0; j_prime < 4; j_prime++) {
        dx[j+1][j_prime+1] = this->node[j]->x[0] - this->node[j_prime]->x[0];
        dy[j+1][j_prime+1] = this->node[j]->x[1] - this->node[j_prime]->x[1];
        dz[j+1][j_prime+1] = this->node[j]->x[2] - this->node[j_prime]->x[2];
      }
    }
  
    // arrancan en uno
    sixV = dx[2][1] * (dy[2][3] * dz[3][4] - dy[3][4] * dz[2][3] ) + dx[3][2] * (dy[3][4] * dz[1][2] - dy[1][2] * dz[3][4] ) + dx[4][3] * (dy[1][2] * dz[2][3] - dy[2][3] * dz[1][2]);
 
    // estos si arrancan en cero
//    sixV01 = this->node[1]->x[0] * (this->node[2]->x[1]*this->node[3]->x[2] - this->node[3]->x[1]*this->node[2]->x[2]) + this->node[2]->x[0] * (this->node[3]->x[1]*this->node[1]->x[2] - this->node[1]->x[1]*this->node[3]->x[2]) + this->node[3]->x[0] * (this->node[1]->x[1]*this->node[2]->x[2] - this->node[2]->x[1]*this->node[1]->x[2]);
    sixV02 = this->node[0]->x[0] * (this->node[3]->x[1]*this->node[2]->x[2] - this->node[2]->x[1]*this->node[3]->x[2]) + this->node[2]->x[0] * (this->node[0]->x[1]*this->node[3]->x[2] - this->node[3]->x[1]*this->node[0]->x[2]) + this->node[3]->x[0] * (this->node[2]->x[1]*this->node[0]->x[2] - this->node[0]->x[1]*this->node[2]->x[2]);
    sixV03 = this->node[0]->x[0] * (this->node[1]->x[1]*this->node[3]->x[2] - this->node[3]->x[1]*this->node[1]->x[2]) + this->node[1]->x[0] * (this->node[3]->x[1]*this->node[0]->x[2] - this->node[0]->x[1]*this->node[3]->x[2]) + this->node[3]->x[0] * (this->node[0]->x[1]*this->node[1]->x[2] - this->node[1]->x[1]*this->node[0]->x[2]);
    sixV04 = this->node[0]->x[0] * (this->node[2]->x[1]*this->node[1]->x[2] - this->node[1]->x[1]*this->node[2]->x[2]) + this->node[1]->x[0] * (this->node[0]->x[1]*this->node[2]->x[2] - this->node[2]->x[1]*this->node[0]->x[2]) + this->node[2]->x[0] * (this->node[1]->x[1]*this->node[0]->x[2] - this->node[0]->x[1]*this->node[1]->x[2]);
    
    // otra vez en uno
//    xi0 =                1.0/sixV * (sixV01 * 1 + (dy[4][2]*dz[3][2] - dy[3][2]*dz[4][2])*gsl_vector_get(x, 0) + (dx[3][2]*dz[4][2] - dx[4][2]*dz[3][2])*gsl_vector_get(x, 1) + (dx[4][2]*dy[3][2] - dx[3][2]*dy[4][2])*gsl_vector_get(x, 2));
    r[0] = 1.0/sixV * (sixV02 * 1 + (dy[3][1]*dz[4][3] - dy[3][4]*dz[1][3])*x[0] + (dx[4][3]*dz[3][1] - dx[1][3]*dz[3][4])*x[1] + (dx[3][1]*dy[4][3] - dx[3][4]*dy[1][3])*x[2]);
    r[1] = 1.0/sixV * (sixV03 * 1 + (dy[2][4]*dz[1][4] - dy[1][4]*dz[2][4])*x[0] + (dx[1][4]*dz[2][4] - dx[2][4]*dz[1][4])*x[1] + (dx[2][4]*dy[1][4] - dx[1][4]*dy[2][4])*x[2]);
    r[2] = 1.0/sixV * (sixV04 * 1 + (dy[1][3]*dz[2][1] - dy[1][2]*dz[3][1])*x[0] + (dx[2][1]*dz[1][3] - dx[3][1]*dz[1][2])*x[1] + (dx[1][3]*dy[2][1] - dx[1][2]*dy[3][1])*x[2]);
    
/*    
    one = xi0 + gsl_vector_get(r,0) + gsl_vector_get(r,1) + gsl_vector_get(r,2);
    if (gsl_fcmp(one, 1.0, 1e-3) != 0) {
      printf("internal mismatch when computing canonical coordinates in element %d\n", this->id);
      return FEENOX_ERROR;
    }
 */
    
  } else {
    feenox_push_error_message("not for element type %d", this->type->id) ;
    return FEENOX_ERROR;
  }

  return FEENOX_OK;
}



/*
double mesh_interpolate_function_cell(struct function_t *function, const double *x) {
  
  int i;
  static cell_t *chosen_cell;
  node_t *nearest_node;
  element_list_item_t *associated_element;
  mesh_t *mesh = function->mesh;


  if (mesh->kd_nodes != NULL) {
    nearest_node = (node_t *)(kd_res_item_data(kd_nearest(mesh->kd_nodes, x)));
    chosen_cell = NULL;
    LL_FOREACH(nearest_node->associated_elements, associated_element) {
      if (associated_element->element->type->dim == mesh->bulk_dimensions && associated_element->element->type->point_in_element(associated_element->element, x)) {
        chosen_cell = associated_element->element->cell;
        break;
      }
    }
  }

  // primero probamos la ultima celda;
  if (chosen_cell == NULL || chosen_cell->element->type->point_in_element(chosen_cell->element, x) == 0) {
    chosen_cell = NULL;
    // y si no barremos hasta que lo encontramos
    for (i = 0; i < mesh->n_cells; i++) {
      if (mesh->cell[i].element->type->point_in_element(mesh->cell[i].element, x)) {
        chosen_cell = &mesh->cell[i];
        break;
      }
    }
  }
  
  if (chosen_cell == NULL) {
    return 0;
  }

  return function->data_value[chosen_cell->id - 1];
 
}
*/

double feenox_function_property_eval(struct function_t *function, const double *x) {
  double y = 0;
  property_data_t *property_data = NULL;
  property_t *property = function->property;
  mesh_t *mesh = function->mesh;

  // if there is no mesh it could be that it has not been read
  // this is not necessarily an error
  if (mesh == NULL || mesh->n_nodes == 0) {
    return 1.0;
  }
  
  element_t *element = feenox_mesh_find_element(function->mesh, NULL, x);
  if (element != NULL && element->physical_group != NULL && element->physical_group->material != NULL) {
    HASH_FIND_STR(element->physical_group->material->property_datums, property->name, property_data);
    if (property_data != NULL) {
      feenox_var_value(feenox.mesh.vars.x) = x[0];
      if (function->n_arguments > 1) {
        feenox_var_value(feenox.mesh.vars.y) = x[1];
        if (function->n_arguments > 2) {
          feenox_var_value(feenox.mesh.vars.z) = x[2];
        }  
      }

      // finally evalaute the expression of the material found
      y = feenox_expression_eval(&property_data->expr);
    }
  }
  return y;
}

