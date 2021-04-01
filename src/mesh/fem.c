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
int feenox_mesh_jacobian_invert(gsl_matrix *direct, gsl_matrix *inverse) {

  double det = 0;
  double invdet = 0;

  switch (direct->size1) {
    case 1:
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


// calcula el determinante de una matriz de 2x2 //
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

//}

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
void feenox_mesh_compute_dhdx(element_t *this, double *r, gsl_matrix *drdx_ref, gsl_matrix *dhdx) {

  gsl_matrix *dxdr;
  gsl_matrix *drdx;
  int j, m, m_prime;
  
  if (drdx_ref != NULL) {
    // si ya nos dieron drdx usamos esa
    drdx = drdx_ref;
    
  } else {
    // sino la calculamos
    drdx = gsl_matrix_calloc(this->type->dim, this->type->dim);
    dxdr = gsl_matrix_calloc(this->type->dim, this->type->dim);
    
    feenox_mesh_compute_dxdr(this, r, dxdr);
    feenox_mesh_jacobian_invert(dxdr, drdx);
  }
  
  
  gsl_matrix_set_zero(dhdx);
  for (j = 0; j < this->type->nodes; j++) {
    for (m = 0; m < this->type->dim; m++) {
      for (m_prime = 0; m_prime < this->type->dim; m_prime++) {
        gsl_matrix_add_to_element(dhdx, j, m, this->type->dhdr(j, m_prime, r) * gsl_matrix_get(drdx, m_prime, m));
      }
    }
  }

  if (drdx_ref == NULL) {
    gsl_matrix_free(drdx);
    gsl_matrix_free(dxdr);
  }
  
  return;

}


// compute the gradient of the shape functions with respect to x evalauted at gauss point v of scheme integration
void mesh_compute_dhdx_at_gauss(element_t *this, int v, int integration) {

  int m, m_prime;    // dimensions
  int j;             // nodes
  int V_changed = 0;
  gsl_matrix *dhdx;
  
  
  if (this->dhdx == NULL || this->V_dhdx != this->type->gauss[integration].V) {
    int v_prime;
    for (v_prime = 0; v_prime < this->V_dhdx; v_prime++) {
      gsl_matrix_free(this->dhdx[v_prime]);
    }
    feenox_free(this->dhdx);

    this->V_dhdx = this->type->gauss[integration].V;
    this->dhdx = calloc(this->V_dhdx, sizeof(gsl_matrix *));
    V_changed = 1;
  }
  
  if (this->dhdx[v] == NULL) {
    this->dhdx[v] = gsl_matrix_calloc(this->type->nodes, this->type->dim);
  } else {
    return;
  }
  
  if (this->drdx == NULL || this->drdx[v] == NULL || V_changed) {
    feenox_mesh_compute_drdx_at_gauss(this, v, integration);
  }

  dhdx = this->dhdx[v];
  
  // TODO: matrix-matrix multiplication with blas?
  for (j = 0; j < this->type->nodes; j++) {
    for (m = 0; m < this->type->dim; m++) {
      for (m_prime = 0; m_prime < this->type->dim; m_prime++) {
      	gsl_matrix_add_to_element(dhdx, j, m, gsl_matrix_get(this->type->gauss[integration].dhdr[v], j, m_prime) * gsl_matrix_get(this->drdx[v], m_prime, m));
      }
    }
  }
  
#ifdef FEM_DUMP  
  printf("dhdx(%d,%d) = \n", this->index, v);
  gsl_matrix_fprintf(stdout, this->dhdx[v], "%g");  
#endif
  

  return;

}

void mesh_compute_h(element_t *this, double *r, double *h) {

  int j;

  for (j = 0; j < this->type->nodes; j++) {
    h[j] = this->type->h(j, r);
  }

  return;

}


void feenox_mesh_compute_drdx_at_gauss(element_t *this, int v, int integration) {
  
  int V_changed = 0;
  
  if (this->drdx == NULL || this->V_drdx != this->type->gauss[integration].V) {
    int v_prime;
    for (v_prime = 0; v_prime < this->V_drdx; v_prime++) {
      gsl_matrix_free(this->drdx[v_prime]);
    }
    feenox_free(this->drdx);

    this->V_drdx = this->type->gauss[integration].V;
    this->drdx = calloc(this->V_drdx, sizeof(gsl_matrix *));
    V_changed = 1;
  }
  if (this->drdx[v] == NULL) {
    this->drdx[v] = gsl_matrix_calloc(this->type->dim, this->type->dim);
  } else {
    return;
  }
  
  if (this->dxdr == NULL || this->dxdr[v] == NULL || V_changed) {
    feenox_mesh_compute_dxdr_at_gauss(this, v, integration);
  }
  
  feenox_mesh_jacobian_invert(this->dxdr[v], this->drdx[v]);

#ifdef FEM_DUMP  
  printf("drdx(%d,%d) = \n", this->index, v);
  gsl_matrix_fprintf(stdout, this->drdx[v], "%g");  
#endif
  
  return;
  
}



void feenox_mesh_compute_dxdr(element_t *this, double *r, gsl_matrix *dxdr) {

  int m, m_prime, j;
  
  // OJO! esto solo camina en elementos volumetricos, ver dxdr_at_gauss
  for (m = 0; m < this->type->dim; m++) {
    for (m_prime = 0; m_prime < this->type->dim; m_prime++) {
      for (j = 0; j < this->type->nodes; j++) {
        gsl_matrix_add_to_element(dxdr, m, m_prime, this->type->dhdr(j, m_prime, r) * this->node[j]->x[m]);
      }
    }
  }
  
  return;
}


void mesh_compute_x(element_t *this, double *r, double *x) {

  int j, m;

  
  // solo para elementos volumetricos
  for (m = 0; m < 3; m++) {
    x[m] = 0;
    for (j = 0; j < this->type->nodes; j++) {
      x[m] += this->type->h(j, r) * this->node[j]->x[m];
    }
  }

  return;
}

/*
double mesh_integration_weight(mesh_t *mesh, element_t *element, int v) {
  return 0;
}
*/

void mesh_compute_integration_weight_at_gauss(element_t *this, int v, int integration) {
  
  if (this->w == NULL) {
    this->w = calloc(this->type->gauss[integration].V, sizeof(double));
  }
  
  if (this->dxdr == NULL || this->dxdr[v] == NULL) {
    feenox_mesh_compute_dxdr_at_gauss(this, v, integration);
  }
  
  if (this->w[v] == 0) {
    this->w[v] = this->type->gauss[integration].w[v] * fabs(feenox_mesh_determinant(this->dxdr[v]));
  }  
#ifdef FEM_DUMP  
  printf("w(%d,%d) = %g\n", this->index, v, this->w[v]);
#endif

  return;
}



void feenox_mesh_compute_dxdr_at_gauss(element_t *this, int v, int integration) {

  int m, m_prime;
  int j;
//  int V_changed = 0;
  double *r;
  gsl_matrix *dxdr;
  
  if (this->dxdr == NULL || this->V_dxdr != this->type->gauss[integration].V) {
    int v_prime;
    for (v_prime = 0; v_prime < this->V_dxdr; v_prime++) {
      gsl_matrix_free(this->dxdr[v_prime]);
    }
    feenox_free(this->dxdr);

    this->V_dxdr = this->type->gauss[integration].V;
    this->dxdr = calloc(this->V_dxdr, sizeof(gsl_matrix *));
//    V_changed = 1;
  }
  
  if (this->dxdr[v] == NULL) {
    this->dxdr[v] = gsl_matrix_calloc(this->type->dim, this->type->dim);
  } else {
    return;
  }

  dxdr = this->dxdr[v];
  r = this->type->gauss[integration].r[v];

  if (this->type->dim == 0) {
    // a point does not have any derivative, if we are here then just keep silence
    ;
  } else if (this->type->dim == 1 && (this->node[0]->x[1] != 0 || this->node[1]->x[1] != 0 ||
                                      this->node[0]->x[2] != 0 || this->node[1]->x[2] != 0)) {
    // if we are a line but not aligned with the x axis we have to compute the axial coordinate l
    double dx = this->node[1]->x[0] - this->node[0]->x[0];
    double dy = this->node[1]->x[1] - this->node[0]->x[1];
    double dz = this->node[1]->x[2] - this->node[0]->x[2];
    double l = gsl_hypot3(dx, dy, dz);

    for (j = 0; j < this->type->nodes; j++) {
      gsl_matrix_add_to_element(dxdr, 0, 0, this->type->dhdr(j, 0, r) * (this->node[j]->x[0] * dx/l +
                                                                         this->node[j]->x[1] * dy/l +
                                                                         this->node[j]->x[2] * dz/l));
    }
    
  } else if (this->type->dim == 2 && (this->node[0]->x[2] != 0 || this->node[1]->x[2] != 0 || this->node[2]->x[2])) {
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
    feenox_mesh_subtract(this->node[0]->x, this->node[1]->x, a);
    feenox_mesh_subtract(this->node[0]->x, this->node[2]->x, b);
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
      for (j = 0; j < this->type->nodes; j++) {
        for (m_prime = 0; m_prime < 2; m_prime++) {
          gsl_matrix_add_to_element(dxdr, m, m_prime, this->type->dhdr(j, m, r) * (this->node[j]->x[0] * R[m_prime][0] +
                                                                                      this->node[j]->x[1] * R[m_prime][1] +
                                                                                      this->node[j]->x[2] * R[m_prime][2]));
        }
      }
    }
    
  } else {
    // we can do a full traditional computation
    // i.e. lines are in the x axis
    //      surfaces are on the xy plane
    //      volumes are always volumes!
    for (m = 0; m < this->type->dim; m++) {
      for (m_prime = 0; m_prime < this->type->dim; m_prime++) {
        for (j = 0; j < this->type->nodes; j++) {
          // TODO: matrix-vector product
          gsl_matrix_add_to_element(dxdr, m, m_prime, gsl_matrix_get(this->type->gauss[integration].dhdr[v], j, m_prime) * this->node[j]->x[m]);
        }
      }
    }
    
  } 

#ifdef FEM_DUMP  
  printf("dxdr(%d,%d) = \n", this->index, v);
  gsl_matrix_fprintf(stdout, this->dxdr[v], "%g");  
#endif
  
  return;
}


void feenox_mesh_compute_x_at_gauss(element_t *this, int v, int integration) {

  int j, m;
  
  if (this->x == NULL) {
    this->x = calloc(this->type->gauss[integration].V, sizeof(double *));
  }
  if (this->x[v] == NULL) {
    this->x[v] = calloc(3, sizeof(double));
  } else {
    return;
  }
  
  for (j = 0; j < this->type->nodes; j++) {
    for (m = 0; m < 3; m++) {
      this->x[v][m] += this->type->gauss[integration].h[v][j] * this->node[j]->x[m];
    }
  }
  
#ifdef FEM_DUMP  
  printf("x(%d,%d) = %g %g %g\n", this->index, v, this->x[v][0], this->x[v][1], this->x[v][2]);
#endif

  return;  
}


void mesh_compute_H_at_gauss(element_t *this, int v, int dofs, int integration) {
  int j;
  int d;

  if (this->H == NULL) {
    this->H = calloc(this->type->gauss[integration].V, sizeof(gsl_matrix *));
  }
  if (this->H[v] == NULL) {
    this->H[v] = gsl_matrix_calloc(dofs, dofs*this->type->nodes);
  } else {
    return;
  }
  
  for (d = 0; d < dofs; d++) {
    for (j = 0; j < this->type->nodes; j++) {
      gsl_matrix_set(this->H[v], d, dofs*j+d, this->type->gauss[integration].h[v][j]);
    }
  }
  
#ifdef FEM_DUMP  
  printf("H(%d,%d) = \n", this->index, v);
  gsl_matrix_fprintf(stdout, this->H[v], "%g");  
#endif
  
  


  return;
  
}

/*
void mesh_compute_B_at_gauss(element_t *element, int v, int dofs, int integration) {

  int m, d, j;  
  
  if (element->B == NULL) {
    element->B = calloc(element->type->gauss[integration].V, sizeof(gsl_matrix *));
  }
  if (element->B[v] == NULL) {
    element->B[v] = gsl_matrix_calloc(dofs*element->type->dim, dofs*element->type->nodes);
  } else {
    return;
  }
  
  if (element->dhdx == NULL || element->dhdx[v] == NULL) {
    mesh_compute_dhdx_at_gauss(element, v, integration);
  }
  
  
  for (m = 0; m < element->type->dim; m++) {
    for (d = 0; d < dofs; d++) {
      for (j = 0; j < element->type->nodes; j++) {
        gsl_matrix_set(element->B[v], dofs*m+d, dofs*j+d, gsl_matrix_get(element->dhdx[v], j, m));
      }
    }
  }
 
  return;
}
*/
void feenox_mesh_compute_l(mesh_t *mesh, element_t *element) {
  int j, d;
  
  if (element->l == NULL) {
    element->l = calloc(element->type->nodes * mesh->degrees_of_freedom, sizeof(double));
  } else {
    return;
  }
  
  // the vector l contains the global indexes of each DOF in the element
  // note that this vector is always node major independently of the global orderin
  for (j = 0; j < element->type->nodes; j++) {
    for (d = 0; d < mesh->degrees_of_freedom; d++) {
      element->l[mesh->degrees_of_freedom*j + d] = element->node[j]->index_dof[d];
    }  
  }
  
  return;
  
}

/*
inline int mesh_update_coord_vars(double *x) {
  feenox_var(feenox_mesh.vars.x) = x[0];
  feenox_var(feenox_mesh.vars.y) = x[1];
  feenox_var(feenox_mesh.vars.z) = x[2];    
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
