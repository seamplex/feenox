/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's physical groups
 *
 *  Copyright (C) 2021 jeremy theler
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
#define _GNU_SOURCE  // for asprintf
#include "../feenox.h"
extern feenox_t feenox;


// TODO: return int here and make another one _get_ptr
physical_group_t *feenox_define_physical_group(mesh_t *new_mesh, const char *name, int dimension) {
  char *dummy_aux = NULL;
  physical_group_t *physical_group;
  mesh_t *mesh; 
  int already_exists = 0;
  
  if (name == NULL) {
    feenox_push_error_message("mandatory name needed for physical group");
    return NULL;
  }
  if ((mesh = new_mesh) == NULL) {
    if ((mesh = feenox.mesh.mesh_main) == NULL) {
      feenox_push_error_message("no mesh for defining physical group '%s'", name);
      return NULL;  
    }
  }
  
  if ((physical_group = feenox_get_physical_group_ptr(mesh, name)) == NULL) {
    already_exists = 0; // esto lo usamos para definir las variables especiales abajo
    physical_group = calloc(1, sizeof(physical_group_t));
    physical_group->name = strdup(name);
    HASH_ADD_KEYPTR(hh, mesh->physical_groups, physical_group->name, strlen(name), physical_group);
  } else {
    already_exists = 1;
  }
  
/*  
  if (physical_group->id != 0 && id != 0 && physical_group->id != id) {
    feenox_push_error_message("physical group '%s' has been previously defined using id '%d' and now id '%d' is required", name, physical_group->id, id);
    return NULL;
  } else if (physical_group->id == 0 && id != 0) {
    physical_group->id = id;
  }
 */
  if (physical_group->dimension != 0 && dimension != 0 && physical_group->dimension != dimension) {
    feenox_push_error_message("physical group '%s' has been previously defined as dimension '%d' and now dimension '%d' is required", name, physical_group->dimension, dimension);
    return NULL;
  } else if (physical_group->dimension == 0 && dimension != 0) {
    physical_group->dimension = dimension;
  }
  
  // -----------------------------  
  if (already_exists == 0) {
    if (feenox_check_name(name) == FEENOX_OK) {
      // volumen (o area o longitud)
      if (asprintf(&dummy_aux, "%s_vol", physical_group->name) == -1) {
        feenox_push_error_message("memory allocation error");
        return NULL;
      }
      // volvemos a chequear por si acaso hay duplicados en mallas 
      if (feenox_check_name(dummy_aux) == FEENOX_OK) {
        if ((physical_group->var_vol = feenox_define_variable_get_ptr(dummy_aux)) == NULL) {
          return NULL;
        }
      } else {
        feenox_pop_error_message();
      }  
    } else {
      feenox_pop_error_message();
    }
    free(dummy_aux);

    // centro de gravedad
    if (asprintf(&dummy_aux, "%s_cog", physical_group->name) == -1) {
      feenox_push_error_message("memory allocation error");
      return NULL;
    }
    if (feenox_check_name(dummy_aux) == FEENOX_OK) {
      if ((physical_group->vector_cog = feenox_define_vector_get_ptr(dummy_aux, 3)) == NULL) {
        return NULL;
      }
    } else {
      feenox_pop_error_message();
    }  
    free(dummy_aux);

  } 
  return physical_group;
}
