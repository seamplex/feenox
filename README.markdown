# FeenoX: A cloud-first free no-fee no-X uniX-like finite-element(ish) computational engineering tool

- [<span class="toc-section-number">1</span> About FeenoX]
  - [<span class="toc-section-number">1.1</span> Extents]
    - [<span class="toc-section-number">1.1.1</span> Examples]
    - [<span class="toc-section-number">1.1.2</span> Tutorials]
    - [<span class="toc-section-number">1.1.3</span> Tests]
  - [<span class="toc-section-number">1.2</span> Capabilities]
  - [<span class="toc-section-number">1.3</span> Usefulness]
- [<span class="toc-section-number">2</span> Documentation]
- [<span class="toc-section-number">3</span> Quickstart]
  - [<span class="toc-section-number">3.1</span> Install from apt]
  - [<span class="toc-section-number">3.2</span> Download]
  - [<span class="toc-section-number">3.3</span> Git repository]
- [<span class="toc-section-number">4</span> Licensing]
  - [<span class="toc-section-number">4.1</span> Contributing]
- [<span class="toc-section-number">5</span> Further information]

[![DOI]][1]

  [<span class="toc-section-number">1</span> About FeenoX]: #about-feenox
  [<span class="toc-section-number">1.1</span> Extents]: #extents
  [<span class="toc-section-number">1.1.1</span> Examples]: #examples
  [<span class="toc-section-number">1.1.2</span> Tutorials]: #tutorials
  [<span class="toc-section-number">1.1.3</span> Tests]: #tests
  [<span class="toc-section-number">1.2</span> Capabilities]: #capabilities
  [<span class="toc-section-number">1.3</span> Usefulness]: #usefulness
  [<span class="toc-section-number">2</span> Documentation]: #documentation
  [<span class="toc-section-number">3</span> Quickstart]: #quickstart
  [<span class="toc-section-number">3.1</span> Install from apt]: #install-from-apt
  [<span class="toc-section-number">3.2</span> Download]: #download
  [<span class="toc-section-number">3.3</span> Git repository]: #git-repository
  [<span class="toc-section-number">4</span> Licensing]: #licensing
  [<span class="toc-section-number">4.1</span> Contributing]: #sec:contributing
  [<span class="toc-section-number">5</span> Further information]: #further-information
  [DOI]: https://joss.theoj.org/papers/10.21105/joss.05846/status.svg
  [1]: https://doi.org/10.21105/joss.05846

# About FeenoX

[FeenoX] is a [cloud-first][] [free and open source] tool to solve
[engineering-related problems] using a computer (or [many computers in
parallel]) with a [particular design basis].

Consider the following input file:

``` feenox
PROBLEM mechanical MESH box.msh

T(x,y,z) = sqrt(x^2 + y^2) + z   # non-trivial temperature distribution
E = 200e3 * (1-T(x,y,z)/300)     # temperature-dependent Young modulus
nu = 0.3

BC left fixed           # left face fully fixed
BC top  p=1e4*(1+x)     # top face with a triapezoidal load

WRITE_RESULTS FORMAT vtu
```

Note that

- The [input file] is made of [simple][] [self-descriptive English][]
  [keywords]
- [Material properties and boundary conditions] can be [expressions of
  $`x`$, $`y`$ and $`z`$]
- Input mesh is is [Gmsh’s format `.msh`]
- Output results are in [Paraview’s `.vtk`/`.vtu`
  formats][Gmsh’s format `.msh`]

Each of these items has a lot of design and usage implications,
thoroughly discussed in the [Software Design Specification], ranging
from integrating FeenoX as a back end for different front ends with a
reasonably low effort (e.g. [SunCAE]), down to suitability for
interaction with Large Language Models. As a reference, the reader is
encouraged to compare the above input syntax with any other FEA solver
and then consider how to ask ChatGPT & friends for help to create an
input file from scratch. Or how to write a Graphical User Interface that
would need to create something the solver can read. Plus, it can be
installed with

