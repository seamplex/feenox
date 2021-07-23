/*------------ -------------- -------- --- ----- ---   --       -            -
 *  DAE evaluation routines
 *
 *  Copyright (C) 2009--2021 jeremy theler
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
extern const char factorseparators[];


// API
int feenox_add_time_path(const char *string) {
  expr_t *expr;
  
  if ((expr = calloc(1, sizeof(expr))) == NULL) {
    feenox_push_error_message("memory allocation failed for expression in feenox_add_time_path()");
    return FEENOX_ERROR;
  }
  
  feenox_call(feenox_expression_parse(expr, string));
  LL_APPEND(feenox.time_paths, expr);
  
  return FEENOX_OK;
}

// API
int feenox_phase_space_add_object(const char *string) {

  var_t *variable = NULL;
  vector_t *vector = NULL;
  matrix_t *matrix = NULL;
  char *buffer = NULL;
  
  phase_object_t *phase_object = calloc(1, sizeof(phase_object_t));
  if ((vector = feenox_get_vector_ptr(string)) != NULL) {

    phase_object->vector = vector;
    phase_object->name = vector->name;

    feenox_check_minusone(asprintf(&buffer, "%s_dot", vector->name));
    feenox_call(feenox_define_vector(buffer, vector->size_expr.string));
    feenox_free(buffer);

  } else if ((matrix = feenox_get_matrix_ptr(string)) != NULL) {

    phase_object->matrix = matrix;
    phase_object->name = matrix->name;

    feenox_check_minusone(asprintf(&buffer, "%s_dot", matrix->name));
    feenox_call(feenox_define_matrix(buffer, matrix->rows_expr.string, matrix->cols_expr.string));
    feenox_free(buffer);

  } else {

    if ((variable = feenox_get_or_define_variable_get_ptr(string)) == NULL) {
      return FEENOX_ERROR;
    }

    phase_object->variable = variable;
    phase_object->name = variable->name;

    feenox_check_minusone(asprintf(&buffer, "%s_dot", variable->name));
    if ((phase_object->variable_dot = feenox_get_or_define_variable_get_ptr(buffer)) == NULL) {
      return FEENOX_ERROR;
    }
    feenox_free(buffer);

  }
  
  LL_APPEND(feenox.dae.phase_objects, phase_object);
  return FEENOX_OK;
}

// API
int feenox_phase_space_mark_diff(const char *string) {

  phase_object_t *phase_object = NULL;  
    
  LL_FOREACH(feenox.dae.phase_objects, phase_object) {
    if ((phase_object->variable != NULL && strcmp(string, phase_object->variable->name) == 0) ||
        (phase_object->vector   != NULL && strcmp(string, phase_object->vector->name) == 0) ||
        (phase_object->matrix   != NULL && strcmp(string, phase_object->matrix->name) == 0) ) {
         phase_object->differential = 1;
         return FEENOX_OK;
    }
  }
  
  feenox_push_error_message("object '%s' is not in the phase space", string);
  return FEENOX_ERROR;
}  


int feenox_add_dae(const char *lhs, const char *rhs) {
  char *dummy;
  phase_object_t *phase_object;
      
  dae_t *dae = calloc(1, sizeof(dae_t));
      
  if (feenox.dae.daes == NULL) {
    // if this is the first DAE we define a instruction
    if ((feenox.dae.instruction = feenox_add_instruction_and_get_ptr(&feenox_instruction_dae, NULL)) == NULL) {
      return FEENOX_ERROR;
    }
    // and we need to put a "finite-state machine" in mode "reading daes"
    // to prevent other instructions to be defined before finishing the DAEs
    // this flag cannot be in the parser structure
    feenox.dae.reading_daes = 1;
  }

  size_t n = strlen(rhs)+strlen(lhs)+8;
  char *residual = malloc(n);
  snprintf(residual, n, "(%s)-(%s)", rhs, lhs);

  // check if the equation is differential or algebraic
  if ((dummy = feenox_find_first_dot(residual)) != NULL) {
    int found = 0;
    LL_FOREACH(feenox.dae.phase_objects, phase_object) {
      if (strcmp(dummy, phase_object->name) == 0) {
        phase_object->differential = 1;
        found = 1;
      }
    }
    feenox_free(dummy);
    if (found == 0) {
      feenox_push_error_message("requested derivative of object '%s' but it is not in the phase space", dummy);
      return FEENOX_ERROR;
    }
  } 

  feenox_call(feenox_expression_parse(&dae->residual, residual));

  size_t n_daes;
  dae_t *tmp;
  LL_COUNT(feenox.dae.daes, tmp, n_daes);
  if (n_daes == feenox.dae.dimension) {
    // if this is the last equation, check if there were any other instructions
    if (feenox.last_defined_instruction != feenox.dae.instruction) {
      feenox_push_error_message("cannot have instructions within DAEs\n");
      return FEENOX_ERROR;
    }
    // turn off the "finite-state machine"
    feenox.dae.reading_daes = 0;
  }
    
  feenox_free(residual);

  LL_APPEND(feenox.dae.daes, dae);
  return FEENOX_OK;
  
}

char *feenox_find_first_dot(const char *s) {

  char *line;
  feenox_check_alloc_null(line = strdup(s));
  char *token = NULL;
  char *dummy = NULL;
  char *wanted = NULL;

  token = strtok(line, factorseparators);
  while (token != NULL) {

    if ((dummy = strstr(token, "_dot")) != NULL) {
      *dummy = '\0';
      wanted = strdup(token);
      feenox_free(line);
      return wanted;
    }

    token = strtok(NULL, factorseparators);
  }

  feenox_free(line);
  return NULL;

}

int feenox_dae_init(void) {
  
#ifdef HAVE_IDA
  if (sizeof(realtype) != sizeof(double)) {
    feenox_push_error_message("SUNDIALS's realtype is not double");
    return FEENOX_ERROR;
  }
  
  // primero calculamos el tamanio del phase space
  feenox.dae.dimension = 0;
  phase_object_t *phase_object = NULL;
  LL_FOREACH(feenox.dae.phase_objects, phase_object) {
    
    if (phase_object->variable != NULL) {
      
      phase_object->size = 1;
      
    } else if (phase_object->vector != NULL) {
      
      if (!phase_object->vector->initialized) {
        feenox_call(feenox_vector_init(phase_object->vector, 0));
      }
      if (!phase_object->vector_dot->initialized) {
        feenox_call(feenox_vector_init(phase_object->vector_dot, 0));
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
  feenox_check_alloc(feenox.dae.phase_value = calloc(feenox.dae.dimension, sizeof(double *)));
  feenox_check_alloc(feenox.dae.phase_derivative = calloc(feenox.dae.dimension, sizeof(double *)));
  
  sunindextype i = 0;  
  LL_FOREACH(feenox.dae.phase_objects, phase_object) {

    if (phase_object->variable != NULL) {
      phase_object->offset = i;
      feenox.dae.phase_value[i] = feenox_value_ptr(phase_object->variable);
      feenox.dae.phase_derivative[i] = feenox_value_ptr(phase_object->variable_dot);
      i++;
      
    } else if (phase_object->vector != NULL) {

      phase_object->offset = i;
      sunindextype k = 0;
      for (k = 0; k < phase_object->vector->size; k++) {
        feenox.dae.phase_value[i] = gsl_vector_ptr(feenox_value_ptr(phase_object->vector), k);
        feenox.dae.phase_derivative[i] = gsl_vector_ptr(feenox_value_ptr(phase_object->vector_dot), k);
        i++;
      }

    } else if (phase_object->matrix != NULL) {

      phase_object->offset = i;
      int k = 0;
      int l = 0;
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
  dae_t *dae = NULL;
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
        feenox_call(feenox_vector_init(dae->vector, 0));
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

  if ((feenox.dae.system = IDACreate()) == NULL)  {
    feenox_push_error_message("failed to create IDA object");
    return FEENOX_ERROR;
  }
  
  // llenamos el vector de ida con las variables del usuario 
  for (i = 0; i < feenox.dae.dimension; i++) {
    NV_DATA_S(feenox.dae.x)[i] = *(feenox.dae.phase_value[i]);
    NV_DATA_S(feenox.dae.dxdt)[i] = *(feenox.dae.phase_derivative[i]);
    // suponemos que son algebraicas, despues marcamos las diferenciales
    // pero hay que inicializar el vector id!
    // TODO: user IDA's header
    NV_DATA_S(feenox.dae.id)[i] = DAE_ALGEBRAIC; 
  }
  // initialize IDA
  int err = 0; // this is used inside the ida_call() macro
  ida_call(IDAInit(feenox.dae.system, feenox_ida_dae, feenox_special_var_value(t), feenox.dae.x, feenox.dae.dxdt));

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

    ida_call(IDASVtolerances(feenox.dae.system, feenox_special_var_value(rel_error), feenox.dae.abs_error));
  } else {
    // si no nos dieron vector de absolutas, ponemos absolutas igual a relativas 
    ida_call(IDASStolerances(feenox.dae.system, feenox_special_var_value(rel_error), feenox_special_var_value(rel_error)));
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
        
  
  ida_call(IDASetId(feenox.dae.system, feenox.dae.id));
 
  if ((feenox.dae.A = SUNDenseMatrix(feenox.dae.dimension, feenox.dae.dimension)) == NULL) {
    return FEENOX_ERROR;
  }
    
  if ((feenox.dae.LS = SUNDenseLinearSolver(feenox.dae.x, feenox.dae.A)) == NULL) {
    return FEENOX_ERROR;
  }

  ida_call(IDADlsSetLinearSolver(feenox.dae.system, feenox.dae.LS, feenox.dae.A));
  ida_call(IDASetInitStep(feenox.dae.system, feenox_special_var_value(dt)));

  if (feenox_special_var_value(max_dt) != 0) {
    ida_call(IDASetMaxStep(feenox.dae.system, feenox_special_var_value(max_dt)));
  }
  
  // set a stop time equal to end_time to prevent overshooting
  // this only works if end_time is constant!
  ida_call(IDASetStopTime(feenox.dae.system, feenox_special_var_value(end_time)));
  
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
 
  // this function is called many times in a single FeenoX time step for intermediate
  // times which are internal to IDA, nevertheless there might be some residuals
  // that explicitly depend on time so we update FeenoX's time with IDA's time
  feenox_special_var_value(t) = t;

  // copy the phase space from IDA to FeenoX
  for (k = 0; k < feenox.dae.dimension; k++) {
    *(feenox.dae.phase_value[k]) = NV_DATA_S(yy)[k];
    *(feenox.dae.phase_derivative[k]) = NV_DATA_S(yp)[k];
  }

  // evaluate the residuals in FeenoX and leave them for IDA
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

// dummy instruction
int feenox_instruction_dae(void *arg) {
  
  return FEENOX_OK;
  
}
