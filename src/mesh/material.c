/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's materials
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
#define _GNU_SOURCE  // for asprintf
#include "../feenox.h"
extern feenox_t feenox;

// get a pointer to a material
material_t *feenox_get_material_ptr(const char *name) {
  material_t *material = NULL;
  HASH_FIND_STR(feenox.mesh.materials, name, material);
  return material;
}


int feenox_define_material(const char *material_name, const char *mesh_name) {
  
  mesh_t *mesh = NULL;
  if (mesh_name == NULL || strcmp(mesh_name, "") == 0) {
    if ((mesh = feenox_get_mesh_ptr(mesh_name)) == NULL) {
      return FEENOX_ERROR;
    }  
  } else {
    if (feenox.mesh.mesh_main == NULL) {
      feenox_push_error_message("define at least one mesh before a material");
      return FEENOX_ERROR;
    }  
  }      
  
  return (feenox_define_material_get_ptr(material_name, mesh) != NULL) ? FEENOX_OK : FEENOX_ERROR;
}

material_t *feenox_define_material_get_ptr(const char *name, mesh_t *mesh) {

  material_t *material = NULL;
  if ((material = feenox_get_material_ptr(name)) == NULL) {
    
    // create a new material
    feenox_check_alloc_null(material = calloc(1, sizeof(material_t)));
    feenox_check_alloc_null(material->name = strdup(name));
    
    if ((material->mesh = mesh) == NULL) {
      if ((material->mesh = feenox.mesh.mesh_main) == NULL) {
        feenox_push_error_message("physical properties can be given only after at least giving one mesh");
        return NULL;  
      }
    }
    
    HASH_ADD_STR(feenox.mesh.materials, name, material);
    
  } else {
    if (mesh != NULL && material->mesh != mesh) {
      feenox_push_error_message("material '%s' already defined over mesh '%s' and re-defined over '%s'", material->name, material->mesh->file->name, mesh->file->name);
      return NULL;
    }
  }

  return material;
}

int feenox_define_property(const char *name, const char *mesh_name) {
  return (feenox_define_property_get_ptr(name, feenox_get_mesh_ptr(mesh_name)) != NULL) ? FEENOX_OK : FEENOX_ERROR;
}


property_t *feenox_define_property_get_ptr(const char *name, mesh_t *mesh) {

  mesh_t *actual_mesh;
  if ((actual_mesh = mesh) == NULL) {
    if ((actual_mesh = feenox.mesh.mesh_main) == NULL) {
      feenox_push_error_message("physical properties can be given only after at least giving one mesh");
      return NULL;  
    }
  }

  property_t *property = NULL;
  HASH_FIND_STR(feenox.mesh.properties, name, property);
  if (property == NULL) {
    feenox_check_alloc_null(property = calloc(1, sizeof(property_t)));
    feenox_check_alloc_null(property->name = strdup(name));
    HASH_ADD_KEYPTR(hh, feenox.mesh.properties, property->name, strlen(property->name), property);
  }
  
  // besides the property, we define a function with the property's name
  // that depends on x,y,z (or the dimension of the mesh)
  function_t *function = feenox_get_function_ptr(name);
  if (function != NULL) {
    if (function->n_arguments != actual_mesh->dim) {
      feenox_push_error_message("there already exists a function named '%s' and it has %d arguments instead of %d", name, function->n_arguments, actual_mesh->dim);
      return NULL;  
    }
    if (function->type != function_type_pointwise_mesh_property) {
      feenox_push_error_message("there already exists a function named '%s' and it is not related to a property");
      return NULL;  
    }
    if (function->mesh != actual_mesh) {
      feenox_push_error_message("there already exists a function named '%s' and it is not over the mesh '%s'", actual_mesh->file->name);
      return NULL;  
    }
  } else {
    if ((function = feenox_define_function_get_ptr(name, actual_mesh->dim)) == NULL) {
      return NULL;
    }  
    
    // here the mesh is that of the first property... is that right?
    function->mesh = actual_mesh;
    function->type = function_type_pointwise_mesh_property;
    function->property = property;
    
  }  


  return property;
}

int feenox_define_property_data(const char *property_name, const char *material_name, const char *expr_string) {

  material_t *material = NULL;
  // sometimes it is no ok to re-define and re-use materials (here it is)
  if ((material = feenox_get_material_ptr(material_name)) == NULL) {
    if ((material = feenox_define_material_get_ptr(material_name, NULL)) == NULL) {
      return FEENOX_ERROR;
    }
  }
  // it is ok to re-define and re-use properties
  property_t *property = NULL;
  if ((property = feenox_define_property_get_ptr(property_name, material->mesh)) == NULL) {
    return FEENOX_ERROR;
  }
  return (feenox_define_property_data_get_ptr(property, material, expr_string) != NULL) ? FEENOX_OK : FEENOX_ERROR; 
}

property_data_t *feenox_define_property_data_get_ptr(property_t *property, material_t *material, const char *expr_string) {
  // there's a function called material_property with the algebraic
  // expression that depends on x,y,z
  // if we need to fail, fail befor allocating
  if (property == NULL || material == NULL || material->mesh == NULL) {
    feenox_push_error_message("something is null when calling feenox_define_property_data_get_ptr()");
    return NULL;
  }
  if (material->mesh->dim == 0) {
    feenox_push_error_message("mesh '%s' has zero dimensions when defining property '%s', keyword DIMENSIONS is needed for MESH definition", material->mesh->file->format, property->name);
    return NULL;
  }

  property_data_t *property_data = NULL;
  feenox_check_alloc_null(property_data = calloc(1, sizeof(property_data_t)));
  property_data->material = material;
  property_data->property = property;
  if (feenox_expression_parse(&property_data->expr, expr_string) != FEENOX_OK) {
    return NULL;
  }

  char *name = NULL; 
  if (asprintf(&name, "%s_%s", material->name, property->name) == -1) {
    return NULL;
  }
  
  function_t *function;
  if ((function = feenox_define_function_get_ptr(name, material->mesh->dim)) == NULL) {
    return NULL;
  }
  
  function->type = function_type_algebraic;
  function->var_argument = feenox.mesh.vars.arr_x;
  function->algebraic_expression = property_data->expr;
  feenox_free(name);
  
  HASH_ADD_KEYPTR(hh, material->property_datums, property->name, strlen(property->name), property_data);
//  HASH_ADD_KEYPTR(hh, property->property_datums, material->name, strlen(material->name), property_data);

  return property_data;
}
