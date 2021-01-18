/*------------ -------------- -------- --- ----- ---   --       -            -
 *  wasora parametric runs
 *
 *  Copyright (C) 2009--2017 jeremy theler
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


#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

#include <sys/types.h>
#include <sys/wait.h>

#ifndef _WASORA_H_
#include "wasora.h"
#endif

int wasora_instruction_parametric(void *arg) {
  return WASORA_RUNTIME_OK;
}

int wasora_parametric_run(void) {
  int i;
  int step = 0;
  int *local_step;
  
  gsl_rng *r = NULL;
  gsl_qrng *q = NULL;
  double *v;
  
  char parallel_semaphore_name[32];
  sem_t *parallel_semaphore = NULL;
  pid_t pid;
  int status;
  
  if (wasora.parametric.max_daughters > 1) {
    sprintf(parallel_semaphore_name, "wasora-%d-XXXXXX", getpid());
    if (mkdtemp(parallel_semaphore_name) == NULL) {
      wasora_push_error_message("mkdtemp call failed");
      return WASORA_RUNTIME_ERROR;
    }
    if ((parallel_semaphore = sem_open(parallel_semaphore_name, O_CREAT, 0644, 0)) == SEM_FAILED) {
      wasora_push_error_message("cannot open parallel semaphore");
      return WASORA_RUNTIME_ERROR;
    }
    
    // arrancamos con el semaforo en max_daughters
    for (i = 0; i < wasora.parametric.max_daughters; i++) {
      sem_post(parallel_semaphore);
    }
  }
          
  local_step = calloc(wasora.parametric.dimensions, sizeof(int));
  v = calloc(wasora.parametric.dimensions, sizeof(double));
  wasora.parametric.nsteps = calloc(wasora.parametric.dimensions, sizeof(int));

  
  // hacemos una primer pasada hasta donde esta el parametric
  // para asignar las variables de las cuales pueda depender el parametric
  wasora.ip = wasora.instructions;
  while (wasora.ip != NULL && wasora.ip->routine != wasora_instruction_parametric) {
    wasora_call(wasora.ip->routine(wasora.ip->argument));

    if (wasora.next_flow_instruction != NULL) {
      wasora.ip = wasora.next_flow_instruction;
      wasora.next_flow_instruction = NULL;
    } else {
      wasora.ip = wasora.ip->next;
    }
  }
  
  
  wasora.parametric.min = calloc(wasora.parametric.dimensions, sizeof(double));
  wasora.parametric.max = calloc(wasora.parametric.dimensions, sizeof(double));
  wasora.parametric.step = calloc(wasora.parametric.dimensions, sizeof(double));
  
  for (i = 0; i < wasora.parametric.dimensions; i++) {
    if (wasora.parametric.range.min != NULL) {
      wasora.parametric.min[i] = wasora_evaluate_expression(&wasora.parametric.range.min[i]);
    } else {
      wasora.parametric.min[i] = 0.0;
    }


    if (wasora.parametric.range.max != NULL) {
      wasora.parametric.max[i] = wasora_evaluate_expression(&wasora.parametric.range.max[i]);
    } else {
      wasora.parametric.max[i] = 1.0;
    }

    // el step no importa
    if (wasora.parametric.range.step != NULL) {
      wasora.parametric.step[i] = wasora_evaluate_expression(&wasora.parametric.range.step[i]);
    } else if (wasora.parametric.range.nsteps != NULL) {
      wasora.parametric.step[i] = (wasora.parametric.max[i]-wasora.parametric.min[i])/(wasora_evaluate_expression(&wasora.parametric.range.nsteps[i])-1);
    }

    if (wasora.parametric.max[i] < wasora.parametric.min[i]) {
      wasora_push_error_message("min (%e) greater than max (%e)", wasora.parametric.min[i], wasora.parametric.max[i]);
      return WASORA_PARSER_ERROR;
    }
    if (wasora.parametric.max[i]-wasora.parametric.min[i] < wasora_value(wasora_special_var(zero))) {
      wasora_push_error_message("min and max too close");
      return WASORA_PARSER_ERROR;
    }

    if (wasora.parametric.outer_steps == 0 && wasora.parametric.step[0] == 0) {
      wasora_push_error_message("neither STEP, NSTEPS nor TOTAL_STEPS keywords given");
      return WASORA_PARSER_ERROR;
    }
  }
  
  
  if (wasora.parametric.outer_steps == 0) {
    wasora.parametric.outer_steps = 1;
    for (i = 0; i < wasora.parametric.dimensions; i++) {
      wasora.parametric.nsteps[i] = (int)((wasora.parametric.max[i] - wasora.parametric.min[i])/wasora.parametric.step[i]);
      wasora.parametric.outer_steps *= 1 + wasora.parametric.nsteps[i];
    }
  } else {
    for (i = 0; i < wasora.parametric.dimensions; i++) {
      wasora.parametric.step[i] = (wasora.parametric.max[i] - wasora.parametric.min[i])/pow((double)wasora.parametric.outer_steps, 1.0/(double)wasora.parametric.dimensions);
      wasora.parametric.nsteps[i] = (int)((wasora.parametric.max[i] - wasora.parametric.min[i])/wasora.parametric.step[i]);
    }
  }
  
  // the offset only has meaning for rng methods
  wasora.parametric.outer_steps -= wasora.parametric.offset;
 
  for (step = 0; step < wasora.parametric.outer_steps; step++) {
    wasora_var(wasora.special_vars.in_outer_initial) = (step == 0)?1:0;
    wasora_var(wasora.special_vars.step_outer) = (double)(step+1);

    // calculamos el vector de parametros
    switch (wasora.parametric.type) {
      case parametric_linear:
        for (i = 0; i < wasora.parametric.dimensions; i++) {
          v[i] = (double)local_step[i] * wasora.parametric.step[i]/(wasora.parametric.max[i] - wasora.parametric.min[i]);
        }
      break;
      case parametric_logarithmic:
        for (i = 0; i < wasora.parametric.dimensions; i++) {
//            v[i] = (gsl_sf_exp((double)local_step[i]/(double)(wasora.parametric.steps[i]-1))-1)/(M_E-1);
          v[i] = (wasora.parametric.min[i]*gsl_sf_exp((double)local_step[i]*gsl_sf_log(wasora.parametric.max[i]/wasora.parametric.min[i])/(double)(wasora.parametric.nsteps[i]-1))-wasora.parametric.min[i])/(wasora.parametric.max[i]-wasora.parametric.min[i]);
        }
        break;
      case parametric_random:
        if (r == NULL) {
          r = gsl_rng_alloc(DEFAULT_RANDOM_METHOD);
          gsl_rng_set(r, (unsigned long int)(time(NULL)));
        }
        for (i = 0; i < wasora.parametric.dimensions; i++) {
          v[i] = gsl_rng_uniform(r);
        }
      break;
      case parametric_gaussianrandom:
        if (r == NULL) {
          r = gsl_rng_alloc(DEFAULT_RANDOM_METHOD);
          gsl_rng_set(r, (unsigned long int)(time(NULL)));
        }
        for (i = 0; i < wasora.parametric.dimensions; i++) {
          // el rango es tres sigma
          v[i] = 0.5 + gsl_ran_gaussian(r, 0.5/3.0);
        }
      break;
      case parametric_sobol:
        if (q == NULL) {
          q = gsl_qrng_alloc(gsl_qrng_sobol, wasora.parametric.dimensions);
          for (i = 0; i < wasora.parametric.offset; i++) {
            gsl_qrng_get(q, v);
          }
        }
        gsl_qrng_get(q, v);
      break;
      case parametric_niederreiter:
        if (q == NULL) {
          q = gsl_qrng_alloc(gsl_qrng_niederreiter_2, wasora.parametric.dimensions);
          for (i = 0; i < wasora.parametric.offset; i++) {
            gsl_qrng_get(q, v);
          }
        }
        gsl_qrng_get(q, v);
      break;
      case parametric_halton:
        if (q == NULL) {
          q = gsl_qrng_alloc(gsl_qrng_halton, wasora.parametric.dimensions);
          for (i = 0; i < wasora.parametric.offset; i++) {
            gsl_qrng_get(q, v);
          }
        }
        gsl_qrng_get(q, v);
      break;
      case parametric_reversehalton:
        if (q == NULL) {
          q = gsl_qrng_alloc(gsl_qrng_reversehalton, wasora.parametric.dimensions);
          for (i = 0; i < wasora.parametric.offset; i++) {
            gsl_qrng_get(q, v);
          }
        }
        gsl_qrng_get(q, v);
      break;
    }
    
    for (i = 0; i < wasora.parametric.dimensions; i++) {
      wasora_value(wasora.parametric.variable[i]) = wasora.parametric.min[i] + v[i]*(wasora.parametric.max[i] - wasora.parametric.min[i]);
    }
    

    // miramos si es el ultimo pase
    wasora_var(wasora_special_var(done_outer)) = (step == wasora.parametric.outer_steps-1);

    if (wasora.parametric.max_daughters <= 1) {
      
      // corremos el programa en modo estandar
      wasora_call(wasora_standard_run());
      
    } else {
      
      // esperamos a que haya lugar
      sem_wait(parallel_semaphore);
      if (step > wasora.parametric.max_daughters) {
        // si ya largamos mas que los que nos pidieron hacemos join
        wait(&status);
        if (!WIFEXITED(status)) {
          wasora_push_error_message("child did not exit succesfully");
          return WASORA_RUNTIME_ERROR;
        }
      }
      
      // lanzamos
      if ((pid = fork()) == 0) {
        wasora_call(wasora_standard_run());
        sem_post(parallel_semaphore);
        exit(0);
        
      } else {
        if (pid == -1) {
          wasora_push_error_message("'%s' when forking", strerror(errno));
          return WASORA_RUNTIME_ERROR;
        }
      }
    }
    
    // incrementamos steps
    ++local_step[wasora.parametric.dimensions-1];
    for (i = wasora.parametric.dimensions-2; i >= 0; i--) {
      if (local_step[i+1] == (wasora.parametric.nsteps[i+1]+1)) {
        local_step[i+1] = 0;
        ++local_step[i];
      }
    }
    
  }

  // esperamos a los que nos quedan
  if (wasora.parametric.max_daughters > 1) {
    for (i = 0; i < wasora.parametric.max_daughters; i++) {
      wait(&status);
      if (!WIFEXITED(status)) {
        wasora_push_error_message("child did not exit succesfully");
        return WASORA_RUNTIME_ERROR;
      }
    }
    sem_close(parallel_semaphore);
    sem_unlink(parallel_semaphore_name);
  }
  
  if (r != NULL) {
    gsl_rng_free(r);
  }
  if (q != NULL) {
    gsl_qrng_free(q);
  }
  free(local_step);
  free(wasora.parametric.nsteps);
  free(v);

  return WASORA_RUNTIME_OK;
  
}


