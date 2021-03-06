# FeenoX documentation index

-   [Hands on](#hands-on)
    -   [Quick examples](#quick-examples)
    -   [Tutorials](#tutorials)
    -   [Case files](#case-files)
-   [Manual and reference](#manual-and-reference)
    -   [UNIX manpage](#unix-manpage)
    -   [FeenoX manual](#feenox-manual)
    -   [Reference sheet](#reference-sheet)
-   [Background and generalities](#background-and-generalities)
    -   [Software requirements
        specifications](#software-requirements-specifications)
    -   [Software design
        specifications](#software-design-specifications)
    -   [FAQ](#faq)
    -   [History](#history)
-   [Programming and contributing](#programming-and-contributing)
    -   [Asking questions & reporting
        bugs](#asking-questions-reporting-bugs)
    -   [Compiling from source](#compiling-from-source)
    -   [Contributing guidelines](#contributing-guidelines)
    -   [How FeenoX documentation system
        works](#how-feenox-documentation-system-works)

There is a brief and general description in the main [README](..).

# Hands on

Go directly to the point and see how to solve problems with FeenoX.
Everything (except the case files) is included in the Git repository.

## Quick examples

Annotated examples can be found in the [examples](../examples)
directory. These are simple and quick (but varied) cases.

## Tutorials

Step-by-step instructions and explanations to solve increasingly-complex
problems are given in the [tutorials](../tutorials) directory.

## Case files

Detailed solutions of benchmarks, V&V cases and/or problems with
industrial interest. These cases include parametric studies for mesh
convergence, comparisons with analytical or other programs, efficiency
measurements, etc. Since these cases involve other tools, figures, data
files, etc. they have a separate repository.

# Manual and reference

## UNIX manpage

See [the Markdown source](./feenox.1.md) of the actual [UNIX
manpage](./feenox.1). It should be accessible with `man feenox` after
installation.

## FeenoX manual

The full FeenoX manual is available as a Texinfo PDF, compiled from an
M4-preprocessed [Markdown source](./feenox.md) converted to `texi` by
Pandoc.

## Reference sheet

-   Keywords
-   Variables
-   Distributions
-   Boundary conditions
-   Functions

# Background and generalities

## Software requirements specifications

The FeenoX project starts as an offer to an imaginary “request for
quotations” that defines [software requirements specifications](srs.md)
for a computational tool.

## Software design specifications

The “quotation” to the above tender is given in a [software design
specifications](sds.md) document that explains the design decisions and
features included in FeenoX.

## FAQ

Read the [Frequently Asked Questions](FAQ.md) before asking what has
already been answered.

## History

To be done.

# Programming and contributing

## Asking questions & reporting bugs

These are two most basic ways of contributing to an open source project,
so do not hesitate to ask questions in our [mailing
list](https://www.seamplex.com/lists.html) and to report bugs in our
[Github issue tracker](https://github.com/seamplex/feenox/issues).

## Compiling from source

> If these instructions make no sense to you, read the [FAQs](FAQ.md).

1.  Install mandatory dependencies

        sudo apt-get install git gcc make automake autoconf libgsl-dev

2.  Install optional dependencies (of course these are *optional* but
    recommended)

        sudo apt-get install lib-sundials-dev petsc-dev slepc-dev libreadline-dev

3.  Clone Github repository

        git clone https://github.com/seamplex/feenox

4.  Boostrap, configure, compile & make

        cd feenox
        ./autogen.sh
        ./configure
        make

5.  Run test suite (optional, this might take some time)

        make check

6.  Install the binary system wide (optional)

        sudo make install

## Contributing guidelines

1.  Read the [programming guide](./programming.md)
2.  Join the [mailing list](https://www.seamplex.com/lists.html)
3.  Fork the [Github repository](https://github.com/seamplex/feenox/)
4.  Create a pull request

It is mandatory to observe the following code of conduct:

> Be kind to others. Do not insult or put down others. Behave
> professionally. Remember that harassment and sexist, racist, or
> exclusionary jokes are not appropriate for FeenoX.
>
> All communication should be appropriate for a professional audience
> including people of many different backgrounds. Sexual language and
> imagery is not appropriate.
>
> FeenoX is dedicated to providing a harassment-free community for
> everyone, regardless of gender, sexual orientation, gender identity
> and expression, disability, physical appearance, body size, race, or
> religion. We do not tolerate harassment of community members in any
> form.
>
> Help to make this a welcoming, friendly community for all.

## How FeenoX documentation system works

To be done.