``` terminal
apt install feenox
```

------------------------------------------------------------------------

Choose your background for further details about the what, how and whys:

- [Industry Engineer]
- [Unix Hacker]
- [Academic Professor]

See the following paper for a brief summary (and to [cite FeenoX in your
work]):

- Theler J, “FeenoX: a cloud-first finite-element(ish) computational
  engineering tool,” *Journal of Open Source Software*, vol. 9, no. 95,
  p. 5846, Mar. 2024, doi: [10.21105/joss.05846].

> ### News
>
> - FeenoX is now in Debian (sid) and Ubuntu 25.04!  
>   You can install FeenoX from APT
>
>   ``` terminal
>   sudo apt install feenox
>   ```
>
>   Check progress on the following pages:
>
>   - <https://packages.debian.org/unstable/science/feenox>
>   - <https://launchpad.net/ubuntu/+source/feenox>
>
> - Open-source web-based UX for FeenoX.  
>   Take a look at [SunCAE] for an example of how to write a front end
>   for FeenoX.
>
> - IB students  
>   Remember that your first read should be the 0-th tutorial, [Setting
>   up your workspace]

  [FeenoX]: https://seamplex.com/feenox
  [cloud-first]: https://seamplex.com/feenox/doc/sds.html#cloud-first
  [free and open source]: #licensing
  [engineering-related problems]: #extents
  [many computers in parallel]: https://www.seamplex.com/feenox/doc/sds.html#sec:scalability
  [particular design basis]: https://seamplex.com/feenox/doc/sds.html
  [input file]: https://www.seamplex.com/feenox/doc/sds.html#sec:input
  [simple]: https://www.seamplex.com/feenox/doc/sds.html#sec:simple
  [self-descriptive English]: https://www.seamplex.com/feenox/doc/sds.html#sec:sugar
  [keywords]: https://www.seamplex.com/feenox/doc/sds.html#sec:nouns_verbs
  [Material properties and boundary conditions]: https://www.seamplex.com/feenox/doc/sds.html#sec:flexibility
  [expressions of $`x`$, $`y`$ and $`z`$]: https://www.seamplex.com/feenox/doc/sds.html#sec:expression
  [Gmsh’s format `.msh`]: https://www.seamplex.com/feenox/doc/sds.html#sec:interoperability
  [Software Design Specification]: https://www.seamplex.com/feenox/doc/sds.html
  [SunCAE]: https://www.seamplex.com/suncae
  [Industry Engineer]: README4engineers.markdown
  [Unix Hacker]: README4hackers.markdown
  [Academic Professor]: README4academics.markdown
  [cite FeenoX in your work]: doc/FAQ.markdown#how-should-i-cite-feenox
  [10.21105/joss.05846]: https://doi.org/10.21105/joss.05846
  [Setting up your workspace]: https://www.seamplex.com/feenox/doc/tutorials/000-setup/

## Extents

### Examples

- [Basic mathematics]
- [Systems of ODEs/DAEs]
- [Laplace’s equation]
- [Heat conduction]
- [Linear elasticity]
- [Modal analysis]
- [Neutron diffusion]
- [Neutron S<sub>N</sub>]

  [Basic mathematics]: https://seamplex.com/feenox/examples/basic.html
  [Systems of ODEs/DAEs]: https://seamplex.com/feenox/examples/daes.html
  [Laplace’s equation]: https://seamplex.com/feenox/examples/laplace.html
  [Heat conduction]: https://seamplex.com/feenox/examples/thermal.html
  [Linear elasticity]: https://seamplex.com/feenox/examples/mechanical.html
  [Modal analysis]: https://seamplex.com/feenox/examples/modal.html
  [Neutron diffusion]: https://seamplex.com/feenox/examples/neutron_diffusion.html
  [Neutron S<sub>N</sub>]: https://seamplex.com/feenox/examples/neutron_sn.html

### Tutorials

