# FeenoX: a free no-fee no-X uniX-like finite-element(ish) computational engineering tool

<div class="alert alert-warning">

> Please note that FeenoX is a [back end][] aimed at advanced users. It
> **does not include a graphical interface**. For an easy-to-use
> web-based front end with FeenoX running in the cloud directly from
> your browser see [CAEplex][] at <https://www.caeplex.com>. Any
> contribution to make dekstop GUIs such as [PrePoMax][] or [FreeCAD][]
> to work with FeenoX are welcome.

</div>

  [What is FeenoX?]: #what-is-feenox
  [Download]: #download
  [Git repository]: #git-repository
  [Licensing]: #licensing
  [Further information]: #further-information
  [back end]: https://en.wikipedia.org/wiki/Front_and_back_ends
  [CAEplex]: https://www.caeplex.com
  [PrePoMax]: https://prepomax.fs.um.si/
  [FreeCAD]: http://https://www.freecadweb.org

# What is FeenoX?

FeenoX can be seen as

-   a syntactically-sweetened way of asking the computer to solve
    engineering-related mathematical problems, and/or
-   a finite-element(ish) tool with a particular design basis

Note that some of the problems solved with FeenoX might not actually
rely on the finite element method, but on general mathematical models
and even on the finite volumes method. That is why we say it is a
finite-element(ish) tool.

One of the main features of this allegedly particular design basis is
that **simple problems ought to have simple inputs** (*rule of
simplicity*). For instance, to solve one-dimensional heat conduction
over the domain *x* ∈ \[0, 1\] (which is indeed one of the most simple
engineering problems we can find) the following input file is enough:

``` feenox
READ_MESH slab.msh               # read mesh in Gmsh's v4.1 format
PROBLEM thermal DIMENSIONS 1     # tell FeenoX what we want to solve 
k = 1                            # set uniform conductivity
BC left  T=0                     # set fixed temperatures as BCs
BC right T=1                     # "left" and "right" are defined in the mesh
SOLVE_PROBLEM                    # tell FeenoX we are ready to solve the problem
PRINT T(0.5)                     # ask for the temperature at x=0.5
```

``` terminal
$ feenox thermal-1d-dirichlet-constant-k.fee 
0.5
$ 
```

The mesh is assumed to have been already created with [Gmsh][] (or any
other pre-processing tool and converted to `.msh` format with [Meshio][]
for example). This assumption follows the *rule of composition* and
prevents the actual input file to be polluted with mesh-dependent data
(such as node coordinates and/or nodal loads) so as to keep it simple
and make it [Git][]-friendly (*rule of generation*). The only link
between the mesh and the FeenoX input file is through physical groups
(in the case above `left` and `right`) used to set boundary conditions
and/or material properties.

Another design-basis decision is that **similar problems ought to have
similar inputs** (*rule of least surprise*). So in order to have a
space-dependent conductivity, we only have to replace one line in the
input above: instead of defining a scalar *k* we define a function of
*x* (we also update the output to show the analytical solution as well):

``` feenox
READ_MESH slab.msh
PROBLEM thermal DIMENSIONS 1
k(x) := 1+x                      # space-dependent conductivity
BC left  T=0
BC right T=1
SOLVE_PROBLEM
PRINT T(1/2) log(1+1/2)/log(2)   # print numerical and analytical solutions
```

``` terminal
$ feenox thermal-1d-dirichlet-space-k.fee 
0.584959    0.584963
$
```

FeenoX has an **everything is an expression** design principle, meaning
that any numerical input can be an algebraic expression (e.g. `T(1/2)`
is the same as `T(0.5)`). If we want to have a temperature-dependent
conductivity (which renders the problem non-linear) we can take
advantage of the fact that *T*(*x*) is available not only as an argument
to `PRINT` but also for the definition of algebraic functions:

``` feenox
READ_MESH slab.msh
PROBLEM thermal DIMENSIONS 1
k(x) := 1+T(x)                   # temperature-dependent conductivity
BC left  T=0
BC right T=1
SOLVE_PROBLEM
PRINT T(1/2) sqrt(1+(3*0.5))-1   # print numerical and analytical solutions
```

