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


int feenox_fem_elemental_caches_reset(void) {
  
  if (feenox.fem.current_gauss_type != NULL) {
    int integration = (feenox.pde.mesh != NULL) ? feenox.pde.mesh->integration : 0;
    for (unsigned int q = 0; q < feenox.fem.current_gauss_type->gauss[integration].Q; q++) {
      if (feenox.fem.x != NULL && feenox.fem.x[q] != NULL) {
        feenox_free(feenox.fem.x[q]);
      }
      
      if (feenox.fem.Ji != NULL) {
        gsl_matrix_free(feenox.fem.Ji[q]);
        feenox.fem.Ji[q] = NULL;
      }

      if (feenox.fem.invJi != NULL) {
        gsl_matrix_free(feenox.fem.invJi[q]);
        feenox.fem.invJi[q] = NULL;
      }

      if (feenox.fem.Bi != NULL) {
        gsl_matrix_free(feenox.fem.Bi[q]);
        feenox.fem.Bi[q] = NULL;
      }

      if (feenox.fem.B_Gi != NULL) {
        gsl_matrix_free(feenox.fem.B_Gi[q]);
        feenox.fem.B_Gi[q] = NULL;
      }
    }
  }
  
  feenox_free(feenox.fem.w);
  
  gsl_matrix_free(feenox.fem.C);
  feenox.fem.C = NULL; 
  
  feenox_free(feenox.fem.x);
  feenox_free(feenox.fem.Ji);
  feenox_free(feenox.fem.invJi);
  feenox_free(feenox.fem.Bi);
  feenox_free(feenox.fem.B_Gi);
  
  feenox.fem.current_gauss_element_tag = 0;
  feenox.fem.current_gauss_type = NULL;
  
  return FEENOX_OK;

}



// inverts a small-size square matrix
// TODO: virtual methods linked to the element type?
gsl_matrix *feenox_fem_matrix_invert(gsl_matrix *direct, gsl_matrix *inverse) {

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

inline double feenox_fem_determinant(gsl_matrix *this) {

  if (this == NULL) {
    return 1.0;
  }
  
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
  
  return 1.0;
}

// build the canonic B_c matrix at an arbitrary location xi
gsl_matrix *feenox_fem_compute_B_c(element_t *e, double *xi) {
  
  gsl_matrix *B_c = gsl_matrix_calloc(e->type->dim, e->type->nodes);
  for (int d = 0; d < e->type->dim; d++) {
    for (int j = 0; j < e->type->nodes; j++) {
      gsl_matrix_set(B_c, d, j, e->type->dhdxi(j, d, xi));
    }
  }
  
  return B_c;
}

// compute the gradient of h with respect to x evaluated at any arbitrary location
gsl_matrix *feenox_fem_compute_B(element_t *e, double *xi) {

  gsl_matrix *J = feenox_fem_compute_J(e, xi);
  gsl_matrix *invJ = feenox_fem_matrix_invert(J, NULL);
  
  gsl_matrix *B_c = feenox_fem_compute_B_c(e, xi);
  gsl_matrix *B = gsl_matrix_calloc(e->type->dim, e->type->nodes);
  gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1.0, invJ, B_c, 0.0, B);
  
  
  return B;
}


inline gsl_matrix *feenox_fem_compute_invJ_at_gauss(element_t *e, unsigned int q, int integration) {
  
  // TODO: macro
  gsl_matrix ***invJ = (feenox.fem.cache_J) ? &e->invJ : &feenox.fem.invJi;
  if (*invJ == NULL) {
    feenox_check_alloc_null(*invJ = calloc(e->type->gauss[integration].Q, sizeof(gsl_matrix *)));
  }
  if ((*invJ)[q] == NULL) {
    (*invJ)[q] = gsl_matrix_calloc(e->type->dim, e->type->dim);
  } else if (feenox.fem.cache_J) {
    return (*invJ)[q];
  }

  gsl_matrix *J = feenox_fem_compute_J_at_gauss(e, q, integration);
  feenox_fem_matrix_invert(J, (*invJ)[q]);
  return (*invJ)[q];
}

