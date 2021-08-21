#include "feenox.h"
#include "mechanical.h"
extern feenox_t feenox;
extern mechanical_t mechanical;

int feenox_problem_solve_post_mechanical(void) {

  double displ2 = 0;
  double max_displ2= 0;
  
  unsigned int g = 0;
  size_t j = 0;
  for (j = 0; j < feenox.pde.mesh->n_nodes; j++) {
    
    displ2 = 0;
    for (g = 0; g < feenox.pde.dofs; g++) {
      displ2 += gsl_pow_2(feenox.pde.solution[g]->data_value[j]);
    }
    if (displ2 >= max_displ2) {
      max_displ2 = displ2;
      
      feenox_var_value(mechanical.displ_max) = sqrt(displ2);
      feenox_var_value(mechanical.displ_max_x) = feenox.pde.solution[0]->data_argument[0][j];
      feenox_var_value(mechanical.displ_max_y) = feenox.pde.solution[0]->data_argument[1][j];
      if (feenox.pde.dim == 3) {
        feenox_var_value(mechanical.displ_max_z) = feenox.pde.solution[0]->data_argument[2][j];
      }
      
      feenox_var_value(mechanical.u_at_displ_max) = feenox.pde.solution[0]->data_value[j];
      feenox_var_value(mechanical.v_at_displ_max) = feenox.pde.solution[1]->data_value[j];
      if (feenox.pde.dim == 3) {
        feenox_var_value(mechanical.w_at_displ_max) = feenox.pde.solution[2]->data_value[j];
      }
      
    }
  }  
  
  return FEENOX_OK;
}