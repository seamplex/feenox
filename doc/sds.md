---
title: FeenoX Software Design Specification
lang: en-US
abstract: This Software Design Specifications (SDS) document applies to an imaginary Software Requirement Specifications (SRS) document issued by a fictitious agency asking for vendors to offer a free and open source cloud-based computational tool to solve engineering problems. The latter can be seen as a request for quotation and the former as an offer for the fictitious tender. Each section  of this SDS addresses one section of the SRS. The original text from the SRS is shown quoted at the very beginning before the actual SDS content.
number-sections: true
toc: true
documentclass: report
...

# Introduction {#sec:introduction}

> ```include
> 100-introduction.md
> ```

Besides noting that software being _free_ (regarding freedom, not price) does not imply the same as being _open source_, the requirement is clear in that the tool has to be both _free_ and _open source_, a combination which is usually called [FOSS](https://en.wikipedia.org/wiki/Free_and_open-source_software). This condition leaves all of the well-known non-free (i.e. wrongly-called "commercial") finite-element solvers in the market (NASTRAN, Abaqus, ANSYS, Midas, etc.) out of the tender.

```include
licensing.md
```

## Objective {#sec:objective}

> ```include
> 110-objective.md
> ```

The choice of the initial supported features is based on the types of problem that the FeenoX' precursor codes (namely wasora, Fino and milonga, referred to as "previous versions" from now ow) already have been supporting since more than ten years now. It is also a first choice so scope can be bounded. A subsequent road map and release plans can be designed as requested. FeenoX' first version includes a subset of the required functionality, namely

 * open and closed-loop dynamical systems
 * Laplace/Poisson/Helmholtz equations
 * heat conduction
 * mechanical elasticity
 * structural modal analysis
 * multi-group neutron transport and diffusion
 
FeenoX is designed to be developed and executed under GNU/Linux, which is the architecture of more than 95% of the internet servers which we collectively call “the cloud.” It should be noted that GNU/Linux is a POSIX-compliant version of UNIX and that FeenoX follows the rules of UNIX philosophy for the actual computational implementation. Besides POSIX, as explained further below, FeenoX also uses MPI which is a well-known industry standard for massive parallel executions of processes, both in multi-core hosts and multi-hosts environments. Finally, if performance and/or scalability are not important issues, FeenoX can be run in a (properly cooled) local PC or laptop.

The requirement to run in the cloud and scale up as needed rules out some the open source solver [CalculiX](http://www.calculix.de/). There are some other requirements in the SRS that also leave CalculiX out of the tender.

## Scope {#sec:scope}

> ```include
> 120-scope.md
> ```


Indeed, FeenoX is designed to work very much like a transfer function between one (or more) files and zero or more output files:

```include
transfer.md
```

Technically speaking, FeenoX can be seen as a UNIX filter designed to read an ASCII-based stream of characters (i.e. the input file, which in turn can include other input files or read data from mesh and/or data files) and to write ASCII-formatted data into the standard output and/or other files. The input file can be created either by a human or by another programs. The output stream and/or files can be read by either a human and/or another programs.
A quotation from [Eric Raymond](http://www.catb.org/esr/)’s [The Art of Unix Programming](http://www.catb.org/esr/writings/taoup/) helps to illustrate this idea:

> [Doug McIlroy](https://en.wikipedia.org/wiki/Douglas_McIlroy), the inventor of [Unix pipes](https://en.wikipedia.org/wiki/Pipeline_%28Unix%29) and one of the founders of the [Unix tradition](https://en.wikipedia.org/wiki/Unix), had this to say at the time:
>
>   (i) Make each program do one thing well. To do a new job, build afresh rather than complicate old programs by adding new features.
>
>   (ii) Expect the output of every program to become the input to another, as yet unknown, program. Don't clutter output with extraneous information. Avoid stringently columnar or binary input formats. Don't insist on interactive input.
>
> [...]
>
> He later summarized it this way (quoted in “A Quarter Century of Unix” in 1994):
>
>   * This is the Unix philosophy: Write programs that do one thing and do it well. Write programs to work together. Write programs to handle text streams, because that is a universal interface.


Keep in mind that even though the quotes above and many FEA programs that are still mainstream today date both from the early 1970s, fifty years later they still

 * Do not make just only one thing well.
 * Do complicate old programs by adding new features.
 * Do not expect the their output to become the input to another.
 * Do clutter output with extraneous information.
 * Do use stringently columnar and/or binary input (and output!) formats.
 * Do insist on interactive output.


```include
lorenz.md
```

As already stated, FeenoX is designed and implemented following the UNIX philosophy in general and Eric Raymond's 17 Unix Rules ([sec:unix]) in particular. One of the main ideas is the rule of _separation_ that essentially implies to separate mechanism from policy, that in the computational engineering world translates into separating the frontend from the backend. The usage of FeenoX to compute and of gnuplot to plot is a clear example of separation.
Even though most FEA programs eventually separate the interface from the solver up to some degree, there are cases in which they are still dependent such that changing the former needs updating the latter.

From the very beginning, FeenoX is designed as a pure backend which should nevertheless provide appropriate mechanisms for different frontends to be able to communicate and to provide a friendly interface for the final user. Yet, the separation is complete in the sense that the nature of the frontends can radically change (say from a desktop-based point-and-click program to a web-based immersive augmented-reality application) without needing the modify the backend. Not only far more flexibility is given by following this path, but also develop efficiency and quality is encouraged since programmers working on the lower-level of an engineering tool usually do not have the skills needed to write good user-experience interfaces, and conversely.

In the very same sense, FeenoX does not discretize continuous domains for PDE problems itself, but relies on separate tools for this end. Fortunately, there already exists one meshing tool which is FOSS (GPLv2) and shares most (if not all) of the design basis principles with FeenoX: the three-dimensional finite element mesh generator [Gmsh](http://gmsh.info/).
Strictly speaking, FeenoX does not need to be used along with Gmsh but with any other mesher able to write meshes in Gmsh's format `.msh`. But since Gmsh also

 * is free and open source,
 * works also in a transfer-function-like fashion,
 * runs natively on GNU/Linux,
 * has a similar (but more comprehensive) API for Python/Julia,
 * etc.
 
it is a perfect match for FeenoX. Even more, it provides suitable domain decomposition methods (through other FOSS third-party libraries such as [Metis](http://glaros.dtc.umn.edu/gkhome/metis/metis/overview)) for scaling up large problems.

```include
nafems-le10.md
```

See <https://www.caeplex.com> for a mobile-friendly web-based interface for solving finite elements in the cloud directly from the browser.

Even though the initial version of FeenoX does not provide an API for high-level interpreted languages such as Python or Julia, the code is written in such a way that this feature can be added without needing a major refactoring. This will allow to fully define a problem in a procedural way, increasing also flexibility.


# Architecture {#sec:architecture}

> ```include
> 200-architecutre.md
> ```

FeenoX can be seen as a third-system effect, being the third version written from scratch after a first implementation in 2009 and an second one which was far more complex and had far more features circa 2012--2014. The third attempt explicitly addresses the "do one thing well" idea from UNIX. 

Furthermore, not only is FeenoX itself both [free](https://www.gnu.org/philosophy/free-sw.en.html) and [open-source](https://opensource.com/resources/what-open-source) software but, following the _rule of composition_, it also is designed to connect and to work with  other free and open source software such as

 * [Gmsh](http://gmsh.info/) for pre and/or post-processing
 * [ParaView](https://www.paraview.org/) for post-processing
 * [Gnuplot](http://gnuplot.info/) for plotting
 * [Pyxplot](http://www.pyxplot.org.uk/) for plotting
 * [Pandoc](https://pandoc.org/) for creating tables and documents
 * [TeX](https://tug.org/) for creating tables and documents
 
and many others, which are readily available in all major GNU/Linux distributions.

FeenoX also makes use of high-quality free and open source mathematical libraries which contain numerical methods designed by mathematicians and programmed by professional programmers. In particuar, it depends on

 * [GNU Scientific Library](https://www.gnu.org/software/gsl/) for general mathematics,
 * [SUNDIALS IDA](https://computing.llnl.gov/projects/sundials/ida) for ODEs and DAEs,
 * [PETSc](http://https://petsc.org) for PDEs, and
 * [SLEPc](http://slepc.upv.es/) for PDEs involving eigen problems
 
Therefore, if one zooms in into the block of the transfer function above, FeenoX can also be seen as a glue layer between the input file and the mesh defining a PDE-casted problem and the mathematical libraries used to solve the discretized equations. This way, FeenoX bounds its scope to do only one thing and to do it well: to build and solve finite-element formulations of thermo-mechanical problems. And it does so on high grounds, both

 i. ethical: since it is [free software](https://www.gnu.org/philosophy/open-source-misses-the-point.en.html), all users can
      0. run,
      1. share,
      2. modify, and/or
      3. re-share their modifications.
      
    If a user cannot read or write code to make FeenoX suit her needs, at least she has the _freedom_ to hire someone to do it for her, and
    
 ii. technological: since it is [open source](http://www.catb.org/~esr/writings/cathedral-bazaar/cathedral-bazaar/), advanced users can detect and correct bugs and even improve the algorithms. [Given enough eyeballs, all bugs are shallow.](https://en.wikipedia.org/wiki/Linus%27s_law)

FeenoX’ main development architecture is Debian GNU/Linux running over 64-bits Intel-compatible processors.
All the dependencies are free and/or open source and already available in Debian’s official repositories, as explained in @sec:deployment.

The POSIX standard is followed whenever possible, allowing thus FeenoX to be compiled in other operating systems and architectures such as Windows (using [Cygwin](https://www.cygwin.com/)) and MacOS. The build procedure is the well-known and mature `./configure && make` command. 

FeenoX is written in plain\ C conforming to the ISO\ C99 specification (plus POSIX extensions), which is a standard, mature and widely supported language with compilers for a wide variety of architectures. 
For its basic mathematical capabilities, FeenoX uses the [GNU Scientifc Library](https://www.gnu.org/software/gsl/).
For solving ODEs/DAEs, FeenoX relies on [Lawrence Livermore’s SUNDIALS library](https://computing.llnl.gov/projects/sundials/ida). For PDEs, FeenoX uses [Argonne’s PETSc library](https://www.mcs.anl.gov/petsc/) and [Universitat Politècnica de València's SLEPc library](https://slepc.upv.es/). All of them are

 * free and open source,
 * written in C (neither Fortran nor C++),
 * mature and stable,
 * actively developed and updated,
 * very well known in the industry and academia.

Moreover, PETSc and SLEPc are scalable through the [MPI standard](https://www.mcs.anl.gov/research/projects/mpi/standard.html). This means that programs using both these libraries can run on either large high-performance supercomputers or low-end laptops. FeenoX has been run on

 - Raspberry Pi
 - Laptop (GNU/Linux & Windows 10)
 - Macbook
 - Desktop PC
 - Bare-metal servers
 - Vagrant/Virtualbox
 - Docker/Kubernetes
 - AWS/DigitalOcean/Contabo

Due to the way that FeenoX is designed and the policy separated from the mechanism, it is possible to control a running instance remotely from a separate client which can eventually run on a mobile device ([@fig:caeplex-ipad]).

![The web-based platform [CAEplex](https://www.caeplex.com) is mobile-friendly. <https://www.youtube.com/watch?v=7KqiMbrSLDc>](caeplex-ipad.jpg){#fig:caeplex-ipad}


The following example illustrates how well FeenoX works as one of many links in a chain that goes from tracing a bitmap with the problem's geometry down to creating a nice figure with the results of a computation:

```include
mazes.md
```

## Deployment {#sec:deployment}

> ```include
> 210-deployment.md
> ```


As already stated, FeenoX can be compiled from its sources using the well-established `configure` & `make` procedure. The code’s source tree is hosted on Github so cloning the repository is the preferred way to obtain FeenoX, but source tarballs are periodically released too according to the requirements in @sec:traceability.

The configuration and compilation is based on [GNU Autotools](https://www.gnu.org/software/automake/manual/html_node/Autotools-Introduction.html) that has more than thirty years of maturity and it is the most portable way of compiling C code in a wide variety of UNIX variants. It has been tested with

 * GNU C compiler
 * LLVM Clang compiler
 * Intel C compiler

 
FeenoX depends on the four open source libraries stated in @sec:architecture, with the last three of them being optional. The only mandatory library is the GNU Scientific Library which is part of the GNU/Linux operating system and as such is readily available in all distributions as `libgsl-dev`. The sources of the rest of the optional libraries are also widely available in most common GNU/Linux distributions.

In effect, doing 

```terminal
sudo apt-get install gcc make libgsl-dev libsundials-dev petsc-dev slepc-dev
```

is enough to provision all the dependencies needed compile FeenoX from the source tarball with the full set of features. 
If using the Git repository as a source, then [Git](https://git-scm.com/) itself and the [GNU Autoconf](https://www.gnu.org/software/autoconf/) and [Automake](https://www.gnu.org/software/automake/) packages are also needed:

```terminal
sudo apt-get install git autoconf automake
```

Even though compiling FeenoX from sources is the recommended way to obtain the tool, since the target binary can be compiled using particularly suited compilation options, flags and optimizations (especially those related to MPI, linear algebra kernels and direct and/or iterative sparse solvers), there are also tarballs with usable binaries for some of the most common architectures---including some non-GNU/Linux variants. These binary distributions contain statically-linked executables that do not need any other shared libraries to be present on the target host, but their flexibility and efficiency is generic and far from ideal. Yet the flexibility of having an execution-ready distribution package for users that do not know how to compile C source code outweights the limited functionality and scalability of the tool.

For example, first PETSc can be built with a `-Ofast` flag:

```terminal
$ cd $PETSC_DIR
$ export PETSC_ARCH=linux-fast
$ ./configure --with-debug=0 COPTFLAGS="-Ofast"
$ make -j8
$ cd $HOME
```
 
And then not only can FeenoX be configured to use that particular PETSc build but also to use a different compiler such as Clang instead of GNU GCC and to use the same `-Ofast` flag to compile FeenoX itself:
   
```terminal
$ git clone https://github.com/seamplex/feenox
$ cd feenox
$ ./autogen.sh
$ export PETSC_ARCH=linux-fast
$ ./configure MPICH_CC=clang CFLAGS=-Ofast
$ make -j8
# make install
```
   
If one does not care about the details of the compilation, then a pre-compiled statically-linked binaries can be directly downloaded very much as when downloading Gmsh:
 
```terminal
$ wget http://gmsh.info/bin/Linux/gmsh-Linux64.tgz
$ wget https://seamplex.com/feenox/dist/linux/feenox-linux-amd64.tar.gz
```

Appendix has @sec:download more details about how to download and compile FeenoX. The full documentation contains a [compilation guide](https://seamplex.com/feenox/doc/compilation.html) with further detailed explanations of each of the steps involved.
Since all the commands needed to either download a binary executable or to compile from source with customized optimization flags can be automatized, FeenoX can be built into a container such as Docker. This way, deployment and scalability can be customized and tweaked as needed.


## Execution {#sec:execution}


> ```include
> 220-execution.md
> ```

As FeenoX is designed to run as a file filter (i.e. as a transfer function between input and output files) and it explicitly avoids having a graphical interface, the binary executable works as any other UNIX terminal command. When invoked without arguments, it prints its version (a through explanation of the versioning scheme is given in @sec:traceability), a one-line description and the usage options:

```terminal
$ feenox
FeenoX v0.1.77-g9325958
a free no-fee no-X uniX-like finite-element(ish) computational engineering tool

usage: feenox [options] inputfile [replacement arguments]

  -h, --help         display usage and commmand-line help and exit
  -v, --version      display brief version information and exit
  -V, --versions     display detailed version information
  -s, --sumarize     list all symbols in the input file and exit

Instructions will be read from standard input if “-” is passed as
inputfile, i.e.

    $ echo "PRINT 2+2" | feenox -
    4

Report bugs at https://github.com/seamplex/feenox or to jeremy@seamplex.com
Feenox home page: https://www.seamplex.com/feenox/
```

The program can also be executed remotely

 1. on a server through a SSH session
 2. in a container as part of a provisioning script


 
FeenoX provides mechanisms to inform its progress by writing certain information to devices or files, which in turn can be monitored remotely or even trigger server actions. Progress can be as simple as an ASCII bar (triggered with `--progress`) to more complex mechanisms like writing the status in a shared memory segment.

Regarding its execution, there are three ways of solving problems: direct execution, parametric runs and optimization loops.
 
### Direct execution

When directly executing FeenoX, one gives a single argument to the executable with the path to the main input file. For example, the following input computes the first twenty numbers of the [Fibonacci sequence](https://en.wikipedia.org/wiki/Fibonacci_number) using the closed-form formula

$$
f(n) = \frac{\varphi^n - (1-\varphi)^n}{\sqrt{5}}
$$

where $\varphi=(1+\sqrt{5})/2$ is the [Golden ratio](https://en.wikipedia.org/wiki/Golden_ratio):

```{.feenox include="fibo_formula.fee"}
```

FeenoX can be directly executed to print the function\ $f(n)$ for $n=1,\dots,20$ both to the standard output and to a file named `one` (because it is the first way of solving Fibonacci with Feenox):

```terminal
$ feenox fibo_formula.fee | tee one
1	1
2	1
3	2
4	3
5	5
6	8
7	13
8	21
9	34
10	55
11	89
12	144
13	233
14	377
15	610
16	987
17	1597
18	2584
19	4181
20	6765
$
```

Now, we could also have computed these twenty numbers by using the direct definition of the sequence into a vector $\vec{f}$ of size 20. This time we redirect the output to a file named `two`:

```{.feenox include="fibo_vector.fee"}
```

```terminal
$ feenox fibo_vector.fee > two
$ 
```

Finally, we print the sequence as an iterative problem and check that the three outputs are the same:

```{.feenox include="fibo_iterative.fee"}
```

```terminal
$ feenox fibo_iterative.fee > three
$ diff one two
$ diff two three
$
```

These three calls were examples of direct execution of FeenoX: a single call with a single argument to solve a single fixed problem.

## Parametric

To use FeenoX in a parametric run, one has to successively call the executable passing the main input file path in the first argument followed by an arbitrary number of parameters. These extra parameters will be expanded as string literals `$1`, `$2`, etc. appearing in the input file. For example, if `hello.fee` is

```{.feenox include="hello.fee"}
```

then

```terminal
$ feenox hello.fee World
Hello World!
$ feenox hello.fee Universe
Hello Universe!
$
```

To have an actual parametric run, an external loop has to successively call FeenoX with the parametric arguments. For example, say this file `cantilever.fee` fixes the face called "left" and sets a load in the negative\ $z$ direction of a mesh called `cantilever-$1-$2.msh`. The output is a single line containing the number of nodes of the mesh and the displacement in the vertical direction\ $w(500,0,0)$ at the center of the cantilever's free face:

```{.feenox include="cantilever.fee"}
```

::: {#fig:cantilever-mesh}
![Tetrahedra](cantilever-tet.png){width_latex=49%}
![Hexahedra](cantilever-hex.png){width_latex=49%}

Cantilevered beam meshed with structured tetrahedra and hexahedra
:::

Now the following Bash script first calls Gmsh to create the meshes `cantilever-${element}-${c}.msh` where

 * `${element}`: tet4, tet10, hex8, hex20, hex27
 * `${c}`: 1,2,\dots,10

It then calls FeenoX with the input above and passes `${element}` and `${c}` as extra arguments, which then are expanded as `$1` and `$2` respectively.
 
```{.bash include="cantilever.sh"}
```

After the execution of the Bash script, thanks to the design decision that output is 100% defined by the user (in this case with the `PRINT` instruction), one has several files `cantilever-${element}.dat` files. When plotted, these show the shear locking effect of fully-integrated first-order elements as illustrated in @fig:cantilever-displacement. The theoretical Euler-Bernoulli result is just a reference as, among other things, it does not take into account the effect of the material's Poisson's ratio.
Note that the abscissa shows the number of _nodes_, which are proportional to the number of degrees of freedom (i.e. the size of the problem matrix) and not the number of _elements_, which is irrelevant here and in most problems.


![Displacement at the free tip of a cantilevered beam vs. number of nodes for different element types](cantilever-displacement.svg){#fig:cantilever-displacement}


### Optimization loops

Optimization loops work very much like parametric runs from the FeenoX point of view. The difference is mainly on the calling script that has to implement a certain optimization algorithm such as conjugate gradients, Nelder-Mead, simulated annealing, genetic algorithms, etc. to choose which parameters to pass to FeenoX as command-line argument.
The only particularity on FeenoX' side is that since the next argument that the optimization loop will pass might depend on the result of the current step, care has to be taken in order to be able to return back to the calling script whatever results it needs in order to compute the next arguments. This is usually just the scalar being optimized for, but it can also include other results such as derivatives or other relevant data.

To illustrate how to use FeenoX in an optimization loop, let us consider the problem of finding the length\ $\ell_1$ of a tuning fork (@fig:fork-meshed) such that the fundamental frequency on a free-free oscillation is equal to the base\ A frequency at\ 440\ Hz.


![What length\ $\ell_1$ is needed so the fork vibrates at 440\ Hz?](fork-meshed.svg){#fig:fork-meshed width=20%}

This extremely simple input file (_rule of simplicity_) solves the free-free mechanical modal problem (i.e. without any Dirichlet boundary condition) and prints the fundamental frequency:

```{.feenox include="fork.fee"}
```

Note that in this particular case, the FeenoX input files does not expand any command-line argument. The trick is that the mesh file `fork.msh` is overwritten in each call of the optimization loop. Since this time the loop is slightly more complex than in the parametric run of the last section, we now use Python. The function `create_mesh()` first creates a CAD model of the fork with geometrical parameters $r$, $w$, $\ell_1$ and $\ell_2$. It then meshes the CAD using\ $n$ structured hexahedra through the fork's thickness. Both the CAD and the mesh are created using the Gmsh Python API. The detailed steps between `gmsh.initialize()` and `gmsh.finalize()` are not shown here, just the fact that this function overwrites the previous mesh and always writes it into the file called `fork.msh` which is the one that `fork.fee`  reads. Hence, there is no need to pass command-liner arguments to FeenoX. The full implementation of the function is available in the examples directory of the FeenoX distribution.

```python
import math
import gmsh
import subprocess  # to call FeenoX and read back

def create_mesh(r, w, l1, l2, n):
  gmsh.initialize()
  ...
  gmsh.write("fork.msh")  
  gmsh.finalize()
  return len(nodes)
  
def main():
  target = 440    # target frequency
  eps = 1e-2      # tolerance
  r = 4.2e-3      # geometric parameters
  w = 3e-3
  l1 = 30e-3
  l2 = 60e-3

  for n in range(1,7):   # mesh refinement level
    l1 = 60e-3              # restart l1 & error
    error = 60
    while abs(error) > eps:   # loop
      l1 = l1 - 1e-4*error
      # mesh with Gmsh Python API
      nodes = create_mesh(r, w, l1, l2, n)
      # call FeenoX and read scalar back
      # TODO: FeenoX Python API (like Gmsh)
      result = subprocess.run(['feenox', 'fork.fee'], stdout=subprocess.PIPE)
      freq = float(result.stdout.decode('utf-8'))
      error = target - freq
    
    print(nodes, l1, freq)
```

Since the computed frequency depends both on the length\ $\ell_1$ and on the mesh refinement level\ $n$, there are actually two nested loops: one parametric over $n=1,2\dots,7$ and the optimization loop itself that tries to find\ $\ell_1$ so as to obtain a frequency equal to 440\ Hz within 0.01% of error.

```terminal
$ python fork.py > fork.dat
$
``` 

![Estimated length\ $\ell_1$ needed to get 440\ Hz for different mesh refinement levels\ $n$](fork.svg){#fig:fork}

Note that the approach used here is to use Gmsh Python API to build the mesh and then fork the FeenoX executable to solve the fork (no pun intended). There are plans to provide a Python API for FeenoX so the problem can be set up, solved and the results read back directly from the script instead of needing to do a fork+exec, read back the standard output as a string and then convert it to a Python `float`.

@Fig:fork shows the results of the combination of the optimization loop over $\ell_1$ and a parametric run over\ $n$. The difference for $n=6$ and $n=7$ is in the order of one hundredth of millimeter.



## Efficiency {#sec:efficiency}

> ```include
> 230-efficiency.md
> ```

**TO DO**: re-solve NAFEMS LE10 for different meshes with

 * FeenoX
 * Code Aster
 * Sparselizard
 * CCX?
 
Measure (with `time`):

 * CPU time
 * Wall time
 * Memory

## Scalability  {#sec:scalability}
 
> ```include
> 240-scalability.md
> ```

 * OpenMP in PETSc
 * Gmsh partitions
 * run something big to see how it fails
 
 * show RAM vs. nodes for mumps & gamg

## Flexibility
 
> ```include
> 250-flexibility.md
> ```

FeenoX comes from nuclear + experience (what to do and what not to do)

Materials: a material library (perhaps included in a frontend GUI?) can write FeenoX’ material definitions. Flexiblity.
 
 * everything is an expression, show sophomore's identity
 * 1d & 2d interpolated data for functions
 * thermal transient valve with k(T) and BCs(x,t)

## Extensibility {#sec:extensibility}


> ```include
> 260-extensibility.md
> ```

 * user-provided routines
 * skel for pdes and annotated models
 * laplace skel

## Interoperability {#sec:interoperability}
 
> ```include
> 270-interoperatibility.md
> ```

 * UNIX
 * POSIX
 * shmem
 * mpi
 * Gmsh
 * moustache
 * print -> awk -> latex tables NUREG

# Interfaces

 
> ```include
> 300-interfaces.md
> ```

## Problem input {#sec:input}

> ```include
> 310-input.md
> ```

dar ejemplos
comparar con <https://cofea.readthedocs.io/en/latest/benchmarks/004-eliptic-membrane/tested-codes.html>

macro-friendly inputs, rule of generation

**Simple problems should need simple inputs.**

English-like input. Nouns are definitions, verbs are instructions.

**Similar problems should need similar inputs.**

thermal slab steady state and transient

1d neutron

VCS tracking, example with hello world.

API in C?

## Results output {#sec:output}

> ```include
> 320-output.md
> ```

JSON/YAML, state of the art open post-processing formats.
Mobile & web-friendly.

Common and preferably open-source formats.


100% user-defined output with PRINT, rule of silence
rule of economy, i.e. no RELAP
yaml/json friendly outputs
vtk (vtu), gmsh, frd?

90% is serial (vtk), no need to complicate due to 10%

# Quality assurance {#sec:qa}

> ```include
> 400-qa.md
> ```


## Reproducibility and traceability {#sec:traceability}

> ```include
> 410-reproducibility.md
> ```


simple <-> simple

similar <-> Similar

## Automated testing {#sec:testing}


> ```include
> 420-testing.md
> ```

make check

regressions, example of the change of a sign

## Bug reporting and tracking


> ```include
> 430-bugs.md
> ```

github

mailing listings

## Verification {#sec:verification}

> ```include
> 440-verification.md
> ```


open source (really, not like CCX -> mostrar ejemplo)
GPLv3+ free
Git + gitlab, github, bitbucket

## Validation

> ```include
> 450-validation.md
> ```

already done for Fino

hip implant, 120+ pages, ASME, cases of increasing complexity

## Documentation {#sec:documentation}

> ```include
> 460-documentation.md
> ```

it's not compact, but almost!
Compactness is the property that a design can fit inside a human being's head. A good practical test for compactness is this: Does an experienced user normally need a manual? If not, then the design (or at least the subset of it that covers normal use) is compact.
unix man page
markdown + pandoc = html, pdf, texinfo
 

\appendix
 
# Appendix: Downloading and compiling FeenoX

## Binary executables

```include
binary.md
```

## Source tarballs

```include
source.md
```

## Git repository

```include
git.md
```



# Appendix: Rules of UNIX philosophy {#sec:unix}

```{.include shift-heading-level-by=1}
unix.md
```
