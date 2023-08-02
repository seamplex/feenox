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
// TODO: virtual methods linked to the element type?
gsl_matrix *feenox_mesh_matrix_invert(gsl_matrix *direct, gsl_matrix *inverse) {

  switch (direct->size1) {
    case 1:
      if (inverse == NULL) {
        inverse = gsl_matrix_alloc(1, 1);
      }
    	gsl_matrix_set(inverse, 0, 0, 1.0/gsl_matrix_get(direct, 0, 0));
      break;
      
    case 2:
      {
        double a = gsl_matrix_get(direct, 0, 0);
        double d = gsl_matrix_get(direct, 1, 1); 
        double b = gsl_matrix_get(direct, 0, 1);
        double c = gsl_matrix_get(direct, 1, 0);
        double det = a*d - b*c;
        double invdet = 1.0/det;
        
        if (inverse == NULL) {
          inverse = gsl_matrix_alloc(2, 2);
        }
        gsl_matrix_set(inverse, 0, 0, +invdet*d);
        gsl_matrix_set(inverse, 0, 1, -invdet*b);
        gsl_matrix_set(inverse, 1, 0, -invdet*c);
        gsl_matrix_set(inverse, 1, 1,  invdet*a);
      }
      break;
    case 3:
      {
        // code from PETSc src/ksp/ksp/tutorials/ex42.c
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
        double t17 = 1.0 / den;
  
        if (inverse == NULL) {
          inverse = gsl_matrix_alloc(3, 3);
        }
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
  return inverse;
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
// TODO: measure
      
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

// build the canonic B_c matrix at an arbitrary location xi
gsl_matrix *feenox_mesh_compute_B_c(element_type_t *element_type, double *xi) {
  
  gsl_matrix *B_c = gsl_matrix_alloc(element_type->dim, element_type->nodes);
  for (int d = 0; d < element_type->dim; d++) {
    for (int j = 0; j < element_type->nodes; j++) {
      gsl_matrix_set(B_c, d, j, element_type->dhdxi(j, d, xi));
    }
  }
  
  return B_c;
}

// compute the gradient of h with respect to x evaluated at any arbitrary location
gsl_matrix *feenox_mesh_compute_B(element_t *e, double *xi) {

  gsl_matrix *J = feenox_mesh_compute_J(e, xi);
  gsl_matrix *invJ = feenox_mesh_matrix_invert(J, NULL);
  
  gsl_matrix *B_c = feenox_mesh_compute_B_c(e->type, xi);
  gsl_matrix *B = gsl_matrix_alloc(e->type->dim, e->type->nodes);
  gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1.0, invJ, B_c, 0.0, B);
  
  gsl_matrix_free(B_c);
  gsl_matrix_free(invJ);
  gsl_matrix_free(J);
  
  return B;
}


inline gsl_matrix *feenox_mesh_compute_invJ_at_gauss(element_t *e, unsigned int q, int integration) {
  
  gsl_matrix ***invJ = (feenox.pde.cache_J) ? &e->invJ : &feenox.pde.invJi;
  if (*invJ == NULL) {
    feenox_check_alloc_null(*invJ = calloc(e->type->gauss[integration].Q, sizeof(gsl_matrix *)));
  }
  if ((*invJ)[q] == NULL) {
    (*invJ)[q] = gsl_matrix_alloc(e->type->dim, e->type->dim);
  } else if (feenox.pde.cache_J) {
    return (*invJ)[q];
  }

  gsl_matrix ***J = (feenox.pde.cache_J) ? &e->J : &feenox.pde.Ji;
  if ((*J) == NULL) {
    feenox_check_alloc_null((*J) = calloc(e->type->gauss[integration].Q, sizeof(gsl_matrix *)));
  }
  if (feenox.pde.cache_J == 0 || (*J)[q] == NULL) {
    (*J)[q] = feenox_mesh_compute_J_at_gauss(e, q, integration);
  }

  feenox_mesh_matrix_invert((*J)[q], (*invJ)[q]);
  return (*invJ)[q];
}

// matrix with the coordinates
inline gsl_matrix *feenox_mesh_compute_C(element_t *e) {
  
  gsl_matrix *C = NULL;
  // TODO: I don't like this
/*
  if (feenox.pde.cache_B) {
    if (e->C != NULL) {
      return e->C;
    } else {
      C = gsl_matrix_alloc(e->type->dim, e->type->nodes);
      e->C = C;
    }
  } else if (C == NULL) {
*/
    C = gsl_matrix_alloc(e->type->dim, e->type->nodes);

// }
 
  // TODO: this is not cache friendly, is it?
  for (unsigned int j = 0; j < e->type->nodes; j++) {
    for (unsigned int d = 0; d < e->type->dim; d++) {
      gsl_matrix_set(C, d, j, e->node[j]->x[d]);
    }
  }
  
  return C;
}


inline gsl_matrix *feenox_mesh_compute_J(element_t *e, double *xi) {

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

  gsl_matrix *J = gsl_matrix_alloc(e->type->dim, e->type->dim);
  gsl_matrix *B_c = feenox_mesh_compute_B_c(e->type, xi);
  gsl_matrix *C = feenox_mesh_compute_C(e);
  gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, B_c, C, 0.0, J);
  if (feenox.pde.cache_B == PETSC_FALSE) {
    feenox_free(C);
  }
  gsl_matrix_free(B_c);
  
  return J;
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
  return (condition) ? feenox_mesh_compute_x_at_gauss(e, q, feenox.pde.mesh->integration) : NULL;
}

inline double *feenox_mesh_compute_x_at_gauss_if_needed_and_update_var(element_t *e, unsigned int q, int condition) {
  if (condition) {
    double *x = feenox_mesh_compute_x_at_gauss(e, q, feenox.pde.mesh->integration);
    feenox_mesh_update_coord_vars(x);
    return x;
  }
  
  return NULL;
}

inline material_t *feenox_mesh_get_material(element_t *e) {
  return (e->physical_group != NULL) ? e->physical_group->material : NULL;
}

/*
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
*/

inline double feenox_mesh_compute_w_det_at_gauss(element_t *e, unsigned int q, int integration) {

/*
  if (e->w == NULL) {
    feenox_check_alloc(e->w = calloc(e->type->gauss[integration].Q, sizeof(double)));
  }
  gsl_matrix *J = NULL;
  if (e->J == NULL || e->J[q] == NULL) {
    J = feenox_mesh_compute_J_at_gauss(e, q, integration);
  } else {
    J = e->J[q];
  }
 */
  gsl_matrix *J = feenox_mesh_compute_J_at_gauss(e, q, integration); 
  
//  if (e->w[q] == 0) {
    double det = feenox_mesh_determinant(J);
    
  // TODO: choose to complain about zero or negative?
  // TODO: choose to take the absolute value or not? put these two as defines
    return e->type->gauss[integration].w[q] * fabs(det);
//    e->w[v] = e->type->gauss[integration].w[v] * det;
//  }  

}

inline gsl_matrix *feenox_mesh_compute_J_at_gauss_1d(element_t *e, unsigned int q, int integration) {
  
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
  
  gsl_matrix *J = gsl_matrix_alloc(1,1);
  gsl_matrix_set(J, 0, 0, dxdxi);
  
  return J;
}

inline gsl_matrix *feenox_mesh_compute_J_at_gauss_2d(element_t *e, unsigned int q, int integration) {
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
  
  gsl_matrix *J = gsl_matrix_alloc(2,2);
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
      gsl_matrix_set(J, d, d_prime, s);
    }
  }

  return FEENOX_OK;
}


