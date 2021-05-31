/*------------ -------------- -------- --- ----- ---   --       -            -
 *  FeenoX mathematical functions evaluation
 *
 *  Copyright (C) 2009--2020 jeremy theler
 *
 *  This file is part of feenox.
 *
 *  FeenoX is free software: you can redistribute it and/or modify
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
extern feenox_t feenox;

int feenox_define_function(const char *name, unsigned int n_arguments) {
  return (feenox_define_function_get_ptr(name, n_arguments) != NULL) ? FEENOX_OK : FEENOX_ERROR;
}

  
function_t *feenox_define_function_get_ptr(const char *name, unsigned int n_arguments) {

  if (feenox_check_name(name) != FEENOX_OK) {
    return NULL;
  }

  function_t *function = NULL;
  feenox_check_alloc_null(function = calloc(1, sizeof(function_t)));
  feenox_check_alloc_null(function->name = strdup(name));
  function->n_arguments = n_arguments;
  
  feenox_check_alloc_null(function->var_argument = calloc(n_arguments, sizeof(var_t *)));
  function->var_argument_allocated = 1;
  
  // default columns
  feenox_check_alloc_null(function->column = calloc((n_arguments+1), sizeof(int)));
  int i;
  for (i = 0; i < n_arguments+1; i++) {
    function->column[i] = i+1;
  }
  
  HASH_ADD_KEYPTR(hh, feenox.functions, function->name, strlen(function->name), function);

  return function;
}

int feenox_function_set_argument_variable(const char *name, unsigned int i, const char *variable_name) {
  
  function_t *function;
  if ((function = feenox_get_function_ptr(name)) == NULL) {
    feenox_push_error_message("unkown function '%s'", name);
    return FEENOX_ERROR;
  }
  
  if (i < 0) {
    feenox_push_error_message("negative argument number '%d'", i);
    return FEENOX_ERROR;
  }
  if (i >= function->n_arguments) {
    feenox_push_error_message("argument number '%d' greater or equal than the number of arguments '%d' (they start from zero)", i, function->n_arguments);
    return FEENOX_ERROR;
  }
  
  if ((function->var_argument[i] = feenox_get_or_define_variable_get_ptr(variable_name)) == NULL) {
    return FEENOX_ERROR;
  }
  
  // mark that we received an argument to see if we got them all when initializing
  function->n_arguments_given++;
  
  return FEENOX_OK;
}


int feenox_function_set_expression(const char *name, const char *expression) {
  
  function_t *function = NULL;
  if ((function = feenox_get_function_ptr(name)) == NULL) {
    feenox_push_error_message("unkown function '%s'", name);
    return FEENOX_ERROR;
  }
  
  function->type = function_type_algebraic;
  feenox_call(feenox_expression_parse(&function->algebraic_expression, expression));  
  
  return FEENOX_OK;
}

int feenox_function_set_interpolation(const char *name, const char *type) {

  function_t *function = NULL;
  if ((function = feenox_get_function_ptr(name)) == NULL) {
    feenox_push_error_message("unkown function '%s'", name);
    return FEENOX_ERROR;
  }
  
  if (strcasecmp(type, "linear") == 0) {
    function->interp_type = *gsl_interp_linear;
  } else if (strcasecmp(type, "polynomial") == 0) {
    function->interp_type = *gsl_interp_polynomial;
  } else if (strcasecmp(type, "spline") == 0 || strcasecmp(type, "cspline") == 0 || strcasecmp(type, "splines") == 0) {
    function->interp_type = *gsl_interp_cspline;
  } else if (strcasecmp(type, "spline_periodic") == 0 || strcasecmp(type, "cspline_periodic") == 0 || strcasecmp(type, "splines_periodic") == 0) {
    function->interp_type = *gsl_interp_cspline_periodic;
  } else if (strcasecmp(type, "akima") == 0) {
    function->interp_type = *gsl_interp_akima;
  } else if (strcasecmp(type, "akima_periodic") == 0) {
    function->interp_type = *gsl_interp_akima_periodic;
  } else if (strcasecmp(type, "steffen") == 0) {
    function->interp_type = *gsl_interp_steffen;
  } else if (strcasecmp(type, "nearest") == 0) {
    function->multidim_interp = interp_nearest;
  } else if (strcasecmp(type, "shepard") == 0) {
    function->multidim_interp = interp_shepard;
  } else if (strcasecmp(type, "shepard_kd") == 0 || strcasecmp(type, "modified_shepard") == 0) {
    function->multidim_interp = interp_shepard_kd;
  } else if (strcasecmp(type, "bilinear") == 0 || strcasecmp(type, "rectangle") == 0 || strcasecmp(type, "rectangular") == 0) {
    function->multidim_interp = interp_bilinear;
  } else {
    feenox_push_error_message("undefined interpolation method '%s'", type);
    return FEENOX_ERROR;
  }
  
  return FEENOX_OK;
  
}

// set the variables that are a function's argument equal to the vector x
void feenox_function_set_args(function_t *this, double *x) {
  int i;
  
  if (this->var_argument != NULL) {
    for (i = 0; i < this->n_arguments; i++) {
      if (this->var_argument[i] != NULL) { 
        feenox_var_value(this->var_argument[i]) = x[i];
      }
    }
  }
  
  return;
  
}

// evaluates a function inside a factor
// the arguments are inside the structure
double feenox_factor_function_eval(expr_item_t *this) {

  double y = 0;

  // in order to avoid having to allocate and free too much,
  // one-dimensional functions are treated differently
  if (this->function->n_arguments == 1) {
    
    double x0 = feenox_expression_eval(&this->arg[0]);
    y = feenox_function_eval(this->function, &x0);

  } else {

    double *x = NULL;
    feenox_check_alloc(x = malloc(this->function->n_arguments*sizeof(double)));

    unsigned int i;
    for (i = 0; i < this->function->n_arguments; i++) {
      x[i] = feenox_expression_eval(&this->arg[i]);
    }

    y = feenox_function_eval(this->function, x);
    feenox_free(x);
    
  }

  if (gsl_isnan(y) || gsl_isinf(y)) {
    feenox_nan_error();
  }

  return y;

}


int feenox_function_init(function_t *this) {

  if (this->initialized) {
    return 0;
  }
  
  // TODO: arrange into smaller functions
  // TODO: initialize mesh-based functions
  if (this->type == function_type_algebraic && this->n_arguments_given != this->n_arguments) {
    feenox_push_error_message("algebraic function '%s' needs %d arguments and %d were given", this->name, this->n_arguments, this->n_arguments_given);
    return FEENOX_ERROR;
  }
  
/*  
  if (this->type == function_type_pointwise_vector) {
    unsigned int i = 0;
    for (i = 0; i < this->n_arguments; i++) {
      
      if (!this->vector_argument[i]->initialized) {
        feenox_call(feenox_vector_init(this->vector_argument[i]));
      }
      
      if (this->data_size == 0) {
        this->data_size = this->vector_argument[i]->size;
      } else {
        if (this->vector_argument[i]->size != this->data_size) {
          feenox_push_error_message("vector sizes do not match (%d and %d) in function '%s'", this->data_size, this->vector_argument[i]->size, this->name);
          return FEENOX_ERROR;
        }
      }

      this->data_argument[i] = gsl_vector_ptr(&feenox_var_value(this->vector_argument[i]), 0);
    }
  }
  
  if (this->vector_value != NULL) {

    if (!this->vector_value->initialized) {
      feenox_call(feenox_vector_init(this->vector_value));
    }
    if (this->vector_value->size != this->data_size) {
      feenox_push_error_message("vector sizes do not match (%d and %d)", this->data_size, this->vector_value->size);
      return FEENOX_ERROR;
    }
    
    // rellenamos las variables f_a f_b y f_c
    if (this->n_arguments == 1) {
      var_t *dummy_var;
      
      // TODO: asprintf
      char *dummy_aux = malloc(strlen(this->name) + 4);
      
      sprintf(dummy_aux, "%s_a", this->name);
      if ((dummy_var = feenox_get_variable_ptr(dummy_aux)) == NULL) {
        return FEENOX_ERROR;
      }
      feenox_realloc_variable_ptr(dummy_var, &this->data_argument[0][0], 0);
      
      sprintf(dummy_aux, "%s_b", this->name);
      if ((dummy_var = feenox_get_variable_ptr(dummy_aux)) == NULL) {
        return FEENOX_ERROR;
      }
      feenox_realloc_variable_ptr(dummy_var, &this->data_argument[0][this->data_size-1], 0);
      
      sprintf(dummy_aux, "%s_n", this->name);
      if ((dummy_var = feenox_get_variable_ptr(dummy_aux)) == NULL) {
        return FEENOX_ERROR;
      }
      feenox_free(dummy_aux);
      feenox_var_value(dummy_var) = (double)this->data_size;
    }

    this->data_value = gsl_vector_ptr(&feenox_var_value(this->vector_value), 0);

  }
*/
  
  if (this->data_size != 0) {
    this->multidim_threshold = (this->expr_multidim_threshold.items != NULL) ? feenox_expression_eval(&this->expr_multidim_threshold) : DEFAULT_MULTIDIM_INTERPOLATION_THRESHOLD;
    if (this->mesh == NULL) {
      if (this->n_arguments == 1) {

        if (this->interp_type.name == NULL) {
          // interpolacion 1D por default 
          this->interp_type = DEFAULT_INTERPOLATION;      
        }

        if ((this->interp = gsl_interp_alloc(&this->interp_type, this->data_size)) == NULL) {
          feenox_push_error_message("interpolation method for function '%s' needs more points", this->name);
          return FEENOX_ERROR;
        }

        this->interp_accel = gsl_interp_accel_alloc();

        // si no es de tipo vector, inicializamos el interpolador (los datos quedan fijos) 
        // porque si es vector y los vectores tienen fruta, el interpolador se queja 
        if (this->type != function_type_pointwise_vector) {
          feenox_push_error_message("in function %s", this->name);
          gsl_interp_init(this->interp, this->data_argument[0], this->data_value, this->data_size);
          feenox_pop_error_message();
        }

      } else {

        this->shepard_radius = (this->expr_shepard_radius.items != NULL) ? feenox_expression_eval(&this->expr_shepard_radius) : DEFAULT_SHEPARD_RADIUS;
        this->shepard_exponent = (this->expr_shepard_exponent.items != NULL) ? feenox_expression_eval(&this->expr_shepard_exponent) : DEFAULT_SHEPARD_EXPONENT;

        if (this->n_arguments == 2) {

          // miramos si tiene una malla regular en 2d
          int nx, ny;
          if (feenox_is_structured_grid_2d(this->data_argument[0], this->data_argument[1], this->data_size, &nx, &ny)) {
            this->rectangular_mesh = 1;
            this->x_increases_first = 1;
          } else if (feenox_is_structured_grid_2d(this->data_argument[1], this->data_argument[0], this->data_size, &ny, &nx)) {
            this->rectangular_mesh = 1;
            this->x_increases_first = 0;
          } else {
            this->rectangular_mesh = 0;
          }

          if (this->rectangular_mesh) {
            this->rectangular_mesh_size = malloc(2*sizeof(int));
            this->rectangular_mesh_size[0] = nx;
            this->rectangular_mesh_size[1] = ny;
          }

        } else if (this->n_arguments == 3) {

          // miramos si tiene una malla regular en 3d
          int nx, ny, nz;
          if (feenox_is_structured_grid_3d(this->data_argument[0], this->data_argument[1], this->data_argument[2], this->data_size, &nx, &ny, &nz)) {
            this->rectangular_mesh = 1;
            this->x_increases_first = 1;
          } else if (feenox_is_structured_grid_3d(this->data_argument[2], this->data_argument[1], this->data_argument[0], this->data_size, &nz, &ny, &nx)) {
            this->rectangular_mesh = 1;
            this->x_increases_first = 0;
          } else {
            this->rectangular_mesh = 0;
          }

          if (this->rectangular_mesh) {
            this->rectangular_mesh_size = malloc(3*sizeof(int));
            this->rectangular_mesh_size[0] = nx;
            this->rectangular_mesh_size[1] = ny;
            this->rectangular_mesh_size[2] = nz;
          }

        } else {

          // estamos en el caso de mayor dimension que 3
          // confiamos de una que es rectangular
          this->rectangular_mesh = 1;

          if (this->expr_x_increases_first.items != NULL) {
            this->x_increases_first = feenox_expression_eval(&this->expr_x_increases_first);
          } else {
            feenox_push_error_message("missing expression for X_INCREASES_FIRST keyword");
            return FEENOX_ERROR;
          }

          this->rectangular_mesh_size = malloc(this->n_arguments*sizeof(int));

          if (this->expr_rectangular_mesh_size != NULL) {
            unsigned int i = 0;
            for (i = 0; i < this->n_arguments; i++) {
              if (this->expr_rectangular_mesh_size[i].items != NULL) {
                this->rectangular_mesh_size[i] = round(feenox_expression_eval(&this->expr_rectangular_mesh_size[i]));
                if (this->rectangular_mesh_size[i] < 2) {
                  feenox_push_error_message("size %d for argument number %d in function '%s' cannot be less than two", this->rectangular_mesh_size[i], i+1, this->name);
                  return FEENOX_ERROR;
                }
              }
            }
          } else {
            feenox_push_error_message("missing expressions for SIZES keyword");
            return FEENOX_ERROR;
          }
        }

        if (this->rectangular_mesh) {

          int step;

          // checkeamos solo el size (porq el usuario pudo meter fruta para dimension mayor a 3)
          unsigned int i = 0;
          unsigned int j = 1;
          for (i = 0; i < this->n_arguments; i++) {
            j *= this->rectangular_mesh_size[i];
          }
          if (this->data_size != j) {
            feenox_push_error_message("data size of function %s do not match with the amount of given values", this->name);
            return FEENOX_ERROR;
          }

          this->rectangular_mesh_point = malloc(this->n_arguments*sizeof(double *));
          for (i = 0; i < this->n_arguments; i++) {
            this->rectangular_mesh_point[i] = malloc(this->rectangular_mesh_size[i]*sizeof(double));
          }

          if (this->x_increases_first) {
            step = 1;
            for (i = 0; i < this->n_arguments; i++) {
              for (j = 0; j < this->rectangular_mesh_size[i]; j++) {
                this->rectangular_mesh_point[i][j] = this->data_argument[i][step*j];
              }
              step *= this->rectangular_mesh_size[i];
            }
          } else {
            step = this->data_size;
            for (i = 0; i < this->n_arguments; i++) {
              step /= this->rectangular_mesh_size[i];
              for (j = 0; j < this->rectangular_mesh_size[i]; j++) {
                this->rectangular_mesh_point[i][j] = this->data_argument[i][step*j];
              }
            }
          }
        }
      }
    
      if (this->rectangular_mesh == 0 && this->multidim_interp == interp_bilinear) {
        feenox_push_error_message("rectangular interpolation of this '%s' needs a rectangular mesh", this->name);
        return FEENOX_ERROR;
      }

//      if (this->n_arguments > 3 && this->multidim_interp == rectangle) {
//        feenox_push_error_message("rectangular interpolation does not work for dimensions higher than three (function '%s' has %d arguments)", this->name, this->n_arguments);
//        return FEENOX_ERROR;
//      }

      if (this->n_arguments > 1 && (this->multidim_interp == interp_nearest || this->multidim_interp == interp_shepard_kd)) {

        double *point;

        point = malloc(this->n_arguments*sizeof(double));
        this->kd = kd_create(this->n_arguments);

        size_t j = 0;
        unsigned int k = 0;
        for (j = 0; j < this->data_size; j++) {
          for (k = 0; k < this->n_arguments; k++) {
            point[k] = this->data_argument[k][j];
          }
          kd_insert(this->kd, point, &this->data_value[j]);
        }

        feenox_free(point);
      }
    }
  }
  
  this->initialized = 1;

  return FEENOX_OK;  
}

