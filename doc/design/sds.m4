---
title: FeenoX Software Design Specification
lang: en-US
abstract: This design specification addresses the (hypothetical) Software Requirement Specification for developing a piece of free and open source engineering software with certain specifications, defined in an imaginary tender.
number-sections: true
---

dnl --------------------------------------------------------------------------------
esyscmd(awk -f quote.awk 010-introduction.md)


Besides noting that software being _free_ (regarding freedom, not price) does not imply the same as being _open source_, the requirement is clear in that the tool has to be both _free_ and _open source_, a combination which is usually called FOSS. This condition leaves all of the well-known non-free finite-element solvers in the market out of the tender.

FeenoX is licensed under the terms of the GNU General Public License version\ 3 or, at the user convenience, any later version. This means that users get the four essential freedoms:

 0. The freedom to _run_ the program as they wish, for _any_ purpose.
 1. The freedom to _study_ how the program works, and _change_ it so it does their computing as they wish.
 2. The freedom to _redistribute_ copies so they can help others.
 3. The freedom to _distribute_ copies of their _modified_ versions to others.

There are some examples of pieces of computational software which are described as “open source” in which even the first of the four freedoms is denied. In the nuclear industry, it is the case of a Monte Carlo particle-transport program that requests users to sign an agreement about the objective of its usage before allowing its execution. The software itself might be open source because the source code is provided after signing the agreement, but it is not free at all.
 
Licensing FeenoX as GPLv3+ also implies that the source code and all the scripts and makefiles needed to compile and run it are available for anyone that requires it. Anyone wanting to modify the program either to fix bugs, improve it or add new features is free to do so. And if they do not know how to program, the have the freedom to hire a programmer to do it without needing to ask permission to the original authors. 

Nevertheless, since the original authors are the copyright holders, they still can use it to either enforce or prevent further actions the users that receive FeenoX under the GPLv3+. In particular, the license allows re-distribution of modified versions only if they are clearly marked as different from the original and only under the same terms of the GPLv3+. It is forbidden to include either totally or partially FeenoX in another piece of software whose license is incompatible with the GPLv3+.


dnl --------------------------------------------------------------------------------
esyscmd(awk -f quote.awk 020-objective.md)

The choice of the initial supported features is based on the types of problem that the FeenoX' precursor codes (namely wasora, Fino and milonga) already have been supporting since more than ten years now. It is also a first choice so work can be bounded and a subsequent road map and release plans can be designed. FeenoX' first version includes a subset of the required functionality, namely

 * dynamical systems
 * plant control dynamics
 * mechanical elasticity
 * heat conduction
 * structural modal analysis
 
FeenoX is designed to be developed and executed under GNU/Linux, which is the architecture og more than 95% of the internet servers which we collectively call “the cloud.” It should be noted that GNU/Linux is a POSIX-compliant version of UNIX and that FeenoX follows the rules of UNIX philosophy ([@sec:unix]) regarding its computational implementation code. Besides POSIX, FeenoX also uses MPI which is a well-known industry standard for massive parallel executions of processes, both in multi-core hosts and multi-hosts environments. Finally, if performance and/or scalability are not important issues, FeenoX can be run in a (properly cooled) local PC or laptop.

The requirement to run in the cloud and scale up as needed rules out some of the FOSS solvers available online, such as CalculiX.


dnl --------------------------------------------------------------------------------
esyscmd(awk -f quote.awk 030-scope.md)

Indeed, FeenoX is designed to work very much like a transfer function between two (or more) files---usually the FeenoX input file and the problem mesh file---and zero or more output files---usually the terminal output and a VTK post-processing file---as illustrated in [@fig:transfer].

