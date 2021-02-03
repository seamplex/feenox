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

// set the variables that are a function's argument equal to the vector x
void feenox_set_function_args(function_t *this, double *x) {
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

  int i;
  double *x;
  double x0;
  double y;


//   feenox_push_error_message("when computing function %s", token->function->name);

  if (this->function->n_arguments == 1) {

    // para no tener que hacer malloc y frees todo el tiempo, para un solo argumento lo
    // hacemos diferente
    x0 = feenox_expression_eval(&this->arg[0]);
    y = feenox_function_eval(this->function, &x0);

  } else {

    // muchos argumentos necesitan vectores
    x = malloc(this->function->n_arguments*sizeof(double));

    for (i = 0; i < this->function->n_arguments; i++) {
      x[i] = feenox_expression_eval(&this->arg[i]);
    }

    y = feenox_function_eval(this->function, x);

    free(x);
    
  }

  if (gsl_isnan(y) || gsl_isinf(y)) {
    feenox_nan_error();
  }

//   feenox_pop_error_message();  

  return y;

}


int feenox_function_init(function_t *this) {

/*  
  int i, j, k;
  int nx, ny, nz;
*/
  if (this->initialized) {
    return 0;
  }
  
  if (this->n_arguments_given != this->n_arguments) {
    feenox_push_error_message("function '%s' needs %d arguments and %d were given", this->name, this->n_arguments, this->n_arguments_given);
  }
  
  this->initialized = 1;

/*  
  if (function->type == type_pointwise_vector) {
    for (i = 0; i < function->n_arguments; i++) {
      
      if (!function->vector_argument[i]->initialized) {
        feenox_call(feenox_vector_init(function->vector_argument[i]));
      }
      
      if (function->data_size == 0) {
        function->data_size = function->vector_argument[i]->size;
      } else {
        if (function->vector_argument[i]->size != function->data_size) {
          feenox_push_error_message("vector sizes do not match (%d and %d) in function '%s'", function->data_size, function->vector_argument[i]->size, function->name);
          return WASORA_RUNTIME_ERROR;
        }
      }

      function->data_argument[i] = gsl_vector_ptr(feenox_var_value_ptr(function->vector_argument[i]), 0);
    }
  }
  
  if (function->vector_value != NULL) {

    if (!function->vector_value->initialized) {
      feenox_call(feenox_vector_init(function->vector_value));
    }
    if (function->vector_value->size != function->data_size) {
      feenox_push_error_message("vector sizes do not match (%d and %d)", function->data_size, function->vector_value->size);
      return WASORA_PARSER_ERROR;
    }
    
    // rellenamos las variables f_a f_b y f_c
    if (function->n_arguments == 1) {
      var_t *dummy_var;
      char *dummy_aux = malloc(strlen(function->name) + 4);
      
      sprintf(dummy_aux, "%s_a", function->name);
      if ((dummy_var = feenox_get_variable_ptr(dummy_aux)) == NULL) {
        return WASORA_PARSER_ERROR;
      }
      feenox_realloc_variable_ptr(dummy_var, &function->data_argument[0][0], 0);
      
      sprintf(dummy_aux, "%s_b", function->name);
      if ((dummy_var = feenox_get_variable_ptr(dummy_aux)) == NULL) {
        return WASORA_PARSER_ERROR;
      }
      feenox_realloc_variable_ptr(dummy_var, &function->data_argument[0][function->data_size-1], 0);
      
      sprintf(dummy_aux, "%s_n", function->name);
      if ((dummy_var = feenox_get_variable_ptr(dummy_aux)) == NULL) {
        return WASORA_PARSER_ERROR;
      }
      free(dummy_aux);
      feenox_var_value(dummy_var) = (double)function->data_size;
    }

    function->data_value = gsl_vector_ptr(feenox_var_value_ptr(function->vector_value), 0);

  }
  
  if (function->data_size != 0) {
    if (function->expr_multidim_threshold.n_tokens != 0) {
      function->multidim_threshold = feenox_expression_eval(&function->expr_multidim_threshold);
    } else {
      function->multidim_threshold = DEFAULT_MULTIDIM_INTERPOLATION_THRESHOLD;
    }
    if (function->mesh == NULL) {
      if (function->n_arguments == 1) {

        if (function->interp_type.name == NULL) {
          // interpolacion 1D por default 
          function->interp_type = DEFAULT_INTERPOLATION;      
        }

        if ((function->interp = gsl_interp_alloc(&function->interp_type, function->data_size)) == NULL) {
          feenox_push_error_message("interpolation method for function '%s' needs more points", function->name);
          return WASORA_PARSER_ERROR;
        }

        function->interp_accel = gsl_interp_accel_alloc();

        // si no es de tipo vector, inicializamos el interpolador (los datos quedan fijos) 
        // porque si es vector y los vectores tienen fruta, el interpolador se queja 
        if (function->type != type_pointwise_vector) {
          feenox_push_error_message("in function %s", function->name);
          gsl_interp_init(function->interp, function->data_argument[0], function->data_value, function->data_size);
          feenox_pop_error_message();
        }

      } else {

        if (function->expr_shepard_radius.n_tokens != 0) {
          function->shepard_radius = feenox_expression_eval(&function->expr_shepard_radius);
        } else {
          function->shepard_radius = DEFAULT_SHEPARD_RADIUS;
        }
        if (function->expr_shepard_exponent.n_tokens != 0) {
          function->shepard_exponent = feenox_expression_eval(&function->expr_shepard_exponent);
        } else {
          function->shepard_exponent = DEFAULT_SHEPARD_EXPONENT;
        }

        if (function->n_arguments == 2) {

          // miramos si tiene una malla regular en 2d
          if (feenox_is_structured_grid_2d(function->data_argument[0], function->data_argument[1], function->data_size, &nx, &ny)) {
            function->rectangular_mesh = 1;
            function->x_increases_first = 1;
          } else if (feenox_is_structured_grid_2d(function->data_argument[1], function->data_argument[0], function->data_size, &ny, &nx)) {
            function->rectangular_mesh = 1;
            function->x_increases_first = 0;
          } else {
            function->rectangular_mesh = 0;
          }

          if (function->rectangular_mesh) {
            function->rectangular_mesh_size = malloc(2*sizeof(int));
            function->rectangular_mesh_size[0] = nx;
            function->rectangular_mesh_size[1] = ny;
          }

        } else if (function->n_arguments == 3) {

          // miramos si tiene una malla regular en 3d
          if (feenox_is_structured_grid_3d(function->data_argument[0], function->data_argument[1], function->data_argument[2], function->data_size, &nx, &ny, &nz)) {
            function->rectangular_mesh = 1;
            function->x_increases_first = 1;
          } else if (feenox_is_structured_grid_3d(function->data_argument[2], function->data_argument[1], function->data_argument[0], function->data_size, &nz, &ny, &nx)) {
            function->rectangular_mesh = 1;
            function->x_increases_first = 0;
          } else {
            function->rectangular_mesh = 0;
          }

          if (function->rectangular_mesh) {
            function->rectangular_mesh_size = malloc(3*sizeof(int));
            function->rectangular_mesh_size[0] = nx;
            function->rectangular_mesh_size[1] = ny;
            function->rectangular_mesh_size[2] = nz;
          }

        } else {

          // estamos en el caso de mayor dimension que 3
          // confiamos de una que es rectangular
          function->rectangular_mesh = 1;

          if (function->expr_x_increases_first.n_tokens != 0) {
            function->x_increases_first = feenox_expression_eval(&function->expr_x_increases_first);
          } else {
            feenox_push_error_message("missing expression for X_INCREASES_FIRST keyword");
            return WASORA_PARSER_ERROR;
          }

          function->rectangular_mesh_size = malloc(function->n_arguments*sizeof(int));

          if (function->expr_rectangular_mesh_size != NULL) {
            for (i = 0; i < function->n_arguments; i++) {
              if (function->expr_rectangular_mesh_size[i].n_tokens != 0) {
                function->rectangular_mesh_size[i] = round(feenox_expression_eval(&function->expr_rectangular_mesh_size[i]));
                if (function->rectangular_mesh_size[i] < 2) {
                  feenox_push_error_message("size %d for argument number %d in function '%s' cannot be less than two", function->rectangular_mesh_size[i], i+1, function->name);
                  return WASORA_RUNTIME_ERROR;
                }
              }
            }
          } else {
            feenox_push_error_message("missing expressions for SIZES keyword");
            return WASORA_PARSER_ERROR;
          }
        }

        if (function->rectangular_mesh) {

          int step;

          // checkeamos solo el size (porq el usuario pudo meter fruta para dimension mayor a 3)
          j = 1;
          for (i = 0; i < function->n_arguments; i++) {
            j *= function->rectangular_mesh_size[i];
          }
          if (function->data_size != j) {
            feenox_push_error_message("data size of function %s do not match with the amount of given values", function->name);
            return WASORA_RUNTIME_ERROR;
          }

          function->rectangular_mesh_point = malloc(function->n_arguments*sizeof(double *));
          for (i = 0; i < function->n_arguments; i++) {
            function->rectangular_mesh_point[i] = malloc(function->rectangular_mesh_size[i]*sizeof(double));
          }

          if (function->x_increases_first) {
            step = 1;
            for (i = 0; i < function->n_arguments; i++) {
              for (j = 0; j < function->rectangular_mesh_size[i]; j++) {
                function->rectangular_mesh_point[i][j] = function->data_argument[i][step*j];
              }
              step *= function->rectangular_mesh_size[i];
            }
          } else {
            step = function->data_size;
            for (i = 0; i < function->n_arguments; i++) {
              step /= function->rectangular_mesh_size[i];
              for (j = 0; j < function->rectangular_mesh_size[i]; j++) {
                function->rectangular_mesh_point[i][j] = function->data_argument[i][step*j];
              }
            }
          }
        }
      }
    
      if (function->rectangular_mesh == 0 && function->multidim_interp == bilinear) {
        feenox_push_error_message("rectangular interpolation of function '%s' needs a rectangular mesh", function->name);
        return WASORA_RUNTIME_ERROR;
      }

//      if (function->n_arguments > 3 && function->multidim_interp == rectangle) {
//        feenox_push_error_message("rectangular interpolation does not work for dimensions higher than three (function '%s' has %d arguments)", function->name, function->n_arguments);
//        return WASORA_RUNTIME_ERROR;
//      }

      if (function->n_arguments > 1 && (function->multidim_interp == nearest || function->multidim_interp == shepard_kd)) {

        double *point;

        point = malloc(function->n_arguments*sizeof(double));
        function->kd = kd_create(function->n_arguments);

        for (j = 0; j < function->data_size; j++) {
          for (k = 0; k < function->n_arguments; k++) {
            point[k] = function->data_argument[k][j];
          }
          kd_insert(function->kd, point, &function->data_value[j]);
        }

        free(point);
      }
    }
  }
*/
  return FEENOX_OK;  
}