// matrix with the coordinates
inline gsl_matrix *feenox_fem_compute_C(element_t *e) {
  if (feenox.fem.cache_J == PETSC_FALSE && e->type != feenox.fem.current_gauss_type) {
    feenox_fem_elemental_caches_reset();
  }
  
  gsl_matrix **C = (feenox.fem.cache_J) ? &e->C : &feenox.fem.C;
  if ((*C) == NULL) {
    (*C) = gsl_matrix_calloc(e->type->dim, e->type->nodes);
  } else if (feenox.fem.cache_J || e->tag == feenox.fem.current_gauss_element_tag) {
    return (*C);
  }
  
  // TODO: this is not cache friendly, is it?
  for (unsigned int j = 0; j < e->type->nodes; j++) {
    for (unsigned int d = 0; d < e->type->dim; d++) {
      gsl_matrix_set((*C), d, j, e->node[j]->x[d]);
    }
  }
  feenox.fem.current_gauss_element_tag = e->tag;
  
  return (*C);
}


inline gsl_matrix *feenox_fem_compute_J(element_t *e, double *xi) {

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

  gsl_matrix *J = gsl_matrix_calloc(e->type->dim, e->type->dim);
  gsl_matrix *B_c = feenox_fem_compute_B_c(e, xi);
  gsl_matrix *C = feenox_fem_compute_C(e);
  gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, B_c, C, 0.0, J);
  gsl_matrix_free(B_c);
  
  return J;
}

inline double *feenox_fem_compute_x_at_gauss_if_needed(element_t *e, unsigned int q, int integration, int condition) {
  return (condition) ? feenox_fem_compute_x_at_gauss(e, q, integration) : NULL;
}

inline double *feenox_fem_compute_x_at_gauss_if_needed_and_update_var(element_t *e, unsigned int q, int integration, int condition) {
  if (condition) {
    double *x = feenox_fem_compute_x_at_gauss(e, q, integration);
    feenox_fem_update_coord_vars(x);
    return x;
  }
  
  return NULL;
}

inline double *feenox_fem_compute_x_at_gauss_and_update_var(element_t *e, unsigned int q, int integration) {
  double *x = feenox_fem_compute_x_at_gauss(e, q, integration);
  feenox_fem_update_coord_vars(x);
  return x;
}


inline material_t *feenox_fem_get_material(element_t *e) {
  return (e->physical_group != NULL) ? e->physical_group->material : NULL;
}

inline double feenox_fem_compute_w_det_at_gauss(element_t *e, unsigned int q, int integration) {

  if (feenox.fem.cache_J == PETSC_FALSE && e->type != feenox.fem.current_gauss_type) {
    feenox_fem_elemental_caches_reset();
  }
  
  double **w = (feenox.fem.cache_J) ? &e->w : &feenox.fem.w;
  if ((*w) == NULL) {
    (*w) = calloc(e->type->gauss[integration].Q, sizeof(double));
    feenox.fem.current_gauss_type = e->type;
  } else if (((feenox.fem.current_weight_element_tag == e->tag && feenox.fem.current_weight_gauss_point == q) || feenox.fem.cache_J) && (*w)[q] != 0) {
    return (*w)[q];
  }
  
  gsl_matrix *J = feenox_fem_compute_J_at_gauss(e, q, integration);
  
  // TODO: choose to complain about zero or negative?
  // TODO: choose to take the absolute value or not? put these two as defines
  (*w)[q] = e->type->gauss[integration].w[q] * fabs(feenox_fem_determinant(J));
  
  feenox.fem.current_weight_element_tag = e->tag;
  feenox.fem.current_weight_gauss_point = q;
  
  return (*w)[q];
}

inline gsl_matrix *feenox_fem_compute_J_at_gauss_1d(element_t *e, unsigned int q, int integration, gsl_matrix *J) {
  
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
  
  if (J == NULL) {
    J = gsl_matrix_calloc(1,1);
  }
  gsl_matrix_set(J, 0, 0, dxdxi);
  return J;
}

inline gsl_matrix *feenox_fem_compute_J_at_gauss_2d(element_t *e, unsigned int q, int integration, gsl_matrix *J) {
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

//  double c = feenox_fem_dot(e->normal, e_z); // cosine
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
  
  if (J == NULL) {
    J = gsl_matrix_calloc(2,2);
  }
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

  return J;
}


