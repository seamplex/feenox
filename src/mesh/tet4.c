/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's mesh-related tetrahedron element routines
 *
 *  Copyright (C) 2015--2020 jeremy theler
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

#include "element.h"

// -------------------------------------
// iso-parametric four-noded tetrahedron
// -------------------------------------

int mesh_tet4_init(void) {
  
  element_type_t *element_type;
  double r[3];
  double a, b, c, d;
  int j, v;
  
  element_type = &feenox.mesh.element_types[ELEMENT_TYPE_TETRAHEDRON4];
  element_type->name = strdup("tet4");
  element_type->id = ELEMENT_TYPE_TETRAHEDRON4;
  element_type->dim = 3;
  element_type->order = 1;
  element_type->nodes = 4;
  element_type->faces = 4;
  element_type->nodes_per_face = 3;
  element_type->h = mesh_tet4_h;
  element_type->dhdr = mesh_tet4_dhdr;
  element_type->point_in_element = mesh_point_in_tetrahedron;
  element_type->element_volume = mesh_tet_vol;


  // node coordinates (from Gmsh doc)
/*
Tetrahedron:                     

                   v
                 .
               ,/
              /
           2                     
         ,/|`\                   
       ,/  |  `\                 
     ,/    '.   `\               
   ,/       |     `\             
 ,/         |       `\           
0-----------'.--------1 --> u    
 `\.         |      ,/           
    `\.      |    ,/             
       `\.   '. ,/               
          `\. |/                 
             `3                  
                `\.
                   ` w

*/     
  element_type->node_coords = calloc(element_type->nodes, sizeof(double *));
  element_type->node_parents = calloc(element_type->nodes, sizeof(node_relative_t *));  
  for (j = 0; j < element_type->nodes; j++) {
    element_type->node_coords[j] = calloc(element_type->dim, sizeof(double));  
  }
  
  element_type->vertices++;
  element_type->node_coords[0][0] = 0;
  element_type->node_coords[0][1] = 0;
  element_type->node_coords[0][2] = 0;
  
  element_type->vertices++;
  element_type->node_coords[1][0] = 1;  
  element_type->node_coords[1][1] = 0;
  element_type->node_coords[1][2] = 0;
  
  element_type->vertices++;
  element_type->node_coords[2][0] = 0;  
  element_type->node_coords[2][1] = 1;
  element_type->node_coords[2][2] = 0;

  element_type->vertices++;
  element_type->node_coords[3][0] = 0;  
  element_type->node_coords[3][1] = 0;
  element_type->node_coords[3][2] = 1;
  
  // ------------
  // gauss points and extrapolation matrices
  
  // full integration: 4 points
  mesh_gauss_init_tet4(element_type, &element_type->gauss[integration_full]);
  element_type->gauss[integration_full].extrap = gsl_matrix_calloc(element_type->nodes, 4);

  // reduced integration: 1 point
  mesh_gauss_init_tet1(element_type, &element_type->gauss[integration_reduced]);
  element_type->gauss[integration_reduced].extrap = gsl_matrix_calloc(element_type->nodes, 1);
  
  // the two extrapolation matrices
  a = (5.0-M_SQRT5)/20.0;
  b = (5.0+3.0*M_SQRT5)/20.0;
  c = -a/(b-a);
  d = 1+(1-b)/(b-a);
    
  r[0] = c;
  r[1] = c;
  r[2] = c;
  for (v = 0; v < 4; v++) {
    gsl_matrix_set(element_type->gauss[integration_full].extrap, 0, v, mesh_tet4_h(v, r));
  }

  r[0] = d;
  r[1] = c;
  r[2] = c;
  for (v = 0; v < 4; v++) {
    gsl_matrix_set(element_type->gauss[integration_full].extrap, 1, v, mesh_tet4_h(v, r));
  }

  r[0] = c;
  r[1] = d;
  r[2] = c;
  for (v = 0; v < 4; v++) {
    gsl_matrix_set(element_type->gauss[integration_full].extrap, 2, v, mesh_tet4_h(v, r));
  }

  r[0] = c;
  r[1] = c;
  r[2] = d;
  for (v = 0; v < 4; v++) {
    gsl_matrix_set(element_type->gauss[integration_full].extrap, 3, v, mesh_tet4_h(v, r));
  }
  
  // reduced
  for (j = 0; j < element_type->nodes; j++) {
    gsl_matrix_set(element_type->gauss[integration_reduced].extrap, j, 0, 1.0);
  }  

  return FEENOX_OK;
}


