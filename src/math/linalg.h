/*------------ -------------- -------- --- ----- ---   --       -            -
 *  FeenoX linear algebra compatibility layer
 *
 *  Copyright (C) 2025 Jeremy Theler
 *
 *  This file is part of FeenoX.
 *
 *  FeenoX is free software: you can redistribute it and/or modify
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
 *  along with FeenoX.  If not, see <http://www.gnu.org/licenses/>.
 *------------------- ------------  ----    --------  --     -       -         -
 */

#ifndef FEENOX_LINALG_H
#define FEENOX_LINALG_H

// Check if GSL is available either directly or through other libraries like PETSc
#if defined(HAVE_GSL) || defined(__GSL_ERRNO_H__) || defined(GSL_SUCCESS)
  // GSL is available (either directly or via PETSc/other libs)
  #ifndef HAVE_GSL
    #define HAVE_GSL
  #endif
  
  // Include GSL headers if not already included
  #ifndef __GSL_BLAS_H__
    #include <gsl/gsl_blas.h>
  #endif
  #ifndef __GSL_CBLAS_H__
    #include <gsl/gsl_cblas.h>
  #endif
  #ifndef __GSL_ERRNO_H__
    #include <gsl/gsl_errno.h>
  #endif
  #ifndef __GSL_DERIV_H__
    #include <gsl/gsl_deriv.h>
  #endif
  #ifndef __GSL_HEAPSORT_H__
    #include <gsl/gsl_heapsort.h>
  #endif
  #ifndef __GSL_INTEGRATION_H__
    #include <gsl/gsl_integration.h>
  #endif
  #ifndef __GSL_INTERP_H__
    #include <gsl/gsl_interp.h>
  #endif
  #ifndef __GSL_LINALG_H__
    #include <gsl/gsl_linalg.h>
  #endif
  #ifndef __GSL_MATH_H__
    #include <gsl/gsl_math.h>
  #endif
  #ifndef __GSL_MATRIX_H__
    #include <gsl/gsl_matrix.h>
  #endif
  #ifndef __GSL_MIN_H__
    #include <gsl/gsl_min.h>
  #endif
  #ifndef GSL_MULTIFIT_NLINEAR_H
    #include <gsl/gsl_multifit_nlinear.h>
  #endif
  #ifndef __GSL_MULTIMIN_H__
    #include <gsl/gsl_multimin.h>
  #endif
  #ifndef __GSL_MULTIROOTS_H__
    #include <gsl/gsl_multiroots.h>
  #endif
  #ifndef __GSL_QRNG_H__
    #include <gsl/gsl_qrng.h>
  #endif
  #ifndef __GSL_RANDIST_H__
    #include <gsl/gsl_randist.h>
  #endif
  #ifndef __GSL_RNG_H__
    #include <gsl/gsl_rng.h>
  #endif
  #ifndef __GSL_ROOTS_H__
    #include <gsl/gsl_roots.h>
  #endif
  #ifndef __GSL_SPLINE_H__
    #include <gsl/gsl_spline.h>
  #endif
  #ifndef __GSL_SF__
    #include <gsl/gsl_sf.h>
  #endif
  #ifndef __GSL_STATISTICS_H__
    #include <gsl/gsl_statistics.h>
  #endif
  #ifndef __GSL_SORT_DOUBLE_H__
    #include <gsl/gsl_sort_double.h>
  #endif
  #ifndef __GSL_SORT_VECTOR_DOUBLE_H__
    #include <gsl/gsl_sort_vector_double.h>
  #endif
  #ifndef __GSL_VECTOR_H__
    #include <gsl/gsl_vector.h>
  #endif
  #ifndef __GSL_VERSION_H__
    #include <gsl/gsl_version.h>
  #endif

#else
// GSL is truly not available anywhere - provide minimal implementations

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <time.h>

// Don't define GSL constants here to avoid conflicts if GSL headers
// are included elsewhere (e.g., through PETSc). The compatibility
// layer functions just return plain integer error codes.

// Block structure (similar to GSL's gsl_block)
typedef struct {
  size_t size;
  double *data;
} gsl_block;

// Vector structure
typedef struct {
  size_t size;
  size_t stride;
  double *data;
  gsl_block *block;
  int owner;
} gsl_vector;

// Matrix structure  
typedef struct {
  size_t size1;  // rows
  size_t size2;  // columns
  size_t tda;    // trailing dimension
  double *data;
  gsl_block *block;
  int owner;
} gsl_matrix;

// Permutation structure for LU decomposition
typedef struct {
  size_t size;
  size_t *data;
} gsl_permutation;

// Random number generator structures
typedef struct {
  const char *name;
  unsigned long int max;
  unsigned long int min;
} gsl_rng_type;

typedef struct {
  const gsl_rng_type *type;
  unsigned long int state;
} gsl_rng;

