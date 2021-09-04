---
abstract: This design specification addresses the (hypothetical)
  Software Requirement Specification for developing a piece of free and
  open source engineering software with certain specifications, defined
  in an imaginary tender.
autoEqnLabels: false
autoSectionLabels: false
ccsDelim: ", "
ccsLabelSep: " --- "
ccsTemplate: $$i$$$$ccsLabelSep$$$$t$$
chapDelim: .
chapters: false
chaptersDepth: 1
codeBlockCaptions: false
colorlinks: true
cref: false
crossrefYaml: pandoc-crossref.yaml
eqLabels: arabic
eqnPrefix:
- eq.
- eqns.
eqnPrefixTemplate: $$p$$ $$i$$
equationNumberTeX: qquad
figLabels: arabic
figPrefix:
- fig.
- figs.
figPrefixTemplate: $$p$$ $$i$$
figureTemplate: $$figureTitle$$ $$i$$$$titleDelim$$ $$t$$
figureTitle: Figure
fontsize: 11pt
geometry:
- paper=a4paper
- left=2.5cm
- right=2cm
- bottom=3.5cm
- foot=2cm
- top=3.5cm
- head=2cm
header-includes: |
  ```{=tex}
  \include{syntax.tex}
  ```
lang: en-US
lastDelim: ", "
linkReferences: false
listings: true
listingTemplate: $$listingTitle$$ $$i$$$$titleDelim$$ $$t$$
listingTitle: Listing
lofTitle: |
  # List of Figures
lolTitle: |
  # List of Listings
lotTitle: |
  # List of Tables
lstLabels: arabic
lstPrefix:
- lst.
- lsts.
lstPrefixTemplate: $$p$$ $$i$$
mathspec: true
nameInLink: false
number-sections: true
numberSections: false
pairDelim: ", "
rangeDelim: "-"
refDelim: ", "
refIndexTemplate: $$i$$$$suf$$
secHeaderDelim: 
secHeaderTemplate: $$i$$$$secHeaderDelim[n]$$$$t$$
secLabels: arabic
secPrefix:
- sec.
- secs.
secPrefixTemplate: $$p$$ $$i$$
sectionsDepth: 0
subfigGrid: false
subfigLabels: alpha a
subfigureChildTemplate: $$i$$
subfigureRefIndexTemplate: $$i$$$$suf$$ ($$s$$)
subfigureTemplate: $$figureTitle$$ $$i$$$$titleDelim$$ $$t$$. $$ccs$$
syntax-definition: feenox.xml
tableEqns: false
tableTemplate: $$tableTitle$$ $$i$$$$titleDelim$$ $$t$$
tableTitle: Table
tblLabels: arabic
tblPrefix:
- tbl.
- tbls.
tblPrefixTemplate: $$p$$ $$i$$
title: FeenoX Software Design Specification
titleDelim: ":"
toc: true
---

```{=tex}
\include{syntax.tex}
```

-   [Introduction]
    -   [Objective]
    -   [Scope]
-   [Architecture]
    -   [Deployment]
    -   [Execution]
    -   [Efficiency]
    -   [Scalability]
    -   [Flexibility]
    -   [Extensibility]
    -   [Interoperability]
-   [Interfaces]
    -   [Problem input]
    -   [Results output]
-   [Quality assurance]
    -   [Reproducibility and traceability]
    -   [Automated testing]
    -   [Bug reporting and tracking]
    -   [Verification]
    -   [Validation]
    -   [Documentation]
-   [Appendix: Rules of UNIX philosophy]

  [Introduction]: #sec:introduction
  [Objective]: #sec:objective
  [Scope]: #sec:scope
  [Architecture]: #sec:architecture
  [Deployment]: #sec:deployment
  [Execution]: #sec:execution
  [Efficiency]: #sec:efficiency
  [Scalability]: #sec:scalability
  [Flexibility]: #flexibility
  [Extensibility]: #sec:extensibility
  [Interoperability]: #sec:interoperability
  [Interfaces]: #interfaces
  [Problem input]: #sec:input
  [Results output]: #sec:output
  [Quality assurance]: #sec:qa
  [Reproducibility and traceability]: #reproducibility-and-traceability
  [Automated testing]: #automated-testing
  [Bug reporting and tracking]: #bug-reporting-and-tracking
  [Verification]: #sec:verification
  [Validation]: #validation
  [Documentation]: #sec:documentation
  [Appendix: Rules of UNIX philosophy]: #sec:unix

# Introduction {#sec:introduction}

> A cloud-based computational tool (herein after referred to as *the
> tool*) is required in order to solve engineering problems following
> the current state-of-the-art methods and technologies impacting the
> high-performance computing world. This (imaginary but plausible)
> Software Requirements Specification document describes the mandatory
> features this tool ought to have and lists some features which would
> be nice the tool had. Also it contains requirements and guidelines
> about architecture, execution and interfaces in order to fulfill the
> needs of cognizant engineers as of 2021 (and the years to come) are
> defined.
>
> On the one hand, the tool should be applicable to solving industrial
> problems under stringent efficiency (sec. 2.3) and quality (sec. 4)
> requirements. It is therefore mandatory to be able to assess the
> source code for potential performance and verification revision by
> qualified third parties from all around the world, so it has to be
> *open source*. On the other hand, the initial version of the tool is
> expected to be a basic framework which might be extended (sec. 1.1 and
> sec. 2.6) by academic researchers and programmers. It thus should also
> be *free*---in the sense of freedom, not in the sense of price. There
> is no requirement on the pricing scheme. The detailed licensing terms
> are left to the offer but it should allow users to solve their
> problems the way they need and, eventually, to modify and improve the
> tool to suit their needs. If they cannot program themselves, they
> should have the freedom to hire somebody to do it for them.

Besides noting that software being *free* (regarding freedom, not price)
does not imply the same as being *open source*, the requirement is clear
in that the tool has to be both *free* and *open source*, a combination
which is usually called [FOSS]. This condition leaves all of the
well-known non-free finite-element solvers in the market out of the
tender.

