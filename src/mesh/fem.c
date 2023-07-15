/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's mesh-related finite-element routines
 *
 *  Copyright (C) 2014--2023 jeremy theler
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
// compare to eigen's proposal
/*
template<typename Derived>
inline const typename Derived::Scalar bruteforce_det3_helper
(const MatrixBase<Derived>& matrix, int a, int b, int c)
{
  return matrix.coeff(0,a)
         * (matrix.coeff(1,b) * matrix.coeff(2,c) - matrix.coeff(1,c) * matrix.coeff(2,b));
}
template<typename Derived> struct determinant_impl<Derived, 3>
{
  static inline typename traits<Derived>::Scalar run(const Derived& m)
  {
    return bruteforce_det3_helper(m,0,1,2)
          - bruteforce_det3_helper(m,1,0,2)
          + bruteforce_det3_helper(m,2,0,1);
  }
};
*/
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

// build the canonica B_c matrix at an arbitrary location xi
gsl_matrix *feenox_mesh_compute_B_c(element_type_t *element_type, double *xi) {
  
  gsl_matrix *B_c = NULL;
  feenox_check_alloc_null(B_c = gsl_matrix_alloc(element_type->dim, element_type->nodes));
  for (int d = 0; d < element_type->dim; d++) {
    for (int j = 0; j < element_type->nodes; j++) {
      gsl_matrix_set(B_c, d, j, element_type->dhdxi(j, d, xi));
    }
  }
  
  return B_c;
}

// copmpute the gradient of h with respect to x evaluated at any arbitrary location
int feenox_mesh_compute_B(element_t *e, double *xi, gsl_matrix *invJ_ref, gsl_matrix *B) {

  gsl_matrix *J = NULL;
  gsl_matrix *invJ = NULL;
  
  if (invJ_ref != NULL) {
    // if the caller already provided a J{-1} we use that one
    invJ = invJ_ref;
    
  } else {
    // otherwise compute dxidx = J^{-1}
    feenox_check_alloc(invJ = gsl_matrix_calloc(e->type->dim, e->type->dim));
    feenox_check_alloc(J = gsl_matrix_calloc(e->type->dim, e->type->dim));
    
    feenox_call(feenox_mesh_compute_J(e, xi, J));
    feenox_call(feenox_mesh_matrix_invert(J, invJ));
  }
  
  gsl_matrix *B_c = feenox_mesh_compute_B_c(e->type, xi);
  feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1.0, invJ, B_c, 0.0, B));
  gsl_matrix_free(B_c);

  if (invJ_ref == NULL) {
    gsl_matrix_free(invJ);
    gsl_matrix_free(J);
  }
  
  return FEENOX_OK;
}


inline int feenox_mesh_compute_invJ_at_gauss(element_t *e, unsigned int q, int integration) {
  
  if (e->invJ == NULL) {
    feenox_check_alloc(e->invJ = calloc(e->type->gauss[integration].Q, sizeof(gsl_matrix *)));
  }
  if (e->invJ[q] == NULL) {
    feenox_check_alloc(e->invJ[q] = gsl_matrix_calloc(e->type->dim, e->type->dim));
  } else {
    return FEENOX_OK;
  }
  
  if (e->J == NULL || e->J[q] == NULL) {
    feenox_call(feenox_mesh_compute_J_at_gauss(e, q, integration));
  }
  
  feenox_call(feenox_mesh_matrix_invert(e->J[q], e->invJ[q]));
  
  return FEENOX_OK; 
}

// matrix with the coordinates
inline int feenox_mesh_compute_C(element_t *e) {
  
  if (e->C != NULL) {
    return FEENOX_OK;
  }
  
  feenox_check_alloc(e->C = gsl_matrix_alloc(e->type->dim, e->type->nodes));
  // TODO: this is not cache friendly, is it?
  for (unsigned int j = 0; j < e->type->nodes; j++) {
    for (unsigned int d = 0; d < e->type->dim; d++) {
      gsl_matrix_set(e->C, d, j, e->node[j]->x[d]);
    }
  }
  
  return FEENOX_OK;
}


