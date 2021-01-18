/*------------ -------------- -------- --- ----- ---   --       -            -
 *  wasora history processing
 *
 *  Copyright (C) 2009--2014 jeremy theler
 *
 *  This file is part of wasora.
 *
 *  wasora is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  wasora is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with wasora.  If not, see <http://www.gnu.org/licenses/>.
 *------------------- ------------  ----    --------  --     -       -         -
 */

#ifndef _WASORA_H_
#include "wasora.h"
#endif

// agrega el valor actual de una variable a la funcion historia
int wasora_instruction_history(void *arg) {
  history_t *history = (history_t *)arg;

  if (history->function->data_size == 0) {
    wasora_init_history(history);
    }
  
  if (history->position >= history->function->data_size) {
    history->function->data_size *= 2;
    history->function->data_argument[0] = realloc(history->function->data_argument[0], sizeof(double)*history->function->data_size);
    history->function->data_value = realloc(history->function->data_value, sizeof(double)*history->function->data_size);
  }

  // OJO! function->size es el maximo tamanio esperado
  //      history->position es la cantidad de puntos conocidos
  history->function->data_argument[0][history->position] = wasora_var(wasora_special_var(time));
  history->function->data_value[history->position] = wasora_value(history->variable);

  history->position++;

//  gsl_interp_init(history->function->interp, history->function->data_argument[0], history->function->data_value, history->function->data_size);
  
  return WASORA_RUNTIME_OK;
}

// inicializa una historia
void wasora_init_history(history_t *history) {

  int j;
  int size;
  
  if (wasora_var(wasora_special_var(dt)) != 0) {
    size = (size_t)5*fabs(1+wasora_value(wasora_special_var(end_time))/wasora_value(wasora_special_var(dt)));
  } else {
    size = 5;
  }

  history->function->data_size = size;
  history->function->interp_type = DEFAULT_INTERPOLATION;

  history->function->data_argument = malloc(sizeof(double *));
  history->function->data_argument[0] = calloc(size, sizeof(double));
  history->function->data_value = calloc(size, sizeof(double));

  // inicializamos para menos infinito todo igual a lo que habia en t = 0
  // 5 valores porque es lo minimo que quiere akima
  history->function->data_argument[0][0] = -wasora_var(wasora_special_var(infinite));
  history->function->data_argument[0][1] = -0.8*wasora_var(wasora_special_var(infinite));
  history->function->data_argument[0][2] = -0.6*wasora_var(wasora_special_var(infinite));
  history->function->data_argument[0][3] = -0.4*wasora_var(wasora_special_var(infinite));
  history->function->data_argument[0][4] = -0.2*wasora_var(wasora_special_var(infinite));

  history->function->data_value[0] = history->variable->initial_transient[0];
  history->function->data_value[1] = history->variable->initial_transient[0];
  history->function->data_value[2] = history->variable->initial_transient[0];
  history->function->data_value[3] = history->variable->initial_transient[0];
  history->function->data_value[4] = history->variable->initial_transient[0];

  history->position = 5;

  // pero ademas tenemos que llenar los lugares que faltan con infinitos porque
  // las absisas tienen que estar ordenadas
  for (j = history->position; j < history->function->data_size; j++) {
    history->function->data_argument[0][j] = wasora_var(wasora_special_var(infinite))*(1+1e-3*j);
  }


  history->function->interp = gsl_interp_alloc(&history->function->interp_type, history->function->data_size);
  history->function->interp_accel = gsl_interp_accel_alloc();
  
  gsl_interp_init(history->function->interp, history->function->data_argument[0], history->function->data_value, history->function->data_size);

  return;
  
}