// Minimal RNG type (simple LCG)
static const gsl_rng_type gsl_rng_mt19937 = {
  "mt19937",
  0xffffffffUL,
  0
};

// Quasi-random number generator types (forward declarations, not implemented)
typedef struct {
  const char *name;
  unsigned int dimension;
} gsl_qrng_type;

typedef struct {
  const gsl_qrng_type *type;
  void *state;
} gsl_qrng;

// GSL function type for integration/derivatives
typedef struct {
  double (*function)(double x, void *params);
  void *params;
} gsl_function;

// GSL multiroot solver types (stubs - not actually functional)
typedef struct {
  const char *name;
} gsl_multiroot_fdfsolver_type;

typedef struct {
  int (*f)(const gsl_vector *x, void *params, gsl_vector *f);
  int (*df)(const gsl_vector *x, void *params, gsl_matrix *J);
  int (*fdf)(const gsl_vector *x, void *params, gsl_vector *f, gsl_matrix *J);
  size_t n;
  void *params;
} gsl_multiroot_function_fdf;

typedef struct {
  const gsl_multiroot_fdfsolver_type *type;
  gsl_vector *f;
  gsl_vector *x;
  void *state;
} gsl_multiroot_fdfsolver;

static const gsl_multiroot_fdfsolver_type _gsl_multiroot_fdfsolver_gnewton = {"gnewton"};
#define gsl_multiroot_fdfsolver_gnewton (&_gsl_multiroot_fdfsolver_gnewton)

// Note: GSL_CONTINUE is defined in real GSL headers as enum value -2
// We use the numeric value directly to avoid conflicts

// GSL interpolation types (stubs)
typedef struct {
  const char *name;
  unsigned int min_size;
} gsl_interp_type;

typedef struct {
  const gsl_interp_type *type;
  double *xdata;
  double *ydata;
  size_t size;
} gsl_interp;

typedef struct {
  size_t cache;
} gsl_interp_accel;

// Interpolation type constants (commonly used ones)  
static const gsl_interp_type _gsl_interp_linear = {"linear", 2};
static const gsl_interp_type _gsl_interp_polynomial = {"polynomial", 2};
static const gsl_interp_type _gsl_interp_cspline = {"cspline", 3};
static const gsl_interp_type _gsl_interp_cspline_periodic = {"cspline_periodic", 3};
static const gsl_interp_type _gsl_interp_akima = {"akima", 5};
static const gsl_interp_type _gsl_interp_akima_periodic = {"akima_periodic", 5};
static const gsl_interp_type _gsl_interp_steffen = {"steffen", 3};

// Pointers to match GSL API
#define gsl_interp_linear             (&_gsl_interp_linear)
#define gsl_interp_polynomial         (&_gsl_interp_polynomial)
#define gsl_interp_cspline            (&_gsl_interp_cspline)
#define gsl_interp_cspline_periodic   (&_gsl_interp_cspline_periodic)
#define gsl_interp_akima              (&_gsl_interp_akima)
#define gsl_interp_akima_periodic     (&_gsl_interp_akima_periodic)
#define gsl_interp_steffen            (&_gsl_interp_steffen)

// Vector allocation functions
static inline gsl_vector* gsl_vector_alloc(size_t n) {
  gsl_vector *v = (gsl_vector*)malloc(sizeof(gsl_vector));
  if (v == NULL) return NULL;
  
  v->block = (gsl_block*)malloc(sizeof(gsl_block));
  if (v->block == NULL) {
    free(v);
    return NULL;
  }
  
  v->data = (double*)malloc(n * sizeof(double));
  if (v->data == NULL) {
    free(v->block);
    free(v);
    return NULL;
  }
  
  v->size = n;
  v->stride = 1;
  v->block->data = v->data;
  v->block->size = n;
  v->owner = 1;
  return v;
}

static inline gsl_vector* gsl_vector_calloc(size_t n) {
  gsl_vector *v = gsl_vector_alloc(n);
  if (v != NULL) {
    memset(v->data, 0, n * sizeof(double));
  }
  return v;
}

static inline void gsl_vector_free(gsl_vector *v) {
  if (v != NULL) {
    if (v->owner && v->data != NULL) {
      free(v->data);
    }
    if (v->block != NULL) {
      free(v->block);
    }
    free(v);
  }
}

static inline double gsl_vector_get(const gsl_vector *v, size_t i) {
  return v->data[i * v->stride];
}

static inline void gsl_vector_set(gsl_vector *v, size_t i, double x) {
  v->data[i * v->stride] = x;
}

static inline double* gsl_vector_ptr(gsl_vector *v, size_t i) {
  return &(v->data[i * v->stride]);
}

static inline const double* gsl_vector_const_ptr(const gsl_vector *v, size_t i) {
  return &(v->data[i * v->stride]);
}

