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

[FeenoX](https://www.seamplex.com/feenox) is a computational tool to solve (currently)

 * [dynamical systems written as sets of differential-algebraic equations](https://seamplex.com/feenox/examples/daes.html), or
 * [steady or transient heat conduction problems](https://seamplex.com/feenox/examples/thermal.html), or
 * [steady or quasi-static thermo-mechanical problems](https://seamplex.com/feenox/examples/mechanical.html), or
 * [modal analysis problems](https://seamplex.com/feenox/examples/modal.html), or
 * [steady-state neutronics](https://seamplex.com/feenox/examples/neutron_diffusion.html)
 
in such a way that the input is a near-English text file that defines the problem.
FeenoX is a cloud-first tool to solve engineering problems, with a particular design basis.
Two of the main features of this [design basis](https://seamplex.com/feenox/doc/sds.html) are

 #. Simple problems ought to have simple inputs.
 #. There should be a one-to-one correspondence between the problem definition and FeenoX's input file (fig. \ref{le10}).
 
![The NAFEMS LE10 problem statement [@national1990nafems] and the corresponding FeenoX input illustrating the one-to-one correspondence between the two.\label{le10}](nafems-le10-problem-input.svg)

# Statement of need

As discussed in the [README](https://seamplex.com/feenox/#why-feenox), FeenoX is---in a certain sense---to desktop FEA programs (like [Code_Aster](https://www.code-aster.org/spip.php?rubrique2) with [Salome-Meca](https://www.code-aster.org/V2/spip.php?article303) or [CalculiX](http://www.calculix.de/) with [PrePoMax](https://prepomax.fs.um.si/)) and libraries (like [MoFEM](http://mofem.eng.gla.ac.uk/mofem/html/) or [Sparselizard](http://sparselizard.org/)) what [Markdown](https://commonmark.org/) is to Word and [(La)TeX](https://en.wikipedia.org/wiki/LaTeX), respectively and _deliberately_.

FeenoX meets a fictitious-yet-plausible [Software Requirement Specifications](https://www.seamplex.com/feenox/doc/srs.html).
The FeenoX [Software Design Specifications](https://www.seamplex.com/feenox/doc/sds.html) address each requirement of the SRS.
The most important idea is that FeenoX should give a general mathematical framework to solve PDEs with a bunch of entry points (as C functions) where new types of PDEs (e.g. electromagnetism, fluid mechanics, etc.) can be added to the set of problems FeenoX can solve. In particular, FeenoX provides means to

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

  - access element shape functions and its derivatives evaluated either at Gauss points or at arbitrary locations for computing elementary contributions to
  
     * stiffness matrix
     * mass matrix
     * right-hand side vector
     
    For example, this snippet would build the elemental stiffness matrix for the Laplace problem:
    
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
    * [KSP](https://petsc.org/release/manual/ksp/) for linear static problems
    * [SNES](https://petsc.org/release/manual/snes/) for non-linear static problems
    * [TS](https://petsc.org/release/manual/ts/) for transient problems
    * [EPS](https://slepc.upv.es/documentation/current/docs/manualpages/EPS/index.html) for eigenvalue problems

This general framework constitutes the bulk of [FeenoX’s source code](https://github.com/seamplex/feenox).
The particular functions that implement each problem type are located in subdirectories within [`src/pdes`](https://github.com/seamplex/feenox/tree/main/src/pdes), namely

 * [`laplace`](https://github.com/seamplex/feenox/tree/main/src/pdes/laplace)
 * [`thermal`](https://github.com/seamplex/feenox/tree/main/src/pdes/thermal)
 * [`mechanical`](https://github.com/seamplex/feenox/tree/main/src/pdes/mechanical)
 * [`modal`](https://github.com/seamplex/feenox/tree/main/src/pdes/modal)
 * [`neutron_diffusion`](https://github.com/seamplex/feenox/tree/main/src/pdes/neutron_difussion)
 * [`neutron_sn`](https://github.com/seamplex/feenox/tree/main/src/pdes/neutron_sn)
      
Engineers, researchers, scientists, developers and/or hobbyists just need to use one of these directories (say [`laplace`](https://github.com/seamplex/feenox/tree/main/src/pdes/laplace)) as a template and

 #. replace every occurrence of `laplace` in symbol names with the name of the new PDE
 #. modify the initialization functions in `init.c` and set 
     * the names of the unknowns (e.g `T` for thermal, `u`, `v` and `w` for elasticity, etc.)
     * the names of the material properties (e.g. `k` for thermal, `E` & `nu` for elasticity, etc.)
     * the mathematical type and properties of problem (linear/non-linear static, transient, eigen, etc.)
     * etc.
 #. modify the contents of the elemental matrices in `bulk.c` in the FEM formulation of the problem being added
 #. modify the contents of how the boundary conditions are parsed and set in `bc.c`
 #. re-run `autogen.sh`, `./configure` and `make` to get a FeenoX executable with support for the new PDE.

FeenoX's main goal is to keep things simple as possible from the user’s point of view without sacrificing flexibility.
There exist other tools which are similar in functionality but differ in the way the problem is set up.
For example, [FeniCSx](https://fenicsproject.org/) uses the Unified Form Language where the PDE being solved has to be written by the user in weak form [@ufl].
This approach is very flexible, but even simple problems end up with non-trivial input files so it does not fulfill the first requirement stated in the summary.
As simple as it is, FeenoX is still pretty flexible. A proof of this fact is that its applications range from coupling neutronics with CFD in nuclear reactors [@milonga-openfoam] to providing a back end to [web-based thermo-mechanical solvers](https://www.caeplex.com).


# References

