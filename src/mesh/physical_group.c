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


// get a pointer to a physical group
physical_group_t *feenox_get_physical_group_ptr(const char *name, mesh_t *mesh) {
  physical_group_t *physical_group;
  mesh_t *dummy;
  mesh_t *tmp;
  if (mesh != NULL) {
    HASH_FIND_STR(mesh->physical_groups, name, physical_group);
  } else {
    // barremos todas las mallas
    HASH_ITER(hh, feenox.mesh.meshes, dummy, tmp) {
      HASH_FIND_STR(dummy->physical_groups, name, physical_group);
      if (physical_group != NULL) {
        return physical_group;
      }
    } 
  }
  return physical_group;
}

int feenox_define_physical_group(const char *name, const char *mesh_name, int dimension, int tag) {
  return (feenox_define_physical_group_get_ptr(name, feenox_get_mesh_ptr(mesh_name), dimension, tag) != NULL) ? FEENOX_OK : FEENOX_ERROR;
}


physical_group_t *feenox_define_physical_group_get_ptr(const char *name, mesh_t *mesh, int dimension, int tag) {
  char *dummy_aux = NULL;
  physical_group_t *physical_group;
  mesh_t *actual_mesh; 
  
  if (name == NULL || strcmp(name, "") == 0) {
    feenox_push_error_message("mandatory name needed for physical group");
    return NULL;
  }
  if ((actual_mesh = mesh) == NULL) {
    if ((actual_mesh = feenox.mesh.mesh_main) == NULL) {
      feenox_push_error_message("no mesh for defining physical group '%s'", name);
      return NULL;  
    }
  }
  
  int already_exists = 0;
  if ((physical_group = feenox_get_physical_group_ptr(name, actual_mesh)) == NULL) {
    already_exists = 0; // this is used to define special variables below
    feenox_check_alloc_null(physical_group = calloc(1, sizeof(physical_group_t)));
    feenox_check_alloc_null(physical_group->name = strdup(name));
    HASH_ADD_KEYPTR(hh, mesh->physical_groups, physical_group->name, strlen(name), physical_group);
  } else {
    already_exists = 1;
  }
  
  if (tag != 0) {
    if (physical_group->tag != 0) {
      if (physical_group->tag != tag) {
        feenox_push_error_message("physical group '%s' had been previously defined using id '%d' and now id '%d' is required", name, physical_group->tag, tag);
        return NULL;
      }
    }
    physical_group->tag = tag;
  }

  if (dimension != 0) {
    if (physical_group->dimension != 0) {
      if (physical_group->dimension != dimension) {
        feenox_push_error_message("physical group '%s' had been previously defined as dimension '%d' and now dimension '%d' is required", name, physical_group->dimension, dimension);
        return NULL;
      }
    }  
    physical_group->dimension = dimension;
  }
  
  // -----------------------------  
  if (already_exists == 0) {
    if (feenox_check_name(name) == FEENOX_OK) {
      // volume (or area or length)
      feenox_check_minusone_null(asprintf(&dummy_aux, "%s_vol", physical_group->name));
      // check again in case there are duplicates in meshes
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
    feenox_free(dummy_aux);

    // center of gravity
    feenox_check_minusone_null(asprintf(&dummy_aux, "%s_cog", physical_group->name));
    if (feenox_check_name(dummy_aux) == FEENOX_OK) {
      if ((physical_group->vector_cog = feenox_define_vector_get_ptr(dummy_aux, 3)) == NULL) {
        return NULL;
      }
    } else {
      feenox_pop_error_message();
    }  
    feenox_free(dummy_aux);

  } 
  return physical_group;
}