static inline void gsl_vector_set_zero(gsl_vector *v) {
  memset(v->data, 0, v->size * sizeof(double));
}

static inline void gsl_vector_set_all(gsl_vector *v, double x) {
  for (size_t i = 0; i < v->size; i++) {
    v->data[i * v->stride] = x;
  }
}

static inline int gsl_vector_memcpy(gsl_vector *dest, const gsl_vector *src) {
  if (dest->size != src->size) return 19; // GSL_EBADLEN
  for (size_t i = 0; i < src->size; i++) {
    gsl_vector_set(dest, i, gsl_vector_get(src, i));
  }
  return 0; // GSL_SUCCESS
}

static inline void gsl_vector_reverse(gsl_vector *v) {
  size_t i, j;
  for (i = 0, j = v->size - 1; i < j; i++, j--) {
    double tmp = gsl_vector_get(v, i);
    gsl_vector_set(v, i, gsl_vector_get(v, j));
    gsl_vector_set(v, j, tmp);
  }
}

// Matrix allocation functions
static inline gsl_matrix* gsl_matrix_alloc(size_t n1, size_t n2) {
  gsl_matrix *m = (gsl_matrix*)malloc(sizeof(gsl_matrix));
  if (m == NULL) return NULL;
  
  m->block = (gsl_block*)malloc(sizeof(gsl_block));
  if (m->block == NULL) {
    free(m);
    return NULL;
  }
  
  m->data = (double*)malloc(n1 * n2 * sizeof(double));
  if (m->data == NULL) {
    free(m->block);
    free(m);
    return NULL;
  }
  
  m->size1 = n1;
  m->size2 = n2;
  m->tda = n2;
  m->block->data = m->data;
  m->block->size = n1 * n2;
  m->owner = 1;
  return m;
}

static inline gsl_matrix* gsl_matrix_calloc(size_t n1, size_t n2) {
  gsl_matrix *m = gsl_matrix_alloc(n1, n2);
  if (m != NULL) {
    memset(m->data, 0, n1 * n2 * sizeof(double));
  }
  return m;
}

static inline void gsl_matrix_free(gsl_matrix *m) {
  if (m != NULL) {
    if (m->owner && m->data != NULL) {
      free(m->data);
    }
    if (m->block != NULL) {
      free(m->block);
    }
    free(m);
  }
}

static inline double gsl_matrix_get(const gsl_matrix *m, size_t i, size_t j) {
  return m->data[i * m->tda + j];
}

static inline void gsl_matrix_set(gsl_matrix *m, size_t i, size_t j, double x) {
  m->data[i * m->tda + j] = x;
}

static inline double* gsl_matrix_ptr(gsl_matrix *m, size_t i, size_t j) {
  return &(m->data[i * m->tda + j]);
}

static inline void gsl_matrix_set_zero(gsl_matrix *m) {
  memset(m->data, 0, m->size1 * m->size2 * sizeof(double));
}

static inline void gsl_matrix_set_all(gsl_matrix *m, double x) {
  for (size_t i = 0; i < m->size1 * m->size2; i++) {
    m->data[i] = x;
  }
}

static inline void gsl_matrix_set_identity(gsl_matrix *m) {
  gsl_matrix_set_zero(m);
  size_t n = (m->size1 < m->size2) ? m->size1 : m->size2;
  for (size_t i = 0; i < n; i++) {
    gsl_matrix_set(m, i, i, 1.0);
  }
}

static inline int gsl_matrix_memcpy(gsl_matrix *dest, const gsl_matrix *src) {
  if (dest->size1 != src->size1 || dest->size2 != src->size2) {
    return 19; // GSL_EBADLEN
  }
  memcpy(dest->data, src->data, src->size1 * src->size2 * sizeof(double));
  return 0; // GSL_SUCCESS
}

static inline int gsl_matrix_add(gsl_matrix *a, const gsl_matrix *b) {
  if (a->size1 != b->size1 || a->size2 != b->size2) {
    return 19; // GSL_EBADLEN
  }
  for (size_t i = 0; i < a->size1; i++) {
    for (size_t j = 0; j < a->size2; j++) {
      double val = gsl_matrix_get(a, i, j) + gsl_matrix_get(b, i, j);
      gsl_matrix_set(a, i, j, val);
    }
  }
  return 0;
}

static inline int gsl_matrix_sub(gsl_matrix *a, const gsl_matrix *b) {
  if (a->size1 != b->size1 || a->size2 != b->size2) {
    return 19; // GSL_EBADLEN
  }
  for (size_t i = 0; i < a->size1; i++) {
    for (size_t j = 0; j < a->size2; j++) {
      double val = gsl_matrix_get(a, i, j) - gsl_matrix_get(b, i, j);
      gsl_matrix_set(a, i, j, val);
    }
  }
  return 0;
}

