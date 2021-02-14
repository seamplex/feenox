---
title: Software Requirements Specification for a computational tool
lang: en-US
abstract: An hypothetical situation (a thought experiment if you will) where a certain company or agency asks for vendors to develop a piece of free and open source engineering software with certain specifications, defined in this imaginary tender.
number-sections: true
---


# Introduction {#sec:introduction}

A computational tool (herein after referred to as _the tool_) is required in order to solve engineering problems in the cloud.
This (imaginary but plausible) Software Requirements Specification document describes the mandatory features the tool ought to have and lists some features which would be nice the tool had.
Also the requirements and guidelines about architecture, execution and interfaces in order to fulfill the needs of cognizant engineers as of 2021 (and the years to come) are defined. 

On the one hand, the tool should be applicable to solving industrial problems under stringent efficiency ([@sec:efficiency]) and quality ([@sec:qa]) requirements. It is therefore mandatory to be able to assess the source code for potential performance and verification revision by qualified third parties. On the other hand, the initial version of the tool is expected to be a basic framework which might be extended ([@sec:objective] and [@sec:extensibility]) by academic researchers and programmers. It thus should also be free.^[Free as in “free speech,” not as in “free beer.”]
The detailed licensing terms are left to the offer but it should allow users to solve their problems the way they need and, eventually, to modify and improve the tool to suit their needs. If they cannot program themselves, they should have the freedom to hire somebody to do it for them.


## Objective {#sec:objective}

The main objective of the tool is to be able to solve engineering problems which are usually casted as differential-algebraic equations (DAEs) or partial differential equations (PDEs), such as

 * dynamical systems
 * plant control dynamics
 * mechanical elasticity
 * heat conduction
 * structural modal analysis
 * electromagnetism
 * chemical diffusion
 * computational fluid dynamics
 * ...

\noindent
on one or more mainstream computers.
 
The initial version of the tool must be able to handle a subset of the above list of problem types.
Afterward, the set of supported problem types, models, equations and features of the tool should grow to include other models as well, as required in\ [@sec:extensibility].



## Scope {#sec:scope}

The tool should allow advanced users to define the problem to be solved programmatically, using one or more files either...
 
 a. specifically formatted for the tool to read such as JSON or a particular input format (historically called input decks in punched-card days), and/or 
 b. written in an high-level interpreted language such as Python or Julia.
  
For a basic usage involving simple cases, a user interface engine should be able to create these problem-definition files in order to give access to less advanced users to the tool using a desktop, mobile and/or web GUI.

The tool should work as a transfer function from the set of input files described above to a set of output files containing the desired results, either a set of scalar outputs (such as maximum stresses or mean temperatures), and/or a time and/or spatial distribution ([@fig:transfer]). If needed, a discretization of the domain may to be taken as a know input, i.e. the tool is not required to create the mesh as long as a suitable mesher can be employed using a similar workflow specified in this\ SRS.

