[FeenoX](https://www.seamplex.com/feenox) is to finite-element software and libraries what Markdown is to word processors and typesetting systems. It can be seen as

 * a syntactically-sweetened way of asking the computer to solve engineering-related mathematical problems, and/or
 * a finite-element(ish) tool with a particular design basis

Note that some of the problems solved with FeenoX might not actually rely on the finite element method, but on general mathematical models and even on the finite volumes method. That is why we say it is a finite-element(ish) tool.

> * FeenoX Overview Presentation, August 2021
>   - [Slides in PDF](https://www.seamplex.com/feenox/doc/2021-feenox.pdf)
>   - [Markdown examples sources](https://github.com/gtheler/2021-presentation)


```{.include}
design/thermal1d.md
design/lorenz-le11.md
```
 
In other words, FeenoX is a computational tool to solve

 * dynamical systems written as sets of ODEs/DAEs, or
 * steady or quasi-static thermo-mechanical problems, or
 * steady or transient heat conduction problems, or
 * modal analysis problems,
 * neutron diffusion or transport problems
 * community-contributed problems

in such a way that the input is a near-English text file that defines the problem to be solved. Some basic rules are

 * FeenoX is just a **solver** working as a _transfer function_ between input and output files. Following the _rules of separation, parsimony and diversity_, **there is no embedded graphical interface** but means of using generic pre and post processing tools---in particular, [Gmsh](http://gmsh.info/) and [Paraview](https://www.paraview.org/) respectively. See also [CAEplex](www.caeplex.com).
 
<!-- show the same output (NAFEMS LE1?) with Gmsh and Paraview, quads struct/tri unstruct. -->
 
 * The input files should be [syntactically sugared](https://en.wikipedia.org/wiki/Syntactic_sugar) so as to be as self-describing as possible.
 * Simple problems ought to need simple input files.
 * Similar problems ought to need similar input files.
 * Everything is an expression. Whenever a number is expected, an algebraic expression can be entered as well. Variables, vectors, matrices and functions are supported. Here is how to replace the boundary condition on the right side of the slab above with a radiation condition:
 
   ```feenox
   sigma = 1       # non-dimensional stefan-boltzmann constant
   e = 0.8         # emissivity 
   Tinf=1          # non-dimensional reference temperature
   BC right q=sigma*e*(Tinf^4-T(x)^4)
   ```
   
<!-- dnl this also allows a direct application of the MMS for verification (in addition to being open source!) -->
   
 * FeenoX should run natively in the cloud and be able to massively scale in parallel. See the [Software Requirements Specification](doc/sds.md) and the [Software Development Specification](doc/sds.md) for details.
 
Since it is free ([as in freedom](https://www.gnu.org/philosophy/free-sw.en.html)) and open source, contributions to add features (and to fix bugs) are welcome. In particular, each kind of problem supported by FeenoX (thermal, mechanical, modal, etc.) has a subdirectory of source files which can be used as a template to add new problems, as implied in the “community-contributed problems” bullet above (_rules of modularity and extensibility_). See the [documentation](doc) for details about how to contribute.