inline gsl_matrix *feenox_mesh_compute_J_at_gauss_general(element_t *e, unsigned int q, int integration) {
  // we can do a full traditional computation
  // i.e. lines are in the x axis
  //      surfaces are on the xy plane
  //      volumes are always volumes!

  if (e->type->dim == 0)
  {
    return NULL;
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
  
  gsl_matrix *C = (e->C != NULL) ? e->C : feenox_mesh_compute_C(e);
  gsl_matrix *J = gsl_matrix_alloc(e->type->dim, e->type->dim);
  gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1.0, C, e->type->gauss[integration].B_c[q], 0.0, J);
  
  return J;
}


// magic magic magic!
inline gsl_matrix *feenox_mesh_compute_J_at_gauss(element_t *e, unsigned int q, int integration) {
  
  gsl_matrix *J = NULL;
/*
  if (feenox.pde.cache_B) {
    if (e->J == NULL) {
      e->J = calloc(e->type->gauss[integration].Q, sizeof(gsl_matrix *));
    }
  
    if (e->J[q] == NULL) {
      e->J[q] = gsl_matrix_calloc(e->type->dim, e->type->dim);
    } else {
      return e->J[q];
    }
  }
*/
  // TODO: once the problem type and dimension is set, we know which element type
  //       needs which kind of xixdr computation: we can then use some virtual
  //       functions defined in e->type
  if (e->type->dim == 1 && (e->node[0]->x[1] != 0 || e->node[1]->x[1] != 0 ||
                            e->node[0]->x[2] != 0 || e->node[1]->x[2] != 0)) {

    J = feenox_mesh_compute_J_at_gauss_1d(e, q, integration);

  } else if (e->type->dim == 2 && (e->node[0]->x[2] != 0 || e->node[1]->x[2] != 0 || e->node[2]->x[2])) {

    feenox_mesh_compute_normal_2d(e);
    double eps = feenox_var_value(feenox.mesh.vars.eps);
    // ANDs are more efficient than ORs because the minute one does not hold the evaluation finishes
    if (fabs(e->normal[0]) < eps && fabs(e->normal[1]) < eps && fabs(fabs(e->normal[2])-1) < eps) {

      J = feenox_mesh_compute_J_at_gauss_general(e, q, integration);

    } else {

      J = feenox_mesh_compute_J_at_gauss_2d(e, q, integration);

    }

  } else {

    J = feenox_mesh_compute_J_at_gauss_general(e, q, integration);
  }
  
  return J;
}


