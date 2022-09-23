/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's mesh-related finite-element routines
 *
 *  Copyright (C) 2014--2022 jeremy theler
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

// inverts a small-size square matrix
/*
The inverse of a 2x2 matrix:

| a11 a12 |-1             |  a22 -a12 |
| a21 a22 |    =  1/DET * | -a21  a11 |

with DET  =  a11a22-a12a21

The inverse of a 3x3 matrix:

| a11 a12 a13 |-1             |   a33a22-a32a23  -(a33a12-a32a13)   a23a12-a22a13  |
| a21 a22 a23 |    =  1/DET * | -(a33a21-a31a23)   a33a11-a31a13  -(a23a11-a21a13) |
| a31 a32 a33 |               |   a32a21-a31a22  -(a32a11-a31a12)   a22a11-a21a12  |

with DET  =  a11(a33a22-a32a23)-a21(a33a12-a32a13)+a31(a23a12-a22a13)
*/

// TODO: virtual methods linked to the element type
int feenox_mesh_matrix_invert(gsl_matrix *direct, gsl_matrix *inverse) {

  double det = 0;
  double invdet = 0;

  switch (direct->size1) {
    case 1:
      if (gsl_matrix_get(direct, 0, 0) == 0) {
        feenox_push_error_message("singular 1x1 jacobian");
        return FEENOX_ERROR;
      }
    	gsl_matrix_set(inverse, 0, 0, 1.0/gsl_matrix_get(direct, 0, 0));
      break;
    case 2:
      det = + gsl_matrix_get(direct, 0, 0) * gsl_matrix_get(direct, 1, 1)
            - gsl_matrix_get(direct, 0, 1) * gsl_matrix_get(direct, 1, 0);
      if (det == 0) {
        feenox_push_error_message("singular 2x2 jacobian");
        return FEENOX_ERROR;
      }
      invdet = 1.0/det;
      gsl_matrix_set(inverse, 0, 0, +invdet*gsl_matrix_get(direct, 1, 1));
      gsl_matrix_set(inverse, 0, 1, -invdet*gsl_matrix_get(direct, 0, 1));
      gsl_matrix_set(inverse, 1, 0, -invdet*gsl_matrix_get(direct, 1, 0));
      gsl_matrix_set(inverse, 1, 1,  invdet*gsl_matrix_get(direct, 0, 0));
      break;
    case 3:
      // code from PETSc src/ksp/ksp/tutorials/ex42.c
      {
        double a00 = gsl_matrix_get(direct, 0, 0);
        double a01 = gsl_matrix_get(direct, 0, 1);
        double a02 = gsl_matrix_get(direct, 0, 2);
        double a10 = gsl_matrix_get(direct, 1, 0);
        double a11 = gsl_matrix_get(direct, 1, 1);
        double a12 = gsl_matrix_get(direct, 1, 2);
        double a20 = gsl_matrix_get(direct, 2, 0);
        double a21 = gsl_matrix_get(direct, 2, 1);
        double a22 = gsl_matrix_get(direct, 2, 2);

        double t4  = a20 * a01;
        double t6  = a20 * a02;
        double t8  = a10 * a01;
        double t10 = a10 * a02;
        double t12 = a00 * a11;
        double t14 = a00 * a12;
        double den = (t4 * a12 - t6 * a11 - t8 * a22 + t10 * a21 + t12 * a22 - t14 * a21);
/*        
        if (den == 0) {
          feenox_push_error_message("singular 3x3 jacobian");
          return FEENOX_ERROR;
        }
*/
        double t17 = 1.0 / den;

        gsl_matrix_set(inverse, 0, 0, +(a11 * a22 - a12 * a21) * t17);
        gsl_matrix_set(inverse, 0, 1, -(a01 * a22 - a02 * a21) * t17);
        gsl_matrix_set(inverse, 0, 2, +(a01 * a12 - a02 * a11) * t17);
        gsl_matrix_set(inverse, 1, 0, -(-a20 * a12 + a10 * a22) * t17);
        gsl_matrix_set(inverse, 1, 1, +(-t6 + a00 * a22) * t17);
        gsl_matrix_set(inverse, 1, 2, -(-t10 + t14) * t17);
        gsl_matrix_set(inverse, 2, 0, +(-a20 * a11 + a10 * a21) * t17);
        gsl_matrix_set(inverse, 2, 1, -(-t4 + a00 * a21) * t17);
        gsl_matrix_set(inverse, 2, 2, +(-t8 + t12) * t17);
      }
      break;
    default:
      feenox_push_error_message("invalid size %d of matrix to invert", direct->size1);
      break;
  }
  return FEENOX_OK;
}

