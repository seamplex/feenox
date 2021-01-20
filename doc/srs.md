---
title: Software Design Requirements for a FEA tool
lang: en-US
abstract: An hypothetical situation (a thought experiment if you will) where a certain company or agency asks for vendors to develop a piece of engineering software with certain specifications, defined in this imaginary tender.
...


# Introduction

A computational tool (herein after referred to as _the tool_) is required in order to solve engineering problems in the cloud.
This Software Requirements Document describes the mandatory features the tool ought to have and lists some features which would be nice the tool had.
Also the requirements about architecture, execution and interfaces in order to fulfill the needs of state-of-the-art cloud-based users as of 2021 (and the years to come) are defined. 


## Objective

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

The initial version of the tool must be able to handle a subset of the above list of problem types.
Eventually, the set of supported problem types should grow to include other models as well, as described in\ [@sec:extensibility].


## Intended use {#sec:usage}

The tool should allow advanced user to define the problem to be solved programa¨tically, either using one or more files
 
 a. specifically formatted for the tool to read such as JSON or a particular input format, and/or 
 b. written in an high-level interpreted language such as Python or Julia.
  
For a reasonable basic usage involving simple cases, a user interface engine should be able to create these problem-definition files in order to give access to less advanced users to the tool.
 

## Scope

The tool should work as a transfer function from the set of input files described in\ [@sec:usage] to a set of output files containing the desired results, either a set of scalar outputs (such as maximum stresses or mean temperatures), and/or a time and/or spatial distribution. If needed, a discretization of the domain may to be taken as a know input, i.e. the tool is not required to create the mesh as long as a suitable mesher can be in the workflow.

# Architecture

The tool must be aimed at being executed unattended on remote cloud servers which are expected to have a mainstream^[as of 2021] architecture regarding operating system (UNIX-like GNU/Linux variants) and hardware stack (number and type of CPUs, number and type of memory caches, disk and RAM storae size, etc.). It should successfully run on virtual and/or containerized servers using standard compilers, dependencies and libraries already available in the repositories of current operating systems distributions. Preference should be given to open source compiler, dependencies and libraries. Small problems might be executed in a single host but large problems ought to be split through several server instances depending on the processing and memory requirements. 
The computational implementation should adhere to open and well-established standards.

> C, quote petsc, flat memory address space -> that's what virtual servers have!

## Deployment

The tool should be easily deployed to production servers. Both

 a. an automated method for compiling the sources from scratch aiming at obtaining optimized binaries for a particular host architecture should be provided using a well-established procedures, and
 b. one (or more) generic binary version aiming at common server architectures should be provided.

> autoconf vs. cmake, rule of diversity 
 

## Execution 

The execution of the tool in order to solve the engineering problem should be possible to be done remotely, either with a direct action from the user or from a higher-order workflow which could be triggered by a human or by an automated script. 
The tool shall provide a mean to perform parametric computations by varying one or more problem parameters in a certain prescribed way.

> command line args for paramteric runs from a shell script

## Efficiency or Performance

The time and resources needed to solve a problem should be comparable to other similar finite-element tools.


## Scalability 

> error handling, rule of repair

## Flexibility


## Extensibility {#sec:extensibility}


## Interoperability


# Interfaces


The tool should be able to allow remote execution without any user intervention after the tool is launched. The problem should be completely defined in one or more input files and the output should be complete and useful after the tool finishes its execution.

The tool should be able to report the status of the execution (i.e. progress, errors, etc.) and to make this information available to the user or process that launched the execution, possibly from a remote location.

## Problem input

No GUI. Plain ASCII input file and/or interpreted high-level language API.
Mobile & web-friendly.

**Simple problems should need simple inputs.**

> macro-friendly inputs, rule of generation

## Results output

JSON/YAML, state of the art open post-processing formats.
Mobile & web-friendly.

> user-defined output with PRINT, rule of silence
> yaml/json friendly outputs


# Quality assurance

## Reproducibility and traceability 

All the information needed to solve a particular problem (i.e. meshes, boundary conditions, spatially-distributed material properties, etc.) should be  generated from a very simple set of files which ought to be susceptible of being tracked with current state-of-the-art version control systems.


## Verification

The source code should be available for verification by independent third parties.
Changes in the source code should be controlled, traced and documented.
Stable releases ought to be digitally signed by a responsible engineer.

## Validation

The tool should be verified against known analytical results and other already-validated tools according to ASME.
There should be a comprehensive set of automated tests able to detect possible regressions introduced in the tool.


## Documentation

Program documentation should be complete and made available online in a way that it can be both printed in hard copy and accessed easily from a mobile device.

