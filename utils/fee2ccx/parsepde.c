#include "feenox.h"

int feenox_pde_parse_problem_type(const char *token) {

  if (strcasecmp(token, "mechanical") == 0) {
    feenox.pde.parse_problem = feenox_problem_parse_problem_mechanical;
  } else {
    feenox_push_error_message("unknown problem type '%s'", token);
    return FEENOX_ERROR;
  }
    
  return FEENOX_OK;
}
