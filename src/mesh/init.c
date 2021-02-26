/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's mesh-related initialization routines
 *
 *  Copyright (C) 2014--2021 jeremy theler
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
#include "../feenox.h"
extern feenox_t feenox;


int feenox_mesh_init_special_objects(void) {
  
  if (feenox.mesh.vars.x != NULL) {
    return FEENOX_OK;
  }
  
  // vector con las coordenadas globales
  feenox.mesh.vars.vec_x = feenox_define_vector_get_ptr("x_global", 3);
  // como ahora hacemos alias de a los elementos del vector x_global lo inicializamos
  feenox_call(feenox_vector_init(feenox.mesh.vars.vec_x));
  
//va+x+name x
//va+x+desc Holder variable for spatial dependance of functions, such spatial distribution
//va+x+desc of physical properties or results of partial differential equations.
  feenox.mesh.vars.x = feenox_get_or_define_variable_get_ptr("x");
  feenox_realloc_variable_ptr(feenox.mesh.vars.x, gsl_vector_ptr(feenox_value_ptr(feenox.mesh.vars.vec_x), 0), 0);

//va+y+name y
//va+y+desc Idem as `x`.
  feenox.mesh.vars.y = feenox_get_or_define_variable_get_ptr("y");
  feenox_realloc_variable_ptr(feenox.mesh.vars.y, gsl_vector_ptr(feenox_value_ptr(feenox.mesh.vars.vec_x), 1), 0);

//va+z+name z
//va+z+desc Idem as `x`.
  feenox.mesh.vars.z = feenox_get_or_define_variable_get_ptr("z");
  feenox_realloc_variable_ptr(feenox.mesh.vars.z, gsl_vector_ptr(feenox_value_ptr(feenox.mesh.vars.vec_x), 2), 0);

// y ya que estamos las ponemos en un array numerico
  feenox.mesh.vars.arr_x[0] = feenox.mesh.vars.x;
  feenox.mesh.vars.arr_x[1] = feenox.mesh.vars.y;
  feenox.mesh.vars.arr_x[2] = feenox.mesh.vars.z;
  
  
  // idem para la normal
  feenox.mesh.vars.vec_n = feenox_define_vector_get_ptr("n_global", 3);
  // como ahora hacemos alias de a los elementos del vector n_global lo inicializamos
  feenox_call(feenox_vector_init(feenox.mesh.vars.vec_n));
  
//va+x+name nx
//va+x+desc Holder variable for the local outward normal in surfaces.
  feenox.mesh.vars.nx = feenox_get_or_define_variable_get_ptr("nx");
  feenox_realloc_variable_ptr(feenox.mesh.vars.nx, gsl_vector_ptr(feenox_value_ptr(feenox.mesh.vars.vec_n), 0), 0);

//va+y+name ny
//va+y+desc Idem as `nx`.
  feenox.mesh.vars.ny = feenox_get_or_define_variable_get_ptr("ny");
  feenox_realloc_variable_ptr(feenox.mesh.vars.ny, gsl_vector_ptr(feenox_value_ptr(feenox.mesh.vars.vec_n), 1), 0);

//va+z+name z
//va+z+desc Idem as `x`.
  feenox.mesh.vars.nz = feenox_get_or_define_variable_get_ptr("nz");
  feenox_realloc_variable_ptr(feenox.mesh.vars.nz, gsl_vector_ptr(feenox_value_ptr(feenox.mesh.vars.vec_n), 2), 0);

// y ya que estamos las ponemos en un array numerico
  feenox.mesh.vars.arr_n[0] = feenox.mesh.vars.nx;
  feenox.mesh.vars.arr_n[1] = feenox.mesh.vars.ny;
  feenox.mesh.vars.arr_n[2] = feenox.mesh.vars.nz;  
  
///va+nodes+name nodes
///va+nodes+desc Number of nodes of the unstructured grid.
  feenox.mesh.vars.nodes = feenox_get_or_define_variable_get_ptr("nodes");

///va+elements+name elements
///va+elements+desc Number of total elements of the unstructured grid. This number
///va+elements+desc include those surface elements that belong to boundary physical groups.
  feenox.mesh.vars.elements = feenox_get_or_define_variable_get_ptr("elements");

///va+cells+name cells
///va+cells+desc Number of cells of the unstructured grid. This number is the actual
///va+cells+desc quantity of volumetric elements in which the domain was discretized.
  feenox.mesh.vars.cells = feenox_get_or_define_variable_get_ptr("cells");

///va+bbox_min+name bbox_min
///va+bbox_min+desc Minimum values of the mesh’s bounding box (vector of size 3)
//  feenox.mesh.vars.bbox_min = feenox_define_vector("bbox_min", 3, NULL, NULL);

///va+bbox_min+name bbox_min
///va+bbox_min+desc Maximum values of the mesh’s bounding box (vector of size 3)
//  feenox.mesh.vars.bbox_max = feenox_define_vector("bbox_max", 3, NULL, NULL);
  
//va+eps+name eps
//va+eps+desc Small value. Default is $10^{-6}$.
  feenox.mesh.vars.eps = feenox_get_or_define_variable_get_ptr("eps");
  feenox_var_value(feenox.mesh.vars.eps) = MESH_TOL;

  
//va+mesh_failed_interpolation_factor+name mesh_failed_interpolation_factor
//va+mesh_failed_interpolation_factor+desc When interpolating a mesh-defined function, the interpolation point\ $\vec{x}$ seems to fall outside
//va+mesh_failed_interpolation_factor+desc an element using the $k$-dimensional tree (most efficient), and more robust brute-force approach is taken
//va+mesh_failed_interpolation_factor+desc less eficient using a radius of size `mesh_failed_interpolation_factor` times the distance between $\vec{x}$
//va+mesh_failed_interpolation_factor+desc and the nearest node to \vec{x$} is performed.
//va+mesh_failed_interpolation_factor+desc If this factor is zero or negative, then the value at the nearest node to $x$ is returned. Default is DEFAULT_MESH_FAILED_INTERPOLATION_FACTOR.
  feenox.mesh.vars.mesh_failed_interpolation_factor = feenox_get_or_define_variable_get_ptr("mesh_failed_interpolation_factor");
  feenox_var_value(feenox.mesh.vars.mesh_failed_interpolation_factor) = MESH_FAILED_INTERPOLATION_FACTOR;
  
// initialize the static data of the elements
  feenox_call(feenox_mesh_element_types_init());
     
  return FEENOX_OK;
}