0.  [Setting up your workspace][2]
1.  [Overview: the tensile test case]
2.  [Fun & games: solving mazes with PDES instead of AI]
3.  [Heat conduction][3]

  [2]: https://www.seamplex.com/feenox/doc/tutorials/000-setup
  [Overview: the tensile test case]: https://www.seamplex.com/feenox/doc/tutorials/110-tensile-test
  [Fun & games: solving mazes with PDES instead of AI]: https://www.seamplex.com/feenox/doc/tutorials/120-mazes
  [3]: https://www.seamplex.com/feenox/doc/tutorials/320-thermal

### Tests

The [`tests`] directory in the repository has hundreds of

- `grep`-able examples
- [unit and regression tests],
- (crude) mathematical & code verification tests (as in “are we solving
  right the equations?”),
- subdirectories with further case studies
  - [`mms`]
    - [`thermal`]
      - [`2d`]
      - [`3d`]
    - [`mechanical`]
      - [`sdef`]
      - [`ldef`]
  - [`nafems`]
    - [`le10`]

  [`tests`]: https://github.com/seamplex/feenox/tree/main/tests
  [unit and regression tests]: https://seamplex.com/feenox/doc/compilation.html#sec:test-suite
  [`mms`]: https://github.com/seamplex/feenox/tree/main/tests/mms
  [`thermal`]: https://github.com/seamplex/feenox/tree/main/tests/mms/thermal
  [`2d`]: https://github.com/seamplex/feenox/tree/main/tests/mms/thermal/2d
  [`3d`]: https://github.com/seamplex/feenox/tree/main/tests/mms/thermal/3d
  [`mechanical`]: https://github.com/seamplex/feenox/tree/main/tests/mms/mechanical
  [`sdef`]: https://github.com/seamplex/feenox/tree/main/tests/mms/thermal/sdef
  [`ldef`]: https://github.com/seamplex/feenox/tree/main/tests/mms/thermal/ldef
  [`nafems`]: https://github.com/seamplex/feenox/tree/main/tests/nafems
  [`le10`]: https://github.com/seamplex/feenox/tree/main/tests/nafems/le10

## Capabilities

- Both *free* as in “free speech” and in “free beer”
- The [problem] to solve is defined through a [syntactically-sugared][]
  [self-descriptive English-like plain-text input file] that should
  [resemble the original human-friendly problem formulation] as much as
  possible:
  - [no need to recompile if the problem changes] (FeenoX is a
    *program*, not a library)
  - [nouns are definitions and verbs are instructions]
  - [simple problems need simple inputs]
  - [everything is an expression]
  - [100%-defined user output] (no [`PRINT`][problem] nor
    [`WRITE_RESULTS`] instructions, no output)
- [Cloud-first design] (cloud friendliness is not enough)
- [MPI parallelization]
- Leverages high-quality well-established free and open source libraries
  to solve…
  - [general mathematical problems] using [GNU GSL]
  - [sets of ODEs/DAEs] using [SUNDIALS]
  - [PDEs] formulated with the [finite element method]
    - reads mesh in [Gmsh] format
    - uses [PETSc]/[SLEPc] to solve
      - linear systems ([KSP])
      - non-linear systems ([SNES])
      - time-dependent systems ([TS])
      - generalized eigen-value problems ([EPS])
    - writes results in Gmsh’s [.msh] format and/or [Paraview]’s
      [VTU/VTK] formats
- Focuses on [flexibility], especially when defining [non-uniform
  multi-solid material properties from ASME tables]
- Follows the [Unix programming philosophy]
  - [rule of separation]
  - [rule of silence]
  - [rule of economy]
- Each PDE (i.e. from Laplace downward in the list of examples) [is
  implemented in a subdirectory] within [`src/pde`][PDEs] of the source
  tree
  - any subdirectory can be removed if a particular PDE is not needed
  - any subdirectory can be used as a template to add a new PDE to the
    capabilities