// evaluate a function
double feenox_function_eval(function_t *this, const double *x) {

  double y = 0;
  
  // check if we need to initialize
  if (this->initialized == 0) {
    if (feenox_function_init(this) != FEENOX_OK) {
      feenox_runtime_error();
    }
  }
  
  // check if the function is a dummy to take derivatives against
  if (this->dummy_for_derivatives) {
    return this->dummy_for_derivatives_value;
  }
  
  // if the function is mesh, check if the time is the correct one
  if (this->mesh != NULL && this->name_in_mesh != NULL && this->mesh->reader == feenox_mesh_read_gmsh
      && this->mesh_time < feenox_special_var_value(t)-0.001*feenox_special_var_value(dt)) {
    feenox_call(feenox_mesh_gmsh_update_function(this, feenox_special_var_value(t), feenox_special_var_value(dt)));
    this->mesh_time = feenox_special_var_value(t);
  }
    
  // TODO: virtual methods
  if (this->type == function_type_pointwise_mesh_node) {
    return feenox_mesh_interpolate_function_node(this, x);
    
  } else if (this->type == function_type_pointwise_mesh_cell) {
//    return mesh_interpolate_function_cell(this, x);
    y = 0;
    
  } else if (this->type == function_type_pointwise_mesh_property) {
    y = feenox_function_property_eval(this, x);

  // TODO
/*    
  } else if (this->type == function_type_routine) {
    y = this->routine(x);
    
  } else if (this->type == function_type_routine_internal) {
    y = this->routine_internal(x, this);
*/
  } else if (this->algebraic_expression.items != NULL) {

    if (this->n_arguments == 1) {
      
      feenox_var_value(this->var_argument[0]) = x[0];
      y = feenox_expression_eval(&this->algebraic_expression);
      
    } else {

      unsigned int i = 0;
      for (i = 0; i < this->n_arguments; i++) {
        feenox_var_value(this->var_argument[i]) = x[i];
      }

      y = feenox_expression_eval(&this->algebraic_expression);

    }
    
  } else if (this->data_size != 0) {
    
    if (this->n_arguments == 1) {

      // one-dimensional pointwise-defined functions are handled by GSL
      if (this->type == function_type_pointwise_vector) {
        if (gsl_interp_init(this->interp, this->data_argument[0], this->data_value, this->data_size) != GSL_SUCCESS) {
          feenox_runtime_error();
        }
      }
    
      y = (this->interp != NULL) ? gsl_interp_eval(this->interp, this->data_argument[0], this->data_value, x[0], this->interp_accel) : 0;

    } else {
      

      // multi-dimensional pointwise-defined function are not handled by GSL
      y = 0;
/*    
    int j;
 
    if (this->multidim_interp == nearest) {
      // vecino mas cercano en un k-dimensional tree
      y = *((double *)kd_res_item_data(kd_nearest(this->kd, x)));
      
    } else if (this->multidim_interp == shepard) {
      int flag = 0;  // suponemos que NO nos pidieron un punto del problema
      double num = 0;
      double den = 0;
      double w_i, y_i, dist2, diff;
      
      for (i = 0; i < this->data_size; i++) {
        
        y_i = this->data_value[i];
        
        dist2 = 0;
        for (j = 0; j < this->n_arguments; j++) {
          diff = (x[j]-this->data_argument[j][i]);
          dist2 += diff*diff;
        }
        if (dist2 < this->multidim_threshold) {
          y = y_i;
          flag = 1;   // nos pidieron un punto de la definicion
          break;
        } else {
          w_i = (this->shepard_exponent == 2)? 1.0/dist2 : 1.0/pow(dist2, 0.5*this->shepard_exponent);
          num += w_i * y_i;
          den += w_i;
        }
      }
 
      if (flag == 0) {
        if (den == 0) {
          feenox_push_error_message("denominator equal to zero");
          feenox_runtime_error();
        }
        y = num/den;
      }
      
      
    } else if (this->multidim_interp == shepard_kd) {
      struct kdres *presults;
      int flag = 0;  // suponemos que NO nos pidieron un punto del problema
      int n = 0;
      double num = 0;
      double den = 0;
      double w_i, y_i, dist2, diff;
      double dist;
      double *x_i = malloc(this->n_arguments*sizeof(double));
      
      do {
        presults = kd_nearest_range(this->kd, x, this->shepard_radius);
        while (kd_res_end(presults) == 0) {
          n++;
          y_i = *((double *)kd_res_item(presults, x_i));
        
          dist2 = 0;
          for (j = 0; j < this->n_arguments; j++) {
            diff = (x[j]-x_i[j]);
            dist2 += diff*diff;
          }
          if (dist2 < this->multidim_threshold) {
            y = y_i;
            flag = 1;   // nos pidieron un punto de la definicion
            break;
          } else {
//            w_i = (this->shepard_exponent == 2)? 1.0/dist2 : 1.0/pow(dist2, 0.5*this->shepard_exponent);
            dist = sqrt(dist2);
            w_i = pow((this->shepard_radius-dist)/(this->shepard_radius*dist), this->shepard_exponent);
            num += w_i * y_i;
            den += w_i;
          }
          kd_res_next(presults);
        }
        kd_res_free(presults);
        
        // si no encontramos ningun punto, duplicamos el radio
        if (n == 0) {
          this->shepard_radius *= 2;
        }
      } while (n == 0);

      
      if (flag == 0) {
        if (den == 0) {
          if (this->n_arguments == 3) {
            feenox_push_error_message("no definition point found in a range %g around point (%g,%g,%g), try a larger SHEPARD_RADIUS", this->shepard_radius, x[0], x[1], x[2]);
          } else {
            feenox_push_error_message("no definition point found in a range %g around point, try a larger SHEPARD_RADIUS", this->shepard_radius);
          }
        }
        y = num/den;
      }
      feenox_free(x_i);
    
    } else if (this->multidim_interp == bilinear && this->rectangular_mesh_size != NULL) {

      // flag que indica si nos pidieron un punto de la definicion
      int flag;
      int *ctilde;
      double shape;
      
      // indices para la biseccion
      int *a;
      int *b;
      int *c;
      // coordenadas normalizadas a [-1:1]
      double *r;

      a = malloc(this->n_arguments*sizeof(int));
      b = malloc(this->n_arguments*sizeof(int));
      c = malloc(this->n_arguments*sizeof(int));
      r = malloc(this->n_arguments*sizeof(double));

      flag = 1;  // suponemos que nos pidieron un punto del problema
      for (i = 0; i < this->n_arguments; i++) {

        a[i] = 0;
        b[i] = this->rectangular_mesh_size[i];

        while ((b[i]-a[i]) > 1) {
          c[i] = floor(0.5*(a[i]+b[i]));

          if (this->rectangular_mesh_point[i][c[i]] > x[i]) {
            b[i] = c[i];
          } else {
            a[i] = c[i];
          }
        }

        c[i] = floor(0.5*(a[i]+b[i]));
        if (gsl_fcmp(this->rectangular_mesh_point[i][c[i]], x[i], this->multidim_threshold) != 0) {
          flag = 0;
        }

      }

      if (flag) {
        // nos pidieron un punto de la definicion
        y = this->data_value[feenox_structured_scalar_index(this->n_arguments, this->rectangular_mesh_size, c, this->x_increases_first)];
      } else {
        // tenemos que interpolar
        for (i = 0; i < this->n_arguments; i++) {
          // calculamos las coordenadas normalizadas en [-1:1], teniendo en cuenta
          // que capaz nos pidieron extrapolar
          if (x[i] < this->rectangular_mesh_point[i][0]) {
            r[i] = -1;
          } else if (c[i] == this->rectangular_mesh_size[i]-1) {
            c[i]--;
            r[i] = 1;
          } else {
            r[i] = -1 + 2*(x[i] - this->rectangular_mesh_point[i][c[i]])/(this->rectangular_mesh_point[i][c[i]+1] - this->rectangular_mesh_point[i][c[i]]);
          } 
         
        }

        y = 0;
        ctilde = malloc(this->n_arguments * sizeof(int));
        for (i = 0; i < (1<<this->n_arguments); i++)  {
          shape = 1;
          for (j = 0; j < this->n_arguments; j++) {
            // el desarrollo binario de i nos dice si hay que sumar uno o no al indice
            ctilde[j] = c[j] + ((i & (1<<j)) != 0);
            // y tambien como es el termino correspondiente a la funcion de forma
            shape *= ((i & (1<<j)) == 0) ? (1.0-r[j]) : (1.0+r[j]);
          }
          index = feenox_structured_scalar_index(this->n_arguments, this->rectangular_mesh_size, ctilde, this->x_increases_first);
          y += shape * this->data_value[index];
        }
        
        y *= 1.0/(1<<this->n_arguments);
        feenox_free(ctilde);
        
      }

      feenox_free(r);
      feenox_free(c);
      feenox_free(b);
      feenox_free(a);



    } else if (this->multidim_interp == triangle) {

      double *dist;
      int *index;
      int far_away;
      double leg;

      size_t *closest;

      gsl_matrix *A;
      gsl_vector *coeff;
      gsl_vector *value;
      gsl_permutation *p;
      int s;

      int err;


      // calculamos las distancias de todos los puntos dato al punto pedido
      // el calloc las inicializa en cero
      n = 0;
      dist = calloc(this->data_size, sizeof(double));
      index = calloc(this->data_size, sizeof(int));
      for (i = 0; i < this->data_size; i++) {

        far_away = 0;

        for (j = 0; j < this->n_arguments; j++) {

          // leg es cateto en ingles segun google
          leg = fabs(this->data_argument[j][i]-x[j]);

          // si nos dieron una distancia caracteristica, entonces no hace falta
          // calcular todas las distancias, aquellos puntos que tengan al menos
          // una coordenada que este a mas tres o cuatro distancias del punto
          // pedido le ponemos dist = infinito

          if (this->multidim_characteristic_length != 0 && leg > this->multidim_characteristic_length) {
            far_away = 1;
            break;
          }

          // TODO: hay que dar una metrica para calcular la distancia
          // cuando los ejes tienen unidades diferentes!!!
          dist[n] += leg*leg;

        }

        if (!far_away) {
          // si dist[n] es muy chiquito, nos pidieron justo un punto de los
          // de la tabla asi que devolvemos ese y a comerla
          if (dist[n] < this->multidim_threshold) {
            feenox_free(dist);
            feenox_free(index);
            return this->data_value[i];
          }

          // si no esta lejos nos acordamos de i e incrementamos n
          index[n] = i;
          n++;
        }

      }


      // aca vamos a guardar los indices de los puntos dato mas cercanos, pero
      // como eventualmente los mas cercanos caen en un plano capaz que tenemos
      // que buscar bastantes mas que el minimo indispensable
      closest = calloc(n, sizeof(size_t));

      // para calcular los coeficientes del plano tenemos que resolver un problema
      // de algebra lineal
      A = gsl_matrix_alloc(this->n_arguments+1, this->n_arguments+1);
      coeff = gsl_vector_alloc(this->n_arguments+1);
      value = gsl_vector_alloc(this->n_arguments+1);
      p = gsl_permutation_alloc(this->n_arguments+1);

      // elegimos los tres mas cercanos
      gsl_sort_smallest_index(closest, this->n_arguments+1, dist, 1, n);

      for (i = 0; i < this->n_arguments+1; i++) {
        // nos fabricamos una matriz para calcular los coeficientes del plano definido
        // por estos tres (o n) puntos
        for (j = 0; j < this->n_arguments; j++) {
          gsl_matrix_set(A, i, j, this->data_argument[j][index[closest[i]]]);
        }

        gsl_matrix_set(A, i, this->n_arguments, 1);

      //
      //    A x = b
      //
      //        [ x1   y1   1 ]            [ z1 ]           [ a ]
      //    A = [ x2   y2   1 ]       b =  [ z2 ]      x =  [ b ]
      //        [ x3   y4   1 ]            [ z3 ]           [ c ]
      //
      // donde *1, *2 y *3 son los puntos mas cercanos al pedido
      //  el plano es ax + by + c - z = 0
      /  entonces z = ax + by + c

        // y el vector de terminos independientes para resolver el problema
        gsl_vector_set(value, i, this->data_value[index[closest[i]]]);
      }

        // solve!
        for (i = 0; i < 3; i++) {
          for (j = 0; j < 3; j++) {
            printf("%lf  ", gsl_matrix_get(A, i, j));
          }
          printf("\n");
        }
        printf("\n");
      err = gsl_linalg_LU_decomp(A, p, &s);
      if ((err = gsl_linalg_LU_solve(A, p, value, coeff)) != GSL_SUCCESS) {

        int k;

        // si no se pudo resolver el sistema, los tres puntos estan alineados
           (o los n puntos forman un hiperplano de dimension n-1 en lugar de n)

        // devolvemos el mas cercano
        //y = this->data_value[closest[0]];

        // o... tiramos el tercer punto y buscamos otro que de un plano valido
        // en general, la que no sirve es la fila this->n_arguments (empezando
        // a contar de cero) de la matriz A
        // para eso podemos
        //   a. ordenar TODOS los puntos y vamos buscando alguno que de un plano valido
        //   b. vamos seleccionando los k mas cercanos y tomamos los dos primeros y el ultimo
        //      hasta que encontremos un plano valido
        //
        // opcion a: te la debo
        // opcion b
        k = this->n_arguments;

        do {
          k++;
          // elegimos los k+1 mas cercanos
          gsl_sort_smallest_index(closest, k+1, dist, 1, n);


          // tenemos que volver a escribir la matriz completa porque el LU la rompe
          for (i = 0; i < this->n_arguments; i++) {
            // arriba que quiere decir esta mantriz
            for (j = 0; j < this->n_arguments; j++) {
              gsl_matrix_set(A, i, j, this->data_argument[j][index[closest[i]]]);
            }
            gsl_matrix_set(A, i, this->n_arguments, 1);
            // y el vector de terminos independientes para resolver el problema
            gsl_vector_set(value, i, this->data_value[index[closest[i]]]);
          }

          // escribimos la ultima fila de la matriz con los nuevos valores
          for (j = 0; j < this->n_arguments; j++) {
            gsl_matrix_set(A, this->n_arguments, j, this->data_argument[j][index[closest[k]]]);
          }
          gsl_matrix_set(A, this->n_arguments, this->n_arguments, 1);

          // y el vector de terminos independientes para resolver el problema
          gsl_vector_set(value, this->n_arguments, this->data_value[index[closest[k]]]);

          // solve!
            for (i = 0; i < 3; i++) {
              for (j = 0; j < 3; j++) {
                printf("%lf  ", gsl_matrix_get(A, i, j));
              }
              printf("\n");
            }
          gsl_linalg_LU_decomp(A, p, &s);
          err = gsl_linalg_LU_solve(A, p, value, coeff);
        } while (err != GSL_SUCCESS);
      }


      // calculamos el punto a partir de la ecuacion del plano
      y = 0;

      for (i = 0; i < this->n_arguments; i++) {
        y += gsl_vector_get(coeff, i)*x[i];
      }
      y += gsl_vector_get(coeff, this->n_arguments);

      gsl_permutation_free(p);
      gsl_vector_free(value);
      gsl_vector_free(coeff);
      gsl_matrix_free(A);

      feenox_free(closest);

      feenox_free(dist);
      feenox_free(index);

    }
*/
    }
  }


  if (gsl_isnan(y) || gsl_isinf(y)) {
    feenox_nan_error();
  }

  return y;

}
/*
int feenox_structured_scalar_index(int n_dims, int *size, int *index, int x_increases_first) {
  int i, j;
  int k;
  int scalar_index = 0;
  
  if (x_increases_first) {
    for (i = 0; i < n_dims; i++) {
      k = 1;
      for (j = i-1; j >= 0; j--) {
        k *= size[j];
      }
      scalar_index += k*index[i];  
    }
  } else {
    for (i = n_dims-1; i >= 0; i--) {
      k = 1;
      for (j = i+1; j < n_dims; j++) {
        k *= size[j];
      }
      scalar_index += k*index[i];  
    }
  }
  
  return scalar_index;
  
}
*/


