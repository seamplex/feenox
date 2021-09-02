#include "feenox.h"
#include "neutron_diffusion_fem.h"
extern feenox_t feenox;
neutron_diffusion_fem_t neutron_diffusion_fem;
#include "../../parser/parser.h"
extern feenox_parser_t feenox_parser;

int feenox_problem_parse_problem_neutron_diffusion_fem(const char *token) {
  
  if (token != NULL) {
    if (strcasecmp(token, "GROUPS") == 0) {
      double xi = 0;
      feenox_call(feenox_parser_expression_in_string(&xi));
      neutron_diffusion_fem.groups = (unsigned int)(xi);
      
    } else {
      feenox_push_error_message("undefined keyword '%s'", token);
      return FEENOX_ERROR;
    }
  } 
  
  return FEENOX_OK;
}  