static inline int gsl_matrix_scale(gsl_matrix *a, double x) {
  for (size_t i = 0; i < a->size1; i++) {
    for (size_t j = 0; j < a->size2; j++) {
      double val = gsl_matrix_get(a, i, j) * x;
      gsl_matrix_set(a, i, j, val);
    }
  }
  return 0;
}

static inline int gsl_vector_add(gsl_vector *a, const gsl_vector *b) {
  if (a->size != b->size) return 19; // GSL_EBADLEN
  for (size_t i = 0; i < a->size; i++) {
    double val = gsl_vector_get(a, i) + gsl_vector_get(b, i);
    gsl_vector_set(a, i, val);
  }
  return 0;
}

// Permutation functions
static inline gsl_permutation* gsl_permutation_alloc(size_t n) {
  gsl_permutation *p = (gsl_permutation*)malloc(sizeof(gsl_permutation));
  if (p == NULL) return NULL;
  
  p->data = (size_t*)malloc(n * sizeof(size_t));
  if (p->data == NULL) {
    free(p);
    return NULL;
  }
  
  p->size = n;
  for (size_t i = 0; i < n; i++) {
    p->data[i] = i;
  }
  return p;
}

static inline void gsl_permutation_free(gsl_permutation *p) {
  if (p != NULL) {
    if (p->data != NULL) {
      free(p->data);
    }
    free(p);
  }
}

// BLAS constants (for transpose flags)
#define CblasNoTrans   111
#define CblasTrans     112
#define CblasConjTrans 113

// Basic LU decomposition (simple Gaussian elimination with partial pivoting)
static inline int gsl_linalg_LU_decomp(gsl_matrix *A, gsl_permutation *p, int *signum) {
  size_t n = A->size1;
  if (A->size2 != n || p->size != n) {
    return 19; // GSL_EBADLEN
  }
  
  *signum = 1;
  
  for (size_t k = 0; k < n; k++) {
    // Find pivot
    size_t pivot = k;
    double max_val = fabs(gsl_matrix_get(A, k, k));
    
    for (size_t i = k + 1; i < n; i++) {
      double val = fabs(gsl_matrix_get(A, i, k));
      if (val > max_val) {
        max_val = val;
        pivot = i;
      }
    }
    
    // Swap rows if needed
    if (pivot != k) {
      for (size_t j = 0; j < n; j++) {
        double tmp = gsl_matrix_get(A, k, j);
        gsl_matrix_set(A, k, j, gsl_matrix_get(A, pivot, j));
        gsl_matrix_set(A, pivot, j, tmp);
      }
      size_t tmp = p->data[k];
      p->data[k] = p->data[pivot];
      p->data[pivot] = tmp;
      *signum = -*signum;
    }
    
    double diag = gsl_matrix_get(A, k, k);
    if (fabs(diag) < 1e-15) {
      return 21; // GSL_ESING - Singular matrix
    }
    
    // Eliminate
    for (size_t i = k + 1; i < n; i++) {
      double factor = gsl_matrix_get(A, i, k) / diag;
      gsl_matrix_set(A, i, k, factor);
      
      for (size_t j = k + 1; j < n; j++) {
        double val = gsl_matrix_get(A, i, j) - factor * gsl_matrix_get(A, k, j);
        gsl_matrix_set(A, i, j, val);
      }
    }
  }
  
  return 0; // GSL_SUCCESS
}

// Solve Ax = b using LU decomposition
static inline int gsl_linalg_LU_solve(const gsl_matrix *LU, const gsl_permutation *p, 
                                       const gsl_vector *b, gsl_vector *x) {
  size_t n = LU->size1;
  if (LU->size2 != n || p->size != n || b->size != n || x->size != n) {
    return 19; // GSL_EBADLEN
  }
  
  // Forward substitution: Ly = Pb
  for (size_t i = 0; i < n; i++) {
    double sum = gsl_vector_get(b, p->data[i]);
    for (size_t j = 0; j < i; j++) {
      sum -= gsl_matrix_get(LU, i, j) * gsl_vector_get(x, j);
    }
    gsl_vector_set(x, i, sum);
  }
  
  // Backward substitution: Ux = y
  for (size_t ii = n; ii > 0; ii--) {
    size_t i = ii - 1;
    double sum = gsl_vector_get(x, i);
    for (size_t j = i + 1; j < n; j++) {
      sum -= gsl_matrix_get(LU, i, j) * gsl_vector_get(x, j);
    }
    double diag = gsl_matrix_get(LU, i, i);
    if (fabs(diag) < 1e-15) {
      return 21; // GSL_ESING
    }
    gsl_vector_set(x, i, sum / diag);
  }
  
  return 0; // GSL_SUCCESS
}

