# FeenoX programming guide

## Languages

 * Stick to plain C.
 * Keep C++ away. Libraries written in C++ are allowed as long as they provide C wrappers and do not meddle with stuff we do not need.
 * Keep Fortran even farther away. It is allowed only for existing libaries dating from ancient well tested and optimized code, but for nothing else.

## Makefiles

 * Use autotools and friends and comply with the GNU requirements.
 * CMake is allowed as being part of other libraries but not for any sources directly related toFeenoX.

## Coding style

 * K&R
 * no tabs
 * two spaces per indent
 * don't worry about long lines, only wrap lines when it makes sense to from a logic point of view not becuase "it looks bad on screen"

## Naming conventions

 * Use snake case such as in `this_is_a_long_name`.
 * All functions ought to start with `feenox_`. This is the small price we need to pay in order to keep a magnificent beast like C++ away from our lives (those who can). The name should go from general to particular such as `feenox_expression_parse()` and `feenox_expression_eval()` (and not `feenox_parse_expression()`/`feenox_eval_expression()`) so all function related with expressions can be easily found. There are exceptions, like functions which do similar tasks such as `feenox_add_assignemnt()` and `feenox_add_instructions()`. Here the `add` part is the common one.
 
## Git workflow

 * Only team members are allowed to commit directly to the main branch.
 * All contributions ought to come from pull/merge requests either from forked repositories and/or non-main branches.
 
## Standards

 * Try to adhere to POSIX as much as possible. Eventually all OSes will use it.

## Mentioning other libraries, programs, codes, etc.

 * Try no to mention any piece of software which is not free and open source.

## Documentation

 * See the README in the `doc` directory.