inline double feenox_mesh_determinant(gsl_matrix *this) {

  switch (this->size1) {
    case 0:
      return 1.0;
    break;
    case 1:
      return gsl_matrix_get(this, 0, 0);
    break;
    case 2:
      return + gsl_matrix_get(this, 0, 0) * gsl_matrix_get(this, 1, 1)
             - gsl_matrix_get(this, 0, 1) * gsl_matrix_get(this, 1, 0);
    break;
    case 3:
      return + gsl_matrix_get(this, 0, 0) * gsl_matrix_get(this, 1, 1) * gsl_matrix_get(this, 2, 2)
             + gsl_matrix_get(this, 0, 1) * gsl_matrix_get(this, 1, 2) * gsl_matrix_get(this, 2, 0)
             + gsl_matrix_get(this, 0, 2) * gsl_matrix_get(this, 1, 0) * gsl_matrix_get(this, 2, 1) 
             - gsl_matrix_get(this, 0, 2) * gsl_matrix_get(this, 1, 1) * gsl_matrix_get(this, 2, 0)
             - gsl_matrix_get(this, 0, 1) * gsl_matrix_get(this, 1, 0) * gsl_matrix_get(this, 2, 2) 
             - gsl_matrix_get(this, 0, 0) * gsl_matrix_get(this, 1, 2) * gsl_matrix_get(this, 2, 1);  
    break;
  }
  
  return 0.0;
}

// copmpute the gradient of h with respect to x evaluated at r
int feenox_mesh_compute_dhdx(element_t *e, double *r, gsl_matrix *drdx_ref, gsl_matrix *dhdx) {

  gsl_matrix *dxdr = NULL;
  gsl_matrix *drdx = NULL;
  
  if (drdx_ref != NULL) {
    // si ya nos dieron drdx usamos esa
    drdx = drdx_ref;
    
  } else {
    // sino la calculamos
    feenox_check_alloc(drdx = gsl_matrix_calloc(e->type->dim, e->type->dim));
    feenox_check_alloc(dxdr = gsl_matrix_calloc(e->type->dim, e->type->dim));
    
    feenox_call(feenox_mesh_compute_dxdr(e, r, dxdr));
    feenox_call(feenox_mesh_matrix_invert(dxdr, drdx));
  }

  gsl_matrix_set_zero(dhdx);
  unsigned int j, m, m_prime;
  for (j = 0; j < e->type->nodes; j++) {
    for (m = 0; m < e->type->dim; m++) {
      for (m_prime = 0; m_prime < e->type->dim; m_prime++) {
        gsl_matrix_add_to_element(dhdx, j, m, e->type->dhdr(j, m_prime, r) * gsl_matrix_get(drdx, m_prime, m));
      }
    }
  }

  if (drdx_ref == NULL) {
    gsl_matrix_free(drdx);
    gsl_matrix_free(dxdr);
  }
  
  return FEENOX_OK;
}


// compute the gradient of the shape functions with respect to x evalauted at gauss point v of scheme integration
inline int feenox_mesh_compute_dhdx_at_gauss(element_t *e, int v, int integration) {
 
  // the first time we allocate the array of matrices
  if (e->dhdx == NULL) {
    feenox_check_alloc(e->dhdx = calloc(e->type->gauss[integration].V, sizeof(gsl_matrix *)));
  }
  
  // if the v-th matrix does not exist, allocate and continue otherwise use the cached result
  if (e->dhdx[v] == NULL) {
    feenox_check_alloc(e->dhdx[v] = gsl_matrix_calloc(e->type->nodes, e->type->dim));
  } else {
    return FEENOX_OK;
  }
  
  if (e->drdx == NULL || e->drdx[v] == NULL) {
    feenox_call(feenox_mesh_compute_drdx_at_gauss(e, v, integration));
  }

  // dhdx = dhdr * drdx
  feenox_call(gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, e->type->gauss[integration].dhdr[v], e->drdx[v], 0.0, e->dhdx[v]));

  return FEENOX_OK;
}