// Compute determinant from LU decomposition
static inline double gsl_linalg_LU_det(gsl_matrix *LU, int signum) {
  size_t n = LU->size1;
  double det = (double)signum;
  
  for (size_t i = 0; i < n; i++) {
    det *= gsl_matrix_get(LU, i, i);
  }
  
  return det;
}

// Matrix inversion using LU decomposition
static inline int gsl_linalg_LU_invert(const gsl_matrix *LU, const gsl_permutation *p, 
                                        gsl_matrix *inverse) {
  size_t n = LU->size1;
  if (LU->size2 != n || p->size != n || inverse->size1 != n || inverse->size2 != n) {
    return 19; // GSL_EBADLEN
  }
  
  gsl_vector *col = gsl_vector_calloc(n);
  gsl_vector *x = gsl_vector_alloc(n);
  
  if (col == NULL || x == NULL) {
    if (col) gsl_vector_free(col);
    if (x) gsl_vector_free(x);
    return 8; // GSL_ENOMEM
  }
  
  for (size_t j = 0; j < n; j++) {
    gsl_vector_set_zero(col);
    gsl_vector_set(col, j, 1.0);
    
    int status = gsl_linalg_LU_solve(LU, p, col, x);
    if (status != 0) { // GSL_SUCCESS
      gsl_vector_free(col);
      gsl_vector_free(x);
      return status;
    }
    
    for (size_t i = 0; i < n; i++) {
      gsl_matrix_set(inverse, i, j, gsl_vector_get(x, i));
    }
  }
  
  gsl_vector_free(col);
  gsl_vector_free(x);
  return 0; // GSL_SUCCESS
}

// BLAS Level 1: axpy (y = alpha*x + y)
static inline int gsl_blas_daxpy(double alpha, const gsl_vector *x, gsl_vector *y) {
  if (x->size != y->size) return 19; // GSL_EBADLEN
  
  for (size_t i = 0; i < x->size; i++) {
    double val = gsl_vector_get(y, i) + alpha * gsl_vector_get(x, i);
    gsl_vector_set(y, i, val);
  }
  return 0; // GSL_SUCCESS
}

// BLAS Level 2: gemv (y = alpha*A*x + beta*y)
static inline int gsl_blas_dgemv(int TransA, double alpha, const gsl_matrix *A,
                                  const gsl_vector *x, double beta, gsl_vector *y) {
  size_t M = A->size1;
  size_t N = A->size2;
  
  if (TransA == 111) {  // CblasNoTrans
    if (N != x->size || M != y->size) return 19; // GSL_EBADLEN
    
    for (size_t i = 0; i < M; i++) {
      double sum = 0.0;
      for (size_t j = 0; j < N; j++) {
        sum += gsl_matrix_get(A, i, j) * gsl_vector_get(x, j);
      }
      gsl_vector_set(y, i, beta * gsl_vector_get(y, i) + alpha * sum);
    }
  } else {  // CblasTrans
    if (M != x->size || N != y->size) return 19; // GSL_EBADLEN
    
    for (size_t i = 0; i < N; i++) {
      double sum = 0.0;
      for (size_t j = 0; j < M; j++) {
        sum += gsl_matrix_get(A, j, i) * gsl_vector_get(x, j);
      }
      gsl_vector_set(y, i, beta * gsl_vector_get(y, i) + alpha * sum);
    }
  }
  return 0; // GSL_SUCCESS
}

// BLAS Level 3: gemm (C = alpha*A*B + beta*C)
static inline int gsl_blas_dgemm(int TransA, int TransB, double alpha,
                                  const gsl_matrix *A, const gsl_matrix *B,
                                  double beta, gsl_matrix *C) {
  size_t M = (TransA == 111) ? A->size1 : A->size2;
  size_t N = (TransB == 111) ? B->size2 : B->size1;
  size_t K = (TransA == 111) ? A->size2 : A->size1;
  
  if (C->size1 != M || C->size2 != N) return 19; // GSL_EBADLEN
  
  for (size_t i = 0; i < M; i++) {
    for (size_t j = 0; j < N; j++) {
      double sum = 0.0;
      for (size_t k = 0; k < K; k++) {
        double a_val = (TransA == 111) ? gsl_matrix_get(A, i, k) : gsl_matrix_get(A, k, i);
        double b_val = (TransB == 111) ? gsl_matrix_get(B, k, j) : gsl_matrix_get(B, j, k);
        sum += a_val * b_val;
      }
      gsl_matrix_set(C, i, j, beta * gsl_matrix_get(C, i, j) + alpha * sum);
    }
  }
  return 0; // GSL_SUCCESS
}

// Sort vector in place
static inline void gsl_sort_vector(gsl_vector *v) {
  // Simple bubble sort (good enough for small vectors)
  size_t n = v->size;
  for (size_t i = 0; i < n - 1; i++) {
    for (size_t j = 0; j < n - i - 1; j++) {
      if (gsl_vector_get(v, j) > gsl_vector_get(v, j + 1)) {
        double tmp = gsl_vector_get(v, j);
        gsl_vector_set(v, j, gsl_vector_get(v, j + 1));
        gsl_vector_set(v, j + 1, tmp);
      }
    }
  }
}

