---
title: FeenoX Software Design Specification
lang: en-US
abstract: This design specification addresses the (hypothetical) Software Requirement Specification for developing a piece of free and open source engineering software with certain specifications, defined in an imaginary tender.
number-sections: true
---

# Introduction {#sec:introduction}

> 
> A cloud-based computational tool (herein after referred to as _the tool_) is required in order to solve engineering problems following the current state-of-the-art methods and technologies impacting the high-performance computing world.
> This (imaginary but plausible) Software Requirements Specification document describes the mandatory features this tool ought to have and lists some features which would be nice the tool had.
> Also it contains requirements and guidelines about architecture, execution and interfaces in order to fulfill the needs of cognizant engineers as of 2021 (and the years to come) are defined. 
> 
> On the one hand, the tool should be applicable to solving industrial problems under stringent efficiency ([@sec:efficiency]) and quality ([@sec:qa]) requirements. It is therefore mandatory to be able to assess the source code for potential performance and verification revision by qualified third parties from all around the world. On the other hand, the initial version of the tool is expected to be a basic framework which might be extended ([@sec:objective] and [@sec:extensibility]) by academic researchers and programmers. It thus should also be free.^[Free as in “free speech,” not as in “free beer.”]
> The detailed licensing terms are left to the offer but it should allow users to solve their problems the way they need and, eventually, to modify and improve the tool to suit their needs. If they cannot program themselves, they should have the freedom to hire somebody to do it for them.



Besides noting that software being _free_ (regarding freedom, not price) does not imply the same as being _open source_, the requirement is clear in that the tool has to be both _free_ and _open source_, a combination which is usually called FOSS. This condition leaves all of the well-known non-free finite-element solvers in the market out of the tender.

FeenoX is licensed under the terms of the GNU General Public License version\ 3 or, at the user convenience, any later version. This means that users get the four essential freedoms:

 0. The freedom to _run_ the program as they wish, for _any_ purpose.
 1. The freedom to _study_ how the program works, and _change_ it so it does their computing as they wish.
 2. The freedom to _redistribute_ copies so they can help others.
 3. The freedom to _distribute_ copies of their _modified_ versions to others.

There are some examples of pieces of computational software which are described as “open source” in which even the first of the four freedoms is denied. In the nuclear industry, it is the case of a Monte Carlo particle-transport program that requests users to sign an agreement about the objective of its usage before allowing its execution. The software itself might be open source because the source code is provided after signing the agreement, but it is not free at all.
 
Licensing FeenoX as GPLv3+ also implies that the source code and all the scripts and makefiles needed to compile and run it are available for anyone that requires it. Anyone wanting to modify the program either to fix bugs, improve it or add new features is free to do so. And if they do not know how to program, the have the freedom to hire a programmer to do it without needing to ask permission to the original authors. 

Nevertheless, since the original authors are the copyright holders, they still can use it to either enforce or prevent further actions the users that receive FeenoX under the GPLv3+. In particular, the license allows re-distribution of modified versions only if they are clearly marked as different from the original and only under the same terms of the GPLv3+. It is forbidden to include either totally or partially FeenoX in another piece of software whose license is incompatible with the GPLv3+.


## Objective {#sec:objective}

> 
> The main objective of the tool is to be able to solve engineering problems which are usually casted as differential-algebraic equations (DAEs) or partial differential equations (PDEs), such as
> 
>  * dynamical systems
>  * plant control dynamics
>  * mechanical elasticity
>  * heat conduction
>  * structural modal analysis
>  * electromagnetism
>  * chemical diffusion
>  * computational fluid dynamics
>  * ...
> 
> on one or more manistream cloud servers, i.e. computers with an architecture (i.e. hardware and operating systems, futher discussed in [@sec:architecture]) that allows them to be available online and accessed remotely either interactively or automatically by other computers as well. Other architectures such as high-end desktop personal computers or even low-end laptops might be supported but they should not the main target. 
>  
> The initial version of the tool must be able to handle a subset of the above list of problem types.
> Afterward, the set of supported problem types, models, equations and features of the tool should grow to include other models as well, as required in\ [@sec:extensibility].


The choice of the initial supported features is based on the types of problem that the FeenoX' precursor codes (namely wasora, Fino and milonga) already have been supporting since more than ten years now. It is also a first choice so work can be bounded and a subsequent road map and release plans can be designed. FeenoX' first version includes a subset of the required functionality, namely

 * dynamical systems
 * plant control dynamics
 * mechanical elasticity
 * heat conduction
 * structural modal analysis
 
FeenoX is designed to be developed and executed under GNU/Linux, which is the architecture og more than 95% of the internet servers which we collectively call “the cloud.” It should be noted that GNU/Linux is a POSIX-compliant version of UNIX and that FeenoX follows the rules of UNIX philosophy ([@sec:unix]) regarding its computational implementation code. Besides POSIX, FeenoX also uses MPI which is a well-known industry standard for massive parallel executions of processes, both in multi-core hosts and multi-hosts environments. Finally, if performance and/or scalability are not important issues, FeenoX can be run in a (properly cooled) local PC or laptop.

