/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's mesh-related post-processing generation routines
 *
 *  Copyright (C) 2014--2025 Jeremy Theler
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
#include "../feenox.h"

int feenox_instruction_mesh_write(void *arg) {
  if (feenox.pde.do_not_solve) {
    return FEENOX_OK;
  }

  mesh_write_t *mesh_write = (mesh_write_t *)arg;

  // TODO: in parallel runs only print from first processor
  if (feenox.mpi_rank != 0) {
    return FEENOX_OK;
  }

  // I don't like these ifs, there should be some "virtual method"
  if (mesh_write->post_format == post_format_gmsh) {  
    // in gmsh we put all the transient steps in the same file
    if (feenox_special_var_value(end_time) == 0) {
      // close the file and open it again
      if (mesh_write->file->pointer != NULL) {
        feenox_call(feenox_instruction_file_close(mesh_write->file));
      }
      feenox_call(feenox_instruction_file_open(mesh_write->file));
    }
  } else if (feenox_special_var_value(end_time) > 0) {
    // check if we have to start a pvd
    if (mesh_write->file_pvd == NULL) {
      feenox_check_alloc(mesh_write->file_pvd = calloc(1, sizeof(file_t)));
      feenox_check_alloc(mesh_write->file_pvd->format = strdup(mesh_write->file->format));
      mesh_write->file_pvd->n_format_args = mesh_write->file->n_format_args;
      mesh_write->file_pvd->arg = mesh_write->file->arg;
      feenox_check_alloc(mesh_write->file_pvd->mode = strdup("w"));
      
      // change extension to pvd
      char *dot = NULL;
      if ((dot = strrchr(mesh_write->file_pvd->format, '.')) == NULL) {
        feenox_push_error_message("output path '%s' needs an extension", mesh_write->file->format);
        return FEENOX_ERROR;
      }
      dot[1] = 'p';
      dot[2] = 'v';
      dot[3] = 'd';
      
      feenox_call(feenox_instruction_file_open(mesh_write->file_pvd));
      
      fprintf(mesh_write->file_pvd->pointer, "<VTKFile type=\"Collection\" version=\"1.0\">\n");
      fprintf(mesh_write->file_pvd->pointer, " <Collection>\n");
    }  
      
      
    // in other formats we have to write one file per time step  
    if (mesh_write->file->pointer != NULL) {
      feenox_call(feenox_instruction_file_close(mesh_write->file));
    }
    
    char new_file_format[strlen(mesh_write->file->format) + 32];
    char *ext = mesh_write->file->format + mesh_write->base_extension_offset;
    if (mesh_write->base_extension_offset == 0) {
      if ((ext = strrchr(mesh_write->file->format, '.')) == NULL) {
        feenox_push_error_message("output path '%s' needs an extension", mesh_write->file->format);
        return FEENOX_ERROR;
      }
      mesh_write->base_extension_offset = ext - mesh_write->file->format;
      mesh_write->base_extension = strdup(ext+1);
    }
    *ext = '\0';
    
    strcpy(new_file_format, mesh_write->file->format);
    char string_step[32];
    snprintf(string_step, 32, "-%g", feenox_special_var_value(step_transient));
    strcat(new_file_format, string_step);
    int n = strlen(new_file_format);
    new_file_format[n] = '.';
    new_file_format[n+1] = '\0';
    strcat(new_file_format, mesh_write->base_extension);
    
    feenox_free(mesh_write->file->format);
    mesh_write->file->format = strdup(new_file_format);
  }

  if (mesh_write->file->pointer == NULL) {
    feenox_call(feenox_instruction_file_open(mesh_write->file));
  }

  // this makes sense only for gmsh
  // THINK!
  if (ftell(mesh_write->file->pointer) == 0) {
    feenox_call(mesh_write->write_header(mesh_write->mesh, mesh_write->file->pointer));
    if (mesh_write->no_mesh == 0) {
      feenox_call(mesh_write->write_mesh(mesh_write->mesh, mesh_write->file->pointer, mesh_write->no_physical_names));
    }
    mesh_write->point_init = 0;
  }

  mesh_write_dist_t *mesh_write_dist = NULL;
  LL_FOREACH(mesh_write->mesh_write_dists, mesh_write_dist) {
    if (mesh_write_dist->field_location == field_location_cells && mesh_write->mesh->n_cells == 0) {
      feenox_call(feenox_mesh_element2cell(mesh_write->mesh));
    }

    if (mesh_write->write_data) {
      feenox_call(mesh_write->write_data(mesh_write, mesh_write_dist));
    }
  }
  
  if (mesh_write->write_footer) {
    feenox_call(mesh_write->write_footer(mesh_write));
  }

  if (mesh_write->post_format == post_format_vtu || mesh_write->post_format == post_format_vtk) {
    feenox_call(feenox_instruction_file_close(mesh_write->file));
  }

  if (mesh_write->file_pvd != NULL && mesh_write->file_pvd->pointer != NULL) {  
    fprintf(mesh_write->file_pvd->pointer, "  <DataSet timestep=\"%g\" file=\"%s\"/>\n", feenox_special_var_value(t), mesh_write->file->path);
 
    // TODO: maybe this should go in the cleanup routine so it is called when trapping a sigterm
    if (feenox_special_var_value(done) != 0) {
      fprintf(mesh_write->file_pvd->pointer, " </Collection>\n");
      fprintf(mesh_write->file_pvd->pointer, "</VTKFile>\n");
    }
  }
  
  
  // for .msh we leave that to the user, to use CLOSE or whatever explicitly

  return FEENOX_OK;
}
