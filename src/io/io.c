/*------------ -------------- -------- --- ----- ---   --       -            -
 *  wasora I/O routines
 *
 *  Copyright (C) 2009--2015 Jeremy Theler
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

#include <stdio.h>
#include <string.h>

#ifndef _WASORA_H_
#include "wasora.h"
#endif

int wasora_instruction_sem(void *arg) {
  struct semaphore_t *semaphore = (struct semaphore_t *)arg;
  
  if (!semaphore->initialized) {
    if ((semaphore->pointer = wasora_get_semaphore(semaphore->name)) == NULL) {
      wasora_push_error_message("opening semaphore '%s'", semaphore->name);
      return WASORA_RUNTIME_ERROR;
    }
    semaphore->initialized = 1;
  }  
  

  if (semaphore->operation == wasora_sem_wait) {
    sem_wait(semaphore->pointer);
  } else if (semaphore->operation == wasora_sem_post) {
    sem_post(semaphore->pointer);
  }
  
  return WASORA_RUNTIME_OK;
}

int wasora_io_init(io_t *io) {
  
  io_thing_t *thing;
  
  io->initialized = 1;
  
  LL_FOREACH(io->things, thing) {
  
    if (thing->variable != NULL) {

      io->size += 1;

    } else if (thing->vector != NULL) {

      if (!thing->vector->initialized) {
        wasora_call(wasora_vector_init(thing->vector));
      }

      thing->row_min = (thing->expr_row_min.n_tokens != 0) ? (int)wasora_evaluate_expression(&thing->expr_row_min)-1 : 0;
      thing->row_max = (thing->expr_row_max.n_tokens != 0) ? (int)wasora_evaluate_expression(&thing->expr_row_max)   : thing->vector->size;

      if (thing->row_max < thing->row_min) {
        wasora_push_error_message("mix row '%d' is greater that max row '%d for vector '%s'", thing->row_min, thing->row_max, thing->vector->name);
        return WASORA_RUNTIME_ERROR;
      }

      io->size += thing->row_max - thing->row_min;

    } else if (thing->matrix != NULL) {

      if (!thing->matrix->initialized) {
        wasora_call(wasora_matrix_init(thing->matrix));
      }

      thing->row_min = (thing->expr_row_min.n_tokens != 0) ? (int)wasora_evaluate_expression(&thing->expr_row_min)-1 : 0;
      thing->row_max = (thing->expr_row_max.n_tokens != 0) ? (int)wasora_evaluate_expression(&thing->expr_row_max)   : thing->matrix->rows;

      thing->col_min = (thing->expr_col_min.n_tokens != 0) ? (int)wasora_evaluate_expression(&thing->expr_col_min)-1 : 0;
      thing->col_max = (thing->expr_col_max.n_tokens != 0) ? (int)wasora_evaluate_expression(&thing->expr_col_max)   : thing->matrix->cols;
      
      if (thing->row_max < thing->row_min) {
        wasora_push_error_message("mix row '%d' is greater that max row '%d in matrix '%s'", thing->row_min, thing->row_max, thing->matrix->name);
        return WASORA_PARSER_ERROR;
      }

      if (thing->col_max < thing->col_min) {
        wasora_push_error_message("mix col '%d' is greater that max col '%d in matrix '%s'", thing->col_min, thing->col_max, thing->matrix->name);
        return WASORA_PARSER_ERROR;
      }

      io->size += (thing->row_max-thing->row_min)*(thing->col_max-thing->col_min);

    } else {

      io->size += 1;

    }  
  }
  
  if (io->type == io_shm && io->size == 0)  {
    wasora_push_error_message("empty shared memory object '%s'", io->shm_name);
    return WASORA_PARSER_ERROR;
  }
  
  // allocamos el objeto de memoria compartida 
  if (io->shm_name != NULL) {
/*      
      switch (io->floating_point_format) {
        case format_double:
*/
          if ((io->shm_pointer_double = wasora_get_shared_pointer(io->shm_name, io->size * sizeof(double))) == NULL) {
            wasora_push_error_message("\"%s\" allocating shared memory object '%s'", strerror(errno), io->shm_name);
            return WASORA_RUNTIME_ERROR;
          }
/*
        break;
        case format_float:
          if ((io->shm_pointer_float = wasora_get_shared_pointer(io->shm_name, io->size * sizeof(float))) == NULL) {
            wasora_push_error_message("\"%s\" allocating shared memory object '%s'", strerror(errno), io->shm_name);
            return WASORA_RUNTIME_ERROR;
          }
        break;
        case format_char:
          if ((io->shm_pointer_char = wasora_get_shared_pointer(io->shm_name, io->size * sizeof(char))) == NULL) {
            wasora_push_error_message("\"%s\" allocating shared memory object '%s'", strerror(errno), io->shm_name);
            return WASORA_RUNTIME_ERROR;
          }
        break;
      }
*/      
    }
  
  
  return WASORA_RUNTIME_OK;
}