The requirement to run in the cloud and scale up as needed rules out some of the FOSS solvers available online, such as CalculiX.


## Scope {#sec:scope}

> 
> The tool should allow advanced users to define the problem to be solved programmatically, using one or more files either...
>  
>  a. specifically formatted for the tool to read such as JSON or a particular input format (historically called input decks in punched-card days), and/or 
>  b. written in an high-level interpreted language such as Python or Julia.
>   
> It should be noted that a graphical user interface is not required. The tool may include one, but it should be able to run without needing any user intervention rather than the preparation of a set of input files.
> Nevertheless, there tool might _allow_ a GUI to be used. For example, for a basic usage involving simple cases, a user interface engine should be able to create these problem-definition files in order to give access to less advanced users to the tool using a desktop, mobile and/or web-based interface in order to run the actual tool without further intervention.
> 
> However, for general usage, users should be able to completely define the problem (or set of problems, i.e. a parametric study) they want to solve in one or more input files and to obtain one or more output files containing the desired results, either a set of scalar outputs (such as maximum stresses or mean temperatures), and/or a time and/or spatial distribution. If needed, a discretization of the domain may to be taken as a know input, i.e. the tool is not required to create the mesh as long as a suitable mesher can be employed using a similar workflow specified in this\ SRS.
> 
> 
> The tool should define and document ([@sec:documentation]) the way the input files for a solving particular problem are to be prepared ([@sec:input]) and how the results are to be written ([@sec:output]).
> Any GUI, pre-processor, post-processor or other related graphical tool used to provide a graphical interface for the user should integrate in the workflow described in the preceding paragraph: a pre-processor should create  the input files needed for the tool and a post-processor should read the output files created by the tool.


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




# Architecture {#sec:architecture}

> 
> The tool must be aimed at being executed unattended on remote cloud servers which are expected to have a mainstream^[as of 2021] architecture regarding operating system (GNU/Linux variants and other UNIX-like OSes) and hardware stack (a few intel-compatible CPUs per host, a few levels of memory caches, a few gigabyes of random-access memory, several gigabytes of disk storage, etc.). It should successfully run on virtual and/or containerized servers using standard compilers, dependencies and libraries already available in the repositories of most current operating systems distributions.
> 
> Preference should be given to open source compilers, dependencies and libraries. Small problems might be executed in a single host but large problems ought to be split through several server instances depending on the processing and memory requirements. The computational implementation should adhere to open and well-established standards.
> 
> Ability to run on local desktop personal computers and/laptops is not required but suggested as a mean of giving the opportunity to users to test and debug small coarse computational models before launching the large computation on the cloud. Support for non-GNU/Linux operating systems is not required but also suggested.
> 
> Mobile platforms such as tablets and phones are not suitable to run engineering simulations due to their lack of proper electronic cooling mechanisms. They are suggested to be used to control one (or more) instances of the tool running on the cloud, and even to pre and post process results through mobile and/or web interfaces.


C, quote petsc, flat memory address space -> that's what virtual servers have!

posix

## Deployment {#sec:deployment}

> 
> The tool should be easily deployed to production servers. Both
> 
>  a. an automated method for compiling the sources from scratch aiming at obtaining optimized binaries for a particular host architecture should be provided using a well-established procedures, and
>  b. one (or more) generic binary version aiming at common server architectures should be provided.
> 
> Either option should be available to be downloaded from suitable online sources, either by real people and/or automated deployment scripts.
> 


autoconf vs. cmake, rule of diversity 

gcc, clang

blas, lapack, atlas

binaries freely available at seamplex.com

Linux, mac, windows 

## Execution {#sec:execution}

> 
> It is mandatory to be able to execute the tool remotely, either with a direct action from the user or from a high-level workflow which could be triggered by a human or by an automated script. The calling party should be able to monitor the status during run time and get the returned error level after finishing the execution.
> 
> The tool shall provide a mean to perform parametric computations by varying one or more problem parameters in a certain prescribed way such that it can be used as an inner solver for an outer-loop optimization tool. In this regard, it is desirable if the tool could compute scalar values such that the figure of merit being optimized (maximum temperature, total weight, total heat flux, minimum natural frequency, maximum displacement, maximum von\ Mises stress, etc.) is already available without needing further post-processing.


command line args for paramteric runs from a shell script

script friendly

## Efficiency {#sec:efficiency}

> 
> The computational resources (i.e. costs measured in CPU/GPU time, random-access memory, long-term storage, etc.) needed to solve a problem should be comparable to other similar state-of-the-art finite-element tools.


cloud, rent don't buy
benchmark and comparisons


## Scalability  {#sec:scalability}

> 
> The tool ought to be able to start solving small problems first to check the inputs and outputs behave as expected and then allow increasing the problem size up in order to achieve to the desired accuracy of the results. As mentioned in [@sec:architecture], large problem should be split among different computers to be able to solve them using a finite amount of per-host computational power (RAM and CPU).


PETSc, MPI
error handling, rule of repair
check all malloc() calls


## Flexibility

> 
> The tool should be able to handle engineering problems involving different materials with potential spatial and time-dependent properties, such as temperature-dependent thermal expansion coefficients and/or non-constant densities.
> Boundary conditions must be allowed to depend on both space and time as well, like non-uniform pressure loads and/or transient heat fluxes.