inline int feenox_mesh_compute_h(element_t *e, double *r, double *h) {

  for (unsigned int j = 0; j < e->type->nodes; j++) {
    h[j] = e->type->h(j, r);
  }

  return FEENOX_OK;

}


inline int feenox_mesh_compute_drdx_at_gauss(element_t *e, unsigned int v, int integration) {
  
  if (e->drdx == NULL) {
    feenox_check_alloc(e->drdx = calloc(e->type->gauss[integration].V, sizeof(gsl_matrix *)));
  }
  if (e->drdx[v] == NULL) {
    feenox_check_alloc(e->drdx[v] = gsl_matrix_calloc(e->type->dim, e->type->dim));
  } else {
    return FEENOX_OK;
  }
  
  if (e->dxdr == NULL || e->dxdr[v] == NULL) {
    feenox_call(feenox_mesh_compute_dxdr_at_gauss(e, v, integration));
  }
  
  feenox_call(feenox_mesh_matrix_invert(e->dxdr[v], e->drdx[v]));
  
  return FEENOX_OK; 
}



inline int feenox_mesh_compute_dxdr(element_t *e, double *r, gsl_matrix *dxdr) {

  // warning! this only works with volumetric elements, see dxdr_at_gauss()
  for (unsigned int m = 0; m < e->type->dim; m++) {
    for (unsigned int m_prime = 0; m_prime < e->type->dim; m_prime++) {
      for (unsigned int j = 0; j < e->type->nodes; j++) {
        gsl_matrix_add_to_element(dxdr, m, m_prime, e->type->dhdr(j, m_prime, r) * e->node[j]->x[m]);
      }
    }
  }
  
  return FEENOX_OK;
}


inline void mesh_compute_x(element_t *e, double *r, double *x) {

  // only for volumetric elements
  x[0] = x[1] = x[2] = 0;
  for (unsigned int j = 0; j < e->type->nodes; j++) {
    double h = e->type->h(j, r);
    for (unsigned int m = 0; m < 3; m++) {
      x[m] += h * e->node[j]->x[m];
    }
  }

  return;
}

inline int feenox_mesh_compute_wH_at_gauss(element_t *e, unsigned int v) {
  
  feenox_call(feenox_mesh_compute_w_at_gauss(e, v, feenox.pde.mesh->integration));
  feenox_call(feenox_mesh_compute_H_at_gauss(e, v, feenox.pde.mesh->integration));
  
  return FEENOX_OK;
}

inline int feenox_mesh_compute_wHB_at_gauss(element_t *e, unsigned int v) {
  
  feenox_call(feenox_mesh_compute_w_at_gauss(e, v, feenox.pde.mesh->integration));
  feenox_call(feenox_mesh_compute_H_at_gauss(e, v, feenox.pde.mesh->integration));
  feenox_call(feenox_mesh_compute_B_at_gauss(e, v, feenox.pde.mesh->integration));
  
  return FEENOX_OK;
}


inline int feenox_mesh_compute_w_at_gauss(element_t *e, unsigned int v, int integration) {

  if (e->w == NULL) {
    feenox_check_alloc(e->w = calloc(e->type->gauss[integration].V, sizeof(double)));
  }
  
  // this is where the magic begins
  if (e->dxdr == NULL || e->dxdr[v] == NULL) {
    feenox_call(feenox_mesh_compute_dxdr_at_gauss(e, v, integration));
  }
  // this is where the magic ends
  
  // TODO: choose to take the absolute value or not?
  // TODO: choose to complain or not
  if (e->w[v] == 0) {
    double det = feenox_mesh_determinant(e->dxdr[v]);
//    if (det <= 0) {
//      feenox_push_error_message("negative determinant for element %ld at integration point %d of %d", e->tag, v+1, e->type->gauss[integration].V);
//      return FEENOX_ERROR;
//      printf("negative determinant %g for element %ld at integration point %d of %d\n", det, e->tag, v+1, e->type->gauss[integration].V);
//      feenox_debug_print_gsl_matrix(e->dxdr[v], stdout);
//    } else {
//      printf("positive determinant %g for element %ld at integration point %d of %d\n", det, e->tag, v+1, e->type->gauss[integration].V);      
//    }      
    
    e->w[v] = e->type->gauss[integration].w[v] * fabs(det);
//    e->w[v] = e->type->gauss[integration].w[v] * det;
  }  

  return FEENOX_OK;
}

