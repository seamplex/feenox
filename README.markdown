# FeenoX: A cloud-first free no-fee no-X uniX-like finite-element(ish) computational engineering tool

- [<span class="toc-section-number">1</span> About FeenoX][]
  - [<span class="toc-section-number">1.1</span> Extents][]
    - [<span class="toc-section-number">1.1.1</span> Examples][]
    - [<span class="toc-section-number">1.1.2</span> Tutorials][]
    - [<span class="toc-section-number">1.1.3</span> Tests][]
  - [<span class="toc-section-number">1.2</span> Capabilities][]
  - [<span class="toc-section-number">1.3</span> Usefulness][]
- [<span class="toc-section-number">2</span> Documentation][]
- [<span class="toc-section-number">3</span> Quickstart][]
  - [<span class="toc-section-number">3.1</span> Download][]
  - [<span class="toc-section-number">3.2</span> Git repository][]
- [<span class="toc-section-number">4</span> Licensing][]
  - [<span class="toc-section-number">4.1</span> Contributing][]
- [<span class="toc-section-number">5</span> Further information][]

  [<span class="toc-section-number">1</span> About FeenoX]: #about-feenox
  [<span class="toc-section-number">1.1</span> Extents]: #extents
  [<span class="toc-section-number">1.1.1</span> Examples]: #examples
  [<span class="toc-section-number">1.1.2</span> Tutorials]: #tutorials
  [<span class="toc-section-number">1.1.3</span> Tests]: #tests
  [<span class="toc-section-number">1.2</span> Capabilities]: #capabilities
  [<span class="toc-section-number">1.3</span> Usefulness]: #usefulness
  [<span class="toc-section-number">2</span> Documentation]: #documentation
  [<span class="toc-section-number">3</span> Quickstart]: #quickstart
  [<span class="toc-section-number">3.1</span> Download]: #download
  [<span class="toc-section-number">3.2</span> Git repository]: #git-repository
  [<span class="toc-section-number">4</span> Licensing]: #licensing
  [<span class="toc-section-number">4.1</span> Contributing]: #sec:contributing
  [<span class="toc-section-number">5</span> Further information]: #further-information

# About FeenoX

[FeenoX][] is a [cloud-first][] [free and open source][] tool to solve
[engineering-related problems][] using a computer (or many computers in
parallel) with a [particular design basis][].

Choose your background for further details about the what, how and whys:

- [Industry Engineer][]
- [Unix Hacker][]
- [Academic Professor][]

  [FeenoX]: https://seamplex.com/feenox
  [cloud-first]: doc/sds.markdown#cloud-first
  [free and open source]: #licensing
  [engineering-related problems]: #extents
  [particular design basis]: doc/srs.markdown
  [Industry Engineer]: README4engineers.markdown
  [Unix Hacker]: README4hackers.markdown
  [Academic Professor]: README4academics.markdown

## Extents

### Examples

- [Basic mathematics][]
- [Systems of ODEs/DAEs][]
- [Laplace’s equation][]
- [Heat conduction][]
- [Linear elasticity][]
- [Modal analysis][]
- [Neutron diffusion][]
- [Neutron S<sub>N</sub>][]

  [Basic mathematics]: examples/basic.markdown
  [Systems of ODEs/DAEs]: https://seamplex.com/feenox/examples/daes.html
  [Laplace’s equation]: https://seamplex.com/feenox/examples/laplace.html
  [Heat conduction]: https://seamplex.com/feenox/examples/thermal.html
  [Linear elasticity]: https://seamplex.com/feenox/examples/mechanical.html
  [Modal analysis]: https://seamplex.com/feenox/examples/modal.html
  [Neutron diffusion]: https://seamplex.com/feenox/examples/neutron_diffusion.html
  [Neutron S<sub>N</sub>]: https://seamplex.com/feenox/examples/neutron_sn.html

### Tutorials