inline double *feenox_mesh_compute_x_at_gauss(element_t *e, unsigned int q, int integration) {

  double *x = NULL;
  if (feenox.pde.cache_B) {
    if (e->x == NULL) {
      e->x = calloc(e->type->gauss[integration].Q, sizeof(double *));
    }
    if (e->x[q] == NULL) {
      x = e->x[q] = calloc(3, sizeof(double));
    } else {
      return e->x[q];
    }
  } else {
    x = calloc(3, sizeof(double));
  }
  
  for (unsigned int j = 0; j < e->type->nodes; j++) {
    double h = gsl_matrix_get(e->type->gauss[integration].H_c[q], 0, j);
    for (unsigned int d = 0; d < 3; d++) {
      e->x[q][d] += h * e->node[j]->x[d];
    }
  }

  return x;
}


inline gsl_matrix *feenox_mesh_compute_H_Gc_at_gauss(element_type_t *element_type, unsigned int q, int integration) {
  
  unsigned int G = feenox.pde.dofs;
  if (element_type->H_Gc == NULL) {
    if (G == 1) {
      element_type->H_Gc = element_type->gauss[integration].H_c;
      return element_type->H_Gc[q];
    }
    element_type->H_Gc = calloc(element_type->gauss[integration].Q, sizeof(gsl_matrix *));
  }
  
  if (element_type->H_Gc[q] != NULL) {
    return element_type->H_Gc[q];
  }
  
  
  unsigned int J = element_type->nodes;
  element_type->H_Gc[q] = gsl_matrix_calloc(G, G*J);
  
  // TODO: measure order of loops
  for (unsigned int j = 0; j < J; j++) {
    double h = gsl_matrix_get(element_type->gauss[integration].H_c[q], 0, j);
    for (unsigned int g = 0; g < G; g++) {
      gsl_matrix_set(element_type->H_Gc[q], g, G*j+g, h);
    }
  }

  return element_type->H_Gc[q];  
}

