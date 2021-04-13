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

// get a pointer to a bc
bc_t *feenox_get_bc_ptr(const char *name) {
  bc_t *bc = NULL;
  HASH_FIND_STR(feenox.mesh.bcs, name, bc);
  return bc;
}

int feenox_define_bc(const char *bc_name, const char *mesh_name) {
  
  mesh_t *mesh = NULL;
  if (mesh_name == NULL || strcmp(mesh_name, "") == 0) {
    if ((mesh = feenox_get_mesh_ptr(mesh_name)) == NULL) {
      return FEENOX_ERROR;
    }  
  } else {
    if (feenox.mesh.mesh_main == NULL) {
      feenox_push_error_message("define at least one mesh before a BC");
      return FEENOX_ERROR;
    }  
  }      
  
  return (feenox_define_bc_get_ptr(bc_name, mesh) != NULL) ? FEENOX_OK : FEENOX_ERROR;
}

bc_t *feenox_define_bc_get_ptr(const char *name, mesh_t *mesh) {

  bc_t *bc = NULL;
  if ((bc = feenox_get_bc_ptr(name)) == NULL) {
    
    // create a new bc
    feenox_check_alloc_null(bc = calloc(1, sizeof(material_t)));
    feenox_check_alloc_null(bc->name = strdup(name));
    
    if ((bc->mesh = mesh) == NULL) {
      if ((bc->mesh = feenox.mesh.mesh_main) == NULL) {
        feenox_push_error_message("boundary conditions can be given only after at least giving one mesh");
        return NULL;  
      }
    }
    
    HASH_ADD_STR(feenox.mesh.bcs, name, bc);
    
  } else {
    if (mesh != NULL && bc->mesh != mesh) {
      feenox_push_error_message("BC '%s' already defined over mesh '%s' and re-defined over '%s'", bc->name, bc->mesh->file->name, mesh->file->name);
      return NULL;
    }
  }

  return bc;
}


int feenox_add_bc_data(const char *bc_name, const char *string) {
  
  bc_t *bc = NULL;
  if ((bc = feenox_get_bc_ptr(bc_name)) == NULL) {
    if ((bc = feenox_define_bc_get_ptr(bc_name, NULL)) == NULL) {
      return FEENOX_ERROR;
    }
  }
  
  return (feenox_add_bc_data_get_ptr(bc, string) != NULL) ? FEENOX_OK : FEENOX_ERROR; 
}

bc_data_t *feenox_add_bc_data_get_ptr(bc_t *bc, const char *string) {

  bc_data_t *bc_data;
  feenox_check_alloc_null(bc_data = calloc(1, sizeof(bc_data_t)));
  feenox_check_alloc_null(bc_data->string = strdup(string));
  
  
  // see if there is a "=>" that implies that the BC has a condition
  char *lhs = NULL;
  if ((lhs = strstr(bc_data->string, "=>")) != NULL) {
    *lhs = '\0';
    lhs += 2;
    feenox_call_null(feenox_expression_parse(&bc_data->condition, bc_data->string));
  } else {
    lhs = bc_data->string;
  }
      
  // if there is an equal sign there is an expression, otherwise not
  char *rhs = NULL;
  char *equal_sign = NULL;
  if ((equal_sign = strchr(lhs, '=')) != NULL) {
    *equal_sign = '\0';
    rhs = equal_sign+1;
  }
  
  // we now call the problem-specific function that understands
  // what the string means and fill in the other bc_data fields
  // TODO: virtual
  feenox_call_null(feenox_problem_bc_parse_thermal(bc_data, lhs, rhs));
  
  LL_APPEND(bc->bc_datums, bc_data);

  return bc_data;
}