inline int feenox_mesh_compute_J(element_t *e, double *xi, gsl_matrix *dxdxi) {

  // warning! this only works with volumetric elements, see dxdr_at_gauss()
  
  // TODO: measure
  // dxdxi = J = B_c * C_i
  // for (unsigned int d = 0; d < e->type->dim; d++) {
  //   for (unsigned int d_prime = 0; d_prime < e->type->dim; d_prime++) {
  //     for (unsigned int j = 0; j < e->type->nodes; j++) {
  //       gsl_matrix_add_to_element(dxdxi, d, d_prime, e->type->dhdxi(j, d_prime, r) * e->node[j]->x[d]);
  //     }
  //   }
  // }

  gsl_matrix *B_c = feenox_mesh_compute_B_c(e->type, xi);
  feenox_call(feenox_mesh_compute_C(e));
  feenox_call(gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, B_c, e->C, 0.0, dxdxi));
  gsl_matrix_free(B_c);
  
  return FEENOX_OK;
}


inline void mesh_compute_x(element_t *e, double *xi, double *x) {

  x[0] = x[1] = x[2] = 0;
  for (unsigned int j = 0; j < e->type->nodes; j++) {
    double h = e->type->h(j, xi);
    for (unsigned int d = 0; d < 3; d++) {
      x[d] += h * e->node[j]->x[d];
    }
  }

  return;
}

inline double *feenox_mesh_compute_x_at_gauss_if_needed(element_t *e, unsigned int q, int condition) {
  if (condition) {
    feenox_call_null(feenox_mesh_compute_x_at_gauss(e, q, feenox.pde.mesh->integration));
    return e->x[q];
  }
  
  return NULL;
}

inline double *feenox_mesh_compute_x_at_gauss_if_needed_and_update_var(element_t *e, unsigned int q, int condition) {
  if (condition) {
    feenox_call_null(feenox_mesh_compute_x_at_gauss(e, q, feenox.pde.mesh->integration));
    feenox_mesh_update_coord_vars(e->x[q]);
    return e->x[q];
  }
  
  return NULL;
}


inline material_t *feenox_mesh_get_material(element_t *e) {
  return (e->physical_group != NULL) ? e->physical_group->material : NULL;
}


inline int feenox_mesh_compute_wH_at_gauss(element_t *e, unsigned int q) {
  
  // TODO: can we lump these two in a single loop?
  feenox_call(feenox_mesh_compute_w_at_gauss(e, q, feenox.pde.mesh->integration));
  feenox_call(feenox_mesh_compute_H_Gc_at_gauss(e->type, q, feenox.pde.mesh->integration));
  
  return FEENOX_OK;
}

inline int feenox_mesh_compute_wHB_at_gauss(element_t *e, unsigned int q) {
  
  // TODO: can we lump these three in a single loop?
  feenox_call(feenox_mesh_compute_w_at_gauss(e, q, feenox.pde.mesh->integration));
  feenox_call(feenox_mesh_compute_H_Gc_at_gauss(e->type, q, feenox.pde.mesh->integration));
  feenox_call(feenox_mesh_compute_B_G_at_gauss(e, q, feenox.pde.mesh->integration));

  
  return FEENOX_OK;
}


inline int feenox_mesh_compute_w_at_gauss(element_t *e, unsigned int q, int integration) {

  if (e->w == NULL) {
    feenox_check_alloc(e->w = calloc(e->type->gauss[integration].Q, sizeof(double)));
  }
  
  // --- this is where the magic begins -----------------------
  if (e->J == NULL || e->J[q] == NULL) {
    feenox_call(feenox_mesh_compute_J_at_gauss(e, q, integration));
  }
  // --- this is where the magic ends -------------------------
  
  if (e->w[q] == 0) {
    double det = feenox_mesh_determinant(e->J[q]);
    
  // TODO: choose to complain about zero or negative?
  // TODO: choose to take the absolute value or not?
    e->w[q] = e->type->gauss[integration].w[q] * fabs(det);
//    e->w[v] = e->type->gauss[integration].w[v] * det;
  }  

  return FEENOX_OK;
}

