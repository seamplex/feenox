---
title: 'FeenoX: a cloud-first finite-element(ish) computational engineering tool'
tags:
  - engineering
  - partial differential equations
  - differential algebraic equations
authors:
  - name: Jeremy Theler
    orcid: 0000-0002-4142-4980
    affiliation: "1, 2"
affiliations:
 - name: Seamplex, Argentina
   index: 1
 - name: Instituto Balseiro, Argentina
   index: 2
date: 13 August 2023
bibliography: paper.bib
syntax-definition: feenox.xml
---

# Summary

[FeenoX](https://seamplex.com/feenox) is a
[cloud-first](https://seamplex.com/feenox/doc/sds.html#cloud-first)
[free](https://www.seamplex.com/feenox/#licensing)
[no-X](https://www.seamplex.com/feenox/README4engineers.html#how-feenox-works)
[uniX-like](https://www.seamplex.com/feenox/doc/sds.html#sec:unix)
finite-element(ish) computational engineering tool designed to solve
[engineering-related problems](https://www.seamplex.com/feenox/#extents) using
[cloud servers in parallel](https://www.seamplex.com/feenox/doc/sds.html#sec:scalability) in such a way that
[the problem](https://www.seamplex.com/feenox/doc/sds.html#sec:scope) is defined in a plain-text near-English self descriptive
[input file](https://www.seamplex.com/feenox/doc/sds.html#sec:input) read at
[run time](https://www.seamplex.com/feenox/doc/sds.html#sec:architecture),
[without requiring further user intervention](https://www.seamplex.com/feenox/doc/sds.html#cloud-first) after
[the invocation](https://www.seamplex.com/feenox/doc/feenox-manual.html#running-feenox).
FeenoX meets a fictitious-yet-plausible [Software Requirement Specifications](https://www.seamplex.com/feenox/doc/srs.html).
The FeenoX [Software Design Specifications](https://www.seamplex.com/feenox/doc/sds.html) address each requirement of the SRS.
FeenoX provides a set of common
[extents](https://www.seamplex.com/feenox/#extents),
[capabilities](https://www.seamplex.com/feenox/#capabilities) and
[usefulness](https://www.seamplex.com/feenox/#usefulness) but offers different features (following slightly different spirits) for
[industry engineers](https://www.seamplex.com/feenox/README4engineers.html),
[Unix hackers](https://www.seamplex.com/feenox/README4hackers.html) and
[academic researchers](https://www.seamplex.com/feenox/README4academics.html).
The main features of this [design basis](https://seamplex.com/feenox/doc/sds.html) are

 * The tool has to be an [already-compiled program](https://www.seamplex.com/feenox/doc/sds.html#sec:architecture) (not a library) so regular users do not have to compile anything to solve a problem.
 * Simple problems ought to need [simple input files](https://www.seamplex.com/feenox/doc/sds.html#sec:simple).
 * There should be a [one-to-one correspondence](https://www.seamplex.com/feenox/doc/sds.html#sec:matching-formulations) between the problem definition and FeenoX's input file (illustrated in fig. \ref{le10}).
 * There should be an [extension mechanism](https://www.seamplex.com/feenox/doc/sds.html#sec:extensibility) to allow hackers and researchers to add new partial differential equations to the tool.
 
![The NAFEMS LE10 problem statement [@national1990nafems] and the corresponding FeenoX input illustrating the one-to-one correspondence between the two.\label{le10}](nafems-le10-problem-input.svg)


# Statement of need

Open-source finite-element tools are either

 a. libraries which need code to use them such as
    - [Sparselizard](http://sparselizard.org/)
    - [MoFEM](http://mofem.eng.gla.ac.uk/mofem/html/)
    - [FEniCS](https://fenicsproject.org/)
    - [MFEM](https://mfem.org/)
 b. end-user programs which need a GUI such as
    - [CalculiX](http://www.calculix.de/)
    - [CodeAster](https://code-aster.org)
    
FeenoX sits in the middle.
First, it can solve

 - [Basic mathematics](https://seamplex.com/feenox/examples/examples/basic.html)
 - [Systems of ODEs/DAEs](https://seamplex.com/feenox/examples/daes.html)
 - [Laplace’s equation](https://seamplex.com/feenox/examples/laplace.html)
 - [Heat conduction](https://seamplex.com/feenox/examples/thermal.html)
 - [Linear elasticity](https://seamplex.com/feenox/examples/mechanical.html)
 - [Modal analysis](https://seamplex.com/feenox/examples/modal.html)
 - [Neutron diffusion](https://seamplex.com/feenox/examples/neutron_diffusion.html)
 - [Neutron S$_N$](https://seamplex.com/feenox/examples/neutron_sn.html)
 

Second, it is the only free and open-source tool that satisfies the [Software Requirement Specifications](https://www.seamplex.com/feenox/doc/srs.html), including that...

 * in order to solve a problem one needs to prepare a (relatively) [simple input file](https://seamplex.com/feenox/doc/sds.html#sec:input) (not a script) which is [read at run-time](https://seamplex.com/feenox/doc/sds.html#sec:execution) (not code which calls a library).
 For example, considering the [NAFEMS LE10 Benchmark problem](https://seamplex.com/feenox/examples/mechanical.html#nafems-le10-thick-plate-pressure-benchmark) from fig. \ref{le10}, FeenoX works as two ["glue layers,"](https://www.linuxtopia.org/online_books/programming_books/art_of_unix_programming/ch04s03_1.html)

   1. between the mesher [Gmsh](http://gmsh.info/) and the [PETSc library](https://petsc.org/release/) [@petsc-user-ref;@petsc-efficient]
   2. between the [PETSc library](https://petsc.org/release/) and the post-processor [Paraview](https://www.paraview.org/)

   ![](transfer-le10-zoom.svg)\  
   
 * these input files can [expand generic command-line options using Bash syntax as `$1`, `$2`, etc.](https://seamplex.com/feenox/doc/sds.html#sec:run-time-arguments), which allow [parametric](https://seamplex.com/feenox/doc/sds.html#sec:parametric) or [optimization loops](https://seamplex.com/feenox/doc/sds.html#sec:optimization) driven by higher-level scripts
 * for solving partial differential equations (PDEs), the input file has to refer to at least [one Gmsh `.msh` file](https://seamplex.com/feenox/doc/sds.html#sec:interoperability) that defines the domain where the PDE is solved
 * the [material properties and boundary conditions](https://seamplex.com/feenox/doc/sds.html#nafems-le10-benchmark) are defined using physical groups and not individual nodes nor elements, so the input file is independent of the mesh and thus can be [tracked with Git](https://seamplex.com/feenox/doc/sds.html#sec:git-friendliness) to increase [traceability and repeatability](https://seamplex.com/feenox/doc/sds.html#sec:traceability).
 * it uses the [Unix philosophy](https://seamplex.com/feenox/doc/sds.html#sec:unix) which, among others, [separates policy from mechanism](https://seamplex.com/feenox/doc/sds.html#sec:unix-separation) and thus FeenoX is a natural choice for web-based interfaces like [CAEplex](https://www.caeplex.com) (fig. \ref{caeplex}).

![[CAEplex](https://www.caeplex.com) is a web-based interface to solve thermo-mechanical problems in the cloud.\label{caeplex}](caeplex-ipad.jpg){width=65%}
 
FeenoX tries to achieve its goals by...

 * standing on [both ethical (since it is free) and technical (since it is open source) ground](https://www.seamplex.com/feenox/doc/sds.html#sec:architecture) while interacting with other free and open operating systems, libraries, compilers and pre and post-processing tools, thus encouraging science and engineering to shift from privative environments into the free world.

 * leveraging the [Unix programming philosophy](https://seamplex.com/feenox/doc/sds.html#sec:unix) to come up with a [cloud-first tool](https://seamplex.com/feenox/doc/sds.html#cloud-first) suitable to be [automatically deployed](https://seamplex.com/feenox/doc/sds.html#sec:deployment) and serve as the back end of web-based interfaces such as [CAEplex](https://www.caeplex.com).

 * providing a [ready-to-run program](https://www.seamplex.com/feenox/doc/feenox-manual.html#running-feenox) that reads [an input file at run time](https://seamplex.com/feenox/doc/sds.html#sec:input) (and not a library that has to be linked for each particular problem to be solved) as deliberate decision discussed in the [Software Design Specifications](https://www.seamplex.com/feenox/doc/sds.html).

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

  - solve the discretized equations using the appropriate [PETSc](https://petsc.org/) [@petsc-user-ref;@petsc-efficient] or [SLEPc](https://slepc.upv.es/) [@slepc-manual;@slepc-toms] objects, i.e.
    * [KSP](https://petsc.org/release/manual/ksp/) for [linear static problems](https://seamplex.com/feenox/doc/tutorials/320-thermal/#linear-steady-state-problems)
    * [SNES](https://petsc.org/release/manual/snes/) for [non-linear static problems](https://seamplex.com/feenox/doc/tutorials/320-thermal/#non-linear-state-state-problems)
    * [TS](https://petsc.org/release/manual/ts/) for [transient problems](https://seamplex.com/feenox/doc/tutorials/320-thermal/#sec:transient)
    * [EPS](https://slepc.upv.es/documentation/current/docs/manualpages/EPS/index.html) for [eigenvalue problems](https://seamplex.com/feenox/examples/neutron_diffusion.html#iaea-3d-pwr-benchmark)

The particular functions that implement each problem type are located in subdirectories [`src/pdes`](https://github.com/seamplex/feenox/tree/main/src/pdes), namely

 * [`laplace`](https://github.com/seamplex/feenox/tree/main/src/pdes/laplace)
 * [`thermal`](https://github.com/seamplex/feenox/tree/main/src/pdes/thermal)
 * [`mechanical`](https://github.com/seamplex/feenox/tree/main/src/pdes/mechanical)
 * [`modal`](https://github.com/seamplex/feenox/tree/main/src/pdes/modal)
 * [`neutron_diffusion`](https://github.com/seamplex/feenox/tree/main/src/pdes/neutron_difussion)
 * [`neutron_sn`](https://github.com/seamplex/feenox/tree/main/src/pdes/neutron_sn)

Researchers with both knowledge of mathematical theory of finite elements and programming skills might, with the aid of [the community](https://github.com/seamplex/feenox/discussions), add support for other PDEs.
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

The addition of non-trivial PDEs is not straightforward, but possible.
The [programming guide](https://www.seamplex.com/feenox/doc/programming.html) contains further details about how to contribute to the code base.

# Conclusions

FeenoX's main goal is to keep things simple as possible from the user’s point of view without sacrificing flexibility.
There exist other tools which are similar in functionality but differ in the way the problem is set up.
For example, [FeniCSx](https://fenicsproject.org/) uses the Unified Form Language where the PDE being solved has to be written by the user in weak form [@ufl].
This approach is very flexible, but even simple problems end up with non-trivial input files so it does not fulfill the first requirement stated in the summary.
As simple as it is, FeenoX is still pretty flexible. A proof of this fact is that its applications range from coupling neutronics with CFD in nuclear reactors [@milonga-openfoam] to providing a back end to [web-based thermo-mechanical solvers](https://www.caeplex.com).


# References

