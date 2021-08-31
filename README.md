---
title: FeenoX
subtitle: A free no-fee no-X uniX-like finite-element(ish) computational engineering tool
titleblock: |
 FeenoX: a free no-fee no-X uniX-like finite-element(ish) computational engineering tool
 =======================================================================================

lang: en-US
number-sections: true
fontsize: 11pt
geometry:
- paper=a4paper
- left=2.5cm
- right=2cm
- bottom=3.5cm
- foot=2cm
- top=3.5cm
- head=2cm
colorlinks: true
mathspec: true
syntax-definition: feenox.xml
header-includes: \include{syntax.tex}
listings: true
toc: false
...

# What is FeenoX?

```{.include}
doc/introduction.md
```


## Why?

The world is already full of finite-element programs and every day a grad student creates a new one from scratch.
However, almost every piece of FEA software falls in either one of these two categories:

 a. Powerful, flexible and complex advanced numerical solvers of general non-linear partial differential equations written by academics (for academics) distributed under the form of 
     i. libraries, which the user has to compile and link to their own codes, or
     ii. interpreted languages (i.e. Python) wrappers, which the user has to call from their own scripts, or
     iii. input-file reading binaries, which the user needs to fill in with the weak form of the equation they need to solve.
     
    **Examples:** [MoFEM](http://mofem.eng.gla.ac.uk/mofem/html/), [Sparselizard](http://sparselizard.org/), [GetDP](http://getdp.info/), [FEnICS](https://fenicsproject.org/), [MOOSE](https://mooseframework.org/), [FreeFEM](https://freefem.org/), ...  
     
 b. Commercial, non-free (well some of them are free but coded in FORTRAN 77 so the source is unintelligible) and complex GUI-based programs that are
     i. closed-source, so nobody can know what the actual equations are nor how they are solved, and/or
     ii. complicated, so the only way to use them is through their embedded mouse-based GUI, and/or
     iii. expensive and out of the league of many companies and professionals.
     
    **Examples:** [CalculiX](http://calculix.de/), [CodeAster](https://www.code-aster.org), [NASTRAN](https://github.com/nasa/NASTRAN-95)^[We list just the open-source ones because we [at Seamplex do not want to encourage the usage of non-free software](https://www.seamplex.com/mission.html#principles), but---with some exceptions---any of the commercial packages out there would also apply.] 
     
Hence, Fino tries to fill in the gap between these two worlds with a different design basis.


# Download

FeenoX is distributed under the terms of the [GNU General Public License version 3](https://www.gnu.org/licenses/gpl-3.0.en.html) or (at your option) any later version. See [licensing below](#licensing) for details.

|                             |                                                       |
|-----------------------------|-------------------------------------------------------|
|  GNU/Linux binaries         | <https://www.seamplex.com/feenox/dist/linux>          |
|  Windows binaries           | <https://www.seamplex.com/feenox/dist/windows>        |
|  Source tarballs            | <https://www.seamplex.com/feenox/dist/src>            |


 * Binaries are provided as statically-linked executables for convenience.
 * They do not support MUMPS nor MPI and have only basic optimization flags. Please compile from source for high-end applications.
 * Try to avoid Windows as much as you can. The binaries are provided as transitional packages for people that for some reason still use such an outdated, anachronous, awful and invasive operating system. They are compiled with Cygwin and have no support whatsoever. Really, really, get rid of Windows ASAP.
 
   > “It is really worth any amount of time and effort to get away from Windows if you are doing computational science.”
   >
   > [<https://lists.mcs.anl.gov/pipermail/petsc-users/2015-July/026388.html>]

## Git repository

If the statically-linked binaries above do not fit your needs, the recommended way of getting FeenoX is to compile from source.

```{.include}
doc/design/git.md
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
Follow us: [YouTube](https://www.youtube.com/channel/UCC6SzVLxO8h6j5rLlfCQPhA)
           [LinkedIn](https://www.linkedin.com/company/seamplex/)
           [Github](https://github.com/seamplex)

---------------------------

FeenoX is copyright ©2009-2021 [Seamplex](https://www.seamplex.com)  
FeenoX is licensed under [GNU GPL version 3](http://www.gnu.org/copyleft/gpl.html) or (at your option) any later version.  
FeenoX is free software: you are free to change and redistribute it.  
There is **NO WARRANTY**, to the extent permitted by law.  
See the [copying conditions](COPYING).  