inline gsl_matrix *feenox_fem_compute_J_at_gauss_general(element_t *e, unsigned int q, int integration, gsl_matrix *J) {
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
  
  gsl_matrix *C = feenox_fem_compute_C(e);
  if (J == NULL) {
    J = gsl_matrix_calloc(e->type->dim, e->type->dim);
  }

  gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1.0, C, e->type->gauss[integration].B_c[q], 0.0, J);
  
  return J;
}


// magic magic magic!
inline gsl_matrix *feenox_fem_compute_J_at_gauss(element_t *e, unsigned int q, int integration) {

  if (feenox.fem.cache_J == PETSC_FALSE && e->type != feenox.fem.current_gauss_type) {
    feenox_fem_elemental_caches_reset();
  }
  
  gsl_matrix ***J = (feenox.fem.cache_J) ? &e->J : &feenox.fem.Ji;
  if ((*J) == NULL) {
    feenox_check_alloc_null((*J) = calloc(e->type->gauss[integration].Q, sizeof(gsl_matrix *)));
    feenox.fem.current_gauss_type = e->type;
  }
  if ((*J)[q] == NULL) {
    (*J)[q] = gsl_matrix_calloc(e->type->dim, e->type->dim);
  } else if ((feenox.fem.current_gauss_element_tag == e->tag && feenox.fem.current_jacobian_gauss_point == q) || feenox.fem.cache_J) {
    return (*J)[q];
  }
  
  // TODO: once the problem type and dimension is set, we know which element type
  //       needs which kind of xixdr computation: we can then use some virtual
  //       functions defined in e->type
  if (e->type->dim == 1 && (e->node[0]->x[1] != 0 || e->node[1]->x[1] != 0 ||
                            e->node[0]->x[2] != 0 || e->node[1]->x[2] != 0)) {

    (*J)[q] = feenox_fem_compute_J_at_gauss_1d(e, q, integration, (*J)[q]);

  } else if (e->type->dim == 2 && (e->node[0]->x[2] != 0 || e->node[1]->x[2] != 0 || e->node[2]->x[2])) {

    feenox_mesh_compute_normal_2d(e);
    double eps = feenox_var_value(feenox.mesh.vars.eps);
    // ANDs are more efficient than ORs because the minute one does not hold the evaluation finishes
    if (fabs(e->normal[0]) < eps && fabs(e->normal[1]) < eps && fabs(fabs(e->normal[2])-1) < eps) {

      (*J)[q] = feenox_fem_compute_J_at_gauss_general(e, q, integration, (*J)[q]);

    } else {

      (*J)[q] = feenox_fem_compute_J_at_gauss_2d(e, q, integration, (*J)[q]);

    }

  } else {

    (*J)[q] = feenox_fem_compute_J_at_gauss_general(e, q, integration, (*J)[q]);
  }
  
  feenox.fem.current_jacobian_element_tag = e->tag;
  feenox.fem.current_jacobian_gauss_point = q;

  return (*J)[q];
}


inline double *feenox_fem_compute_x_at_gauss(element_t *e, unsigned int q, int integration) {

  if (feenox.fem.cache_J == 0 && e->type != feenox.fem.current_gauss_type) {
    feenox_fem_elemental_caches_reset();
  }
  
  double ***x = (feenox.fem.cache_J) ? &e->x : &feenox.fem.x;
  if ((*x) == NULL) {
    feenox_check_alloc_null((*x) = calloc(e->type->gauss[integration].Q, sizeof(double *)));
    feenox.fem.current_gauss_type = e->type;
  }
  if ((*x)[q] == NULL) {
    (*x)[q] = calloc(3, sizeof(double));
  } else if (feenox.fem.cache_J) {
    return (*x)[q];
  }
  
  gsl_matrix *H = feenox_fem_compute_H_c_at_gauss(e, q, integration);
  (*x)[q][0] = (*x)[q][1] = (*x)[q][2] = 0;
  for (unsigned int j = 0; j < e->type->nodes; j++) {
    double h = gsl_matrix_get(H, 0, j);
    for (unsigned int d = 0; d < 3; d++) {
      (*x)[q][d] += h * e->node[j]->x[d];
    }
  }
  
  return (*x)[q];
}

inline gsl_matrix *feenox_fem_compute_H_c_at_gauss(element_t *e, unsigned int q, int integration) {
  return e->type->gauss[integration].H_c[q];
}