inline int feenox_mesh_compute_J_at_gauss_1d(element_t *e, unsigned int q, int integration) {
  
  // we are a line but not aligned with the x axis we have to compute the axial coordinate l
  double dx = e->node[1]->x[0] - e->node[0]->x[0];
  double dy = e->node[1]->x[1] - e->node[0]->x[1];
  double dz = e->node[1]->x[2] - e->node[0]->x[2];
  double l = gsl_hypot3(dx, dy, dz);
  dx /= l;
  dy /= l;
  dz /= l;

  double dxdxi = 0;
  for (unsigned int j = 0; j < e->type->nodes; j++) {
    dxdxi += gsl_matrix_get(e->type->gauss[integration].B_c[q], 0, j) *
              (e->node[j]->x[0] * dx + e->node[j]->x[1] * dy + e->node[j]->x[2] * dz);
  }
  
  gsl_matrix_set(e->J[q], 0, 0, dxdxi);
  
  return FEENOX_OK;
}

inline int feenox_mesh_compute_J_at_gauss_2d(element_t *e, unsigned int q, int integration) {
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

  // the versor in the z direction
  double e_z[3] = {0, 0, 1};

  double t[3];
  feenox_mesh_cross(e->normal, e_z, t);

//  double c = feenox_mesh_dot(e->normal, e_z); // cosine
  double k = 1/(1+feenox_mesh_dot(e->normal, e_z));
  
/*
(%i4) T:matrix([0, -t2, t1],[t2, 0, -t0],[-t1, t0, 0]);
                           [  0    - t2   t1  ]
                           [                  ]
(%o4)                      [  t2    0    - t0 ]
                           [                  ]
                           [ - t1   t0    0   ]
(%i5) T . T;
                 [     2     2                           ]
                 [ - t2  - t1      t0 t1        t0 t2    ]
                 [                                       ]
(%o5)            [                  2     2              ]
                 [    t0 t1     - t2  - t0      t1 t2    ]
                 [                                       ]
                 [                               2     2 ]
                 [    t0 t2        t1 t2     - t1  - t0  ]
*/    

  double R[3][3] = {{ 1     + k * (-t[2]*t[2] - t[1]*t[1]),
                      -t[2] + k * (t[0]*t[1]),
                      +t[1] + k * (t[0]*t[2])},
                    {
                      +t[2] + k * (t[0]*t[1]),
                      1     + k * (-t[2]*t[2] - t[0]*t[0]),
                      -t[0] + k * (t[1]*t[2])},
                    {
                      -t[1] + k * (t[0]*t[2]),
                      +t[0] + k * (t[1]*t[2]),
                      1     + k * (-t[1]*t[1] - t[0]*t[0])
                    }};

  
  // TODO: measure
  // write the new coordinates matrix and compute J (a.k.a dxdxi)
  // as the product between B and C
  
  double s = 0;
  for (int d = 0; d < 2; d++) {
    for (int d_prime = 0; d_prime < 2; d_prime++) {
      s = 0;
      for (int j = 0; j < e->type->nodes; j++) {
        s += gsl_matrix_get(e->type->gauss[integration].B_c[q], d, j) *
               (e->node[j]->x[0] * R[d_prime][0] +
                e->node[j]->x[1] * R[d_prime][1] +
                e->node[j]->x[2] * R[d_prime][2]);
      }
      gsl_matrix_set(e->J[q], d, d_prime, s);
    }
  }

  return FEENOX_OK;
}


