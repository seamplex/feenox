/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox read data from files
 *
 *  Copyright (C) 2025 Jeremy Theler
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
#include "feenox.h"

int feenox_instruction_read_data(void *arg) {
 
  read_data_t *read_data = (read_data_t *)arg;
  if (read_data->file->pointer == NULL) {
    feenox_call(feenox_instruction_file_open(read_data->file));
  }  
  
  if (read_data->variable != NULL) {
    if (fscanf(read_data->file->pointer, "%lf", read_data->variable->value) != 1) {
      feenox_push_error_message("error reading data from file '%s' into variable '%s'", read_data->file->name, read_data->variable->name);
      return FEENOX_ERROR;
    }
  } else {
    
    if (read_data->vector->initialized == 0) {
      if (feenox_vector_init(read_data->vector, FEENOX_VECTOR_INITIAL) != FEENOX_OK) {
        return FEENOX_ERROR;
      }
    }    
    double xi = 0;
    for (int i = 0; i < read_data->vector->size; i++) {
      if (fscanf(read_data->file->pointer, "%lf", &xi) != 1) {
        feenox_push_error_message("error reading data from file '%s' into vector '%s' at offset %d", read_data->file->name, read_data->vector->name, i+1);
        return FEENOX_ERROR;
      }
      gsl_vector_set(read_data->vector->value, i, xi);
    }
  }
  
  return FEENOX_OK;
}
