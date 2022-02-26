/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's mesh-related FEM routines
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
      if ((det = feenox_mesh_determinant(direct)) == 0) {
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
      if ((det = feenox_mesh_determinant(direct)) == 0) {
        feenox_push_error_message("singular 3x3 matrix");
        return FEENOX_ERROR;
      }
      invdet = 1.0/det;
      gsl_matrix_set(inverse, 0, 0, +invdet*(gsl_matrix_get(direct, 2, 2)*gsl_matrix_get(direct, 1, 1) -
                                             gsl_matrix_get(direct, 2, 1)*gsl_matrix_get(direct, 1, 2)));
      gsl_matrix_set(inverse, 0, 1, -invdet*(gsl_matrix_get(direct, 2, 2)*gsl_matrix_get(direct, 0, 1) -
                                             gsl_matrix_get(direct, 2, 1)*gsl_matrix_get(direct, 0, 2)));
      gsl_matrix_set(inverse, 0, 2, +invdet*(gsl_matrix_get(direct, 1, 2)*gsl_matrix_get(direct, 0, 1) -
                                             gsl_matrix_get(direct, 1, 1)*gsl_matrix_get(direct, 0, 2)));
      
      gsl_matrix_set(inverse, 1, 0, -invdet*(gsl_matrix_get(direct, 2, 2)*gsl_matrix_get(direct, 1, 0) -
                                             gsl_matrix_get(direct, 2, 0)*gsl_matrix_get(direct, 1, 2)));
      gsl_matrix_set(inverse, 1, 1, +invdet*(gsl_matrix_get(direct, 2, 2)*gsl_matrix_get(direct, 0, 0) -
                                             gsl_matrix_get(direct, 2, 0)*gsl_matrix_get(direct, 0, 2)));
      gsl_matrix_set(inverse, 1, 2, -invdet*(gsl_matrix_get(direct, 1, 2)*gsl_matrix_get(direct, 0, 0) -
                                             gsl_matrix_get(direct, 1, 0)*gsl_matrix_get(direct, 0, 2)));

      gsl_matrix_set(inverse, 2, 0, +invdet*(gsl_matrix_get(direct, 2, 1)*gsl_matrix_get(direct, 1, 0) -
                                             gsl_matrix_get(direct, 2, 0)*gsl_matrix_get(direct, 1, 1)));
      gsl_matrix_set(inverse, 2, 1, -invdet*(gsl_matrix_get(direct, 2, 1)*gsl_matrix_get(direct, 0, 0) -
                                             gsl_matrix_get(direct, 2, 0)*gsl_matrix_get(direct, 0, 1)));
      gsl_matrix_set(inverse, 2, 2, +invdet*(gsl_matrix_get(direct, 1, 1)*gsl_matrix_get(direct, 0, 0) -
                                             gsl_matrix_get(direct, 1, 0)*gsl_matrix_get(direct, 0, 1)));
      break;
    default:
      feenox_push_error_message("invalid size %d of matrix to invert", direct->size1);
      break;
  }

/*  
static void matrix_inverse_3x3(PetscScalar A[3][3],PetscScalar B[3][3])
{
  PetscScalar t4, t6, t8, t10, t12, t14, t17;

  t4  = A[2][0] * A[0][1];
  t6  = A[2][0] * A[0][2];
  t8  = A[1][0] * A[0][1];
  t10 = A[1][0] * A[0][2];
  t12 = A[0][0] * A[1][1];
  t14 = A[0][0] * A[1][2];
  t17 = 0.1e1 / (t4 * A[1][2] - t6 * A[1][1] - t8 * A[2][2] + t10 * A[2][1] + t12 * A[2][2] - t14 * A[2][1]);

  B[0][0] = (A[1][1] * A[2][2] - A[1][2] * A[2][1]) * t17;
  B[0][1] = -(A[0][1] * A[2][2] - A[0][2] * A[2][1]) * t17;
  B[0][2] = (A[0][1] * A[1][2] - A[0][2] * A[1][1]) * t17;
  B[1][0] = -(-A[2][0] * A[1][2] + A[1][0] * A[2][2]) * t17;
  B[1][1] = (-t6 + A[0][0] * A[2][2]) * t17;
  B[1][2] = -(-t10 + t14) * t17;
  B[2][0] = (-A[2][0] * A[1][1] + A[1][0] * A[2][1]) * t17;
  B[2][1] = -(-t4 + A[0][0] * A[2][1]) * t17;
  B[2][2] = (-t8 + t12) * t17;
}
  
  return;
*/
  
  return FEENOX_OK;
}


