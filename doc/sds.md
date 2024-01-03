---
title: FeenoX Software Design Specifications
lang: en-US
abstract: This Software Design Specifications (SDS) document applies to an imaginary [Software Requirement Specifications](./srs.md) (SRS) document issued by a fictitious agency asking for vendors to offer a free and open source cloud-based computational tool to solve engineering problems. The latter can be seen as a “Request for Quotation” and the former as an offer for the fictitious tender. Each section  of this SDS addresses one section of the SRS. The original text from the SRS is shown quoted at the very beginning before the actual SDS content.
number-sections: true
toc: true
documentclass: report
...

# Introduction {#sec:introduction}

> ```include
> 100-introduction.md
> ```

[FeenoX](https://www.seamplex.com/feenox/) is a cloud-first computational tool aimed at solving engineering problems with a particular design basis.

### "Cloud first" vs. "cloud friendly" {#cloud-first .unnumbered}

```include
cloud-first.md
```

### Unfair advantage {.unnumbered}

```include
word-md-tex.md
```

### Licensing {.unnumbered}

Besides noting that software being _free_ (regarding freedom, not price) does not imply the same as being _open source_, the requirement is clear in that the tool has to be both _free_ and _open source_, a combination which is usually called [FOSS](https://en.wikipedia.org/wiki/Free_and_open-source_software). 

```include
licensing.md
```

To sum up this introduction, FeenoX is...

 #. a cloud-first computational tool (not just cloud _friendly,_ but cloud **first**).
 #. to traditional computational software and to specialized libraries what [Markdown](https://en.wikipedia.org/wiki/Markdown) is to [Word](https://en.wikipedia.org/wiki/Microsoft_Word) and [TeX](https://en.wikipedia.org/wiki/TeX), respectively.
 #. both free ([as in freedom](https://en.wikipedia.org/wiki/Free_as_in_Freedom)) and open source.



## Objective {#sec:objective}

> ```include
> 110-objective.md
> ```

The choice of the initial supported features is based on the types of problem that the FeenoX's precursor codes (namely wasora, Fino and milonga, referred to as "previous versions" from now on) already have been supporting since more than ten years now. It is also a first usable version so scope can be bounded. A subsequent road map and release plans can be designed as requested. FeenoX's first version includes a subset of the required functionality, namely

 * open and closed-loop dynamical systems
 * Laplace/Poisson/Helmholtz equations
 * heat conduction
 * mechanical elasticity
 * structural modal analysis
 * multi-group neutron transport and diffusion

@Sec:extensibility explains the mechanisms that FeenoX provides in order to add (or even remove) other types of problems  to be solved.
 
Recalling that FeenoX is a “cloud-first” tool, it is designed to be developed and executed primarily on [GNU/Linux](https://www.gnu.org/gnu/linux-and-gnu.html) hosts, which is the architecture of more than 90% of the internet servers which we collectively call “the public cloud.” It should be noted that GNU/Linux is a [POSIX](https://en.wikipedia.org/wiki/POSIX)-compliant operating system which is compatible with [Unix](https://en.wikipedia.org/wiki/Unix), and that FeenoX follows the rules of Unix philosophy (further explained in @sec:unix) for the actual computational implementation. Besides the POSIX standard, as explained further below in @sec:scalability, FeenoX also uses [MPI](https://en.wikipedia.org/wiki/Message_Passing_Interface) which is a well-known industry standard for massive execution of parallel processes, both in multi-core hosts and multi-hosts environments. Finally, if performance and/or scalability are not important issues, FeenoX can be run in a (properly cooled) local PC, laptop or even in embedded systems such as [Raspberry\ Pi](https://en.wikipedia.org/wiki/Raspberry_Pi) (see @sec:architecture).


## Scope {#sec:scope}

> ```include
> 120-scope.md
> ```


Since FeenoX is designed to be executed _in the cloud_, it works very much like a transfer function between one (or more) files and zero or more output files:

```include
transfer.md
```

Technically speaking, FeenoX can be seen as a [Unix filter](https://en.wikipedia.org/wiki/Filter_(software)) designed to read an [ASCII](https://en.wikipedia.org/wiki/ASCII)-based stream of characters (i.e. the input file, which in turn can include other input files or contain instructions to read data from mesh and/or other data files) and to write ASCII-formatted data into the standard output and/or other files. The input file can be prepared either by a human or by another program. The output stream and/or files can be read by either a human and/or another programs.
A quotation from [Eric Raymond](http://www.catb.org/esr/)’s [The Art of Unix Programming](http://www.catb.org/esr/writings/taoup/) helps to illustrate this idea:

> [Doug McIlroy](https://en.wikipedia.org/wiki/Douglas_McIlroy), the inventor of [Unix pipes](https://en.wikipedia.org/wiki/Pipeline_%28Unix%29) and one of the founders of the [Unix tradition](https://en.wikipedia.org/wiki/Unix_philosophy), had this to say at the time:
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


Keep in mind that even though both the quotes above and many finite-element programs that are still mainstream today date both from the early 1970s, fifty years later the latter still

 * do not make just only one thing well,
 * do complicate old programs by adding new features,
 * do not expect the their output to become the input to another,
 * do clutter output with extraneous information,
 * do use stringently columnar and/or binary input (and output!) formats, and/or
 * do insist on interactive output.

There are other FEA tools that even though born closer in time, also follow the above bullets literally.

```include
lorenz.md
```

As already stated, FeenoX is designed and implemented following the UNIX philosophy in general and Eric Raymond's\ 17\ Unix Rules (@sec:unix) in particular. One of the main ideas is the _rule of separation_ (@sec:separation) that essentially asks to separate mechanism from policy, that in the computational engineering world translates into separating the frontend from the backend. The usage of FeenoX to compute and of Gnuplot to plot is a clear example of separation. Same idea applies to partial differential equations (PDEs), where the mesh is created with Gmsh and the output can be post-processed with Gmsh, Paraview or any other post-processing system (even a web-based interface) that follows rule of separation.
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
> 200-architecture.md
> ```

Very much like the C language (after A & B) and Unix itself (after a first attempt and the failed MULTICS), FeenoX can be seen as a third-system effect:

> A notorious ‘second-system effect’ often afflicts the successors of small experimental prototypes. The urge to add everything that was left out the first time around all too frequently leads to huge and overcomplicated design. Less well known, because less common, is the ‘third-system effect’: sometimes, after the second system has collapsed of its own weight, there is a chance to go back to simplicity and get it right.
> 
> From [Eric Raymond](http://www.catb.org/esr/)’s [The Art of Unix Programming](http://www.catb.org/esr/writings/taoup/)

Feenox is indeed the third version written from scratch after a first implementation in 2009 (different small components with different names) and a second one (named wasora) which was far more complex and had far more features circa 2012--2014. The third attempt, FeenoX, explicitly addresses the “do one thing well” idea from Unix. 

Furthermore, not only is FeenoX itself both [free](https://www.gnu.org/philosophy/free-sw.en.html) and [open-source](https://opensource.com/resources/what-open-source) software but, following the _rule of composition_ (@sec:composition), it also is designed to connect and to work with  other free and open source software such as

 * [Gmsh](http://gmsh.info/) for pre and/or post-processing
 * [ParaView](https://www.paraview.org/) for post-processing
 * [Gnuplot](http://gnuplot.info/) for plotting 1D/2D results
 * [Pyxplot](http://www.pyxplot.org.uk/) for plotting 1D results
 * [Pandoc](https://pandoc.org/) for creating tables and documents
 * [TeX](https://tug.org/) for creating tables and documents
 
and many others, which are readily available in all major GNU/Linux distributions.

FeenoX also makes use of high-quality free and open source mathematical libraries which contain numerical methods designed by mathematicians and implemented by professional programmers. In particular, it depends on

 * [GNU Scientific Library](https://www.gnu.org/software/gsl/) for general mathematics,
 * [SUNDIALS IDA](https://computing.llnl.gov/projects/sundials/ida) for ODEs and DAEs,
 * [PETSc](http://https://petsc.org) for PDEs, and
 * [SLEPc](http://slepc.upv.es/) for PDEs involving eigen problems
 
Therefore, if one zooms in into the block of the transfer function above, FeenoX can also be seen as a [glue layer](https://www.linuxtopia.org/online_books/programming_books/art_of_unix_programming/ch04s03_1.html) between the input files defining a physical problem and the mathematical libraries used to solve the discretized equations. This way, FeenoX bounds its scope to do only one thing and to do it well: to build and solve finite-element formulations of physical problems. And it does so on high grounds, both ethical and technological:

Ethical

:   Since it is [free software](https://www.gnu.org/philosophy/open-source-misses-the-point.en.html), all users can

     0. run,
     1. share,
     2. modify, and/or
     3. re-share their modifications.
      
    If a user cannot read or write code to make FeenoX suit her needs, at least she has the _freedom_ to hire someone to do it for her.

Technological

:   Since it is [open source](http://www.catb.org/~esr/writings/cathedral-bazaar/cathedral-bazaar/), advanced users can detect and correct bugs and even improve the algorithms. [Given enough eyeballs, all bugs are shallow.](https://en.wikipedia.org/wiki/Linus%27s_law)


FeenoX’s main development architecture is [Debian GNU/Linux](https://www.debian.org/) running over 64-bits Intel-compatible processors.
All the dependencies are free and/or open source and already available in Debian’s latest stable official repositories, as explained in @sec:deployment.

The POSIX standard is followed whenever possible, allowing thus FeenoX to be compiled in other operating systems and architectures such as Windows (using [Cygwin](https://www.cygwin.com/)) and MacOS. The build procedure is the well-known and mature `./configure && make` command. 

FeenoX is written in\ [C](https://en.wikipedia.org/wiki/C_(programming_language)) conforming to the [ISO\ C99](https://en.wikipedia.org/wiki/C99) specification (plus POSIX extensions), which is a standard, mature and widely supported language with compilers for a wide variety of architectures. 
For its basic mathematical capabilities, FeenoX uses the [GNU Scientifc Library](https://www.gnu.org/software/gsl/).
For solving ODEs/DAEs, FeenoX relies on [Lawrence Livermore’s SUNDIALS library](https://computing.llnl.gov/projects/sundials/ida). For PDEs, FeenoX uses [Argonne’s PETSc library](https://www.mcs.anl.gov/petsc/) and [Universitat Politècnica de València's SLEPc library](https://slepc.upv.es/). All of them are

 * free and open source,
 * written in C (neither Fortran nor C++),
 * mature and stable,
 * actively developed and updated,
 * very well known both in the industry and academia.

Moreover, PETSc and SLEPc are scalable through the [MPI standard](https://www.mcs.anl.gov/research/projects/mpi/standard.html) (further discussed in @sec:scalability). This means that programs using both these libraries can run on either large high-performance supercomputers or low-end laptops. FeenoX has been run on

 - Raspberry Pi
 - Laptop (GNU/Linux & Windows 10)
 - Macbook
 - Desktop PC
 - Bare-metal servers
 - Vagrant/Virtualbox virtual machines
 - Docker/Kubernetes containers
 - AWS/DigitalOcean/Contabo instances

Due to the way that FeenoX is designed and the policy separated from the mechanism, it is possible to control a running instance remotely from a separate client which can eventually run on a mobile device ([@fig:caeplex-ipad]).

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

 * [GNU C compiler](https://gcc.gnu.org/)
 * [LLVM Clang compiler](http://clang.org/)
 * [Intel C compiler](https://www.intel.com/content/www/us/en/developer/tools/oneapi/dpc-compiler.html)

 
FeenoX depends on the four open source libraries stated in @sec:architecture, although the last three of them are optional. The only mandatory library is the GNU Scientific Library which is part of the GNU/Linux operating system and as such is readily available in all distributions as `libgsl-dev`. The sources of the rest of the optional libraries are also widely available in most common GNU/Linux distributions.

In effect, doing 

```terminal
sudo apt-get install gcc make libgsl-dev libsundials-dev petsc-dev slepc-dev
```

is enough to provision all the dependencies needed compile FeenoX from the source tarball with the full set of features. 
If using the Git repository as a source, then [Git](https://git-scm.com/) itself and the [GNU Autoconf](https://www.gnu.org/software/autoconf/) and [Automake](https://www.gnu.org/software/automake/) packages are also needed:

```terminal
sudo apt-get install git autoconf automake
```

Even though compiling FeenoX from sources is the recommended way to obtain the tool, since the target binary can be compiled using particularly suited compilation options, flags and optimizations (especially those related to MPI, linear algebra kernels and direct and/or iterative sparse solvers), there are also tarballs with usable binaries for some of the most common architectures---including some non-GNU/Linux variants. These binary distributions contain statically-linked executable files that do not need any other shared libraries to be installed on the target host. However, their flexibility and efficiency is generic and far from ideal. Yet the flexibility of having an execution-ready distribution package for users that do not know how to compile C source code outweighs the limited functionality and scalability of the tool.

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
   
If one does not care about the details of the compilation, then a pre-compiled statically-linked binary can be directly downloaded very much as when downloading Gmsh:
 
```terminal
$ wget http://gmsh.info/bin/Linux/gmsh-Linux64.tgz
$ wget https://seamplex.com/feenox/dist/linux/feenox-linux-amd64.tar.gz
```

Appendix @sec:download has more details about how to download and compile FeenoX.
The full documentation contains a [compilation guide](https://seamplex.com/feenox/doc/compilation.html) with further detailed explanations of each of the steps involved.
Since all the commands needed to either download a binary executable or to compile from source with customized optimization flags can be automatized, FeenoX can be built into a container such as Docker. This way, deployment and scalability can be customized and tweaked as needed.


## Execution {#sec:execution}


> ```include
> 220-execution.md
> ```

As FeenoX is designed to run as a file filter---or as a transfer function between input and output files from a more traditional engineering point of view---and it explicitly avoids having a graphical interface, the binary executable works as any other Unix terminal command. When invoked without arguments, it prints its version (a thorough explanation of the versioning scheme is given in @sec:traceability), a one-line description and the usage options:

```terminal
$ feenox
FeenoX v0.2.37-g5b19c32
a free no-fee no-X uniX-like finite-element(ish) computational engineering tool

usage: feenox [options] inputfile [replacement arguments] [petsc options]

  -h, --help         display options and detailed explanations of commmand-line usage
  -v, --version      display brief version information and exit
  -V, --versions     display detailed version information
  --pdes             list the types of PROBLEMs that FeenoX can solve, one per line

Run with --help for further explanations.
```

The program can also be executed remotely either

 a. on a server through a [SSH](https://en.wikipedia.org/wiki/Secure_Shell) session
 b. in a [container](https://en.wikipedia.org/wiki/OS-level_virtualization) as part of a provisioning script


 
FeenoX provides mechanisms to inform its progress by writing certain information to devices or files, which in turn can be monitored remotely or even trigger server actions. Progress can be as simple as an ASCII bar (triggered with `--progress` in the command line or with the keyword `PROGRESS` in the input file) to more complex mechanisms like writing the status in a shared memory segment.

Regarding its execution, there are three ways of solving problems:

 1. direct execution
 2. parametric runs, and
 3. optimization loops.
 
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



## Parametric {#sec:parametric}

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

To have an actual parametric run, an external loop has to successively call FeenoX with the parametric arguments. For example, say this file `cantilever.fee` fixes the face called “left” and sets a load in the negative\ $z$ direction of a mesh called `cantilever-$1-$2.msh`. The output is a single line containing the number of nodes of the mesh and the displacement in the vertical direction\ $w(500,0,0)$ at the center of the cantilever's free face:

```{.feenox include="cantilever.fee"}
```

::: {#fig:cantilever-mesh layout-ncol=2}
![Tetrahedra](cantilever-tet.png){width_latex=49%}

![Hexahedra](cantilever-hex.png){width_latex=49%}

Cantilevered beam meshed with structured tetrahedra and hexahedra
:::

Now the following [Bash](https://en.wikipedia.org/wiki/Bash_(Unix_shell)) script first calls Gmsh to create the meshes `cantilever-${element}-${c}.msh` where

 * `${element}`: tet4, tet10, hex8, hex20, hex27
 * `${c}`: 1,2,\dots,10

It then calls FeenoX with the input above and passes `${element}` and `${c}` as extra arguments, which then are expanded as `$1` and `$2` respectively.
 
```{.bash include="cantilever.sh"}
```

After the execution of the script, thanks to the design decision that output is 100% defined by the user (in this case with the `PRINT` instruction), one has several files `cantilever-${element}.dat` files. When plotted, these show the shear locking effect of fully-integrated first-order elements as illustrated in @fig:cantilever-displacement. The theoretical [Euler-Bernoulli](https://en.wikipedia.org/wiki/Euler%E2%80%93Bernoulli_beam_theory) result is just a reference as, among other things, it does not take into account the effect of the material's [Poisson's ratio](https://en.wikipedia.org/wiki/Poisson%27s_ratio).
Note that the abscissa shows the number of _nodes_, which are proportional to the number of degrees of freedom (i.e. the size of the problem matrix) and not the number of _elements_, which is irrelevant here and in most problems.


![Displacement at the free tip of a cantilevered beam vs. number of nodes for different element types](cantilever-displacement.svg){#fig:cantilever-displacement}


### Optimization loops

Optimization loops work very much like parametric runs from the FeenoX point of view. The difference is mainly on the calling script that has to implement a certain optimization algorithm such as [conjugate gradients](https://en.wikipedia.org/wiki/Conjugate_gradient_method), [Nelder-Mead](https://en.wikipedia.org/wiki/Nelder%E2%80%93Mead_method), [simulated annealing](https://en.wikipedia.org/wiki/Simulated_annealing), [genetic algorithms](https://en.wikipedia.org/wiki/Genetic_algorithm), etc. to choose which parameters to pass to FeenoX as command-line argument.
The only particularity on FeenoX's side is that since the next argument that the optimization loop will pass might depend on the result of the current step, care has to be taken in order to be able to return back to the calling script whatever results it needs in order to compute the next arguments. This is usually just the scalar being optimized for, but it can also include other results such as derivatives or other relevant data.

To illustrate how to use FeenoX in an optimization loop, let us consider the problem of finding the length\ $\ell_1$ of a tuning fork (@fig:fork-meshed) such that the fundamental frequency on a free-free oscillation is equal to the base\ A frequency at\ 440\ Hz.


![What length\ $\ell_1$ is needed so the fork vibrates at 440\ Hz?](fork-meshed.svg){#fig:fork-meshed width=20%}

This extremely simple input file (_rule of simplicity_ @sec:simplicity) solves the free-free mechanical modal problem (i.e. without any Dirichlet boundary condition) and prints the fundamental frequency:

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

Since the computed frequency depends both on the length\ $\ell_1$ and on the mesh refinement level\ $n$, there are actually two nested loops: one parametric over $n=1,2\dots,7$ and the optimization loop itself that tries to find\ $\ell_1$ so as to obtain a frequency equal to 440\ Hz within\ 0.01% of error.

```terminal
$ python fork.py > fork.dat
$
``` 

![Estimated length\ $\ell_1$ needed to get 440\ Hz for different mesh refinement levels\ $n$](fork.svg){#fig:fork width_latex=85%}

Note that the approach used here is to use Gmsh Python API to build the mesh and then fork the FeenoX executable to solve the fork (no pun intended). There are plans to provide a Python API for FeenoX so the problem can be set up, solved and the results read back directly from the script instead of needing to do a fork+exec, read back the standard output as a string and then convert it to a Python `float`.

@Fig:fork shows the results of the combination of the optimization loop over $\ell_1$ and a parametric run over\ $n$. The difference for $n=6$ and $n=7$ is in the order of one hundredth of millimeter.



## Efficiency {#sec:efficiency}

> ```include
> 230-efficiency.md
> ```

One of the most widely known quotations in computer science is that one that says “premature optimization is the root of all evil.” that is an extremely over-simplified version of [Donald E. Knuth’s](https://en.wikipedia.org/wiki/Donald_Knuth) analysis in his [The Art of Computer Programming](https://en.wikipedia.org/wiki/The_Art_of_Computer_Programming). Bottom line is that the programmer should not not spend too much time trying to optimize code based on hunches but based on profiling measurements. Yet a disciplined programmer can tell when an algorithm will be way too inefficient (say something that scales up like $O(n^2)$) and how small changes can improve performance (say by understanding how caching levels work in order to implement faster nested loops). It is also true that usually an improvement in one aspect leads to a deterioration in another one (e.g. a decrease in CPU time by caching intermediate results in an increase of RAM usage).

Even though FeenoX is still evolving so it could be premature in many cases, it is informative to compare running times and memory consumption when solving the same problem with different cloud-friendly FEA programs. In effect, a [serial single-thread single-host comparison of resource usage when solving the NAFEMS LE10 problem](https://seamplex.com/feenox/tests/nafems/le10/) introduced above was performed, using both [unstructured tetrahedral](https://www.seamplex.com/feenox/tests/nafems/le10/report-tet.html) and [structured hexahedral](https://www.seamplex.com/feenox/tests/nafems/le10/report-hex.html) meshes. @Fig:le10-tet shows two figures of the many ones contained in the detailed report. In general, FeenoX using the iterative approach based on PETSc’s Geometric-Algebraic Multigrid Preconditioner  and a conjugate gradients solver is faster for (relatively) large problems at the expense of a larger memory consumption. The curves that use MUMPS confirm the well-known theoretical result that direct linear solvers are robust but not scalable. 

The large memory consumption shown by FeenoX is due to a high level of caching intermediate results. For instance, all the shape functions evaluated at the integration points are computed once when building the stiffness matrix, stored in RAM and then re-used when recovering the gradients of the displacements needed to compute the stresses.  There are also a number of non-premature optimization tasks that can improve both the CPU and memory usage that ought to be performed at later stages of the project.

::: {#fig:le10-tet layout-ncol=2}
![Wall time vs. number of degrees of freedom](wall-dofs-tet.svg){#fig:wall-dofs-tet width_html=100% width_latex=90%}

![Memory vs. number of degrees of freedom](memory-dofs-tet.svg){#fig:memory-dofs-tet width_html=100% width_latex=90%}

Resource consumption when solving the NAFEMS LE10 problem in the cloud for tetrahedral meshes.
:::

Regarding storage, FeenoX needs space to store the input file (negligible), the mesh file in `.msh` format (which can be either ASCII or binary) and the optional output files in `.msh` or `.vtk` formats. All of these files can be stored gzip-compressed and un-compressed on demand by exploiting FeenoX’s script-friendliness using proper calls to `gzip` before and/or after calling the `feenox` binary.
 
## Scalability  {#sec:scalability}
 
> ```include
> 240-scalability.md
> ```

The time needed to solve a relatively large problem can be reduced by exploiting the fact that most cloud servers (and even laptop computers) have more than one CPU available. There are some tasks that can be split into several processors sharing a common memory address space that will scale up almost perfectly, such as building the elemental matrices and assembling the global stiffness matrix. There are some other tasks that might not scale perfectly but that nevertheless might (or might not) reduce the overall wall time if split among processors using a common memory space, such as solving the linear system\ $K \cdot \vec{u} = \vec{b}$. The usual scheme to parallelize a problem under these conditions is to use the [OpenMP](https://en.wikipedia.org/wiki/OpenMP) framework.

Yet, if the problem is large enough, a server might not have enough physical random-access memory to be able to handle the whole problem. The problem now has to be split among different servers which, in turn, might have several processors each. Some of the processors share the same address space but most of them will only have access to a fraction of the whole global problem data. In these cases, there are no tasks that can scale up perfectly since even when building and assembling the matrices, a processor needs some piece of data which is handled by another processor with a different address space and that has to be conveyed specifically from one process to another one. The usual scheme to parallelize a problem under these conditions is to use the [MPI](https://en.wikipedia.org/wiki/Message_Passing_Interface) standard and one of its two most well-known implementations, either [Open\ MPI](https://www.open-mpi.org/) or [MPICH](https://www.mpich.org/).

It might seem that the most effective approach to solve a large problem is to use OpenMP (not to be confused with OpenMPI!) among threads running in processors that share the memory address space and to use MPI among processes running in different hosts. But even though this hybrid OpenMP+MPI scheme is possible, there are at least three main drawbacks with respect to a pure MPI approach:

 i. the overall performance is not be significantly better
 ii. the amount of lines of code that has to be maintained is more than doubled
 iii. the number of possible points of synchronization failure increases

In many ways, the pure MPI mode has fewer synchronizations and thus should perform better.
Hence, FeenoX uses MPI (mainly through PETSc and SLEPc) to handle large parallel problems.

::: {#fig:nafems-le1-metis layout-ncol=2}
![Structured grid](nafems-le1-struct-metis.png){width=49%}

![Unstructured grid](nafems-le1-unstruct-metis.png){width=49%}

Partition of the [2D NAFEMS LE1](https://www.seamplex.com/feenox/examples/#nafems-le1-elliptical-membrane-plane-stress-benchmark) domain into four different sub-domains computed in [Gmsh](http://gmsh.info/) using [Metis](http://glaros.dtc.umn.edu/gkhome/metis/metis/overview).
:::

Most of the overhead of parallelized tasks come from the fact that processes need data stored in other processes (i.e. the so-called ghost points) that use a differet virtual memory address space.
Therefore, the discretized domain has to be split among processes in such a way as to minimize the number of inter-process communication. This problem, called domain decomposition, can be handled either by the mesher or by the solver itself, usually using a third-part library such as [Metis](http://glaros.dtc.umn.edu/gkhome/metis/metis/overview). It should be noted that the domain decomposition problem does not have a unique solution. On the one hand, it depends on the actual mesh being distributed over parallel processes as illustrated in @fig:nafems-le1-metis. On the other hand, the optimal solution might depend on the kind of topology boundaries to minimize (shared nodes, shared faces) and other subtle parameters and options that partitioning libraries allow.

FeenoX relies on [Gmsh](http://gmsh.info/) to perform the domain decomposition (using Metis at mesh-time) and to provide the partitioning information in the mesh file read by the `READ_MESH` keyword.


## Flexibility {#sec:flexibility}
 
> ```include
> 250-flexibility.md
> ```

The third-system effect mentioned in @sec:architecture involves more than ten years of experience in the nuclear industry,^[This experience also shaped many of the features that FeenoX has and most of the features is does deliberately not have.] where complex dependencies of multiple material properties over space through intermediate distributions (temperature, neutronic poisons, etc.) and time (control rod positions, fuel burn-up, etc.) are mandatory.
One of the cornerstone design decisions in FeenoX is that **everything is an expression** (@sec:expression). Here, “everything” means any location in the input file where a numerical value is expected. The most common use case is in the `PRINT` keyword. For example, the [Sophomore's dream](https://en.wikipedia.org/wiki/Sophomore%27s_dream) (in contrast to [Freshman's dream](https://en.wikipedia.org/wiki/Freshman%27s_dream)) identity


$$
\int_{0}^{1} x^{-x} \, dx = \sum_{n=1}^{\infty} n^{-n}
$$

can be illustrated like this:

```{.feenox include="sophomore.fee"}
```

```terminal
$ feenox sophomore.fee
1.2912861
1.2912860
$
```

Of course most engineering problems will not need explicit integrals---although a few of them do---but some might need summation loops, so it is handy to have these functionals available inside the FEA tool. This might seem to go against the “keep it simple” and “do one thing good” Unix principle, but definitely follows [Alan Kay](https://en.wikipedia.org/wiki/Alan_Kay)’s idea that “simple things should be simple, complex things should be possible” (further discussion in\ @sec:complex).

Flexibility in defining non-trivial material properties is illustrated with the following example, where two squares made of different dimensionless materials are juxtaposed in thermal contact (glued?) and subject to different boundary conditions at each of the four sides (@fig:two-squares-mesh).

![Two non-dimensional $1 \times 1$ squares each in thermal contact made of different materials.](two-squares-mesh.svg){#fig:two-squares-mesh width_latex=75% width_html=100%}

The yellow square is made of a certain material with a conductivity that depends algebraically (and fictitiously) the temperature like

$$
k_\text{yellow}(x,y) = \frac{1}{2} + T(x,y)
$$

The cyan square has a space-dependent temperature given by a table of scattered data as a function of the spatial coordinates\ $x$ and\ $y$ (origin is left bottom corner of the yellow square) without any particular structure on the definition points:

| $x$ | $y$ | $k_\text{cyan}(x,y)$ |
|:---:|:---:|:---------------:|
|  1  |  0  |      1.0        |
|  1  |  1  |      1.5        |
|  2  |  0  |      1.3        |
|  2  |  1  |      1.8        |
| 1.5 | 0.5 |      1.7        |

The cyan square generates a temperature-dependent power density (per unit area) given by

$$
q^{\prime \prime}_\text{cyan}(x,y) = 0.2 \cdot T(x,y)
$$

The yellow one does not generate any power so $q^{\prime \prime}_\text{yellow} = 0$. Boundary conditions are

$$
\begin{cases}
T(x,y) = y & \text{at the left edge $y=0$} \\
T(x,y) = 1-\cos\left(\frac{1}{2}\pi \cdot x\right) & \text{at the bottom edge $x=0$} \\
q'(x,y) = 2-y & \text{at the right edge $x=2$} \\
q'(x,y) = 1 & \text{at the top edge $y=1$} \\
\end{cases}
$$

The input file illustrate how flexible FeenoX is and, again, how the problem definition in a format that the computer can understand resembles the humanly-written formulation of the original engineering problem:

```{.feenox include="two-squares.fee"}
```

Note that FeenoX is flexible enough to...

 1. handle mixed meshes (the yellow square is meshed with triangles and the other one with quadrangles) 
 2. use point-wise defined properties even though there is not underlying structure nor topology for the points where the data is defined (FeenoX could have read data from a `.msh` or `.vtk` file respecting the underlying topology)
 3. understand that the problem is non-linear so as to use PETSc’s SNES framework automatically (the conductivity and power source depend on the temperature).

 
::: {#fig:two-squares-results layout-ncol=2}
![Temperature defined at nodes](two-squares-temperature.png){width_latex=75%  width_html=100%}

![Conductivity defined at cells](two-squares-conductivity.png){width_latex=75%  width_html=100%}

Temperature (main result) and conductivity for the two-squares thermal problem.
:::

In the very same sense that variables `x`, `y` and `z` appearing in the input refer to the spatial coordinates\ $x$, $y$ and\ $z$ respectively, the special variable `t` refers to the time\ $t$. The requirement of allowing time-dependent boundary conditions can be illustrated by solving the NAFEMS\ T3 one-dimensional transient heat transfer benchmark. It consists of a slab of\ 0.1\ meters long subject to a fixed homogeneous temperature on one side, i.e.

$$T(x=0)=0~\text{°C}$$

and to a transient temperature

$$
T(x=0.1~\text{m},t)=100~\text{°C} \cdot \sin\left( \frac{\pi \cdot t}{40~\text{s}}\right)
$$

at the other side. There is zero internal heat generation, at $t=0$ all temperature is equal to 0°C (sic) and conductivity, specific heat and density are constant and uniform. The problem asks for the temperature at location\ $x=0.08~\text{m}$ at time\ $t=32~\text{s}$. The reference result is $T(0.08~\text{m},32~\text{s})=36.60~\text{°C}$.

```{.feenox include="nafems-t3.fee"}
```

```terminal
$ gmsh -1 slab-0.1m.geo 
[...]
Info    : Done meshing 1D (Wall 0.000213023s, CPU 0.000836s)
Info    : 61 nodes 62 elements
Info    : Writing 'slab-0.1m.msh'...
Info    : Done writing 'slab-0.1m.msh'
Info    : Stopped on Sun Dec 12 19:41:18 2021 (From start: Wall 0.00293443s, CPU 0.02605s)
$ feenox nafems-t3.fee 
T(0.08m,32s) = 	36.5996	ºC
$ pyxplot nafems-t3.ppl
$
```

![Temperature vs. time at three axial locations for the NAFEMS\ T3 benchmark](nafems-t3.svg){#fig:nafems-t3 width=100%}


Besides “everything is an expression,” FeenoX follows another cornerstone rule: **simple problems ought to have simple inputs**, akin to Unix’ _rule of simplicity_---that addresses the first half of Alan Kay’s quote above. This rule is further discussed in @sec:input.


## Extensibility {#sec:extensibility}


> ```include
> 260-extensibility.md
> ```

Even though FeenoX is written in\ [C](https://en.wikipedia.org/wiki/C_(programming_language)), it makes extensive use of [function pointers](https://en.wikipedia.org/wiki/Function_pointer) to mimic [C++](https://en.wikipedia.org/wiki/C%2B%2B)’s [virtual methods](https://en.wikipedia.org/wiki/Virtual_function). This way, depending on the problem type given with the [`PROBLEM`](https://www.seamplex.com/feenox/doc/feenox-manual.html#problem) keyword, particular routines are called to

 1. initialize and set up solver options (steady-state/transient, linear/non-linear, regular/eigenproblem, etc.)
 2. parse boundary conditions given in the `BC` keyword
 3. build elemental contributions for
     a. volumetric stiffness and/or mass matrices
     b. natural boundary conditions
 4. compute secondary fields (heat fluxes, strains and stresses, etc.) out of the gradients of the primary fields
 5. compute per-problem key performance indicators (min/max temperature, displacement, stress, etc.)
 6. write particular post-processing outputs
 
Indeed, each of the supported problems, namely

```include
pdes.md
```

is a separate directory under [`src/pdes`](https://github.com/seamplex/feenox/tree/main/src/pdes) that implements these “virtual” methods (recall that they are function pointers) that are resolved at runtime when parsing the main input file.

FeenoX was designed with separated common "mathematical" routines from the particular "physical" ones in such a way that any of these directories can be removed and the code would still compile. For example, let us remove the directory `src/pdes/thermal` from a temporary clone of the main Git repository. The `autogen.sh` script is smart enough to 

 i. tell which types of PDEs are available by reading the subdirectories in `src/pdes`, and
 ii. link the available virtual methods to the appropriate function pointers which are resolved at runtime.
 
In effect,

```terminal
~$ cd tmp/
~/tmp$ git clone https://github.com/seamplex/feenox
Cloning into 'feenox'...
remote: Enumerating objects: 6908, done.
remote: Counting objects: 100% (4399/4399), done.
remote: Compressing objects: 100% (3208/3208), done.
remote: Total 6908 (delta 3085), reused 2403 (delta 1126), pack-reused 2509
Receiving objects: 100% (6908/6908), 10.94 MiB | 6.14 MiB/s, done.
Resolving deltas: 100% (4904/4904), done.
~/tmp$ cd feenox
~/tmp/feenox$ rm -rf src/pdes/thermal/
~/tmp/feenox$ ./autogen.sh 
creating Makefile.am... ok
creating src/Makefile.am... ok
calling autoreconf... 
configure.ac:18: installing './compile'
configure.ac:15: installing './config.guess'
configure.ac:15: installing './config.sub'
configure.ac:17: installing './install-sh'
configure.ac:17: installing './missing'
parallel-tests: installing './test-driver'
src/Makefile.am: installing './depcomp'
done
~/tmp/feenox$ ./configure.sh 
[...]
configure: creating ./config.status
config.status: creating Makefile
config.status: creating src/Makefile
config.status: creating doc/Makefile
config.status: executing depfiles commands
~/tmp/feenox$ make
[...]
make[1]: Leaving directory '/home/gtheler/tmp/feenox'
~/tmp/feenox$
```

Now if one wants to run the thermal problem with the two juxtaposed squares from\ @sec:flexibility above, the "temporary" FeenoX will complain. But it will still solve the [NAFEMS\ LE10 problem](https://www.seamplex.com/feenox/examples/#nafems-le10-thick-plate-pressure-benchmark) problem right away:

```terminal
~/tmp/feenox$ cd doc/
~/tmp/feenox/doc$ ../feenox two-squares.fee 
error: two-squares.fee: 1: unknown problem type 'thermal'
~/tmp/feenox/doc$ cd ../examples
~/tmp/feenox/examples$ ../feenox nafems-le10.fee 
sigma_y @ D =   -5.38367        MPa
~/tmp/feenox/examples$
```

The list of available PDEs that a certain FeenoX binary has can be found by using the `--pdes` option. They are sorted alphabetically, one type per line:

```terminal
~/tmp/feenox/examples$ feenox --pdes
laplace
mechanical
modal
neutron_diffusion
~/tmp/feenox/examples$
```


Besides removals, additions---which are also handled by `autogen.sh` as describe above--- are far more interesting to discuss.
Additional elliptic problems can be added by using the `laplace` directory as a template while using the other directories as examples about how to add further features (e.g. a Robin-type boundary condition in `thermal` and a vector-valued unknown in `mechanical`). More information can be found in the [FeenoX programming & contributing](https://www.seamplex.com/feenox/doc/#programming-and-contributing) section.

As already discussed in @sec:introduction, FeenoX is [free-as-in-freedom](https://en.wikipedia.org/wiki/Free_as_in_Freedom) software licensed under the terms of the [GNU General Public License](https://www.gnu.org/licenses/gpl-3.0) version\ 3 or, at the user convenience, any later version.
In the particular case of additions to the code base, this fact has two implications.

 i. Every person in the world is _free_ to modify FeenoX to suit their needs, including adding a new problem type either by
 
     a. using one of the existing ones as a template, or
     b. creating a new directory from scratch
     
    without asking anybody for any kind of permission. In case this person does not how to program, he or she has the _freedom_ to hire somebody else to do it. It is this the sense of the word “free” in the compound phrase “free software:” freedom to do what they think fit (except to make it non-free, see next bullet).
 
 ii. People adding code own the copyright of the additional code. Yet, if they want to distribute the modified version they have to do it also under the terms of the GPLv3+ and under a name that does not induce the users to think the modified version is the original FeenoX distribution.^[Even better, these authors should ask to merge their contributions into FeenoX’s main code base.] That is to say, free software ought to remain free---a.k.a. as [copyleft](https://en.wikipedia.org/wiki/Copyleft).
 

Regarding additional material models, the virtual methods that compute the elemental contributions to the stiffness matrix also use function pointers to different material models (linear isotropic elastic, orthotropic elastic, etc.) and behaviors (isotropic thermal expansion, orthotropic thermal expansion, etc.) that are resolved at run time. Following the same principle, new models might be added by adding new routines and resolving them depending on the user’s input.

 
## Interoperability {#sec:interoperability}
 
> ```include
> 270-interoperatibility.md
> ```

@Sec:scope already introduced the ideas about interoperability behind the Unix philosophy which make up for most the the FeenoX design basis. Essentially, they sum up to “do only one thing but do it well.” Since FeenoX is  filter (or a transfer-function), interoperability is a must. So far, this SDS has already shown examples of exchanging information with:

 * [Kate](https://kate-editor.org/) (with syntax highlighting): @fig:nafems-le10-problem-input
 * [Gmsh](http://gmsh.info/) (both as a mesher and a post-processor): [@fig:maze123;@fig:mazes;@fig:cantilever-mesh;@fig:fork-meshed;@fig:nafems-le1-metis;@fig:two-squares-mesh;@fig:two-squares-results]
 * [Paraview](https://www.paraview.org/): @fig:nafems-le10-sigmay
 * [Gnuplot](http://gnuplot.info/): [@fig:lorenz-gnuplot;@fig:le10-tet]
 * [Pyxplot](http://www.pyxplot.org.uk/): [@fig:cantilever-displacement;@fig:fork;@fig:nafems-t3]

 
To illustrate both the filter approach, consider the following input file that solves Laplace’s equation $\nabla^2 \phi = 0$ on a square with some space-dependent boundary conditions. Either Gmsh or Paraview can be used to post-process the results:
 
```include
laplace.md
```

A great deal of FeenoX interoperability capabilities comes from another design decision: **output is 100% controlled by the user** (further discussed in @sec:output), a.k.a. “no `PRINT`, no OUTPUT” whose corollary is the UNIX _rule of silence_ (@sec:silence).
The following input file computes the natural frequencies of oscillation of a cantilevered wire both using the Euler-Bernoulli theory and finite elements. It writes a [GFM table](https://github.github.com/gfm/#tables-extension-) into the standard output which is then piped to [Pandoc](https://pandoc.org/) and then converted to HTML:

```{.feenox include="wire.fee"}
```

```terminal
$ gmsh -3 wire-hex.geo 
[...]
$ $ feenox wire.fee | pandoc  
<table>
<caption>Comparison of analytical and numerical frequencies, in Hz</caption>
<thead>
<tr class="header">
<th style="text-align: center;"><span class="math inline"><em>n</em></span></th>
<th style="text-align: center;">FEM</th>
<th style="text-align: center;">Euler</th>
<th style="text-align: center;">Relative difference [%]</th>
</tr>
</thead>
<tbody>
<tr class="odd">
<td style="text-align: center;">1</td>
<td style="text-align: center;">45.84</td>
<td style="text-align: center;">45.84</td>
<td style="text-align: center;">0.02</td>
</tr>
<tr class="even">
<td style="text-align: center;">2</td>
<td style="text-align: center;">287.1</td>
<td style="text-align: center;">287.3</td>
<td style="text-align: center;">0.06</td>
</tr>
<tr class="odd">
<td style="text-align: center;">3</td>
<td style="text-align: center;">803.4</td>
<td style="text-align: center;">804.5</td>
<td style="text-align: center;">0.13</td>
</tr>
<tr class="even">
<td style="text-align: center;">4</td>
<td style="text-align: center;">1573</td>
<td style="text-align: center;">1576</td>
<td style="text-align: center;">0.24</td>
</tr>
<tr class="odd">
<td style="text-align: center;">5</td>
<td style="text-align: center;">2596</td>
<td style="text-align: center;">2606</td>
<td style="text-align: center;">0.38</td>
</tr>
</tbody>
</table>
$
```

Of course these kind of FeenoX-generated tables can be inserted verbatim into Markdown documents (just like this one) and rendered as @tbl:wire-freq.

  $n$ |   FEM  | Euler | Relative difference [%]
:----:+:------:+:-----:+:-----------------------:
1 | 45.84 | 45.84 | 0.02
2 | 287.1 | 287.3 | 0.06
3 | 803.4 | 804.5 | 0.13
4 | 1573 | 1576 | 0.24
5 | 2596 | 2606 | 0.38

: Comparison of analytical and numerical frequencies, in Hz {#tbl:wire-freq}



::: {#fig:latex-tables layout-ncol=1}
![A multi-billion-dollar agency using the Windows philosophy (presumably mouse-based copy and paste into Word)](nureg.png)

![A small third-world consulting company using the Unix philosophy (FeenoX+AWK+LaTeX)](cne.png){#fig:cne}

Results of the same fatigue problem solved using two different philosophies.
:::

 
It should be noted that all of the programs and tools mentioned to be interoperable with FeenoX are [free and open source software](https://en.wikipedia.org/wiki/Free_and_open-source_software). This is not a requirement from the SRS, but is indeed a nice-to-have feature.
 
# Interfaces

 
> ```include
> 300-interfaces.md
> ```

FeenoX is provided as a console-only executable which can be run remotely through SSH or inside a containerized environment without any requirement such as graphical servers or special input devices.
When executed without any arguments, FeenoX writes a brief message with the version (further discussed in\ @sec:traceability) and the basic usage on the standard output and return to the calling shell with a return errorlevel zero:

```terminal
$ feenox 
FeenoX v0.2.12-gc5934bb
a free no-fee no-X uniX-like finite-element(ish) computational engineering tool

usage: feenox [options] inputfile [replacement arguments]

  -h, --help         display options and detailed explanations of commmand-line usage
  -v, --version      display brief version information and exit
  -V, --versions     display detailed version information

Run with --help for further explanations.
$ echo $?
0
$ 
```

The `--version` option follows the [GNU Coding Standards guidelines](https://www.gnu.org/prep/standards/standards.html#g_t_002d_002dversion):

```terminal
$ feenox --version
FeenoX v0.2.14-gbbf48c9
a free no-fee no-X uniX-like finite-element(ish) computational engineering tool

Copyright © 2009--2022 Seamplex, https://seamplex.com/feenox
GNU General Public License v3+, https://www.gnu.org/licenses/gpl.html. 
FeenoX is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.
$
```

The `--versions` option shows more information about the FeenoX build and the libraries the binary was linked against:

```terminal
$ feenox -V
FeenoX v0.2.14-gbbf48c9
a free no-fee no-X uniX-like finite-element(ish) computational engineering tool

Last commit date   : Sat Feb 12 15:35:05 2022 -0300
Build date         : Sat Feb 12 15:35:44 2022 -0300
Build architecture : linux-gnu x86_64
Compiler version   : gcc (Debian 10.2.1-6) 10.2.1 20210110
Compiler expansion : gcc -Wl,-z,relro -I/usr/include/x86_64-linux-gnu/mpich -L/usr/lib/x86_64-linux-gnu -lmpich
Compiler flags     : -O3
Builder            : gtheler@tom
GSL version        : 2.6
SUNDIALS version   : 5.7.0
PETSc version      : Petsc Release Version 3.16.3, Jan 05, 2022 
PETSc arch         : arch-linux-c-debug
PETSc options      : --download-eigen --download-hdf5 --download-hypre --download-metis --download-mumps --download-parmetis --download-pragmatic --download-scalapack
SLEPc version      : SLEPc Release Version 3.16.1, Nov 17, 2021
$
```

The `--help` option gives a more detailed usage:

```terminal
$ feenox --help
usage: feenox [options] inputfile [replacement arguments] [petsc options]

  -h, --help         display options and detailed explanations of commmand-line usage
  -v, --version      display brief version information and exit
  -V, --versions     display detailed version information

  --progress         print ASCII progress bars when solving PDEs
  --mumps            ask PETSc to use the direct linear solver MUMPS
  --linear           force FeenoX to solve the PDE problem as linear
  --non-linear       force FeenoX to solve the PDE problem as non-linear

Instructions will be read from standard input if “-” is passed as
inputfile, i.e.

    $ echo 'PRINT 2+2' | feenox -
    4

The optional [replacement arguments] part of the command line mean that
each argument after the input file that does not start with an hyphen
will be expanded verbatim in the input file in each occurrence of $1,
$2, etc. For example

    $ echo 'PRINT $1+$2' | feenox - 3 4
    7

PETSc and SLEPc options can be passed in [petsc options] as well, with
the difference that two hyphens have to be used instead of only once.
For example, to pass the PETSc option -ksp_view the actual FeenoX
invocation should be

    $ feenox input.fee --ksp_view

See https://www.seamplex.com/feenox/examples for annotated examples.

Report bugs at https://github.com/seamplex/feenox/issues
Ask questions at https://github.com/seamplex/feenox/discussions
Feenox home page: https://www.seamplex.com/feenox/
$
```


The input file provided as the first argument to the `feenox` binary contains all the information needed to solve the problem, so any further human intervention is not needed after execution begins. 
If the execution finishes successfully, FeenoX returns a zero errorlevel to the calling shell (and follows the UNIX _rule of silence_):

```terminal
$ feenox maze.fee
$ echo $?
0
$
```

If there is problem during execution (including parsing and run-time errors), a line prefixed with `error:` is written into the standard error file descriptor and a non-zero errorlevel is returned:

```terminal
$ feenox hello.fee 
error: input file needs at least one more argument in commandline
$ echo $?
1
$ feenox hello.fee world
Hello world!
$ echo $?
0
$ 
```

This way, the error line can easily be parsed with standard UNIX tools like `grep` and `cut` or with a proper regular expression parser. Eventually, any error should be forwarded back to the initiating entity---which depending on the workflow can be a human or an automation script---in order for he/she/it to fix it.

Following the _rule of repair_ (@sec:repair), ill input files with missing material properties or inconsistent boundary conditions are detected before the actual assembly of the matrix begins:

```terminal
$ feenox thermal-1d-dirichlet-no-k.fee
error: undefined thermal conductivity 'k'
$ feenox thermal-1d-dirichlet-wrong-bc.fee
error: boundary condition 'xxx' does not have a physical group in mesh file 'slab.msh'
$ 
```

Error code are designed to be useful and helpful. An attempt to open a file might fail due to a wide variety of reasons. FeenoX clearly states which one caused the error so it can be remedied:

```terminal
$ cat test.fee 
READ_MESH cantilever.msh
$ feenox test.fee 
$ chmod -r cantilever.msh 
$ feenox test.fee 
error: 'Permission denied' when opening file 'cantilever.msh' with mode 'r'
$ rm cantilever.msh 
$ feenox test.fee 
error: 'No such file or directory' when opening file 'cantilever.msh' with mode 'r'
$ 
```


If the command-line option `--progress` (or the `PROGRESS` keyword in [`PROBLEM`](https://www.seamplex.com/feenox/doc/feenox-manual.html#problem)) is used, then FeenoX writes into the standard output three “bars” showing the progress of

 1. (`.`) the build and assembly of the problem matrices (stiffness and mass if applicable)
 2. (`-`) the iterative solution of the problem (either linear or non-linear)
 3. (`=`) the recovery of gradient-based (i.e. strains and stresses) out of the primary solution
 
::: {.not-in-format .html}
```terminal
$ gmsh -3 nafems-le10.geo
Info    : Running 'gmsh -3 nafems-le10.geo' [Gmsh 4.9.4-git-10d6a15fd, 1 node, max. 1 thread]
Info    : Started on Sat Feb  5 11:26:39 2022
Info    : Reading 'nafems-le10.geo'...
Info    : Reading 'nafems-le10.step'...
Info    :  - Label 'Shapes/Open CASCADE STEP translator 7.6 1' (3D)
Info    : Done reading 'nafems-le10.step'
Info    : Done reading 'nafems-le10.geo'
Info    : Meshing 1D...
[...]
Info    : Done optimizing mesh (0.106654 s)
Info    : Done optimizing high-order mesh (0.106654 s)
Info    : Done optimizing mesh (Wall 0.114461s, CPU 0.114465s)
Info    : 50580 nodes 40278 elements
Info    : Writing 'nafems-le10.msh'...
Info    : Done writing 'nafems-le10.msh'
Info    : Stopped on Sat Feb  5 11:26:40 2022 (From start: Wall 1.08693s, CPU 1.1709s)
$ feenox nafems-le10.fee --progress
....................................................................................................
----------------------------------------------------------------------------------------------------
====================================================================================================
sigma_y @ D =   -5.38228        MPa
$ 
```
:::

:::: {.only-in-format .html }
```{=html}
<div id="feenox-progress"></div>
<script>AsciinemaPlayer.create('feenox-progress.cast', document.getElementById('feenox-progress'), {cols:128,rows:32, poster: 'npt:0:1'});</script>
```
::::

Once again, these ASCII-based progress bars can be parsed by the calling entity and then present it back to the user. For example,\ @fig:caeplex-progress shows how the web-based GUI CAEplex shows progress inside an Onshape tab.


![ASCII progress bars parsed and converted into a web-based interface](caeplex-progress.png){#fig:caeplex-progress width_latex=65% width_html=100%}

Since FeenoX uses PETSc (and SLEPc), command-line options can be passed from FeenoX to PETSc. The only difference is that since FeenoX follows the POSIX standard regarding options and PETSc does not, double dashes are required instead of PETSc' single-dash approach. That is to say, instead of `-ksp_monitor` one would have to pass `--ksp_monitor` (see @sec:simple for details about the input files):

```terminal
$ feenox thermal-1d-dirichlet-uniform-k.fee --ksp_monitor
  0 KSP Residual norm 1.913149816332e+00 
  1 KSP Residual norm 2.897817223901e-02 
  2 KSP Residual norm 3.059845525572e-03 
  3 KSP Residual norm 1.943995979588e-04 
  4 KSP Residual norm 7.418444674938e-06 
  5 KSP Residual norm 1.233527903582e-07 
0.5
$
```

Any PETSc command-line option takes precedence over the settings in the input file, so the preconditioner can be changed even if explicitly given with the [`PRECONDITIONER`](https://www.seamplex.com/feenox/doc/feenox-manual.html#problem) keyword:

```terminal
$ feenox thermal-1d-dirichlet-uniform-k.fee --ksp_monitor --pc_type ilu
  0 KSP Residual norm 1.962141687033e+00 
  1 KSP Residual norm 5.362273771017e-16 
0.5
$
```

If PETSc is compiled with MUMPS, FeenoX provides a `--mumps` option:

```terminal
$ feenox thermal-1d-dirichlet-uniform-k.fee --ksp_monitor --mumps
  0 KSP Residual norm 1.004987562109e+01 
  1 KSP Residual norm 4.699798436762e-15 
0.5
$
```

An illustration of the usage of PETSc arguments and the fact that FeenoX automatically detects whether a problem is linear or not is given below. The case `thermal-1d-dirichlet-uniform-k.fee` is linear while the `two-squares.fee` from section\ @sec:flexibility is not. Therefore, an SNES monitor should give output for the latter but not for the former. In effect:

```terminal
$ feenox thermal-1d-dirichlet-uniform-k.fee --snes_monitor
0.5
$ feenox two-squares.fee --snes_monitor
  0 SNES Function norm 9.658033489479e+00 
  1 SNES Function norm 1.616559951959e+00 
  2 SNES Function norm 1.879821597500e-01 
  3 SNES Function norm 2.972104258103e-02 
  4 SNES Function norm 2.624028350822e-03 
  5 SNES Function norm 1.823396478825e-04 
  6 SNES Function norm 2.574514225532e-05 
  7 SNES Function norm 2.511975376809e-06 
  8 SNES Function norm 4.230090605033e-07 
  9 SNES Function norm 5.154440365087e-08 
$
```

As already explained in @sec:parametric, FeenoX supports run-time replacement arguments that get replaced verbatim in the input file. This feature is handy when the same problem has to be solved over different meshes, such as when investigating the $h$-convergence order over Gmsh's element scale factor `-clscale`:

```{.feenox include="thermal-1d-dirichlet-temperature-k-parametric.fee"}
```

```terminal
$ for c in $(feenox steps.fee); do gmsh -v 0 -1 slab.geo -clscale ${c} -o slab-${c}.msh; feenox thermal-1d-dirichlet-temperature-k-parametric.fee ${c}; done  | sort -g
11      +6.50e-07
13      +3.15e-07
14      +2.29e-07
15      +1.70e-07
17      +1.00e-07
20      +5.04e-08
24      +2.34e-08
32      +7.19e-09
39      +3.46e-09
49      +1.31e-09
$
```

Since the main input file is the first argument (not counting POSIX options starting with at least one dash), FeenoX might be invoked indirectly by adding a shebang line to the input file with the location of the system-wide executable and setting execution permissions on the input file itself. So if we modify the above `hello.fee` example as `hello`

```{.feenox include="hello"}
```

and then we can do

```terminal
$ chmod +x hello
$ ./hello world
Hello world!
$ ./hello universe
Hello universe!
$
```

For example, the following she-banged input file can be used to compute the derivative of a column with respect to the other as a UNIX filter:

```{.feenox include="derivative.fee"}
```

```terminal
$ feenox f.fee "sin(t)" 1 | ./derivative.fee 
0.05    0.998725
0.15    0.989041
0.25    0.968288
0.35    0.939643
0.45    0.900427
0.55    0.852504
0.65    0.796311
0.75    0.731216
0.85    0.66018
0.95    0.574296
$
```



## Problem input {#sec:input}

> ```include
> 310-input.md
> ```

FeenoX currently works by reading an input file (which in turn can recursively include further input files) with an _ad-hoc_ format, whose rationale is described in this section. Therefore, it does satisfy requirement\ a. but eventually could also satisfy requirement\ b.

As already explained in\ @sec:introduction, the motto is “FeenoX is---in a certain sense---to desktop FEA programs  and libraries what Markdown is to Word and (La)TeX, respectively and _deliberately_.” Hence, the input files act as the Markdown source: instructions about what to do but not how to do it.
The input files are indeed plain-text ASCII files with English-like keywords that fully define the problem.
The main features of the input format, thoroughly described below, are:

 #. It is [syntactically sugared](https://en.wikipedia.org/wiki/Syntactic_sugar) by using English-like keywords.
 #. Nouns are definitions and verbs are instructions.
 #. Simple problems need simple inputs.
 #. Simple things should be simple, complex things should be possible. 
 #. Whenever a numerical value is needed an expression can be given (i.e. “everything is an expression.”)
 #. The input file should match as much as possible the paper (or blackboard) formulation of the problem.
 #. It provides means to compare numerical solutions against analytical ones.
 #. It should be possible to read run-time arguments from the command line.
 #. Input files are [distributed version control](https://en.wikipedia.org/wiki/Distributed_version_control)-friendly.
 
### Syntactic sugar & highlighting

The first argument not starting with a dash to the `feenox` executable is the path to the main input file. This main input file can in turn include other FeenoX input files and/or read data from other files (such as meshes) or resources (such as shared memory objects). The input files are plain text files, either pure ASCII or UTF-8 (more details in @sec:git-friendliness). The extension can be anything, but a particular one is recommended so that per-extension syntax highlighting can be enabled in text editors (both graphical such as [Kate](https://kate-editor.org/) and cloud-friendly such as [Vim](https://www.vim.org/) as illustrated in @fig:highlighting) and in documentation (e.g. both HTML and PDF using Pandoc/LaTeX as in the [FeenoX website](https://www.seamplex.com/feenox)).

In principle any extension (even no extension) can be used for the FeenoX input files.
Throughout the FeenoX repository and documentation the extension `.fee` is used, which has a couple of advantages:

 1. The `.fee` extension is detected by syntax-highlighting extensions as shown in @fig:highlighting.
 2. The expression `$0` (or `${0}`) is expanded to the basename of the input file, i.e. the directory part (if present) is removed  and the `.fee` extension is removed. Therefore, 
 
    ```feenox
    WRITE_MESH ${0}.vtk VECTOR u v w  sigma
    ```
    
    would print the displacement field as a vector and the von\ Mises stress as a scalar in a file named `whatever-the-input-file-name-is.vtk`.

Still, the extension is not enforced.
For instance, the [test directory](https://github.com/seamplex/feenox/tree/main/tests) includes some [spinning-disk cases](https://github.com/seamplex/feenox/blob/main/tests/spinning-disk-parallel-solid-half.fee) that compare the analytical solution for the hoop and radial stresses with the numerical ones obtained with FeenoX.
These cases read the radius\ $R$ and thickness\ $t$ from the `.geo` file used by Gmsh to build the mesh in the first place:

```feenox
# analytical solution
INCLUDE spinning-disk-dimensions.geo
S_h(r) = ((3+nu)*R^2 - (1+3*nu)*r^2)
S_r(r) = (3+nu) * (R^2 - r^2)
```

where `spinning-disk-dimensions.geo` is

```c
R = 0.1;
t = 0.003;
```


::: {#fig:highlighting layout-ncol=2}
![Kate](highlighting-kate.png){width=49%}

![Vim](highlighting-vim.png){width=49%}

Syntax highlighitng of input files in GUI and cloud-friendly text editors
:::

The ultimate goal of FeenoX is to solve mathematical equations that are hard to solve with pencil and paper. In particular, to integrate differential equations (recall that the first usable computer was named ENIAC, which stands for Electronic Numerical Integrator and Computer). The input file format was designed as to how to ask the _computer_ what to _compute_. The syntax, based on keywords and alphanumerical arguments was chosen as to sit in the middle of the purely binary numerical system employed by digital computers^[Analog and quantum computers are out of the scope.] and the purely linguistical nature of human communication. The rationale behind its design is that an average user can peek a FeenoX input file and tell what it is asking the computer to compute, as already illustrated for the [NAFEMS\ LE10 problem](https://www.seamplex.com/feenox/examples/#nafems-le10-thick-plate-pressure-benchmark) in @fig:nafems-le10-problem-input. Even if the input files are created by a computer and not by a human, the code used to create a human-friendly input file will be human-friendlier than a code that writes only zeroes and ones as its output (that will become the input of another one following the Unix _rule of composition_ @sec:composition).
As an exercise, compare the input file in @fig:nafems-le10-problem-input (or in @fig:highlighting) with the inputs files used by other open source FEA solvers shown in appendix @sec:le10-other.



### Definitions and instructions {#sec:nouns_verbs}

The way to tell the computer what problem it has to solve and how to solve it is by using keywords in the input file.
Each non-commented line of the input file should start with either

 i. a primary keyword such as [`PROBLEM`](https://www.seamplex.com/feenox/doc/feenox-manual.html#problem) or [`READ_MESH`](https://www.seamplex.com/feenox/doc/feenox-manual.html#read_mesh), or
 ii. a variable such as [`end_time`](https://www.seamplex.com/feenox/doc/feenox-manual.html#end_time) or a vector or matrix with the corresponding index(es) such as `v[2]` or `A[i][j]` followed by the `=` keyword, or
 iii. a function name with its arguments such as `f(x,y)` followed by the `=` keyword.
 
A primary keyword usually is followed by arguments and/or secondary keywords, which in turn can take arguments as well. For example, in

```feenox
PROBLEM mechanical DIMENSIONS 3
READ_MESH nafems-le10.msh 
[...]
# print the direct stress y at D (and nothing more)
PRINT "σ_y @ D = " sigmay(2000,0,300) "MPa"
```

we have `PROBLEM` acting as a primary keyword, taking `mechanical` as its first argument and then `DIMENSIONS` as a secondary keyword with `3` being an argument to the secondary keyword. Then `READ_MESH` is another primary keyword  taking `nafems-le10.msh` as its argument. 

A primary keyword can be

 1. a definition,
 2. an instruction, or
 3. both.
 
Definitions are English _nouns_ and instructions are English _verbs_. 
In the example above, `PROBLEM` is a definition because it tells FeenoX about something it has to do (i.e. that it has to solve a three-dimensional problem), but does not do anything actually. On the other hand, `READ_MESH` is both a definition and an instruction: it defines that there exists a mesh named `nafems-le10.msh` which might be referenced later (for example in an `INTEGRATE` or `WRITE_MESH` instructions), but it also asks FeenoX to read the mesh at that point of the instruction list (more details below).  Finally, `PRINT` is a primary keyword taking different types and number or arguments. It is an instruction because it does not define anything, it just asks FeenoX to print the value of the function named `sigmay` evaluated at\ $2000,0,300$. In this case, `sigmay` is a function which is implicitly defined when `PROBLEM` is set to `mechanical`. If `sigmay` was referenced before `PROBLEM`, FeenoX would not find it. And if the problem was of any other type, FeenoX would not find it even when referenced from the last line of the input file.

The following example further illustrates the differences between definitions and instructions. It compares the result of (numerically but adaptively) integrating $f(x,y,z) = \sin(x^3 + y^2 + z)$ over a unit cube against using a 3D Gauss integration scheme over a fixed set of quadrature points on the same unit cube meshes with two regular hexahedra in each direction (totalling 8 hexahedra). In one case hex20 are used and in the other one, hex27. Both cases use 27 quadrature points per element.

```{.feenox include=integral_over_hex.fee}
```

```terminal
$ $ feenox  integral_over_hex.fee 
0.7752945175
0.7753714586	+7.69e-05
0.7739155101	-1.38e-03
$
```


### Simple inputs {#sec:simple}

Consider solving heat conduction on a one-dimensional slab spanning the unitary range\ $x \in [0,1]$. The slab has a uniform unitary conductivity\ $k=1$ and Dirichlet boundary conditions

$$
\begin{cases}
T(0) &= 0 \\
T(1) &= 1
\end{cases}
$$

This simple problem has the following simple input:

```{.feenox include=thermal-1d-dirichlet-uniform-k.fee}
```

```terminal
$ feenox thermal-1d-dirichlet-uniform-k.fee
0.5
$
```

Now, if instead of having a uniform conductivity the problem had a space-dependent\ $k(x) = 1+x$ then the input would read

```{.feenox include=thermal-1d-dirichlet-space-k.fee}
```

```terminal
$ feenox thermal-1d-dirichlet-space-k.fee 
0.584893	0.584963
$
```

Finally, if the conductivity depended on temperature (rendering the problem non-linear) say like\ $k(x) = 1 + T(x)$ then

```{.feenox include=thermal-1d-dirichlet-temperature-k.fee}
```

```terminal
$ feenox thermal-1d-dirichlet-space-k.fee 
0.581139	0.581139
$
```

Note that FeenoX could figure out by itself that the two first cases were linear while the last one was not. This can be verified by passing the extra argument `--snes_view`. In the first two cases, there will be no extra output. In the last one, the details of the non-linear solver used by PETSc will be written into the standard output. The experienced reader should take some time to compare the effort and level of complexity that other FEA solvers require in order to set up simple problems like these. 

### Complex things {#sec:complex}

[Alan Kay](https://en.wikipedia.org/wiki/Alan_Kay)’s idea that “simple things should be simple, complex things should be possible” has already been mentioned. The first part of the quote was addressed in the previous section. Of course, complexity can scale up almost indefinitely so we cannot show an example right here. But the following repositories contain the scripts and input files (for Gmsh, FeenoX and other common Unix tools such as Sed and Awk) that solve non-trivial problems using FeenoX as the main tool and employing free and open source software only, both for the computation and for the creation of figures and result tables.

[Convergence study on stress linearization of an infinite pipe according to ASME](https://github.com/seamplex/pipe-linearize)

:   A parametric study over the number of elements through the thickness of a pipe and the error committed when computing membrane and bending stresses according to ASME\ VIII Div\ 2 Sec\ 5. The study uses the following element types

     - unstructured tet4
     - unstructured straight tet10
     - unstructured curved tet10
     - structured straight tet10
     - structured curved tet10
     - structured hex8
     - structured straight hex20
     - structured curved hex20
     - structured straight hex27
     - structured curved hex27

    The linearized stresses for different number of elements through the pipe thickness are compared against the analytical solution. Figures with the meshes employed in each case and with plots of profiles vs. the radial coordinate and linearized stresses vs. number of elements through the thickness are created. 

[Environmentally-assisted fatigue analysis of dissimilar material interfaces in piping systems of a nuclear power plant](https://github.com/seamplex/piping-asme-fatigue)

:   A case that studies environmentally-assisted fatigue using environment factors applied to traditional in-air ASME fatigue analysis for operational an incidental transients in nuclear power plant as propose by EPRI. A fictitious CAD geometry representing a section of a piping system is studied. Four operational transients are made up with time-dependent data for pressure and water temperature.

    1. A transient heat conduction problem with temperature-dependent material properties (according to ASME property tables) are solved over a small region around a material interface between carbon and stainless steel.
    2. Primary stresses according to ASME are computed for each of the operational transients.
    3. The results of a modal analysis study are convoluted with a frequency spectrum of a design-basis earthquake using the SRSS method to obtain an equivalent static volumetric force distribution.
    4. The time-dependent temperature distribution for each transient is then used in quasi-static mechanical problems to compute secondary stresses according to ASME, including the equivalent seismic loads at the moment of higher thermal stresses.
    5. The history of linearized Tresca stresses are juxtaposed to compute the cumulative usage factors using the ASME peak-valley method.
    6. Environmental data is used to affect each cumulative usage factors with an environment factor to account for in-water conditions.

These repositories contain a `run.sh` that, when executed in a properly-set-up GNU/Linux host (either on premises or in the cloud), will perform a number of steps including

 - creation of appropriate meshes
 - execution FeenoX
 - generation post-processing views, plots or tables with the results
 - etc.
 
Refer to the READMEs in each repository for further details about the mathematical models involved.

 
### Everything is an expression {#sec:expression}

As explained in the history of FeenoX (@sec:history), the first objective of the code was to solve ODEs written in an ASCII file as human-friendly as possible. So even before any other feature, the first thing I coded in the FeenoX ancestors was an algebraic parser and evaluator. This was back in 2009, and I performed an online search before writing the whole thing from scratch. I found a nice post in Slack Overflow^[<http://stackoverflow.com/questions/1384811/code-golf-mathematical-expression-evaluator-that-respects-pemdas>] that discussed some cool ideas and even had some code published under the terms of the Creative Commons license.

Besides ODEs, algebraic expressions are a must if one will be dealing with arbitrary functions in general and spatial distributions in particular---which is essentially what PDE solvers are for. If a piece of software wants to allow features ranging from comparing numerical results with analytical expression to converting material properties from a system of units to another one in a straightforward way for the user (either an actual human being or a computer creating an input file), it ought to be able to handle algebraic expressions. 

Appropriately handling algebraic expressions leads to fulfilling the Unix rule of least surprise (@sec:least-surprise). If the user needs to define a function $f(x) = 1/2 \cdot x^2$, all she has to do is write

```feenox
f(x) = 1/2 * x^2
```

And conversely, if someone reads the line above, she can rest assure that there's a function called\ $f(x)$ that will evaluate to\ $1/2 \cdot x^2$ when needed. In effect, anyone can expect the output of this instruction

```feenox
PRINT integral(f(x), x, 0, 1)
```

to be one third.

Of course expressions are needed to get 100%-user defined output (further discussed in\ @sec:output), as with the `PRINT` instruction above. 
But once an algebraic parser and evaluator is available, it does not make sense to keep force the user to write numerical data only. What if a the angular speed is in RPM and the model needs it in radians per second? Instead of having to write `104.72`, FeenoX allows the user to write

```
w = 1000 * 60*pi/180
```

This way, 

 1. it is easy to see what the speed in RPM is
 2. precision is not lost
 3. should the speed change, it is trivial to change the `1000` for anything else.
 
 

Whenever an input keyword needs a numerical value, any expression will do:

```feenox
n = 3
VECTOR x SIZE 2+n
x[i] = i^2
PRINT x
```

```terminal
$ feenox vector_size_one_plus_n.fee 
1       4       9       16      25
$
```

It goes without saying that algebraic expressions are a must when defining transient and/or space-dependent boundary conditions for PDEs:

```{.feenox include="thermal-1d-transient.fee"}
```

Besides purely algebraic expressions, FeenoX can handle point-wise defined functions which can then be used in algebraic expressions.
A useful example is allowing material properties (e.g. Young modulus) to depend on temperature.
Consider a simple plane-strain square\ $[-1,+1]\times[-1,+1]$ fixed on one side and with a uniform tension in the opposite one while leaving the other two free.
The square's Young modulus depends on temperature according to a one-dimensional point-wise defined function $E_\text{carbon}(T)$ given by pairs stated according to one of the many material properties tables from ASME\ II and interpolated using Steffen's method. Althouhg in this example the temperature is given as an algebraic expression of space, in particular

$$T(x,y)~[\text{ºC}] = 200 + 350 \cdot y$$

```{.feenox include="mechanical-square-temperature.fee"}
```

By replacing `T(x,y) = 200 + 350*y` with `T(x,y) = 200` we can compare the results of the temperature-dependent case with the uniform-properties case (@fig:mechanical-square):

```terminal
$ feenox mechanical-square-temperature.fee 
$ diff mechanical-square-temperature.fee mechanical-square-uniform.fee 
29c29
< T(x,y) := 200 + 350*y
---
> T(x,y) := 200
38c38
< WRITE_MESH mechanical-square-temperature.vtk  E  VECTOR u v 0   
---
> WRITE_MESH mechanical-square-uniform.vtk  E  VECTOR u v 0   
$ feenox mechanical-square-uniform.fee 
$
```

::: {#fig:mechanical-square layout-ncol=2}
![Temperature-dependent\ $E$](mechanical-square-temperature.png){width_latex=49%}

![Uniform $E$ for reference](mechanical-square-uniform.png){width_latex=49%}

Mechanical plane-strain square with temperature-dependent Young modulus and comparison with uniform reference case.
:::


In real applications this distribution\ $T(x,y)$ can be read from the output of an actual heat conduction problem. See @sec:non-conformal for a revisit of this case, reading the temperature from an unstructured triangular mesh instead of hard-coding it as an algebraic expression of space.

So remember, in FeenoX *everything is an expression*---especially temperature, as also shown in the next section.

### Matching formulations


The [Lorenz’ dynamical system](http://en.wikipedia.org/wiki/Lorenz_system) system and the [NAFEMS\ LE10](https://www.nafems.org/publications/resource_center/p18/) benchmark problem, both discussed earlier in @sec:scope, illustrate how well the FeenoX input file matches the usual human-readable formulation of ODE or PDE problems. In effect, @fig:nafems-le10-problem-input shows there is a trivial one-to-one correspondence between the sections of the problem formuated in a sheet of paper and the text file `nafems-le10.fee`. A further example can be given by solving the following case.

```include
nafems-le11.md
```

This feature can be better appreciated by comparing the input files needed to solve these kind of NAFEMS benchmarks with other finite-element tools. @Sec:le10-other gives a glympse for the NAFEMS\ LE10 problem, where the input files are way too cryptic and cumbersome as compared to what FeenoX needs.



### Comparison of solutions

One cornerstone design feature is that FeenoX has to provide a way to compare its numerical results with other already-know solutions---either analytical or numerical---in order to verify their validity. Indeed, being able to take the difference between the numerical result and an algebraic expression evaluated at arbitrary locations (usually quadrature points to compute~$L_p$ norms of the error) is a must if code verification through the Method of Manufactured Solutions is required (see @sec:mms).

Let us consider a one-dimensional slab reactor with uniform macroscopic cross sections

$$
\begin{aligned}
\Sigma_t &= 0.54628~\text{cm}^{-1} \\
\Sigma_s &= 0.464338~\text{cm}^{-1} \\
\nu\Sigma_f &= 1.70 \cdot 0.054628~\text{cm}^{-1}
\end{aligned}
$$

such that, if computed with neutron transport theory, is exactly critical with a width of $a = 2 \cdot 10.371065~\text{cm}$. Just to illustrate a simple case, we can solve it using the diffusion approximation with zero flux at both as. This case has an analytical solution for both the effective multiplication factor

$$
k_\text{eff} = \frac{\nu\Sigma_f}{(\Sigma_t - \Sigma_s) + D \cdot \left(\frac{\pi}{a} \right)^2}
$$

and the flux distribution

$$
\phi(x) = \frac{\pi}{2} \cdot \sin\left(\frac{x}{a} \cdot \pi\right)
$$

provided the diffusion coefficient\ $D$ is defined as

$$
D = \frac{1}{3 \cdot \Sigma_t}
$$


We can solve both the numerical and analytical problems in FeenoX, and more importantly, we can subtract them at any point of space we want:

```{.feenox include="neutron-diffusion-1d-null.fee"}
```

```terminal
$ feenox neutron-diffusion-1d-null.fee 
# x     phi1    phi_diff        phi1(x)-phi_diff(x)
+0.000  +0.000  +0.000  +0.000
+10.371 +1.574  +1.571  +0.003
+20.742 +0.000  +0.000  -0.000
+1.474  +0.348  +0.348  +0.001
+2.829  +0.654  +0.653  +0.001
+4.074  +0.911  +0.909  +0.002
+5.217  +1.118  +1.116  +0.002
+6.268  +1.280  +1.277  +0.002
+7.233  +1.399  +1.397  +0.003
+8.120  +1.483  +1.480  +0.003
+8.935  +1.537  +1.534  +0.003
+9.683  +1.565  +1.562  +0.003
+11.059 +1.565  +1.562  +0.003
+11.807 +1.537  +1.534  +0.003
+12.622 +1.483  +1.480  +0.003
+13.509 +1.399  +1.397  +0.003
+14.474 +1.280  +1.277  +0.002
+15.525 +1.118  +1.116  +0.002
+16.668 +0.911  +0.909  +0.002
+17.913 +0.654  +0.653  +0.001
+19.268 +0.348  +0.348  +0.001
# keff      =   0.96774162
# kdiff     =   0.96797891
# rel error =   -2.45e-04
$
```

Something similar could have been done for two groups of energy, although the equations get a little bit more complex so we leave it as an example in the Git repository.

A notable non-trivial thermo-mechanical problem that nevertheless has an analytical solution for the displacement field is the  [“Parallelepiped whose Young’s modulus is a function of the temperature”](https://www.seamplex.com/feenox/examples/#parallelepiped-whose-youngs-modulus-is-a-function-of-the-temperature) (@fig:parallelepiped).
The problem consists of finding the non-dimensional temperature\ $T$ and displacements\ $u$, $v$
and $w$ distributions within a solid parallelepiped of length\ $\ell$ whose base is a square of $h\times h$.
The solid is subject to heat fluxes and to a traction pressure at the same time.
The non-dimensional Young’s modulus\ $E$ of the material depends on the temperature\ $T$
in a know algebraically way, whilst both the Poisson coefficient\ $\nu$ and the thermal conductivity\ $k$
are uniform and do not depend on the spatial coordinates:

$$
\begin{aligned}
E(T) &= \frac{1000}{800-T} \\
\nu &= 0.3 \\
k &= 1
\end{aligned}
$$

![Parallelepiped whose Young’s modulus is a function of the temperature. Original figure from [v7.03.100.pdf](http://www.code-aster.org/V2/doc/default/fr/man_v/v7/v7.03.100.pdf)](parallelepiped.svg){#fig:parallelepiped}

The thermal boundary conditions are

 * Temperature at point\ $A$ at $(\ell,0,0)$ is zero \label{temp0}
 * Heat flux $q^{\prime \prime}$ through $x=\ell$ is +2
 * Heat flux $q^{\prime \prime}$ through $x=0$ is -2
 * Heat flux $q^{\prime \prime}$ through $y=h/2$ is +3
 * Heat flux $q^{\prime \prime}$ through $y=-h/2$ is -3
 * Heat flux $q^{\prime \prime}$ through $z=h/2$ is +4
 * Heat flux $q^{\prime \prime}$ through $z=-h/2$ is -4

The mechanical boundary conditions are

 * Point $O$ at $(0,0,0)$ is fixed
 * Point $B$ at $(0,h/2,0)$ is restricted to move only in the\ $y$ direction
 * Point $C$ at $(0,0,/h2)$ cannot move in the\ $x$ direction
 * Surfaces\ $x=0$ and\ $x=\ell$ are subject to an uniform normal traction equal to one

The analytical solution is

$$
\begin{aligned}
T(x,y,z) &= -2x -3y -4z + 40 \\
u(x,y,z) &= \frac{A}{2} \cdot\left[x^2 + \nu\cdot\left(y^2+z^2\right)\right] + B\cdot xy + C\cdot xz + D\cdot x - \nu\cdot \frac{Ah}{4} \cdot \left(y+z\right) \\
v(x,y,z) &= -\nu\cdot \left[A\cdot x y + \frac{B}{2} \cdot \left(y^2-z^2+\frac{x^2}{\nu}\right) + C\cdot y z + D\cdot y -A\cdot h/4\cdot x - C\cdot h/4\cdot z\right] \\
w(x,y,z) &= -\nu\cdot \left[A\cdot x z + B\cdot yz + C/2\cdot \left(z^2-y^2+\frac{x^2}{\nu}\right) + D\cdot z + \frac{Ch}{4} \cdot y - \frac{Ah}{4} \cdot x\right] \\
\end{aligned}
$$

where~$A=0.002$, $B=0.003$, $C=0.004$ and~$D=0.76$.
The reference results are the temperature at points O and D and the displacements at points A and D (@tbl:parallelepiped}.

Point |  Unknown  |  Reference value
:----:|:---------:|:---------------------
  O   |    $T$    | +40.0
  D   |    $T$    | -35.0
  A   |    $u$    | +15.6
      |    $v$    | -0.57
      |    $w$    | -0.77
  D   |    $u$    | +16.3
      |    $v$    | -1.785
      |    $w$    | -2.0075

: Reference results the original benchmark problem {#tbl:parallelepiped}

First, the thermal problem is solved with FeenoX and the temperature distribution $T(x,y,z)$ is written into a `.msh` file.

```{.feenox include="neutron-diffusion-1d-null.fee"}
```

Then, the mechanical problem reads two meshes: one for solving the actual mechanical problem and another one for reading $T(x,y,z)$ from the previous step. Note that the former contains second-order hexahedra and the latter first-order tetrahedra. After effectively solving the problem, it writes again @tbl:parallelepiped in Markdown. 

The FeenoX implementation illustrates several design features, namely????????????

 



### Run-time arguments

Already shown

### Git and macro-friendliness {#sec:git-friendliness}

The FeenoX input files as plain ASCII files by design.
This means that they can be tracked with Git or any other version control system so as to allow reliable traceability of computations. Along with the facts that FeenoX interacts well with
 
 a. Gmsh, that can either use ASCII input files as well or be used as an API from C, C++, Python and Julia, and
 b. Other scripting languages such as Bash, Python or even AWK, whose input files are ASCII files as well,
 
makes it possible to track a whole computation using FeenoX as a Git repository, as already exemplified in @sec:complex.
It is important to note that what files that should be tracked in Git include

 1. READMEs and documentation in Markdown
 2. Shell scripts
 3. Gmsh input files and/or scripts
 4. FeenoX input files

Files that should not be tracked include

 1. Documentation in HTML or PDF
 2. Mesh files
 3. VTK and result files

since in principle they could be generated from the files in the Git repository by executing the scripts, Gmsh and/or FeenoX.

Even more, in some cases, the FeenoX input files---following the Unix rule of generation @sec:generation--can be created out of generic macros that create particular cases. For example, say one has a mesh of a fin-based dissipator where all the surfaces are named `surf_1_`$i$ for $i=1,...,26$. All of them will have a convection boundary condition while surface number\ 6 is the one that is attached to the electronic part that has to be cooled. Instead of having to "manually" giving the list of surfaces that have the convection condition, we can use M4 to do it for us:

```{.feenox include="fins.fee.m4"}
```

Note that since FeenoX was born in Unix, we can pipe the output of `m4` to FeenoX directly by using `-` as the input file in the command line:

```terminal
$ m4 fins.fee.m4 | feenox -
$
```

@Fig:fins-temperature confirms that all the faces have the right boundary conditions: face number six got the power BC and all the rest got the convection BC.

![Temperature distribution in a fin dissipator where all the faces have a convection BC except one that has a fixed heat flux of $q'' = 1,000 \text{W} \cdot \text{m}^{-2}$.](fins-temp.png){#fig:fins-temperature}



Besides being ASCII files, should special characters be needed for any reason within a particular application of FeenoX, UTF-8 characters can be used natively as illustrated in @fig:utf-8.

::: {#fig:utf-8 layout-ncol=2}
![UTF-8 in Kate](utf8-kate.png)

![UTF-8 in Bash (through Konsole)](utf8-shell.png)

Mechanical plane-strain square with temperature-dependent Young modulus and comparison with uniform reference case.
:::



macro-friendly inputs, rule of generation
ejemplo del alpha-flex


VCS tracking, example with hello world.
data separated from mesh


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

### Data exchange between non-conformal meshes {#sec:non-conformal}

To illustrate how the output of a FeenoX execution can be read by another FeenoX instance, let us revisit the plane-strain square from @sec:expression. This time, instead of setting the temperature with an algebraic expression, we will solve a thermal problem that gives rise to the same temperature distribution but on a different mesh.

First, we solve a thermal problem on the same square\ $[-1,+1]\times[-1,+1]$ such that the resulting temperature field is $T(x,y) = 200 + 350\cdot y$:

```{.feenox include="thermal-square.fee"}
```

Now, we read the temperature\ $T(x,y)$ from the thermal output mesh file `thermal-square-temperature.msh` (which is a triangular unstructured grid) into the mechanical input mesh file `square-centered.msh` (which is a structured quadrangular grid):

```{.feenox include="mechanical-square-temperature-from-msh.fee"}
```

Indeed, the terminal mimic shows the difference between the mechanical input from this section and the one that used an explicit algebraic expression.

```terminal
$ gmsh -2 square-centered-unstruct.geo 
[...]
Info    : Done meshing 2D (Wall 0.012013s, CPU 0.033112s)
Info    : 65 nodes 132 elements
Info    : Writing 'square-centered-unstruct.msh'...
Info    : Done writing 'square-centered-unstruct.msh'
Info    : Stopped on Wed Aug  3 17:47:39 2022 (From start: Wall 0.0208329s, CPU 0.064825s)
$ feenox thermal-square.fee 
$ feenox mechanical-square-temperature-from-msh.fee 
$ diff mechanical-square-temperature-from-msh.fee mechanical-square-temperature.fee
26,27c26,29
< # read the temperature from a previous result
< READ_MESH thermal-square-temperature.msh DIM 2 READ_FUNCTION T
---
> 
> # known temperature distribution
> # (we could have read it from an outpout of a thermal problem)
> T(x,y) := 200 + 350*y
36c38
< WRITE_MESH mechanical-square-temperature-from-msh.vtk  E T VECTOR u v 0   
---
> WRITE_MESH mechanical-square-temperature.vtk  E  VECTOR u v 0   
$ 
```

::: {#fig:mechanical-square-from-temp layout-ncol=2}
![Numerical temperature$T(x,y)$ as the result of a thermal problem over an unstructured grid](thermal-square-temperature.png){width_latex=42%}

![Plane-strain with temperature-dependent$E(T)$ case](mechanical-square-temperature-from-msh.png){width_latex=55%}

Mechanical plane-strain square with temperature-dependent Young modulus read from a non-conformal mesh as the result of a thermal problem. Compare with @fig:mechanical-square.
:::



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


## Verification {#sec:verification}

> ```include
> 440-verification.md
> ```


open source (really, not like CCX -> mostrar ejemplo)
GPLv3+ free
Git + gitlab, github, bitbucket

### Method of Exact Solutions {#sec:mes}

### Method of Manufactured Solutions {#sec:mms}

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



```include
sds-appendices.md
```