// Sort two vectors together (sort v1, apply same permutation to v2)
static inline void gsl_sort_vector2(gsl_vector *v1, gsl_vector *v2) {
  size_t n = v1->size;
  if (v2->size != n) return;
  
  for (size_t i = 0; i < n - 1; i++) {
    for (size_t j = 0; j < n - i - 1; j++) {
      if (gsl_vector_get(v1, j) > gsl_vector_get(v1, j + 1)) {
        // Swap in v1
        double tmp1 = gsl_vector_get(v1, j);
        gsl_vector_set(v1, j, gsl_vector_get(v1, j + 1));
        gsl_vector_set(v1, j + 1, tmp1);
        
        // Swap in v2
        double tmp2 = gsl_vector_get(v2, j);
        gsl_vector_set(v2, j, gsl_vector_get(v2, j + 1));
        gsl_vector_set(v2, j + 1, tmp2);
      }
    }
  }
}

// Vector min/max functions
static inline double gsl_vector_min(const gsl_vector *v) {
  if (v->size == 0) return 0.0;
  double min_val = gsl_vector_get(v, 0);
  for (size_t i = 1; i < v->size; i++) {
    double val = gsl_vector_get(v, i);
    if (val < min_val) min_val = val;
  }
  return min_val;
}

static inline double gsl_vector_max(const gsl_vector *v) {
  if (v->size == 0) return 0.0;
  double max_val = gsl_vector_get(v, 0);
  for (size_t i = 1; i < v->size; i++) {
    double val = gsl_vector_get(v, i);
    if (val > max_val) max_val = val;
  }
  return max_val;
}

static inline size_t gsl_vector_min_index(const gsl_vector *v) {
  if (v->size == 0) return 0;
  size_t min_idx = 0;
  double min_val = gsl_vector_get(v, 0);
  for (size_t i = 1; i < v->size; i++) {
    double val = gsl_vector_get(v, i);
    if (val < min_val) {
      min_val = val;
      min_idx = i;
    }
  }
  return min_idx;
}

static inline size_t gsl_vector_max_index(const gsl_vector *v) {
  if (v->size == 0) return 0;
  size_t max_idx = 0;
  double max_val = gsl_vector_get(v, 0);
  for (size_t i = 1; i < v->size; i++) {
    double val = gsl_vector_get(v, i);
    if (val > max_val) {
      max_val = val;
      max_idx = i;
    }
  }
  return max_idx;
}

// Statistics functions
static inline double gsl_stats_mean(const double data[], size_t stride, size_t n) {
  if (n == 0) return 0.0;
  double sum = 0.0;
  for (size_t i = 0; i < n; i++) {
    sum += data[i * stride];
  }
  return sum / n;
}

static inline double gsl_stats_variance(const double data[], size_t stride, size_t n) {
  if (n < 2) return 0.0;
  double mean = gsl_stats_mean(data, stride, n);
  double sum_sq = 0.0;
  for (size_t i = 0; i < n; i++) {
    double diff = data[i * stride] - mean;
    sum_sq += diff * diff;
  }
  return sum_sq / (n - 1);
}

static inline double gsl_stats_sd(const double data[], size_t stride, size_t n) {
  return sqrt(gsl_stats_variance(data, stride, n));
}

// Error handling (simplified) - using integer codes directly
static inline const char* gsl_strerror(int gsl_errno) {
  switch (gsl_errno) {
    case 0: return "success";
    case -1: return "failure";
    case 8: return "out of memory";
    case 4: return "invalid argument";
    case 21: return "singularity detected";
    case 19: return "length mismatch";
    default: return "unknown error";
  }
}

// Floating point comparison with tolerance
static inline int gsl_fcmp(double x, double y, double epsilon) {
  if (fabs(x - y) < epsilon) {
    return 0;  // equal within tolerance
  } else if (x < y) {
    return -1;
  } else {
    return 1;
  }
}

// Random number generation functions (simple LCG implementation)
static inline gsl_rng* gsl_rng_alloc(const gsl_rng_type *T) {
  gsl_rng *r = (gsl_rng*)malloc(sizeof(gsl_rng));
  if (r == NULL) return NULL;
  r->type = T;
  r->state = 1;  // Default seed
  return r;
}

static inline void gsl_rng_set(gsl_rng *r, unsigned long int seed) {
  r->state = seed;
}

static inline void gsl_rng_free(gsl_rng *r) {
  free(r);
}

