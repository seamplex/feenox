---
title: FeenoX
subtitle: a free no-fee no-X uniX-like finite-element(ish) computational engineering tool
titleblock: |
 FeenoX: a free no-fee no-X uniX-like finite-element(ish) computational engineering tool
 =======================================================================================
...

::: {.not-in-format .plain .markdown .gfm .plain .latex }

![](doc/logo.svg)

:::::: {.text-center}
[Downloads](doc/install){.btn .btn-primary}
[Documentation](doc){.btn .btn-secondary}
[Licensing](#licensing){.btn .btn-info}
::::::
:::

:::{.alert .alert-warning}
> Please note that FeenoX is a [back end](https://en.wikipedia.org/wiki/Front_and_back_ends) aimed at advanced users.
> It **does not include a graphical interface**. For an easy-to-use web-based front end with FeenoX running in the cloud directly from your browser see [CAEplex](https://www.caeplex.com) at <https://www.caeplex.com>.

:::::: {.not-in-format .plain .markdown .gfm .plain .latex }
<div class="embed-responsive embed-responsive-16by9 mb-3">
 <iframe class="embed-responsive-item" src="https://www.youtube.com/embed/kD3tQdq17ZE" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>
</div> 
::::::
:::


# What is FeenoX?

FeenoX can be seen as

 * a syntactically-sweetened way of asking the computer to solve engineering-related mathematical problems, and/or
 * a finite-element tool with a particular design basis

include(doc/design/lorenz-le11.md)
 
In other words, FeenoX is a computational tool to solve

 * dynamical systems written as sets of ODEs/DAEs, or
 * steady or quasi-static thermo-mechanical problems, or
 * steady or transient heat conduction problems, or
 * modal analysis problems,
 * neutron diffusion or transport problems

in such a way that the input is a near-English text file that defines the problem to be solved. Some basic rules are

 * FeenoX is just a _solver_ working as a transfer function between input and output files. Following the [UNIX rule of separation](http://catb.org/~esr/writings/taoup/html/ch01s06.html), **there is no embedded graphical interface** but means of using generic pre and post processing tools---in particular, [Gmsh](http://gmsh.info/) and [Paraview](https://www.paraview.org/) respectively. See also [CAEplex](www.caeplex.com).
 * The input files should be [syntactically sugared](https://en.wikipedia.org/wiki/Syntactic_sugar) so as to be as self-describing as possible.
 * Simple problems ought to need simple input files. Here is the input for solving thermal conduction on a simple dimensionless one-dimensional slab with a temperature-dependent conductivity given by an algebraic expression:
 
   ```{.feenox style=feenox}
   READ_MESH slab.msh
   PROBLEM heat DIMENSIONS 1
   k(x) := 1+T(x)  # temperature-dependent conductivity
   BC left  T=0    # Dirichlet conditions at both ends
   BC right T=1
   SOLVE_PROBLEM
   # difference between FEM and analytical solution, should be zero
   PRINT T(0.5)-(sqrt(1+(3*0.5))-1)  
   ```
 * Whenever a number is expected, an algebraic expression can be entered as well. Variables, vectors, matrices and functions are supported. Here is how to replace the boundary condition on the right side of the slab above with a radiation condition:
 
   ```{.feenox style=feenox}
   sigma = 1       # non-dimensional stefan-boltzmann constant
   e = 0.8         # emissivity 
   Tinf=1          # non-dimensional reference temperature
   BC right q=sigma*e*(Tinf^4-T(x)^4)
   ```
 * FeenoX should run natively in the cloud and be able to massively scale in parallel. See the [Software Requirements Specification](doc/sds.md) and the [Software Development Specification](doc/sds.md) for details.
 
Since it is free ([as in freedom](https://www.gnu.org/philosophy/free-sw.en.html)) and open source, contributions to add features (and to fix bugs) are welcome. See the [documentation](doc) for details about how to contribute.


 
# Download

FeenoX is distributed under the terms of the [GNU General Public License version 3](https://www.gnu.org/licenses/gpl-3.0.en.html) or (at your option) any later version. See [licensing below](#licensing) for details.

|                   |                                                       |
|-------------------|-------------------------------------------------------|
|  Binaries         | <https://www.seamplex.com/feenox/dist/bin>            |
|  Source tarballs  | <https://www.seamplex.com/feenox/dist/src>            |

## Git repository

If the statically-linked binaries above do not fit your needs, the recommended way of getting FeenoX is to compile from source.

 1. Install mandatory dependencies

    ```
    sudo apt-get install git gcc make automake autoconf libgsl-dev
    ```

 2. Install optional dependencies (of course these are _optional_ but recommended)
 
    ```
    sudo apt-get install lib-sundials-dev petsc-dev slepc-dev libreadline-dev
    ```

 3. Clone Github repository
 
    ```
    git clone https://github.com/seamplex/feenox
    ```

 4. Boostrap, configure, compile & make
 
    ```
    cd feenox
    ./autogen.sh
    ./configure
    make
    ```

 5. Run test suite (optional, this might take some time)
 
    ``` 
    make check
    ```

 6. Install the binary system wide (optional)
 
    ```
    sudo make install
    ```


# Licensing

FeenoX is distributed under the terms of the [GNU General Public License](http://www.gnu.org/copyleft/gpl.html) version 3 or (at your option) any later version. The following text was borrowed from the [Gmsh documentation](http://gmsh.info/doc/texinfo/gmsh.html#Copying-conditions). Replacing “Gmsh” with “FeenoX” gives:

::: {.alert .alert-light}
>FeenoX is “free software”; this means that everyone is free to use it and to redistribute it on a free basis. FeenoX is not in the public domain; it is copyrighted and there are restrictions on its distribution, but these restrictions are designed to permit everything that a good cooperating citizen would want to do. What is not allowed is to try to prevent others from further sharing any version of FeenoX that they might get from you.
>
>Specifically, we want to make sure that you have the right to give away copies of FeenoX, that you receive source code or else can get it if you want it, that you can change FeenoX or use pieces of FeenoX in new free programs, and that you know you can do these things.
>
>To make sure that everyone has such rights, we have to forbid you to deprive anyone else of these rights. For example, if you distribute copies of FeenoX, you must give the recipients all the rights that you have. You must make sure that they, too, receive or can get the source code. And you must tell them their rights.
>
>Also, for our own protection, we must make certain that everyone finds out that there is no warranty for FeenoX. If FeenoX is modified by someone else and passed on, we want their recipients to know that what they have is not what we distributed, so that any problems introduced by others will not reflect on our reputation.
>
>The precise conditions of the license for FeenoX are found in the [General Public License](https://github.com/seamplex/feenox/blob/master/COPYING) that accompanies the source code. Further information about this license is available from the GNU Project webpage <http://www.gnu.org/copyleft/gpl-faq.html>.
:::

# Further information

Home page: <https://www.seamplex.com/feenox>  
Repository: <https://github.com/seamplex/feenox.git>  
Mailing list and bug reports: <wasora@seamplex.com>  (you need to subscribe first at <wasora+subscribe@seamplex.com>)  
Web interface for mailing list: <https://www.seamplex.com/lists.html>  
Follow us: [Twitter](https://twitter.com/seamplex/) [YouTube](https://www.youtube.com/channel/UCC6SzVLxO8h6j5rLlfCQPhA) [LinkedIn](https://www.linkedin.com/company/seamplex/) [Github](https://github.com/seamplex)

---------------------------

FeenoX is copyright ©2009-2021 [Seamplex](https://www.seamplex.com)  
FeenoX is licensed under [GNU GPL version 3](http://www.gnu.org/copyleft/gpl.html) or (at your option) any later version.  
FeenoX is free software: you are free to change and redistribute it.  
There is **NO WARRANTY**, to the extent permitted by law.  
See the [copying conditions](COPYING).  