double feenox_mesh_determinant(gsl_matrix *this) {

  switch (this->size1) {
    case 0:
      return 1.0;
    break;
    case 1:
      return gsl_matrix_get(this, 0, 0);
    break;
    case 2:
      return + gsl_matrix_get(this, 0, 0)*gsl_matrix_get(this, 1, 1)
             - gsl_matrix_get(this, 0, 1)*gsl_matrix_get(this, 1, 0);
    break;
    case 3:
      return + gsl_matrix_get(this, 0, 0)*gsl_matrix_get(this, 1, 1)*gsl_matrix_get(this, 2, 2)
             + gsl_matrix_get(this, 0, 1)*gsl_matrix_get(this, 1, 2)*gsl_matrix_get(this, 2, 0)
             + gsl_matrix_get(this, 0, 2)*gsl_matrix_get(this, 1, 0)*gsl_matrix_get(this, 2, 1) 
             - gsl_matrix_get(this, 0, 2)*gsl_matrix_get(this, 1, 1)*gsl_matrix_get(this, 2, 0)
             - gsl_matrix_get(this, 0, 1)*gsl_matrix_get(this, 1, 0)*gsl_matrix_get(this, 2, 2) 
             - gsl_matrix_get(this, 0, 0)*gsl_matrix_get(this, 1, 2)*gsl_matrix_get(this, 2, 1);  
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
    drdx = gsl_matrix_calloc(e->type->dim, e->type->dim);
    dxdr = gsl_matrix_calloc(e->type->dim, e->type->dim);
    
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
int feenox_mesh_compute_dhdx_at_gauss(element_t *e, int v, int integration) {

  gsl_matrix *dhdx;
  
  if (e->dhdx == NULL) {
    feenox_check_alloc(e->dhdx = calloc(e->type->gauss[integration].V, sizeof(gsl_matrix *)));
  }
  
  if (e->dhdx[v] == NULL) {
    feenox_check_alloc(e->dhdx[v] = gsl_matrix_calloc(e->type->nodes, e->type->dim));
  } else {
    return FEENOX_OK;
  }
  
  if (e->drdx == NULL || e->drdx[v] == NULL) {
    feenox_call(feenox_mesh_compute_drdx_at_gauss(e, v, integration));
  }

  dhdx = e->dhdx[v];
  
  // TODO: matrix-matrix multiplication with blas?
  unsigned int j, m, m_prime;
  for (j = 0; j < e->type->nodes; j++) {
    for (m = 0; m < e->type->dim; m++) {
      for (m_prime = 0; m_prime < e->type->dim; m_prime++) {
      	gsl_matrix_add_to_element(dhdx, j, m, gsl_matrix_get(e->type->gauss[integration].dhdr[v], j, m_prime) * gsl_matrix_get(e->drdx[v], m_prime, m));
      }
    }
  }  

  return FEENOX_OK;

}

int feenox_mesh_compute_h(element_t *e, double *r, double *h) {

  unsigned int j;
  for (j = 0; j < e->type->nodes; j++) {
    h[j] = e->type->h(j, r);
  }

  return FEENOX_OK;

}


int feenox_mesh_compute_drdx_at_gauss(element_t *e, unsigned int v, int integration) {
  
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



int feenox_mesh_compute_dxdr(element_t *e, double *r, gsl_matrix *dxdr) {

  // warning! ths only works with volumetric elements, see dxdr_at_gauss()
  unsigned int m, m_prime, j;
  for (m = 0; m < e->type->dim; m++) {
    for (m_prime = 0; m_prime < e->type->dim; m_prime++) {
      for (j = 0; j < e->type->nodes; j++) {
        gsl_matrix_add_to_element(dxdr, m, m_prime, e->type->dhdr(j, m_prime, r) * e->node[j]->x[m]);
      }
    }
  }
  
  return FEENOX_OK;
}


void mesh_compute_x(element_t *e, double *r, double *x) {

  // only for volumetric elements
  unsigned int m, j;
  for (m = 0; m < 3; m++) {
    x[m] = 0;
    for (j = 0; j < e->type->nodes; j++) {
      x[m] += e->type->h(j, r) * e->node[j]->x[m];
    }
  }

  return;
}

int feenox_mesh_compute_w_at_gauss(element_t *e, unsigned int v, int integration) {

  if (e->w == NULL) {
    feenox_check_alloc(e->w = calloc(e->type->gauss[integration].V, sizeof(double)));
  }
  
  if (e->dxdr == NULL || e->dxdr[v] == NULL) {
    feenox_call(feenox_mesh_compute_dxdr_at_gauss(e, v, integration));
  }
  
  if (e->w[v] == 0) {
    e->w[v] = e->type->gauss[integration].w[v] * fabs(feenox_mesh_determinant(e->dxdr[v]));
  }  

  return FEENOX_OK;
}



int feenox_mesh_compute_dxdr_at_gauss(element_t *e, unsigned int v, int integration) {

  int m, m_prime;
  int j;
//  int V_changed = 0;
  double *r;
  gsl_matrix *dxdr;
  
  if (e->dxdr == NULL) {
    feenox_check_alloc(e->dxdr = calloc(e->type->gauss[integration].V, sizeof(gsl_matrix *)));
  }
  
  if (e->dxdr[v] == NULL) {
    feenox_check_alloc(e->dxdr[v] = gsl_matrix_calloc(e->type->dim, e->type->dim));
  } else {
    return FEENOX_OK;
  }

  dxdr = e->dxdr[v];
  r = e->type->gauss[integration].r[v];

  if (e->type->dim == 0) {
    // a point does not have any derivative, if we are here then just keep silence
    ;
  } else if (e->type->dim == 1 && (e->node[0]->x[1] != 0 || e->node[1]->x[1] != 0 ||
                                      e->node[0]->x[2] != 0 || e->node[1]->x[2] != 0)) {
    // if we are a line but not aligned with the x axis we have to compute the axial coordinate l
    double dx = e->node[1]->x[0] - e->node[0]->x[0];
    double dy = e->node[1]->x[1] - e->node[0]->x[1];
    double dz = e->node[1]->x[2] - e->node[0]->x[2];
    double l = gsl_hypot3(dx, dy, dz);

    for (j = 0; j < e->type->nodes; j++) {
      gsl_matrix_add_to_element(dxdr, 0, 0, e->type->dhdr(j, 0, r) * (e->node[j]->x[0] * dx/l +
                                                                         e->node[j]->x[1] * dy/l +
                                                                         e->node[j]->x[2] * dz/l));
    }
    
  } else if (e->type->dim == 2 && (e->node[0]->x[2] != 0 || e->node[1]->x[2] != 0 || e->node[2]->x[2])) {
    // if we are a triangle or a quadrangle (quadrangles are double-triangles so they are alike)
    // but we do not live on the x-y plane we have to do some tricks
    double n[3];
    double xy[3];
    double a[3];
    double b[3];
    double v[3];
    double R[3][3];
    double s, c, k;
    
    // http://math.stackexchange.com/questions/180418/calculate-rotation-matrix-to-align-vector-a-to-vector-b-in-3d
    feenox_mesh_subtract(e->node[0]->x, e->node[1]->x, a);
    feenox_mesh_subtract(e->node[0]->x, e->node[2]->x, b);
    feenox_mesh_normalized_cross(a, b, n);
    xy[0] = 0;
    xy[1] = 0;
    xy[2] = 1;

    feenox_mesh_cross(n, xy, v);
    s = gsl_hypot3(v[0], v[1], v[2]);

/*
 (%i4) V:matrix([0, -v2, v1],[v2, 0, -v0],[-v1, v0, 0]);
                             [  0    - v2   v1  ]
                             [                  ]
(%o4)                        [  v2    0    - v0 ]
                             [                  ]
                             [ - v1   v0    0   ]
(%i5) V . V;
                   [     2     2                           ]
                   [ - v2  - v1      v0 v1        v0 v2    ]
                   [                                       ]
(%o5)              [                  2     2              ]
                   [    v0 v1     - v2  - v0      v1 v2    ]
                   [                                       ]
                   [                               2     2 ]
                   [    v0 v2        v1 v2     - v1  - v0  ]
*/    

    if (fabs(s) < feenox_var_value(feenox.mesh.vars.eps)) {
      // too late, but the transformation is a translation
      // TODO: it's not too late! just move the blocks around
      R[0][0] = 1;
      R[0][1] = 0;
      R[0][2] = 0;
      R[1][0] = 0;
      R[1][1] = 1;
      R[1][2] = 0;
      R[2][0] = 0;
      R[2][1] = 0;
      R[2][2] = 1;
    } else {
      c = feenox_mesh_dot(n, xy);
      k = (1-c)/(s*s);
      R[0][0] = 1     + k * (-v[2]*v[2] - v[1]*v[1]);
      R[0][1] = -v[2] + k * (v[0]*v[1]);
      R[0][2] = +v[1] + k * (v[0]*v[2]);
      R[1][0] = +v[2] + k * (v[0]*v[1]);
      R[1][1] = 1     + k * (-v[2]*v[2] - v[0]*v[0]);
      R[1][2] = -v[0] + k * (v[1]*v[2]);
      R[2][0] = -v[1] + k * (v[0]*v[2]);
      R[2][1] = +v[0] + k * (v[1]*v[2]);
      R[2][2] = 1     + k * (-v[1]*v[1] - v[0]*v[0]);
    }
    
    for (m = 0; m < 2; m++) {
      for (j = 0; j < e->type->nodes; j++) {
        for (m_prime = 0; m_prime < 2; m_prime++) {
          gsl_matrix_add_to_element(dxdr, m, m_prime, e->type->dhdr(j, m, r) * (e->node[j]->x[0] * R[m_prime][0] +
                                                                                      e->node[j]->x[1] * R[m_prime][1] +
                                                                                      e->node[j]->x[2] * R[m_prime][2]));
        }
      }
    }
    
  } else {
    // we can do a full traditional computation
    // i.e. lines are in the x axis
    //      surfaces are on the xy plane
    //      volumes are always volumes!
    for (m = 0; m < e->type->dim; m++) {
      for (m_prime = 0; m_prime < e->type->dim; m_prime++) {
        for (j = 0; j < e->type->nodes; j++) {
          // TODO: matrix-vector product
          gsl_matrix_add_to_element(dxdr, m, m_prime, gsl_matrix_get(e->type->gauss[integration].dhdr[v], j, m_prime) * e->node[j]->x[m]);
        }
      }
    }
    
  } 
  
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
  if (e->H[v] == NULL) {
    feenox_check_alloc(e->H[v] = gsl_matrix_calloc(feenox.pde.dofs, feenox.pde.dofs*e->type->nodes));
  } else {
    return FEENOX_OK;
  }
  
  for (unsigned int d = 0; d < feenox.pde.dofs; d++) {
    for (unsigned int j = 0; j < e->type->nodes; j++) {
      gsl_matrix_set(e->H[v], d, feenox.pde.dofs*j+d, e->type->gauss[integration].h[v][j]);
    }
  }

  return FEENOX_OK;  
}


int feenox_mesh_compute_B_at_gauss(element_t *element, unsigned int v, unsigned int dofs, int integration) {

  if (element->B == NULL) {
    feenox_check_alloc(element->B = calloc(element->type->gauss[integration].V, sizeof(gsl_matrix *)));
  }
  if (element->B[v] == NULL) {
    feenox_check_alloc(element->B[v] = gsl_matrix_calloc(dofs*element->type->dim, dofs*element->type->nodes));
  } else {
    return FEENOX_OK;
  }
  
  if (element->dhdx == NULL || element->dhdx[v] == NULL) {
    feenox_call(feenox_mesh_compute_dhdx_at_gauss(element, v, integration));
  }
  
  
  for (unsigned int m = 0; m < element->type->dim; m++) {
    for (unsigned int g = 0; g < dofs; g++) {
      for (unsigned int j = 0; j < element->type->nodes; j++) {
        gsl_matrix_set(element->B[v], dofs*m+g, dofs*j+g, gsl_matrix_get(element->dhdx[v], j, m));
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