- [Space], [time] and/or solution-dependent [material properties] and
  [boundary conditions]
  - [algebraic expressions], and/or
  - [point-wise interpolated data]
- [Command-line argument expansion] for
  - [increased flexibility],
  - [parametric sweeps], and/or
  - [optimization loops]
- [Steady-state], \[quasi-static\] and/or [transient problems]
- [Linear] and [non-linear] problems
- Possibility to verify the code using the [Method of Manufactured
  Solutions]
- Separate [repository to profile and study code performance] using
  [Google’s benchmark library]

  [problem]: https://www.seamplex.com/feenox/doc/feenox-manual.html#problem
  [syntactically-sugared]: https://seamplex.com/feenox/doc/sds.html#sec:syntactic
  [self-descriptive English-like plain-text input file]: https://seamplex.com/feenox/doc/sds.html#sec:input
  [resemble the original human-friendly problem formulation]: https://seamplex.com/feenox/doc/sds.html#sec:matching-formulations
  [no need to recompile if the problem changes]: https://seamplex.com/feenox/doc/sds.html#sec:introduction
  [nouns are definitions and verbs are instructions]: https://seamplex.com/feenox/doc/sds.html#sec:nouns_verbs
  [simple problems need simple inputs]: https://seamplex.com/feenox/doc/sds.html#sec:simple
  [everything is an expression]: https://seamplex.com/feenox/doc/sds.html#sec:expression
  [100%-defined user output]: https://seamplex.com/feenox/doc/sds.html#sec:output
  [`WRITE_RESULTS`]: https://www.seamplex.com/feenox/doc/feenox-manual.html#write_results
  [Cloud-first design]: https://seamplex.com/feenox/doc/sds.html#cloud-first
  [MPI parallelization]: https://seamplex.com/feenox/doc/sds.html#sec:scalability
  [general mathematical problems]: https://seamplex.com/feenox/examples/basic.html
  [GNU GSL]: https://www.gnu.org/software/gsl/
  [sets of ODEs/DAEs]: https://seamplex.com/feenox/examples/daes.html
  [SUNDIALS]: https://computing.llnl.gov/projects/sundials
  [PDEs]: https://github.com/seamplex/feenox/tree/main/src/pdes
  [finite element method]: https://en.wikipedia.org/wiki/Finite_element_method
  [Gmsh]: http://gmsh.info/
  [PETSc]: https://petsc.org/release/
  [SLEPc]: https://slepc.upv.es/
  [KSP]: https://petsc.org/release/manual/ksp/
  [SNES]: https://petsc.org/release/manual/snes/
  [TS]: https://petsc.org/release/manual/ts/
  [EPS]: https://slepc.upv.es/documentation/current/docs/manualpages/EPS/index.html
  [.msh]: http://gmsh.info/doc/texinfo/gmsh.html#MSH-file-format
  [Paraview]: https://www.paraview.org/
  [VTU/VTK]: https://docs.vtk.org/en/latest/design_documents/VTKFileFormats.html
  [flexibility]: https://seamplex.com/feenox/doc/sds.html#sec:flexibility
  [non-uniform multi-solid material properties from ASME tables]: https://www.seamplex.com/feenox/doc/tutorials/320-thermal/#from-a-steady-state
  [Unix programming philosophy]: https://seamplex.com/feenox/doc/sds.html#sec:unix
  [rule of separation]: https://seamplex.com/feenox/doc/sds.html#sec:separation
  [rule of silence]: https://seamplex.com/feenox/doc/sds.html#sec:silence
  [rule of economy]: https://seamplex.com/feenox/doc/sds.html#sec:economy
  [is implemented in a subdirectory]: https://seamplex.com/feenox/doc/sds.html#sec:extensibility
  [Space]: https://github.com/seamplex/feenox/blob/main/tests/thermal-two-squares-material-explicit-space.fee
  [time]: https://github.com/seamplex/feenox/blob/main/tests/nafems-t3-1d.fee
  [material properties]: https://github.com/seamplex/feenox/blob/main/tests/wilson-2d.fee
  [boundary conditions]: https://github.com/seamplex/feenox/blob/main/tests/radiation-as-convection-celsius.fee
  [algebraic expressions]: https://seamplex.com/feenox/examples/mechanical.html#parallelepiped-whose-youngs-modulus-is-a-function-of-the-temperature
  [point-wise interpolated data]: https://seamplex.com/feenox/examples/mechanical.html#temperature-dependent-material-properties
  [Command-line argument expansion]: https://seamplex.com/feenox/doc/sds.html#sec:run-time-arguments
  [increased flexibility]: https://seamplex.com/feenox/examples/basic.html#computing-the-derivative-of-a-function-as-a-unix-filter
  [parametric sweeps]: https://seamplex.com/feenox/doc/sds.html#sec:parametric
  [optimization loops]: https://seamplex.com/feenox/doc/sds.html#optimization-loops
  [Steady-state]: https://www.seamplex.com/feenox/doc/tutorials/120-mazes/#solving-the-steady-state-laplace-equation
  [transient problems]: https://www.seamplex.com/feenox/doc/tutorials/320-thermal/#sec:transient
  [Linear]: https://www.seamplex.com/feenox/doc/tutorials/320-thermal/#linear-steady-state-problems
  [non-linear]: https://www.seamplex.com/feenox/doc/tutorials/320-thermal/#non-linear-state-state-problems
  [Method of Manufactured Solutions]: https://github.com/seamplex/feenox/tree/main/tests/mms
  [repository to profile and study code performance]: https://github.com/seamplex/feenox-benchmark
  [Google’s benchmark library]: https://github.com/google/benchmark

