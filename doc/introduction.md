FeenoX can be seen either as

 * a syntactically-sweetened way of asking the computer to solve engineering-related mathematical problems, and/or
 * a finite-element(ish) tool with a particular design basis.

Note that some of the problems solved with FeenoX might not actually rely on the finite element method, but on general mathematical models and even on the finite volumes method. That is why we say it is a finite-element(ish) tool.

In other words, FeenoX is a computational tool to solve

 * dynamical systems written as sets of ODEs/DAEs, or
 * steady or quasi-static thermo-mechanical problems, or
 * steady or transient heat conduction problems, or
 * modal analysis problems, or
 * neutron diffusion or transport problems, or
 * community-contributed problems

in such a way that the input is a near-English text file that defines the problem to be solved.


One of the main features of this allegedly particular design basis is that **simple problems ought to have simple inputs** (_rule of simplicity_) or, quoting Alan Kay, “simple things should be simple, complex things should be possible.”

```include
thermal1d.md
```

```include
lorenz.md
```

```include
nafems-le10.md
```

Please note the following two points about both cases above:

 1. The input files are very similar to the statements of each problem in plain English words (_rule of clarity_). Those with some experience may want to compare them to the inputs decks (sic) needed for other common FEA programs.
 2. By design, 100% of FeenoX’ output is controlled by the user. Had there not been any `PRINT` or `WRITE_MESH` instructions, the output would have been empty, following the _rule of silence_. This is a significant change with respect to traditional engineering codes that date back from times when one CPU hour was worth dozens (or even hundreds) of engineering hours. At that time, cognizant engineers had to dig into thousands of lines of data to search for a single individual result. Nowadays, following the _rule of economy_, it is actually far easier to ask the code to write only what is needed in the particular format that suits the user.


Some basic rules are

 * FeenoX is just a **solver** working as a _transfer function_ between input and output files.
 
   ```include
   transfer.md
   ```
 
   Following the _rules of separation, parsimony and diversity_, **there is no embedded graphical interface** but means of using generic pre and post processing tools---in particular, [Gmsh](http://gmsh.info/) and [Paraview](https://www.paraview.org/) respectively. See also [CAEplex](www.caeplex.com) for a web-based interface.
 
 * The input files should be [syntactically sugared](https://en.wikipedia.org/wiki/Syntactic_sugar) so as to be as self-describing as possible.
 
 * **Simple** problems ought to need **simple** input files.
 
 * Similar problems ought to need similar input files.
 
 * **Everything is an expression**. Whenever a number is expected, an algebraic expression can be entered as well. Variables, vectors, matrices and functions are supported. Here is how to replace the boundary condition on the right side of the slab above with a radiation condition:
 
   ```feenox
   sigma = 1       # non-dimensional stefan-boltzmann constant
   e = 0.8         # emissivity 
   Tinf=1          # non-dimensional reference temperature
   BC right q=sigma*e*(Tinf^4-T(x)^4)
   ```
   
   This "everything is an expression" principle directly allows the application of the Method of Manufactured Solutions for code verification.
   
 * FeenoX should run natively in the cloud and be able to massively scale in parallel. See the [Software Requirements Specification](doc/sds.md) and the [Software Development Specification](doc/sds.md) for details.
 
Since it is free ([as in freedom](https://www.gnu.org/philosophy/free-sw.en.html)) and open source, contributions to add features (and to fix bugs) are welcome. In particular, each kind of problem supported by FeenoX (thermal, mechanical, modal, etc.) has a subdirectory of source files which can be used as a template to add new problems, as implied in the “community-contributed problems” bullet above (_rules of modularity and extensibility_). See the [documentation](doc) for details about how to contribute.
