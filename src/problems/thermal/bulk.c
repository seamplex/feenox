#include "feenox.h"
#include "thermal.h"
extern feenox_t feenox;
extern thermal_t thermal;

int feenox_build_element_volumetric_gauss_point_thermal(element_t *element, int v) {

#ifdef HAVE_PETSC
  
  feenox_call(feenox_mesh_compute_w_at_gauss(element, v, feenox.pde.mesh->integration));
  feenox_call(feenox_mesh_compute_H_at_gauss(element, v, feenox.pde.dofs, feenox.pde.mesh->integration));
  feenox_call(feenox_mesh_compute_B_at_gauss(element, v, feenox.pde.dofs, feenox.pde.mesh->integration));
  feenox_call(feenox_mesh_compute_x_at_gauss(element, v, feenox.pde.mesh->integration));
  
  // TODO
//  r_for_axisymmetric = feenox_compute_r_for_axisymmetric(element, v);
  double r_for_axisymmetric = 1;
  double w = element->w[v] * r_for_axisymmetric;
  
  material_t *material = NULL;
  if (element->physical_group != NULL && element->physical_group->material != NULL) {
    material = element->physical_group->material;
  }

  // thermal stiffness matrix
  double k = thermal.k.eval(&thermal.k, element->x[v], material);
  gsl_blas_dgemm(CblasTrans, CblasNoTrans, w*k, element->B[v], element->B[v], 1.0, feenox.pde.Ki);

  // TODO: total source Q
  if (thermal.q.defined) {
    // the volumetric heat source term
    double q = thermal.q.eval(&thermal.q, element->x[v], material);
    unsigned int j = 0;
    for (j = 0; j < element->type->nodes; j++) {
      gsl_vector_add_to_element(feenox.pde.bi, j, w * element->type->gauss[feenox.pde.mesh->integration].h[v][j] * q);
    }
  }
  
  if (feenox.pde.M != NULL) {
    // compute the mass matrix Ht*rho*cp*H
    double rhocp = 0;
    if (thermal.rhocp.defined) {
      rhocp = thermal.rhocp.eval(&thermal.rhocp, element->x[v], material);
    } else if (thermal.kappa.defined) {
      rhocp = k / thermal.kappa.eval(&thermal.kappa, element->x[v], material);
    } else if (thermal.rho.defined && thermal.cp.defined) {
      rhocp = thermal.rho.eval(&thermal.rho, element->x[v], material) * thermal.cp.eval(&thermal.cp, element->x[v], material);
    } else {
      // this should have been already check
      feenox_push_error_message("no heat capacity found");
      return FEENOX_ERROR;
    }
    gsl_blas_dgemm(CblasTrans, CblasNoTrans, w * rhocp, element->H[v], element->H[v], 1.0, feenox.pde.Mi);
  } 

#endif
  
  return FEENOX_OK;
  
}