inline gsl_matrix *feenox_fem_compute_H_Gc_at_gauss(element_t *e, unsigned int q, int integration) {
  
  if (e->type->H_Gc == NULL) {
    if (feenox.pde.dofs == 1) {
      e->type->H_Gc = e->type->gauss[integration].H_c;
      return e->type->H_Gc[q];
    } else {
      e->type->H_Gc = calloc(e->type->gauss[integration].Q, sizeof(gsl_matrix *));
    }
  }
  
  if (e->type->H_Gc[q] == NULL) {
    unsigned int J = e->type->nodes;
    unsigned int G = feenox.pde.dofs;
    e->type->H_Gc[q] = gsl_matrix_calloc(G, G*J);
  
    // TODO: measure order of loops
    for (unsigned int j = 0; j < J; j++) {
      double h = gsl_matrix_get(e->type->gauss[integration].H_c[q], 0, j);
      for (unsigned int g = 0; g < G; g++) {
        gsl_matrix_set(e->type->H_Gc[q], g, G*j+g, h);
      }
    }
  }

  return e->type->H_Gc[q];  
}

inline gsl_matrix *feenox_fem_compute_B_at_gauss(element_t *e, unsigned int q, int integration) {

  if (feenox.fem.cache_J == 0 && e->type != feenox.fem.current_gauss_type) {
    feenox_fem_elemental_caches_reset();
  }
  
  gsl_matrix ***B = (feenox.fem.cache_B) ? &e->B : &feenox.fem.Bi;
  if ((*B) == NULL) {
    feenox_check_alloc_null((*B) = calloc(e->type->gauss[integration].Q, sizeof(gsl_matrix *)));
    feenox.fem.current_gauss_type = e->type;
  }
  if ((*B)[q] == NULL) {
    (*B)[q] = gsl_matrix_calloc(e->type->dim, e->type->nodes);
  } else if (feenox.fem.cache_B) {
    return (*B)[q];
  }
  
  gsl_matrix *invJ = feenox_fem_compute_invJ_at_gauss(e, q, feenox.pde.mesh->integration);
  gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1.0, invJ, e->type->gauss[integration].B_c[q], 0.0, (*B)[q]);  
  return (*B)[q];
}  


inline gsl_matrix *feenox_fem_compute_B_G_at_gauss(element_t *e, unsigned int q, int integration) {

  unsigned int G = feenox.pde.dofs;
  if (G == 1) {
    // for G=1 B_G = B
    return feenox_fem_compute_B_at_gauss(e, q, integration);
  }
  gsl_matrix ***B_G = (feenox.fem.cache_B) ? &e->B_G : &feenox.fem.B_Gi;
  if ((*B_G) == NULL) {
    feenox_check_alloc_null((*B_G) = calloc(e->type->gauss[integration].Q, sizeof(gsl_matrix *)));
  }

  unsigned int J = e->type->nodes;
  unsigned int D = e->type->dim;

  if ((*B_G)[q] == NULL) {
    (*B_G)[q] = gsl_matrix_calloc(G*D, G*J);
  } else if (feenox.fem.cache_B) {
    return (*B_G)[q];
  }

  gsl_matrix *B = feenox_fem_compute_B_at_gauss(e, q, integration);
  for (unsigned int d = 0; d < D; d++) {
    size_t Gd = G*d;
    for (unsigned int j = 0; j < J; j++) {
      size_t Gj = G*j;
      double dhdxi = gsl_matrix_get(B, d, j);
      for (unsigned int g = 0; g < feenox.pde.dofs; g++) {
        gsl_matrix_set((*B_G)[q], Gd+g, Gj+g, dhdxi);
      }
    }
  }

  return (*B_G)[q];
}


#ifdef HAVE_PETSC
PetscInt *feenox_fem_compute_dof_indices(element_t *e, int G) {
  
  if (feenox.fem.l == NULL) {
    feenox_check_alloc_null(feenox.fem.l = calloc(G * e->type->nodes, sizeof(PetscInt)));
  }
  
  // the vector l contains the global indexes of each DOF in the element
  // note that this vector is always node major independently of the global ordering
  for (unsigned int j = 0; j < e->type->nodes; j++) {
    for (unsigned int g = 0; g < G; g++) {
      feenox.fem.l[G*j + g] = e->node[j]->index_dof[g];
    }  
  }
  
  return feenox.fem.l;
  
}
#endif

