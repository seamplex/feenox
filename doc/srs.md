---
title: Software Design Requirements for a FEA tool
lang: en-US
...


1. Introduction

1.1 Purpose

1.2 Intended Audience

1.3 Intended Use

1.4 Scope

1.5 Definitions and Acronyms

2. Overall Description

2.1 User Needs

2.2 Assumptions and Dependencies

3. System Features and Requirements

            3.1 Functional Requirements

            3.2 External Interface Requirements

            3.3 System Features

            3.4 Nonfunctional Requirements


# Introduction

## Objective

The main objective of the program is to be able to solve thermal-mechanical problems using the finite element method, but other problems and features should be also considered. For instance, being able to perform modal analysis and/or to compute linearized stresses according to ASME. Also, introducing general mathematical methods to add flexibility allowing to solve other problems (such as fatigue analysis) is highly encouraged.

## Intended use

DAEs
PDEs
Linear and non-linear elasticity.
Topological optimization.


## Scope


# Architecture

The program should be aimed at being executed unattended on remote cloud servers with a mainstream (as of 2021) architecture regarding operating system and hardware stack. It should successfully run on virtual and/or containerized servers using open compilers, dependencies and libraries already available in the repositories of mainstream operating systems distributions. Small problems might be executed in a single host but large problems ought to be split through several server instances depending on the processing and memory requirements. 
The implementation should adhere to open and well-established standards.

## Deployment

The program should be easily deployed to production servers. Both

 a. an automated method for compiling the sources from scratch aiming at obtaining optimized binaries for a particular host architecture should be provided using a well-established procedures, and
 b. one (or more) generic binary version aiming at common server architectures should be provided.


## Execution 




# Interfaces


The program should be able to allow remote execution without any user intervention after the  
The program should be able to report the status of the execution (i.e. progress, errors, etc.) and to make this information available to the user or process that launched the execution, possibly from a remote location.

## Problem input

No GUI. Plain ASCII input file and/or interpreted high-level language API.
Mobile & web-friendly.

**Simple problems should need simple inputs.**

## Results output

JSON/YAML, state of the art open post-processing formats.
Mobile & web-friendly.




# Quality assurance

## Reproducibility and traceability 

All the information needed to solve a particular problem (i.e. meshes, boundary conditions, spatially-distributed material properties, etc.) should be  generated from a very simple set of files which ought to be susceptible of being tracked with current state-of-the-art version control systems.


## Verification

The program source code should be available for verification according to ASME.
Changes in the source code should be controlled, traced and documented.
Stable releases ought to be digitally signed by the a responsible engineer.

## Validation

The program should be verified against known analytical results and other already-validated programs according to ASME.
There should be a comprehensive set of automated tests able to detect possible regressions introduced in the program.


## Documentation

Program documentation should be complete and made available online in a way that it can be either printed in hardcopy and accessed easily from a mobile device.