void mesh_gauss_init_tet1(element_type_t *element_type, gauss_t *gauss) {
 
  // ---- one Gauss point ----  
  mesh_alloc_gauss(gauss, element_type, 4);
    
  gauss->w[0] = 1.0/6.0 * 1.0;
  gauss->r[0][0] = 1.0/4.0;
  gauss->r[0][1] = 1.0/4.0;
  gauss->r[0][2] = 1.0/4.0;

  mesh_init_shape_at_gauss(gauss, element_type);  
    
  return;
}

void mesh_gauss_init_tet4(element_type_t *element_type, gauss_t *gauss) {

  double a = (5.0-M_SQRT5)/20.0;
  double b = (5.0+3.0*M_SQRT5)/20.0;

  // ---- two Gauss points ----  
  mesh_alloc_gauss(gauss, element_type, 4);
    
  gauss->w[0] = 1.0/6.0 * 1.0/4.0;
  gauss->r[0][0] = a;
  gauss->r[0][1] = a;
  gauss->r[0][2] = a;
  
  gauss->w[1] = 1.0/6.0 * 1.0/4.0;
  gauss->r[1][0] = b;
  gauss->r[1][1] = a;
  gauss->r[1][2] = a;
 
  gauss->w[2] = 1.0/6.0 * 1.0/4.0;
  gauss->r[2][0] = a;
  gauss->r[2][1] = b;
  gauss->r[2][2] = a;
    
  gauss->w[3] = 1.0/6.0 * 1.0/4.0;
  gauss->r[3][0] = a;
  gauss->r[3][1] = a;
  gauss->r[3][2] = b;

  mesh_init_shape_at_gauss(gauss, element_type);  
  
}



double mesh_tet4_h(int j, double *vec_r) {
  double r = vec_r[0];
  double s = vec_r[1];
  double t = vec_r[2];

  switch (j) {
    case 0:
      return 1-r-s-t;
      break;
    case 1:
      return r;
      break;
    case 2:
      return s;
      break;
    case 3:
      return t;
      break;
  }

  return 0;

}


double mesh_tet4_dhdr(int j, int m, double *vec_r) {
  
  switch (j) {
    case 0:
      switch(m) {
        case 0:
          return -1;
        break;
        case 1:
          return -1;
        break;
        case 2:
          return -1;
        break;
      }
    break;
    case 1:
      switch(m) {
        case 0:
          return +1;
        break;
        case 1:
          return 0;
        break;
        case 2:
          return 0;
        break;
      }
    break;
    case 2:
      switch(m) {
        case 0:
          return 0;
        break;
        case 1:
          return +1;
        break;
        case 2:
          return 0;
        break;
      }
    break;
    case 3:
      switch(m) {
        case 0:
          return 0;
        break;
        case 1:
          return 0;
        break;
        case 2:
          return +1;
        break;
      }
    break;
  }

  return 0;


}



/*
int mesh_point_in_tetrahedron(element_t *element, const double *x) {

// http://en.wikipedia.org/wiki/Barycentric_coordinate_system  
  double zero, one, lambda1, lambda2, lambda3, lambda4;
  double xi;
  gsl_matrix *T = gsl_matrix_alloc(3, 3);
  gsl_vector *xx4 = gsl_vector_alloc(3);
  gsl_vector *lambda = gsl_vector_alloc(3);
  gsl_permutation *p = gsl_permutation_alloc(3);
  int s, flag;
  int i, j;
  
  for (i = 0; i < 3; i++) {
    for (j = 0; j < 3; j++) {
      gsl_matrix_set(T, i, j, element->node[j]->x[i] - element->node[3]->x[i]);
    }
    gsl_vector_set(xx4, i, x[i] - element->node[3]->x[i]);
  }
  gsl_linalg_LU_decomp (T, p, &s);
  if ((xi = fabs(gsl_linalg_LU_det (T, s))) < 1e-20) {
//    feenox_push_error_message("element %d is degenerate", element->tag);
//    feenox_runtime_error();
    return 0;
  }
  gsl_linalg_LU_solve (T, p, xx4, lambda);

  zero = -feenox_var(wasora_mesh.vars.eps);
  one = 1+feenox_var(wasora_mesh.vars.eps);
  lambda1 = gsl_vector_get(lambda, 0);
  lambda2 = gsl_vector_get(lambda, 1);
  lambda3 = gsl_vector_get(lambda, 2);
  lambda4 = 1 - gsl_vector_get(lambda, 0) - gsl_vector_get(lambda, 1) - gsl_vector_get(lambda, 2);
  
  flag = (lambda1 > zero && lambda1 < one &&
          lambda2 > zero && lambda2 < one &&
          lambda3 > zero && lambda3 < one &&
          lambda4 > zero && lambda4 < one);

  
  gsl_matrix_free(T);
  gsl_vector_free(xx4);
  gsl_vector_free(lambda);
  gsl_permutation_free(p);
  
  return flag;
}
*/