FeenoX comes from nuclear + experience (what to do and what not to do)


## Extensibility {#sec:extensibility}

> 
> It should be possible to add other PDE-casted problem types (such as the Schröedinger equation) to the tool using a reasonable amount of time by one or more skilled programmers. The tool should also allow new models (such as non-linear stress-strain constitutive relationships) to be added as well.


user-provided routines
skel for pdes and annotated models


## Interoperability {#sec:interoperability}

> 
> A mean of exchanging data with other computational tools complying to requirements similar to the ones outlined in this document.


UNIX
POSIX


# Interfaces

> 
> 
> The tool should be able to allow remote execution without any user intervention after the tool is launched. The problem should be completely defined in one or more input files and the output should be complete and useful after the tool finishes its execution as in [@fig:transfer].  
> 
> The tool should be able to report the status of the execution (i.e. progress, errors, etc.) and to make this information available to the user or process that launched the execution, possibly from a remote location.


## Problem input {#sec:input}

> 
> No GUI. Plain ASCII input file and/or interpreted high-level language API.
> Mobile & web-friendly.
> 
> **Simple problems should need simple inputs.**
> 
> **Similar problems should need similar inputs.**
> 
> VCS tracking
> 
> These input files might be
> 
>  * particularly formatted files to be read by the tool in an _ad-hoc_ way, and/or
>  * source files for interpreted languages which can call the tool through and API or equivalent method, and/or
>  * any other method that can fulfill the requirement illustrated in\ [@fig:transfer]


dar ejemplos
comparar con <https://cofea.readthedocs.io/en/latest/benchmarks/004-eliptic-membrane/tested-codes.html>

macro-friendly inputs, rule of generation

## Results output {#sec:output}

> 
> Output should not be cluttered up with non-mandatory information. Time of cognizant engineers should be more important than time needed for computation. 
> JSON/YAML, state of the art open post-processing formats.
> Mobile & web-friendly.
> 
> Common and preferably open-source formats.


100% user-defined output with PRINT, rule of silence
rule of economy, i.e. no RELAP
yaml/json friendly outputs
vtk (vtu), gmsh


# Quality assurance {#sec:qa}

> 
> Since the results obtained with the tools might be used in verifying existing equipment or in designing new mechanical parts in sensitive industries, a certain level of software quality assurance is needed. Some best-practices for developing generic software as required, such as employment of a version control system, automated unit testing and bug tracking support. But also more particular verification and validation procedures for the particular case of engineering computational software is 




## Reproducibility and traceability 

> 
> The full source code and the documentation of the tool ought to be maintained under a control version system hosted on a public server accessible worldwide without needing any special credentials to get a copy of the code. 
> 
> All the information needed to solve a particular problem (i.e. meshes, boundary conditions, spatially-distributed material properties, etc.) should be  generated from a very simple set of files which ought to be susceptible of being tracked with current state-of-the-art version control systems.
> 
> simple <-> simple
> 
> similar <-> similar
> 
> Mesh data should be mixed with the problem data like material properties or boundary conditions.
> Changes in the mesh should be tracked on the files needed to create the mesh and not on the mesh itself.




## Automated testing

> 
> A mean to automatically test the code for regressions is mandatory. A set of problems with known solutions should be solved with the tool after each modification of the code to make sure these changes still give the right answers for the right questions. Unit software testing practices like continuous integration are recommended.



## Bug reporting and tracking

> 
> A system to allow developers and users to report errors, bugs and improvements should be provided. If applicable, these reports should be tracked,  addressed and documented.
> 



## Verification {#sec:verification}

> 
> The source code should be available for verification by independent third parties.
> Changes in the source code should be controllable, traceable and well documented.
> Stable releases ought to be digitally signed by a responsible engineer.



open source (really, not like CCX -> mostrar ejemplo)
GPLv3+ free
Git + gitlab, github, bitbucket


## Validation

> 
> The tool should be verified against known analytical results and other already-validated tools according to existing industry standards such as ASME or IAEA.



## Documentation {#sec:documentation}

> 
> Documentation should be complete and cover both the user and the developer point of view. It should contain a user manual adequate for both reference and tutorial purposes. Other forms of simplified documentation such as quick reference cards or video tutorials are not mandatory but highly recommended. Since the tool should be extendable ([@sec:extensibility]), there should be a separate development manual covering the programming design and implementation and how to extend and add new features.
> Also, as non-trivial mathematics which should be verified ([@sec:verification]) are expected, an explanation what equations are taken into account and how they are solved is required.
> 
> It should be possible to make the full documentation available online in a way that it can be both printed in hard copy and accessed easily from a mobile device. Users modifying the tool to suit their own needs should be able to modify the associated documentation as well.
> 


it's not compact, but almost!
Compactness is the property that a design can fit inside a human being's head. A good practical test for compactness is this: Does an experienced user normally need a manual? If not, then the design (or at least the subset of it that covers normal use) is compact.
unix man page
markdown + pandoc = html, pdf, texinfo
 

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