``` terminal
$ feenox thermal-1d-dirichlet-temperature-k.fee 
0.581139    0.581139
$
```

For example, we can solve the [NAFEMS LE11][] “Solid
cylinder/Taper/Sphere-Temperature” benchmark like

``` feenox
READ_MESH nafems-le11.msh DIMENSIONS 3
PROBLEM mechanical

# linear temperature gradient in the radial and axial direction
T(x,y,z) := sqrt(x^2 + y^2) + z

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

Another example would be the famous chaotic [Lorenz’ dynamical
system][]—the one of the butterfly—whose differential equations are

<div class="not-in-format plain latex">

*ẋ* = *σ* ⋅ (*y* − *x*)
  
*ẏ* = *x* ⋅ (*r* − *z*) − *y*
  
*ż* = *x* ⋅ *y* − *b* ⋅ *z*

</div>

where *σ* = 10, *b* = 8/3 and *r* = 28 are the classical parameters that
generate the butterfly as presented by Edward Lorenz back in his seminal
1963 paper [Deterministic non-periodic flow][]. We can solve it with
FeenoX by writing the equations in the input file as naturally as
possible, as illustrated in the input file that follows:

``` feenox
PHASE_SPACE x y z     # Lorenz attractor’s phase space is x-y-z
end_time = 40         # we go from t=0 to 40 non-dimensional units

sigma = 10            # the original parameters from the 1963 paper
r = 28
b = 8/3

x_0 = -11             # initial conditions
y_0 = -16
z_0 = 22.5

# the dynamical system's equations written as naturally as possible
x_dot .= sigma*(y - x)
y_dot .= x*(r - z) - y
z_dot .= x*y - b*z

PRINT t x y z        # four-column plain-ASCII output
```

Please note the following two points about both cases above:

1.  The input files are very similar to the statements of each problem
    in plain English words (*rule of clarity*). Take some time to read
    the [problem statement of the NAFEMS LE11 benchmark][] and the
    FeenoX input to see how well the latter matches the former. Same for
    the Lorenz’ chaotic system. Those with some experience may want to
    compare them to the inputs decks (sic) needed for other common FEA
    programs.
2.  By design, 100% of FeenoX’ output is controlled by the user. Had
    there not been any `PRINT` or `WRITE_MESH` instructions, the output
    would have been empty, following the *rule of silence*. This is a
    significant change with respect to traditional engineering codes
    that date back from times when one CPU hour was worth dozens (or
    even hundreds) of engineering hours. At that time, cognizant
    engineers had to dig into thousands of lines of data to search for a
    single individual result. Nowadays, following the *rule of economy*,
    it is actually far easier to ask the code to write only what is
    needed in the particular format that suits the user.

In other words, FeenoX is a computational tool to solve

-   dynamical systems written as sets of ODEs/DAEs, or
-   steady or quasi-static thermo-mechanical problems, or
-   steady or transient heat conduction problems, or
-   modal analysis problems,
-   neutron diffusion or transport problems
-   community-contributed problems

in such a way that the input is a near-English text file that defines
the problem to be solved. Some basic rules are

-   FeenoX is just a **solver** working as a *transfer function* between
    input and output files. Following the *rules of separation,
    parsimony and diversity*, **there is no embedded graphical
    interface** but means of using generic pre and post processing
    tools—in particular, [Gmsh][] and [Paraview][] respectively. See
    also [CAEplex][1].

-   The input files should be [syntactically sugared][] so as to be as
    self-describing as possible.

-   Simple problems ought to need simple input files.

-   Similar problems ought to need similar input files.

-   Everything is an expression. Whenever a number is expected, an
    algebraic expression can be entered as well. Variables, vectors,
    matrices and functions are supported. Here is how to replace the
    boundary condition on the right side of the slab above with a
    radiation condition:

    ``` feenox
    sigma = 1       # non-dimensional stefan-boltzmann constant
    e = 0.8         # emissivity 
    Tinf=1          # non-dimensional reference temperature
    BC right q=sigma*e*(Tinf^4-T(x)^4)
    ```

-   FeenoX should run natively in the cloud and be able to massively
    scale in parallel. See the [Software Requirements Specification][]
    and the [Software Development
    Specification][Software Requirements Specification] for details.

Since it is free ([as in freedom][]) and open source, contributions to
add features (and to fix bugs) are welcome. In particular, each kind of
problem supported by FeenoX (thermal, mechanical, modal, etc.) has a
subdirectory of source files which can be used as a template to add new
problems, as implied in the “community-contributed problems” bullet
above (*rules of modularity and extensibility*). See the
[documentation][] for details about how to contribute.

  [Gmsh]: http://gmsh.info/
  [Meshio]: https://github.com/nschloe/meshio
  [Git]: https://git-scm.com/
  [NAFEMS LE11]: https://www.nafems.org/publications/resource_center/p18/
  [Lorenz’ dynamical system]: http://en.wikipedia.org/wiki/Lorenz_system
  [Deterministic non-periodic flow]: http://journals.ametsoc.org/doi/abs/10.1175/1520-0469%281963%29020%3C0130%3ADNF%3E2.0.CO%3B2
  [problem statement of the NAFEMS LE11 benchmark]: doc/design/nafems-le11/nafems-le11.png
  [Paraview]: https://www.paraview.org/
  [1]: www.caeplex.com
  [syntactically sugared]: https://en.wikipedia.org/wiki/Syntactic_sugar
  [Software Requirements Specification]: doc/sds.md
  [as in freedom]: https://www.gnu.org/philosophy/free-sw.en.html
  [documentation]: doc

# Download

FeenoX is distributed under the terms of the [GNU General Public License
version 3][] or (at your option) any later version. See [licensing
below][] for details.

|                 |                                            |
|-----------------|--------------------------------------------|
| Binaries        | <https://www.seamplex.com/feenox/dist/bin> |
| Source tarballs | <https://www.seamplex.com/feenox/dist/src> |

  [GNU General Public License version 3]: https://www.gnu.org/licenses/gpl-3.0.en.html
  [licensing below]: #licensing

## Git repository

If the statically-linked binaries above do not fit your needs, the
recommended way of getting FeenoX is to compile from source.

1.  Install mandatory dependencies

    ``` terminal
    sudo apt-get install git gcc make automake autoconf libgsl-dev
    ```

2.  Install optional dependencies (of course these are *optional* but
    recommended)

    ``` terminal
    sudo apt-get install lib-sundials-dev petsc-dev slepc-dev libreadline-dev
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