inline int feenox_mesh_compute_dxdr_at_gauss_1d(element_t *e, unsigned int v, int integration) {
  
  // we are a line but not aligned with the x axis we have to compute the axial coordinate l
  double dx = e->node[1]->x[0] - e->node[0]->x[0];
  double dy = e->node[1]->x[1] - e->node[0]->x[1];
  double dz = e->node[1]->x[2] - e->node[0]->x[2];
  double l = gsl_hypot3(dx, dy, dz);

  double dxdr = 0;
  for (unsigned int j = 0; j < e->type->nodes; j++) {
    dxdr += gsl_matrix_get(e->type->gauss[integration].dhdr[v], j, 0) *
              (e->node[j]->x[0] * dx/l + e->node[j]->x[1] * dy/l + e->node[j]->x[2] * dz/l);
  }
  
  gsl_matrix_set(e->dxdr[v], 0, 0, dxdr);
  
  return FEENOX_OK;
}

inline int feenox_mesh_compute_dxdr_at_gauss_2d(element_t *e, unsigned int v, int integration) {
  /*
   * if we are a triangle or a quadrangle (quadrangles are two triangles so they are alike)
   * but we do not live on the x-y plane we have to do some tricks:
   * we need a transformation matrix R that maps the outward normal n into [0,0,1]
   * i.e. such that when R is applied to the triangle, it will now live in the xy plane
   * note that there are infinite Rs that do this (because the resulting triangle can look like this
   * 
   * 
   *    +                                       +----------+
   *    |`\                                     `\         |
   *    |  `\                                     `\       |
   *    |    `\             or like this            `\     |
   *    |      `\                                     `\   |
   *    |        `\                                     `\ |
   *    +----------+                                      `+
   * 
   * see 
   * http://math.stackexchange.com/questions/180418/calculate-rotation-matrix-to-align-vector-a-to-vector-b-in-3d
   */

  double e_z[3] = {0, 0, 1};
  double u[3];
  feenox_mesh_cross(e->normal, e_z, u);
  double s = gsl_hypot3(u[0], u[1], u[2]);

/*
(%i4) U:matrix([0, -u2, u1],[u2, 0, -u0],[-u1, u0, 0]);
                           [  0    - u2   u1  ]
                           [                  ]
(%o4)                      [  u2    0    - u0 ]
                           [                  ]
                           [ - u1   u0    0   ]
(%i5) U . U;
                 [     2     2                           ]
                 [ - u2  - u1      u0 u1        u0 u2    ]
                 [                                       ]
(%o5)            [                  2     2              ]
                 [    u0 u1     - u2  - u0      u1 u2    ]
                 [                                       ]
                 [                               2     2 ]
                 [    u0 u2        u1 u2     - u1  - u0  ]
*/    

  double k = (1-feenox_mesh_dot(e->normal, e_z))/(s*s);
  double R[3][3] = {{ 1     + k * (-u[2]*u[2] - u[1]*u[1]),
                      -u[2] + k * (u[0]*u[1]),
                      +u[1] + k * (u[0]*u[2])},
                    {
                      +u[2] + k * (u[0]*u[1]),
                      1     + k * (-u[2]*u[2] - u[0]*u[0]),
                      -u[0] + k * (u[1]*u[2])},
                    {
                      -u[1] + k * (u[0]*u[2]),
                      +u[0] + k * (u[1]*u[2]),
                      1     + k * (-u[1]*u[1] - u[0]*u[0])
                    }};

/*  
  printf("coords\n");
  for (int i = 0; i < 3; i++) {
    printf("%g\t%g\t%g\n", e->node[i]->x[0], e->node[i]->x[1], e->node[i]->x[2]);
  }
  

  printf("R\n");
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      printf("%g\t", R[i][j]);
    }
    printf("\n");
  }  
*/                    
  // TODO: can this loop be re-written as a matrix-something product?
  double xi = 0;
  for (int m = 0; m < 2; m++) {
    for (int m_prime = 0; m_prime < 2; m_prime++) {
      xi = 0;
      for (int j = 0; j < e->type->nodes; j++) {
        xi += gsl_matrix_get(e->type->gauss[integration].dhdr[v], j, m) *
               (e->node[j]->x[0] * R[m_prime][0] +
                e->node[j]->x[1] * R[m_prime][1] +
                e->node[j]->x[2] * R[m_prime][2]);
      }
      gsl_matrix_set(e->dxdr[v], m, m_prime, xi);
    }
  }

  return FEENOX_OK;
}


