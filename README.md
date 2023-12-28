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

FeenoX is a cloud-first free and open source tool to solve engineering-related problems using a computer (or many computers in parallel) with a particular design basis.

::::: {.only-in-format .html }
```{=html}
<a class="btn btn-lg btn-outline-primary"   href="https://www.seamplex.com/feenox/README-engineers.html" role="button">FeenoX for Industry Engineers</a>
<a class="btn btn-lg btn-outline-secondary" href="https://www.seamplex.com/feenox/README-hackers.html"   role="button">FeenoX for Unix Hackers</a>
<a class="btn btn-lg btn-outline-info"      href="https://www.seamplex.com/feenox/README-academics.html" role="button">FeenoX for Academic Professors</a>
```
:::::

Choose your background for further details about the what, how and whys:

 - [Industry Engineer](README-engineers.md)
 - [Unix Hacker](README-hackers.md)
 - [Academic Professor](README-academics.md)


## Extents

### Examples

 - [Basic mathematics](https://seamplex.com/feenox/examples/basic.html)
 - [Systems of ODEs/DAEs](https://seamplex.com/feenox/examples/daes.html)
 - [Laplace’s equation](https://seamplex.com/feenox/examples/laplace.html)
 - [Heat conduction](https://seamplex.com/feenox/examples/thermal.html)
 - [Linear elasticity](https://seamplex.com/feenox/examples/mechanical.html)
 - [Modal analysis](https://seamplex.com/feenox/examples/modal.html)
 - [Neutron diffusion](https://seamplex.com/feenox/examples/neutron_diffusion.html)
 - [Neutron S<sub>N</sub>](https://seamplex.com/feenox/examples/neutron_sn.html)

### Tutorials


 0. [Setting up your workspace](https://www.seamplex.com/feenox/doc/tutorials/000-setup)
 1. [Overview: the tensile test case](https://www.seamplex.com/feenox/doc/tutorials/110-tensile-test)
 2. [Fun & games: solving mazes with PDES instead of AI](https://www.seamplex.com/feenox/doc/tutorials/120-mazes)
 3. [Heat conduction](https://www.seamplex.com/feenox/doc/tutorials/320-thermal)

::::: {.only-in-format .html }

::: {.container .text-center .my-5 .ratio .ratio-16x9}
<iframe class="embed-responsive-item" src="https://www.youtube.com/embed/b3K2QewI8jE?rel=0" allowfullscreen></iframe>
:::

:::::
    
    
### Tests

The [`tests`](https://github.com/seamplex/feenox/tree/main/tests) directory in the repository has hundreds of
 
 - examples,
 - unit tests,
 - regression tests, and/or
 - (crude) mathematical verification tests.
 
## Capabilities

 * The problem to solve is defined through a self-descriptive English-like plain-text input file
   - no need to recompile if the problem changes (FeenoX is a _program_, not a library)
   - simple problems need simple inputs
   - similar problems need similar inputs
   - everything is an expresion
   - 100%-defined user output (no print or write instructions, no output) 
 * General mathematical problems using GNU GSL
 * Sets of ODEs/DAEs using SUNDIALS
 * PDEs using FEM 
   - reads mesh in Gmsh format
   - uses PETSc/SLEPc to solve
     - linear systems (KSP)
     - non-linear systems (SNES)
     - time-dependent systems (TS)
     - generalized eigen-value problems (EPS)
   - writes results in either Gmsh or VTK (Paraview) format 
 * Cloud-first design (cloud friendliness is not enough)
 * MPI parallelization
 * Unix programming philosophy
   - rule of separation
   - rule of silence
   - rule of economy
 * Each PDE (i.e. from Laplace downward in the list of examples) is implemented in a subdirectory within [`src/pde`](https://github.com/seamplex/feenox/tree/main/src/pdes) of the source tree
   - any subdirectory can be removed if a particular PDE is not needed
   - any subdirectory can be used as a template to add a new PDE to the capabilities
 * Space, time and/or solution-dependent material properties and boundary conditions
   - algebraic expressions, and/or
   - point-wise interpolated data
 * Command-line argument expansion for
   - increased flexibility,
   - parametric sweeps, and/or
   - optimization loops
 * Steady-state, quasi-static and/or transient problems
 * Linear and non-linear problems
 * Possibility to verify the code using the Method of Manufactured Solutions

::::: {.only-in-format .html }

::: {.container .text-center .my-5 .ratio .ratio-16x9}
<iframe class="embed-responsive-item" src="https://www.youtube.com/embed/EZSjFzJAhbw?rel=0" allowfullscreen></iframe>
:::

::::: 
 
## Usefulness

 * Web-based thermo-mechanical solver running on the cloud
 * Non-conformal mesh mapping
 * ASME stress linearization for pressurized pipes and vessels
 * Environmentally-assisted fatigue analysis
 * Neutron transport in the cloud
 * Solving a maze without AI
 * Parametric NAFEMS LE10 benchmark: comparison of resource consumption for different FEA programs
 * Some Youtube videos


# Documentation

Browse through the [documentation index](https://seamplex.com/feenox/doc/) and/or the [`doc`](https://github.com/seamplex/feenox/tree/main/doc) subdirectory of the [Github repository](https://github.com/seamplex/feenox/) for

 * [FAQs](https://seamplex.com/feenox/doc/FAQ.html), including the [what FeenoX means](https://seamplex.com/feenox/doc/FAQ.html#what-does-feenox-mean)
 * [Manual](https://www.seamplex.com/feenox/doc/feenox-manual.html)
 * [Description](https://www.seamplex.com/feenox/doc/feenox-desc.html)
 * [Software Design Requirements](https://seamplex.com/feenox/doc/srs.html)
 * [Software Design Specifications](https://seamplex.com/feenox/doc/sds.html)
 * [Unix man page](https://www.seamplex.com/feenox/doc/feenox.1.html) (accesible through `man feenox` after installation)
 * [History](https://seamplex.com/feenox/doc/history.html)
 * [Compilation guide](https://seamplex.com/feenox/doc/compilation.html)
 * [Programming guide](https://seamplex.com/feenox/doc/programming.html)
 


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

# Licensing {#sec:licensing}

FeenoX is distributed under the terms of the [GNU General Public License](http://www.gnu.org/copyleft/gpl.html) version 3 or (at your option) any later version. The following text was borrowed from the [Gmsh documentation](http://gmsh.info/doc/texinfo/gmsh.html#Copying-conditions). Replacing “Gmsh” with “FeenoX” gives:

> ```include
> doc/freesw.md
> ```

```include
doc/licensing.md
```

## Contributing {#sec:contributing}

Contributions from hackers and/or academics are welcome, especially new types of PDEs and new formulations of existing PDEs.
For elliptic operators feel free to use the Laplace equation at `src/pdes/laplace` as a template.

 1. Read the [Programming Guide](doc/programming.md).
 2. Browse [Github discussions](https://github.com/seamplex/feenox/discussions) and open a new thread explaining what you want to do and/or asking for help.
 3. Fork the [Git repository](https://github.com/seamplex/feenox/) under your Github account
 4. Create a pull request, including
    - code,
    - documentation, and
    - tests.
 5. Follow up the review procedure.

Note that

 * It is mandatory to observe the [Code of Conduct](CODE_OF_CONDUCT.md).
 * The contributed code has to be compatible with the [GPLv3+ license](#sec:licensing).
 * Each author keeps the copyright of the contribution.


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