5.  Run test suite (optional, this might take some time)

    ``` terminal
    make check
    ```

6.  Install the binary system wide (optional)

    ``` terminal
    sudo make install
    ```

# Licensing

FeenoX is distributed under the terms of the [GNU General Public
License][] version 3 or (at your option) any later version. The
following text was borrowed from the [Gmsh documentation][]. Replacing
“Gmsh” with “FeenoX” gives:

<div class="alert alert-light">

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

</div>

  [GNU General Public License]: http://www.gnu.org/copyleft/gpl.html
  [Gmsh documentation]: http://gmsh.info/doc/texinfo/gmsh.html#Copying-conditions
  [General Public License]: https://github.com/seamplex/feenox/blob/master/COPYING

# Further information

Home page: <https://www.seamplex.com/feenox>  
Repository: <https://github.com/seamplex/feenox.git>  
Mailing list and bug reports: <wasora@seamplex.com> (you need to
subscribe first at <wasora+subscribe@seamplex.com>)  
Web interface for mailing list: <https://www.seamplex.com/lists.html>  
Follow us: [YouTube][] [LinkedIn][] [Github][]

------------------------------------------------------------------------

FeenoX is copyright ©2009-2021 [Seamplex][]  
FeenoX is licensed under [GNU GPL version 3][] or (at your option) any
later version.  
FeenoX is free software: you are free to change and redistribute it.  
There is **NO WARRANTY**, to the extent permitted by law.  
See the [copying conditions][].

  [YouTube]: https://www.youtube.com/channel/UCC6SzVLxO8h6j5rLlfCQPhA
  [LinkedIn]: https://www.linkedin.com/company/seamplex/
  [Github]: https://github.com/seamplex
  [Seamplex]: https://www.seamplex.com
  [GNU GPL version 3]: http://www.gnu.org/copyleft/gpl.html
  [copying conditions]: COPYING
