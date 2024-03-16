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

FeenoX is a
cloud-first
free
no-X
uniX-like
finite-element(ish) computational engineering tool designed to solve
engineering-related problems using
cloud servers in parallel in such a way that
the problem is defined in a plain-text near-English self descriptive
input file read at
run time,
without requiring further user intervention after
the invocation.
FeenoX meets fictitious-yet-plausible Software Requirement Specifications (SRS).
The FeenoX Software Design Specifications address each requirement of the SRS.
FeenoX provides a set of common
extents,
capabilities and
usefulness but offers different features (following slightly different spirits) for
industry engineers,
Unix hackers and
academic researchers.
The main features of this design basis are

 * The tool has to be an already-compiled program (not a library) so regular users do not have to compile anything to solve a problem.
 * Simple problems ought to need simple input files.
 * There should be a one-to-one correspondence between the problem definition and FeenoX's input file, as illustrated in fig. \ref{le10}.
 * There should be an extension mechanism to allow hackers and researchers to add new partial differential equations to the tool.
 
![The NAFEMS LE10 problem statement [@national1990nafems] and the corresponding FeenoX input illustrating the one-to-one correspondence between the two.\label{le10}](nafems-le10-problem-input.svg)


# Statement of need

Open-source finite-element tools are either

 a. libraries which need code to use them such as
    - [Sparselizard](http://sparselizard.org/) [@sparselizard]
    - [MoFEM](http://mofem.eng.gla.ac.uk/mofem/html/) [@mofem]
    - [FEniCS](https://fenicsproject.org/) [@fenics]
    - [MFEM](https://mfem.org/) [@mfem]
 b. end-user programs which need a GUI such as
    - [CalculiX](http://www.calculix.de/)
    - [CodeAster](https://code-aster.org)
    
FeenoX sits in the middle.
First, it can solve

 - [Basic mathematics](https://www.seamplex.com/feenox/examples/basic.html)
 - [Systems of ODEs/DAEs](https://www.seamplex.com/feenox/examples/daes.html)
 - [Laplace’s equation](https://www.seamplex.com/feenox/examples/laplace.html)
 - [Heat conduction](https://www.seamplex.com/feenox/examples/thermal.html)
 - [Linear elasticity](https://www.seamplex.com/feenox/examples/mechanical.html)
 - [Modal analysis](https://www.seamplex.com/feenox/examples/modal.html)
 - [Neutron diffusion](https://www.seamplex.com/feenox/examples/neutron_diffusion.html)
 - [Neutron $\text{S}_N$](https://www.seamplex.com/feenox/examples/neutron_sn.html)
 

Second, it is the only free and open-source tool that satisfies the [Software Requirement Specifications](https://www.seamplex.com/feenox/doc/srs.html), including that...

 * in order to solve a problem one needs to prepare a (relatively) simple input file (not a script nor a deck) which is read at run-time (not code which calls a library).
 For example, considering the [NAFEMS LE10 Benchmark problem](https://www.seamplex.com/feenox/examples/mechanical.html#nafems-le10-thick-plate-pressure-benchmark) from fig. \ref{le10}, FeenoX works as two "glue layers" [@raymond]

   1. between the mesher [Gmsh](http://gmsh.info/) [@gmsh] and the [PETSc library](https://petsc.org/release/) [@petsc-user-ref;@petsc-efficient]
   2. between the PETSc library and a post-processor such as [Paraview](https://www.paraview.org/) [@paraview]

   ![](transfer-le10-zoom.svg)\  
   
 * these input files can expand generic command-line options using Bash syntax as `$1`, `$2`, etc., which allow parametric or optimization loops driven by higher-level scripts.
 * for solving partial differential equations (PDEs), the input file has to refer to at least one Gmsh `.msh` file that defines the domain where the PDE is solved.
 * the material properties and boundary conditions are defined using physical groups and not individual nodes nor elements, so the input file is independent of the mesh and thus can be tracked with Git to increase traceability and repeatability.
 * it follows the Unix philosophy [@raymond] which, among others, separates policy from mechanism rendering FeenoX as a natural choice for web-based interfaces like [CAEplex](https://www.caeplex.com) (fig. \ref{caeplex}).

![[CAEplex](https://www.caeplex.com) is a web-based interface to solve thermo-mechanical problems in the cloud that uses FeenoX as the back end.\label{caeplex}](caeplex-ipad.jpg){width=65%}
 
FeenoX tries to achieve its goals by...

 * standing on both ethical (since it is free) and technical (since it is open source) grounds while interacting with other free and open operating systems, libraries, compilers and pre and post-processing tools, thus encouraging science and engineering to shift from privative environments into the free world.

 * leveraging the Unix programming philosophy to come up with a cloud-first tool suitable to be automatically deployed and serve as the back end of web-based interfaces such as CAEplex.

 * providing a ready-to-run program that reads an input file at run time (and not a library that has to be linked for each particular problem to be solved) as a deliberate design decision discussed in the [Software Design Specifications](https://www.seamplex.com/feenox/doc/sds.html).

 * designing and implementing an extensibility mechanism to allow hackers and/or academics to add new PDE formulations by adding a new subdirectory to [`src/pdes`]() in the repository and then
   a. re-bootstrapping with `autogen.sh`,
   b. re-configuring with `configure`, and
   c. re-compiling with `make`

In effect, FeenoX provides a general mathematical framework to solve PDEs with a bunch of entry points (as C functions) where new types of PDEs (e.g. electromagnetism, fluid mechanics, etc.) can be added to the set of what FeenoX can solve.
This general framework provides means to

  - [parse the input file](https://www.seamplex.com/feenox/doc/sds.html#sec:nouns_verbs), [handle command-line arguments](https://www.seamplex.com/feenox/doc/sds.html#sec:run-time-arguments), [read mesh files](https://www.seamplex.com/feenox/doc/feenox-manual.html#read_mesh), [assign variables](https://www.seamplex.com/feenox/doc/feenox-manual.html#description), [evaluate conditionals](https://www.seamplex.com/feenox/doc/feenox-manual.html#if), [write results](https://www.seamplex.com/feenox/doc/sds.html#sec:output), etc.

    ```feenox
    PROBLEM laplace 2D
    READ_MESH square-$1.msh
    [...]
    WRITE_RESULTS FORMAT vtk
    ```
    
  - handle [material properties](https://www.seamplex.com/feenox/doc/sds.html#sec:flexibility) given as [algebraic expressions](https://www.seamplex.com/feenox/doc/sds.html#sec:expression) involving pointwise-defined functions of [space](https://www.seamplex.com/feenox/doc/tutorials/320-thermal/#sec:mms), [temperature](https://www.seamplex.com/feenox/examples/mechanical.html#temperature-dependent-material-properties), [time](https://www.seamplex.com/feenox/doc/tutorials/320-thermal/#from-a-steady-state), etc.
    
    ```feenox
    MATERIAL steel     E=210e3*(1-1e-3*(T(x,y,z)-20))   nu=0.3
    MATERIAL aluminum  E=69e3                           nu=7/25
    ```
    
  - read problem-specific [boundary conditions as algebraic expressions](https://www.seamplex.com/feenox/doc/tutorials/320-thermal/#temperature-dependent-heat-flux-radiation)
    
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
    
    The calls for computing the weights and the matrices with the shape functions and/or their derivatives currently support first and second-order iso-geometric elements, but other element types can be added as well. More complex cases involving non-uniform material properties, volumetric sources, etc. can be found in the [examples](https://www.seamplex.com/feenox/examples/), [tutorials](https://www.seamplex.com/feenox/doc/tutorials/) and [tests](https://github.com/seamplex/feenox/tree/main/tests).

  - solve the discretized equations using the appropriate PETSc [@petsc-user-ref;@petsc-efficient] or [SLEPc](https://slepc.upv.es/) [@slepc-manual;@slepc-toms] objects, i.e.
    * [KSP](https://petsc.org/release/manual/ksp/) for [linear static problems](https://www.seamplex.com/feenox/doc/tutorials/320-thermal/#linear-steady-state-problems)
    * [SNES](https://petsc.org/release/manual/snes/) for [non-linear static problems](https://www.seamplex.com/feenox/doc/tutorials/320-thermal/#non-linear-state-state-problems)
    * [TS](https://petsc.org/release/manual/ts/) for [transient problems](https://www.seamplex.com/feenox/doc/tutorials/320-thermal/#sec:transient)
    * [EPS](https://slepc.upv.es/documentation/current/docs/manualpages/EPS/index.html) for [eigenvalue problems](https://www.seamplex.com/feenox/examples/neutron_diffusion.html#iaea-3d-pwr-benchmark)

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
     * the names of the material properties
     * the mathematical type and characteristics of problem 
     * etc.
 #. modify the contents of the elemental matrices in `bulk.c` in the FEM formulation of the problem being added
 #. modify the contents of how the boundary conditions are parsed and set in `bc.c`
 #. re-run `autogen.sh`, `./configure` and `make` to get a FeenoX executable with support for the new PDE.

The addition of non-trivial PDEs is not straightforward, but possible.
The [programming guide](https://www.seamplex.com/feenox/doc/programming.html) contains further details about how to contribute to the code base.

# Conclusions

FeenoX's main goal is to keep things simple as possible from the user’s point of view without sacrificing flexibility.
There exist other tools which are similar in functionality but differ in the way the problem is set up.
For example, FeniCSx uses the Unified Form Language where the PDE being solved has to be written by the user in weak form [@ufl].
This approach is very flexible, but even simple problems end up with non-trivial input files so it does not fulfill the first requirement stated in the summary.
As simple as it is, FeenoX is still pretty flexible. A proof of this fact is that its applications range from coupling neutronics with CFD in nuclear reactors [@milonga-openfoam] to providing a back end to [web-based thermo-mechanical solvers](https://www.caeplex.com).


# References

