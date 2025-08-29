/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's mesh-related vtu routines
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
 * 
 * Portions of this file were developed with the assistance of GitHub Copilot.
 * 
 *------------------- ------------  ----    --------  --     -       -         -
 */
#include "../feenox.h"


int feenox_mesh_write_header_vtu(mesh_t *mesh, FILE *fp) {
  fprintf(fp, "<?xml version=\"1.0\"?>\n");
  fprintf(fp, "<VTKFile type=\"UnstructuredGrid\" version=\"1.0\">\n");
  fprintf(fp, "  <UnstructuredGrid>\n");
  if (mesh->n_cells == 0) {
    feenox_call(feenox_mesh_element2cell(mesh));
  }
  fprintf(fp, "    <Piece NumberOfPoints=\"%ld\" NumberOfCells=\"%ld\">\n", mesh->n_nodes, mesh->n_cells);

  return FEENOX_OK;
}

int feenox_mesh_write_footer_vtu(mesh_write_t *this) {
  if (this->point_init == 1) {
    fprintf(this->file->pointer, "      </PointData>\n");
  } else {
    fprintf(this->file->pointer, "      </CellData>\n");
  } 
  fprintf(this->file->pointer, "    </Piece>\n");
  fprintf(this->file->pointer, "  </UnstructuredGrid>\n");
  fprintf(this->file->pointer, "</VTKFile>\n");

  return FEENOX_OK;
}

int feenox_mesh_write_mesh_vtu(mesh_t *mesh, FILE *file, int dummy) {

/*
  if (mesh->sparse && mesh->tag2index == NULL) {
    feenox_call(feenox_mesh_create_index2tag(mesh));
  }
*/  
  // Points
  fprintf(file, "      <Points>\n");
  fprintf(file, "        <DataArray Name=\"Points\" type=\"Float64\" NumberOfComponents=\"3\" format=\"ascii\">\n");
  for (size_t j = 0; j < mesh->n_nodes; j++) {
    for (int d = 0; d < 3; d++) {
      fprintf(file, " %.6g", mesh->node[j].x[d]);
    }
    fprintf(file, "\n");
  }
  fprintf(file, "        </DataArray>\n");
  fprintf(file, "      </Points>\n");
    
  // Cells
  fprintf(file, "      <Cells>\n");
  fprintf(file, "        <DataArray Name=\"connectivity\" type=\"Int32\" format=\"ascii\">\n");
  // the zero means "do not write the number of nodes"
  feenox_mesh_write_vtk_cells(mesh, file, 0);
  fprintf(file, "        </DataArray>\n");
 
  fprintf(file, "        <DataArray Name=\"offsets\" type=\"Int32\" format=\"ascii\">\n");
  size_t current_offset = 0;
  for (size_t i = 0; i < mesh->n_elements; i++) {
    if (mesh->element[i].type->dim == mesh->dim_topo) {
      current_offset += mesh->element[i].type->nodes;
      fprintf(file, "%ld\n", current_offset);
    }
  }
  fprintf(file, "        </DataArray>\n");
  
  fprintf(file, "        <DataArray Name=\"types\" type=\"Int32\" format=\"ascii\">\n");
  feenox_mesh_write_vtk_types(mesh, file);
  fprintf(file, "        </DataArray>\n");
  
//    write_data_array_ascii(fp, "types", "UInt8", types, mesh->n_elements, 1);
    
  fprintf(file, "      </Cells>\n");
  
  return FEENOX_OK;
}


int feenox_mesh_write_data_vtu(mesh_write_t *this, mesh_write_dist_t *dist) {

  if (dist->field_location == field_location_cells) {
    if (this->cell_init == 0) {
      if (this->point_init == 1) {
        fprintf(this->file->pointer, "      </PointData>\n");
      }
      fprintf(this->file->pointer, "      <CellData>\n");
      this->cell_init = 1;
      this->point_init = 0;
    }
  } else {
    if (this->point_init == 0) {
      if (this->cell_init == 1) {
        fprintf(this->file->pointer, "      </CellData>\n");
      }
      fprintf(this->file->pointer, "      <PointData>\n");
      this->point_init = 1;
      this->cell_init = 0;
    }
  }
      
  // custom printf format
  char *format = NULL;
  if (dist->printf_format == NULL) {
    feenox_check_alloc(format = strdup(" %g"));
  } else {
    if (dist->printf_format[0] == '%') {
      feenox_check_minusone(asprintf(&format, " %s", dist->printf_format));
    } else {
      feenox_check_minusone(asprintf(&format, " %%%s", dist->printf_format));
    }  
  }

  fprintf(this->file->pointer, "        <DataArray Name=\"%s\" type=\"Float64\" NumberOfComponents=\"%d\" format=\"ascii\">\n", dist->name, dist->size);  
  
  // TODO: virtual methods
  if (dist->field_location == field_location_cells) {

    for (size_t i = 0; i < this->mesh->n_cells; i++) {
      for (int g = 0; g < dist->size; g++) {
        // TODO: remove the conditional from the loop
        double value = (dist->field[g]->type == function_type_pointwise_mesh_cell && dist->field[g]->mesh == this->mesh) ?
                  feenox_vector_get(dist->field[g]->vector_value, i) :
                  feenox_function_eval(dist->field[g], this->mesh->cell[i].x);
        fprintf(this->file->pointer, format, value);
      }
      fprintf(this->file->pointer, "\n");
    }
    
  } else {
    
    for (size_t j = 0; j < this->mesh->n_nodes; j++) {
      for (int g = 0; g < dist->size; g++) {
        // TODO: remove the conditional from the loop
        double value = (dist->field[g]->type == function_type_pointwise_mesh_node && dist->field[g]->mesh == this->mesh) ?
                  feenox_vector_get(dist->field[g]->vector_value, j) :
                  feenox_function_eval(dist->field[g], this->mesh->node[j].x);
        fprintf(this->file->pointer, format, value);
      }
      fprintf(this->file->pointer, "\n");
    }
  }  
  fprintf(this->file->pointer, "        </DataArray>\n");  
 
  fflush(this->file->pointer);
  feenox_free(format);
  
  return FEENOX_OK;
  
}
