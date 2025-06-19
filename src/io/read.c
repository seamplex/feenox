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
    fscanf(read_data->file->pointer, "%lf", read_data->variable->value);
  } else {
    double xi = 0;
    for (int i = 0; i < read_data->vector->size; i++) {
      fscanf(read_data->file->pointer, "%lf", &xi);
      gsl_vector_set(read_data->vector->value, i, xi);
    }
  }
  
  return FEENOX_OK;
}