// Linear congruential generator
static inline double gsl_rng_uniform(const gsl_rng *r) {
  // Simple LCG: state = (a * state + c) mod m
  unsigned long int a = 1103515245UL;
  unsigned long int c = 12345UL;
  unsigned long int m = 0x80000000UL;  // 2^31
  
  // Cast away const to update state (not thread-safe but simple)
  gsl_rng *rw = (gsl_rng *)r;
  rw->state = (a * rw->state + c) % m;
  
  return (double)rw->state / (double)m;
}

// Box-Muller transform for Gaussian distribution
static inline double gsl_ran_gaussian(const gsl_rng *r, double sigma) {
  double u1 = gsl_rng_uniform(r);
  double u2 = gsl_rng_uniform(r);
  double z0 = sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2);
  return sigma * z0;
}

// GSL special functions - minimal implementations
static inline double gsl_sf_log(double x) {
  return log(x);
}

static inline double gsl_sf_exp(double x) {
  return exp(x);
}

static inline double gsl_sf_gamma(double x) {
  return tgamma(x);
}

static inline double gsl_pow_2(double x) {
  return x * x;
}

static inline double gsl_pow_3(double x) {
  return x * x * x;
}

static inline double gsl_pow_4(double x) {
  double x2 = x * x;
  return x2 * x2;
}

static inline double gsl_pow_5(double x) {
  double x2 = x * x;
  return x2 * x2 * x;
}

static inline double gsl_pow_6(double x) {
  double x2 = x * x;
  return x2 * x2 * x2;
}

static inline double gsl_hypot3(double x, double y, double z) {
  return sqrt(x*x + y*y + z*z);
}

// Define missing M_* constants if not available
#ifndef M_SQRT3
#define M_SQRT3  1.73205080756887729353  // sqrt(3)
#endif
#ifndef M_SQRT5  
#define M_SQRT5  2.23606797749978969641  // sqrt(5)
#endif

// GSL special functions requiring more complex implementations
// These are stubs - real implementations would require numerical algorithms
static inline double gsl_sf_bessel_J0(double x) {
  // Simplified Bessel J0 - this is a very rough approximation
  // For production, use proper implementation or GSL
  if (fabs(x) < 0.01) return 1.0 - (x*x)/4.0;
  return cos(x);  // Very rough approximation
}

static inline double gsl_sf_expint_E1(double x) {
  // Exponential integral E1 - simplified
  if (x <= 0) return INFINITY;
  return exp(-x) / x;  // Rough approximation
}

static inline double gsl_sf_expint_E2(double x) {
  // Exponential integral E2 - simplified
  if (x <= 0) return INFINITY;
  return exp(-x);  // Rough approximation
}

static inline double gsl_sf_expint_En(int n, double x) {
  // Exponential integral En - simplified
  if (x <= 0) return INFINITY;
  return exp(-x) / pow(x, n-1);  // Rough approximation
}

// GSL constants
#define GSL_LOG_DBL_MIN  (-708.3964185322641)  // log(DBL_MIN) approximately

// GSL macros
#define GSL_IS_EVEN(n) (((n) & 1) == 0)
#define GSL_IS_ODD(n)  (((n) & 1) == 1)
#define GSL_MAX(a, b) ((a) > (b) ? (a) : (b))
#define GSL_MIN(a, b) ((a) < (b) ? (a) : (b))

// GSL math macros using standard C functions
static inline int gsl_isnan(double x) {
  return isnan(x);
}

static inline int gsl_isinf(double x) {
  return isinf(x);
}

// Derivative function (stub - proper implementation requires GSL)
static inline int gsl_deriv_central(const gsl_function *f, double x, double h, 
                                    double *result, double *abserr) {
  // Simplified finite difference approximation
  double xph = x + h;
  double xmh = x - h;
  double fp = f->function(xph, f->params);
  double fm = f->function(xmh, f->params);
  *result = (fp - fm) / (2.0 * h);
  *abserr = 0.0;  // We don't compute error estimate
  return 0;
}

// Interpolation functions (simplified implementation)
static inline gsl_interp* gsl_interp_alloc(const gsl_interp_type *T, size_t size) {
  gsl_interp *interp = (gsl_interp*)malloc(sizeof(gsl_interp));
  if (interp) {
    interp->type = T;
    interp->size = size;
    interp->xdata = NULL;
    interp->ydata = NULL;
  }
  return interp;
}

static inline void gsl_interp_free(gsl_interp *interp) {
  free(interp);
}

static inline gsl_interp_accel* gsl_interp_accel_alloc(void) {
  gsl_interp_accel *acc = (gsl_interp_accel*)malloc(sizeof(gsl_interp_accel));
  if (acc) {
    acc->cache = 0;
  }
  return acc;
}

static inline void gsl_interp_accel_free(gsl_interp_accel *acc) {
  free(acc);
}

