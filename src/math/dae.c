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

// API
int feenox_add_time_path(const char *token) {
  expr_t *expr;
  
  if ((expr = calloc(1, sizeof(expr))) == NULL) {
    feenox_push_error_message("memory allocation failed for expression in feenox_add_time_path()");
    return FEENOX_ERROR;
  }
  
  feenox_call(feenox_parse_expression(token, expr));
  LL_APPEND(feenox.time_paths, expr);
  
  return FEENOX_OK;
}

/*
int feenox_dae_init(void) {
  
#ifdef HAVE_IDA
  int i, k, l;
  phase_object_t *phase_object;
  dae_t *dae;
  
  // primero calculamos el tamanio del phase space
  feenox_dae.dimension = 0;
  LL_FOREACH(feenox_dae.phase_objects, phase_object) {
    
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
    
    feenox_dae.dimension += phase_object->size;
      
  }
  
  if (feenox_dae.dimension == 0) {
    feenox_push_error_message("empty phase space in PHASE_SPACE keyword");
    return WASORA_RUNTIME_ERROR;
  }

  // alocamos
  feenox_dae.phase_value = malloc(feenox_dae.dimension * sizeof(double *));
  feenox_dae.phase_derivative = malloc(feenox_dae.dimension * sizeof(double *));
  
  i = 0;  
  LL_FOREACH(feenox_dae.phase_objects, phase_object) {

    if (phase_object->variable != NULL) {
      phase_object->offset = i;
      feenox_dae.phase_value[i] = feenox_value_ptr(phase_object->variable);
      feenox_dae.phase_derivative[i] = feenox_value_ptr(phase_object->variable_dot);
      i++;
      
    } else if (phase_object->vector != NULL) {

      phase_object->offset = i;
      for (k = 0; k < phase_object->vector->size; k++) {
        feenox_dae.phase_value[i] = gsl_vector_ptr(feenox_value_ptr(phase_object->vector), k);
        feenox_dae.phase_derivative[i] = gsl_vector_ptr(feenox_value_ptr(phase_object->vector_dot), k);
        i++;
      }

    } else if (phase_object->matrix != NULL) {

      phase_object->offset = i;
    for (k = 0; k < phase_object->matrix->rows; k++) {
        for (l = 0; l < phase_object->matrix->cols; l++) {
          feenox_dae.phase_value[i] = gsl_matrix_ptr(feenox_value_ptr(phase_object->matrix), k, l);
          feenox_dae.phase_derivative[i] = gsl_matrix_ptr(feenox_value_ptr(phase_object->matrix_dot), k, l);
          i++;
        }
      }
    }
  }

  // procesamos las DAEs
  i = 0;
  LL_FOREACH(feenox_dae.daes, dae) {
    
    if (dae->matrix != NULL) {
      
      if (!dae->matrix->initialized) {
        feenox_call(feenox_matrix_init(dae->matrix));
      }
      
      dae->i_min = (dae->expr_i_min.n_tokens != 0) ? feenox_evaluate_expression(&dae->expr_i_min)-1 : 0;
      dae->i_max = (dae->expr_i_max.n_tokens != 0) ? feenox_evaluate_expression(&dae->expr_i_max)   : dae->matrix->rows;
      if (dae->i_max <= dae->i_min) {
        feenox_push_error_message("i_max %d is smaller or equal than i_min %d", dae->i_max, dae->i_min+1);
      }

      dae->j_min = (dae->expr_j_min.n_tokens != 0) ? feenox_evaluate_expression(&dae->expr_j_min)-1 : 0;
      dae->j_max = (dae->expr_j_max.n_tokens != 0) ? feenox_evaluate_expression(&dae->expr_j_max)   : dae->matrix->cols;
      if (dae->j_max <= dae->j_min) {
        feenox_push_error_message("j_max %d is smaller or equal than j_min %d", dae->j_max, dae->j_min+1);
      }
      i += (dae->i_max - dae->i_min) * (dae->j_max - dae->j_min);

    } else if (dae->vector != NULL) {
      
      if (!dae->vector->initialized) {
        feenox_call(feenox_vector_init(dae->vector));
      }
      
      dae->i_min = (dae->expr_i_min.n_tokens != 0) ? feenox_evaluate_expression(&dae->expr_i_min)-1 : 0;
      dae->i_max = (dae->expr_i_max.n_tokens != 0) ? feenox_evaluate_expression(&dae->expr_i_max)   : dae->vector->size;
      if (dae->i_max <= dae->i_min) {
        feenox_push_error_message("i_max %d is smaller or equal than i_max %d", dae->i_max, dae->i_min+1);
      }
    
      i += (dae->i_max - dae->i_min);
    } else {
     
      // scalar
      i++;
      
    }
  }
  
  if (i > feenox_dae.dimension) {
    feenox_push_error_message("more DAE equations than phase space dimension %d", feenox_dae.dimension);
    return WASORA_PARSER_ERROR;
  } else if (i < feenox_dae.dimension) {
    feenox_push_error_message("less DAE equations than phase space dimension %d", feenox_dae.dimension);
    return WASORA_PARSER_ERROR;
  }
  
  
  

  feenox_dae.x = N_VNew_Serial(feenox_dae.dimension);
  feenox_dae.dxdt = N_VNew_Serial(feenox_dae.dimension);
  feenox_dae.id = N_VNew_Serial(feenox_dae.dimension);

  feenox_dae.system = IDACreate();
  
  // llenamos el vector de ida con las variables del usuario 
  for (i = 0; i < feenox_dae.dimension; i++) {
    NV_DATA_S(feenox_dae.x)[i] = *(feenox_dae.phase_value[i]);
    NV_DATA_S(feenox_dae.dxdt)[i] = *(feenox_dae.phase_derivative[i]);
    // suponemos que son algebraicas, despues marcamos las diferenciales
    // pero hay que inicializar el vector id!
    NV_DATA_S(feenox_dae.id)[i] = DAE_ALGEBRAIC; 
  }
  // inicializamos IDA  
  if (IDAInit(feenox_dae.system, feenox_ida_dae, feenox_var(feenox_special_var(time)), feenox_dae.x, feenox_dae.dxdt) != IDA_SUCCESS) {
    return WASORA_RUNTIME_ERROR;
  }

  // seteo de tolerancias 
  if ((feenox.special_vectors.abs_error = feenox_get_vector_ptr("abs_error")) != NULL) {

    // si hay un vector de absolutas, lo usamos 
    if (feenox.special_vectors.abs_error->size != feenox_dae.dimension) {
      feenox_push_error_message("dimension of system is %d and dimension of abs_error is %d", feenox_dae.dimension, feenox.special_vectors.abs_error->size);
      return WASORA_RUNTIME_ERROR;
    }

    feenox_dae.abs_error = N_VNew_Serial(feenox_dae.dimension);

    for (i = 0; i < feenox_dae.dimension; i++) {
      if (gsl_vector_get(feenox_value_ptr(feenox.special_vectors.abs_error), i) > 0) {
        NV_DATA_S(feenox_dae.abs_error)[i] = gsl_vector_get(feenox_value_ptr(feenox.special_vectors.abs_error), i);
      } else {
        NV_DATA_S(feenox_dae.abs_error)[i] = feenox_var(feenox_special_var(rel_error));
      }
    }

    if (IDASVtolerances(feenox_dae.system, feenox_var(feenox_special_var(rel_error)), feenox_dae.abs_error) != IDA_SUCCESS) {
      return WASORA_RUNTIME_ERROR;
    }
  } else {
    // si no nos dieron vector de absolutas, ponemos absolutas igual a relativas 
    if (IDASStolerances(feenox_dae.system, feenox_var(feenox_special_var(rel_error)), feenox_var(feenox_special_var(rel_error))) != IDA_SUCCESS) {
      return WASORA_RUNTIME_ERROR;
    }
  }

  //  estas tienen que venir despues de IDAInit

  // marcamos como differential aquellos elementos del phase space que lo sean
  LL_FOREACH(feenox_dae.phase_objects, phase_object) {
    if (phase_object->differential) {
      for (i = phase_object->offset; i < phase_object->offset+phase_object->size; i++) {
        NV_DATA_S(feenox_dae.id)[i] = DAE_DIFFERENTIAL;
      }
    }
  }
        
  
  if (IDASetId(feenox_dae.system, feenox_dae.id) != IDA_SUCCESS) {
    return WASORA_RUNTIME_ERROR;
  }
 
#if IDA_VERSION == 2
  if (IDADense(feenox_dae.system, feenox_dae.dimension) != IDA_SUCCESS) {
    return WASORA_RUNTIME_ERROR;
  }
#elif IDA_VERSION == 3
  if ((feenox_dae.A = SUNDenseMatrix(feenox_dae.dimension, feenox_dae.dimension)) == NULL) {
    return WASORA_RUNTIME_ERROR;
  }
    
  if ((feenox_dae.LS = SUNDenseLinearSolver(feenox_dae.x, feenox_dae.A)) == NULL) {
    return WASORA_RUNTIME_ERROR;
  }
  
  if (IDADlsSetLinearSolver(feenox_dae.system, feenox_dae.LS, feenox_dae.A) != IDA_SUCCESS) {
    return WASORA_RUNTIME_ERROR;
  }
#endif
  
  if (IDASetInitStep(feenox_dae.system, feenox_var(feenox_special_var(dt))) != IDA_SUCCESS) {
    return WASORA_RUNTIME_ERROR;
  }
  if (feenox_var(feenox_special_var(max_dt)) != 0) {
    if (IDASetMaxStep(feenox_dae.system, feenox_var(feenox_special_var(max_dt))) != IDA_SUCCESS) {
      return WASORA_RUNTIME_ERROR;
    }
  }
  
  return WASORA_RUNTIME_OK;
#else

  feenox_push_error_message("feenox cannot solve DAE systems as it was not linked against SUNDIALS IDA library.");
  return WASORA_RUNTIME_ERROR;

#endif
  
}

int feenox_dae_ic(void) {
  
#ifdef HAVE_IDA
  int err;

  if (feenox_dae.initial_conditions_mode == from_variables) {
    if ((err = IDACalcIC(feenox_dae.system, IDA_YA_YDP_INIT, feenox_var(feenox_special_var(dt)))) != IDA_SUCCESS) {
      feenox_push_error_message("error computing initial conditions from variable values, error = %d", err);
      return WASORA_RUNTIME_ERROR;
    }
  } else if (feenox_dae.initial_conditions_mode == from_derivatives) {
    if ((err = IDACalcIC(feenox_dae.system, IDA_Y_INIT, feenox_var(feenox_special_var(dt)))) != IDA_SUCCESS)  {
      feenox_push_error_message("error computing initial conditions from derivative values, error = %d", err);
      return WASORA_RUNTIME_ERROR;
    }
  }  
  
#endif
  return WASORA_RUNTIME_OK;
}


#ifdef HAVE_IDA
int feenox_ida_dae(realtype t, N_Vector yy, N_Vector yp, N_Vector rr, void *params) {

  int i, j, k;
  dae_t *dae;
 
  // esta funcion se llama un monton de veces en pasos de tiempo
  // intermedios, entoces le decimos cuanto vale el tiempo nuevo por
  //  si hay residuos que dependen explicitamente del tiempo
  feenox_var(feenox_special_var(time)) = t;

  // copiamos el estado del solver de IDA a feenox
  for (k = 0; k < feenox_dae.dimension; k++) {
    *(feenox_dae.phase_value[k]) = NV_DATA_S(yy)[k];
    *(feenox_dae.phase_derivative[k]) = NV_DATA_S(yp)[k];
  }

  // evaluamos los residuos en feenox y los dejamos en IDA
  k = 0;
  LL_FOREACH(feenox_dae.daes, dae) {
    
    // fall-offs para detectar errores (i.e. una expresion vectorial que dependa de j)
    j = 0;
    i = 0;
    
    if (dae->i_max == 0) {
      // ecuacion escalar
      NV_DATA_S(rr)[k++] = feenox_evaluate_expression(&dae->residual);
      
    } else {
         
      for (i = dae->i_min; i < dae->i_max; i++) {
        feenox_var(feenox_special_var(i)) = (double)i+1;
        if (dae->j_max == 0) {
          
          // ecuacion vectorial
          NV_DATA_S(rr)[k++] = feenox_evaluate_expression(&dae->residual);
        } else {
          for (j = dae->j_min; j < dae->j_max; j++) {
            feenox_var(feenox_special_var(j)) = (double)j+1;
            
            // ecuacion matricial
            NV_DATA_S(rr)[k++] = feenox_evaluate_expression(&dae->residual);
            
          }
        }
      }
    }
  }
  
#else
 int feenox_ida_dae(void) {
#endif

   return WASORA_RUNTIME_OK;

}

// instruccion dummy
int feenox_instruction_dae(void *arg) {
  
  return WASORA_RUNTIME_OK;
  
}
*/