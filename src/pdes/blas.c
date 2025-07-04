/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's BLAS wrapper routines
 *
 *  Copyright (C) 2023 Jeremy Theler
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

// c = alpha * A*b
int feenox_blas_Ab(const gsl_matrix *A, const gsl_vector *b, const double alpha, gsl_vector *c) {
  feenox_call(gsl_blas_dgemv(CblasNoTrans, alpha, A, b, 0.0, c));
  return FEENOX_OK;
}

// c = alpha * A'*b
int feenox_blas_Atb(const gsl_matrix *A, const gsl_vector *b, const double alpha, gsl_vector *c) {
  feenox_call(gsl_blas_dgemv(CblasTrans, alpha, A, b, 0.0, c));
  return FEENOX_OK;
}

// c += alpha * A*b
int feenox_blas_Ab_accum(const gsl_matrix *A, const gsl_vector *b, const double alpha, gsl_vector *c) {
  feenox_call(gsl_blas_dgemv(CblasNoTrans, alpha, A, b, 1.0, c));
  return FEENOX_OK;
}

// c += alpha * A'*b
int feenox_blas_Atb_accum(const gsl_matrix *A, const gsl_vector *b, const double alpha, gsl_vector *c) {
  feenox_call(gsl_blas_dgemv(CblasTrans, alpha, A, b, 1.0, c));
  return FEENOX_OK;
}


// R += alpha * B'*B
int feenox_blas_BtB_accum(const gsl_matrix *B, const double alpha, gsl_matrix *R) {
  feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, alpha, B, B, 1.0, R));
  return FEENOX_OK;
}

// R = alpha * B'*B
int feenox_blas_BtB(const gsl_matrix *B, const double alpha, gsl_matrix *R) {
  feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, alpha, B, B, 0.0, R));
  return FEENOX_OK;
}

// R = alpha * A*B'
int feenox_blas_ABt(const gsl_matrix *A, const gsl_matrix *B, const double alpha, gsl_matrix *R) {
  feenox_call(gsl_blas_dgemm(CblasNoTrans, CblasTrans, alpha, A, B, 0.0, R));
  return FEENOX_OK;
}



// R += alpha * B'*C*B
int feenox_blas_BtCB_accum(const gsl_matrix *B, const gsl_matrix *C, gsl_matrix *CB, const double alpha, gsl_matrix *R) {
  int alloc = 0;
  if (CB == NULL) {
    CB = gsl_matrix_alloc(C->size1, B->size2);
    alloc = 1;
  }
  
  feenox_call(gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, C, B, 0.0, CB));
  feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, alpha, B, CB, 1.0, R));
  
  if (alloc) {
    gsl_matrix_free(CB);
  }
  
  return FEENOX_OK; 
}

// R = alpha * B'*C*B
int feenox_blas_BtCB(const gsl_matrix *B, const gsl_matrix *C, gsl_matrix *CB, const double alpha, gsl_matrix *R) {
  int alloc = 0;
  if (CB == NULL) {
    CB = gsl_matrix_alloc(C->size1, B->size2);
    alloc = 1;
  }
  
  feenox_call(gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, C, B, 0.0, CB));
  feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, alpha, B, CB, 0.0, R));
  
  if (alloc) {
    gsl_matrix_free(CB);
  }
  
  return FEENOX_OK; 
}

// R = alpha * B*C*B'
int feenox_blas_BCBt(const gsl_matrix *B, const gsl_matrix *C, gsl_matrix *CB, const double alpha, gsl_matrix *R) {
  int alloc = 0;
  if (CB == NULL) {
    CB = gsl_matrix_alloc(C->size1, B->size2);
    alloc = 1;
  }
  
  feenox_call(gsl_blas_dgemm(CblasTrans, CblasTrans, 1.0, C, B, 0.0, CB));
  feenox_call(gsl_blas_dgemm(CblasNoTrans, CblasTrans, alpha, B, CB, 0.0, R));
  
  if (alloc) {
    gsl_matrix_free(CB);
  }
  
  return FEENOX_OK; 
}


// R += alpha * P'*C*B
int feenox_blas_PtCB_accum(const gsl_matrix *P, const gsl_matrix *C, const gsl_matrix *B, gsl_matrix *CB, const double alpha, gsl_matrix *R) {
  int alloc = 0;
  if (CB == NULL) {
    CB = gsl_matrix_alloc(C->size1, B->size2);
    alloc = 1;
  }

  feenox_call(gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, C, B, 0.0, CB));
  feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, alpha, P, CB, 1.0, R));
  
  if (alloc) {
    gsl_matrix_free(CB);
  }
  
  return FEENOX_OK; 
}

// R = alpha * P'*C*B
int feenox_blas_PtCB(const gsl_matrix *P, const gsl_matrix *C, const gsl_matrix *B, gsl_matrix *CB, const double alpha, gsl_matrix *R) {
  int alloc = 0;
  if (CB == NULL) {
    CB = gsl_matrix_alloc(C->size1, B->size2);
    alloc = 1;
  }

  feenox_call(gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, C, B, 0.0, CB));
  feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, alpha, P, CB, 0.0, R));
  
  if (alloc) {
    gsl_matrix_free(CB);
  }
  
  return FEENOX_OK; 
}