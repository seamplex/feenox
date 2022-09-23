/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for Laplace's equation: global header
 *
 *  Copyright (C) 2021 jeremy theler
 *
 *  This file is part of FeenoX <https://www.seamplex.com/feenox>.
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
#ifndef LAPLACE_H
#define LAPLACE_H

typedef struct laplace_t laplace_t;

struct laplace_t {
  distribution_t f;
  distribution_t alpha;

  // TODO: uniform instead of "space-dependent"
  // TODO: constant (i.e. non-time-depedent)
  int space_dependent_mass;
  int phi_dependent_mass;
  
  int space_dependent_source;
  int phi_dependent_source;

  int space_dependent_bc;
  int phi_dependent_bc;
  
  // caches for uniform properties
  struct {
    double f;
    double alpha;
  } cache;
  
};  
extern laplace_t laplace;


#endif /* LAPLACE_H */

