#include "feenox.h"

int feenox_parse_abort(void) {
  
///kw+ABORT+usage ABORT
///kw+ABORT+desc Catastrophically abort the execution and quit feenox.
///kw+ABORT+detail Whenever the instruction `ABORT` is executed, feenox quits without closing files
///kw+ABORT+detail or unlocking shared memory objects. The objective of this instruction is, as
///kw+ABORT+detail illustrated in the examples, either to debug complex input files and check the
///kw+ABORT+detail values of certain variables or to conditionally abort the execution using `IF`
///kw+ABORT+detail clauses.
///kw+ABORT+example abort1.was abort2.was
  
  // TODO
/*  
  if (feenox_define_instruction(feenox_instruction_abort, NULL) == NULL) {
    return FEENOX_PARSER_ERROR;
  }
 */
  return FEENOX_PARSER_OK;
}

