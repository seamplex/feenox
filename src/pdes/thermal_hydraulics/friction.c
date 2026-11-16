/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for one-dimensional thermal-hydraulics: friction factors
 *
 *  Copyright (C) 2026 Jeremy Theler
 *
 *  This file is part of FeenoX <https://www.seamplex.com/feenox>.
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
 *  along with FeenoX.  If not, see <http://www.gnu.org/licenses/>.
 *------------------- ------------  ----    --------  --     -       -         -
 */
#include "feenox.h"
#include "thermal_hydraulics.h"

// Churchill (1977) Darcy friction factor, valid for laminar, transition and
// turbulent flow (smooth and rough pipes) with a single continuous expression
double thermal_hydraulics_friction_churchill(double Re, double relative_roughness) {
  
  // avoid overflows for stagnant flow, f*Re -> 64 anyway
  if (Re < 1e-6) {
    Re = 1e-6;
  }
  
  double A = gsl_pow_int(2.457 * log(1.0/(pow(7.0/Re, 0.9) + 0.27*relative_roughness)), 16);
  double B = gsl_pow_int(37530.0/Re, 16);
  return 8 * pow(gsl_pow_int(8.0/Re, 12) + pow(A+B, -1.5), 1.0/12.0);
}
