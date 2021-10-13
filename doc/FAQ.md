---
title: Frequently Asked Questions about FeenoX
author: Jeremy Theler
lang: en-US
---

# What is FeenoX?

It is "a free no-fee no-X uniX-like finite-element(ish) computational engineering tool."
Essentially, a finite-element program with a particular design basis:

> FeenoX is to finite-element programs and libraries what Markdown is to word processors (like Word) and typesetting systems (like TeX), respectively.

In increasing order of complexity and comprehensiveness, these resources explain what [FeenoX](https://www.seamplex.com/feenox) is:

 * The [examples](https://www.seamplex.com/feenox/examples) will give a brief overview of what FeenoX can do.
 * The [README](https://www.seamplex.com/feenox/) in the [GitHub repository](https://github.com/seamplex/feenox/) has a brief introduction (after explaining why).
 * There is also a [description](https://www.seamplex.com/feenox/doc/feenox-desc.html) in the [documentation](https://seamplex.com/feenox/doc/).
 * FeenoX is an "offer" to a fictitious "tender" for a computational tool. The RFQ is the [Software Requirements Specification](https://www.seamplex.com/feenox/doc/srs.pdf) and the explanation of how FeenoX addresses each requirement is the [Software Design Specification](https://www.seamplex.com/feenox/doc/srs.pdf).
 * This [presentation from August 2021]((https://www.seamplex.com/feenox/doc/2021-feenox.pdf)) explains the SRS/SDS pair. The sources and the examples can be found in [this Github repository](https://github.com/gtheler/2021-presentation). There is a [recording of the presentation]((https://youtu.be/-RJ5qn7E9uE)) (audio is in Spanish).
 * Finally the [manual](https://www.seamplex.com/feenox/doc/feenox-manual.pdf) will be the ultimate guide.



# What does FeenoX mean?

It does not mean anything particular, but

 * The last X makes it rhyme with UNIX and Linux.
 * "noX" means that there is no graphical (i.e. _X_) interface
 * Fee-no means that there are no fees involved (free as in "free beer")
 * FeenoX is the successor of the now-superseded FEA program Fino
 * It rhymes with FEniCS
 * With some luck one can read "Finite ElEments NO-X"
 * With mode luck, "FrEE" (as in "free speech")


# Why nothing happens when I double click on `feenox.exe`?

Because by design, FeenoX does not have a Graphical User Interface. Instead, it works like a transfer function between one or more input files and zero or more output files:

```include
transfer.md
```

Recall that FeenoX is essentially a cloud-base tool, and “the cloud” runs on UNIX (essentially GNU/Linux) so FeenoX is based on the [UNIX philosophy](unix.md). In this world, programs act like filters (i.e. transfer functions). They are executed from a command-line terminal. So instead of “double clicking” the executable, one has to open a terminal and execute it there. Without any arguments it says how to use it:

```terminal
> feenox.exe
FeenoX v0.1.67-g8899dfd-dirty 
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
>
```

It is explained there that the main input file has to be given as the first argument. So go to the tests or examples directory, find a test you like and run it:

```terminal
> cd tests
> feenox parallelepiped.fee
0.000295443
>
```

In any case, recall once again that FeenoX is a cloud tool, and Windows is not cloud-friendly.
It is time to re-think what you expect from a finite-element(ish) tool.
If you still need a GUI, please check [CAEplex](https://www.caeplex.com).

Try to avoid Windows as much as you can. The binaries are provided as transitional packages for people that for some reason still use such an outdated, anachronous, awful and invasive operating system. They are compiled with [Cygwin](http://cygwin.com/) and have no support whatsoever. Really, really, **get rid of Windows ASAP**.

```include
windows.md
```

# How do I create input decks for FeenoX?

FeenoX does not have "input decks." It has "input files," which are syntactically-sugared English-like plain-text ASCII files that describe the problem to be solved. First see the examples and the test directory. Then read the documentation.

There is a syntax highlight file for the text Kate that helps the edition of input files.

![The Kate Text Editor can be used to prepare input files with syntax highlighting.](nafems-le10-problem-input.svg)

# Does FeenoX support beam and/or shell elements?

No, currently supports solid elements. Therefore, three-dimensional problems need to have tetrahedra, hexahedra, prisms and/or pyramids; and two-dimensional problems need to have triangles or quadrangles.

It might support non-solid elements for elasticity in future versions, though.
Contributions are welcome. Check out the [contributing guidelines](https://www.seamplex.com/feenox/doc/#contributing-guidelines).

# What license does FeenoX have?

TL;DR:

 * The code is GPLv3+: you can use it, modify it and re-distribute it freely (as in free speech) as long as you keep the same licensing terms.
 * The documentation is released under the terms of the GNU Free Documentation License version 1.3 or, at your option, any later version: same thing but with particular considerations for documentation instead of code.
 
```include
licensing.md
```


# Why is FeenoX written in C and not in...

## C++?

<https://www.quora.com/Why-is-C-still-in-use-even-though-we-have-C++-Is-there-anything-that-C-can-do-but-C++-cant-or-maybe-something-that-is-easier-to-do-in-C-rather-than-C++>

> **Why is PETSc programmed in C, instead of Fortran or C++?**
>
> C enables us to build data structures for storing sparse matrices, solver information, etc. in ways that Fortran simply does not allow. ANSI C is a complete standard that all modern C compilers support. The language is identical on all machines. C++ is still evolving and compilers on different machines are not identical. Using C function pointers to provide data encapsulation and polymorphism allows us to get many of the advantages of C++ without using such a large and more complicated language. It would be natural and reasonable to have coded PETSc in C++; we opted to use C instead. 
>
> <https://www.mcs.anl.gov/petsc/documentation/faq.html#why-c>



> **Why Git is written in C and not in C++, by Linus Torvalds**
>
> C++ is a horrible language. It's made more horrible by the fact that a lot 
> of substandard programmers use it, to the point where it's much much 
> easier to generate total and utter crap with it. Quite frankly, even if 
> the choice of C were to do *nothing* but keep the C++ programmers out, 
> that in itself would be a huge reason to use C.
>
> <http://harmful.cat-v.org/software/c++/linus>


## Fortran?

Because I am not insane (yet).

## Python?

If you actually asked this, then FeenoX is not for you.

## Go, Python or Julia?

I don't know either of them nor if any of these languages would be a good fit for FeenoX.
Keep in mind that it took me a while to know why not Fortran nor C++ even though there are people that would choose them over C.
Maybe something of the sort happens with these new ideas (or not, I don't know).

