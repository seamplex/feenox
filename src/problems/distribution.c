/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's approach to handling distributions of properties
 *
 *  Copyright (C) 2015-2016 jeremy theler
 *
 *  This file is part of Fino <https://www.seamplex.com/feenox>.
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
#include "feenox.h"
extern feenox_t feenox;

int feenox_distribution_init(distribution_t *this, const char *name) {

  // first try a property
  HASH_FIND_STR(feenox.mesh.properties, name, this->property);
  if (this->property != NULL) {
    this->defined = 1;
    return FEENOX_OK;
  }

  // TODO: try one function for each material
  
  // try a single function
  if ((this->function = feenox_get_function_ptr(name)) != NULL) {
    if (this->function->n_arguments != feenox.pde.dim) {
      feenox_push_error_message("function '%s' should have %d arguments instead of %d to be used as a distribution", this->function->name, feenox.pde.dim, this->function->n_arguments);
      return FEENOX_ERROR;
    }
    this->defined = 1;
    return FEENOX_OK;
  }
  
  // try a variable
  if ((this->variable = feenox_get_variable_ptr(name)) != NULL) {
    this->defined = 1;
    // TODO: set a virtual method to evaluate
    return FEENOX_OK;
  }
  

  // not finding an actual distribution is not an error, there are optional distributions
  // TODO: set a virtual method to return zero
  return FEENOX_OK;
}

// TODO: split in virtual methods
double feenox_distribution_eval(distribution_t *this, const double *x, material_t *material) {
  
  if (this->variable != NULL) {
    return feenox_var_value(this->variable);
    
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
    
  } else if (this->function != NULL) {
    return feenox_function_eval(this->function, x);
    
  }
  
  return 0;
}