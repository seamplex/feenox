---
title: FeenoX programming guide
language: en-US
---

# Why we program FeenoX

The main objective is to comply with the Software Requirements Specification. Within this goal, there are three levels of importance:

 1. FeenoX is Free Software as defined by the Free Software Foundation. The four essential freedoms ought to be assured. Feenox is GPLv3+ so make sure you understand what "free-as-in-free-speech" means and all the details that the license implies regarding using other libraries and code copyright.
 2. FeenoX is Open Source as defined by The Open Source initiative. Stick to open standards, formats, practices, etc. Make sure you understand what the difference between "free software" and "open source" is.
 3. Only after making sure every piece of code meets the two criteria above, the technical and/or efficiency aspects mentioned in the SRS are to be considered.

# How we program FeenoX


## Operating systems

 * The main target operating system is GNU/Linux, particularly `linux-amd64`.
 * The reference distribution is Debian. All the required dependencies should be available in the Debian repositories.
 * Support for other architectures is encouraged but not required.

 
## Languages

 * Stick to plain C.
 
   > C enables us to build data structures for storing sparse matrices, solver information, etc. in ways that Fortran simply does not allow. ANSI C is a complete standard that all modern C compilers support. The language is identical on all machines. C++ is still evolving and compilers on different machines are not identical. Using C function pointers to provide data encapsulation and polymorphism allows us to get many of the advantages of C++ without using such a large and more complicated language.
   >
   > PETSc FAQs  
   > <https://www.mcs.anl.gov/petsc/documentation/faq.html#why-c>
   
 * Keep C++ away.
 
   > C++ is a horrible language. It's made more horrible by the fact that a lot of substandard programmers use it, to the point where it's much much easier to generate total and utter crap with it. Quite frankly, even if the choice of C were to do *nothing* but keep the C++ programmers out, that in itself would be a huge reason to use C.
   >
   > Linus Torvalds about why Git is written in C  
   > <http://harmful.cat-v.org/software/c++/linus>
 
   Libraries written in C++ are allowed as long as they provide C wrappers and do not meddle with stuff we do not need.
   
 * Keep Fortran even farther away. It is allowed only for existing libaries dating from ancient well tested and optimized code, but for nothing else.
 * Modern high-level languages like Python or Julia are targets and not sources of FeenoX.
 * For documentation and comments within the code, American English should be used.

## Makefiles

 * Use autotools and friends and comply with the GNU requirements.
 * CMake is allowed as being part of other libraries but not for any sources directly related to FeenoX.

## Coding style

 * K&R
 * no tabs
 * two spaces per indent
 * don't worry about long lines, only wrap lines when it makes sense to from a logic point of view not becuase "it looks bad on screen"
 * Follow the 17 rules of UNIX philosophy
    1. modularity
    2. clarity
    3. composition
    4. separation
    5. simplicity
    6. parsimony
    7. transparency
    8. robustness
    9. representation
    10. least surprise
    11. silence
    12. repair
    13. economy
    14. generation
    15. optimization
    16. diversity
    17. extensibility
 
 
## Memory management

 * Check all `malloc()` calls for `NULL`. You can use the `feenox_check_alloc()/feenox_check_alloc_null()` macros.
 * Use `valgrind` to check for invalid memory access and leaks
 
    > Memory analysis tools such as valgrind can be useful, but don’t complicate a program merely to avoid their false alarms. For example, if memory is used until just before a process exits, don’t free it simply to silence such a tool. 

   Mind that FeenoX might be used in parametric or minimization mode which would re-allocate some stuff so make sure you know which alarms you ignore.
 
## Naming conventions

 * Use snake case such as in `this_is_a_long_name`.
 * All functions ought to start with `feenox_`. This is the small price we need to pay in order to keep a magnificent beast like C++ away from our lives (those who can). The name should go from general to particular such as `feenox_expression_parse()` and `feenox_expression_eval()` (and not `feenox_parse_expression()`/`feenox_eval_expression()`) so all function related with expressions can be easily found. There are exceptions, like functions which do similar tasks such as `feenox_add_assignemnt()` and `feenox_add_instructions()`. Here the `add` part is the common one.
 
## Comments

 * Use single-line comments for commenting the code `//` so we can use multiline comments `/*`-`*/` to _comment out_ certain parts of code while developing new features.
 
## Git workflow

 * Of course, use `git`. This may seem obvious but predecessors of this project have used Subversion, Bazaar and Mercurial in that order. I have myself used CVS (in the past century).
 * Only team members are allowed to commit directly to the main branch.
 * All contributions ought to come from pull/merge requests either from forked repositories and/or non-main branches.
 * Issues can be opened forked for
    - bug reporting
    - feature requests
    - development discussion
    - general questions (installation, usage, etc)
 
## Standards

 * Try to adhere to POSIX as much as possible. Eventually all operating systems will adopt it.

## Mentioning other libraries, programs, codes, etc.

 * Try no to mention any piece of software which is not free and open source.

## Documentation

 * See the README in the `doc` directory.

 
# What we program in FeenoX

 * The features described in the SRS.
 
# Release plans

 * v0.x for first development until partially fulfilling the SRS
 * v1.x first stable release meaning:
    - usable features
    - full documentation matching the code features
 * v2.x further features and improvements 
 