// mira si los n puntos en x[] y en y[] forman una grilla estruturada
int feenox_is_structured_grid_2d(double *x, double *y, int n, int *nx, int *ny) {

  int i, j;
    
  // hacemos una primera pasada sobre x hasta encontrar de nuevo el primer valor
  i = 0;
  do {
    if (++i == n) {   
      // si recorrimos todo el set y nunca aparecio otra vez el x[0] a comerla
      return 0;
    }
  } while (gsl_fcmp(x[i], x[0], DEFAULT_MULTIDIM_INTERPOLATION_THRESHOLD) != 0);
  
  // si i es uno entonces a comerla
  if (i == 1) {
    return 0;
  }
    
  // asumimos que la cantidad de datos en x es i
  *nx = i;
  // y entonces ny tiene que ser la cantidad de datos totales dividido data_size
  if (n % (*nx) != 0) {
    // si esto no es entero, no puede ser rectangular
    return 0;
  }
  *ny = n/(*nx);

  // si x[j] != x[j-nx] entonces no es una grilla estructurada
  for (i = *nx; i < n; i++) {
    if (gsl_fcmp(x[i], x[i-(*nx)], DEFAULT_MULTIDIM_INTERPOLATION_THRESHOLD*fabs(x[(*nx)-1]-x[0])) != 0) {
      return 0;
    }
  }
  
  // ahora miramos el conjunto y, tienen que ser nx valores igualitos consecutivos
  for (i = 0; i < *ny; i++) {
    for (j = 0; j < (*nx)-1; j++) {
      if (gsl_fcmp(y[i*(*nx) + j], y[i*(*nx) + j + 1], DEFAULT_MULTIDIM_INTERPOLATION_THRESHOLD*fabs(y[n-1]-y[0])) != 0) {
        return 0;
      }
    }
  }
  

  // si llegamos hasta aca, es una grilla bidimensional estructurada!
  return 1;
    
}


