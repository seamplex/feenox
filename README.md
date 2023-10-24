---
title: FeenoX
subtitle: A cloud-first free no-fee no-X uniX-like finite-element(ish) computational engineering tool
titleblock: |
 FeenoX: A cloud-first free no-fee no-X uniX-like finite-element(ish) computational engineering tool
 =======================================================================================
lang: en-US
number-sections: true
toc: true
...

# About FeenoX

## Why FeenoX?

> If by "Why FeenoX?" you mean "Why is FeenoX named that way?," read the [FAQs](doc/FAQ.md).


The world is already full of finite-element programs and every day a grad student creates a new one from scratch.
So why adding FeenoX to the already-crowded space of FEA tools?
Here is ***why FeenoX is different**.

```{.include}
doc/why.md
```

## How is FeenoX different?

FeenoX meets fictitious-yet-plausible [Software Requirement Specifications](https://www.seamplex.com/feenox/doc/srs.html).
The FeenoX [Software Design Specifications](https://www.seamplex.com/feenox/doc/sds.html) address each requirement of the SRS.
Two of the most important design-basis features are that FeenoX is...

 #. a cloud-native computational tool (not just cloud _friendly,_ but cloud **first**).
 #. both free (as in freedom) and open source. See [Licensing].
 
But the most important idea is that FeenoX provides a general mathematical framework to solve PDEs with a bunch of entry points (as C functions) where new types of PDEs (e.g. electromagnetism, fluid mechanics, etc.) can be added to the set of what FeenoX can solve. FeenoX will provide means to

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
      
Engineers, researchers, scientists, developers and/or hobbyists just need to use one of these directories (say [`laplace`](https://github.com/seamplex/feenox/tree/main/src/pdes/laplace)) as a template and

 #. replace every occurrence of `laplace` in symbol names with the name of the new PDE
 #. modify the initialization functions in `init.c` and set 
     * the names of the unknowns
     * the names of the materials
     * the mathematical type and properties of problem 
     * etc.
 #. modify the contents of the elemental matrices in `bulk.c` in the FEM formulation of the problem being added
 #. modify the contents of how the boundary conditions are parsed and set in `bc.c`
 #. re-run `autogen.sh`, `./configure` and `make` to get a FeenoX executable with support for the new PDE.

They also ought to to read, understand and mind the [GPLv3+ licensing terms](#sec:licensing).
See below for contribution guidelines.


## What is FeenoX anyway?

```{.include}
doc/introduction.md
```

# Quickstart

## Download

```include
doc/downloads.md
```

## Git repository

```include
doc/git.md
```

See the [download page](https://seamplex.com/feenox/download.html) for more detailed information.


# Examples

See the [examples page](https://seamplex.com/feenox/examples) for examples.
The FeenoX examples are divided by the type of problem they solve:

 #. [Basic mathematics](https://seamplex.com/feenox/examples/basic.html)
 #. [Systems of ODEs/DAEs](https://seamplex.com/feenox/examples/daes.html)
 #. [Laplace’s equation](https://seamplex.com/feenox/examples/laplace.html)
 #. [Heat conduction](https://seamplex.com/feenox/examples/thermal.html)
 #. [Linear elasticity](https://seamplex.com/feenox/examples/mechanical.html)
 #. [Modal analysis](https://seamplex.com/feenox/examples/modal.html)
 #. [Neutron diffusion](https://seamplex.com/feenox/examples/neutron_diffusion.html)
 #. [Neutron $S_N$](https://seamplex.com/feenox/examples/neutron_sn.html)

> Each type of partial differential equation (i.e. from Laplace downward) is implemented in a subdirectory within [`src/pde`](https://github.com/seamplex/feenox/tree/main/src/pdes) of the source tree.
> Feel free to...
>
>  * ask questions in the [Github discussion page](https://github.com/seamplex/feenox/discussions)
>  * propose features, corrections, improvements, etc.
>  * create a pull request for other PDEs. Candidates would be
>    - fluid mechanics
>    - thermal hydraulics
>    - electromagnetism
>    - non-linear elasticity
>    - cell-level neutronics (i.e. method of characteristics, collision probabilities, etc)
 
All the files needed to run theses examples are available in the [examples](https://github.com/seamplex/feenox/tree/main/examples) directory of the [Git repository](https://github.com/seamplex/feenox) and any of the [tarballs](https://www.seamplex.com/feenox/download.html), both [source](https://www.seamplex.com/feenox/dist/src) and [binary](https://www.seamplex.com/feenox/dist/linux).


## Test suite
 
The `tests` [directory in the Github repository](https://github.com/seamplex/feenox/tree/main/tests) has dozens of test cases which can be used as examples for reference and for mathematical verification of the results obtained with FeenoX.
These are run when doing `make check` after [compiling the source code](doc/compile.md). 

The test cases usually return a single number (which should be near zero) comparing the numerical result with the expected one.
Feel free to propose benchmark problems to add to the suite.



# Licensing {#sec:licensing}

FeenoX is distributed under the terms of the [GNU General Public License](http://www.gnu.org/copyleft/gpl.html) version 3 or (at your option) any later version. The following text was borrowed from the [Gmsh documentation](http://gmsh.info/doc/texinfo/gmsh.html#Copying-conditions). Replacing “Gmsh” with “FeenoX” gives:

> ```include
> doc/freesw.md
> ```

```include
doc/licensing.md
```

## Contributing

Contributions under the terms of the GPLv3+ are welcome, especially new types of PDEs and new formulations of existing PDEs.
For elliptic operators feel free to use the Laplace equation at `src/pdes/laplace` as a template.

 1. Read the [compilation](./doc/compilation.md) and [programming](./doc/programming.md) guides.
 2. Browse [Github discussions](https://github.com/seamplex/feenox/discussions) and open a new thread explaining what you want to do and/or asking for help.
 3. Fork the [Github repository](https://github.com/seamplex/feenox/)
 4. Create a pull request

Note that

 * It is mandatory to observe the [Code of Conduct](CODE_OF_CONDUCT.md).
 * Each author keeps the copyright of the contributed code.


# Further information

Home page: <https://www.seamplex.com/feenox>  
Repository: <https://github.com/seamplex/feenox>  
Bug reporting: <https://github.com/seamplex/feenox/issues>  
Discussions: <https://github.com/seamplex/feenox/discussions>  
Follow us: [YouTube](https://www.youtube.com/channel/UCC6SzVLxO8h6j5rLlfCQPhA)
           [LinkedIn](https://www.linkedin.com/company/seamplex/)
           [Github](https://github.com/seamplex)

---------------------------

FeenoX is copyright ©2009-2023 [Seamplex](https://www.seamplex.com)  
FeenoX is licensed under [GNU GPL version 3](http://www.gnu.org/copyleft/gpl.html) or (at your option) any later version.  
FeenoX is free software: you are free to change and redistribute it.  
There is **NO WARRANTY**, to the extent permitted by law.  
