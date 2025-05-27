/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's approach to handling distributions of properties
 *
 *  Copyright (C) 2015-2025 Jeremy Theler
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

// this method updates the linked list of variables and functions the distribution
// depends on, but it does not set the non_linear nor space_dependent flags
int feenox_distribution_init(distribution_t *this, const char *name) {

  feenox_check_alloc(this->name = strdup(name));

  if (feenox.pde.mesh->n_groups == 0) {
    feenox.pde.mesh->n_groups = HASH_COUNT(feenox.pde.mesh->physical_groups);
  }
  feenox_check_alloc(this->defined_per_group = calloc(feenox.pde.mesh->n_groups, sizeof(int)));
  
  // first try a property, if this is the case then we have it easy
  int n_volumes = 0;
  int non_uniform = 0;
  HASH_FIND_STR(feenox.mesh.properties, name, this->property);
  if (this->property != NULL) {
    int full = 1; // asume it is full
    physical_group_t *physical_group = NULL;
    int i = 0;
    for (physical_group = feenox.pde.mesh->physical_groups; physical_group != NULL; physical_group = physical_group->hh.next) {
      if (physical_group->dimension == feenox.pde.dim) {
        if (physical_group->material != NULL) {
          property_data_t *property_data = NULL;
          HASH_FIND_STR(physical_group->material->property_datums, this->property->name, property_data);
          if (property_data != NULL) {
            feenox_pull_dependencies_variables(&this->dependency_variables, property_data->expr.variables);
            feenox_pull_dependencies_functions(&this->dependency_functions, property_data->expr.functions);
            non_uniform = (n_volumes++ > 0) || (feenox_depends_on_space(this->dependency_variables));
            this->defined_per_group[i] = 1;            
            this->defined = 1;
          } else  {
            full = 0;
          }  
        } else {
          full = 0;
        }
      }
      i++;
    }
    
    this->full = full;
    this->non_uniform = non_uniform;
    this->eval = feenox_distribution_eval_property;
    this->constant = (feenox_depends_on_time(this->dependency_variables) == 0);
    
    return FEENOX_OK;
  }

  // try one function for each volume
  int full = 1;
  non_uniform = 0;
  n_volumes = 0;
  physical_group_t *physical_group = NULL;
  int i = 0;
  for (physical_group = feenox.pde.mesh->physical_groups; physical_group != NULL; physical_group = physical_group->hh.next) {
    if (physical_group->dimension == feenox.pde.dim) {
      char *function_name = NULL;
      feenox_check_minusone(asprintf(&function_name, "%s_%s", name, physical_group->name));
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
        
        non_uniform = (n_volumes++ > 0) || (feenox_depends_on_space(this->dependency_variables));
        this->defined_per_group[i] = 1;            
        this->defined = 1;

      } else {
        full = 0;
      }
      feenox_free(function_name);
    }
    i++;
  }
  
  if (this->function) {
    this->full = full;
    this->non_uniform = non_uniform;
    this->eval = feenox_distribution_eval_function_local;
    this->constant = (feenox_depends_on_time(this->dependency_variables) == 0);
    return FEENOX_OK;
  }
  
  
  // try a single function
  if ((this->function = feenox_get_function_ptr(name)) != NULL) {
    if (this->function->n_arguments != feenox.pde.dim) {
      feenox_push_error_message("function '%s' should have %d arguments instead of %d to be used as a distribution", this->function->name, feenox.pde.dim, this->function->n_arguments);
      return FEENOX_ERROR;
    }

    for (int i = 0; i < feenox.pde.mesh->n_groups; i++) {
      this->defined_per_group[i] = 1;            
    }
    this->defined = 1;
    this->full = 1;
    this->eval = feenox_distribution_eval_function_global;
    this->expr = &this->function->algebraic_expression;
    feenox_call(feenox_pull_dependencies_variables_function(&this->dependency_variables, this->function));
    feenox_call(feenox_pull_dependencies_functions_function(&this->dependency_functions, this->function));
    this->non_uniform = feenox_depends_on_space(this->dependency_variables);
    this->constant = (feenox_depends_on_time(this->dependency_variables) == 0);
    
    return FEENOX_OK;
  }
  
  // try one variable for each volume
  full = 1;
  non_uniform = 0;
  n_volumes = 0;
  physical_group = NULL;
  i = 0;
  for (physical_group = feenox.pde.mesh->physical_groups; physical_group != NULL; physical_group = physical_group->hh.next) {
    if (physical_group->dimension == feenox.pde.dim) {
      char *var_name = NULL;
      feenox_check_minusone(asprintf(&var_name, "%s_%s", name, physical_group->name));
      var_t *variable = NULL;
      if ((variable = feenox_get_variable_ptr(var_name)) != NULL) {
        if (this->variable == NULL) {
          this->variable = variable;
        }
        
        // if there's no explicit material we create one
        if (physical_group->material == NULL) {
          physical_group->material = feenox_define_material_get_ptr(physical_group->name, feenox.pde.mesh);
        }
        
        // if there are many volumes, we are not uniform
        non_uniform = (n_volumes++ > 0);
        this->defined_per_group[i] = 1;
        this->defined = 1;

      } else {
        full = 0;
      }
      feenox_free(var_name);
    }  
    i++;
  }
  
  if (this->variable) {
    this->full = full;
    this->non_uniform = non_uniform;
    this->eval = feenox_distribution_eval_variable_local;
    return FEENOX_OK;
  }
  
  
  // try a single global variable
  if ((this->variable = feenox_get_variable_ptr(name)) != NULL) {
    for (int i = 0; i < feenox.pde.mesh->n_groups; i++) {
      this->defined_per_group[i] = 1;
    }
    this->defined = 1;
    this->full = 1;
    this->eval = feenox_distribution_eval_variable_global;
    return FEENOX_OK;
  }
  

  // not finding an actual distribution is not an error, there are optional distributions
  this->full = 0;
  this->eval = feenox_distribution_eval;
  return FEENOX_OK;
}

