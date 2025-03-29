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
 *------------------- ------------  ----    --------  --     -       -         -
 */
#include "../feenox.h"


int feenox_mesh_write_header_vtu(mesh_t *mesh, FILE *fp) {
  fprintf(fp, "<?xml version=\"1.0\"?>\n");
  fprintf(fp, "<VTKFile type=\"UnstructuredGrid\" version=\"1.0\" byte_order=\"LittleEndian\">\n");
  fprintf(fp, "  <UnstructuredGrid>\n");
  fprintf(fp, "    <Piece NumberOfPoints=\"%ld\" NumberOfCells=\"%ld\">\n", mesh->n_nodes, mesh->n_cells);

  return FEENOX_OK;
}

int feenox_mesh_write_footer_vtu(FILE *fp) {
  fprintf(fp, "    </Piece>\n");
  fprintf(fp, "  </UnstructuredGrid>\n");
  fprintf(fp, "</VTKFile>\n");

  return FEENOX_OK;
}

// Write mesh geometry and topology
int feenox_mesh_write_mesh_vtu(mesh_t *mesh, FILE *file, int dummy) {
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