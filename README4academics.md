---
title: FeenoX for Academics
titleblock: |
 FeenoX for Academics
 ====================
lang: en-US
number-sections: true
toc: true
...


# What

[FeenoX](https://www.seamplex.com/feenox/) is a [cloud-first](https://seamplex.com/feenox/doc/sds.html#cloud-first) [Unix](./README4hackers.md) stand-alone **program** (i.e. something your [run](https://www.seamplex.com/feenox/doc/feenox-manual.html#running-feenox), not something you have to link existing code against) that reads an [engineering-related problem](./README4engineers.md) in a [plain-text file containing definitions and instructions](https://seamplex.com/feenox/doc/sds.html#sec:input).
That is to say, it reads the problem to be solved at run time and does not require the user (most of the time these will be [industry engineers](./README4engineers.md) and not [hackers](./README4hackers.md) nor PhDs) to compile and link custom code in order to solve a problem because it is _not a library_.
It does not require the users to write a weak form of the PDE they want to solve, because most of them will not even know what a weak form is (and they certainly do not need to know that).
The user chooses from a set of built-in PDEs using the [`PROBLEM`](https://www.seamplex.com/feenox/doc/feenox-manual.html#problem) definition which internally resolves (at run time) a set of function pointers to the appropriate locations which will build the elemental objects which correspond the to chosen PDE.
The list of available PDEs can be peeked by executing the `feenox` binary with the `--pdes` option:

```
$ feenox --pdes
laplace
mechanical
modal
neutron_diffusion
neutron_sn
thermal
$ 
```

During the compilation procedure (based on Autotools), the source tree in [`src/pdes`](https://github.com/seamplex/feenox/tree/main/src/pdes) is parsed.
For each subdirectory, a new PDE is embedded into the compiled binary.
See below for further details about this [extensibility mechanism](https://www.seamplex.com/feenox/doc/sds.html#sec:extensibility).

This program then solves the problem and, eventually, writes the outputs which the input file requests with [explicit instructions](https://www.seamplex.com/feenox/doc/sds.html#sec:output) (and nothing if nothing is asked for) and returns back to the Unix shell:

```{.feenox include="doc/nafems-le10.fee"}
```

:::::: {.only-in-format .html }
```{=html}
<div id="cast-le10"></div>
<script>AsciinemaPlayer.create('doc/le10.cast', document.getElementById('cast-le10'), {cols:133,rows:32, poster: 'npt:0:3'});</script>
```
::::::

It can be seen as a Unix filter (or as a transfer function)

```include
doc/transfer.md
```

which, when zoomed in, acts as a "glue layer" between a mesher ([Gmsh](http://gmsh.info/)) and a library for solving large sparse problems ([PETSc](https://petsc.org/release/)) which for linear elastic looks as follows:

![](doc/transfer-le10-zoom.svg)\  


Further discussion can be found in the [tensile test tutorial](https://www.seamplex.com/feenox/doc/tutorials/110-tensile-test/).
Check out the section about [invocation](https://www.seamplex.com/feenox/doc/feenox-manual.html#running-feenox) in the [FeenoX manual](https://www.seamplex.com/feenox/doc/feenox-manual.html).


The design responds to a [Software Requirements Specification](https://www.seamplex.com/feenox/doc/srs.html) document that acts as a "request for quotations" of a computational engineering tool that should satisfy some fictitious (but plausible) requirements.
The [Software Design Specification](https://www.seamplex.com/feenox/doc/sds.html) document explains how FeenoX addresses each requirement of the SRS.

In principle, even though FeenoX can solve [generic numerical problems](https://seamplex.com/feenox/examples/basic.html) and [systems of ordinary differential/algebraic equations](https://seamplex.com/feenox/examples/daes.html), its main objective is to solve partial differential equations using the finite element method---eventually [in parallel using the MPI standard](https://seamplex.com/feenox/doc/sds.html#sec:scalability).
The current version can solve

```include
doc/examples-list.md
```

> **Heads up!** The background of FeenoX's main author is Nuclear Engineering.
> Hence,
>
>  * Two of the supported PDEs are related to neutron [diffusion](https://www.seamplex.com/feenox/examples/neutron_diffusion.html) and [transport](https://www.seamplex.com/feenox/examples/neutron_sn.html).
>  * There is a [PhD thesis (in Spanish only)](https://seamplex.com/thesis/html/) that discusses the design and implementation of FeenoX in view of core-level neutronics.

As mentioned in the previous section, FeenoX provides a [mechanism to add new types of PDEs](https://www.seamplex.com/feenox/doc/sds.html#sec:extensibility) by adding a new subdirectory to the [`src/pdes`](https://github.com/seamplex/feenox/tree/main/src/pdes) [directory of the source tree and then re-bootstrapping, re-configuring and re-compiling the code](https://seamplex.com/feenox/doc/sds.html#sec:extensibility).

Since in FeenoX's input file [everything is an expression](https://seamplex.com/feenox/doc/sds.html#sec:expression), the code is especially suited for [verification using the method of manufactured solutions](https://github.com/seamplex/feenox/tree/main/tests/mms).


 * See [FeenoX for Engineers](./README4engineers.md) for links to further examples and tutorials.
 * See [FeenoX for Hackers](./README4hackers.md) for more details about the implementation and the code.


# How

FeenoX tries to achieve its goals by...

 * standing on [both ethical (since it is free) and technical (since it is open source) ground](https://www.seamplex.com/feenox/doc/sds.html#sec:architecture) while interacting with other free and open specimens such as
    - operating systems
    - libraries
    - compilers
    - pre and post-processing tools
  thus encouraging science and engineering to shift from privative environments into the free world.

 * leveraging the [Unix programming philosophy](https://seamplex.com/feenox/doc/sds.html#sec:unix) to come up with a [cloud-first tool](https://seamplex.com/feenox/doc/sds.html#cloud-first) suitable to be [automatically deployed](https://seamplex.com/feenox/doc/sds.html#sec:deployment) and serve as the back end of web-based interfaces (fig. \ref{caeplex}).

 * providing a [ready-to-run program](https://seamplex.com/feenox/doc/sds.html#sec:execution) that reads [an input file at run time](https://seamplex.com/feenox/doc/sds.html#sec:input) (and not a library that has to be linked for each particular problem to be solved) as deliberate decision discussed in the [Software Design Specification](https://www.seamplex.com/feenox/doc/sds.html).

 * designing and implementing an extensibility mechanism to allow hackers and/or academics to add new PDE formulations by adding a new subdirectory to [`src/pdes`]() in the repository and then
   a. re-bootstrapping with `autogen.sh`,
   b. re-configuring with `configure`, and
   c. re-compiling with `make`

In effect, FeenoX provides a general mathematical framework to solve PDEs with a bunch of entry points (as [C functions](https://www.seamplex.com/feenox/doc/programming.html#languages)) where new types of PDEs (e.g. electromagnetism, fluid mechanics, etc.) can be added to the set of what FeenoX can solve.
This general framework provides means to

  - [parse the input file](https://seamplex.com/feenox/doc/sds.html#sec:nouns_verbs), [handle command-line arguments](https://seamplex.com/feenox/doc/sds.html#sec:run-time-arguments), [read mesh files](https://seamplex.com/feenox/doc/feenox-manual.html#read_mesh), [assign variables](https://seamplex.com/feenox/doc/feenox-manual.html#description), [evaluate conditionals](https://seamplex.com/feenox/doc/feenox-manual.html#if), [write results](https://seamplex.com/feenox/doc/sds.html#sec:output), etc.

    ```feenox
    PROBLEM laplace 2D
    READ_MESH square-$1.msh
    [...]
    WRITE_RESULTS FORMAT vtk
    ```
    
  - handle [material properties](https://seamplex.com/feenox/doc/sds.html#sec:flexibility) given as [algebraic expressions](https://seamplex.com/feenox/doc/sds.html#sec:expression) involving pointwise-defined functions of [space](https://seamplex.com/feenox/doc/tutorials/320-thermal/#sec:mms), [temperature](https://www.seamplex.com/feenox/examples/mechanical.html#temperature-dependent-material-properties), [time](https://seamplex.com/feenox/doc/tutorials/320-thermal/#from-a-steady-state), etc.
    
    ```feenox
    MATERIAL steel     E=210e3*(1-1e-3*(T(x,y,z)-20))   nu=0.3
    MATERIAL aluminum  E=69e3                           nu=7/25
    ```
    
  - read problem-specific [boundary conditions as algebraic expressions](https://seamplex.com/feenox/doc/tutorials/320-thermal/#temperature-dependent-heat-flux-radiation)
    
    ```feenox
    sigma = 5.670374419e-8  # W m^2 / K^4 as in wikipedia
    e = 0.98      # non-dimensional
    T0 = 1000     # K
    Tinf = 300    # K

    BC left  T=T0
    BC right q=sigma*e*(Tinf^4-T(x,y,z)^4)
    ```

  - access shape functions and its derivatives evaluated either at Gauss points or at arbitrary locations for computing elementary contributions to
     * [stiffness matrix](https://github.com/seamplex/feenox/blob/main/src/pdes/laplace/bulk.c#L33)
     * [mass matrix](https://github.com/seamplex/feenox/blob/main/src/pdes/modal/bulk.c#L98)
     * [right-hand side vector](https://github.com/seamplex/feenox/blob/main/src/pdes/thermal/bulk.c#L41)
     
    For example, this snippet would build the elemental stiffness matrix for the [Laplace problem](https://github.com/seamplex/feenox/tree/main/src/pdes/laplace):
    
    ```c
    int build_laplace_Ki(element_t *e, unsigned int q) {
      double wdet = feenox_fem_compute_w_det_at_gauss(e, q);
      gsl_matrix *B = feenox_fem_compute_B_at_gauss(e, q);
      feenox_call(feenox_blas_BtB_accum(B, wdet, feenox.fem.Ki));  
      return FEENOX_OK;
    }
    ```
    
    The calls for computing the weights and the matrices with the shape functions and/or their derivatives currently support first and second-order iso-geometric elements, but other element types can be added as well.    
    More complex cases involving non-uniform material properties, volumetric sources, etc. can be found in the actual source.

     
    
  - solve the discretized equations using the appropriate [PETSc](https://petsc.org/)/[SLEPc](https://slepc.upv.es/) objects, i.e.
    * [KSP](https://petsc.org/release/manual/ksp/) for [linear static problems](https://seamplex.com/feenox/doc/tutorials/320-thermal/#linear-steady-state-problems)
    * [SNES](https://petsc.org/release/manual/snes/) for [non-linear static problems](https://seamplex.com/feenox/doc/tutorials/320-thermal/#non-linear-state-state-problems)
    * [TS](https://petsc.org/release/manual/ts/) for [transient problems](https://seamplex.com/feenox/doc/tutorials/320-thermal/#sec:transient)
    * [EPS](https://slepc.upv.es/documentation/current/docs/manualpages/EPS/index.html) for [eigenvalue problems](https://seamplex.com/feenox/examples/neutron_diffusion.html#iaea-3d-pwr-benchmark)

The particular functions that implement each problem type are located in subdirectories [`src/pdes`](https://github.com/seamplex/feenox/tree/main/src/pdes), namely

```include
doc/pdes.md
```

Researchers with both knowledge of mathematical theory of finite elements and programming skills might, with the aid of [the community](https://github.com/seamplex/feenox/discussions), add support for other PDES.
They might do that by using one of these directories (say [`laplace`](https://github.com/seamplex/feenox/tree/main/src/pdes/laplace)) as a template and

 #. replace every occurrence of `laplace` in symbol names with the name of the new PDE
 #. modify the initialization functions in `init.c` and set 
     * the names of the unknowns
     * the names of the materials
     * the mathematical type and properties of problem 
     * etc.
 #. modify the contents of the elemental matrices in `bulk.c` in the FEM formulation of the problem being added
 #. modify the contents of how the boundary conditions are parsed and set in `bc.c`
 #. re-run `autogen.sh`, `./configure` and `make` to get a FeenoX executable with support for the new PDE.

As we mentioned in [FeenoX for hackers](./README4hackers.md), [Alan Kay](https://en.wikipedia.org/wiki/Alan_Kay)'s says: ["simple things should be simple and complex things should be possible."](https://www.quora.com/What-is-the-story-behind-Alan-Kay-s-adage-Simple-things-should-be-simple-complex-things-should-be-possible)
Of course, the addition of non-trivial PDEs is not straightforward, but possible (at that time we were discussing the first half of the quote, now we refer to the second part).
The [programming guide](doc/programming.md) contains further details about how to contribute to the code base.

 * See [FeenoX for engineers](./README4engineers.md) for more details about the problem types FeenoX can solve and how to solve them.
 * See [FeenoX for hackers](./README4hackers.md) for more technical details about how FeenoX works.



# Why

The world is already full of finite-element programs, and every day a grad student creates a new one from scratch.
So why adding FeenoX to the already-crowded space of FEA tools?
Because there are either

 a. libraries which need code to use them such as
    - Sparselizard
    - MoFEM
    - FEniCS
    - MFEM
 b. end-user programs which need a GUI such as
    - CalculiX
    - CodeAster

FeenoX sits in the middle. It is the only free and open-source tool that satisfies the [Software Requirements Specification](https://www.seamplex.com/feenox/doc/srs.html), including that...

 * in order to solve a problem one needs to prepare an [input file](https://seamplex.com/feenox/doc/sds.html#sec:input) (not a script) which is [read at run-time](https://seamplex.com/feenox/doc/sds.html#sec:execution) (not code which calls a library)
 * these input files can [expand generic command-line options using Bash syntax as `$1`, `$2`, etc.](https://seamplex.com/feenox/doc/sds.html#sec:run-time-arguments), which allow [parametric](https://seamplex.com/feenox/doc/sds.html#sec:parametric) or [optimization loops](https://seamplex.com/feenox/doc/sds.html#sec:optimization) driven by higher-level scripts
 * for solving PDEs, the input file has to refer to at least [one Gmsh `.msh` file](https://seamplex.com/feenox/doc/sds.html#sec:interoperability) that defines the domain where the PDE is solved
 * the [material properties and boundary conditions](https://seamplex.com/feenox/doc/sds.html#nafems-le10-benchmark) are defined using physical groups and not individual nodes nor elements, so the input file is independent of the mesh and thus can be [tracked with Git](https://seamplex.com/feenox/doc/sds.html#sec:git-friendliness) to increase [traceability and repeatability](https://seamplex.com/feenox/doc/sds.html#sec:traceability).
 * it uses the [Unix philosophy](https://seamplex.com/feenox/doc/sds.html#sec:unix) which, among others, [separates policy from mechanism](https://seamplex.com/feenox/doc/sds.html#sec:unix-separation) and thus FeenoX is a natural choice for web-based interfaces like [CAEplex](https://www.caeplex.com).
 

See [FeenoX for hackers](./README4hackers.md) for another explanation about why FeenoX is different from other computational tools.

 
::::: {.only-in-format .html }
```{=html}
<a class="btn btn-lg btn-outline-primary"   href="./README4engineers.md" role="button">FeenoX for Industry Engineers</a>
<a class="btn btn-lg btn-outline-secondary" href="./README4hackers.md"   role="button">FeenoX for Unix Hackers</a>
```
:::::