![FeenoX working as a transfer function between input and output files](transfer.svg){#fig:transfer width=50%}

In some particular cases, FeenoX can also provide an API for high-level interpreted languages such as Python or Julia such that a problem can be completely defined in a script, increasing also flexiblity.


As already stated, FeenoX is designed and implemented following the UNIX philosophy in general and Eric Raymond's 17 Unix Rules ([sec:unix]) in particular. One of the main ideas is the rule of _separation_ that essentially implies to separate mechanism from policy, that in the computational engineering world translates into separating the frontend from the backend. Even though most FEA programs eventually separate the interface from the solver up to some degree, there are cases in which they are still dependent such that changing the former needs updating the latter.

From the very beginning, FeenoX is designed as a pure backend which should nevertheless provide appropriate mechanisms for different frontends to be able to communicate and to provide a friendly interface for the final user. Yet, the separation is complete in the sense that the nature of the frontends can radically change (say from a desktop-based point-and-click program to a web-based immersive augmented-reality application) without needing the modify the backend. Not only far more flexibility is given by following this path, but also develop efficiency and quality is encouraged since programmers working on the lower-level of an engineering tool usually do not have the skills needed to write good user-experience interfaces, and conversely.

In the very same sense, FeenoX does not discretize continuous domains for PDE problems itself, but relies on separate tools for this end. Fortunately, there already exists one meshing tool which is FOSS (GPLv2) and shares most (if not all) of the design basis principles with FeenoX: the three-dimensional finite element mesh generator Gmsh.
Strictly speaking, FeenoX does not need to be used along with Gmsh but with any other mesher able to write meshes in Gmsh's format `.msh`. But since Gmsh also

 * is free and open source,
 * works also in a transfer-function-like fashion,
 * runs natively on GNU/Linux,
 * has a similar (but more comprehensive) API for Python/Julia,
 * etc
 
\noindent it is a perfect match for FeenoX. Even more, it provides suitable domain decomposition methods (through other FOSS third-party libraries such as Metis) for scaling up large problems,


ejemplo de input - alguno de NAFEMS




dnl --------------------------------------------------------------------------------
esyscmd(awk -f quote.awk 040-architecture.md)

C, quote petsc, flat memory address space -> that's what virtual servers have!

posix

dnl --------------------------------------------------------------------------------
esyscmd(awk -f quote.awk 050-deployment.md)

autoconf vs. cmake, rule of diversity 

gcc, clang

blas, lapack, atlas

binaries freely available at seamplex.com

Linux, mac, windows 

dnl --------------------------------------------------------------------------------
esyscmd(awk -f quote.awk 060-execution.md)

command line args for paramteric runs from a shell script

script friendly

dnl --------------------------------------------------------------------------------
esyscmd(awk -f quote.awk 070-efficienciy.md)

cloud, rent don't buy
benchmark and comparisons


dnl --------------------------------------------------------------------------------
esyscmd(awk -f quote.awk 080-scalability.md)

PETSc, MPI
error handling, rule of repair
check all malloc() calls


dnl --------------------------------------------------------------------------------
esyscmd(awk -f quote.awk 090-flexibility.md)

FeenoX comes from nuclear + experience (what to do and what not to do)


dnl --------------------------------------------------------------------------------
esyscmd(awk -f quote.awk 100-extensibility.md)

user-provided routines
skel for pdes and annotated models


dnl --------------------------------------------------------------------------------
esyscmd(awk -f quote.awk 110-interoperability.md)

UNIX
POSIX


dnl --------------------------------------------------------------------------------
esyscmd(awk -f quote.awk 120-interfaces.md)

dnl --------------------------------------------------------------------------------
esyscmd(awk -f quote.awk 130-input.md)

dar ejemplos
comparar con <https://cofea.readthedocs.io/en/latest/benchmarks/004-eliptic-membrane/tested-codes.html>

macro-friendly inputs, rule of generation

dnl --------------------------------------------------------------------------------
esyscmd(awk -f quote.awk 140-output.md)

100% user-defined output with PRINT, rule of silence
rule of economy, i.e. no RELAP
yaml/json friendly outputs
vtk (vtu), gmsh


dnl --------------------------------------------------------------------------------
esyscmd(awk -f quote.awk 150-qa.md)



dnl --------------------------------------------------------------------------------
esyscmd(awk -f quote.awk 160-reproducibility.md)



dnl --------------------------------------------------------------------------------
esyscmd(awk -f quote.awk 170-testing.md)


dnl --------------------------------------------------------------------------------
esyscmd(awk -f quote.awk 180-bugs.md)


dnl --------------------------------------------------------------------------------
esyscmd(awk -f quote.awk 190-verification.md)


open source (really, not like CCX -> mostrar ejemplo)
GPLv3+ free
Git + gitlab, github, bitbucket


dnl --------------------------------------------------------------------------------
esyscmd(awk -f quote.awk 200-validation.md)


dnl --------------------------------------------------------------------------------
esyscmd(awk -f quote.awk 210-documentation.md)

it's not compact, but almost!
Compactness is the property that a design can fit inside a human being's head. A good practical test for compactness is this: Does an experienced user normally need a manual? If not, then the design (or at least the subset of it that covers normal use) is compact.
unix man page
markdown + pandoc = html, pdf, texinfo
 

dnl --------------------------------------------------------------------------------
# Appendix: Rules of UNIX philosophy {#sec:unix}

Rule of Modularity

:    Developers should build a program out of simple parts connected by well defined interfaces, so problems are local, and parts of the program can be replaced in future versions to support new features. This rule aims to save time on debugging code that is complex, long, and unreadable.

Rule of Clarity

:    Developers should write programs as if the most important communication is to the developer who will read and maintain the program, rather than the computer. This rule aims to make code as readable and comprehensible as possible for whoever works on the code in the future.

Rule of Composition

:    Developers should write programs that can communicate easily with other programs. This rule aims to allow developers to break down projects into small, simple programs rather than overly complex monolithic programs.

Rule of Separation

:    Developers should separate the mechanisms of the programs from the policies of the programs; one method is to divide a program into a front-end interface and a back-end engine with which that interface communicates. This rule aims to prevent bug introduction by allowing policies to be changed with minimum likelihood of destabilizing operational mechanisms.

Rule of Simplicity

:    Developers should design for simplicity by looking for ways to break up program systems into small, straightforward cooperating pieces. This rule aims to discourage developers’ affection for writing “intricate and beautiful complexities” that are in reality bug prone programs.

Rule of Parsimony

:    Developers should avoid writing big programs. This rule aims to prevent overinvestment of development time in failed or suboptimal approaches caused by the owners of the program’s reluctance to throw away visibly large pieces of work. Smaller programs are not only easier to write, optimize, and maintain; they are easier to delete when deprecated.

Rule of Transparency

:    Developers should design for visibility and discoverability by writing in a way that their thought process can lucidly be seen by future developers working on the project and using input and output formats that make it easy to identify valid input and correct output. This rule aims to reduce debugging time and extend the lifespan of programs.

Rule of Robustness

:    Developers should design robust programs by designing for transparency and discoverability, because code that is easy to understand is easier to stress test for unexpected conditions that may not be foreseeable in complex programs. This rule aims to help developers build robust, reliable products.

Rule of Representation

:    Developers should choose to make data more complicated rather than the procedural logic of the program when faced with the choice, because it is easier for humans to understand complex data compared with complex logic. This rule aims to make programs more readable for any developer working on the project, which allows the program to be maintained.

Rule of Least Surprise

:    Developers should design programs that build on top of the potential users' expected knowledge; for example, ‘+’ in a calculator program should always mean 'addition'. This rule aims to encourage developers to build intuitive products that are easy to use.

Rule of Silence

:    Developers should design programs so that they do not print unnecessary output. This rule aims to allow other programs and developers to pick out the information they need from a program's output without having to parse verbosity.

Rule of Repair

:    Developers should design programs that fail in a manner that is easy to localize and diagnose or in other words “fail noisily”. This rule aims to prevent incorrect output from a program from becoming an input and corrupting the output of other code undetected.

Rule of Economy

:    Developers should value developer time over machine time, because machine cycles today are relatively inexpensive compared to prices in the 1970s. This rule aims to reduce development costs of projects.

Rule of Generation

:    Developers should avoid writing code by hand and instead write abstract high-level programs that generate code. This rule aims to reduce human errors and save time.

Rule of Optimization

:    Developers should prototype software before polishing it. This rule aims to prevent developers from spending too much time for marginal gains.

Rule of Diversity

:    Developers should design their programs to be flexible and open. This rule aims to make programs flexible, allowing them to be used in ways other than those their developers intended.

Rule of Extensibility

:    Developers should design for the future by making their protocols extensible, allowing for easy plugins without modification to the program's architecture by other developers, noting the version of the program, and more. This rule aims to extend the lifespan and enhance the utility of the code the developer writes.
