#include "feenox.h"
#include "thermal.h"
extern feenox_t feenox;
extern thermal_t thermal;

int feenox_build_element_volumetric_gauss_point_thermal(element_t *this, unsigned int v) {

#ifdef HAVE_PETSC
  
  feenox_call(feenox_mesh_compute_w_at_gauss(this, v, feenox.pde.mesh->integration));
  feenox_call(feenox_mesh_compute_H_at_gauss(this, v, feenox.pde.dofs, feenox.pde.mesh->integration));
  feenox_call(feenox_mesh_compute_B_at_gauss(this, v, feenox.pde.dofs, feenox.pde.mesh->integration));
  double zero[3] = {0, 0, 0};
  double *x = zero;
  if (thermal.properties_depend_space || thermal.sources_depend_space) {
    feenox_call(feenox_mesh_compute_x_at_gauss(this, v, feenox.pde.mesh->integration));
    x = this->x[v];
  }
  
  // TODO: axisymmetric
//  r_for_axisymmetric = feenox_compute_r_for_axisymmetric(this, v);
  double r_for_axisymmetric = 1;
  double w = this->w[v] * r_for_axisymmetric;
  
  material_t *material = NULL;
  if (this->physical_group != NULL && this->physical_group->material != NULL) {
    material = this->physical_group->material;
  }

  // thermal stiffness matrix Bt*k*B
  double k = thermal.k.eval(&thermal.k, x, material);
  gsl_blas_dgemm(CblasTrans, CblasNoTrans, w*k, this->B[v], this->B[v], 1.0, feenox.pde.Ki);

  // volumetric heat source term Ht*q
  // TODO: total source Q
  if (thermal.q.defined) {
    double q = thermal.q.eval(&thermal.q, this->x[v], material);
    unsigned int j = 0;
    for (j = 0; j < this->type->nodes; j++) {
      gsl_vector_add_to_element(feenox.pde.bi, j, w * this->type->gauss[feenox.pde.mesh->integration].h[v][j] * q);
    }
  }
  
  // mass matrix Ht*rho*cp*H
  if (feenox.pde.M != NULL) {
    double rhocp = 0;
    if (thermal.rhocp.defined) {
      rhocp = thermal.rhocp.eval(&thermal.rhocp, this->x[v], material);
    } else if (thermal.kappa.defined) {
      rhocp = k / thermal.kappa.eval(&thermal.kappa, this->x[v], material);
    } else if (thermal.rho.defined && thermal.cp.defined) {
      rhocp = thermal.rho.eval(&thermal.rho, this->x[v], material) * thermal.cp.eval(&thermal.cp, this->x[v], material);
    } else {
      // this should have been already checked
      feenox_push_error_message("no heat capacity found");
      return FEENOX_ERROR;
    }
    feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, w * rhocp, this->H[v], this->H[v], 1.0, feenox.pde.Mi));
  } 

#endif
  
  return FEENOX_OK;
  
}
