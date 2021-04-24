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
#include "feenox.h"
extern feenox_t feenox;

int feenox_distribution_init(distribution_t *this, const char *name) {

  feenox_check_alloc(this->name = strdup(name));
  
  // first try a property, if this is the case then we have it easy
  HASH_FIND_STR(feenox.mesh.properties, name, this->property);
  if (this->property != NULL) {
    // check if the property is "full," i.e. defined over all volumes
    int full = 1;
    physical_group_t *physical_group = NULL;
    physical_group_t *tmp_group = NULL;
    HASH_ITER(hh, feenox.pde.mesh->physical_groups, physical_group, tmp_group) {
      if (physical_group->dimension == feenox.pde.dim) {
        if (physical_group->material != NULL) {
          property_data_t *property_data = NULL;
          HASH_FIND_STR(physical_group->material->property_datums, this->property->name, property_data);
          if (property_data != NULL) {
            feenox_pull_dependencies_variables(&this->dependency_variables, property_data->expr.variables);
            feenox_pull_dependencies_functions(&this->dependency_functions, property_data->expr.functions);
          } else  {
            full = 0;
          }  
        } else {
          full = 0;
        }
      }
    }

    this->defined = 1;
    this->full = full;
    this->eval = feenox_distribution_eval_property;
    
    return FEENOX_OK;
  }

  // try one function for each volume
  int full = 1;
  physical_group_t *physical_group = NULL;
  physical_group_t *tmp_group = NULL;
  HASH_ITER(hh, feenox.pde.mesh->physical_groups, physical_group, tmp_group) {
    if (physical_group->dimension == feenox.pde.dim) {
      char *function_name = NULL;
      asprintf(&function_name, "%s_%s", name, physical_group->name);
      function_t *function = NULL;
      if ((function = feenox_get_function_ptr(function_name)) != NULL) {
        if (function->n_arguments != feenox.pde.dim) {
          feenox_push_error_message("function '%s' should have %d arguments instead of %d to be used as a distribution", function->name, feenox.pde.dim, function->n_arguments);
          return FEENOX_ERROR;
        }
        
        if (this->function == NULL) {
          this->function = function;
        }
        
        // if there's no explicit material we create one
        if (physical_group->material == NULL) {
          physical_group->material = feenox_define_material_get_ptr(physical_group->name, feenox.pde.mesh);
        }  
        
        feenox_call(feenox_pull_dependencies_variables_function(&this->dependency_variables, function));
        feenox_call(feenox_pull_dependencies_functions_function(&this->dependency_functions, function));

      } else {
        full = 0;
      }
      feenox_free(function_name);
    }  
  }
  
  if (this->function) {
    this->defined = 1;
    this->full = full;
    this->eval = feenox_distribution_eval_function_local;
    return FEENOX_OK;
  }
  
  
  // try a single function
  if ((this->function = feenox_get_function_ptr(name)) != NULL) {
    if (this->function->n_arguments != feenox.pde.dim) {
      feenox_push_error_message("function '%s' should have %d arguments instead of %d to be used as a distribution", this->function->name, feenox.pde.dim, this->function->n_arguments);
      return FEENOX_ERROR;
    }
    
    this->defined = 1;
    this->full = 1;
    this->eval = feenox_distribution_eval_function_global;
    feenox_call(feenox_pull_dependencies_variables_function(&this->dependency_variables, this->function));
    feenox_call(feenox_pull_dependencies_functions_function(&this->dependency_functions, this->function));
    
    return FEENOX_OK;
  }
  
  // try one variable for each volume
  full = 1;
  physical_group = NULL;
  tmp_group = NULL;
  HASH_ITER(hh, feenox.pde.mesh->physical_groups, physical_group, tmp_group) {
    if (physical_group->dimension == feenox.pde.dim) {
      char *var_name = NULL;
      asprintf(&var_name, "%s_%s", name, physical_group->name);
      var_t *variable = NULL;
      if ((variable = feenox_get_variable_ptr(var_name)) != NULL) {
        if (this->variable == NULL) {
          this->variable = variable;
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
  
  if (this->variable) {
    this->defined = 1;
    this->full = full;
    this->eval = feenox_distribution_eval_variable_local;
    return FEENOX_OK;
  }
  
  
  // try a single global variable
  if ((this->variable = feenox_get_variable_ptr(name)) != NULL) {
    this->defined = 1;
    this->full = 1;
    this->eval = feenox_distribution_eval_variable_global;
    return FEENOX_OK;
  }
  

  // not finding an actual distribution is not an error, there are optional distributions
  this->defined = 0;
  this->full = 0;
  this->eval = feenox_distribution_eval;
  return FEENOX_OK;
}

// default virtual method for undefined distributions
double feenox_distribution_eval(distribution_t *this, const double *x, material_t *material) {
  return 0;
}

double feenox_distribution_eval_variable_global(distribution_t *this, const double *x, material_t *material) {
  return feenox_var_value(this->variable);
}

double feenox_distribution_eval_function_global(distribution_t *this, const double *x, material_t *material) {
  return feenox_function_eval(this->function, x);
}

double feenox_distribution_eval_variable_local(distribution_t *this, const double *x, material_t *material) {
  
  if (material == NULL) {
    return 0;
  }
  
  if (material != this->last_material) {
    char *var_name = NULL;
    asprintf(&var_name, "%s_%s", this->name, material->name);
    if ((this->variable = feenox_get_variable_ptr(var_name)) == NULL) {
      feenox_push_error_message("cannot find variable '%s'", var_name);
      feenox_runtime_error();
    }
    this->last_material = material;
    feenox_free(var_name);
  }
  
  return feenox_var_value(this->variable);      
}


double feenox_distribution_eval_function_local(distribution_t *this, const double *x, material_t *material) {
  
  if (material == NULL) {
    return 0;
  }
  
  if (material != this->last_material) {
    char *function_name = NULL;
    asprintf(&function_name, "%s_%s", this->name, material->name);
    if ((this->function = feenox_get_function_ptr(function_name)) == NULL) {
      feenox_push_error_message("cannot find function '%s'", function_name);
      feenox_runtime_error();
    }
    this->last_material = material;
    feenox_free(function_name);
  }
  
  return feenox_function_eval(this->function, x);      
}


double feenox_distribution_eval_property(distribution_t *this, const double *x, material_t *material) {

  if (material == NULL) {
    return 0;
  }
  
  if (material != this->last_material) {
    property_data_t *property_data = NULL;
    HASH_FIND_STR(material->property_datums, this->property->name, property_data);
    if (property_data != NULL) {
      this->last_material = material;
      this->last_property_data = property_data;
    }
  }
  
  if (this->last_property_data != NULL) {
    // the property has an expression of x,y & z, it's not a function
    feenox_var_value(feenox.mesh.vars.x) = x[0];
    if (feenox.pde.dim > 1) {
      feenox_var_value(feenox.mesh.vars.y) = x[1];
      if (feenox.pde.dim > 2) {
        feenox_var_value(feenox.mesh.vars.z) = x[2];
      }
    }
    return feenox_expression_eval(&this->last_property_data->expr);
  }
  
  return 0;
  
}  



int feenox_pull_dependencies_variables_function(var_ll_t **to, function_t *function) {

  if (function->algebraic_expression.items != NULL) {
    feenox_pull_dependencies_variables(to, function->algebraic_expression.variables);
  } else {
    // we have to assume it depends on x,y & z
    var_ll_t *x;
    feenox_check_alloc(x = calloc(1, sizeof(var_ll_t)));
    x->var = feenox.mesh.vars.x;
    LL_APPEND(*to, x);

    if (function->n_arguments > 1) {
      var_ll_t *y;
      feenox_check_alloc(y = calloc(1, sizeof(var_ll_t)));
      y->var = feenox.mesh.vars.y;
      LL_APPEND(*to, y);
      
      if (function->n_arguments > 2) {
        var_ll_t *z;
        feenox_check_alloc(z = calloc(1, sizeof(var_ll_t)));
        z->var = feenox.mesh.vars.z;
        LL_APPEND(*to, z);
      }
    }
  }  
  
  return FEENOX_OK;
}

int feenox_pull_dependencies_functions_function(function_ll_t **to, function_t *function) {

  if (function->algebraic_expression.items != NULL) {
    feenox_pull_dependencies_functions(to, function->algebraic_expression.functions);
  }  
  
  return FEENOX_OK;
}