/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's mesh-related cell routines
 *
 *  Copyright (C) 2014--2016 jeremy theler
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

int feenox_mesh_element2cell(mesh_t *this) {
  
  int i_element, i_cell;
  
  if (this->cell != NULL) {
    return FEENOX_OK;
  }
  feenox.mesh.need_cells = 1;
  
  // contamos cuantas celdas hay
  // una celda = un elemento de la dimension del problema
  this->n_cells = 0;
  for (i_element = 0; i_element < this->n_elements; i_element++) {
    if (this->element[i_element].type != NULL && this->element[i_element].type->dim == this->dim) {
      this->n_cells++;
    }
  }
  // alocamos las celdas
  feenox_check_alloc(this->cell = calloc(this->n_cells, sizeof(cell_t)));
  

  i_cell = 0;
  for (i_element = 0; i_element < this->n_elements; i_element++) {
    if (this->element[i_element].type != NULL && this->element[i_element].type->dim == this->dim) {
  
      // las id las empezamos en uno
      this->cell[i_cell].id = i_cell+1;
      
      // elemento (ida y vuelta)
      this->cell[i_cell].element = &this->element[i_element];
      this->element[i_element].cell = &this->cell[i_cell];
    
      // holder para vecinos
      feenox_check_alloc(this->cell[i_cell].ineighbor = calloc(this->element[i_element].type->faces, sizeof(int)));

      // coordenadas del centro
/*      
      for (i_dim = 0; i_dim < this->spatial_dimensions; i_dim++) {
        this->cell[i_cell].x[i_dim] = 0;
        for (i_node = 0; i_node < this->cell[i_cell].element->type->nodes; i_node++) {
          this->cell[i_cell].x[i_dim] += this->cell[i_cell].element->node[i_node]->x[i_dim];
        }
        this->cell[i_cell].x[i_dim] /= (double)(this->cell[i_cell].element->type->nodes);
      }
 */
      feenox_call(feenox_mesh_compute_element_barycenter(this->cell[i_cell].element, this->cell[i_cell].x));
      
      this->cell[i_cell].volume = this->cell[i_cell].element->type->element_volume(this->cell[i_cell].element);      
      
      i_cell++;

    }
  }

  return FEENOX_OK;
  
}


int mesh_compute_coords(mesh_t *this) {

  size_t i = 0;
  for (i = 0; i < this->n_cells; i++) {
    feenox_call(feenox_mesh_compute_element_barycenter(this->cell[i].element, this->cell[i].x));
    this->cell[i].volume = this->cell[i].element->type->element_volume(this->cell[i].element);
  }
  
  return FEENOX_OK;
}


int mesh_cell_indexes(mesh_t *this, int dofs) {
  
  this->degrees_of_freedom = dofs;
  
  size_t i = 0;
  unsigned int g = 0;
  size_t index = 0;
  for (i = 0; i < this->n_cells; i++) {
    // holder para indices y resultados
    feenox_check_alloc(this->cell[i].index = calloc(this->degrees_of_freedom, sizeof(int)));
    for (g = 0; g < this->degrees_of_freedom; g++) {
      this->cell[i].index[g] = index++;
    }
  }
  
  return FEENOX_OK;
}