int mesh_point_in_tetrahedron(element_t *element, const double *x) {

// http://en.wikipedia.org/wiki/Barycentric_coordinate_system  
  double zero, one, lambda4;
  double T[3][3];
  double inv[3][3];
  double xx0[3];
  double lambda[3];
  double det;
  double t4, t6, t8, t10, t12, t14, t17;
  int i, j, k;
  
  for (j = 1; j < 4; j++) {
    T[0][j-1] = element->node[j]->x[0] - element->node[0]->x[0];
    T[1][j-1] = element->node[j]->x[1] - element->node[0]->x[1];
    T[2][j-1] = element->node[j]->x[2] - element->node[0]->x[2];
  }
  xx0[0] = x[0] - element->node[0]->x[0];
  xx0[1] = x[1] - element->node[0]->x[1];
  xx0[2] = x[2] - element->node[0]->x[2];
  
/*  
  det = + T[0][0] * T[1][1] * T[2][2]
        + T[0][1] * T[1][2] * T[2][0]
        + T[0][2] * T[1][0] * T[2][1] 
        - T[0][2] * T[1][1] * T[2][0]
        - T[0][1] * T[1][0] * T[2][2] 
        - T[0][0] * T[1][2] * T[2][1];    
*/

  t4  = T[2][0] * T[0][1];
  t6  = T[2][0] * T[0][2];
  t8  = T[1][0] * T[0][1];
  t10 = T[1][0] * T[0][2];
  t12 = T[0][0] * T[1][1];
  t14 = T[0][0] * T[1][2];
  det = t4 * T[1][2] - t6 * T[1][1] - t8 * T[2][2] + t10 * T[2][1] + t12 * T[2][2] - t14 * T[2][1];
  if (fabs(det) < 1e-12) {
    // if the element is degenerate it cannot contain any point
    return 0;
  }
  t17 = 1.0 / det;

  inv[0][0] = +(T[1][1] * T[2][2] - T[1][2] * T[2][1]) * t17;
  inv[0][1] = -(T[0][1] * T[2][2] - T[0][2] * T[2][1]) * t17;
  inv[0][2] = +(T[0][1] * T[1][2] - T[0][2] * T[1][1]) * t17;
  inv[1][0] = -(-T[2][0] * T[1][2] + T[1][0] * T[2][2]) * t17;
  inv[1][1] = (-t6 + T[0][0] * T[2][2]) * t17;
  inv[1][2] = -(-t10 + t14) * t17;
  inv[2][0] = (-T[2][0] * T[1][1] + T[1][0] * T[2][1]) * t17;
  inv[2][1] = -(-t4 + T[0][0] * T[2][1]) * t17;
  inv[2][2] = (-t8 + t12) * t17;
    
  for (i = 0; i < 3; i++) {
    lambda[i] = 0;
    for (k = 0; k < 3; k++) {
      lambda[i] += inv[i][k] * xx0[k];
    }
  }
 
  zero = -feenox_var_value(feenox.mesh.vars.eps);
  one = 1+feenox_var_value(feenox.mesh.vars.eps);
  lambda4 = 1 - lambda[0] - lambda[1] - lambda[2];
  
  
//  printf("\n%d %g %g %g %g\n", element->tag, lambda[0], lambda[1], lambda[2], lambda4);
  
  return (lambda[0] > zero && lambda[0] < one &&
          lambda[1] > zero && lambda[1] < one &&
          lambda[2] > zero && lambda[2] < one &&
          lambda4 > zero && lambda4 < one);
}


double mesh_tet_vol(element_t *this) {

  if (this->volume == 0) {
    double a[3], b[3], c[3];
    
    mesh_subtract(this->node[0]->x, this->node[1]->x, a);
    mesh_subtract(this->node[0]->x, this->node[2]->x, b);
    mesh_subtract(this->node[0]->x, this->node[3]->x, c);
  
    this->volume = 1.0/(1.0*2.0*3.0) * fabs(mesh_cross_dot(c, a, b));
  }  
  
  return this->volume;

// AFEM.Ch09.pdf
// 6V = J = x 21 (y 23 z 34 − y34 z 23 ) + x32 (y34 z 12 − y12 z34 ) + x 43 (y12 z23 − y23 z 12),
  
 
}