FeenoX is licensed under the terms of the [GNU General Public License]
version 3 or, at the user convenience, any later version. This means
that users get the four essential freedoms:

0.  The freedom to *run* the program as they wish, for *any* purpose.
1.  The freedom to *study* how the program works, and *change* it so it
    does their computing as they wish.
2.  The freedom to *redistribute* copies so they can help others.
3.  The freedom to *distribute* copies of their *modified* versions to
    others.

There are some examples of pieces of computational software which are
described as "open source" in which even the first of the four freedoms
is denied. The most iconic case is that of Android, whose sources are
readily available online but there is no straightforward way of updating
one's mobile phone firmware with a customized version, not to mention
vendor and hardware lock ins and the possibility of bricking devices if
something unexpected happens. In the nuclear industry, it is the case of
a Monte Carlo particle-transport program that requests users to sign an
agreement about the objective of its usage before allowing its
execution. The software itself might be open source because the source
code is provided after signing the agreement, but it is not free (as in
freedom) at all.

So a free program has to be open source, but it also has to explicitly
provide the four freedoms above both through the written license and
through the mechanisms available to get, modify, compile, run and
document these modifications. That is why licensing FeenoX as GPLv3+
also implies that the source code and all the scripts and makefiles
needed to compile and run it are available for anyone that requires it.
Anyone wanting to modify the program either to fix bugs, improve it or
add new features is free to do so. And if they do not know how to
program, the have the freedom to hire a programmer to do it without
needing to ask permission to the original authors.

Nevertheless, since these original authors are the copyright holders,
they still can use it to either enforce or prevent further actions from
the users that receive FeenoX under the GPLv3+. In particular, the
license allows re-distribution of modified versions only if they are
clearly marked as different from the original and only under the same
terms of the GPLv3+. There are also some other subtle technicalities
that need not be discussed here such as what constitutes a modified
version (which cannot be redistributed under a different license) and
what is an aggregate (in which each part be distributed under different
licenses) and about usage over a network and the possibility of using
[AGPL] instead of GPL to further enforce freedom (TLDR; it does not
matter for FeenoX), but which are already taken into account in FeenoX
licensing scheme.

  [FOSS]: https://en.wikipedia.org/wiki/Free_and_open-source_software
  [GNU General Public License]: https://www.gnu.org/licenses/gpl-3.0
  [AGPL]: https://en.wikipedia.org/wiki/GNU_Affero_General_Public_License

## Objective {#sec:objective}

> The main objective of the tool is to be able to solve engineering
> problems which are usually casted as differential-algebraic equations
> (DAEs) or partial differential equations (PDEs), such as
>
> -   heat conduction
> -   mechanical elasticity
> -   structural modal analysis
> -   frequency studies
> -   electromagnetism
> -   chemical diffusion
> -   process control dynamics
> -   computational fluid dynamics
> -   ...
>
> `\noindent`{=tex} on one or more manistream cloud servers,
> i.e. computers with an architecture (i.e. hardware and operating
> systems, futher discussed in sec. 2) that allows them to be available
> online and accessed remotely either interactively or automatically by
> other computers as well. Other architectures such as high-end desktop
> personal computers or even low-end laptops might be supported but they
> should not the main target.
>
> The initial version of the tool must be able to handle a subset of the
> above list of problem types. Afterward, the set of supported problem
> types, models, equations and features of the tool should grow to
> include other models as well, as required in sec. 2.6.

The choice of the initial supported features is based on the types of
problem that the FeenoX' precursor codes (namely wasora, Fino and
milonga) already have been supporting since more than ten years now. It
is also a first choice so work can be bounded and a subsequent road map
and release plans can be designed. FeenoX' first version includes a
subset of the required functionality, namely

-   dynamical systems
-   Laplace/Poisson/Helmholtz equations
-   plant control dynamics
-   heat conduction
-   mechanical elasticity
-   structural modal analysis
-   multigroup neutron transport and diffusion

FeenoX is designed to be developed and executed under GNU/Linux, which
is the architecture of more than 95% of the internet servers which we
collectively call "the cloud." It should be noted that GNU/Linux is a
POSIX-compliant version of UNIX and that FeenoX follows the rules of
UNIX philosophy (sec. 5) regarding its computational implementation
code. Besides POSIX, FeenoX also uses MPI which is a well-known industry
standard for massive parallel executions of processes, both in
multi-core hosts and multi-hosts environments. Finally, if performance
and/or scalability are not important issues, FeenoX can be run in a
(properly cooled) local PC or laptop.

The requirement to run in the cloud and scale up as needed rules out
some of the FOSS solvers available online, such as CalculiX.

## Scope {#sec:scope}

> The tool should allow advanced users to define the problem to be
> solved programmatically. That is to say, the problem should be
> completely defined using one or more files either...
>
> a.  specifically formatted for the tool to read such as JSON or a
>     particular input format (historically called input decks in
>     punched-card days), and/or
> b.  written in an high-level interpreted language such as Python or
>     Julia.
>
> It should be noted that a graphical user interface is not required.
> The tool may include one, but it should be able to run without needing
> any user intervention rather than the preparation of a set of input
> files. Nevertheless, there tool might *allow* a GUI to be used. For
> example, for a basic usage involving simple cases, a user interface
> engine should be able to create these problem-definition files in
> order to give access to less advanced users to the tool using a
> desktop, mobile and/or web-based interface in order to run the actual
> tool without further intervention.
>
> However, for general usage, users should be able to completely define
> the problem (or set of problems, i.e. a parametric study) they want to
> solve in one or more input files and to obtain one or more output
> files containing the desired results, either a set of scalar outputs
> (such as maximum stresses or mean temperatures), and/or a time and/or
> spatial distribution. If needed, a discretization of the domain may to
> be taken as a known input, i.e. the tool is not required to create the
> mesh as long as a suitable mesher can be employed using a similar
> workflow specified in this SRS.
>
> The tool should define and document (sec. 4.6) the way the input files
> for a solving particular problem are to be prepared (sec. 3.1) and how
> the results are to be written (sec. 3.2). Any GUI, pre-processor,
> post-processor or other related graphical tool used to provide a
> graphical interface for the user should integrate in the workflow
> described in the preceding paragraph: a pre-processor should create
> the input files needed for the tool and a post-processor should read
> the output files created by the tool.

