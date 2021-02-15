#include "feenox.h"
extern feenox_t feenox;

int feenox_add_instruction(int (*routine)(void *), void *argument) {
  return (feenox_add_instruction_and_get_ptr(routine, argument) == NULL);
}


instruction_t *feenox_add_instruction_and_get_ptr(int (*routine)(void *), void *argument) {

  instruction_t *instruction;

  if (feenox.dae.reading_daes == 1) {
    feenox_push_error_message("cannot have instructions within DAEs");
    return NULL;
  }
  
  feenox_check_alloc(instruction = calloc(1, sizeof(instruction_t)), NULL);
      
  instruction->routine = routine;
  instruction->argument = argument;
  
  if (feenox.active_conditional_block != NULL) {
    if (feenox.active_conditional_block->else_of == NULL && feenox.active_conditional_block->first_true_instruction == NULL) {
      feenox.active_conditional_block->first_true_instruction = instruction;
    } else if (feenox.active_conditional_block->else_of != NULL && feenox.active_conditional_block->first_false_instruction == NULL) {
      feenox.active_conditional_block->first_false_instruction = instruction;
    }
  }
  
  feenox.last_defined_instruction = instruction;
  LL_APPEND(feenox.instructions, instruction);

  return instruction;
}