inline int feenox_mesh_compute_dxdr_at_gauss_general(element_t *e, unsigned int v, int integration) {
  // we can do a full traditional computation
  // i.e. lines are in the x axis
  //      surfaces are on the xy plane
  //      volumes are always volumes!

//  printf("traditional\n");

  double xi = 0;
  for (unsigned int m = 0; m < e->type->dim; m++) {
    for (unsigned int m_prime = 0; m_prime < e->type->dim; m_prime++) {
      xi = 0;
      for (unsigned int j = 0; j < e->type->nodes; j++) {
        xi += gsl_matrix_get(e->type->gauss[integration].dhdr[v], j, m_prime) * e->node[j]->x[m];
      }
      gsl_matrix_set(e->dxdr[v], m, m_prime, xi);
    }
  }
  return FEENOX_OK;
}


// magic magic magic!
inline int feenox_mesh_compute_dxdr_at_gauss(element_t *e, unsigned int v, int integration) {

  if (e->dxdr == NULL) {
    feenox_check_alloc(e->dxdr = calloc(e->type->gauss[integration].V, sizeof(gsl_matrix *)));
  }
  
  if (e->dxdr[v] == NULL) {
    feenox_check_alloc(e->dxdr[v] = gsl_matrix_calloc(e->type->dim, e->type->dim));
  } else {
    return FEENOX_OK;
  }

  // TODO: once the problem type and dimension is set, we know which element type
  //       needs which kind of dxdr computation: we can then use some virtual
  //       functions defined in e->type
//  if (e->compute_dxdr == NULL) {
    if (e->type->dim == 1 && (e->node[0]->x[1] != 0 || e->node[1]->x[1] != 0 ||
                              e->node[0]->x[2] != 0 || e->node[1]->x[2] != 0)) {
      
//      e->compute_dxdr = feenox_mesh_compute_dxdr_at_gauss_1d;
      feenox_call(feenox_mesh_compute_dxdr_at_gauss_1d(e, v, integration));
      
    } else if (e->type->dim == 2 && (e->node[0]->x[2] != 0 || e->node[1]->x[2] != 0 || e->node[2]->x[2])) {
      
      feenox_call(feenox_mesh_compute_normal_2d(e));
      double eps = feenox_var_value(feenox.mesh.vars.eps);
      // ANDs are more efficient than ORs because the minute one does not hold the evaluation finishes
      if (fabs(e->normal[0]) < eps && fabs(e->normal[1]) < eps && fabs(fabs(e->normal[2])-1) < eps) {
        
//        e->compute_dxdr = feenox_mesh_compute_dxdr_at_gauss_general;
        feenox_call(feenox_mesh_compute_dxdr_at_gauss_general(e, v, integration));
        
      } else {
//        e->compute_dxdr = feenox_mesh_compute_dxdr_at_gauss_2d;
        feenox_call(feenox_mesh_compute_dxdr_at_gauss_2d(e, v, integration));
      }
      
/*      
      printf("dxdr\n");
      for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
          printf("%g\t", gsl_matrix_get(e->dxdr[v], i, j));
        }
        printf("\n");
      }        
*/      
    } else {
      
//      e->compute_dxdr = feenox_mesh_compute_dxdr_at_gauss_general;
      feenox_call(feenox_mesh_compute_dxdr_at_gauss_general(e, v, integration));
    }
//  }  
//  feenox_call(e->compute_dxdr(e, v, integration));
  
  return FEENOX_OK;
}