inline int feenox_mesh_compute_J_at_gauss_general(element_t *e, unsigned int q, int integration) {
  // we can do a full traditional computation
  // i.e. lines are in the x axis
  //      surfaces are on the xy plane
  //      volumes are always volumes!

  if (e->type->dim == 0)
  {
    return FEENOX_OK;
  }

  // TODO: benchmark
/*  
  double xi = 0;
  for (unsigned int d = 0; d < dim; d++) {
    for (unsigned int d_prime = 0; d_prime < dim; d_prime++) {
      xi = 0;
      for (unsigned int j = 0; j < nodes; j++) {
        xi += gsl_matrix_get(e->type->gauss[integration].B_c[q], d_prime, j) * e->node[j]->x[d];
      }
      gsl_matrix_set(e->J[q], d, d_prime, xi);
    }
  }
*/
  
  if (e->C == NULL) {
    feenox_call(feenox_mesh_compute_C(e));
  }
  feenox_call(gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1.0, e->C, e->type->gauss[integration].B_c[q], 0.0, e->J[q]));
  
  return FEENOX_OK;
}


// magic magic magic!
inline int feenox_mesh_compute_J_at_gauss(element_t *e, unsigned int q, int integration) {

  if (e->J == NULL) {
    feenox_check_alloc(e->J = calloc(e->type->gauss[integration].Q, sizeof(gsl_matrix *)));
  }
  
  if (e->J[q] == NULL) {
    feenox_check_alloc(e->J[q] = gsl_matrix_calloc(e->type->dim, e->type->dim));
  } else {
    return FEENOX_OK;
  }

  // TODO: once the problem type and dimension is set, we know which element type
  //       needs which kind of xixdr computation: we can then use some virtual
  //       functions defined in e->type
  if (e->type->dim == 1 && (e->node[0]->x[1] != 0 || e->node[1]->x[1] != 0 ||
                            e->node[0]->x[2] != 0 || e->node[1]->x[2] != 0)) {

    feenox_call(feenox_mesh_compute_J_at_gauss_1d(e, q, integration));

  } else if (e->type->dim == 2 && (e->node[0]->x[2] != 0 || e->node[1]->x[2] != 0 || e->node[2]->x[2])) {

    feenox_call(feenox_mesh_compute_normal_2d(e));
    double eps = feenox_var_value(feenox.mesh.vars.eps);
    // ANDs are more efficient than ORs because the minute one does not hold the evaluation finishes
    if (fabs(e->normal[0]) < eps && fabs(e->normal[1]) < eps && fabs(fabs(e->normal[2])-1) < eps) {

      feenox_call(feenox_mesh_compute_J_at_gauss_general(e, q, integration));

    } else {

      feenox_call(feenox_mesh_compute_J_at_gauss_2d(e, q, integration));

    }

  } else {

    feenox_call(feenox_mesh_compute_J_at_gauss_general(e, q, integration));
  }
  
  return FEENOX_OK;
}


int feenox_mesh_compute_x_at_gauss(element_t *e, unsigned int q, int integration) {

  if (e->x == NULL) {
    feenox_check_alloc(e->x = calloc(e->type->gauss[integration].Q, sizeof(double *)));
  }
  if (e->x[q] == NULL) {
    feenox_check_alloc(e->x[q] = calloc(3, sizeof(double)));
  } else {
    return FEENOX_OK;
  }
  
  for (unsigned int j = 0; j < e->type->nodes; j++) {
    double h = gsl_matrix_get(e->type->gauss[integration].H_c[q], 0, j);
    for (unsigned int d = 0; d < 3; d++) {
      e->x[q][d] += h * e->node[j]->x[d];
    }
  }

  return FEENOX_OK;
}


int feenox_mesh_compute_H_Gc_at_gauss(element_type_t *element_type, unsigned int q, int integration) {
  
  unsigned int G = feenox.pde.dofs;
  if (element_type->H_Gc == NULL) {
    if (G == 1) {
      element_type->H_Gc = element_type->gauss[integration].H_c;
      return FEENOX_OK;
    }
    feenox_check_alloc(element_type->H_Gc = calloc(element_type->gauss[integration].Q, sizeof(gsl_matrix *)));
  }
  
  if (element_type->H_Gc[q] != NULL) {
    return FEENOX_OK;
  }
  
  
  unsigned int J = element_type->nodes;
  feenox_check_alloc(element_type->H_Gc[q] = gsl_matrix_calloc(G, G*J));
  
  // TODO: measure order of loops
  for (unsigned int j = 0; j < J; j++) {
    double h = gsl_matrix_get(element_type->gauss[integration].H_c[q], 0, j);
    for (unsigned int g = 0; g < G; g++) {
      gsl_matrix_set(element_type->H_Gc[q], g, G*j+g, h);
    }
  }

  return FEENOX_OK;  
}

