/*------------ -------------- -------- --- ----- ---   --       -            -
 *  wasora builtin vector functions
 *
 *  Copyright (C) 2009--2015 jeremy theler
 *
 *  This file is part of wasora.
 *
 *  wasora is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  wasora is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with wasora.  If not, see <http://www.gnu.org/licenses/>.
 *------------------- ------------  ----    --------  --     -       -         -
 */

#ifndef _WASORA_H_
#include "wasora.h"
#endif

///fv+vecsize+name vecsize
///fv+vecsize+usage vecsize(b)
///fv+vecsize+desc Returns the size of vector $\vec{b}$.
///fv+vecsize+example vecsize.was
double builtin_vecsize(vector_t **arg) {
  if (!arg[0]->initialized) {
    wasora_vector_init(arg[0]);
  }
  return (double)arg[0]->size;
}

///fv+vecsum+name vecsum
///fv+vecsum+usage vecsum(b)
///fv+vecsum+math \sum_{i=1}^{\text{vecsize}(\vec{b})} b_i
///fv+vecsum+desc Computes the sum of all the components of vector $\vec{b}$.
///fv+vecsum+example vecsum.was
double builtin_vecsum(vector_t **arg) {

  int i;
  double s;
  
  if (!arg[0]->initialized) {
    wasora_vector_init(arg[0]);
  }

  s = 0;
  for (i = 0; i < arg[0]->size; i++) {
    s += gsl_vector_get(wasora_value_ptr(arg[0]), i);
  }

  return s;
}


///fv+vecnorm+name vecnorm
///fv+vecnorm+usage vecnorm(b)
///fv+vecnorm+math \sqrt{\sum_{i=1}^{\text{vecsize}(\vec{b})} b^2_i}
///fv+vecnorm+desc Computes euclidean norm of vector $\vec{b}$. Other norms can be computed explicitly
///fv+vecnorm+desc using the `sum` functional, as illustrated in the example.
///fv+vecnorm+example vecnorm.was
double builtin_vecnorm(vector_t **arg) {

  int i;
  double s;
  
  if (!arg[0]->initialized) {
    wasora_vector_init(arg[0]);
  }

  s = 0;
  for (i = 0; i < arg[0]->size; i++) {
    s += gsl_pow_2(gsl_vector_get(wasora_value_ptr(arg[0]), i));
  }

  return sqrt(s);
}

///fv+vecmin+name vecmin
///fv+vecmin+usage vecmin(b)
///fv+vecmin+math \min_i b_i
///fv+vecmin+desc Returns the smallest element of vector $\vec{b}$, taking into account its sign
///fv+vecmin+desc (i.e. $-2 < 1$).
///fv+vecmin+example vecmin.was
double builtin_vecmin(vector_t **arg) {

  double y;
  
  if (!arg[0]->initialized) {
    wasora_vector_init(arg[0]);
  }
  
  y = gsl_vector_min(wasora_value_ptr(arg[0]));
  return y;
}

///fv+vecmax+name vecmax
///fv+vecmax+usage vecmax(b)
///fv+vecmax+math \max_i b_i
///fv+vecmax+desc Returns the biggest element of vector $\vec{b}$, taking into account its sign
///fv+vecmax+desc (i.e. $1 > -2$).
///fv+vecmax+example vecmax.was
double builtin_vecmax(vector_t **arg) {

  double y;
  
  if (!arg[0]->initialized) {
    wasora_vector_init(arg[0]);
  }
  
  y = gsl_vector_max(wasora_value_ptr(arg[0]));
  return y;
}

///fv+vecminindex+name vecminindex
///fv+vecminindex+usage vecminindex(b)
///fv+vecminindex+math i / b_i = \min_i b_i
///fv+vecminindex+desc Returns the index of the smallest element of vector $\vec{b}$, taking into account its sign
///fv+vecminindex+desc (i.e. $-2 < 1$).
///fv+vecminindex+example vecminindex.was
double builtin_vecminindex(vector_t **arg) {

  double y;
  
  if (!arg[0]->initialized) {
    wasora_vector_init(arg[0]);
  }
  
  y = (double)(gsl_vector_min_index(wasora_value_ptr(arg[0]))+1);
  return y;
}

///fv+vecmaxindex+name vecmaxindex
///fv+vecmaxindex+usage vecmaxindex(b)
///fv+vecmaxindex+math i / b_i = \max_i b_i
///fv+vecmaxindex+desc Returns the index of the biggest element of vector $\vec{b}$, taking into account its sign
///fv+vecmaxindex+desc (i.e. $2 > -1$).
///fv+vecmaxindex+example vecminindex.was
double builtin_vecmaxindex(vector_t **arg) {

  double y;
  
  if (!arg[0]->initialized) {
    wasora_vector_init(arg[0]);
  }
  
  y = (double)(gsl_vector_max_index(wasora_value_ptr(arg[0]))+1);
  return y;
}

///fv+vecdot+name vecdot
///fv+vecdot+usage vecdot(a,b)
///fv+vecdot+math \vec{a} \cdot \vec{b} = \sum_{i=1}^{\text{vecsize}(\vec{a})} a_i \cdot b_i
///fv+vecdot+desc Computes the dot product between vectors $\vec{a}$ and $\vec{b}$, which should
///fv+vecdot+desc have the same size.
///fv+vecdot+example vecdot.was
double builtin_vecdot(vector_t **arg) {

  int i;
  double s;

  if (!arg[0]->initialized) {
    wasora_vector_init(arg[0]);
  }

  if (!arg[1]->initialized) {
    wasora_vector_init(arg[1]);
  }
  
  if (arg[0]->size != arg[1]->size) {
    wasora_push_error_message("when attempting to compute dot product, vector '%s' has size %d and '%s' has size %d", arg[0]->name, arg[0]->size, arg[1]->name, arg[1]->size);
    wasora_runtime_error();
  }
  
  s = 0;
  for (i = 0; i < arg[0]->size; i++) {
    s += gsl_vector_get(wasora_value_ptr(arg[0]), i)*gsl_vector_get(wasora_value_ptr(arg[1]), i);
  }

  return s;
}

