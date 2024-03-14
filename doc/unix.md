
In 1978, Doug McIlroy---the inventor of Unix pipes and one of the founders of the Unix tradition---stated:

 i. Make each program do one thing well. To do a new job, build afresh rather than complicate old programs by adding new features.

 ii. Expect the output of every program to become the input to another, as yet unknown, program. Don't clutter output with extraneous information. Avoid stringently columnar or binary input formats. Don't insist on interactive input.

 iii. Design and build software, even operating systems, to be tried early, ideally within weeks. Don't hesitate to throw away the clumsy parts and rebuild them.

 iv. Use tools in preference to unskilled help to lighten a programming task, even if you have to detour to build the tools and expect to throw some of them out after you've finished using them.

He later summarized it this way:

 > This is the Unix philosophy: Write programs that do one thing and do it well. Write programs to work together. Write programs to handle text streams, because that is a universal interface.

 
FeenoX explicitly followed the above ideas from scratch, especially the for sentences in bullet ii.
It is even, like Unix itself, a third-system effect where clumsy parts of previous attempts were thrown away and rebuilt from scratch.
The following sections explain how each of the seventeen rules was taken into account when designing and implementing FeenoX.
 
# Rule of Modularity {#sec:unix-modularity}

> Developers should build a program out of simple parts connected by well defined interfaces, so problems are local, and parts of the program can be replaced in future versions to support new features. This rule aims to save time on debugging code that is complex, long, and unreadable.