int wasora_instruction_io(void *arg) {
  io_t *io = (io_t *)arg;
  io_thing_t *thing;
  int i, j;
  int offset;
  
  // TODO: give a stride to allow non-double IOs
  int (*single_io_operation)(io_t *, double *, int) = NULL;
  
  if (!io->initialized) {
    wasora_call(wasora_io_init(io));
  // TODO: poner esto en el parser
  // TODO: poner esto en el parser
  }

  // TODO: poner esto en el parser
  switch (io->type) {
    case io_undefined:
    break;
    case io_tcp:
    break;
    case io_comedi:
    break;
    case io_shm:
      if (io->direction == io_read) {
        single_io_operation = wasora_io_read_shm;
      } else {
        single_io_operation = wasora_io_write_shm;
      }
    break;
    case io_file_ascii:
      if (io->direction == io_read) {
        single_io_operation = wasora_io_read_ascii_file;
      } else {
        single_io_operation = wasora_io_write_ascii_file;
      }
    break;
    case io_file_binary:
      if (io->direction == io_read) {
        single_io_operation = wasora_io_read_binary_file;
      } else {
        single_io_operation = wasora_io_write_binary_file;
      }
    break;
  }

  if (single_io_operation == NULL) {
    wasora_push_error_message("do not know how to handle I/O");
    return WASORA_RUNTIME_ERROR;
  }
  
  // mapeamos el pointer de la estructura file en cada paso (porque puede haber
  // habido OPENs o CLOSEs y nosotros ni enterarnos desde READ/WRITE)
  // capaz que se pueda hacer un apuntador y ya
  if (io->file != NULL) {
    io->filepointer = io->file->pointer;
  }
  
  
  offset = 0;
  LL_FOREACH(io->things, thing) {

    if (thing->variable != NULL) {
      wasora_call(single_io_operation(io, wasora_value_ptr(thing->variable), offset++));
      wasora_check_initial_variable(thing->variable);
    } else if (thing->vector != NULL) {
      for (i = thing->row_min; i < thing->row_max; i++) {
        wasora_call(single_io_operation(io, gsl_vector_ptr(wasora_value_ptr(thing->vector), i), offset++));
      }
      wasora_check_initial_vector(thing->vector);
    } else if (thing->matrix != NULL) {
      for (i = thing->row_min; i < thing->row_max; i++) {
        for (j = thing->col_min; j < thing->col_max; j++) {
          wasora_call(single_io_operation(io, gsl_matrix_ptr(wasora_value_ptr(thing->matrix), i, j), offset++));
        }
      }
      wasora_check_initial_matrix(thing->matrix);
    } else {
      double xi = wasora_evaluate_expression(&thing->expr);
      wasora_call(single_io_operation(io, &xi, offset++));
    }
  }

  
  // cerramos el archivo solo si es FILE_PATH, si es FILE lo cerramos explicitamente (o no)
  if (io->file == NULL && io->filepointer != NULL) {
    fclose(io->filepointer);
    io->filepointer = NULL;
  }
  return WASORA_PARSER_OK;

}


int wasora_io_read_shm(io_t *io, double *data, int offset) {
  double xi;

  xi = io->shm_pointer_double[offset];
  if (io->ignorenull == 0 || xi != 0) {
    *data = xi;
  }

  return WASORA_RUNTIME_OK;
}

int wasora_io_write_shm(io_t *io, double *data, int offset) {

  io->shm_pointer_double[offset] = *data;

  return WASORA_RUNTIME_OK;
}



int wasora_io_read_ascii_file(io_t *io, double *data, int offset) {
  int gotit;

  if (io->filepointer == NULL) {
    if (io->file != NULL) {
      if (io->file->pointer == NULL) {
        if (wasora_instruction_open_file(io->file) != WASORA_RUNTIME_OK) {
          return WASORA_RUNTIME_ERROR;
        }
      }
      io->filepointer = io->file->pointer;
    }
  }

  do {
    if (feof(io->filepointer)) {
      wasora_push_error_message("end of file while reading '%s'", io->file->path);
      return WASORA_RUNTIME_ERROR;
    }
    if ((gotit = fscanf(io->filepointer, "%lf", data)) == 0) {
      if (fscanf(io->filepointer, "%*s") == 0) {
        wasora_push_error_message("error reading file");
        return WASORA_RUNTIME_ERROR;
      }
    }
  } while(gotit == 0);

  return WASORA_RUNTIME_OK;
}


int wasora_io_write_ascii_file(io_t *io, double *data, int offset) {

  if (io->filepointer == NULL) {
    if (io->file != NULL) {
      if (io->file->pointer == NULL) {
        if (wasora_instruction_open_file(io->file) != WASORA_RUNTIME_OK) {
          return WASORA_RUNTIME_ERROR;
        }
      }
      io->filepointer = io->file->pointer;
    }
  }

  fprintf(io->filepointer, "%e\n", *data);

  return WASORA_RUNTIME_OK;
}


int wasora_io_read_binary_file(io_t *io, double *data, int offset) {

  if (io->filepointer == NULL) {
    if (io->file != NULL) {
      if (io->file->pointer == NULL) {
        if (wasora_instruction_open_file(io->file) != WASORA_RUNTIME_OK) {
          return WASORA_RUNTIME_ERROR;
        }
      }
      io->filepointer = io->file->pointer;
    }
  }

  if (fread(data, sizeof(double), 1, io->filepointer) == 0) {
    wasora_push_error_message("end of file while reading '%s'", io->file->path);
    return WASORA_RUNTIME_ERROR;
  }

  return WASORA_RUNTIME_OK;
}

int wasora_io_write_binary_file(io_t *io, double *data, int offset) {

  if (io->filepointer == NULL) {
    if (io->file != NULL) {
      if (io->file->pointer == NULL) {
        if (wasora_instruction_open_file(io->file) != WASORA_RUNTIME_OK) {
          return WASORA_RUNTIME_ERROR;
        }
      }
      io->filepointer = io->file->pointer;
    }
  }

  fwrite(data, sizeof(double), 1, io->filepointer);

  return WASORA_RUNTIME_OK;
}

