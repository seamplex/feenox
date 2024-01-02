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
<a class="btn btn-lg btn-outline-primary"   href="https://www.seamplex.com/feenox/README4engineers.html" role="button">FeenoX for Industry Engineers</a>
<a class="btn btn-lg btn-outline-secondary" href="https://www.seamplex.com/feenox/README4hackers.html"   role="button">FeenoX for Unix Hackers</a>
<a class="btn btn-lg btn-outline-info"      href="https://www.seamplex.com/feenox/README4academics.html" role="button">FeenoX for Academic Professors</a>
```
:::::

Choose your background for further details about the what, how and whys:

 - [Industry Engineer](README4engineers.md)
 - [Unix Hacker](README4hackers.md)
 - [Academic Professor](README4academics.md)


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

 * The [problem](https://www.seamplex.com/feenox/doc/feenox-manual.html#problem) to solve is defined through a [syntactically-sugared](https://seamplex.com/feenox/doc/sds.html#syntactic-sugar-highlighting) self-descriptive English-like plain-text [input file](https://seamplex.com/feenox/doc/sds.html#sec:input) that should [resemble the original human-friendly problem formulation](https://seamplex.com/feenox/doc/sds.html#matching-formulations) as much as possible
   - [no need to recompile if the problem changes](https://seamplex.com/feenox/doc/sds.html#sec:introduction) (FeenoX is a _program_, not a library)
   - [nouns are definitions and verbs are instructions](https://seamplex.com/feenox/doc/sds.html#sec:nouns_verbs)
   - [simple problems need simple inputs](https://seamplex.com/feenox/doc/sds.html#sec:simple)
   - [everything is an expression](https://seamplex.com/feenox/doc/sds.html#sec:expression)
   - 100%-defined user output (no print or write instructions, no output)
   
   ```include
   doc/transfer.md
   ```
   
 * [General mathematical problems](https://seamplex.com/feenox/examples/basic.html) using [GNU GSL](https://www.gnu.org/software/gsl/)
 * [Sets of ODEs/DAEs](https://seamplex.com/feenox/examples/daes.html) using [SUNDIALS](https://computing.llnl.gov/projects/sundials)
 * [PDEs](https://github.com/seamplex/feenox/tree/main/src/pdes) formulated with the [finite element method](https://en.wikipedia.org/wiki/Finite_element_method)
   - reads mesh in [Gmsh](http://gmsh.info/) format
   - uses [PETSc](https://petsc.org/release/)/[SLEPc](https://slepc.upv.es/) to solve
     - linear systems ([KSP](https://petsc.org/release/manual/ksp/))
     - non-linear systems ([SNES](https://petsc.org/release/manual/snes/))
     - time-dependent systems ([TS](https://petsc.org/release/manual/ts/))
     - generalized eigen-value problems ([EPS](https://slepc.upv.es/documentation/current/docs/manualpages/EPS/index.html))
   - writes results in either Gmsh or [VTK](https://docs.vtk.org/en/latest/design_documents/VTKFileFormats.html) ([Paraview](https://www.paraview.org/)) format 
 * Cloud-first design (cloud friendliness is not enough)
 * MPI parallelization
 * [Unix programming philosophy](https://en.wikipedia.org/wiki/Unix_philosophy)
   - [rule of separation](https://seamplex.com/feenox/doc/sds.html#sec:separation)
   - [rule of silence](https://seamplex.com/feenox/doc/sds.html#sec:silence)
   - [rule of economy](https://seamplex.com/feenox/doc/sds.html#sec:economy)
 * Each PDE (i.e. from Laplace downward in the list of examples) [is implemented in a subdirectory](https://seamplex.com/feenox/doc/sds.html#sec:extensibility) within [`src/pde`](https://github.com/seamplex/feenox/tree/main/src/pdes) of the source tree
   - any subdirectory can be removed if a particular PDE is not needed
   - any subdirectory can be used as a template to add a new PDE to the capabilities
 * [Space](https://github.com/seamplex/feenox/blob/main/tests/thermal-two-squares-material-explicit-space.fee),
   [time](https://github.com/seamplex/feenox/blob/main/tests/nafems-t3-1d.fee) and/or solution-dependent
   [material properties](https://github.com/seamplex/feenox/blob/main/tests/wilson-2d.fee) and
   [boundary conditions](https://github.com/seamplex/feenox/blob/main/tests/radiation-as-convection-celsius.fee)
   - [algebraic expressions](https://seamplex.com/feenox/examples/mechanical.html#parallelepiped-whose-youngs-modulus-is-a-function-of-the-temperature), and/or
   - [point-wise interpolated data](https://seamplex.com/feenox/examples/mechanical.html#temperature-dependent-material-properties)
 * Command-line argument expansion for
   - [increased flexibility](https://seamplex.com/feenox/examples/basic.html#computing-the-derivative-of-a-function-as-a-unix-filter),
   - [parametric sweeps](https://seamplex.com/feenox/examples/mechanical.html#parametric-study-on-a-cantilevered-beam), and/or
   - [optimization loops](https://seamplex.com/feenox/examples/modal.html#optimizing-the-length-of-a-tuning-fork) (see also [this non-trivial example](https://seamplex.com/thesis/html/060-resultados/README.html#sec-tres-pescaditos))
 * [Steady-state](https://www.seamplex.com/feenox/doc/tutorials/120-mazes/#solving-the-steady-state-laplace-equation), [quasi-static] and/or [transient problems](https://www.seamplex.com/feenox/doc/tutorials/320-thermal/#sec:transient)
 * [Linear](https://www.seamplex.com/feenox/doc/tutorials/320-thermal/#linear-steady-state-problems) and [non-linear](https://www.seamplex.com/feenox/doc/tutorials/320-thermal/#non-linear-state-state-problems) problems
 * Possibility to verify the code using the [Method of Manufactured Solutions](https://github.com/seamplex/feenox/tree/main/tests/mms)
 * Separate [repository to profile and study code performance](https://github.com/seamplex/feenox-benchmark) using [Google's benchmark library](https://github.com/google/benchmark) 
 
::::: {.only-in-format .html }

::: {.container .text-center .my-5 .ratio .ratio-16x9}
<iframe class="embed-responsive-item" src="https://www.youtube.com/embed/EZSjFzJAhbw?rel=0" allowfullscreen></iframe>
:::

::::: 
 
## Usefulness

 * [Web-based thermo-mechanical solver running on the cloud](https://www.youtube.com/watch?v=DOnoXo_MCZg)
 * [Non-conformal mesh mapping](https://github.com/gtheler/feenox-non-conformal-mesh-interpolation)
 * [ASME stress linearization for pressurized pipes and vessels](https://github.com/seamplex/pipe-linearize)
 * [Assessment of material properties from tabulated sources](https://seamplex.com/feenox/examples/basic.html#on-the-evaluation-of-thermal-expansion-coefficients)
 * [Environmentally-assisted fatigue analysis in dissimilar interfaces of nuclear pipes](https://github.com/seamplex/piping-asme-fatigue) 
 * [Neutron transport in the cloud](https://seamplex.com/thesis/html/front/abstract.html)
 * [Solving mazes without AI](https://www.linkedin.com/feed/update/urn:li:activity:6973982270852325376/)
 * [Parametric NAFEMS LE10 benchmark: comparison of resource consumption for different FEA programs](https://seamplex.com/feenox/tests/nafems/le10/)
 * [Some Youtube videos](https://seamplex.com/feenox/doc/#sec:youtube)


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

See the [download page](https://seamplex.com/feenox/download.html) and the [compilation guide](./doc/compilation.md) for  detailed information.

# Licensing {#sec:licensing}

FeenoX is distributed under the terms of the [GNU General Public License](http://www.gnu.org/copyleft/gpl.html) version 3 or (at your option) any later version. The following text was borrowed from the [Gmsh documentation](http://gmsh.info/doc/texinfo/gmsh.html#Copying-conditions). Replacing “Gmsh” with “FeenoX” gives:

> ```include
> doc/freesw.md
> ```

```include
doc/licensing.md
```

## Contributing {#sec:contributing}

```include
doc/contributing-guidelines.md
```

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
