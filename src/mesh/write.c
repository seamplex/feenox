/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's mesh-related post-processing generation routines
 *
 *  Copyright (C) 2014--2021 jeremy theler
 *
 *  This file is part of feenox.
 *
 *  feenox is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  feenox is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with feenox.  If not, see <http://www.gnu.org/licenses/>.
 *------------------- ------------  ----    --------  --     -       -         -
 */
#include "../feenox.h"
extern feenox_t feenox;

int feenox_instruction_mesh_write(void *arg) {

  mesh_write_t *mesh_write = (mesh_write_t *)arg;
  mesh_write_dist_t *mesh_write_dist;

  // TODO: in parallel runs only print from first processor
  if (feenox.rank != 0) {
    return FEENOX_OK;
  }
  
  if (feenox_special_var(end_time) == 0) {
    // close the file and open it again
    if (mesh_write->file->pointer != NULL) {
      feenox_call(feenox_instruction_file_close(mesh_write->file));
    }
    feenox_call(feenox_instruction_file_open(mesh_write->file));  
  } else {
    if (mesh_write->file->pointer == NULL) {
      feenox_call(feenox_instruction_file_open(mesh_write->file));  
    }
  }  


  if (ftell(mesh_write->file->pointer) == 0) {
    feenox_call(mesh_write->write_header(mesh_write->file->pointer));
    if (mesh_write->no_mesh == 0) {
      feenox_call(mesh_write->write_mesh(mesh_write->mesh, mesh_write->no_physical_names, mesh_write->file->pointer));
    }
    mesh_write->point_init = 0;
  }

  LL_FOREACH(mesh_write->mesh_write_dists, mesh_write_dist) {
    if (mesh_write_dist->field_location == field_location_cells && mesh_write->mesh->n_cells == 0) {
      feenox_call(feenox_mesh_element2cell(mesh_write->mesh));
    }

    if (mesh_write_dist->scalar != NULL) {
      feenox_call(mesh_write->write_scalar(mesh_write, mesh_write_dist->scalar, mesh_write_dist->field_location, mesh_write_dist->printf_format));
    } else {
      feenox_call(mesh_write->write_vector(mesh_write, mesh_write_dist->vector, mesh_write_dist->field_location, mesh_write_dist->printf_format));
    }
    // TODO: tensors
  }
  
  // as vtk does not support multiple time steps, it is better to close the file now
  if (mesh_write->post_format == post_format_vtk) {
    feenox_call(feenox_instruction_file_close(mesh_write->file));
  }
  // for .msh we leave that to the user, to use CLOSE or whatever explicitly

  return FEENOX_OK;
}
