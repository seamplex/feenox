/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's mesh-related geometry routines
 *
 *  Copyright (C) 2014--2023 Jeremy Theler
 *
 *  This file is part of feenox.
 *
 *  feenox is free software: you can redistribute it and/or modify
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
 *  along with feenox.  If not, see <http://www.gnu.org/licenses/>.
 *------------------- ------------  ----    --------  --     -       -         -
 */
#include "../feenox.h"

// c = b - a
void feenox_mesh_subtract(const double *a, const double *b, double *c) {
  c[0] = b[0] - a[0];
  c[1] = b[1] - a[1];
  c[2] = b[2] - a[2];
  return;
}

// c = a \times b
void feenox_mesh_cross(const double *a, const double *b, double *c) {
  c[0] = a[1]*b[2] - a[2]*b[1];
  c[1] = a[2]*b[0] - a[0]*b[2];
  c[2] = a[0]*b[1] - a[1]*b[0];
  return;
}

// c = a \times b / | a \times b |
void feenox_mesh_normalized_cross(const double *a, const double *b, double *c) {
  double norm;
  c[0] = a[1]*b[2] - a[2]*b[1];
  c[1] = a[2]*b[0] - a[0]*b[2];
  c[2] = a[0]*b[1] - a[1]*b[0];
  norm = gsl_hypot3(c[0], c[1], c[2]);
  if (norm != 0) {
    c[0] /= norm;
    c[1] /= norm;
    c[2] /= norm;
  }

  return;
}

// ( (a \times b) \cdot c ) in 3D
double feenox_mesh_cross_dot(const double *a, const double *b, const double *c) {
  return c[0]*(a[1]*b[2] - a[2]*b[1]) + c[1]*(a[2]*b[0] - a[0]*b[2]) + c[2]*(a[0]*b[1] - a[1]*b[0]);
}


// ( (b-a) \times (c-a)) ) in 2d
double feenox_mesh_subtract_cross_2d(const double *a, const double *b, const double *c) {
  return a[0]*(b[1]-c[1]) + b[0]*(c[1]-a[1]) + c[0]*(a[1]-b[1]);
}

// scalar product between a and b
double feenox_mesh_dot(const double *a, const double *b) {
  return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}

// scalar product between (b-a) y c
double feenox_mesh_subtract_dot(const double *b, const double *a, const double *c) {
  return (b[0]-a[0])*c[0] + (b[1]-a[1])*c[1] + (b[2]-a[2])*c[2];
}

// module of (b-a)
double feenox_mesh_subtract_module(const double *b, const double *a) {
  return gsl_hypot3(b[0]-a[0], b[1]-a[1], b[2]-a[2]);
}

// squared module of (b-a)
double feenox_mesh_subtract_squared_module(const  double *b, const  double *a) {
  double dx = (b[0]-a[0]);
  double dy = (b[1]-a[1]);
  double dz = (b[2]-a[2]);
  return dx*dx + dy*dy + dz*dz;
}

// squared module of (b-a) in 2d
double feenox_mesh_subtract_squared_module2d(const  double *b, const  double *a) {
  return (b[0]-a[0])*(b[0]-a[0]) + (b[1]-a[1])*(b[1]-a[1]);
}


// TODO: make a faster one assuming the elements are already oriented
int feenox_mesh_compute_outward_normal(element_t *element, double *n) {

  // gcc14 with optimizations give segfault with these extra dummy assignments
  // double opt_breaker = 0;

  // TODO: a method linked to the element type
  if (element->type->dim == 0) {
    n[0] = 1;
    n[1] = 0;
    n[2] = 0;
    // opt_breaker = n[0];
    
  } else if (element->type->dim == 1) {

    // WATCH out! this does not work with lines which do not lie on the xy plane!
    double module = feenox_mesh_subtract_module(element->node[1]->x, element->node[0]->x);
    n[0] = -(element->node[1]->x[1] - element->node[0]->x[1])/module;
    n[1] = +(element->node[1]->x[0] - element->node[0]->x[0])/module;
    n[2] = 0;
  
  } else if (element->type->dim == 2) {
    
    double a[3] = {0,0,0};
    double b[3] = {0,0,0};
    feenox_mesh_subtract(element->node[0]->x, element->node[1]->x, a);
    feenox_mesh_subtract(element->node[0]->x, element->node[2]->x, b);
    feenox_mesh_normalized_cross(a, b, n);

  } else if (element->type->dim == 3) {
    feenox_push_error_message("trying to compute the outward normal of a volume (element %d)", element->tag);
    return FEENOX_ERROR;
  }

  // opt_breaker = n[0];

  // if there's only one volumetric element, we check if n is the outward normal
  // if there's none (or more than one) then we rely on the element orientation
  if (feenox_mesh_count_element_volumetric_neighbors(element) == 1) {
    // first compute the center of the surface element
    double surface_center[3];
    feenox_call(feenox_mesh_compute_element_barycenter(element, surface_center));

    // then the center of the volume element
    element_t *volumetric_neighbor = NULL;
    volumetric_neighbor = feenox_mesh_find_element_volumetric_neighbor(element);

    double volumetric_neighbor_center[3];
    feenox_call(feenox_mesh_compute_element_barycenter(volumetric_neighbor, volumetric_neighbor_center));

    // compute the product between the proposed normal and the difference between these two
    // if the product is positive, invert the normal
    if (feenox_mesh_subtract_dot(volumetric_neighbor_center, surface_center, n) > 0) {
      n[0] = -n[0];
      n[1] = -n[1];
      n[2] = -n[2];
    }
  }

  // update nx ny and nz
  feenox_var_value(feenox.mesh.vars.arr_n[0]) = n[0];
  feenox_var_value(feenox.mesh.vars.arr_n[1]) = n[1];
  feenox_var_value(feenox.mesh.vars.arr_n[2]) = n[2];

  return FEENOX_OK;
}