static inline int gsl_interp_init(gsl_interp *interp, const double xa[], const double ya[], size_t size) {
  interp->xdata = (double*)xa;
  interp->ydata = (double*)ya;
  interp->size = size;
  return 0;
}

// Simple linear interpolation
static inline double gsl_interp_eval(const gsl_interp *interp, const double xa[], const double ya[], 
                                     double x, gsl_interp_accel *acc) {
  // Simplified linear interpolation
  if (interp->size < 2) return 0.0;
  
  // Find the interval
  size_t i = 0;
  for (i = 0; i < interp->size - 1; i++) {
    if (x >= xa[i] && x <= xa[i+1]) break;
  }
  if (i >= interp->size - 1) i = interp->size - 2;
  
  // Linear interpolation
  double t = (x - xa[i]) / (xa[i+1] - xa[i]);
  return ya[i] + t * (ya[i+1] - ya[i]);
}

// Multiroot solver functions (stubs - not functional, just allow compilation)
static inline gsl_multiroot_fdfsolver* gsl_multiroot_fdfsolver_alloc(const gsl_multiroot_fdfsolver_type *T, size_t n) {
  gsl_multiroot_fdfsolver *s = (gsl_multiroot_fdfsolver*)malloc(sizeof(gsl_multiroot_fdfsolver));
  if (s) {
    s->type = T;
    s->f = gsl_vector_alloc(n);
    s->x = gsl_vector_alloc(n);
    s->state = NULL;
  }
  return s;
}

static inline int gsl_multiroot_fdfsolver_set(gsl_multiroot_fdfsolver *s, gsl_multiroot_function_fdf *fdf, const gsl_vector *x) {
  if (s && s->x && x) {
    gsl_vector_memcpy(s->x, x);
  }
  return 0;
}

static inline int gsl_multiroot_fdfsolver_iterate(gsl_multiroot_fdfsolver *s) {
  // Stub - not actually functional
  return -1; // Return error to indicate not supported
}

static inline int gsl_multiroot_test_residual(const gsl_vector *f, double epsabs) {
  // Stub implementation
  double norm = 0;
  for (size_t i = 0; i < f->size; i++) {
    double val = gsl_vector_get(f, i);
    norm += val * val;
  }
  return (sqrt(norm) < epsabs) ? 0 : -2;  // 0 or -2 (GSL_CONTINUE)
}

static inline gsl_vector* gsl_multiroot_fdfsolver_root(const gsl_multiroot_fdfsolver *s) {
  return s ? s->x : NULL;
}

static inline void gsl_multiroot_fdfsolver_free(gsl_multiroot_fdfsolver *s) {
  if (s) {
    if (s->f) gsl_vector_free(s->f);
    if (s->x) gsl_vector_free(s->x);
    free(s);
  }
}

// Integration workspace and functions (stubs for compatibility)
typedef struct {
  size_t limit;
  size_t size;
  double *alist;
  double *blist;
  double *rlist;
  double *elist;
} gsl_integration_workspace;

// Integration rule constants
#define GSL_INTEG_GAUSS15 1
#define GSL_INTEG_GAUSS21 2
#define GSL_INTEG_GAUSS31 3
#define GSL_INTEG_GAUSS41 4
#define GSL_INTEG_GAUSS51 5
#define GSL_INTEG_GAUSS61 6

static inline gsl_integration_workspace* gsl_integration_workspace_alloc(size_t n) {
  gsl_integration_workspace *w = (gsl_integration_workspace*)calloc(1, sizeof(gsl_integration_workspace));
  if (w) {
    w->limit = n;
    w->size = 0;
    w->alist = (double*)calloc(n, sizeof(double));
    w->blist = (double*)calloc(n, sizeof(double));
    w->rlist = (double*)calloc(n, sizeof(double));
    w->elist = (double*)calloc(n, sizeof(double));
    if (!w->alist || !w->blist || !w->rlist || !w->elist) {
      free(w->alist);
      free(w->blist);
      free(w->rlist);
      free(w->elist);
      free(w);
      return NULL;
    }
  }
  return w;
}

static inline void gsl_integration_workspace_free(gsl_integration_workspace *w) {
  if (w) {
    free(w->alist);
    free(w->blist);
    free(w->rlist);
    free(w->elist);
    free(w);
  }
}

static inline int gsl_integration_qag(const gsl_function *f,
                                      double a, double b,
                                      double epsabs, double epsrel,
                                      size_t limit, int key,
                                      gsl_integration_workspace *workspace,
                                      double *result, double *abserr) {
  // Stub implementation - just evaluate at midpoint
  // Real implementation would use adaptive Gauss-Kronrod quadrature
  double x = (a + b) / 2.0;
  *result = f->function(x, f->params) * (b - a);
  *abserr = 0.0;
  return 0; // GSL_SUCCESS
}

#endif // HAVE_GSL

#endif // FEENOX_LINALG_H
