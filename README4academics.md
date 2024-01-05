---
title: FeenoX for Academics
titleblock: |
 FeenoX for Academics
 ====================
lang: en-US
number-sections: true
toc: true
...


## What

A program, i.e. something your run, reads the problem in a plain text file containing definitions and instructions, solves it, writes the output (if asked for with instructions in the input file) and returns back to the shell.
It is deliberately not a library, i.e. something you have to link your code against. Link to SRS.

see engineers for examples, tutorials

PhD

## How

By leveraging the Unix programming philosophy to come up with a cloud-first tool suitable to serve as the back end of web-based interfaces such as CAEplex.
A mathematical framework with C entry points that tell it what to put in the global stiffness matrix, residuals, etc.




FeenoX meets fictitious-yet-plausible [Software Requirement Specifications](https://www.seamplex.com/feenox/doc/srs.html).
The FeenoX [Software Design Specifications](https://www.seamplex.com/feenox/doc/sds.html) address each requirement of the SRS.
FeenoX provides a general mathematical framework to solve PDEs with a bunch of entry points (as C functions) where new types of PDEs (e.g. electromagnetism, fluid mechanics, etc.) can be added to the set of what FeenoX can solve. FeenoX will provide means to

  - parse the input file, handle command-line arguments, read mesh files, assign variables, evaluate conditionals, write results, etc.

    ```feenox
    PROBLEM laplace 2D
    READ_MESH square-$1.msh
    [...]
    WRITE_RESULTS FORMAT vtk
    ```
    
  - handle material properties given as algebraic expressions involving pointwise-defined functions of space, temperature, time, etc.
    
    ```feenox
    MATERIAL steel     E=210e3*(1-1e-3*(T(x,y,z)-20))   nu=0.3
    MATERIAL aluminum  E=69e3                           nu=7/25
    ```
    
  - read problem-specific boundary conditions as algebraic expressions
    
    ```feenox
    sigma = 5.670374419e-8  # W m^2 / K^4 as in wikipedia
    e = 0.98      # non-dimensional
    T0 = 1000     # K
    Tinf = 300    # K

    BC left  T=T0
    BC right q=sigma*e*(Tinf^4-T(x,y,z)^4)
    ```

  - access shape functions and its derivatives evaluated eitehr at Gauss points or at arbitrary locations for computing elementary contributions to
     * stiffness matrix
     * mass matrix
     * right-hand side vector
    
  - solve the discretized equations using the appropriate [PETSc](https://petsc.org/)/[SLEPc](https://slepc.upv.es/) objects, i.e.
    * [KSP](https://petsc.org/release/manual/ksp/) for linear static problems
    * [SNES](https://petsc.org/release/manual/snes/) for non-linear static problems
    * [TS](https://petsc.org/release/manual/ts/) for transient problems
    * [EPS](https://slepc.upv.es/documentation/current/docs/manualpages/EPS/index.html) for eigenvalue problems

This general framework constitutes the bulk of [FeenoX’s source code](https://github.com/seamplex/feenox).
The particular functions that implement each problem type are located in subdirectories [`src/pdes`](https://github.com/seamplex/feenox/tree/main/src/pdes), namely

```include
doc/pdes.md
```
      
Researchers with both knowledge of mathematical theory of finite elements and programming skills might add support for other PDES by using one of these directories (say [`laplace`](https://github.com/seamplex/feenox/tree/main/src/pdes/laplace)) as a template and

 #. replace every occurrence of `laplace` in symbol names with the name of the new PDE
 #. modify the initialization functions in `init.c` and set 
     * the names of the unknowns
     * the names of the materials
     * the mathematical type and properties of problem 
     * etc.
 #. modify the contents of the elemental matrices in `bulk.c` in the FEM formulation of the problem being added
 #. modify the contents of how the boundary conditions are parsed and set in `bc.c`
 #. re-run `autogen.sh`, `./configure` and `make` to get a FeenoX executable with support for the new PDE.

See the [programming guide](doc/programming.md) for details.




## Why

The world is already full of finite-element programs and every day a grad student creates a new one from scratch.
So why adding FeenoX to the already-crowded space of FEA tools?


Because there are either

 a. libraries which need code to use them, 
 b. end-user programs which need a GUI

FeenoX sits in the middle:

 * to solve a problem one needs to prepare an input file (not a script) which is read at run-time, not code which calls a library
 * these input files can expand generic command-line options using Bash syntax as `$1`, `$2`, etc, which allows for parametric or optimization loops driven by higher-level scripts
 * for solving PDEs, the input file has to refer to at least one Gmsh `.msh` file that defines the domain where the PDE is solved
 * the material properties and boundary condition are defined using physical groups and not individual nodes nor elements, so the input file is independent of the mesh and thus can be tracked with Git to increase traceability and repeatability
 * it uses the Unix philosophy which, among others, separates policy from mechanism and thus FeenoX is a natural choice for web-based interfaces like CAEplex
 

See hackers
 
 
