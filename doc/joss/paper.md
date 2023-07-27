---
title: 'FeenoX: a cloud-first free no-fee no-X uniX-like finite-element(ish) computational engineering tool'
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

 * dynamical systems written as sets of ODEs/DAEs, or
 * steady or transient heat conduction problems, or
 * steady or quasi-static thermo-mechanical problems, or
 * modal analysis problems, or
 * core-level steady-state neutronics
 
in such a way that the input is a near-English text file that defines the problem to be solved.
FeenoX can be seen either as a finite-element(ish) tool with a particular design basis.
Two of the main features of this design basis are

 #. Simple problems ought to have simple inputs.
 #. There should be a one-to-one correspondence between the problem definition and FeenoX's input file (fig. \ref{le10}).
 
![The NAFEMS LE10 problem statement @national1990nafems and the corresponding FeenoX input illustrating the one-to-one correspondence between them.\label{le10}](nafems-le10-problem-input.svg)

# Statement of need

To better illustrate FeenoX’s unfair advantage, let us first consider what the options are when an engineer needs to to write a technical report, paper or document:


| Feature                | Microsoft Word |  Google Docs  |   Markdown   |   (La)TeX    |
|:-----------------------|:--------------:|:-------------:|:------------:|:------------:|
| Aesthetics             |  $\times$      | $\times$      | $\checkmark$ | $\checkmark$ |
| Other formats          |  $\sim$        | $\sim$        | $\checkmark$ | $\sim$       |
| Traceability           |  $\times$      | $\sim$        | $\checkmark$ | $\checkmark$ |
| Mobile-friendliness    |  $\times$      | $\checkmark$  | $\checkmark$ | $\times$     |
| Collaborativeness      |  $\times$      | $\checkmark$  | $\checkmark$ | $\sim$       |
| Licensing/openness     |  $\times$      | $\times$      | $\checkmark$ | $\checkmark$ |
| Non-nerd friendliness  |  $\checkmark$  | $\checkmark$  | $\sim$       | $\times$     |


It should be evident that Markdown^[Here “[Markdown](https://en.wikipedia.org/wiki/Markdown)” means ([Pandoc](https://pandoc.org/) + [Git](https://git-scm.com/) + [Github](https://github.com/) / [Gitlab](https://about.gitlab.com/) / [Gitea](https://gitea.com/}{Gitea}))] gives the best trade off.
We can then perform a similar analysis for the options available in order to solve an engineering problem casted as a partial differential equation, say by using a finite-element formulation:


| Feature               | Desktop GUIs | Web frontends |    FeenoX    |   Libraries  |
|:----------------------|:------------:|:-------------:|:------------:|:------------:|
| Flexibility           | $\sim$       | $\times$      | $\checkmark$ | $\checkmark$ |
| Scalability           | $\times$     | $\sim$        | $\checkmark$ | $\checkmark$ |
| Traceability          | $\times$     | $\sim$        | $\checkmark$ | $\checkmark$ |
| Cloud-friendliness    | $\times$     | $\checkmark$  | $\checkmark$ | $\checkmark$ |
| Collaborativeness     | $\times$     | $\checkmark$  | $\checkmark$ | $\times$     |
| Licensing/openness    | $\sim$       | $\times$      | $\checkmark$ | $\checkmark$ |
| Non-nerd friendliness | $\checkmark$ | $\checkmark$  | $\sim$       | $\times$     |


Therefore, on the one hand, FeenoX^[Here “FeenoX” means ([FeenoX](https://seamplex.com/feenox) + [Gmsh](http://gmsh.info) + [Paraview](https://www.paraview.org/) + [Git](https://git-scm.com/) + [Github](https://github.com/) / [Gitlab](https://about.gitlab.com/) / [Gitea](https://gitea.com/}{Gitea}))] is---in a certain sense---to desktop FEA programs (like [Code_Aster](https://www.code-aster.org/spip.php?rubrique2) with [Salome-Meca](https://www.code-aster.org/V2/spip.php?article303) or [CalculiX](http://www.calculix.de/) with [PrePoMax](https://prepomax.fs.um.si/)) and libraries (like [MoFEM](http://mofem.eng.gla.ac.uk/mofem/html/) or [Sparselizard](http://sparselizard.org/)) what [Markdown](https://commonmark.org/) is to Word and [(La)TeX](https://en.wikipedia.org/wiki/LaTeX), respectively and _deliberately_.

FeenoX meets fictitious-yet-plausible [Software Requirement Specifications](https://www.seamplex.com/feenox/doc/srs.html).
The FeenoX [Software Design Specifications](https://www.seamplex.com/feenox/doc/sds.html) address each requirement of the SRS.
The most important idea is that FeenoX provides a general mathematical framework to solve PDEs with a bunch of entry points (as C functions) where new types of PDEs (e.g. electromagnetism, fluid mechanics, etc.) can be added to the set of what FeenoX can solve. FeenoX will provide means to

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

  - access shape functions and its derivatives evaluated either at Gauss points or at arbitrary locations for computing elementary contributions to
     * stiffness matrix
     * mass matrix
     * right-hand side vector
    
  - solve the discretized equations using the appropriate [PETSc](https://petsc.org/) [@petsc-user-ref;@petsc-efficient] or [SLEPc](https://slepc.upv.es/) [@slepc-manual;@slepc-toms] objects, i.e.
    * [KSP](https://petsc.org/release/manual/ksp/) for linear static problems
    * [SNES](https://petsc.org/release/manual/snes/) for non-linear static problems
    * [TS](https://petsc.org/release/manual/ts/) for transient problems
    * [EPS](https://slepc.upv.es/documentation/current/docs/manualpages/EPS/index.html) for eigenvalue problems

This general framework constitutes the bulk of [FeenoX’s source code](https://github.com/seamplex/feenox).
The particular functions that implement each problem type are located in subdirectories [`src/pdes`](https://github.com/seamplex/feenox/tree/main/src/pdes), namely

 * [`laplace`](https://github.com/seamplex/feenox/tree/main/src/pdes/laplace)
 * [`thermal`](https://github.com/seamplex/feenox/tree/main/src/pdes/thermal)
 * [`mechanical`](https://github.com/seamplex/feenox/tree/main/src/pdes/mechanical)
 * [`modal`](https://github.com/seamplex/feenox/tree/main/src/pdes/modal)
 * [`neutron_diffusion`](https://github.com/seamplex/feenox/tree/main/src/pdes/neutron_difussion)
 * [`neutron_sn`](https://github.com/seamplex/feenox/tree/main/src/pdes/neutron_sn)
      
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


# References