Indeed, FeenoX is designed to work very much like a transfer function
between two (or more) files---usually the FeenoX input file and the
problem mesh file---and zero or more output files---usually the terminal
output and a VTK post-processing file---as illustrated in fig. 1.

![Figure 1: FeenoX working as a transfer function between input and
output files]

In some particular cases, FeenoX can also provide an API for high-level
interpreted languages such as Python or Julia such that a problem can be
completely defined in a script, increasing also flexibility.

As already stated, FeenoX is designed and implemented following the UNIX
philosophy in general and Eric Raymond's 17 Unix Rules (\[sec:unix\]) in
particular. One of the main ideas is the rule of *separation* that
essentially implies to separate mechanism from policy, that in the
computational engineering world translates into separating the frontend
from the backend. Even though most FEA programs eventually separate the
interface from the solver up to some degree, there are cases in which
they are still dependent such that changing the former needs updating
the latter.

From the very beginning, FeenoX is designed as a pure backend which
should nevertheless provide appropriate mechanisms for different
frontends to be able to communicate and to provide a friendly interface
for the final user. Yet, the separation is complete in the sense that
the nature of the frontends can radically change (say from a
desktop-based point-and-click program to a web-based immersive
augmented-reality application) without needing the modify the backend.
Not only far more flexibility is given by following this path, but also
develop efficiency and quality is encouraged since programmers working
on the lower-level of an engineering tool usually do not have the skills
needed to write good user-experience interfaces, and conversely.

In the very same sense, FeenoX does not discretize continuous domains
for PDE problems itself, but relies on separate tools for this end.
Fortunately, there already exists one meshing tool which is FOSS (GPLv2)
and shares most (if not all) of the design basis principles with FeenoX:
the three-dimensional finite element mesh generator [Gmsh]. Strictly
speaking, FeenoX does not need to be used along with Gmsh but with any
other mesher able to write meshes in Gmsh's format `.msh`. But since
Gmsh also

-   is free and open source,
-   works also in a transfer-function-like fashion,
-   runs natively on GNU/Linux,
-   has a similar (but more comprehensive) API for Python/Julia,
-   etc.

`\noindent `{=tex}it is a perfect match for FeenoX. Even more, it
provides suitable domain decomposition methods (through other FOSS
third-party libraries such as [Metis]) for scaling up large problems,

For example, we can solve the [NAFEMS LE11] "Solid
cylinder/Taper/Sphere-Temperature" benchmark like

``` feenox
READ_MESH nafems-le11.msh DIMENSIONS 3
PROBLEM mechanical

# linear temperature gradient in the radial and axial direction
T(x,y,z) = sqrt(x^2 + y^2) + z

# Boundary conditions
BC xz     symmetry  # same as v=0 but "symmetry" follows the statement
BC yz     symmetry  # ide with u=0
BC xy     w=0
BC HIH'I' w=0

# material properties (isotropic & uniform so we can use scalar constants)
E = 210e3*1e6       # mesh is in meters, so E=210e3 MPa -> Pa
nu = 0.3            # dimensionless
alpha = 2.3e-4      # in 1/ºC as in the problem

SOLVE_PROBLEM
WRITE_MESH nafems-le11.vtk VECTOR u v w   T sigma1 sigma2 sigma3 sigma sigmaz
PRINT "sigma_z(A) = " sigmaz(0,1,0)/1e6 "MPa"
```

Another example would be the famous chaotic [Lorenz' dynamical
system]---the one of the butterfly---whose differential equations are

```{=plain}
dx/dt = σ (y-x)  
dy/dt = x (r-z) - y
dz/dt = x y - b z
```
```{=latex}
\begin{equation*}
\begin{cases}
\dot{x} &= \sigma \cdot (y - x)  \\
\dot{y} &= x \cdot (r - z) - y   \\
\dot{z} &= x y - b z       \\
\end{cases}
\end{equation*}
```
::: {.not-in-format .plain .latex}
$$\dot{x} = \sigma \cdot (y - x)$$ $$\dot{y} = x \cdot (r - z) - y$$
$$\dot{z} = x \cdot y - b \cdot z$$
:::

where $\sigma=10$, $b=8/3$ and $r=28$ are the classical parameters that
generate the butterfly as presented by Edward Lorenz back in his seminal
1963 paper [Deterministic non-periodic flow]. We can solve it with
FeenoX by writing the equations in the input file as naturally as
possible, as illustrated in the input file that follows:

``` .feenox
PHASE_SPACE x y z     # Lorenz attractor’s phase space is x-y-z
end_time = 40         # we go from t=0 to 40 non-dimensional units

sigma = 10            # the original parameters from the 1963 paper
r = 28
b = 8/3

x_0 = -11             # initial conditions
y_0 = -16
z_0 = 22.5

# the dynamical system's equations written as naturally as possible
x_dot = sigma*(y - x)
y_dot = x*(r - z) - y
z_dot = x*y - b*z

PRINT t x y z        # four-column plain-ASCII output
```

Please note the following two points about both cases above:

1.  The input files are very similar to the statements of each problem
    in plain English words (*rule of clarity*). Take some time to read
    the [problem statement of the NAFEMS LE11 benchmark] and the FeenoX
    input to see how well the latter matches the former. Same for the
    Lorenz' chaotic system. Those with some experience may want to
    compare them to the inputs decks (sic) needed for other common FEA
    programs.
