#include "feenox.h"
#include "thermal.h"
extern feenox_t feenox;
extern thermal_t thermal;
double zero[3] = {0, 0, 0};

int feenox_problem_build_volumetric_gauss_point_thermal(element_t *this, unsigned int v) {

#ifdef HAVE_PETSC
  
  feenox_call(feenox_mesh_compute_w_at_gauss(this, v, feenox.pde.mesh->integration));
  feenox_call(feenox_mesh_compute_H_at_gauss(this, v, feenox.pde.dofs, feenox.pde.mesh->integration));
  feenox_call(feenox_mesh_compute_B_at_gauss(this, v, feenox.pde.dofs, feenox.pde.mesh->integration));
  double *x = zero;
  if (thermal.space_k || thermal.space_q || thermal.space_m) {
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
  // TODO: there should be a way to use BLAS instead of a for loop
  if (thermal.q.defined) {
    double q = thermal.q.eval(&thermal.q, x, material);
    unsigned int j = 0;
    for (j = 0; j < this->type->nodes; j++) {
      gsl_vector_add_to_element(feenox.pde.bi, j, w * this->type->gauss[feenox.pde.mesh->integration].h[v][j] * q);
    }
  }
  
  if (feenox.pde.has_jacobian) {
    double T = feenox_function_eval(feenox.pde.solution[0], x);
    
    // TODO: this is not working as expected
//    if (thermal.temperature_k) {
      // TODO: this might now work if the distribution is not given as an
      //       algebraic expression but as a pointwise function of T
      //       (but it works if using a property!)
//      double dkdT = feenox_expression_derivative_wrt_function(thermal.k.expr, feenox.pde.solution[0], T);
//      gsl_blas_dgemm(CblasTrans, CblasNoTrans, -1.0/3.0*w*dkdT*T, this->B[v], this->B[v], 1.0, feenox.pde.JKi);
//    }

    if (thermal.temperature_q) {
      double dqdT = feenox_expression_derivative_wrt_function(thermal.q.expr, feenox.pde.solution[0], T);
      // mind the positive sign!
      gsl_blas_dgemm(CblasTrans, CblasNoTrans, w*dqdT, this->H[v], this->H[v], 1.0, feenox.pde.Jbi);
    }  
  }
    
  // mass matrix Ht*rho*cp*H
  if (feenox.pde.has_mass) {
    double rhocp = 0;
    if (thermal.rhocp.defined) {
      rhocp = thermal.rhocp.eval(&thermal.rhocp, x, material);
    } else if (thermal.kappa.defined) {
      rhocp = k / thermal.kappa.eval(&thermal.kappa, x, material);
    } else if (thermal.rho.defined && thermal.cp.defined) {
      rhocp = thermal.rho.eval(&thermal.rho, x, material) * thermal.cp.eval(&thermal.cp, x, material);
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
