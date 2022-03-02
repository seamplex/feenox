#include "feenox.h"
extern feenox_t feenox;

int feenox_pde_parse_problem_type(const char *token) {

///kw_pde+PROBLEM+detail Currently, FeenoX can solve the following types of PDE-casted problems:
///kw_pde+PROBLEM+detail @    

///kw_pde+PROBLEM+usage { laplace
///kw_pde+PROBLEM+usage | mechanical
///kw_pde+PROBLEM+usage | thermal
///kw_pde+PROBLEM+usage | modal
///kw_pde+PROBLEM+usage | neutron_diffusion
///kw_pde+PROBLEM+usage }@
  
///kw_pde+PROBLEM+detail @    
///kw_pde+PROBLEM+detail If you are a programmer and want to contribute with another problem type, please do so!
///kw_pde+PROBLEM+detail Check out the [programming guide in the FeenoX repository](https://github.com/seamplex/feenox/blob/main/doc/programming.md).

  
///kw_pde+PROBLEM+detail  * `laplace` (or `poisson`) solves the Laplace (or Poisson) equation.
  if (strcasecmp(token, "laplace") == 0 || strcasecmp(token, "poisson") == 0) {
    feenox.pde.parse_particular = feenox_problem_parse_problem_laplace;
    feenox.pde.init_parser_particular = feenox_problem_init_parser_laplace;
    
///kw_pde+PROBLEM+detail  * `mechanical` (or `elastic`) solves the mechanical elastic problem.
///kw_pde+PROBLEM+detail If the mesh is two-dimensional, and not `AXISYMMETRIC`, either
///kw_pde+PROBLEM+detail `plane_stress` or `plane_strain` has to be given instead of `mechanical`.
  } else if (strcasecmp(token, "mechanical") == 0 || strcasecmp(token, "elastic") == 0) {
    feenox.pde.parse_particular = feenox_problem_parse_problem_mechanical;
    feenox.pde.init_parser_particular = feenox_problem_init_parser_mechanical;

///kw_pde+PROBLEM+detail  * `thermal` (or `heat` ) solves the heat conduction problem.
  } else if (strcasecmp(token, "thermal") == 0 || strcasecmp(token, "heat") == 0) {
    feenox.pde.parse_particular = feenox_problem_parse_problem_thermal;
    feenox.pde.init_parser_particular = feenox_problem_init_parser_thermal;

///kw_pde+PROBLEM+detail  * `modal` computes the natural mechanical frequencies and oscillation modes.        
  } else if (strcasecmp(token, "modal") == 0) {
#ifndef HAVE_SLEPC
    feenox_push_error_message("modal problems need a FeenoX binary linked against SLEPc.");
    return FEENOX_ERROR;
#endif
    feenox.pde.parse_particular = feenox_problem_parse_problem_modal;
    feenox.pde.init_parser_particular = feenox_problem_init_parser_modal;
     
///kw_pde+PROBLEM+detail  * `neutron_diffusion` multi-group core-level neutron diffusion with a FEM formulation 
  } else if (strcasecmp(token, "neutron_diffusion") == 0) {
    feenox.pde.parse_particular = feenox_problem_parse_problem_neutron_diffusion;
    feenox.pde.init_parser_particular = feenox_problem_init_parser_neutron_diffusion;
      
  } else {
    feenox_push_error_message("unknown problem type '%s'", token);
    return FEENOX_ERROR;
      
  }
    
  return FEENOX_OK;
}
