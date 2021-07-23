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
 * The reference distribution is Debian stable. All the required dependencies should be available in any decently-modern Debian repositories.
 * Support for other architectures (mainly the other two non-server end-user mainstream OSes which are non-free and we do not want to mention explicitly) is encouraged but not required.

 
## Languages

C++ is a great language with used to solve problems it solves naturally well without being forced to solve it (see [LibreBlackjack](https://seamplex.com/blackjack/)). C++ is a terrible language whenever else. It is messy, overwhelming, complicated, not rubust, not easy to debug (especially with templates). The cons are far more than the pros.

Fortran is a terrible language. Period.

Go & Rust, I think still nobody knows. But Rust's automatic memory handling is not a point strong enough for CAE/DAE/FEA software.

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
 
   Libraries written in C++ are allowed as long as they provide C wrappers and do not meddle with stuff we do not need nor want.
   
 * Keep Fortran even farther away. It is allowed only for existing libaries dating from ancient well-tested and optimized code, but for nothing else.
 * Modern high-level languages like Python or Julia are targets and not sources of FeenoX.
 * For documentation and comments within the code, American English should be used.


## Programming IDEs

### Netbeans

To work on FeenoX using [NetBeans](https://netbeans.apache.org/), first make sure you have the C/C++ module working. If you don't, do this:

> In Netbeans go to Tools->Plugins->Settings.
> Entry NetBeans 8.2 Plugin Portal is already present.
> Click the checkbox next to this entry.
> Switch to Available Plugins tab, click Check for Newest.
> C/C++ is now on the list. 

Then create a new C/C++ project File->New Project... choose C/C++ Project with Existing Sources click Next, browse to find the root directory of your local FeenoX sources. It is important to have the `configure` script already created there, so make sure you have run `./autogen.sh`. Make sure the configuration mode is set to Automatic and that NetBeans detected the configure script by adding “using configure” next to “Automatic.” Click to create the project. This will create a directory `nbproject` which will be ignored by `.gitignore`.

## Makefiles

 * FeenoX uses the [GNU Autools](https://en.wikipedia.org/wiki/GNU_Autotools) (i.e. [Autoconf](https://www.gnu.org/software/autoconf/) and [Automake](https://www.gnu.org/software/automake/)).
 * If you really feel that you have to use [CMake](https://cmake.org/) for your contributions, feel free to do so (UNIX rule of diversity) but make sure that at the end of the day `./configure && make` still works.

## Test suite

 * The directory `tests` contains the test suite with shell scripts that return appropriate errorlevels according to [Automake’s generic test scripts](https://www.gnu.org/software/automake/manual/html_node/Scripts_002dbased-Testsuites.html). In a nutshell:
 
   > When no test protocol is in use, an exit status of 0 from a test script will denote a success, an exit status of 77 a skipped test, an exit status of 99 a hard error, and any other exit status will denote a failure. 
   
 * If you add a new feature please also write a test script not just to check your feature works but to prevent further changes by other people (or even by yourself) from breaking the feature.
 * Try to make the check results meaningful and not just check for random results. Try to create tests cases with analytical solution or use benchmarks with known results.
 * We will eventually add some code coverage tools to have as most lines covered by at least one test.
 
## Coding style

 * K&R 1TBS with no tabs and two spaces per indent <https://en.wikipedia.org/wiki/Indentation_style#Variant:_1TBS_(OTBS)>
   
   ```c
   void checknegative(x) {
     if (x < 0) {
       puts("Negative");
     } else {
       nonnegative(x);
     }
   }
   ```
 
 * Do not worry about long lines. Only wrap lines when it makes sense to from a logic point of view not becuase "it looks bad on screen" because “screen” can be anything from a mobile phone to a desktop with many 24" LED monitors.
 * Make sure you understand and follow the 17 rules of UNIX philosophy <https://en.wikipedia.org/wiki/Unix_philosophy>
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

## Virtual methods

Even though we use C, we can still have some “virtual methods” by using function pointers. So in a FEM formulation, the routines that build the stiffness matrix are mostly independent of the problem type (for the same family, say elliptic problems) but in the integrands. So a good practice is to have a common set of routines that loop over elements and over gauss points and then at the inner loop a virtual method is called which depends on the particular problem (thermal, mechanical, etc). This is accomplished by using function pointers so all problems have to provide more or less the same sets of routines: initialization, integrands, BCs, etc. See the basic problems to see how this idea works.

Reading and writing mesh/post-processing files work the same way. Each format has to provide a virtual reader/writer method.
 
## Memory management

 * Check all `malloc()` calls for `NULL`. You can use the `feenox_check_alloc()/feenox_check_alloc_null()` macros.
 * Use the macro `feenox_free()` instead of plain `free()`. The former explictly makes the pointer equal to `NULL` after freeing it, which is handy.
 * Use `valgrind` to check for invalid memory access and leaks
 
    > Memory analysis tools such as valgrind can be useful, but don’t complicate a program merely to avoid their false alarms. For example, if memory is used until just before a process exits, don’t free it simply to silence such a tool. 
    >
    > GNU Coding Standars  
    > <https://www.gnu.org/prep/standards/html_node/Memory-Usage.html>

   Mind that FeenoX might be used in parametric or minimization mode which would re-allocate some stuff so make sure you know which alarms you ignore.
 
## Naming conventions

 * Use snake case such as in `this_is_a_long_name`.
 * All functions ought to start with `feenox_`. This is the small price we need to pay in order to keep a magnificent beast like C++ away from our lives (those who can). The names of functions should go from general to particular such as `feenox_expression_parse()` and `feenox_expression_eval()` (and not `feenox_parse_expression()`/`feenox_eval_expression()`) so all function related with expressions can be easily found. There are exceptions, like functions which do similar tasks such as `feenox_add_assignemnt()` and `feenox_add_instructions()`. Here the `add` part is the common one.
 * In a similar way, “virtual” methods should add the particularity at the end, like 
 
    - `feenox_build_element_volumetric_gauss_point()`---the function pointer which is invoked in the code
    - `feenox_build_element_volumetric_gauss_point_thermal()`---the particular function the pointer will point to and which is actually called
 
## Comments

 * Use single-line comments `//` to add comments to the code so we can easily _comment out_ certain parts of code using multiline comments `/*`---`*/` while developing new features.
 * Explain any non-trivial block or flag that needs to be set. Example
 
   ```c
     switch (c = fgetc(file_ptr)) {
       case '"':
         // if there's an escaped quote, we take away the escape char 
         // and put a magic marker 0x1e, afterwards in get_next_token() 
         // we change back the 0x1e with the unescaped quote
         feenox_parser.line[i++] = 0x1e;
   ```
 
 * Focus on the why and not on the how, except for very complex loops.
 * All nice-to-have things that are welcome to be done should be written as
 
   ```c
   // TODO: allow refreshing file data before each transiet step
   ```
 * Features that might or might not be added should be written as questions
 
   ```c
   // TODO: should we allow unquoted names in the $PhysicalNames section?
   ```

## Indices in PDEs

-------------------------------------------------------------
   Index    Loop over...
----------- -------------------------------------------------
   `i`      elements or cells
   
   `j`      nodes, either global or local. If there are loops
            which need both, use `j_local` and `j_global`.
            
   `k`      dummy index         

   `m`      dimensions
   
   `d`      degree of freedoms
   
   `v`      Gauss points
-------------------------------------------------------------

 
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
 * TLDR;
    1. Use Pandoc-flavored Markdown as the main source.
    2. Knock yourself out with LaTeX maths.
    3. Bitmaps are off the table for figures.

 
# What we program in FeenoX

 * The features described in the SRS.

# Code of coduct

 * See the (code of conduct)[CODE_OF_COUNDUCT.md] for participating in the commnity.

 
# Release plans

 * v0.x for first development until partially fulfilling the SRS
 * v1.x first stable release meaning:
    - usable features
    - full documentation matching the code features
 * v2.x further features and improvements 
 
