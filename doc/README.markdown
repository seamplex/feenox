# FeenoX documentation index

-   [Hands on][]
    -   [Quick examples][]
    -   [Tutorials][]
    -   [Case files][]
-   [Manual and reference][]
    -   [UNIX manpage][]
    -   [FeenoX manual][]
    -   [Reference sheet][]
-   [Background and generalities][]
    -   [Software requirements specifications][]
    -   [Software design specifications][]
    -   [FAQ][]
    -   [History][]
-   [Programming and contributing][]
    -   [Asking questions & reporting bugs][]
    -   [Compiling from source][]
    -   [Contributing guidelines][]
    -   [How FeenoX documentation system works][]

-   There is a brief introduction on the main [README][].
-   See [this presentation][] (August 2021).
-   There’s a [recording of the presentation][] (Slides are in English
    but audio is in Spanish).

  [Hands on]: #hands-on
  [Quick examples]: #quick-examples
  [Tutorials]: #tutorials
  [Case files]: #case-files
  [Manual and reference]: #manual-and-reference
  [UNIX manpage]: #unix-manpage
  [FeenoX manual]: #feenox-manual
  [Reference sheet]: #reference-sheet
  [Background and generalities]: #background-and-generalities
  [Software requirements specifications]: #software-requirements-specifications
  [Software design specifications]: #software-design-specifications
  [FAQ]: #faq
  [History]: #history
  [Programming and contributing]: #programming-and-contributing
  [Asking questions & reporting bugs]: #asking-questions-reporting-bugs
  [Compiling from source]: #compiling-from-source
  [Contributing guidelines]: #contributing-guidelines
  [How FeenoX documentation system works]: #how-feenox-documentation-system-works
  [README]: ..
  [this presentation]: https://www.seamplex.com/feenox/doc/2021-feenox.pdf
  [recording of the presentation]: https://youtu.be/-RJ5qn7E9uE

# Hands on

Go directly to the point and see how to solve problems with FeenoX.
Everything (except the case files) is included in the Git repository.

## Quick examples

Annotated examples can be found in the [examples][] directory. These are
simple and quick (but varied) cases.

  [examples]: ../examples

## Tutorials

Step-by-step instructions and explanations to solve increasingly-complex
problems are given in the [tutorials][1] directory.

  [1]: ../tutorials

## Case files

Detailed solutions of benchmarks, V&V cases and/or problems with
industrial interest. These cases include parametric studies for mesh
convergence, comparisons with analytical or other programs, efficiency
measurements, etc. Since these cases involve other tools, figures, data
files, etc. they have a separate repository.

# Manual and reference

## UNIX manpage

See [the Markdown source][] of the actual [UNIX manpage][2]. It should
be accessible with `man feenox` after installation.

  [the Markdown source]: ./feenox.1.md
  [2]: ./feenox.1.html

## FeenoX manual

The full FeenoX manual is available as a Texinfo PDF, compiled from an
M4-preprocessed [Markdown source][] converted to `texi` by Pandoc.

  [Markdown source]: ./feenox.md

## Reference sheet

-   [Keywords][]
-   Variables
-   Distributions
-   Boundary conditions
-   Functions

  [Keywords]: reference-kw.md

# Background and generalities

## Software requirements specifications

The FeenoX project starts as an offer to an imaginary “request for
quotations” that defines [software requirements specifications][3] for a
computational tool.

  [3]: srs.md

## Software design specifications

The “quotation” to the above tender is given in a [software design
specifications][4] document that explains the design decisions and
features included in FeenoX.

  [4]: sds.md

## FAQ

Read the [Frequently Asked Questions][] before asking what has already
been answered.

  [Frequently Asked Questions]: FAQ.md

## History

To be done.

# Programming and contributing

## Asking questions & reporting bugs

These are two most basic ways of contributing to an open source project,
so do not hesitate to ask questions in our [mailing list][] and to
report bugs in our [Github issue tracker][].

  [mailing list]: https://www.seamplex.com/lists.html
  [Github issue tracker]: https://github.com/seamplex/feenox/issues

## Compiling from source

> If these instructions make no sense to you, read the [FAQs][].

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

See the [compilation guide][] for a full explanation and of the steps
above. See the [programming guide][] for more details about

-   setting custom flags,
-   using different compilers (e.g. `gcc`, `clang`, `icc`, etc),
-   compiling with debugging information,
-   configuring and compiling dependencies from sources,
-   setting up an IDE for development,
-   etc.

  [FAQs]: FAQ.md
  [compilation guide]: ./compile.md
  [programming guide]: ./programming.md

## Contributing guidelines

Any contribution is welcome, especially new types of PDEs and new
formulations of existing PDEs. For elliptic operators feel free to use
the Laplace equation as a template.

1.  Read the [programming guide][]
2.  Join the [mailing list][]
3.  Fork the [Github repository][]
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

  [programming guide]: ./programming.md
  [mailing list]: https://www.seamplex.com/lists.html
  [Github repository]: https://github.com/seamplex/feenox/

## How FeenoX documentation system works

To be done.