## Usefulness

- [CAEplex][]: a [web-based thermo-mechanical solver running on the
  cloud]
- [Non-conformal mesh mapping]
- [ASME stress linearization for pressurized pipes and vessels]
- [Assessment of material properties from tabulated sources]
- [Environmentally-assisted fatigue analysis in dissimilar interfaces of
  nuclear pipes]
- [Neutron transport in the cloud]
- [Solving mazes without AI]
- [Parametric NAFEMS LE10 benchmark: comparison of resource consumption
  for different FEA programs]
- [Some Youtube videos]

  [CAEplex]: https://www.caeplex.com
  [web-based thermo-mechanical solver running on the cloud]: https://www.youtube.com/watch?v=DOnoXo_MCZg
  [Non-conformal mesh mapping]: https://github.com/gtheler/feenox-non-conformal-mesh-interpolation
  [ASME stress linearization for pressurized pipes and vessels]: https://github.com/seamplex/pipe-linearize
  [Assessment of material properties from tabulated sources]: https://seamplex.com/feenox/examples/basic.html#on-the-evaluation-of-thermal-expansion-coefficients
  [Environmentally-assisted fatigue analysis in dissimilar interfaces of nuclear pipes]:
    https://github.com/seamplex/piping-asme-fatigue
  [Neutron transport in the cloud]: https://seamplex.com/thesis/html/front/abstract.html
  [Solving mazes without AI]: https://www.linkedin.com/feed/update/urn:li:activity:6973982270852325376/
  [Parametric NAFEMS LE10 benchmark: comparison of resource consumption for different FEA programs]:
    https://seamplex.com/feenox/tests/nafems/le10/
  [Some Youtube videos]: https://seamplex.com/feenox/doc/#sec:youtube

# Documentation

Browse through the [documentation index] and/or the [`doc`] subdirectory
of the [Github repository] for

