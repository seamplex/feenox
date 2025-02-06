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

[![DOI](https://joss.theoj.org/papers/10.21105/joss.05846/status.svg)](https://doi.org/10.21105/joss.05846)


# About FeenoX

[FeenoX](https://seamplex.com/feenox) is a [cloud-first](https://seamplex.com/feenox/doc/sds.html#cloud-first) [free and open source](#licensing)  tool to solve [engineering-related problems](#extents) using a computer (or [many computers in parallel](https://www.seamplex.com/feenox/doc/sds.html#sec:scalability)) with a [particular design basis](https://seamplex.com/feenox/doc/sds.html).

::::: {.only-in-format .html }
```{=html}
<a class="btn btn-lg btn-outline-primary"   href="./README4engineers.md" role="button">FeenoX for Industry Engineers</a>
<a class="btn btn-lg btn-outline-secondary" href="./README4hackers.md"   role="button">FeenoX for Unix Hackers</a>
<a class="btn btn-lg btn-outline-info"      href="./README4academics.md" role="button">FeenoX for Academic Professors</a>
```
:::::

Choose your background for further details about the what, how and whys:

 - [Industry Engineer](./README4engineers.md)
 - [Unix Hacker](./README4hackers.md)
 - [Academic Professor](./README4academics.md)

See the following paper for a brief summary (and to [cite FeenoX in your work](./doc/FAQ.md#how-should-i-cite-feenox)):

 * Theler J, “FeenoX: a cloud-first finite-element(ish) computational engineering tool,” _Journal of Open Source Software_, vol. 9,
no. 95, p. 5846, Mar. 2024, doi: [10.21105/joss.05846](https://doi.org/10.21105/joss.05846).

> ### Open-source web-based UX for FeenoX
>
> Take a look at [SunCAE](https://www.seamplex.com/suncae) for an example of how to write a front end for FeenoX.

## Extents

### Examples

```include
doc/examples-list.md
```

:::::: {.only-in-format .html }
```{=html}
<div id="cast-le10"></div>
<script>AsciinemaPlayer.create('doc/le10.cast', document.getElementById('cast-le10'), {cols:133,rows:32, poster: 'npt:0:3'});</script>
```
::::::

### Tutorials

```include
doc/tutorials-list.md
```


### Tests

The [`tests`](https://github.com/seamplex/feenox/tree/main/tests) directory in the repository has hundreds of
 
 - `grep`-able examples
 - [unit and regression tests](https://seamplex.com/feenox/doc/compilation.html#sec:test-suite), 
 - (crude) mathematical & code verification tests (as in "are we solving right the equations?"),
 - subdirectories with further case studies
    - [`mms`](https://github.com/seamplex/feenox/tree/main/tests/mms)
      - [`thermal`](https://github.com/seamplex/feenox/tree/main/tests/mms/thermal)
        - [`2d`](https://github.com/seamplex/feenox/tree/main/tests/mms/thermal/2d)
        - [`3d`](https://github.com/seamplex/feenox/tree/main/tests/mms/thermal/3d)
    - [`nafems`](https://github.com/seamplex/feenox/tree/main/tests/nafems)
      - [`le10`](https://github.com/seamplex/feenox/tree/main/tests/nafems/le10)
 
## Capabilities

 * Both _free_ as in "free speech" and in "free beer"
 * The [problem](https://www.seamplex.com/feenox/doc/feenox-manual.html#problem) to solve is defined through a [syntactically-sugared](https://seamplex.com/feenox/doc/sds.html#sec:syntactic) [self-descriptive English-like plain-text input file](https://seamplex.com/feenox/doc/sds.html#sec:input) that should [resemble the original human-friendly problem formulation](https://seamplex.com/feenox/doc/sds.html#sec:matching-formulations) as much as possible:
   - [no need to recompile if the problem changes](https://seamplex.com/feenox/doc/sds.html#sec:introduction) (FeenoX is a _program_, not a library)
   - [nouns are definitions and verbs are instructions](https://seamplex.com/feenox/doc/sds.html#sec:nouns_verbs)
   - [simple problems need simple inputs](https://seamplex.com/feenox/doc/sds.html#sec:simple)
   - [everything is an expression](https://seamplex.com/feenox/doc/sds.html#sec:expression)
   - [100%-defined user output](https://seamplex.com/feenox/doc/sds.html#sec:output) (no [`PRINT`](https://www.seamplex.com/feenox/doc/feenox-manual.html#problem) nor [`WRITE_RESULTS`](https://www.seamplex.com/feenox/doc/feenox-manual.html#write_results) instructions, no output)
 * [Cloud-first design](https://seamplex.com/feenox/doc/sds.html#cloud-first) (cloud friendliness is not enough)
 * [MPI parallelization](https://seamplex.com/feenox/doc/sds.html#sec:scalability)
 * Leverages high-quality well-established free and open source libraries to solve...
   * [general mathematical problems](https://seamplex.com/feenox/examples/basic.html) using [GNU GSL](https://www.gnu.org/software/gsl/)
   * [sets of ODEs/DAEs](https://seamplex.com/feenox/examples/daes.html) using [SUNDIALS](https://computing.llnl.gov/projects/sundials)
   * [PDEs](https://github.com/seamplex/feenox/tree/main/src/pdes) formulated with the [finite element method](https://en.wikipedia.org/wiki/Finite_element_method)
     - reads mesh in [Gmsh](http://gmsh.info/) format
     - uses [PETSc](https://petsc.org/release/)/[SLEPc](https://slepc.upv.es/) to solve
       - linear systems ([KSP](https://petsc.org/release/manual/ksp/))
       - non-linear systems ([SNES](https://petsc.org/release/manual/snes/))
       - time-dependent systems ([TS](https://petsc.org/release/manual/ts/))
       - generalized eigen-value problems ([EPS](https://slepc.upv.es/documentation/current/docs/manualpages/EPS/index.html))
     - writes results in either Gmsh or [VTK](https://docs.vtk.org/en/latest/design_documents/VTKFileFormats.html) ([Paraview](https://www.paraview.org/)) format 
 * Focuses on [flexibility](https://seamplex.com/feenox/doc/sds.html#sec:flexibility), especially when defining [non-uniform multi-solid material properties from ASME tables](https://www.seamplex.com/feenox/doc/tutorials/320-thermal/#from-a-steady-state)
 * Follows the [Unix programming philosophy](https://seamplex.com/feenox/doc/sds.html#sec:unix)
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
 * [Command-line argument expansion](https://seamplex.com/feenox/doc/sds.html#sec:run-time-arguments) for
   - [increased flexibility](https://seamplex.com/feenox/examples/basic.html#computing-the-derivative-of-a-function-as-a-unix-filter),
   - [parametric sweeps](https://seamplex.com/feenox/doc/sds.html#sec:parametric), and/or
   - [optimization loops](https://seamplex.com/feenox/doc/sds.html#optimization-loops)
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

 * [CAEplex](https://www.caeplex.com): a [web-based thermo-mechanical solver running on the cloud](https://www.youtube.com/watch?v=DOnoXo_MCZg)
 * [Non-conformal mesh mapping](https://github.com/gtheler/feenox-non-conformal-mesh-interpolation)
 * [ASME stress linearization for pressurized pipes and vessels](https://github.com/seamplex/pipe-linearize)
 * [Assessment of material properties from tabulated sources](https://seamplex.com/feenox/examples/basic.html#on-the-evaluation-of-thermal-expansion-coefficients)
 * [Environmentally-assisted fatigue analysis in dissimilar interfaces of nuclear pipes](https://github.com/seamplex/piping-asme-fatigue) 
 * [Neutron transport in the cloud](https://seamplex.com/thesis/html/front/abstract.html)
 * [Solving mazes without AI](https://www.linkedin.com/feed/update/urn:li:activity:6973982270852325376/)
 * [Parametric NAFEMS LE10 benchmark: comparison of resource consumption for different FEA programs](https://seamplex.com/feenox/tests/nafems/le10/)
 * [Some Youtube videos](https://seamplex.com/feenox/doc/#sec:youtube)

::::: {.only-in-format .html }

::: {.container .text-center .my-5 .ratio .ratio-16x9}
<iframe class="embed-responsive-item" src="https://www.youtube.com/embed/b3K2QewI8jE?rel=0" allowfullscreen></iframe>
:::

:::::

 
# Documentation

Browse through the [documentation index](doc) and/or the [`doc`](https://github.com/seamplex/feenox/tree/main/doc) subdirectory of the [Github repository](https://github.com/seamplex/feenox/) for

 * [FAQs](./doc/FAQ.md), including [what FeenoX means](https://seamplex.com/feenox/doc/FAQ.html#what-does-feenox-mean)
 * [Manual](https://seamplex.com/feenox/doc/feenox-manual.html)
 * [Description](https://www.seamplex.com/feenox/doc/feenox-desc.html)
 * [Software Design Requirements](https://seamplex.com/feenox/doc/srs.html)
 * [Software Design Specification](https://seamplex.com/feenox/doc/sds.html)
 * [Unix man page](https://seamplex.com/feenox/doc/feenox.1.html) (accessible through `man feenox` after installation)
 * [History](./doc/history.md)
 * [Compilation guide](./doc/compilation.md)
 * [Programming guide](./doc/programming.md)
 


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

# Licensing

FeenoX is distributed under the terms of the [GNU General Public License](http://www.gnu.org/copyleft/gpl.html) version 3 or (at your option) any later version. The following text was borrowed from the [Gmsh documentation](http://gmsh.info/doc/texinfo/gmsh.html#Copying-conditions). Replacing “Gmsh” with “FeenoX” (using Unix's `sed`) gives:

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

FeenoX is licensed under [GNU GPL version 3](http://www.gnu.org/copyleft/gpl.html) or (at your option) any later version.  
FeenoX is free software: you are free to change and redistribute it.  
There is **NO WARRANTY**, to the extent permitted by law.  