int feenox_mesh_compute_B_at_gauss(element_t *e, unsigned int q, int integration) {
  
  // the first time we allocate the array of matrices
  if (e->B == NULL) {
    feenox_check_alloc(e->B = calloc(e->type->gauss[integration].Q, sizeof(gsl_matrix *)));
  }
  
  // if the q-th matrix does not exist, allocate and continue otherwise use the cached result
  unsigned int J = e->type->nodes;
  unsigned int D = e->type->dim;
  if (e->B[q] == NULL) {
    feenox_check_alloc(e->B[q] = gsl_matrix_calloc(D, J));
  } else {
    return FEENOX_OK;
  }

  feenox_call(feenox_mesh_compute_invJ_at_gauss(e, q, feenox.pde.mesh->integration));
  feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1.0, e->invJ[q], e->type->gauss[integration].B_c[q], 0.0, e->B[q]));
  
  return FEENOX_OK;
}  

/*
int feenox_mesh_compute_B_Gc_at_gauss(element_type_t *element_type, unsigned int q, int integration) {

  unsigned int G = feenox.pde.dofs;
  if (element_type->B_Gc == NULL) {
    if (G == 1) {
      element_type->B_Gc = element_type->B_c;
      return FEENOX_OK;
    }
    feenox_check_alloc(element_type->B_Gc = calloc(element_type->gauss[integration].Q, sizeof(gsl_matrix *)));
  }
  
  unsigned int J = element_type->nodes;
  unsigned int D = element_type->dim;
  feenox_check_alloc(element_type->B_Gc[q] = gsl_matrix_calloc(G*D, G*D*J));
  
  for (unsigned int d = 0; d < element_type->dim; d++) {
    size_t Gd = G*d;
    for (unsigned int j = 0; j < element_type->nodes; j++) {
      size_t Gj = G*j;
      double dhdxi = gsl_matrix_get(element_type->gauss[integration].B_c[q], d, j);
      for (unsigned int g = 0; g < feenox.pde.dofs; g++) {
        gsl_matrix_set(element_type->B_Gc[q], Gd+g, Gj+g, dhdxi);
      }
    }
  }

  return FEENOX_OK;
}
*/

int feenox_mesh_compute_B_G_at_gauss(element_t *e, unsigned int q, int integration) {

  feenox_call(feenox_mesh_compute_B_at_gauss(e, q, integration));
  
  unsigned int G = feenox.pde.dofs;
  if (e->B_G == NULL) {
    if (G == 1) {
      e->B_G = e->B;
      return FEENOX_OK;
    }
    feenox_check_alloc(e->B_G = calloc(e->type->gauss[integration].Q, sizeof(gsl_matrix *)));
  }
  
  if (e->B_G[q] != NULL) {
    return FEENOX_OK;
  }
  
  feenox_call(feenox_mesh_compute_B_at_gauss(e, q, integration));
  
  unsigned int J = e->type->nodes;
  unsigned int D = e->type->dim;
  feenox_check_alloc(e->B_G[q] = gsl_matrix_calloc(G*D, G*J));
  
  for (unsigned int d = 0; d < D; d++) {
    size_t Gd = G*d;
    for (unsigned int j = 0; j < J; j++) {
      size_t Gj = G*j;
      double dhdxi = gsl_matrix_get(e->B[q], d, j);
      for (unsigned int g = 0; g < feenox.pde.dofs; g++) {
        gsl_matrix_set(e->B_G[q], Gd+g, Gj+g, dhdxi);
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