// mira si los n puntos en x[], y[] y z[] forman una grilla estruturada
int feenox_is_structured_grid_3d(double *x, double *y, double *z, int n, int *nx, int *ny, int *nz) {

  int i;
  
  // hacemos una pasada sobre x hasta encontrar de nuevo el primer valor
  i = 0;
  do {
    i += 1;
    if (i == n) {   
      // si recorrimos todo el set y nunca aparecio otra vez el x[0] a comerla
      return 0;
    }
  } while (gsl_fcmp(x[i], x[0], DEFAULT_MULTIDIM_INTERPOLATION_THRESHOLD) != 0);

  // si i es uno entonces a comerla
  if (i == 1) {
    return 0;
  }
    
  // asumimos que la cantidad de datos en x es i
  *nx = i;

  // hacemos una pasada sobre y hasta encontrar de nuevo el primer valor
  // pero con step nx
  i = 0;
  do {
    i += *nx;
    if (i >= n) {   
      // si recorrimos todo el set y nunca aparecio otra vez el y[0] a comerla
      return 0;
    }
  } while (gsl_fcmp(y[i], y[0], DEFAULT_MULTIDIM_INTERPOLATION_THRESHOLD) != 0);
  
  // si i es uno entonces a comerla
  if (i == *nx || i % ((*nx)) != 0) {
    return 0;
  }

  // asumimos que la cantidad de datos en x es i
  *ny = i/(*nx);
  
  
  // y entonces nz tiene que ser la cantidad de datos totales dividido data_size
  if (n % ((*ny)*(*nx)) != 0) {
    // si esto no es entero, no puede ser rectangular
    return 0;
  }
  *nz = n/((*ny)*(*nx));


  for (i = *nx; i < n; i++) {
    if (gsl_fcmp(x[i], x[i-(*nx)], DEFAULT_MULTIDIM_INTERPOLATION_THRESHOLD*fabs(x[(*nx)-1]-x[0])) != 0) {
      return 0;
    }
  }
  for (i = (*nx)*(*ny); i < n; i++) {
    if (gsl_fcmp(y[i], y[i-(*nx)*(*ny)], DEFAULT_MULTIDIM_INTERPOLATION_THRESHOLD*fabs(x[(*nx)-1]-x[0])) != 0) {
      return 0;
    }
  }
  
  
  return 1;    
}