int feenox_mesh_compute_x_at_gauss(element_t *e, unsigned int v, int integration) {

  int j, m;
  
  if (e->x == NULL) {
    feenox_check_alloc(e->x = calloc(e->type->gauss[integration].V, sizeof(double *)));
  }
  if (e->x[v] == NULL) {
    feenox_check_alloc(e->x[v] = calloc(3, sizeof(double)));
  } else {
    return FEENOX_OK;
  }
  
  for (j = 0; j < e->type->nodes; j++) {
    for (m = 0; m < 3; m++) {
      e->x[v][m] += e->type->gauss[integration].h[v][j] * e->node[j]->x[m];
    }
  }

  return FEENOX_OK;
}


int feenox_mesh_compute_H_at_gauss(element_t *e, unsigned int v, int integration) {
  if (e->H == NULL) {
    feenox_check_alloc(e->H = calloc(e->type->gauss[integration].V, sizeof(gsl_matrix *)));
  }
  
  unsigned int dofs = feenox.pde.dofs;
  
  if (e->H[v] == NULL) {
    feenox_check_alloc(e->H[v] = gsl_matrix_calloc(dofs, dofs*e->type->nodes));
  } else {
    return FEENOX_OK;
  }
  
  for (unsigned int d = 0; d < dofs; d++) {
    for (unsigned int j = 0; j < e->type->nodes; j++) {
      gsl_matrix_set(e->H[v], d, dofs*j+d, e->type->gauss[integration].h[v][j]);
    }
  }

  return FEENOX_OK;  
}


int feenox_mesh_compute_B_at_gauss(element_t *e, unsigned int v, int integration) {

  if (e->B == NULL) {
    feenox_check_alloc(e->B = calloc(e->type->gauss[integration].V, sizeof(gsl_matrix *)));
  }
  
  unsigned int dofs = feenox.pde.dofs;
  
  if (e->B[v] == NULL) {
    feenox_check_alloc(e->B[v] = gsl_matrix_calloc(dofs*e->type->dim, dofs*e->type->nodes));
  } else {
    return FEENOX_OK;
  }
  
  if (e->dhdx == NULL || e->dhdx[v] == NULL) {
    feenox_call(feenox_mesh_compute_dhdx_at_gauss(e, v, integration));
  }
  
  // TODO: if dofs == 1 this is a transpose
  for (unsigned int m = 0; m < e->type->dim; m++) {
    for (unsigned int g = 0; g < dofs; g++) {
      for (unsigned int j = 0; j < e->type->nodes; j++) {
        gsl_matrix_set(e->B[v], dofs*m+g, dofs*j+g, gsl_matrix_get(e->dhdx[v], j, m));
      }
    }
  }
 
  return FEENOX_OK;
}

#ifdef HAVE_PETSC
int feenox_mesh_compute_dof_indices(element_t *e, mesh_t *mesh) {
  
  if (e->l == NULL) {
    feenox_check_alloc(e->l = calloc(e->type->nodes * mesh->degrees_of_freedom, sizeof(double)));
  } else {
    return FEENOX_OK;
  }
  
  // the vector l contains the global indexes of each DOF in the element
  // note that this vector is always node major independently of the global orderin
  unsigned int j, d;
  for (j = 0; j < e->type->nodes; j++) {
    for (d = 0; d < mesh->degrees_of_freedom; d++) {
      e->l[mesh->degrees_of_freedom*j + d] = e->node[j]->index_dof[d];
    }  
  }
  
  return FEENOX_OK;
  
}
#endif

/*
inline int feenox_mesh_update_coord_vars(double *x) {
  feenox_var_value(feenox.mesh.vars.x) = x[0];
  feenox_var_value(feenox.mesh.vars.y) = x[1];
  feenox_var_value(feenox.mesh.vars.z) = x[2];    
  return FEENOX_OK;
}
*/

/*
inline int mesh_compute_normal(element_t *element) {
  double n[3];

  feenox_call(mesh_compute_outward_normal(element, n));
  feenox_var_value(feenox_mesh.vars.nx) = n[0];
  feenox_var_value(feenox_mesh.vars.ny) = n[1];
  feenox_var_value(feenox_mesh.vars.nz) = n[2];
  
  return 0;
}  
*/
