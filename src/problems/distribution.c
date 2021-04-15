/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's approach to handling distributions of properties
 *
 *  Copyright (C) 2015-2021 jeremy theler
 *
 *  This file is part of Feenox <https://www.seamplex.com/feenox>.
 *
 *  feenox is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Feenox is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Feenox.  If not, see <http://www.gnu.org/licenses/>.
 *------------------- ------------  ----    --------  --     -       -         -
 */
#define _GNU_SOURCE
#include "feenox.h"
extern feenox_t feenox;

int feenox_distribution_init(distribution_t *this, const char *name) {

  this->name = strdup(name);
  
  // first try a property, if this is the case then we have it easy
  HASH_FIND_STR(feenox.mesh.properties, name, this->property);
  if (this->property != NULL) {
    this->defined = 1;
    
    int full = 1;
    physical_group_t *physical_group = NULL;
    physical_group_t *tmp_group = NULL;
    HASH_ITER(hh, feenox.pde.mesh->physical_groups, physical_group, tmp_group) {
      if (physical_group->dimension == feenox.pde.dim) {
        if (physical_group->material != NULL) {
          property_data_t *property_data = NULL;
          HASH_FIND_STR(physical_group->material->property_datums, this->property->name, property_data);
          if (property_data == NULL) {
            full = 0;
          }  
        } else {
          full = 0;
        }
      }
    }
    this->full = full;
    
    return FEENOX_OK;
  }

  // try a single function
  if ((this->function_single = feenox_get_function_ptr(name)) != NULL) {
    if (this->function_single->n_arguments != feenox.pde.dim) {
      feenox_push_error_message("function '%s' should have %d arguments instead of %d to be used as a distribution", this->function_single->name, feenox.pde.dim, this->function_single->n_arguments);
      return FEENOX_ERROR;
    }
    this->defined = 1;
    return FEENOX_OK;
  }
  
  // try one variable for each group
  
  int full = 1;
  physical_group_t *physical_group = NULL;
  physical_group_t *tmp_group = NULL;
  HASH_ITER(hh, feenox.pde.mesh->physical_groups, physical_group, tmp_group) {
    if (physical_group->dimension == feenox.pde.dim) {
      char *var_name = NULL;
      asprintf(&var_name, "%s_%s", name, physical_group->name);
      var_t *dummy = NULL;
      if ((dummy = feenox_get_variable_ptr(var_name)) != NULL) {
        if (this->variable_last == NULL) {
          this->variable_last = dummy;
        }
        
        // if there's no explicit material we create one
        if (physical_group->material == NULL) {
          physical_group->material = feenox_define_material_get_ptr(physical_group->name, feenox.pde.mesh);
        }  

      } else {
        full = 0;
      }
      feenox_free(var_name);
    }  
  }
  
  if (this->variable_last) {
    this->defined = 1;
    this->full = full;
    return FEENOX_OK;
  }
  
  
  // try a single variable
  if ((this->variable_single = feenox_get_variable_ptr(name)) != NULL) {
    this->defined = 1;
    this->full = 1;
    // TODO: set a virtual method to evaluate
    return FEENOX_OK;
  }
  

  // not finding an actual distribution is not an error, there are optional distributions
  // TODO: set a virtual method to return zero
  return FEENOX_OK;
}

// TODO: split in virtual methods
double feenox_distribution_eval(distribution_t *this, const double *x, material_t *material) {
  
  if (this->variable_single != NULL) {
    return feenox_var_value(this->variable_single);
    
  } else if (this->variable_last != NULL) {
    if (material != NULL) {
      if (material != this->last_material) {
        char *var_name = NULL;
        asprintf(&var_name, "%s_%s", this->name, material->name);
        if ((this->variable_last = feenox_get_variable_ptr(var_name)) == NULL) {
          // TODO: runtime error?
          return 0;
        };
        this->last_material = material;
        feenox_free(var_name);
      }
      return feenox_var_value(this->variable_last);      
    }
    // don't like this... we should find out the material out of the coordinates x
    return 0;
    
  } else if (this->property != NULL) {
    if (material != NULL) {
      property_data_t *property_data = NULL;
      // TODO: improve this! I don't like solving a hash table for each gauss point
      HASH_FIND_STR(material->property_datums, this->property->name, property_data);
      if (property_data != NULL) {
        // the property has an expression of x,y & z, it's not a function
        feenox_var_value(feenox.mesh.vars.x) = x[0];
        if (feenox.pde.dim > 1) {
          feenox_var_value(feenox.mesh.vars.y) = x[1];
          if (feenox.pde.dim > 2) {
            feenox_var_value(feenox.mesh.vars.z) = x[2];
          }
        }
        return feenox_expression_eval(&property_data->expr);
      } else {
        // TODO: do not complain if the distribution is optional, just return zero
        feenox_push_error_message("cannot find property '%s' in material '%s'", this->property->name, material->name);
        feenox_runtime_error();
      }
      
    } else {
      // this is a fallback! even less efficient...
      function_t *function = NULL;
      if ((function = feenox_get_function_ptr(this->property->name)) == NULL) {
        feenox_push_error_message("cannot find neither property nor function '%s'", this->property->name);
        feenox_runtime_error();
      }
    }
    
  } else if (this->function_single != NULL) {
    return feenox_function_eval(this->function_single, x);
    
  }
  
  return 0;
}