- [FAQs], including [what FeenoX means]
- [Manual]
- [Description]
- [Software Design Requirements]
- [Software Design Specification][4]
- [Unix man page] (accessible through `man feenox` after installation)
- [History]
- [Compilation guide]
- [Programming guide]

  [documentation index]: doc
  [`doc`]: https://github.com/seamplex/feenox/tree/main/doc
  [Github repository]: https://github.com/seamplex/feenox/
  [FAQs]: doc/FAQ.markdown
  [what FeenoX means]: https://seamplex.com/feenox/doc/FAQ.html#what-does-feenox-mean
  [Manual]: https://seamplex.com/feenox/doc/feenox-manual.html
  [Description]: https://www.seamplex.com/feenox/doc/feenox-desc.html
  [Software Design Requirements]: https://seamplex.com/feenox/doc/srs.html
  [4]: https://seamplex.com/feenox/doc/sds.html
  [Unix man page]: https://seamplex.com/feenox/doc/feenox.1.html
  [History]: doc/history.markdown
  [Compilation guide]: doc/compilation.markdown
  [Programming guide]: doc/programming.markdown

# Quickstart

## Install from apt

If you use [Debian] (either [Sid], or [Trixie] with [backports]) or
[Ubuntu 25.04] or higher, you can do

``` terminal
sudo apt install feenox
```

See these links for details about the packages:

- <https://packages.debian.org/unstable/science/feenox>
- <https://launchpad.net/ubuntu/+source/feenox>

  [Debian]: https://www.debian.org
  [Sid]: https://wiki.debian.org/DebianUnstable
  [Trixie]: https://www.debian.org/releases/trixie
  [backports]: https://backports.debian.org
  [Ubuntu 25.04]: https://releases.ubuntu.com/plucky

## Download

|                    |                                                       |
|--------------------|-------------------------------------------------------|
| Debian package     | <https://packages.debian.org/unstable/science/feenox> |
| Ubuntu package     | <https://launchpad.net/ubuntu/+source/feenox>         |
| GNU/Linux binaries | <https://www.seamplex.com/feenox/dist/linux>          |
| Source tarballs    | <https://www.seamplex.com/feenox/dist/src>            |
| Github repository  | <https://github.com/seamplex/feenox/>                 |

Generic GNU/Linux binaries are provided as statically-linked executables
for convenience. They do not support MUMPS nor MPI and have only basic
optimization flags. Please compile from source for high-end
applications. See [detailed compilation instructions].

> - Be aware that FeenoX **does not have a GUI**. Read the
>   [documentation], especially the [description] and the [FAQs][5]. Ask
>   for help on the [GitHub discussions page] if you do now understand
>   what this means.
>
> - You can still use FeenoX through a **web-based UI** through
>   [SunCAE].

  [detailed compilation instructions]: docompilation.markdown
  [documentation]: https://seamplex.com/feenox/doc/
  [description]: https://www.seamplex.com/feenox/doc/feenox-desc.html
  [5]: https://seamplex.com/feenox/doc/FAQ.html
  [GitHub discussions page]: https://github.com/seamplex/feenox/discussions
  [SunCAE]: https://www.seamplex.com/suncae

## Git repository

The Git repository has the latest sources repository. To compile,
proceed as follows. If something goes wrong and you get an error, do not
hesitate to ask in FeenoX’s [discussion page].

> If you do not have Git or Autotools, download a [source tarball] and
> proceed with the usual `configure` & `make` procedure. See [these
> instructions].

