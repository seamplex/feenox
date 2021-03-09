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


FeenoX arranca con un subset de eso más neutrónica.
En algún lugar hay que poner un límite para arrancar. Release plans.

FeenoX está diseñado desde cero para correr en GNU/Linux, que constituye la arquitectura ampliamente mayoritaria de "la nube."
Usa MPI que es un estándar para cosas en paralelo sore muchos hosts.
También se puede ejecutar en una PC o laptop, aunque la performance y escalabilidad están limitadas por el tipo y número de CPUs, la memoria RAM disponible y los métodos de refrigeración.


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



Transfer function

![The tool working as a transfer function between input and output files](transfer.svg){#fig:transfer width=50%}


UNIX rules as appendix

rule of separation

separate mesher - unix 

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
 
