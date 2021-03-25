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
    free(dummy_aux);

    // center of gravity
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



material_t *feenox_define_material_get_ptr(const char *name) {

  material_t *material;

  HASH_FIND_STR(feenox.mesh.materials, name, material);
  if (material != NULL) {
    feenox_push_error_message("there already exists a material named '%s'", name);
    return NULL;
  }

  material = calloc(1, sizeof(material_t));
  material->name = strdup(name);

  HASH_ADD_KEYPTR(hh, feenox.mesh.materials, material->name, strlen(material->name), material);

  return material;
}

int feenox_define_physical_property(const char *name, const char *mesh_name) {
  return (feenox_define_physical_property_get_ptr(name, feenox_get_mesh_ptr(mesh_name)) != NULL) ? FEENOX_OK : FEENOX_ERROR;
}


physical_property_t *feenox_define_physical_property_get_ptr(const char *name, mesh_t *mesh) {

  physical_property_t *property;
  function_t *function;
  mesh_t *actual_mesh;
    
  if ((actual_mesh = mesh) == NULL) {
    if ((actual_mesh = feenox.mesh.mesh_main) == NULL) {
      feenox_push_error_message("physical properties can be given only after at least giving one mesh");
      return NULL;  
    }
  }

  HASH_FIND_STR(feenox.mesh.physical_properties, name, property);
  if (property != NULL) {
    feenox_push_error_message("there already exists a property named '%s'", name);
    return NULL;
  }

  property = calloc(1, sizeof(physical_property_t));
  property->name = strdup(name);
  HASH_ADD_KEYPTR(hh, feenox.mesh.physical_properties, property->name, strlen(property->name), property);

  // besides the property, we define a function with the property's name
  // that depends on x,y,z (or the dimension of the mesh)
  if ((function = feenox_define_function_get_ptr(name, actual_mesh->dim)) == NULL) {
    return NULL;
  }

  function->mesh = mesh;
  function->type = function_type_pointwise_mesh_property;
  function->property = property;
  

  return property;
}

int feenox_define_property_data(const char *material_name, const char *property_name, const char *expr_string) {
 return (feenox_define_property_data_get_ptr(material_name, property_name, expr_string) != NULL) ? FEENOX_OK : FEENOX_ERROR; 
}

property_data_t *feenox_define_property_data_get_ptr(const char *material_name, const char *property_name, const char *expr_string) {

  property_data_t *property_data;
  material_t *material;
  physical_property_t *property;
  function_t *function;
  char *name;

  HASH_FIND_STR(feenox.mesh.materials, material_name, material);
  if (material == NULL) {
    material = feenox_define_material_get_ptr(material_name);
  }
  HASH_FIND_STR(feenox.mesh.physical_properties, property_name, property);
  if (property == NULL) {
    if ((property = feenox_define_physical_property_get_ptr(property_name, material->mesh)) == NULL) {
      return NULL;
    }
  }

  property_data = calloc(1, sizeof(property_data_t));
  property_data->material = material;
  property_data->property = property;
  if (feenox_expression_parse(&property_data->expr, expr_string) != FEENOX_OK) {
    return NULL;
  }

  // there's a function called material_property with the algebraic
  // expression that depends on x,y,z
  if (material->mesh->dim == 0) {
    feenox_push_error_message("mesh '%s' has zero dimensions when defining property '%s', keyword DIMENSIONS is needed for MESH definition", material->mesh->file->format, property->name);
    return NULL;
  }
  name = malloc(strlen(material->name)+strlen(property->name)+8);
  sprintf(name, "%s_%s", material->name, property->name);
  if ((function = feenox_define_function_get_ptr(name, material->mesh->dim)) == NULL) {
    return NULL;
  }
  function->type = function_type_algebraic;
  function->var_argument = feenox.mesh.vars.arr_x;
  function->algebraic_expression = property_data->expr;
  free(name);
  
  HASH_ADD_KEYPTR(hh, material->property_datums, property->name, strlen(property->name), property_data);
//  HASH_ADD_KEYPTR(hh, property->property_datums, material->name, strlen(material->name), property_data);

  return property_data;
}
