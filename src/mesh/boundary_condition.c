/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's boundary conditions
 *
 *  Copyright (C) 2021 jeremy theler
 *
 *  This file is part of FeenoX.
 *
 *  FeenoX is free software: you can redistribute it and/or modify
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
 *  along with FeenoX.  If not, see <http://www.gnu.org/licenses/>.
 *------------------- ------------  ----    --------  --     -       -         -
 */
#include "../feenox.h"
extern feenox_t feenox;

int feenox_define_bc(const char *name) {
  return (feenox_define_bc_get_ptr(name) != NULL) ? FEENOX_OK : FEENOX_ERROR;
}

bc_t *feenox_define_bc_get_ptr(const char *name) {

  bc_t *bc = NULL;
  HASH_FIND_STR(feenox.mesh.bcs, name, bc);
  if (bc == NULL) {
    bc = calloc(1, sizeof(bc_t));
    bc->name = strdup(name);
    HASH_ADD_KEYPTR(hh, feenox.mesh.bcs, bc->name, strlen(bc->name), bc);
  }

  return bc;
}

int feenox_add_bc_data(const char *bc_name, const char *string) {
  return (feenox_add_bc_data_get_ptr(feenox_define_bc_get_ptr(bc_name), string) != NULL) ? FEENOX_OK : FEENOX_ERROR; 
}

bc_data_t *feenox_add_bc_data_get_ptr(bc_t *bc, const char *string) {

  bc_data_t *bc_data;
  feenox_check_alloc_null(bc_data = calloc(1, sizeof(bc_data_t)));
  feenox_check_alloc_null(bc_data->string = strdup(string));
  
  // TODO: do something with the string
  LL_APPEND(bc->bc_datums, bc_data);

  return bc_data;
}