// default virtual method for undefined distributions
double feenox_distribution_eval(distribution_t *this, const double *x, material_t *material) {
  this->value = 0;
  return this->value;
}

double feenox_distribution_eval_variable_global(distribution_t *this, const double *x, material_t *material) {
  this->value = feenox_var_value(this->variable);
  return this->value;
}

double feenox_distribution_eval_function_global(distribution_t *this, const double *x, material_t *material) {
  this->value = feenox_function_eval(this->function, x);
  return this->value;
}

double feenox_distribution_eval_variable_local(distribution_t *this, const double *x, material_t *material) {
  
  if (material == NULL) {
    this->value = 0;
    return this->value;
  }
  
  if (material != this->last_material) {
    char *var_name = NULL;
    feenox_check_minusone(asprintf(&var_name, "%s_%s", this->name, material->name));
    if ((this->variable = feenox_get_variable_ptr(var_name)) == NULL) {
      feenox_push_error_message("cannot find variable '%s'", var_name);
      feenox_runtime_error();
    }
    this->last_material = material;
    feenox_free(var_name);
  }
  
  this->value = feenox_var_value(this->variable);
  return this->value;
}


double feenox_distribution_eval_function_local(distribution_t *this, const double *x, material_t *material) {
  
  if (material == NULL) {
    this->value = 0;
    return this->value;
  }
  
  if (material != this->last_material) {
    char *function_name = NULL;
    feenox_check_minusone(asprintf(&function_name, "%s_%s", this->name, material->name));
    if ((this->function = feenox_get_function_ptr(function_name)) == NULL) {
      feenox_push_error_message("cannot find function '%s'", function_name);
      feenox_runtime_error();
    }
    this->last_material = material;
    this->expr = &this->function->algebraic_expression;
    feenox_free(function_name);
  }
  
  this->value = feenox_function_eval(this->function, x);
  return this->value;
}

  
double feenox_distribution_eval_property(distribution_t *this, const double *x, material_t *material) {

  if (material == NULL) {
    // these can be zeroes if the properties do not depend on space
    if (x != NULL) {
      switch (feenox.pde.dim) {
        case 1:
          feenox_push_error_message("cannot find a material for point x=%g", x[0]);
          break;
        case 2:
          feenox_push_error_message("cannot find a material for point x=%g, y=%g", x[0], x[1]);
          break;
        case 3:
          feenox_push_error_message("cannot find a material for point x=%g, y=%g, z=%g", x[0], x[1], x[2]);
          break;
      }
    } else {
      feenox_push_error_message("no material nor coordinate for property '%s'", this->name);
    }  
    feenox_runtime_error();
  }
  
  if (material != this->last_material) {
    this->last_material = material;
    
    property_data_t *property_data = NULL;
    HASH_FIND_STR(material->property_datums, this->property->name, property_data);
    this->last_property_data = property_data;
    
    if (property_data != NULL) {
      this->expr = &property_data->expr;
    } else {
      this->expr = NULL;
      this->value = 0;
      return this->value;
    }  
  }
  
  if (this->last_property_data != NULL) {
    if (x != NULL) {
      // the property has an expression of x,y & z, it's not a function
      feenox_var_value(feenox.mesh.vars.x) = x[0];
      if (feenox.pde.dim > 1) {
        feenox_var_value(feenox.mesh.vars.y) = x[1];
        if (feenox.pde.dim > 2) {
          feenox_var_value(feenox.mesh.vars.z) = x[2];
        }  
      }
    }
    this->value = feenox_expression_eval(&this->last_property_data->expr);
    return this->value;
  }
  
  this->value = 0;
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