2.  By design, 100% of FeenoX' output is controlled by the user. Had
    there not been any `PRINT` or `WRITE_MESH` instructions, the output
    would have been empty, following the *rule of silence*. This is a
    significant change with respect to traditional engineering codes
    that date back from times when one CPU hour was worth dozens (or
    even hundreds) of engineering hours. At that time, cognizant
    engineers had to dig into thousands of lines of data to search for a
    single individual result. Nowadays, following the *rule of economy*,
    it is actually far easier to ask the code to write only what is
    needed in the particular format that suits the user.

  [Figure 1: FeenoX working as a transfer function between input and output files]:
    transfer.svg {#fig:transfer width="50%"}
  [Gmsh]: http://gmsh.info/
  [Metis]: http://glaros.dtc.umn.edu/gkhome/metis/metis/overview
  [NAFEMS LE11]: https://www.nafems.org/publications/resource_center/p18/
  [Lorenz' dynamical system]: http://en.wikipedia.org/wiki/Lorenz_system
  [Deterministic non-periodic flow]: http://journals.ametsoc.org/doi/abs/10.1175/1520-0469%281963%29020%3C0130%3ADNF%3E2.0.CO%3B2
  [problem statement of the NAFEMS LE11 benchmark]: doc/design/nafems-le11/nafems-le11.png

# Architecture {#sec:architecture}

> The tool must be aimed at being executed unattended on remote cloud
> servers which are expected to have a mainstream[^1] architecture
> regarding operating system (GNU/Linux variants and other UNIX-like
> OSes) and hardware stack (a few intel-compatible CPUs per host, a few
> levels of memory caches, a few gigabyes of random-access memory,
> several gigabytes of disk storage, etc.). It should successfully run
> on bare-metal, virtual and/or containerized servers using standard
> compilers, dependencies and libraries already available in the
> repositories of most current operating systems distributions.
>
> Preference should be given to open source compilers, dependencies and
> libraries. Small problems might be executed in a single host but large
> problems ought to be split through several server instances depending
> on the processing and memory requirements. The computational
> implementation should adhere to open and well-established standards.
>
> Ability to run on local desktop personal computers and/laptops is not
> required but suggested as a mean of giving the opportunity to users to
> test and debug small coarse computational models before launching the
> large computation on a HPC cluster or on a set of scalable cloud
> instances. Support for non-GNU/Linux operating systems is not required
> but also suggested.
>
> Mobile platforms such as tablets and phones are not suitable to run
> engineering simulations due to their lack of proper electronic cooling
> mechanisms. They are suggested to be used to control one (or more)
> instances of the tool running on the cloud, and even to pre and post
> process results through mobile and/or web interfaces.

FeenoX' main development architecture is Debian GNU/Linux running over
64-bits Intel-compatible processors. Compatibility with other operating
systems and architectures is encouraged although not required. All the
required dependencies are free and/or open source and already available
in Debian's official repositories and the POSIX standard is followed
whenever possible.

FeenoX is written in plain C conforming to the ISO C99 specification
(plus POSIX extensions), which is a standard, mature and widely
supported language with compilers for a wide variety of architectures.
For its basic mathematical capabilities, FeenoX uses the [GNU Scientifc
Library]. For solving ODEs/DAEs, FeenoX relies on [Lawrence Livermore's
SUNDIALS library]. For PDEs, FeenoX uses [Argonne's PETSc library]. All
three of them are

-   free and open source,
-   written in C (neither Fortran nor C++),
-   mature and stable,
-   actively developed and updated,
-   very well known in the industry and academia.

Programs using both these libraries can run on either large
high-performance supercomputers or low-end laptops. There are people
that have been able to compile them under macOS and Windows apart from
the main architecture based on GNU/Linux. Due to the way that FeenoX is
designed and the policy separated from the mechanism, it is possible to
control a running instance remotely from a separate client which can
eventually run on a mobile device.

[^1]: as of 2021

  [GNU Scientifc Library]: https://www.gnu.org/software/gsl/
  [Lawrence Livermore's SUNDIALS library]: https://computing.llnl.gov/projects/sundials/ida
  [Argonne's PETSc library]: https://www.mcs.anl.gov/petsc/

## Deployment {#sec:deployment}

> The tool should be easily deployed to production servers. Both
>
> a.  an automated method for compiling the sources from scratch aiming
>     at obtaining optimized binaries for a particular host architecture
>     should be provided using a well-established procedures, and
> b.  one (or more) generic binary version aiming at common server
>     architectures should be provided.
>
> Either option should be available to be downloaded from suitable
> online sources, either by real people and/or automated deployment
> scripts.

**show how to download and run from binary**

FeenoX can be compiled from its sources using the well-established
`configure` & `make` procedure. The code's source tree is hosted on
Github so cloning the repository is the preferred way to obtain FeenoX,
but source tarballs are periodically released too.

The configuration and compilation is based on GNU Autotools that has
more than thirty years of maturity and it is the most portable way of
compiling C code in a wide variety of UNIX variants. It can use the any
C99-compatible C compiler (it has been tested with GNU C compiler and
LLVM's Clang compiler).

FeenoX relies on a few open source libraries---most of them optional.
The only mandatory library is the GNU Scientific Library which is part
of the GNU/Linux operating system and as such is readily available in
all distributions as `libgsl-dev`. The sources of the rest of the
optional libraries are also widely available in most common GNU/Linux
distributions:

-   [GNU Readline][]: `libreadline-dev`
-   [SUNDIALS IDA][]: `libsundials-dev`
-   [PETSc][]: `petsc-dev`
-   [SLEPc][]: `slepc-dev`

Even though compiling FeenoX from sources is the recommended way to
obtain the tool, since the target binary can be compiled using
particularly suited compilation options, flags and optimizations
(especially those related to MPI, linear algebra kernels and direct
and/or iterative sparse solvers), there are also tarballs with usable
binaries for some of the most common architectures---including some
non-GNU/Linux variants. These binary distributions contain
statically-linked executables that do not need any other shared
libraries to be present on the target host, but their flexibility and
efficiency is generic and far from ideal. Yet the flexibility of having
an execution-ready distribution package for users that do not know how
to compile C source code outweights the limited functionality and
scalability of the tool.

Here are the steps to get FeenoX' source repository, compile it and run
the tests suite. Even though they are slightly more complex, they are
still pretty standard and straightforward:

1.  Install mandatory dependencies

    ``` terminal
    sudo apt-get install gcc make git automake autoconf libgsl-dev
    ```

    If you cannot install `libgsl-dev` but still have `git` and the
    build toolchain, you can have the `configure` script to download and
    compile it for you. See point 4 below.

2.  Install optional dependencies (of course these are *optional* but
    recommended)

    ``` terminal
    sudo apt-get install libsundials-dev petsc-dev slepc-dev
    ```

3.  Clone Github repository

    ``` terminal
    git clone https://github.com/seamplex/feenox
    ```

4.  Boostrap, configure, compile & make

    ``` terminal
    cd feenox
    ./autogen.sh
    ./configure
    make
    ```

    If you cannot (or do not want) to use `libgsl-dev` from a package
    repository, call `configure` with `--enable-download-gsl`:

    ``` terminal
    configure --enable-download-gsl
    ```

    If you do not have Internet access, get the tarball manually, copy
    it to the same directory as `configure` and run again.

5.  Run test suite (optional)

    ``` terminal
    make check
    ```

6.  Install the binary system wide (optional)

    ``` terminal
    sudo make install
    ```

  [GNU Readline]: https://tiswww.case.edu/php/chet/readline/rltop.html
  [SUNDIALS IDA]: https://computing.llnl.gov/projects/sundials/ida
  [PETSc]: https://www.mcs.anl.gov/petsc/
  [SLEPc]: https://slepc.upv.es/

## Execution {#sec:execution}

> It is mandatory to be able to execute the tool remotely, either with a
> direct action from the user or from a high-level workflow which could
> be triggered by a human or by an automated script. The calling party
> should be able to monitor the status during run time and get the
> returned error level after finishing the execution.
>
> The tool shall provide a mean to perform parametric computations by
> varying one or more problem parameters in a certain prescribed way
> such that it can be used as an inner solver for an outer-loop
> optimization tool. In this regard, it is desirable if the tool could
> compute scalar values such that the figure of merit being optimized
> (maximum temperature, total weight, total heat flux, minimum natural
> frequency, maximum displacement, maximum von Mises stress, etc.) is
> already available without needing further post-processing.

> It is mandatory to be able to execute the tool remotely, either with a
> direct action from the user or from a high-level workflow which could
> be triggered by a human or by an automated script. The calling party
> should be able to monitor the status during run time and get the
> returned error level after finishing the execution.

As FeenoX is designed to run as a file filter (i.e. as a transfer
function between input and output files) and it explicitly avoids having
a graphical interface, the binary executable works as any other UNIX
terminal command. When invoked without arguments, it prints its version,
one-line description and the usage options:

``` {.terminal style="terminal"}
FeenoX v0.1.38-gd477abf-dirty 
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

Of course the program can be executed remotely

1.  on a server through a SSH session
2.  in a container as part of a provisioning script

FeenoX provides mechanisms to inform its progress by writing certain
information to devices or files, which in turn can be monitored remotely
or even trigger server actions. This is part of the UNIX philosophy.

> The tool shall provide a mean to perform parametric computations by
> varying one or more problem parameters in a certain prescribed way
> such that it can be used as an inner solver for an outer-loop
> optimization tool. In this regard, it is desirable if the tool could
> compute scalar values such that the figure of merit being optimized
> (maximum temperature, total weight, total heat flux, minimum natural
> frequency, maximum displacement, maximum von Mises stress, etc.) is
> already available without needing further post-processing.

There are two ways of performing parametric runs in FeenoX. The first
one is through the expansion of command line arguments as string
literals `$1`, `$2`, etc. appearing in the input file. For example, if
`expansion.fee` is

``` {.feenox style="feenox"}
PRINT "Hello $1!"
```

`\noindent `{=tex}then it can be used as a custom greeting tool:

``` {.terminal style="terminal"}
$ feenox World
Hello World!
$ feenox Universe
Hello Universe!
```

When this feature is used in conjunction with a shell loop, flexible
parametric runs are possible. Say there are two meshes of the same
domain (two squares made of two different materials): one using
triangles and one using quadrangles. Let us say also that it is desired
to solve a non-linear thermal problem with different values for the
fixed temperature on the right boundary.

::: {#fig:two-squares .subfigures}
![a] ![b]

Figure 2: Heat conduction on two 2D squares with different
temprature-depedent conductivities. a --- Triangular elements, b ---
Quadrangular elements
:::

Consider the input file

``` {.feenox style="feenox"}
READ_MESH two-squares-$2.msh DIMENSIONS 2
PROBLEM thermal

# per-material conductivity
k_soft(x,y) := 1+T(x,y)
k_hard(x,y) := 1-0.1*T(x,y)

BC left  T=0
BC right T=$1

SOLVE_PROBLEM
PRINT TEXT $1 TEXT "Tright=$1" T(1,0.5)
```

`\noindent `{=tex}and the shell script

``` {.bash style="bash"}
#!/bin/bash
for temp in $(seq 1 3); do
 for shape in triang quad; do
   feenox two-squares-thermal.fee ${temp} ${shape}
 done
done
```

Then it is possible to run the six combinations at once, obtaining

``` {.terminal style="terminal"}
$ ./two-squares-thermal.sh
1       Tright=1        0.432843
1       Tright=1        0.432965
2       Tright=2        0.767466
2       Tright=2        0.767729
3       Tright=3        1.03429
3       Tright=3        1.03464
```

This way of performing parametric studies is very flexible since the
varied arguments can be either strings or numbers. Both the actual input
itself and the external driver script can be tracked using version
control systems, allowing for an efficient and flexible traceability
scheme. This is a design feature of FeenoX, inspired by the UNIX rules
of generation and of economy.

**Cantilever beam with tet4/tet10/hex8/hex20/hex27 struct/delaunay/etc**

Max. deflection vs. element size vs. total DOFs vs. CPU (run several
times and take average and standard deviation) vs. memory

MUMPS/GAMG, local, aws, contabo

The second way of running parametric studies is by using the internal
keyword `PARAMETRIC` that allows sweeping a numerical range of one or
more FeenoX variables.

**TODO**

  [a]: design/parametric/two-squares-triang.png "fig:"
  {#fig:two-squares-triang width="50%"}
  [b]: design/parametric/two-squares-quad.png "fig:"
  {#fig:two-squares-quad width="50%"}

## Efficiency {#sec:efficiency}

> The computational resources (i.e. costs measured in CPU/GPU time,
> random-access memory, long-term storage, etc.) needed to solve a
> problem should be comparable to other similar state-of-the-art
> finite-element tools.

Solve LE10 with FeenoX & ccx, show results, CPU & memory

Include time to pre-process ccx!

SPOOLES (ccx) vs. MUMPS (feenox)

-   auto KSP/SNES
-   `--log_view`

cloud, rent don't buy benchmark and comparisons

-   thermal
-   mechanical
-   modal

vs

-   ccx
-   sparselizard
-   elmer
-   code aster

## Scalability {#sec:scalability}

> The tool ought to be able to start solving small problems first to
> check the inputs and outputs behave as expected and then allow
> increasing the problem size up in order to achieve to the desired
> accuracy of the results. As mentioned in sec. 2, large problem should
> be split among different computers to be able to solve them using a
> finite amount of per-host computational power (RAM and CPU).

-   OpenMP in PETSc

-   Gmsh partitions

-   run something big to see how it fails

-   show RAM vs. nodes for mumps & gamg

## Flexibility

> The tool should be able to handle engineering problems involving
> different materials with potential spatial and time-dependent
> properties, such as temperature-dependent thermal expansion
> coefficients and/or non-constant densities. Boundary conditions must
> be allowed to depend on both space and time as well, like non-uniform
> pressure loads and/or transient heat fluxes.

FeenoX comes from nuclear + experience (what to do and what not to do)

Materials: a material library (perhaps included in a frontend GUI?) can
write FeenoX' material definitions. Flexiblity.

-   everything is an expression, show sophomore's identity
-   1d & 2d interpolated data for functions
-   thermal transient valve with k(T) and BCs(x,t)

## Extensibility {#sec:extensibility}

> It should be possible to add other PDE-casted problem types (such as
> the Schröedinger equation) to the tool using a reasonable amount of
> time by one or more skilled programmers. The tool should also allow
> new models (such as non-linear stress-strain constitutive
> relationships) to be added as well.

-   user-provided routines
-   skel for pdes and annotated models
-   laplace skel

## Interoperability {#sec:interoperability}

> A mean of exchanging data with other computational tools complying to
> requirements similar to the ones outlined in this document.

-   UNIX
-   POSIX
-   shmem
-   mpi
-   Gmsh
-   moustache
-   print -\> awk -\> latex tables NUREG

# Interfaces

> The tool should be able to allow remote execution without any user
> intervention after the tool is launched. To achieve this goal it is
> that the problem should be completely defined in one or more input
> files and the output should be complete and useful after the tool
> finishes its execution, as already required. The tool should be able
> to report the status of the execution (i.e. progress, errors, etc.)
> and to make this information available to the user or process that
> launched the execution, possibly from a remote location.

## Problem input {#sec:input}

> The problem should be completely defined by one or more input files.
> These input files might be
>
> -   particularly formatted files to be read by the tool in an *ad-hoc*
>     way, and/or
> -   source files for interpreted languages which can call the tool
>     through and API or equivalent method, and/or
> -   any other method that can fulfill the requirements described so
>     far.
>
> Preferably, these input files should be plain ASCII file in order to
> be tracked by distributed control version systems such as Git. If the
> tool provides an API for an interpreted language such as Python, the
> Python source used to solve a particular problem should be
> Git-friendly. It is recommended not to track revisions of mesh data
> files but of the source input files, i.e. to track the mesher's input
> and not the mesher's output. Therefore, it is recommended not to mix
> the problem definition with the problem mesh data.
>
> It is not mandatory to include a GUI in the main distribution, but the
> input/output scheme should be such that graphical pre and
> post-processing tools can create the input files and read the output
> files so as to allow third parties to develop interfaces. It is
> recommended to design the workflow as to make it possible for the
> interfaces to be accessible from mobile devices and web browsers.
>
> It is acceptable if only basic usage can be achieved through the usage
> of graphical interfaces to ease basic usage at least. Complex problems
> involving non-trivial material properties and boundary conditions
> might Notwithstanding the suggestion above, it is expected that

dar ejemplos comparar con
<https://cofea.readthedocs.io/en/latest/benchmarks/004-eliptic-membrane/tested-codes.html>

macro-friendly inputs, rule of generation

**Simple problems should need simple inputs.**

English-like input. Nouns are definitions, verbs are instructions.

**Similar problems should need similar inputs.**

thermal slab steady state and transient

1d neutron

VCS tracking, example with hello world.

API in C?

## Results output {#sec:output}

> The output ought to contain useful results and should not be cluttered
> up with non-mandatory information such as ASCII art, notices,
> explanations or copyright notices. Since the time of cognizant
> engineers is far more expensive than CPU time, output should be easily
> interpreted by either a human or, even better, by other programs or
> interfaces---especially those based in mobile and/or web platforms.
> Open-source formats and standards should be preferred over privative
> and ad-hoc formatting to encourage the possibility of using different
> workflows and/or interfaces.

JSON/YAML, state of the art open post-processing formats. Mobile &
web-friendly.

Common and preferably open-source formats.

100% user-defined output with PRINT, rule of silence rule of economy,
i.e. no RELAP yaml/json friendly outputs vtk (vtu), gmsh, frd?

90% is serial (vtk), no need to complicate due to 10%

# Quality assurance {#sec:qa}

> Since the results obtained with the tools might be used in verifying
> existing equipment or in designing new mechanical parts in sensitive
> industries, a certain level of software quality assurance is needed.
> Some best-practices for developing generic software are required such
> as
>
> -   employment of a version control system,
> -   automated testing suites,
> -   user-reported bug tracking support.
> -   etc.
>
> `\noindent `{=tex}but since the tool falls in the category of
> engineering computational software, verification and validation
> procedures are also mandatory, as discussed below. Design should be
> such that governance of engineering data including problem definition,
> results and documentation can be efficiently performed using
> state-of-the-art methodologies.
>
> **valgrind**

## Reproducibility and traceability

> The full source code and the documentation of the tool ought to be
> maintained under a control version system hosted on a public server
> accessible worldwide without needing any special credentials to get a
> copy of the code. If there is an executable binary, it should be able
> to report which version of the code the executable corresponds to. If
> there is a library callable through an API, there should be a call
> which returns the version of the code the library corresponds to.
>
> It is recommended not to mix mesh data like nodes and element
> definition with problem data like material properties and boundary
> conditions so as to ease governance and tracking of computational
> models. All the information needed to solve a particular problem
> (i.e. meshes, boundary conditions, spatially-distributed material
> properties, etc.) should be generated from a very simple set of files
> which ought to be susceptible of being tracked with current
> state-of-the-art version control systems.

simple \<-\> simple

similar \<-\> similar

## Automated testing

> A mean to automatically test the code works as expected is mandatory.
> A set of problems with known solutions should be solved with the tool
> after each modification of the code to make sure these changes still
> give the right answers for the right questions and no regressions are
> introduced. Unit software testing practices like continuous
> integration and test coverage are recommended.
>
> **static = compiler flags**
>
> **dynamic = valgrind**

make check

regressions, example of the change of a sign

## Bug reporting and tracking

> A system to allow developers and users to report errors, bugs and
> improvements should be provided. If applicable, these reports should
> be tracked, addressed and documented.
>
> **mistake != errors**
>
> mistake = bug = blunders

github

mailing listings

## Verification {#sec:verification}

> Verification, defined as
>
> > The process of determining that a model implementation accurately
> > represents the developer's conceptual description of the model and
> > the solution to the model.
>
> `\noindent`{=tex} a.k.a. checking if the tool is solving right the
> equations, should be performed before applying the code to solve any
> industrial problem. Depending on the nature and regulation of the
> industry, the verification guidelines and requirements may vary. Since
> it is expected that code verification tasks could be performed by
> arbitrary individuals or organizations, the tool's source code should
> be available to independent third parties. In this regard, changes in
> the source code should be controllable, traceable and well documented.
> Stable releases ought to be digitally signed by a responsible
> engineer.
>
> Even though the verification requirements may vary among problem
> types, industries and particular applications, a common method to
> verify the code is to compare solutions obtained with the tool with
> known exact solutions or benchmarks. It is thus mandatory to be able
> to compare the results with analytical solutions, either internally in
> the tool or through external libraries or tools. This approach is
> called the Method of Exact solutions and it is the most widespread
> scheme for verifying computational software, although it does not
> provide a comprehensive method to verify the whole spectrum of
> features. Another possibility is to follow the Method of Manufactured
> Solutions, which does. It is highly encouraged that the tool allows
> the application of MMS for software verification. Indeed, this method
> needs a full explanation of the equations solved by the tool, up to
> the point that [@sandia-mms] says that
>
> > Difficulties in determination of the governing equations arises
> > frequently with commercial software, where some information is
> > regarded as proprietary. If the governing equations cannot be
> > determined, we would question the validity of using the code.
>
> To enforce the availability of the governing equations, the tool has
> to be open source as required in sec. 1 and well documented as
> required in sec. 4.6.
>
> Whenever a verification task is performed and documented, at least one
> test should be added to the test suite. For MES or MMS, only one mesh
> refinement is enough, there is no need to add to whole test for
> different mesh sizes. The tests in the tests suite should check that
> the verified features are kept by future changes and no regressions
> that break the verification are introduced. Verifications that the
> tool fails when it is expected to fail are encouraged as much as
> positive verifications that results are the expected ones.

open source (really, not like CCX -\> mostrar ejemplo) GPLv3+ free Git +
gitlab, github, bitbucket

## Validation

> As with verification, for each industrial application of the tool
> there should be a documented procedure to perform a set of validation
> tests, defined as
>
> > The process of determining the degree to which a model is an
> > accurate representation of the real world from the perspective of
> > the intended uses of the model.
>
> `\noindent`{=tex} a.k.a. checking that the right equations are being
> solved by the tool. This procedure should be based on existing
> industry standards regarding verification and validation such as ASME,
> AIAA, IAEA, etc. There should be a procedure for each type of physical
> problem (thermal, mechanical, thermomechanical, nuclear, etc.) and for
> each problem type when a new
>
> -   geometry,
> -   mesh type,
> -   material model,
> -   boundary condition,
> -   data interpolation scheme
>
> `\noindent `{=tex}or any other particular application-dependent
> feature is needed.
>
> A report following the V&V procedure defined above should be prepared
> and signed by a responsible engineer in a case-by-case basis for each
> particular field of application of the tool. Verification can be
> performed against
>
> -   known analytical results, and/or
> -   other already-validated tools following the same standards, and/or
> -   experimental results.

already done for Fino

hip implant, 120+ pages, ASME, cases of increasing complexity

## Documentation {#sec:documentation}

> Documentation should be complete and cover both the user and the
> developer point of view. It should contain a user manual adequate for
> both reference and tutorial purposes. Other forms of simplified
> documentation such as quick reference cards or video tutorials are not
> mandatory but highly recommended. Since the tool should be extendable
> (sec. 2.6), there should be a separate development manual covering the
> programming design and implementation, explaining how to extend the
> code and how to add new features. Also, as non-trivial mathematics
> which should be verified (sec. 4.4) are expected, a thorough
> explanation of what equations are taken into account and how they are
> solved is required.
>
> It should be possible to make the full documentation available online
> in a way that it can be both printed in hard copy and accessed easily
> from a mobile device. Users modifying the tool to suit their own needs
> should be able to modify the associated documentation as well, so a
> clear notice about the licensing terms of the documentation itself
> (which might be different from the licensing terms of the source code
> itself) is mandatory.

it's not compact, but almost! Compactness is the property that a design
can fit inside a human being's head. A good practical test for
compactness is this: Does an experienced user normally need a manual? If
not, then the design (or at least the subset of it that covers normal
use) is compact. unix man page markdown + pandoc = html, pdf, texinfo

# Appendix: Rules of UNIX philosophy {#sec:unix}

Rule of Modularity

:   Developers should build a program out of simple parts connected by
    well defined interfaces, so problems are local, and parts of the
    program can be replaced in future versions to support new features.
    This rule aims to save time on debugging code that is complex, long,
    and unreadable.

:   *In FeenoX:* there are some skels that show how new problems can be
    added (i.e. replace the thermal equations with electromagnetism)

Rule of Clarity

:   Developers should write programs as if the most important
    communication is to the developer who will read and maintain the
    program, rather than the computer. This rule aims to make code as
    readable and comprehensible as possible for whoever works on the
    code in the future.

:   *In FeenoX:* we attempt to do that, not sure if we nail it...

Rule of Composition

:   Developers should write programs that can communicate easily with
    other programs. This rule aims to allow developers to break down
    projects into small, simple programs rather than overly complex
    monolithic programs.

:   *In FeenoX:* this is FeenoX' main point, to use a mesh created by
    another tool and to create files which can be post-processed by
    other tools. Other that these files which have a definite format,
    the output of the program is 100% controlled by the user so it can
    be tailored to suit any other programs' input needs.

Rule of Separation

:   Developers should separate the mechanisms of the programs from the
    policies of the programs; one method is to divide a program into a
    front-end interface and a back-end engine with which that interface
    communicates. This rule aims to prevent bug introduction by allowing
    policies to be changed with minimum likelihood of destabilizing
    operational mechanisms.

:   *In FeenoX:* this is related to the previous rule, separation is the
    basic idea of FeenoX.

Rule of Simplicity

:   Developers should design for simplicity by looking for ways to break
    up program systems into small, straightforward cooperating pieces.
    This rule aims to discourage developers' affection for writing
    "intricate and beautiful complexities" that are in reality bug prone
    programs.

:   *In FeenoX:* only way that we reflect this is the choice of C over
    C++ and over Fortran. In the former, the extra complexity is not
    needed. In the latter, we must add the complexity of C over Fortran.

Rule of Parsimony

:   Developers should avoid writing big programs. This rule aims to
    prevent overinvestment of development time in failed or suboptimal
    approaches caused by the owners of the program's reluctance to throw
    away visibly large pieces of work. Smaller programs are not only
    easier to write, optimize, and maintain; they are easier to delete
    when deprecated.

:   *In FeenoX:* we attempt to do that, not sure if we nail it...

Rule of Transparency

:   Developers should design for visibility and discoverability by
    writing in a way that their thought process can lucidly be seen by
    future developers working on the project and using input and output
    formats that make it easy to identify valid input and correct
    output. This rule aims to reduce debugging time and extend the
    lifespan of programs.

:   *In FeenoX:* we attempt to do that, not sure if we nail it...

Rule of Robustness

:   Developers should design robust programs by designing for
    transparency and discoverability, because code that is easy to
    understand is easier to stress test for unexpected conditions that
    may not be foreseeable in complex programs. This rule aims to help
    developers build robust, reliable products.

:   *In FeenoX:* we attempt to do that, not sure if we nail it...

Rule of Representation

:   Developers should choose to make data more complicated rather than
    the procedural logic of the program when faced with the choice,
    because it is easier for humans to understand complex data compared
    with complex logic. This rule aims to make programs more readable
    for any developer working on the project, which allows the program
    to be maintained.

:   *In FeenoX:* that's why we use C instead of Fortran. And in some
    way, that's also why we use C instead of C++. But it might depend on
    who the developer working on the project is and what background she
    has...

Rule of Least Surprise

:   Developers should design programs that build on top of the potential
    users' expected knowledge; for example, '+' in a calculator program
    should always mean 'addition'. This rule aims to encourage
    developers to build intuitive products that are easy to use.

:   *In FeenoX:* a '+' in FeenoX means addition, etc. The input file
    looks like a near-English text that actually works like a
    syntactically-sugared set of instructions that teels FeenoX how to
    solve a problem.

Rule of Silence

:   Developers should design programs so that they do not print
    unnecessary output. This rule aims to allow other programs and
    developers to pick out the information they need from a program's
    output without having to parse verbosity.

:   *In FeenoX:* if there are not explicit `PRINT` instructions, FeenoX
    does not write anythin to the standard output. Actually, without any
    explicit `WRITE` instruction, no output files would be written
    either.

Rule of Repair

:   Developers should design programs that fail in a manner that is easy
    to localize and diagnose or in other words "fail noisily". This rule
    aims to prevent incorrect output from a program from becoming an
    input and corrupting the output of other code undetected.

:   *In FeenoX:* input errors are detected before the computation is
    started and run-time errors (i.e. a division by zero) con be user
    controled, they can be fatal or ignored.

Rule of Economy

:   Developers should value developer time over machine time, because
    machine cycles today are relatively inexpensive compared to prices
    in the 1970s. This rule aims to reduce development costs of
    projects.

:   *In FeenoX:* related to rule of silence, FeenoX will write only what
    the user asks for in order to save her from parsing through tons of
    undesired data. The approach of "compute and write everything you
    can in one single run" made sense in 1970 where CPU time was
    expensive, but not anymore.

Rule of Generation

:   Developers should avoid writing code by hand and instead write
    abstract high-level programs that generate code. This rule aims to
    reduce human errors and save time.

:   *In FeenoX:* not so much for code but definitely for documentation.

Rule of Optimization

:   Developers should prototype software before polishing it. This rule
    aims to prevent developers from spending too much time for marginal
    gains.

:   *In FeenoX:* we are still building, we'll optimize later.

Rule of Diversity

:   Developers should design their programs to be flexible and open.
    This rule aims to make programs flexible, allowing them to be used
    in ways other than those their developers intended.

:   *In FeenoX:* unexpected usage happens more than often. Flexiblity is
    a key cornerstone.

Rule of Extensibility

:   Developers should design for the future by making their protocols
    extensible, allowing for easy plugins without modification to the
    program's architecture by other developers, noting the version of
    the program, and more. This rule aims to extend the lifespan and
    enhance the utility of the code the developer writes.

:   *In FeenoX:* open source code and GPL license encourages extensions.
