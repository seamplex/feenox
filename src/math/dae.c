/*------------ -------------- -------- --- ----- ---   --       -            -
 *  DAE evaluation routines
 *
 *  Copyright (C) 2009--2020 jeremy theler
 *
 *  This file is part of FeenoX.
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

// API
int feenox_add_time_path(const char *token) {
  expr_t *expr;
  
  if ((expr = calloc(1, sizeof(expr))) == NULL) {
    feenox_push_error_message("memory allocation failed for expression in feenox_add_time_path()");
    return FEENOX_ERROR;
  }
  
  feenox_call(feenox_expression_parse(expr, token));
  LL_APPEND(feenox.time_paths, expr);
  
  return FEENOX_OK;
}

// API
int feenox_add_phase_space_object(const char *token) {

  var_t *variable = NULL;
  vector_t *vector = NULL;
  matrix_t *matrix = NULL;
  char *buffer = NULL;
  
  phase_object_t *phase_object = calloc(1, sizeof(phase_object_t));
  if ((vector = feenox_get_vector_ptr(token)) != NULL) {

//     phase_object->offset = i;
    phase_object->vector = vector;
    phase_object->name = vector->name;

    buffer = malloc(strlen(vector->name)+8);
    sprintf(buffer, "%s_dot", vector->name);
    feenox_call(feenox_define_vector(buffer, vector->size_expr.string));
    free(buffer);

  } else if ((matrix = feenox_get_matrix_ptr(token)) != NULL) {

    phase_object->matrix = matrix;
    phase_object->name = matrix->name;

    buffer = malloc(strlen(matrix->name)+8);
    sprintf(buffer, "%s_dot", matrix->name);
    feenox_call(feenox_define_matrix(buffer, matrix->rows_expr.string, matrix->cols_expr.string));
    free(buffer);

  } else {

    if ((variable = feenox_get_or_define_variable_ptr(token)) == NULL) {
      return FEENOX_ERROR;
    }

    phase_object->variable = variable;
    phase_object->name = variable->name;

    buffer = malloc(strlen(variable->name)+8);
    sprintf(buffer, "%s_dot", variable->name);
    if ((phase_object->variable_dot = feenox_get_or_define_variable_ptr(buffer)) == NULL) {
      return FEENOX_ERROR;
    }
    free(buffer);

  }
  
  LL_APPEND(feenox.dae.phase_objects, phase_object);
  return FEENOX_OK;
}

int feenox_dae_init(void) {
  
#ifdef HAVE_IDA
  int i, k, l;
  phase_object_t *phase_object;
  dae_t *dae;
  
  // primero calculamos el tamanio del phase space
  feenox.dae.dimension = 0;
  LL_FOREACH(feenox.dae.phase_objects, phase_object) {
    
    if (phase_object->variable != NULL) {
      
      phase_object->size = 1;
      
    } else if (phase_object->vector != NULL) {
      
      if (!phase_object->vector->initialized) {
        feenox_call(feenox_vector_init(phase_object->vector));
      }
      if (!phase_object->vector_dot->initialized) {
        feenox_call(feenox_vector_init(phase_object->vector_dot));
      }
      
      phase_object->size = phase_object->vector->size;
      

    } else if (phase_object->matrix != NULL) {

      if (!phase_object->matrix->initialized) {
        feenox_call(feenox_matrix_init(phase_object->matrix));
      }
      if (!phase_object->matrix_dot->initialized) {
        feenox_call(feenox_matrix_init(phase_object->matrix_dot));
      }
      
      phase_object->size = phase_object->matrix->rows * phase_object->matrix->cols;
      
    }
    
    feenox.dae.dimension += phase_object->size;
      
  }
  
  if (feenox.dae.dimension == 0) {
    feenox_push_error_message("empty phase space in PHASE_SPACE keyword");
    return FEENOX_ERROR;
  }

  // alocamos
  feenox.dae.phase_value = malloc(feenox.dae.dimension * sizeof(double *));
  feenox.dae.phase_derivative = malloc(feenox.dae.dimension * sizeof(double *));
  
  i = 0;  
  LL_FOREACH(feenox.dae.phase_objects, phase_object) {

    if (phase_object->variable != NULL) {
      phase_object->offset = i;
      feenox.dae.phase_value[i] = feenox_value_ptr(phase_object->variable);
      feenox.dae.phase_derivative[i] = feenox_value_ptr(phase_object->variable_dot);
      i++;
      
    } else if (phase_object->vector != NULL) {

      phase_object->offset = i;
      for (k = 0; k < phase_object->vector->size; k++) {
        feenox.dae.phase_value[i] = gsl_vector_ptr(feenox_value_ptr(phase_object->vector), k);
        feenox.dae.phase_derivative[i] = gsl_vector_ptr(feenox_value_ptr(phase_object->vector_dot), k);
        i++;
      }

    } else if (phase_object->matrix != NULL) {

      phase_object->offset = i;
    for (k = 0; k < phase_object->matrix->rows; k++) {
        for (l = 0; l < phase_object->matrix->cols; l++) {
          feenox.dae.phase_value[i] = gsl_matrix_ptr(feenox_value_ptr(phase_object->matrix), k, l);
          feenox.dae.phase_derivative[i] = gsl_matrix_ptr(feenox_value_ptr(phase_object->matrix_dot), k, l);
          i++;
        }
      }
    }
  }

  // process the DAEs
  i = 0;
  LL_FOREACH(feenox.dae.daes, dae) {
    
    if (dae->matrix != NULL) {
      
      if (!dae->matrix->initialized) {
        feenox_call(feenox_matrix_init(dae->matrix));
      }
      
      dae->i_min = (dae->expr_i_min.items != NULL) ? feenox_expression_eval(&dae->expr_i_min)-1 : 0;
      dae->i_max = (dae->expr_i_max.items != NULL) ? feenox_expression_eval(&dae->expr_i_max)   : dae->matrix->rows;
      if (dae->i_max <= dae->i_min) {
        feenox_push_error_message("i_max %d is smaller or equal than i_min %d", dae->i_max, dae->i_min+1);
      }

      dae->j_min = (dae->expr_j_min.items != NULL) ? feenox_expression_eval(&dae->expr_j_min)-1 : 0;
      dae->j_max = (dae->expr_j_max.items != NULL) ? feenox_expression_eval(&dae->expr_j_max)   : dae->matrix->cols;
      if (dae->j_max <= dae->j_min) {
        feenox_push_error_message("j_max %d is smaller or equal than j_min %d", dae->j_max, dae->j_min+1);
      }
      i += (dae->i_max - dae->i_min) * (dae->j_max - dae->j_min);

    } else if (dae->vector != NULL) {
      
      if (!dae->vector->initialized) {
        feenox_call(feenox_vector_init(dae->vector));
      }
      
      dae->i_min = (dae->expr_i_min.items != NULL) ? feenox_expression_eval(&dae->expr_i_min)-1 : 0;
      dae->i_max = (dae->expr_i_max.items != NULL) ? feenox_expression_eval(&dae->expr_i_max)   : dae->vector->size;
      if (dae->i_max <= dae->i_min) {
        feenox_push_error_message("i_max %d is smaller or equal than i_max %d", dae->i_max, dae->i_min+1);
      }
    
      i += (dae->i_max - dae->i_min);
    } else {
     
      // scalar
      i++;
      
    }
  }
  
  if (i > feenox.dae.dimension) {
    feenox_push_error_message("more DAE equations than phase space dimension %d", feenox.dae.dimension);
    return FEENOX_ERROR;
  } else if (i < feenox.dae.dimension) {
    feenox_push_error_message("less DAE equations than phase space dimension %d", feenox.dae.dimension);
    return FEENOX_ERROR;
  }
  
  
  

  feenox.dae.x = N_VNew_Serial(feenox.dae.dimension);
  feenox.dae.dxdt = N_VNew_Serial(feenox.dae.dimension);
  feenox.dae.id = N_VNew_Serial(feenox.dae.dimension);

  feenox.dae.system = IDACreate();
  
  // llenamos el vector de ida con las variables del usuario 
  for (i = 0; i < feenox.dae.dimension; i++) {
    NV_DATA_S(feenox.dae.x)[i] = *(feenox.dae.phase_value[i]);
    NV_DATA_S(feenox.dae.dxdt)[i] = *(feenox.dae.phase_derivative[i]);
    // suponemos que son algebraicas, despues marcamos las diferenciales
    // pero hay que inicializar el vector id!
    NV_DATA_S(feenox.dae.id)[i] = DAE_ALGEBRAIC; 
  }
  // inicializamos IDA  
  if (IDAInit(feenox.dae.system, feenox_ida_dae, feenox_special_var_value(t), feenox.dae.x, feenox.dae.dxdt) != IDA_SUCCESS) {
    return FEENOX_ERROR;
  }

  // seteo de tolerancias 
  if ((feenox.special_vectors.abs_error = feenox_get_vector_ptr("abs_error")) != NULL) {

    // si hay un vector de absolutas, lo usamos 
    if (feenox.special_vectors.abs_error->size != feenox.dae.dimension) {
      feenox_push_error_message("dimension of system is %d and dimension of abs_error is %d", feenox.dae.dimension, feenox.special_vectors.abs_error->size);
      return FEENOX_ERROR;
    }

    feenox.dae.abs_error = N_VNew_Serial(feenox.dae.dimension);

    for (i = 0; i < feenox.dae.dimension; i++) {
      if (gsl_vector_get(feenox_value_ptr(feenox.special_vectors.abs_error), i) > 0) {
        NV_DATA_S(feenox.dae.abs_error)[i] = gsl_vector_get(feenox_value_ptr(feenox.special_vectors.abs_error), i);
      } else {
        NV_DATA_S(feenox.dae.abs_error)[i] = feenox_special_var_value(rel_error);
      }
    }

    if (IDASVtolerances(feenox.dae.system, feenox_special_var_value(rel_error), feenox.dae.abs_error) != IDA_SUCCESS) {
      return FEENOX_ERROR;
    }
  } else {
    // si no nos dieron vector de absolutas, ponemos absolutas igual a relativas 
    if (IDASStolerances(feenox.dae.system, feenox_special_var_value(rel_error), feenox_special_var_value(rel_error)) != IDA_SUCCESS) {
      return FEENOX_ERROR;
    }
  }

  //  estas tienen que venir despues de IDAInit

  // marcamos como differential aquellos elementos del phase space que lo sean
  LL_FOREACH(feenox.dae.phase_objects, phase_object) {
    if (phase_object->differential) {
      for (i = phase_object->offset; i < phase_object->offset+phase_object->size; i++) {
        NV_DATA_S(feenox.dae.id)[i] = DAE_DIFFERENTIAL;
      }
    }
  }
        
  
  if (IDASetId(feenox.dae.system, feenox.dae.id) != IDA_SUCCESS) {
    return FEENOX_ERROR;
  }
 
  if ((feenox.dae.A = SUNDenseMatrix(feenox.dae.dimension, feenox.dae.dimension)) == NULL) {
    return FEENOX_ERROR;
  }
    
  if ((feenox.dae.LS = SUNDenseLinearSolver(feenox.dae.x, feenox.dae.A)) == NULL) {
    return FEENOX_ERROR;
  }
  
  if (IDADlsSetLinearSolver(feenox.dae.system, feenox.dae.LS, feenox.dae.A) != IDA_SUCCESS) {
    return FEENOX_ERROR;
  }
  
  if (IDASetInitStep(feenox.dae.system, feenox_special_var_value(dt)) != IDA_SUCCESS) {
    return FEENOX_ERROR;
  }
  if (feenox_special_var_value(max_dt) != 0) {
    if (IDASetMaxStep(feenox.dae.system, feenox_special_var_value(max_dt)) != IDA_SUCCESS) {
      return FEENOX_ERROR;
    }
  }
  
  return FEENOX_OK;
#else

  feenox_push_error_message("feenox cannot solve DAE systems as it was not linked against SUNDIALS IDA library.");
  return FEENOX_ERROR;

#endif
  
}

int feenox_dae_ic(void) {
  
#ifdef HAVE_IDA
  int err;

  if (feenox.dae.initial_conditions_mode == initial_conditions_from_variables) {
    if ((err = IDACalcIC(feenox.dae.system, IDA_YA_YDP_INIT, feenox_special_var_value(dt))) != IDA_SUCCESS) {
      feenox_push_error_message("error computing initial conditions from variable values, error = %d", err);
      return FEENOX_ERROR;
    }
  } else if (feenox.dae.initial_conditions_mode == initial_conditions_from_derivatives) {
    if ((err = IDACalcIC(feenox.dae.system, IDA_Y_INIT, feenox_special_var_value(dt))) != IDA_SUCCESS)  {
      feenox_push_error_message("error computing initial conditions from derivative values, error = %d", err);
      return FEENOX_ERROR;
    }
  }  
  
#endif
  return FEENOX_OK;
}


#ifdef HAVE_IDA
int feenox_ida_dae(realtype t, N_Vector yy, N_Vector yp, N_Vector rr, void *params) {

  int i, j, k;
  dae_t *dae;
 
  // esta funcion se llama un monton de veces en pasos de tiempo
  // intermedios, entoces le decimos cuanto vale el tiempo nuevo por
  //  si hay residuos que dependen explicitamente del tiempo
  feenox_special_var_value(t) = t;

  // copiamos el estado del solver de IDA a feenox
  for (k = 0; k < feenox.dae.dimension; k++) {
    *(feenox.dae.phase_value[k]) = NV_DATA_S(yy)[k];
    *(feenox.dae.phase_derivative[k]) = NV_DATA_S(yp)[k];
  }

  // evaluamos los residuos en feenox y los dejamos en IDA
  k = 0;
  LL_FOREACH(feenox.dae.daes, dae) {
    
    // fall-offs para detectar errores (i.e. una expresion vectorial que dependa de j)
    j = 0;
    i = 0;
    
    if (dae->i_max == 0) {
      // ecuacion escalar
      NV_DATA_S(rr)[k++] = feenox_expression_eval(&dae->residual);
      
    } else {
         
      for (i = dae->i_min; i < dae->i_max; i++) {
        feenox_special_var_value(i) = (double)i+1;
        if (dae->j_max == 0) {
          
          // ecuacion vectorial
          NV_DATA_S(rr)[k++] = feenox_expression_eval(&dae->residual);
        } else {
          for (j = dae->j_min; j < dae->j_max; j++) {
            feenox_special_var_value(j) = (double)j+1;
            
            // ecuacion matricial
            NV_DATA_S(rr)[k++] = feenox_expression_eval(&dae->residual);
            
          }
        }
      }
    }
  }
  
#else
 int feenox_ida_dae(void) {
#endif

   return FEENOX_OK;

}

// instruccion dummy
int feenox_instruction_dae(void *arg) {
  
  return FEENOX_OK;
  
}