inline gsl_matrix *feenox_mesh_compute_B_at_gauss(element_t *e, unsigned int q, int integration) {

  gsl_matrix ***B = (feenox.pde.cache_B) ? &e->B : &feenox.pde.Bi;
  if (*B == NULL) {
    feenox_check_alloc_null(*B = calloc(e->type->gauss[integration].Q, sizeof(gsl_matrix *)));
  }
  if ((*B)[q] == NULL) {
    (*B)[q] = gsl_matrix_alloc(e->type->dim, e->type->nodes);
    printf("aloque B\n");
  } else if (feenox.pde.cache_J) {
    return (*B)[q];
  }
//  printf("B = %p\n", &((*B)[q]));
//  feenox_debug_print_gsl_matrix((*B)[q], stdout);

  gsl_matrix ***invJ = (feenox.pde.cache_J) ? &e->invJ : &feenox.pde.invJi;
  if (*invJ == NULL) {
    feenox_check_alloc_null(*invJ = calloc(e->type->gauss[integration].Q, sizeof(gsl_matrix *)));
  }
//  if ((*invJ)[q] == NULL || feenox.pde.cache_J == 0) {
//    (*invJ)[q] = feenox_mesh_compute_invJ_at_gauss(e, q, feenox.pde.mesh->integration);
  gsl_matrix *tmp = feenox_mesh_compute_invJ_at_gauss(e, q, feenox.pde.mesh->integration);
  printf("invJ = %p\n", tmp);
//  }
/*  
  printf("tag = %ld dim = %d\n", e->tag, e->type->dim);
  printf("invJ\n");
  feenox_debug_print_gsl_matrix((*invJ)[q], stdout);
  printf("B_c\n");
  feenox_debug_print_gsl_matrix(e->type->gauss[integration].B_c[q], stdout);
*/
//  gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1.0, (*invJ)[q], e->type->gauss[integration].B_c[q], 0.0, (*B)[q]);
  gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1.0, tmp, e->type->gauss[integration].B_c[q], 0.0, (*B)[q]);  
  gsl_matrix_free(tmp);
  
  return (*B)[q];
}  


inline gsl_matrix *feenox_mesh_compute_B_G_at_gauss(element_t *e, unsigned int q, int integration) {

  
  gsl_matrix ***B_G = (feenox.pde.cache_B) ? &e->B_G : &feenox.pde.B_Gi;
  if (*B_G == NULL) {
    feenox_check_alloc_null(*B_G = calloc(e->type->gauss[integration].Q, sizeof(gsl_matrix *)));
  }
  if (*B_G[q] == NULL) {
    *B_G[q] = gsl_matrix_alloc(e->type->dim, e->type->dim);
  } else if (feenox.pde.cache_J) {
    return *B_G[q];
  }

  gsl_matrix ***B = (feenox.pde.cache_B) ? &e->B : &feenox.pde.Bi;
  if (*B == NULL) {
    feenox_check_alloc_null(*B = calloc(e->type->gauss[integration].Q, sizeof(gsl_matrix *)));
  }
  if (*B[q] == NULL || feenox.pde.cache_B == 0) {
    *B[q] = feenox_mesh_compute_B_at_gauss(e, q, integration);
  }
  unsigned int G = feenox.pde.dofs;
  if (G == 1) {
    *B_G[q] = *B[q];
  } else {
    unsigned int J = e->type->nodes;
    unsigned int D = e->type->dim;
    if (feenox.pde.cache_B == 0) {
      *B_G[q] = gsl_matrix_calloc(G*D, G*J);
    }
  
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
  }

  return *B_G[q];
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
