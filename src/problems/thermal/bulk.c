#include "feenox.h"
feenox_t feenox;

int feenox_build_element_volumetric_gauss_point_thermal(element_t *element, int v) {
  
/*  
  if (distribution_k.defined == 0) {
    feenox_call(feenox_distribution_init(&distribution_k, "k"));
    feenox_call(feenox_distribution_init(&distribution_Q, "Q"));
  }
  if (distribution_k.defined == 0) {
    feenox_push_error_message("cannot find thermal conductivity 'k'");
    return FEENOX_ERROR;
  }

  if (feenox.pde.M != NULL) {
    if (distribution_kappa.defined == 0) {
      feenox_call(feenox_distribution_init(&distribution_kappa, "kappa"));
      if (distribution_kappa.defined == 0) {
        feenox_call(feenox_distribution_init(&distribution_rho, "rho"));
        feenox_call(feenox_distribution_init(&distribution_cp, "cp"));
        if (distribution_cp.defined == 0) {
          feenox_push_error_message("cannot find neither thermal diffusivity 'kappa' nor heat capacity 'cp'");
          return FEENOX_ERROR;
        }
        if (distribution_rho.defined == 0) {
          feenox_push_error_message("cannot find neither thermal diffusivity 'kappa' nor density 'rho'");
          return FEENOX_ERROR;
        }
      }
    }
  }
*/  
  
  material_t *material = NULL;
  if (element->physical_group != NULL && element->physical_group->material != NULL) {
    material = element->physical_group->material;
  }
  
  feenox_call(feenox_mesh_compute_w_at_gauss(element, v, feenox.pde.mesh->integration));
  feenox_call(feenox_mesh_compute_H_at_gauss(element, v, feenox.pde.dofs, feenox.pde.mesh->integration));
  feenox_call(feenox_mesh_compute_B_at_gauss(element, v, feenox.pde.dofs, feenox.pde.mesh->integration));
  feenox_call(feenox_mesh_compute_x_at_gauss(element, v, feenox.pde.mesh->integration));
//  r_for_axisymmetric = feenox_compute_r_for_axisymmetric(element, v);
  double r_for_axisymmetric = 1;
  

/*  
  if (distribution_Q.defined != 0) {
    // the volumetric heat source term
    for (j = 0; j < element->type->nodes; j++) {
      gsl_vector_add_to_element(feenox.pde.bi, j,
        element->w[v] * r_for_axisymmetric * element->type->gauss[feenox.pde.mesh->integration].h[v][j] * feenox_distribution_evaluate(&distribution_Q, material, element->x[v]));
    }
  }
*/ 

  // thermal stiffness matrix
//  k = feenox_distribution_evaluate(&distribution_k, material, element->x[v]);
  double k = 1;
  gsl_blas_dgemm(CblasTrans, CblasNoTrans, element->w[v] * r_for_axisymmetric * k, element->B[v], element->B[v], 1.0, feenox.pde.Ki);

/*  
  if (feenox.pde.M != NULL) {
    // compute the mass matrix Ht*rho*cp*H
    double rhocp = (distribution_kappa.defined) ? k / feenox_distribution_evaluate(&distribution_kappa, material, element->x[v]) : rhocp = feenox_distribution_evaluate(&distribution_rho, material, element->x[v]);
    gsl_blas_dgemm(CblasTrans, CblasNoTrans, element->w[v] * r_for_axisymmetric * rhocp, element->H[v], element->H[v], 1.0, feenox.pde.Mi);
  } 
*/
  
  return FEENOX_OK;
  
}
