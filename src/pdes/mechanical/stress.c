#include "feenox.h"
#include "mechanical.h"
extern feenox_t feenox;
extern mechanical_t mechanical;

int feenox_problem_gradient_fill_mechanical(void) {
  
  feenox_gradient_fill(mechanical, sigmax);
  if (feenox.pde.dim > 1) {
    feenox_gradient_fill(mechanical, sigmay);
    feenox_gradient_fill(mechanical, tauxy);
    if (feenox.pde.dim > 2) {
      feenox_gradient_fill(mechanical, sigmaz);
      feenox_gradient_fill(mechanical, tauyz);
      feenox_gradient_fill(mechanical, tauzx);
    }
  }
  
  return FEENOX_OK;
  
}

// used only in rough
int feenox_problem_gradient_properties_at_element_nodes_mechanical(element_t *element, mesh_t *mesh) {

  
  
  
  return FEENOX_OK;
}

int feenox_problem_gradient_fluxes_at_node_alloc_mechanical(node_t *node) {
  if (node->flux == NULL) {
    feenox_check_alloc(node->flux = calloc(feenox.pde.dim, sizeof(double)));
  } else {
    unsigned int m = 0;
    for (m = 0; m < feenox.pde.dim; m++) {
      node->flux[m] = 0;
    }
  }
  return FEENOX_OK;
}

int feenox_problem_gradient_add_elemental_contribution_to_node_mechanical(node_t *node, element_t *element, unsigned int j, double rel_weight) {

  // TODO: cache  
  double E  = mechanical.E.eval(&mechanical.E, node->x, element->physical_group->material);
  double nu = mechanical.nu.eval(&mechanical.nu, node->x, element->physical_group->material);
  double lambda = E*nu/((1+nu)*(1-2*nu));
  double mu = 0.5*E/(1+nu);
  
  double ex = gsl_matrix_get(element->dphidx_node[j], 0, 0);
  double ey = gsl_matrix_get(element->dphidx_node[j], 1, 1);
  double ez = gsl_matrix_get(element->dphidx_node[j], 2, 2);
  
  double sigmax = lambda * (ex+ey+ez) + 2*mu * ex;
  double sigmay = lambda * (ex+ey+ez) + 2*mu * ey;
  double sigmaz = lambda * (ex+ey+ez) + 2*mu * ez;
  
  // subtract the thermal contribution to the normal stresses (see IFEM.Ch30)
  
  if (mechanical.alpha.defined) {
    double alpha = mechanical.alpha.eval(&mechanical.alpha, node->x, element->physical_group->material);
    if (alpha != 0) {
      double DT = mechanical.T.eval(&mechanical.T, node->x, element->physical_group->material) - mechanical.T0;
      double xi = E/(1-2*nu) * alpha * DT;
    
      sigmax -= xi;
      sigmay -= xi;
      sigmaz -= xi;
    }  
  }  
  
  node->flux[0] += rel_weight * (sigmax - node->flux[0]);
  node->flux[1] += rel_weight * (sigmay - node->flux[1]);
  node->flux[2] += rel_weight * (sigmaz - node->flux[2]);
  
  return FEENOX_OK;
}

int feenox_problem_gradient_fill_fluxes_mechanical(mesh_t *mesh, size_t j) {
  
  mechanical.sigmax->data_value[j] = mesh->node[j].flux[0];
  if (feenox.pde.dim > 1) {
    mechanical.sigmay->data_value[j] = mesh->node[j].flux[1];
    if (feenox.pde.dim > 2) {
      mechanical.sigmaz->data_value[j] = mesh->node[j].flux[2];
    }
  }
  
  // TODO: uncertainties
  
  return FEENOX_OK;
  
}