1.  Install mandatory dependencies

    ``` terminal
    sudo apt-get update
    sudo apt-get install git build-essential make automake autoconf libgsl-dev
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

4.  Bootstrap, configure, compile & make

    ``` terminal
    cd feenox
    ./autogen.sh
    ./configure
    make -j4
    ```

    If you cannot (or do not want to) use `libgsl-dev` from a package
    repository, call `configure` with `--enable-download-gsl`:

    ``` terminal
    ./configure --enable-download-gsl
    ```

    If you do not have Internet access, get the tarball manually, copy
    it to the same directory as `configure` and run again. See the
    [detailed compilation instructions][6] for an explanation.

5.  Run test suite (optional)

    ``` terminal
    make check
    ```

6.  Install the binary system wide (optional)

    ``` terminal
    sudo make install
    ```

    > If you do not have root permissions, configure with your home
    > directory as prefix and then make install as a regular user:
    >
    > ``` terminal
    > ./configure --prefix=$HOME
    > make
    > make install
    > ```

To stay up to date, pull and then `autogen`, `configure` and `make` (and
optionally install):

``` terminal
git pull
./autogen.sh
./configure
make -j4
sudo make install
```

See the [download page] and the [compilation guide] for detailed
information.

  [discussion page]: https://github.com/seamplex/feenox/discussions
  [source tarball]: https://seamplex.com/feenox/dist/src/
  [these instructions]: dosource.markdown
  [6]: compilation.md
  [download page]: https://seamplex.com/feenox/download.html
  [compilation guide]: doc/compilation.markdown

# Licensing

FeenoX is distributed under the terms of the [GNU General Public
License] version 3 or (at your option) any later version. The following
text was borrowed from the [Gmsh documentation]. Replacing “Gmsh” with
“FeenoX” (using Unix’s `sed`) gives:

> FeenoX is “free software”; this means that everyone is free to use it
> and to redistribute it on a free basis. FeenoX is not in the public
> domain; it is copyrighted and there are restrictions on its
> distribution, but these restrictions are designed to permit everything
> that a good cooperating citizen would want to do. What is not allowed
> is to try to prevent others from further sharing any version of FeenoX
> that they might get from you.
>
> Specifically, we want to make sure that you have the right to give
> away copies of FeenoX, that you receive source code or else can get it
> if you want it, that you can change FeenoX or use pieces of FeenoX in
> new free programs, and that you know you can do these things.
>
> To make sure that everyone has such rights, we have to forbid you to
> deprive anyone else of these rights. For example, if you distribute
> copies of FeenoX, you must give the recipients all the rights that you
> have. You must make sure that they, too, receive or can get the source
> code. And you must tell them their rights.
>
> Also, for our own protection, we must make certain that everyone finds
> out that there is no warranty for FeenoX. If FeenoX is modified by
> someone else and passed on, we want their recipients to know that what
> they have is not what we distributed, so that any problems introduced
> by others will not reflect on our reputation.
>
> The precise conditions of the license for FeenoX are found in the
> [General Public License] that accompanies the source code. Further
> information about this license is available from the GNU Project
> webpage <http://www.gnu.org/copyleft/gpl-faq.html>.

FeenoX is licensed under the terms of the [GNU General Public
License][7] version 3 or, at the user convenience, any later version.
This means that users get the four essential freedoms:[^1]

0.  The freedom to *run* the program as they wish, for *any* purpose.
1.  The freedom to *study* how the program works, and *change* it so it
    does their computing as they wish.
2.  The freedom to *redistribute* copies so they can help others.
3.  The freedom to *distribute* copies of their *modified* versions to
    others.

So a free program has to be open source, but it also has to explicitly
provide the four freedoms above both through the written license and
through appropriate mechanisms to get, modify, compile, run and document
these modifications using well-established and/or reasonable
straightforward procedures. That is why licensing FeenoX as GPLv3+ also
implies that the source code and all the scripts and makefiles needed to
compile and run it are available for anyone that requires it (i.e. it is
compiled with `./configure && make`). Anyone wanting to modify the
program either to fix bugs, improve it or add new features is free to do
so. And if they do not know how to program, the have the freedom to hire
a programmer to do it without needing to ask permission to the original
authors. Even more, [the documentation] is released under the terms of
the [Creative Commons Attribution-ShareAlike 4.0 International License]
so these new (or modified) features can be properly documented as well.

Nevertheless, since these original authors are the copyright holders,
they still can use it to either enforce or prevent further actions from
the users that receive FeenoX under the GPLv3+. In particular, the
license allows re-distribution of modified versions only if

1.  they are clearly marked as different from the original, and
2.  they are distributed under the same terms of the GPLv3+.

There are also some other subtle technicalities that need not be
discussed here such as

- what constitutes a modified version (which cannot be redistributed
  under a different license)
- what is an aggregate (in which each part be distributed under
  different licenses)
- usage over a network and the possibility of using [AGPL] instead of
  GPL to further enforce freedom

These issues are already taken into account in the FeenoX licensing
scheme.

It should be noted that not only is FeenoX free and open source, but
also all of the libraries it depends on (and their dependencies) also
are. It can also be compiled using free and open source build tool
chains running over free and open source operating systems.

[^1]:  There are some examples of pieces of computational software which
    are described as “open source” in which even the first of the four
    freedoms is denied. The most iconic case is that of Android, whose
    sources are readily available online but there is no straightforward
    way of updating one’s mobile phone firmware with a customized
    version, not to mention vendor and hardware lock ins and the
    possibility of bricking devices if something unexpected happens. In
    the nuclear industry, it is the case of a Monte Carlo
    particle-transport program that requests users to sign an agreement
    about the objective of its usage before allowing its execution. The
    software itself might be open source because the source code is
    provided after signing the agreement, but it is not free (as in
    freedom) at all.

  [GNU General Public License]: http://www.gnu.org/copyleft/gpl.html
  [Gmsh documentation]: http://gmsh.info/doc/texinfo/gmsh.html#Copying-conditions
  [General Public License]: https://github.com/seamplex/feenox/blob/master/COPYING
  [7]: https://www.gnu.org/licenses/gpl-3.0
  [the documentation]: https://seamplex.com/feenox/doc/
  [Creative Commons Attribution-ShareAlike 4.0 International License]: https://creativecommons.org/licenses/by-sa/4.0/
  [AGPL]: https://en.wikipedia.org/wiki/GNU_Affero_General_Public_License

## Contributing

Contributions from [hackers] and/or [academics] are welcome, especially
new types of PDEs and new formulations of existing PDEs. For elliptic
operators feel free to use the Laplace equation at [`src/pdes/laplace`]
as a template.

1.  Read the [Programming Guide][8].
2.  Browse [Github discussions] and open a new thread explaining what
    you want to do and/or asking for help.
3.  Fork the [Git repository] under your Github account
4.  Create a pull request, including
    - code,
    - documentation, and
    - tests.
5.  Follow up the review procedure.

Note that

- It is mandatory to observe the [Code of Conduct].
- The contributed code has to be compatible with the [GPLv3+ license].
- Each author keeps the copyright of the contribution.
- You can [ask][Github discussions]!

  [hackers]: README4hackers.html
  [academics]: README4academics.html
  [`src/pdes/laplace`]: https://github.com/seamplex/feenox/tree/main/src/pdes/laplace
  [8]: https://seamplex.com/feenox/doc/programming.html
  [Github discussions]: https://github.com/seamplex/feenox/discussions
  [Git repository]: https://github.com/seamplex/feenox/
  [Code of Conduct]: https://seamplex.com/feenox/doc/CODE_OF_CONDUCT.html
  [GPLv3+ license]: https://www.seamplex.com/feenox/#sec:licensing

# Further information

Home page: <https://www.seamplex.com/feenox>  
Repository: <https://github.com/seamplex/feenox>  
Bug reporting: <https://github.com/seamplex/feenox/issues>  
Discussions: <https://github.com/seamplex/feenox/discussions>  
Follow us: [YouTube][] [LinkedIn][] [Github]

------------------------------------------------------------------------

FeenoX is licensed under [GNU GPL version 3] or (at your option) any
later version.  
FeenoX is free software: you are free to change and redistribute it.  
There is **NO WARRANTY**, to the extent permitted by law.

  [YouTube]: https://www.youtube.com/channel/UCC6SzVLxO8h6j5rLlfCQPhA
  [LinkedIn]: https://www.linkedin.com/company/seamplex/
  [Github]: https://github.com/seamplex
  [GNU GPL version 3]: http://www.gnu.org/copyleft/gpl.html
