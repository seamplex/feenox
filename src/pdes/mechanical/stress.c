#include "feenox.h"
#include "mechanical.h"
extern feenox_t feenox;
extern mechanical_t mechanical;

// this works for both 2d and 3d
#define FLUX_SIGMAX  0
#define FLUX_SIGMAY  1
#define FLUX_TAUXY   2
#define FLUX_SIGMAZ  3
#define FLUX_TAUYZ   4
#define FLUX_TAUZX   5

int feenox_problem_gradient_fill_mechanical(void) {
  
  feenox_gradient_fill(mechanical, sigmax);
  feenox_gradient_fill(mechanical, sigmay);
  feenox_gradient_fill(mechanical, tauxy);
  if (feenox.pde.dofs == 3) {
    feenox_gradient_fill(mechanical, sigmaz);
    feenox_gradient_fill(mechanical, tauyz);
    feenox_gradient_fill(mechanical, tauzx);
  }
  
  return FEENOX_OK;
  
}

// used only in rough
int feenox_problem_gradient_properties_at_element_nodes_mechanical(element_t *element, mesh_t *mesh) {

  
  
  
  return FEENOX_OK;
}

int feenox_problem_gradient_fluxes_at_node_alloc_mechanical(node_t *node) {
  
  size_t flux_size = (feenox.pde.dim == 3) ? 6 : 3;
  if (node->flux == NULL) {
    feenox_check_alloc(node->flux = calloc(flux_size, sizeof(double)));
  } else {
    unsigned int m = 0;
    for (m = 0; m < flux_size; m++) {
      node->flux[m] = 0;
    }
  }
  
  return FEENOX_OK;
}

int feenox_problem_gradient_add_elemental_contribution_to_node_mechanical(node_t *node, element_t *element, unsigned int j, double rel_weight) {

  // TODO: cache properties
  // TODO: orthotropic
  // TODO: check what has to be computed and what not
  double E  = mechanical.E.eval(&mechanical.E, node->x, element->physical_group->material);
  double nu = mechanical.nu.eval(&mechanical.nu, node->x, element->physical_group->material);
  double lambda = E*nu/((1+nu)*(1-2*nu));
  double mu = 0.5*E/(1+nu);
  
  double ex = gsl_matrix_get(element->dphidx_node[j], 0, 0);
  double ey = gsl_matrix_get(element->dphidx_node[j], 1, 1);
  double ez = 0;
  if (feenox.pde.dofs == 3) {
    ez = gsl_matrix_get(element->dphidx_node[j], 2, 2);
  }  
  
  double gammaxy = gsl_matrix_get(element->dphidx_node[j], 0, 1) + gsl_matrix_get(element->dphidx_node[j], 1, 0);
  double gammayz = 0;
  double gammazx = 0;
  if (feenox.pde.dofs == 3) {
    gammayz = gsl_matrix_get(element->dphidx_node[j], 1, 2) + gsl_matrix_get(element->dphidx_node[j], 2, 1);
    gammazx = gsl_matrix_get(element->dphidx_node[j], 2, 0) + gsl_matrix_get(element->dphidx_node[j], 0, 2);
  }  
  
  // normal stresses
  double lambda_div = lambda*(ex + ey + ez);
  double two_mu = 2*mu;
  double sigmax = lambda_div + two_mu * ex;
  double sigmay = 0;
  double sigmaz = 0;
  if (feenox.pde.dim == 3) {
    sigmay = lambda_div + two_mu * ey;
    sigmaz = lambda_div + two_mu * ez;
  }  
  
  // subtract the thermal contribution to the normal stresses (see IFEM.Ch30)
  if (mechanical.alpha.defined) {
    double alpha = mechanical.alpha.eval(&mechanical.alpha, node->x, element->physical_group->material);
    if (alpha != 0) {
      double DT = mechanical.T.eval(&mechanical.T, node->x, element->physical_group->material) - mechanical.T0;
      double thermal_stress = E/(1-2*nu) * alpha * DT;
    
      sigmax -= thermal_stress;
      if (feenox.pde.dim == 3) {
        sigmay -= thermal_stress;
        sigmaz -= thermal_stress;
      }  
    }
  }
  
  // shear stresses
  double tauxy = mu * gammaxy;
  double tauyz = 0;
  double tauzx = 0;
  if (feenox.pde.dofs == 3) {
    tauyz = mu * gammayz;
    tauzx = mu * gammazx;
  }
  
  node->flux[FLUX_SIGMAX] += rel_weight * (sigmax - node->flux[FLUX_SIGMAX]);
  node->flux[FLUX_SIGMAY] += rel_weight * (sigmay - node->flux[FLUX_SIGMAY]);
  node->flux[FLUX_TAUXY] += rel_weight * (tauxy - node->flux[FLUX_TAUXY]);
  if (feenox.pde.dofs == 3) {
    node->flux[FLUX_SIGMAZ] += rel_weight * (sigmaz - node->flux[FLUX_SIGMAZ]);
    node->flux[FLUX_TAUYZ] += rel_weight * (tauyz - node->flux[FLUX_TAUYZ]);
    node->flux[FLUX_TAUZX] += rel_weight * (tauzx - node->flux[FLUX_TAUZX]);
  }  
  
  return FEENOX_OK;
}

int feenox_problem_gradient_fill_fluxes_mechanical(mesh_t *mesh, size_t j) {
  
  mechanical.sigmax->data_value[j] = mesh->node[j].flux[FLUX_SIGMAX];
  mechanical.sigmay->data_value[j] = mesh->node[j].flux[FLUX_SIGMAY];
  mechanical.tauxy->data_value[j] = mesh->node[j].flux[FLUX_TAUXY];
  if (feenox.pde.dofs == 3) {
    mechanical.sigmaz->data_value[j] = mesh->node[j].flux[FLUX_SIGMAZ];
    mechanical.tauyz->data_value[j] = mesh->node[j].flux[FLUX_TAUYZ];
    mechanical.tauzx->data_value[j] = mesh->node[j].flux[FLUX_TAUZX];
  }
  
  // TODO: uncertainties
  
  return FEENOX_OK;
  
}
