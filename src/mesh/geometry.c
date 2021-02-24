/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's mesh-related geometry routines
 *
 *  Copyright (C) 2014--2016 jeremy theler
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


//#include <gsl/gsl_math.h>

// c = b - a
void mesh_subtract(const double *a, const double *b, double *c) {
  c[0] = b[0] - a[0];
  c[1] = b[1] - a[1];
  c[2] = b[2] - a[2];
  return;
}

// c = a \times b
void mesh_cross(const double *a, const double *b, double *c) {
  c[0] = a[1]*b[2] - a[2]*b[1];
  c[1] = a[2]*b[0] - a[0]*b[2];
  c[2] = a[0]*b[1] - a[1]*b[0];
  return;
}

// c = a \times b / | a \times b |
void mesh_normalized_cross(const double *a, const double *b, double *c) {
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

// devuelve ( (a \times b) \cdot c ) en tres dimensiones
double mesh_cross_dot(const double *a, const double *b, const double *c) {
  return c[0]*(a[1]*b[2] - a[2]*b[1]) + c[1]*(a[2]*b[0] - a[0]*b[2]) + c[2]*(a[0]*b[1] - a[1]*b[0]);
}


// devuelve ( (b-a) \times (c-a)) ) en dos dimensiones
double mesh_subtract_cross_2d(const double *a, const double *b, const double *c) {
  return a[0]*(b[1]-c[1]) + b[0]*(c[1]-a[1]) + c[0]*(a[1]-b[1]);
}

// devuelve el producto escalar entre a y b
double mesh_dot(const double *a, const double *b) {
  return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}

// devuelve el producto escalar entre (b-a) y c
double mesh_subtract_dot(const double *b, const double *a, const double *c) {
  return (b[0]-a[0])*c[0] + (b[1]-a[1])*c[1] + (b[2]-a[2])*c[2];
}

// devuelve el modulo de (b-a)
double mesh_subtract_module(const double *b, const double *a) {
  return gsl_hypot3(b[0]-a[0], b[1]-a[1], b[2]-a[2]);
}

// devuelve el modulo al cuadrado de (b-a)
double mesh_subtract_squared_module(const  double *b, const  double *a) {
  return (b[0]-a[0])*(b[0]-a[0]) + (b[1]-a[1])*(b[1]-a[1]) + (b[2]-a[2])*(b[2]-a[2]);
}

// devuelve el modulo al cuadrado de (b-a)
double mesh_subtract_squared_module2d(const  double *b, const  double *a) {
  return (b[0]-a[0])*(b[0]-a[0]) + (b[1]-a[1])*(b[1]-a[1]);
}


// TODO: make a faster one assuming the elements are already oriented
int mesh_compute_outward_normal(element_t *element, double *n) {
  
  double a[3], b[3], surface_center[3], volumetric_neighbor_center[3];
  element_t *volumetric_neighbor;

  if (element->type->dim == 0) {
    feenox_push_error_message("trying to compute the outward normal of a point (element %d)", element->tag);
    return FEENOX_ERROR;
  } else if (element->type->dim == 1) {

    // OJO que no camina con lineas que no estan en el plano xy!!
    double module = mesh_subtract_module(element->node[1]->x, element->node[0]->x);
    n[0] = -(element->node[1]->x[1] - element->node[0]->x[1])/module;
    n[1] = +(element->node[1]->x[0] - element->node[0]->x[0])/module;
    n[2] = 0;
  
  } else if (element->type->dim == 2) {
    
    // este algoritmo viene de sn_elements_compute_outward_normal
    mesh_subtract(element->node[0]->x, element->node[1]->x, a);
    mesh_subtract(element->node[0]->x, element->node[2]->x, b);
    mesh_normalized_cross(a, b, n);

  } else if (element->type->dim == 3) {
    feenox_push_error_message("trying to compute the outward normal of a volume (element %d)", element->tag);
    return FEENOX_ERROR;
  }

  
  // ahora tenemos que ver si la normal que elegimos es efectivamente la outward
  // para eso primero calculamos el centro del elemento de superficie
  feenox_call(mesh_compute_element_barycenter(element, surface_center));

  // y despues el centro del elemento de volumen
  if ((volumetric_neighbor = mesh_find_element_volumetric_neighbor(element)) == NULL) {
    feenox_push_error_message("cannot find any volumetric neighbor for surface element %d", element->tag);
    return FEENOX_ERROR;
  }
    
  volumetric_neighbor = mesh_find_element_volumetric_neighbor(element);
  feenox_call(mesh_compute_element_barycenter(volumetric_neighbor, volumetric_neighbor_center));

  // calculamos el producto entre la normal propuesta y la resta de estos dos vectores
  // si elegimos la otra direccion, la damos tavuel
  if (mesh_subtract_dot(volumetric_neighbor_center, surface_center, n) > 0) {
    n[0] = -n[0];
    n[1] = -n[1];
    n[2] = -n[2];
  }    
          
  return FEENOX_OK;
}

/*
int mesh_compute_outward_normal(element_t *element, double *n) {
    // viene de sn_elements_compute_outward_normal
    // calculamos el vector normal para las variables nx ny y nx
    mesh_subtract(element->node[0]->x, element->node[1]->x, a);
    mesh_subtract(element->node[0]->x, element->node[2]->x, b);
    mesh_normalized_cross(a, b, n);
    
    // ahora tenemos que ver si la normal que elegimos es efectivamente la outward
    // para eso primero calculamos el centro del elemento de superficie
    feenox_call(mesh_compute_element_barycenter(element, surface_center));

    // y despues el centro del elemento de volumen
    if ((volumetric_neighbor = mesh_find_element_volumetric_neighbor(element)) == NULL) {
      feenox_push_error_message("cannot find any volumetric neighbor for surface element %d", element->id);
      PetscFunctionReturn(FEENOX_ERROR);
    }
    
    volumetric_neighbor = mesh_find_element_volumetric_neighbor(element);
    feenox_call(mesh_compute_element_barycenter(volumetric_neighbor, volumetric_neighbor_center));

    // calculamos el producto entre la normal propuesta y la resta de estos dos vectores
    // si elegimos la otra direccion, la damos tavuel
    if (mesh_subtract_dot(volumetric_neighbor_center, surface_center, n) > 0) {
      n[0] = -n[0];
      n[1] = -n[1];
      n[2] = -n[2];
    }    
*/