![The tool working as a transfer function between input and output files](transfer.svg){#fig:transfer width=50%}

The tool should define and document ([@sec:documentation]) the way the input files for a solving particular problem are to be prepared ([@sec:input]) and how the results are to be written ([@sec:output]).
Any GUI, pre-processor, post-processor or other related graphical tool used to provide a graphical interface for the user should integrate in the generic workflow depicted in\ [@fig:transfer].

> rule of separation
> separate mesher - unix 

# Architecture {#sec:architecture}

The tool must be aimed at being executed unattended on remote cloud servers which are expected to have a mainstream^[as of 2021] architecture regarding operating system (UNIX-like GNU/Linux variants) and hardware stack (number and type of CPUs, number and type of memory caches, disk and RAM storage size, etc.). It should successfully run on virtual and/or containerized servers using standard compilers, dependencies and libraries already available in the repositories of current operating systems distributions. Preference should be given to open source compilers, dependencies and libraries. Small problems might be executed in a single host but large problems ought to be split through several server instances depending on the processing and memory requirements. The computational implementation should adhere to open and well-established standards.

> C, quote petsc, flat memory address space -> that's what virtual servers have!
> posix


## Deployment {#sec:deployment}

The tool should be easily deployed to production servers. Both

 a. an automated method for compiling the sources from scratch aiming at obtaining optimized binaries for a particular host architecture should be provided using a well-established procedures, and
 b. one (or more) generic binary version aiming at common server architectures should be provided.

Either option should be available to be downloaded from suitable online sources.
 
> autoconf vs. cmake, rule of diversity 
> gcc, clang
> blas, lapack, atlas
> binaries freely available at seamplex.com

> Linux, mac, windows 

## Execution {#sec:execution}

It is mandatory to be able to execute the tool remotely, either with a direct action from the user or from a high-level workflow which could be triggered by a human or by an automated script. 
The tool shall provide a mean to perform parametric computations by varying one or more problem parameters in a certain prescribed way (for instance to perform iterations defined by another outer-loop optimization tool).

> command line args for paramteric runs from a shell script
> script friendly

## Efficiency {#sec:efficiency}

The computational resources (i.e. costs measured in CPU/GPU time, random-access memory, long-term storage, etc.) needed to solve a problem should be comparable to other similar state-of-the-art finite-element tools.

> cloud, rent don't buy
> benchmark and comparisons


## Scalability  {#sec:scalability}

The tool ought to be able to start solving small problems first to check the inputs and outputs behave as expected and then allow increasing the problem size up in order to achieve to the desired accuracy of the results. As mentioned in [@sec:architecture], large problem should be split among different computers to be able to solve them using a finite amount of per-host computational power (RAM and CPU).

> PETSc, MPI
> error handling, rule of repair
> check all malloc() calls

## Flexibility

The tool should be able to handle engineering problems involving different materials with potential spatial and time-dependent properties, such as temperature-dependent thermal expansion coefficients and/or non-constant densities.
Boundary conditions must be allowed to depend on both space and time as well, like non-uniform pressure loads and/or transient heat fluxes.

> FeenoX comes from nuclear + experience (what to do and what not to do)

## Extensibility {#sec:extensibility}

It should be possible to add other PDE-casted problem types (such as the Schröedinger equation) to the tool using a reasonable amount of time by one or more skilled programmers. The tool should also allow new models (such as non-linear stress-strain constitutive relationships) to be added as well.

> skel for pdes and annotated models
> user-provided routines


## Interoperability {#sec:interoperability}

A mean of exchanging data with other computational tools complying to requirements similar to the ones outlined in this document.

> UNIX
> POSIX

# Interfaces


The tool should be able to allow remote execution without any user intervention after the tool is launched. The problem should be completely defined in one or more input files and the output should be complete and useful after the tool finishes its execution as in [@fig:transfer].  

The tool should be able to report the status of the execution (i.e. progress, errors, etc.) and to make this information available to the user or process that launched the execution, possibly from a remote location.

## Problem input {#sec:input}

No GUI. Plain ASCII input file and/or interpreted high-level language API.
Mobile & web-friendly.

**Simple problems should need simple inputs.**

> dar ejemplo

**Similar problems should need similar inputs.**

> dar ejemplo

VCS tracking

These input files might be

 * particularly formatted files to be read by the tool in an _ad-hoc_ way, and/or
 * source files for interpreted languages which can call the tool through and API or equivalent method, and/or
 * any other method that can fulfill the requirement illustrated in\ [@fig:transfer]

> macro-friendly inputs, rule of generation


## Results output {#sec:output}

Output should not be cluttered up with non-mandatory information. Time of cognizant engineers should be more important than time needed for computation. 
JSON/YAML, state of the art open post-processing formats.
Mobile & web-friendly.

Common and preferably open-source formats.

> 
> 100% user-defined output with PRINT, rule of silence
> rule of economy, i.e. no RELAP
> yaml/json friendly outputs
> vtk (vtu), gmsh


# Quality assurance {#sec:qa}

Since the results obtained with the tools might be used in verifying existing equipment or in designing new mechanical parts in sensitive industries, a certain level of software quality assurance is needed. Some best-practices for developing generic software as required, such as employment of a version control system, automated unit testing and bug tracking support. But also more particular verification and validation procedures for the particular case of engineering computational software is 

## Reproducibility and traceability 

The full source code and the documentation of the tool ought to be maintained under a control version system hosted on a public server accessible worldwide without needing any special credentials to get a copy of the code. 

All the information needed to solve a particular problem (i.e. meshes, boundary conditions, spatially-distributed material properties, etc.) should be  generated from a very simple set of files which ought to be susceptible of being tracked with current state-of-the-art version control systems.

simple <-> simple

similar <-> similar

Mesh data should be mixed with the problem data like material properties or boundary conditions.
Changes in the mesh should be tracked on the files needed to create the mesh and not on the mesh itself.

## Automated testing

A mean to automatically test the code for regressions is mandatory. A set of problems with known solutions should be solved with the tool after each modification of the code to make sure these changes still give the right answers for the right questions. Unit software testing practices like continuous integration are recommended.

## Bug reporting and tracking

A system to allow developers and users to report errors, bugs and improvements should be provided. If applicable, these reports should be tracked,  addressed and documented. 

## Verification {#sec:verification}

The source code should be available for verification by independent third parties.
Changes in the source code should be controllable, traceable and well documented.
Stable releases ought to be digitally signed by a responsible engineer.

> open source (really, not like CCX -> mostrar ejemplo)
> GPLv3+ free
> Git + gitlab, github, bitbucket

## Validation

The tool should be verified against known analytical results and other already-validated tools according to existing industry standards such as ASME or IAEA.




## Documentation {#sec:documentation}

Documentation should be complete and cover both the user and the developer point of view. It should contain a user manual adequate for both reference and tutorial purposes. Other forms of simplified documentation such as quick reference cards or video tutorials are not mandatory but highly recommended. Since the tool should be extendable ([@sec:extensibility]), there should be a separate development manual covering the programming design and implementation and how to extend and add new features.
Also, as non-trivial mathematics which should be verified ([@sec:verification]) are expected, an explanation what equations are taken into account and how they are solved is required.

It should be possible to make the full documentation available online in a way that it can be both printed in hard copy and accessed easily from a mobile device. Users modifying the tool to suit their own needs should be able to modify the associated documentation as well.

> it's not compact, but almost!
> Compactness is the property that a design can fit inside a human being's head. A good practical test for compactness is this: Does an experienced user normally need a manual? If not, then the design (or at least the subset of it that covers normal use) is compact.
> unix man page
> markdown + pandoc = html, pdf, texinfo
> 
