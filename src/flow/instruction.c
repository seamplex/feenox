#include "feenox.h"

int feenox_add_instruction(int (*routine)(void *), void *argument) {

  instruction_t *instruction;

  // TODO
/*  
  if (feenox_parser.reading_daes) {
    wasora_push_error_message("cannot have instructions within DAEs");
    return NULL;
  }
*/
  if ((instruction = calloc(1, sizeof(instruction_t))) == NULL) {
    feenox_push_error_message("memory allocation error in feenox_add_instruction()");
    return FEENOX_ERROR;
  }
  instruction->routine = routine;
  instruction->argument = argument;
  
  // TODO
/*  
  if (wasora.active_conditional_block != NULL) {
    if (wasora.active_conditional_block->else_of == NULL && wasora.active_conditional_block->first_true_instruction == NULL) {
      wasora.active_conditional_block->first_true_instruction = instruction;
    } else if (wasora.active_conditional_block->else_of != NULL && wasora.active_conditional_block->first_false_instruction == NULL) {
      wasora.active_conditional_block->first_false_instruction = instruction;
    }
  }
*/
  
  // TODO
//  wasora.last_defined_instruction = instruction;

  LL_APPEND(feenox.instructions, instruction);

  return FEENOX_ERROR;
}
