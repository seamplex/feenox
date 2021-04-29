<div class="text-center">

![][1]

[Downloads][] [Documentation][] [Licensing][]

</div>

<div class="alert alert-warning">

Please note that FeenoX is a [back end][] aimed at advanced users. It
**does not include a graphical interface**. For an easy-to-use web-based
front end with FeenoX running in the cloud directly from your browser
see [CAEplex][] at <https://www.caeplex.com>.

<div class="embed-responsive embed-responsive-16by9 mb-3">

<iframe class="embed-responsive-item" src="https://www.youtube.com/embed/kD3tQdq17ZE" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

</div>

</div>

  [1]: doc/logo.svg
  [Downloads]: doc/install
  [Documentation]: doc
  [Licensing]: #licensing
  [back end]: https://en.wikipedia.org/wiki/Front_and_back_ends
  [CAEplex]: https://www.caeplex.com

# What is FeenoX?

FeenoX can be seen as

-   a syntactically-sweetened way of asking the computer to solve
    engineering-related mathematical problems, and/or
-   a finite-element tool with a particular design basis

In other words, it is a computational tool to solve

-   dynamical systems written as sets of ODEs/DAEs, or
-   steady or quasistatic thermo-mechanical problems, or
-   steady or transient heat conduction problems, or
-   modal analysis problems,
-   neutron diffussion or transport problems.

Since it is free and open source, contributions to add features (and to
fix bugs) are welcome. See the [documentation][2] for details about how
to contribute.

  [2]: doc

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

1.  Install mandatory dependences

<!-- -->

    sudo apt-get install git gcc make automake autoconf libgsl-dev

1.  Install optional dependences (of course these are *optional* but
    recommended)

<!-- -->

    sudo apt-get install lib-sundials-dev petsc-dev slepc-dev libreadline-dev

1.  Clone Github repository

<!-- -->

    git clone https://github.com/seamplex/feenox

1.  Boostrap, configure, compile & make

<!-- -->

    cd feenox
    ./autogen.sh
    ./configure
    make

1.  Run test suite (optional, this might take some time)

<!-- -->

    make check

1.  Install the binary system wide (optional)

<!-- -->

    sudo make install

# Licensing

FeenoX is distributed under the terms of the [GNU General Public
License][] version 3 or (at your option) any later version. The
following text was borrowed from the [Gmsh documentation][]. Replacing
“Gmsh” with “FeenoX” gives:

<div class="alert alert-light">

FeenoX is “free software”; this means that everyone is free to use it
and to redistribute it on a free basis. FeenoX is not in the public
domain; it is copyrighted and there are restrictions on its
distribution, but these restrictions are designed to permit everything
that a good cooperating citizen would want to do. What is not allowed is
to try to prevent others from further sharing any version of FeenoX that
they might get from you.

Specifically, we want to make sure that you have the right to give away
copies of FeenoX, that you receive source code or else can get it if you
want it, that you can change FeenoX or use pieces of FeenoX in new free
programs, and that you know you can do these things.

To make sure that everyone has such rights, we have to forbid you to
deprive anyone else of these rights. For example, if you distribute
copies of FeenoX, you must give the recipients all the rights that you
have. You must make sure that they, too, receive or can get the source
code. And you must tell them their rights.

Also, for our own protection, we must make certain that everyone finds
out that there is no warranty for FeenoX. If FeenoX is modified by
someone else and passed on, we want their recipients to know that what
they have is not what we distributed, so that any problems introduced by
others will not reflect on our reputation.

The precise conditions of the license for FeenoX are found in the
[General Public License][] that accompanies the source code. Further
information about this license is available from the GNU Project webpage
<http://www.gnu.org/copyleft/gpl-faq.html>.

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
Follow us: [Twitter][] [YouTube][] [LinkedIn][] [Github][]

------------------------------------------------------------------------

FeenoX is copyright ©2009-2021 [Seamplex][]  
FeenoX is licensed under [GNU GPL version 3][] or (at your option) any
later version.  
FeenoX is free software: you are free to change and redistribute it.  
There is NO WARRANTY, to the extent permitted by law.  
See the [copying conditions][].

  [Twitter]: https://twitter.com/seamplex/
  [YouTube]: https://www.youtube.com/channel/UCC6SzVLxO8h6j5rLlfCQPhA
  [LinkedIn]: https://www.linkedin.com/company/seamplex/
  [Github]: https://github.com/seamplex
  [Seamplex]: https://www.seamplex.com
  [GNU GPL version 3]: http://www.gnu.org/copyleft/gpl.html
  [copying conditions]: COPYING