FeenoX is designed to be as lightweight as possible.
On the one hand, it relies on third-party high-quality libraries to do the heavy mathematical weightlifting such as

 * [GNU Scientific Library](https://www.gnu.org/software/gsl/) for general mathematics,
 * [SUNDIALS IDA](https://computing.llnl.gov/projects/sundials/ida) for ODEs and DAEs,
 * [PETSc](https://petsc.org/) for linear, non-linear and transient PDEs, and
 * [SLEPc](http://slepc.upv.es/) for PDEs involving eigen problems

because these libraries were written by professional programmers using algorithms designed by professional mathematicians.
Yet-to-be-discovered improved mathematical schemes and/or coding algorithms can be eventually used by FeenoX by just updating those dependencies, which for sure will keep their well-defined interfaces (because they are programmed by professional programmers).

Moreover, the extensibility feature (@sec:unix-extensibility) of having each PDE in separate directories which can be added or removed at compile time without changing any line of the source code goes into this direction as well.
Relying of C function pointers allows (in principle) to replace these "virtual" methods with other ones using the same interface.

> Note that our (human) languages in general and words in particular shape and limit the way we think.
> Fortran's concept of "modules" is _not_ the same as Unix's concept of "modularity."
> I wish two different words had been used.

# Rule of Clarity {#sec:unix-clarity}

> Developers should write programs as if the most important communication is to the developer who will read and maintain the program, rather than the computer. This rule aims to make code as readable and comprehensible as possible for whoever works on the code in the future.

Of course there might be a confirmation bias in this section because every programmer thinks their code is clear (and everybody else's is not).
But the first design decision to fulfill this rule is the programming language:
there is little change to fulfill it with Fortran.
One might argue that C++ can be clearer than C in some points, but for the vast majority of the source code they are equally clear.
Besides, C is far simpler than C++ (see rule of simplicity).


The second decision is not about the FeenoX source code but about FeenoX inputs: clear human-readable input files without any extra unneeded computer-level nonsense.
The two illustrative cases are the NAFEMS [LE10](https://www.seamplex.com/feenox/examples/mechanical.html#nafems-le10-thick-plate-pressure-benchmark) & [LE11](https://www.seamplex.com/feenox/examples/mechanical.html#nafems-le11-solid-cylindertapersphere-temperature-benchmark) benchmarks, where there is a clear one-to-one correspondence between the "engineering" formulation and the input file FeenoX understands.

# Rule of Composition {#sec:unix-composition}

> Developers should write programs that can communicate easily with other programs. This rule aims to allow developers to break down projects into small, simple programs rather than overly complex monolithic programs.

Previous designs of FeenoX' predecessors used to include instructions to perform parametric sweeps( and even optimization loops), non-trivial macro expansions using M4 and even execution of arbitrary shell commands. These non-trivial operations were removed from FeenoX to focus on the rule of composition, paying especially attention to easing the inclusion of calling the `feenox` binary from shell scripts, enforcing the composition with other Unix-like tools.
Emphasis has been put on adding flexibility to programmatic generation of input files (see also rule of generation in @sec:unix-generation) and the handling and expansion of command-line arguments to increase the composition with other programs.

Moreover, the output is 100% controlled by the user at run-time so it can be tailored to suit any other programs’ input needs as well.
An illustrative example is [creating professional-looking tables with results using AWK & LaTeX](https://www.seamplex.com/feenox/doc/sds.html#sec:interoperability).


# Rule of Separation {#sec:unix-separation}

> Developers should separate the mechanisms of the programs from the policies of the programs; one method is to divide a program into a front-end interface and a back-end engine with which that interface communicates. This rule aims to prevent bug introduction by allowing policies to be changed with minimum likelihood of destabilizing operational mechanisms.

FeenoX relies of the rule of separation (which also links to the next two rules of simplicity and parsimony) from the very beginning of its design phase.
It was explicitly designed as a glue layer between a mesher like Gmsh and a post-processor like Gnuplot, Gmsh or Paraview.
This way, not only flexibility and diversity (see #sec:unix-diversity) can be boosted, but also technological changes can be embraced with little or no effort. For example, [CAEplex](https://www.caeplex.com) provides a web-based platform for performing thermo-mechanical analysis on the cloud running from the browser. Had FeenoX been designed as a traditional desktop-GUI program, this would have been impossible.
If in the future CAD/CAE interfaces migrate into virtual and/or augmented reality with interactive 3D holographic input/output devices, the development effort needed to use FeenoX as the back end is negligible.


# Rule of Simplicity {#sec:unix-simplicity}

> Developers should design for simplicity by looking for ways to break up program systems into small, straightforward cooperating pieces. This rule aims to discourage developers’ affection for writing “intricate and beautiful complexities” that are in reality bug prone programs.

The main source of simplicity comes from the design of the syntax of the input files, discussed in detail in the [SDS](https://www.seamplex.com/feenox/doc/sds.html#sec:input):

 * English-like self-evident input files matching as close as possible the problem text.
 * Simple problems need simple input.
 * Similar problems need similar inputs.
 * If there is a single material there is no need to link volumes to properties.


# Rule of Parsimony {#sec:unix-parsimony}

> Developers should avoid writing big programs. This rule aims to prevent overinvestment of development time in failed or suboptimal approaches caused by the owners of the program’s reluctance to throw away visibly large pieces of work. Smaller programs are not only easier to write, optimize, and maintain; they are easier to delete when deprecated.

We already said that FeenoX is a glue layer between a mesher and a post-processing tool.
Even more, at another level, it acts as two glue layers between the mesher and PETSc, and PETSc and the post-processor:

![](transfer-le10-zoom.svg)

On the other hand, we also already stated that FeenoX was written from scratch after throwing away clumsy code from two previous attempts.
For instance, these previous versions used to implement parametric and optimization schemes.
Instead, in FeenoX, these type of runs have to be driven from an outer script (Bash, Python, etc.)
 

 

# Rule of Transparency {#sec:unix-transparency}

> Developers should design for visibility and discoverability by writing in a way that their thought process can lucidly be seen by future developers working on the project and using input and output formats that make it easy to identify valid input and correct output. This rule aims to reduce debugging time and extend the lifespan of programs.

As with the rule of clarity (@sec:unix-clarity), there is a risk of falling into the confirmation bias because every programmer thinks its code is transparent.
Anyway, FeenoX is written in C99 which is way easier to debug than both Fortran and C++.
Yet, very much like PETSc, FeenoX makes use of structures and function pointers to give the same functionality as C++’s virtual methods without needing to introduce other complexities that make the code base harder to maintain and to debug.

Regarding identification of valid inputs and correct outputs,

 1. The build system includes a `make check` target that runs hundreds of [regressions tests](https://github.com/seamplex/feenox/tree/main/tests).
 2. The code supports verification using the [Method of Manufactured Solutions](https://github.com/seamplex/feenox/tree/main/tests/mms)
 

# Rule of Robustness {#sec:unix-robustness}

> Developers should design robust programs by designing for transparency and discoverability, because code that is easy to understand is easier to stress test for unexpected conditions that may not be foreseeable in complex programs. This rule aims to help developers build robust, reliable products.

Robustness is the child of transparency and simplicity.

# Rule of Representation {#sec:unix-representation}

> Developers should choose to make data more complicated rather than the procedural logic of the program when faced with the choice, because it is easier for humans to understand complex data compared with complex logic. This rule aims to make programs more readable for any developer working on the project, which allows the program to be maintained.

There is a trade off between clarity and efficiency.
However, avoiding Fortran should already fulfill this rule.
FeenoX uses C structures with function pointers, which make it far simple to understand than similar Fortran-based FEM tools.
Just compare the source directories of FeenoX and CalculiX.
Take for instance the file [`stress.c`](https://github.com/seamplex/feenox/blob/main/src/pdes/mechanical/stress.c) from [`src/pdes/mechanical`](https://github.com/seamplex/feenox/blob/main/src/pdes/mechanical/stress.c) (which if deleted, will remove support for `mechanical` problems but it will not prevent the compilation of `feenox`) from the former and [`calcstress.f`](https://github.com/calculix/ccx_prool/blob/master/CalculiX/ccx_2.21/src/calcstress.f) (buried inside 2,400 files in [`src`](https://github.com/calculix/ccx_prool/tree/master/CalculiX/ccx_2.21/src)) from the latter.
There might be more illustrative examples showing how FeenoX' design is more representative than of CalculiX, but it is way too hard to understand the source code of the latter (even though the license is supposed to be GPL).

# Rule of Least Surprise {#sec:unix-least-surprise}

> Developers should design programs that build on top of the potential users' expected knowledge; for example, ‘+’ in a calculator program should always mean 'addition'. This rule aims to encourage developers to build intuitive products that are easy to use.

The rules of input syntax have been designed with this rule in mind.
Just note a couple of them:

 * If one needs a problem where the conductivity depends on\ $x$ as $k(x)=1+x$ then the input is
 
   ```feenox
   k(x) = 1+x
   ```
   
 * If a problem needs a temperature distribution given by an algebraic expression $T(x,y,z)=\sqrt{x^2+y^2}+z$ then do
 
   ```feenox
   T(x,y,z) = sqrt(x^2+y^2) + z
   ```
 

# Rule of Silence {#sec:unix-silence}

> Developers should design programs so that they do not print unnecessary output. This rule aims to allow other programs and developers to pick out the information they need from a program's output without having to parse verbosity.

TL;DR: no `PRINT` (or `WRITE_RESULTS`), no output.


# Rule of Repair {#sec:unix-repair}

> Developers should design programs that fail in a manner that is easy to localize and diagnose or in other words “fail noisily”. This rule aims to prevent incorrect output from a program from becoming an input and corrupting the output of other code undetected.

Input errors are detected before the computation is started:
 
```terminal
$ feenox thermal-error.fee 
error: undefined thermal conductivity 'k'
$ 
```

Run-time errors (even inside the numerical libraries) are caught with custom handlers.


# Rule of Economy {#sec:unix-economy}

> Developers should value developer time over machine time, because machine cycles today are relatively inexpensive compared to prices in the 1970s. This rule aims to reduce development costs of projects.

As explained in the [SDS](https://www.seamplex.com/feenox/doc/sds.html#sec:output), output is 100% user-defined so only the desired results are directly obtained instead of needing further digging into tons of undesired data.
The approach of "compute and write everything you can in one single run" made sense in 1970 where CPU time was more expensive than human time,
but not anymore.
Once again, the iconic examples are the NAFEMS [LE10](https://www.seamplex.com/feenox/examples/mechanical.html#nafems-le10-thick-plate-pressure-benchmark) & [LE11](https://www.seamplex.com/feenox/examples/mechanical.html#nafems-le11-solid-cylindertapersphere-temperature-benchmark) benchmarks, where just the required scalar stress at the required location is written into the standard output.

# Rule of Generation {#sec:unix-generation}

> Developers should avoid writing code by hand and instead write abstract high-level programs that generate code. This rule aims to reduce human errors and save time.

Some key points:

 * Input files are M4-like-macro friendly.
 * Parametric runs can be done from scripts through expansion of command line arguments.
 * Documentation is created out of simple Markdown sources and assembled as needed.


# Rule of Optimization {#sec:unix-optimization}

> Developers should prototype software before polishing it. This rule aims to prevent developers from spending too much time for marginal gains.

FeenoX is still "premature" for heavy optimization.
Yet, it is (relatively) faster than other alternatives.
It does use link-time optimization to allow for inlining of small routines.
There is even a FeenoX benchmarking repository that uses Google's Benchmark library to prototype code optimization: <https://github.com/seamplex/feenox-benchmark>.


# Rule of Diversity {#sec:unix-diversity}

> Developers should design their programs to be flexible and open. This rule aims to make programs flexible, allowing them to be used in ways other than those their developers intended.

 * Either Gmsh or Paraview can be used to post-process results.
 * Other formats can be added.

# Rule of Extensibility {#sec:unix-extensibility}

> Developers should design for the future by making their protocols extensible, allowing for easy plugins without modification to the program's architecture by other developers, noting the version of the program, and more. This rule aims to extend the lifespan and enhance the utility of the code the developer writes.

 * FeenoX is GPLv3+. The '+' is for the future.
 * Each PDE has a separate source directory. Any of them can be used as a template for new PDEs, especially `laplace` for elliptic operators.
