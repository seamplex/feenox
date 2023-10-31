/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's BLAS wrapper routines
 *
 *  Copyright (C) 2023 jeremy theler
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
int feenox_blas_Ab(gsl_matrix *A, gsl_vector *b, double alpha, gsl_vector *c) {
  feenox_call(gsl_blas_dgemv(CblasNoTrans, alpha, A, b, 0.0, c));
  return FEENOX_OK;
}

// c = alpha * A'*b
int feenox_blas_Atb(gsl_matrix *A, gsl_vector *b, double alpha, gsl_vector *c) {
  feenox_call(gsl_blas_dgemv(CblasTrans, alpha, A, b, 0.0, c));
  return FEENOX_OK;
}

// c += alpha * A*b
int feenox_blas_Ab_accum(gsl_matrix *A, gsl_vector *b, double alpha, gsl_vector *c) {
  feenox_call(gsl_blas_dgemv(CblasNoTrans, alpha, A, b, 1.0, c));
  return FEENOX_OK;
}

// c += alpha * A'*b
int feenox_blas_Atb_accum(gsl_matrix *A, gsl_vector *b, double alpha, gsl_vector *c) {
  feenox_call(gsl_blas_dgemv(CblasTrans, alpha, A, b, 1.0, c));
  return FEENOX_OK;
}


// R += alpha * B'*B
int feenox_blas_BtB_accum(gsl_matrix *B, double alpha, gsl_matrix *R) {
//  printf("dgemm BtB accum\n");
  feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, alpha, B, B, 1.0, R));
  return FEENOX_OK;
}

// R = alpha * B'*B
int feenox_blas_BtB(gsl_matrix *B, double alpha, gsl_matrix *R) {
  feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, alpha, B, B, 0.0, R));
  return FEENOX_OK;
}


// R += alpha * B'*C*B
int feenox_blas_BtCB_accum(gsl_matrix *B, gsl_matrix *C, gsl_matrix *CB, double alpha, gsl_matrix *R) {
  int alloc = 0;
  if (CB == NULL) {
    CB = gsl_matrix_alloc(C->size1, B->size2);
    alloc = 1;
  }
  
//  printf("dgemm C B accum\n");
  feenox_call(gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, C, B, 0.0, CB));
//  printf("dgemm B CB accum\n");
  feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, alpha, B, CB, 1.0, R));
  
  if (alloc) {
    gsl_matrix_free(CB);
  }
  
  return FEENOX_OK; 
}

// R = alpha * B'*C*B
int feenox_blas_BtCB(gsl_matrix *B, gsl_matrix *C, gsl_matrix *CB, double alpha, gsl_matrix *R) {
  int alloc = 0;
  if (CB == NULL) {
    CB = gsl_matrix_alloc(C->size1, B->size2);
    alloc = 1;
  }
  
//  printf("dgemm C B\n");
  feenox_call(gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, C, B, 0.0, CB));
//  printf("dgemm B CB\n");
  feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, alpha, B, CB, 0.0, R));
  
  if (alloc) {
    gsl_matrix_free(CB);
  }
  
  return FEENOX_OK; 
}


// R += alpha * P'*C*B
int feenox_blas_PtCB_accum(gsl_matrix *P, gsl_matrix *C, gsl_matrix *B, gsl_matrix *CB, double alpha, gsl_matrix *R) {
  int alloc = 0;
  if (CB == NULL) {
    CB = gsl_matrix_alloc(C->size1, B->size2);
    alloc = 1;
  }

//  printf("dgemm CB accum\n");
  feenox_call(gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, C, B, 0.0, CB));
//  printf("dgemm P CB accum\n");
  feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, alpha, P, CB, 1.0, R));
  
  if (alloc) {
    gsl_matrix_free(CB);
  }
  
  return FEENOX_OK; 
}

// R = alpha * P'*C*B
int feenox_blas_PtCB(gsl_matrix *P, gsl_matrix *C, gsl_matrix *B, gsl_matrix *CB, double alpha, gsl_matrix *R) {
  int alloc = 0;
  if (CB == NULL) {
    CB = gsl_matrix_alloc(C->size1, B->size2);
    alloc = 1;
  }

//  printf("dgemm C B\n");
  feenox_call(gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, C, B, 0.0, CB));
//  printf("dgemm P CB\n");
  feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, alpha, P, CB, 0.0, R));
  
  if (alloc) {
    gsl_matrix_free(CB);
  }
  
  return FEENOX_OK; 
}