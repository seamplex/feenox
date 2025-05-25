/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox elastic orthotropic mechanical material
 *
 *  Copyright (C) 2022 Jeremy Theler
 *
 *  This file is part of Feenox <https://www.seamplex.com/feenox>.
 *
 *  feenox is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Feenox is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Feenox.  If not, see <http://www.gnu.org/licenses/>.
 *------------------- ------------  ----    --------  --     -       -         -
 */
#include "feenox.h"
#include "../mechanical.h"

int feenox_mechanical_material_init_linear_elastic_orthotropic(material_t *material, int i) {
  int n_ortho = mechanical.E_x.defined_per_group[i]   + mechanical.E_y.defined_per_group[i]   + mechanical.E_z.defined_per_group[i]   +
                mechanical.nu_xy.defined_per_group[i] + mechanical.nu_yz.defined_per_group[i] + mechanical.nu_zx.defined_per_group[i] +
                mechanical.G_xy.defined_per_group[i]  + mechanical.G_yz.defined_per_group[i]  + mechanical.G_zx.defined_per_group[i];
  
  if (n_ortho < 9) {
    feenox_push_error_message("%d orthotropic properties missing for material '%s'", 9-n_ortho, material->name);
    return FEENOX_ERROR;
  }

  return material_model_elastic_orthotropic;

}


int feenox_problem_mechanical_compute_tangent_matrix_C_elastic_orthotropic(const double *x, material_t *material) {
  
  // TODO: check ranges of validity
  // E > 0
  // G > 0
  // | nu_ij | < sqrt(E_i/E_j)
  // 1 - nu12*nu21 - nu23*nu32 - nu31*nu13 - 2*nu21*nu32*nu13 > 0
  
  double E_x = mechanical.E_x.eval(&mechanical.E_x, x, material);
  double E_y = mechanical.E_y.eval(&mechanical.E_y, x, material);
  double E_z = mechanical.E_z.eval(&mechanical.E_z, x, material);

  // TODO: handle engineering nu12,nu23 and nu13 (instead of nu31)
  double nu_xy = mechanical.nu_xy.eval(&mechanical.nu_xy, x, material);
  double nu_yz = mechanical.nu_yz.eval(&mechanical.nu_yz, x, material);
  double nu_zx = mechanical.nu_zx.eval(&mechanical.nu_zx, x, material);
  
  double G_xy = mechanical.G_xy.eval(&mechanical.G_xy, x, material);
  double G_yz = mechanical.G_yz.eval(&mechanical.G_yz, x, material);
  double G_zx = mechanical.G_zx.eval(&mechanical.G_zx, x, material);
  
  gsl_matrix *S = NULL; // reduced compliance matrix (only the normal-stress stuff)
  feenox_check_alloc(S = gsl_matrix_calloc(3, 3));  
  gsl_matrix *C = NULL; // reduced stiffness matrix
  feenox_check_alloc(C = gsl_matrix_calloc(3, 3));  
  
  // > if you noticed that C is called the stiffness tensor and S is called the compliance
  // > tensor and wondered about it, this is not a mistake and there is no intention to confuse
  // > you. It is a long-time convention that cannot be reverted anymore
  // source: https://www.weizmann.ac.il/chembiophys/bouchbinder/sites/chemphys.bouchbinder/files/uploads/Courses/2016/ta5-linear_elasticity-i.pdf

    
  // fill the 3x3 reduced compliance matrix first
  // [     1/E1  -nu21/E2   -nu31/E3  ]
  // [ -nu12/E1      1/E2   -nu32/E3  ]
  // [ -nu13/E1  -nu23/E2       1/E3  ]
  
  gsl_matrix_set(S, 0, 0, 1.0/E_x);
  gsl_matrix_set(S, 1, 1, 1.0/E_y);
  gsl_matrix_set(S, 2, 2, 1.0/E_z);

  // since S is symmetric,
  // nu21/E2 = nu12/E1
  // nu31/E3 = nu13/E1
  // nu32/E3 = nu23/E2
  // but we ask for nu12, nu23 and nu31 (not nu21, nu32 nor nu13) so we use
  double minus_nu_xy_over_E_x = -nu_xy/E_x;
  double minus_nu_zx_over_E_z = -nu_zx/E_z;
  double minus_nu_yz_over_E_y = -nu_yz/E_y;
  
  // to set the off-diagonal (symmetric) entries
  gsl_matrix_set(S, 0, 1, minus_nu_xy_over_E_x);
  gsl_matrix_set(S, 1, 0, minus_nu_xy_over_E_x);

  gsl_matrix_set(S, 0, 2, minus_nu_zx_over_E_z);
  gsl_matrix_set(S, 2, 0, minus_nu_zx_over_E_z);
  
  gsl_matrix_set(S, 1, 2, minus_nu_yz_over_E_y);
  gsl_matrix_set(S, 2, 1, minus_nu_yz_over_E_y);
  
  
  // compute the stiffness by inverting the 3x3 compliance
  C = feenox_fem_matrix_invert(S, C);
    
  // now fill the full 6x6 C
  gsl_matrix_set(mechanical.C, 0, 0, gsl_matrix_get(C, 0, 0));
  gsl_matrix_set(mechanical.C, 0, 1, gsl_matrix_get(C, 0, 1));
  gsl_matrix_set(mechanical.C, 0, 2, gsl_matrix_get(C, 0, 2));
  gsl_matrix_set(mechanical.C, 1, 0, gsl_matrix_get(C, 1, 0));
  gsl_matrix_set(mechanical.C, 1, 1, gsl_matrix_get(C, 1, 1));
  gsl_matrix_set(mechanical.C, 1, 2, gsl_matrix_get(C, 1, 2));
  gsl_matrix_set(mechanical.C, 2, 0, gsl_matrix_get(C, 2, 0));
  gsl_matrix_set(mechanical.C, 2, 1, gsl_matrix_get(C, 2, 1));
  gsl_matrix_set(mechanical.C, 2, 2, gsl_matrix_get(C, 2, 2));
    
  // the following subscripts have to match rows 3-5 of mechanical.B
  // note that this is not voigt notation! that would be
  // xx,yy,zz,yz,xz,xy and we use xx,yy,zz,xy,yz,zx
  gsl_matrix_set(mechanical.C, 3, 3, G_xy);
  gsl_matrix_set(mechanical.C, 4, 4, G_yz);
  gsl_matrix_set(mechanical.C, 5, 5, G_zx);
    
  gsl_matrix_free(C);
  gsl_matrix_free(S);
    
  return FEENOX_OK;
}