// evaluate a function
double feenox_function_eval(function_t *this, const double *x) {

  int i;
  double y = 0;
  
  // check if we need to initialize
  if (this->initialized == 0) {
    if (feenox_function_init(this) != FEENOX_OK) {
      feenox_runtime_error();
    }
  }
  
  // if the function is mesh, check if the time is the correct one
/*  
  if (this->mesh != NULL && this->name_in_mesh != NULL && this->mesh->format == mesh_format_gmsh
      && this->mesh_time < feenox_special_var_value(t)-0.001*feenox_special_var_value(dt)) {
    feenox_call(mesh_gmsh_update_function(function, feenox_special_var_value(t), feenox_special_var_value(dt)));
    this->mesh_time = feenox_special_var_value(t);
  }
    

  if (this->type == type_pointwise_mesh_node) {
    return mesh_interpolate_function_node(function, x);
    
  } else if (this->type == type_pointwise_mesh_cell) {
    return mesh_interpolate_function_cell(function, x);
    
  } else if (this->type == type_pointwise_mesh_property) {
    return mesh_interpolate_function_property(function, x);
    
  } else if (this->type == type_routine) {
    y = this->routine(x);
    
  } else if (this->type == type_routine_internal) {
    y = this->routine_internal(x, function);
    
  } else if (this->algebraic_expression.n_tokens != 0 && this->n_arguments == 1) {
*/
  
  if (this->algebraic_expression.items != NULL) {
    if (this->n_arguments == 1) {
      
      // TODO: should we keep this or leave the arguments as they are?
//      double x_old = feenox_var_value(this->var_argument[0]);
      feenox_var_value(this->var_argument[0]) = x[0];
      y = feenox_expression_eval(&this->algebraic_expression);
//      feenox_var_value(this->var_argument[0]) = x_old;
      
    } else {

//      double *vecx_old = malloc(this->n_arguments*sizeof(double));
      for (i = 0; i < this->n_arguments; i++) {
//        vecx_old[i] = feenox_var_value(this->var_argument[i]);
        feenox_var_value(this->var_argument[i]) = x[i];
      }

      y = feenox_expression_eval(&this->algebraic_expression);

//      for (i = 0; i < this->n_arguments; i++) {
//        feenox_var_value(this->var_argument[i]) = vecx_old[i];
//      }

//      free(vecx_old);

    }
    
  } else if (this->data_size != 0 && this->n_arguments == 1) {

    // funcion definida por puntos unidimensional
    // OJO! la GSL 1.15 no extrapola, devuelve nan y a comerla, asi que ante
    // de que nos devuelva porquerias miramos que onda
    // TODO: see if we need to keep this
    if (x[0] < this->data_argument[0][0]) {
      y = this->data_value[0] - (this->data_value[1]-this->data_value[0])/(this->data_argument[0][1]-this->data_argument[0][0]) * (this->data_argument[0][0] - x[0]);
    } else if (x[0] > this->data_argument[0][this->data_size-1]) {
      y = this->data_value[this->data_size-1] + (this->data_value[this->data_size-1]-this->data_value[this->data_size-2])/(this->data_argument[0][this->data_size-1]-this->data_argument[0][this->data_size-2]) * (x[0] - this->data_argument[0][this->data_size-1]);
    } else {
      if (this->type == function_type_pointwise_vector) {
        if (gsl_interp_init(this->interp, this->data_argument[0], this->data_value, this->data_size) != GSL_SUCCESS) {
          feenox_runtime_error();
        }
      }
      if (this->interp != NULL) {
        y = gsl_interp_eval(this->interp, this->data_argument[0], this->data_value, x[0], this->interp_accel);
      } else {
        // esto puede ser por la evaluacion a tiempo de parseo
        y = 0;
      }
    }

  } else if (this->data_size != 0 && this->n_arguments > 1) {

    // multi-dimensional pointwise-defined function
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
      free(x_i);
    
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
        free(ctilde);
        
      }

      free(r);
      free(c);
      free(b);
      free(a);



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
            free(dist);
            free(index);
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

      free(closest);

      free(dist);
      free(index);

    }
*/
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