0.  [Setting up your workspace][]
1.  [Overview: the tensile test case][]
2.  [Fun & games: solving mazes with PDES instead of AI][]
3.  [Heat conduction][1]

  [Setting up your workspace]: https://www.seamplex.com/feenox/doc/tutorials/000-setup
  [Overview: the tensile test case]: https://www.seamplex.com/feenox/doc/tutorials/110-tensile-test
  [Fun & games: solving mazes with PDES instead of AI]: https://www.seamplex.com/feenox/doc/tutorials/120-mazes
  [1]: https://www.seamplex.com/feenox/doc/tutorials/320-thermal

### Tests

The [`tests`][] directory in the repository has hundreds of

- examples,
- unit tests,
- regression tests, and/or
- (crude) mathematical verification tests.

  [`tests`]: https://github.com/seamplex/feenox/tree/main/tests

## Capabilities

- The [problem][] to solve is defined through a
  [syntactically-sugared][] self-descriptive English-like plain-text
  [input file][] that should [resemble the original human-friendly
  problem formulation][] as much as possible
  - [no need to recompile if the problem changes][] (FeenoX is a
    *program*, not a library)
  - [nouns are definitions and verbs are instructions][]
  - [simple problems need simple inputs][]
  - [everything is an expression][]
  - 100%-defined user output (no print or write instructions, no output)

  <!-- -->

                                   +------------+
       mesh (*.msh)  }             |            |             { terminal
       data (*.dat)  } input ----> |   FeenoX   |----> output { data files
       input (*.fee) }             |            |             { post (vtk/msh)
                                   +------------+
- [General mathematical problems][] using [GNU GSL][]
- [Sets of ODEs/DAEs][] using [SUNDIALS][]
- [PDEs][] formulated with the [finite element method][]
  - reads mesh in [Gmsh][] format
  - uses [PETSc][]/[SLEPc][] to solve
    - linear systems ([KSP][])
    - non-linear systems ([SNES][])
    - time-dependent systems ([TS][])
    - generalized eigen-value problems ([EPS][])
  - writes results in either Gmsh or [VTK][] ([Paraview][]) format
- [Cloud-first design][] (cloud friendliness is not enough)
- MPI parallelization
- [Unix programming philosophy][]
  - [rule of separation][]
  - [rule of silence][]
  - [rule of economy][]
- Each PDE (i.e. from Laplace downward in the list of examples) [is
  implemented in a subdirectory][] within [`src/pde`][PDEs] of the
  source tree
  - any subdirectory can be removed if a particular PDE is not needed
  - any subdirectory can be used as a template to add a new PDE to the
    capabilities
- [Space][], [time][] and/or solution-dependent [material properties][]
  and [boundary conditions][]
  - [algebraic expressions][], and/or
  - [point-wise interpolated data][]
- Command-line argument expansion for
  - [increased flexibility][],
  - [parametric sweeps][], and/or
  - [optimization loops][] (see also [this non-trivial example][])
- [Steady-state][], \[quasi-static\] and/or [transient problems][]
- [Linear][] and [non-linear][] problems
- Possibility to verify the code using the [Method of Manufactured
  Solutions][]
- Separate [repository to profile and study code performance][] using
  [Google’s benchmark library][]

  [problem]: https://www.seamplex.com/feenox/doc/feenox-manual.html#problem
  [syntactically-sugared]: https://seamplex.com/feenox/doc/sds.html#syntactic-sugar-highlighting
  [input file]: https://seamplex.com/feenox/doc/sds.html#sec:input
  [resemble the original human-friendly problem formulation]: https://seamplex.com/feenox/doc/sds.html#matching-formulations
  [no need to recompile if the problem changes]: https://seamplex.com/feenox/doc/sds.html#sec:introduction
  [nouns are definitions and verbs are instructions]: https://seamplex.com/feenox/doc/sds.html#sec:nouns_verbs
  [simple problems need simple inputs]: https://seamplex.com/feenox/doc/sds.html#sec:simple
  [everything is an expression]: https://seamplex.com/feenox/doc/sds.html#sec:expression
  [General mathematical problems]: https://seamplex.com/feenox/examples/basic.html
  [GNU GSL]: https://www.gnu.org/software/gsl/
  [Sets of ODEs/DAEs]: https://seamplex.com/feenox/examples/daes.html
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
  [VTK]: https://docs.vtk.org/en/latest/design_documents/VTKFileFormats.html
  [Paraview]: https://www.paraview.org/
  [Cloud-first design]: https://seamplex.com/feenox/doc/sds.html#cloud-first
  [Unix programming philosophy]: https://en.wikipedia.org/wiki/Unix_philosophy
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
  [increased flexibility]: https://seamplex.com/feenox/examples/basic.html#computing-the-derivative-of-a-function-as-a-unix-filter
  [parametric sweeps]: https://seamplex.com/feenox/examples/mechanical.html#parametric-study-on-a-cantilevered-beam
  [optimization loops]: https://seamplex.com/feenox/examples/modal.html#optimizing-the-length-of-a-tuning-fork
  [this non-trivial example]: https://seamplex.com/thesis/html/060-resultados/README.html#sec-tres-pescaditos
  [Steady-state]: https://www.seamplex.com/feenox/doc/tutorials/120-mazes/#solving-the-steady-state-laplace-equation
  [transient problems]: https://www.seamplex.com/feenox/doc/tutorials/320-thermal/#sec:transient
  [Linear]: https://www.seamplex.com/feenox/doc/tutorials/320-thermal/#linear-steady-state-problems
  [non-linear]: https://www.seamplex.com/feenox/doc/tutorials/320-thermal/#non-linear-state-state-problems
  [Method of Manufactured Solutions]: https://github.com/seamplex/feenox/tree/main/tests/mms
  [repository to profile and study code performance]: https://github.com/seamplex/feenox-benchmark
  [Google’s benchmark library]: https://github.com/google/benchmark

## Usefulness

- [Web-based thermo-mechanical solver running on the cloud][]
- [Non-conformal mesh mapping][]
- [ASME stress linearization for pressurized pipes and vessels][]
- [Assessment of material properties from tabulated sources][]
- [Environmentally-assisted fatigue analysis in dissimilar interfaces of
  nuclear pipes][]
- [Neutron transport in the cloud][]
- [Solving mazes without AI][]
- [Parametric NAFEMS LE10 benchmark: comparison of resource consumption
  for different FEA programs][]
- [Some Youtube videos][]

  [Web-based thermo-mechanical solver running on the cloud]: https://www.youtube.com/watch?v=DOnoXo_MCZg
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

Browse through the [documentation index][] and/or the [`doc`][]
subdirectory of the [Github repository][] for

- [FAQs][], including the [what FeenoX means][]
- [Manual][]
- [Description][]
- [Software Design Requirements][]
- [Software Design Specifications][]
- [Unix man page][] (accesible through `man feenox` after installation)
- [History][]
- [Compilation guide][]
- [Programming guide][]

  [documentation index]: doc
  [`doc`]: https://github.com/seamplex/feenox/tree/main/doc
  [Github repository]: https://github.com/seamplex/feenox/
  [FAQs]: FAQ.markdown
  [what FeenoX means]: https://seamplex.com/feenox/doc/FAQ.html#what-does-feenox-mean
  [Manual]: doc/feenox-manual.markdown
  [Description]: doc/feenox-desc.markdown
  [Software Design Requirements]: doc/srs.markdown
  [Software Design Specifications]: doc/sds.markdown
  [Unix man page]: doc/feenox.1.markdown
  [History]: doc/history.markdown
  [Compilation guide]: doc/compilation.markdown
  [Programming guide]: programming.markdown

# Quickstart

## Download

FeenoX is distributed under the terms of the [GNU General Public License
version 3][] or (at your option) any later version.

|                                     |                                                |
|-------------------------------------|------------------------------------------------|
| Debian/Ubuntu packages (unofficial) | <https://www.seamplex.com/feenox/dist/deb>     |
| GNU/Linux static binaries           | <https://www.seamplex.com/feenox/dist/linux>   |
| Windows binaries                    | <https://www.seamplex.com/feenox/dist/windows> |
| Source tarballs                     | <https://www.seamplex.com/feenox/dist/src>     |
| Github repository                   | <https://github.com/seamplex/feenox/>          |

- FeenoX is **cloud first**. It was designed to run on servers.

- Be aware that FeenoX **does not have a GUI**. Read the
  [documentation][], especially the [description][2] and the [FAQs][3].
  Ask for help on the [GitHub discussions page][].

- Debian/Ubuntu packages are unofficial, i.e. they are not available in
  `apt` repositories. They contain dynamically-linked binaries and their
  dependencies are hard-coded for each Debian/Ubuntu release. Make sure
  you get the right `.deb` for your release (i.e. `bookworm`/`bullseye`
  for Debian, `kinetic`/`focal` for Ubuntu).

- Generic GNU/Linux binaries are provided as statically-linked
  executables for convenience. They do not support MUMPS nor MPI and
  have only basic optimization flags. Please compile from source for
  high-end applications. See [detailed compilation instructions][].

- Try to avoid Windows as much as you can. The binaries are provided as
  transitional packages for people that for some reason still use such
  an outdated, anachronous, awful and invasive operating system. They
  are compiled with [Cygwin][] and have no support whatsoever. Really,
  really, **get rid of Windows ASAP**.

  > “It is really worth any amount of time and effort to get away from
  > Windows if you are doing computational science.”
  >
  > <https://lists.mcs.anl.gov/pipermail/petsc-users/2015-July/026388.html>

  [GNU General Public License version 3]: https://www.gnu.org/licenses/gpl-3.0.en.html
  [documentation]: https://seamplex.com/feenox/doc/
  [2]: https://www.seamplex.com/feenox/doc/feenox-desc.html
  [3]: https://seamplex.com/feenox/doc/FAQ.html
  [GitHub discussions page]: https://github.com/seamplex/feenox/discussions
  [detailed compilation instructions]: https://seamplex.com/feenox/doc/compilation.html
  [Cygwin]: http://cygwin.com/

## Git repository

To compile the Git repository, proceed as follows. This procedure does
need `git` and `autoconf` but new versions can be pulled and recompiled
easily. If something goes wrong and you get an error, do not hesitate to
ask in FeenoX’s [discussion page][].

1.  Install mandatory dependencies

    ``` terminal
    sudo apt-get update
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
    make -j4
    ```

    If you cannot (or do not want to) use `libgsl-dev` from a package
    repository, call `configure` with `--enable-download-gsl`:

    ``` terminal
    ./configure --enable-download-gsl
    ```

    If you do not have Internet access, get the tarball manually, copy
    it to the same directory as `configure` and run again. See the
    [detailed compilation instructions][4] for an explanation.

5.  Run test suite (optional)

    ``` terminal
    make check
    ```

6.  Install the binary system wide (optional)

    ``` terminal
    sudo make install
    ```

To stay up to date, pull and then autogen, configure and make (and
optionally install):

``` terminal
git pull
./autogen.sh; ./configure; make -j4
sudo make install
```

See the [download page][] and the [compilation guide][] for detailed
information.

  [discussion page]: https://github.com/seamplex/feenox/discussions
  [4]: compilation.md
  [download page]: https://seamplex.com/feenox/download.html
  [compilation guide]: doc/compilation.markdown

# Licensing

FeenoX is distributed under the terms of the [GNU General Public
License][] version 3 or (at your option) any later version. The
following text was borrowed from the [Gmsh documentation][]. Replacing
“Gmsh” with “FeenoX” (using Unix’s `sed`) gives:

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
> [General Public License][] that accompanies the source code. Further
> information about this license is available from the GNU Project
> webpage <http://www.gnu.org/copyleft/gpl-faq.html>.

FeenoX is licensed under the terms of the [GNU General Public
License][5] version 3 or, at the user convenience, any later version.
This means that users get the four essential freedoms:[^1]

0.  The freedom to *run* the program as they wish, for *any* purpose.
1.  The freedom to *study* how the program works, and *change* it so it
    does their computing as they wish.
2.  The freedom to *redistribute* copies so they can help others.
3.  The freedom to *distribute* copies of their *modified* versions to
    others.

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
[AGPL][] instead of GPL to further enforce freedom (TL;DR: it does not
matter for FeenoX), but which are already taken into account in FeenoX
licensing scheme.

It should be noted that not only is FeenoX free and open source, but
also all of the libraries it depends (and their dependencies) are. It
can also be compiled using free and open source build tool chains
running over free and open source operating systems. In addition, the
FeenoX documentation is licensed under the terms of the [GNU Free
Documentation License v1.3][] (or any later version).

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
  [5]: https://www.gnu.org/licenses/gpl-3.0
  [AGPL]: https://en.wikipedia.org/wiki/GNU_Affero_General_Public_License
  [GNU Free Documentation License v1.3]: https://www.gnu.org/licenses/fdl-1.3.html

## Contributing

Contributions from [hackers][] and/or [academics][] are welcome,
especially new types of PDEs and new formulations of existing PDEs. For
elliptic operators feel free to use the Laplace equation at
[`src/pdes/laplace`][] as a template.

1.  Read the [Programming Guide][6].
2.  Browse [Github discussions][] and open a new thread explaining what
    you want to do and/or asking for help.
3.  Fork the [Git repository][] under your Github account
4.  Create a pull request, including
    - code,
    - documentation, and
    - tests.
5.  Follow up the review procedure.

Note that

- It is mandatory to observe the [Code of Conduct][].
- The contributed code has to be compatible with the [GPLv3+ license][].
- Each author keeps the copyright of the contribution.
- You can [ask][Github discussions]!

  [hackers]: http://localhost/milhouse/feenox/README4hackers.html
  [academics]: http://localhost/milhouse/feenox/README4academics.html
  [`src/pdes/laplace`]: https://github.com/seamplex/feenox/tree/main/src/pdes/laplace
  [6]: https://seamplex.com/feenox/doc/programming.html
  [Github discussions]: https://github.com/seamplex/feenox/discussions
  [Git repository]: https://github.com/seamplex/feenox/
  [Code of Conduct]: https://seamplex.com/feenox/doc/CODE_OF_CONDUCT.html
  [GPLv3+ license]: https://www.seamplex.com/feenox/#sec:licensing

# Further information

Home page: <https://www.seamplex.com/feenox>  
Repository: <https://github.com/seamplex/feenox>  
Bug reporting: <https://github.com/seamplex/feenox/issues>  
Discussions: <https://github.com/seamplex/feenox/discussions>  
Follow us: [YouTube][] [LinkedIn][] [Github][]

------------------------------------------------------------------------

FeenoX is copyright ©2009-2025 [Seamplex][]  
FeenoX is licensed under [GNU GPL version 3][] or (at your option) any
later version.  
FeenoX is free software: you are free to change and redistribute it.  
There is **NO WARRANTY**, to the extent permitted by law.

  [YouTube]: https://www.youtube.com/channel/UCC6SzVLxO8h6j5rLlfCQPhA
  [LinkedIn]: https://www.linkedin.com/company/seamplex/
  [Github]: https://github.com/seamplex
  [Seamplex]: https://www.seamplex.com
  [GNU GPL version 3]: http://www.gnu.org/copyleft/gpl.html
