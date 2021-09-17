# FeenoX documentation index

-   [FeenoX manual][]
    -   [2021 Presentation][]
-   [Hands on][]
    -   [Quick examples][]
    -   [Tutorials][]
    -   [Case files][]
-   [Manual and reference][]
    -   [UNIX manpage][]
    -   [FeenoX description][]
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

  [FeenoX manual]: #feenox-manual
  [2021 Presentation]: #presentation
  [Hands on]: #hands-on
  [Quick examples]: #quick-examples
  [Tutorials]: #tutorials
  [Case files]: #case-files
  [Manual and reference]: #manual-and-reference
  [UNIX manpage]: #unix-manpage
  [FeenoX description]: #feenox-description
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

# FeenoX manual

-   [HTML][]
-   [PDF][]

  [HTML]: https://www.seamplex.com/feenox/doc/feenox-manual.html
  [PDF]: https://www.seamplex.com/feenox/doc/feenox-manual.pdf

## 2021 Presentation

-   [Slides in PDF][] (August 2021).
-   [Video recording of the presentation][] (Slides are in English but
    audio is in Spanish).
-   Browse the repository with the sources of the examples in the
    presentation [here][]

  [Slides in PDF]: https://www.seamplex.com/feenox/doc/2021-feenox.pdf
  [Video recording of the presentation]: https://youtu.be/-RJ5qn7E9uE
  [here]: https://github.com/gtheler/2021-presentation

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

**TO BE DONE**

# Manual and reference

## UNIX manpage

See [UNIX manpage converted to HTML][]). It should be accessible with
`man feenox` after installation and its sources are available in the Git
repository.

  [UNIX manpage converted to HTML]: https://www.seamplex.com/feenox/doc/feenox.1.html

## FeenoX description

As per the [GNU Coding Standards][], “a manual should serve both as
tutorial and reference.”

-   [HTML manual][]
-   [PDF manual][]
-   [Texinfo description][]
-   [HTML description][]
-   [PDF description][]

The sources are in the [doc][] directory of the Git repository. They are
in [Pandoc][]-flavored Markdown.

  [GNU Coding Standards]: https://www.gnu.org/prep/standards/standards.html#GNU-Manuals
  [HTML manual]: https://www.seamplex.com/feenox/doc/feenox-manual.html
  [PDF manual]: https://www.seamplex.com/feenox/doc/feenox-manual.pdf
  [Texinfo description]: https://www.seamplex.com/feenox/doc/feenox-desc.texi
  [HTML description]: https://www.seamplex.com/feenox/doc/feenox-desc.html
  [PDF description]: https://www.seamplex.com/feenox/doc/feenox-desc.pdf
  [doc]: https://github.com/seamplex/feenox/tree/main/doc
  [Pandoc]: https://pandoc.org/

# Background and generalities

## Software requirements specifications

The FeenoX project starts as an offer to an imaginary “request for
quotations” that defines [software requirements specifications][2] for a
computational tool.

  [2]: srs.md

## Software design specifications

The “quotation” to the above tender is given in a [software design
specifications][3] document that explains the design decisions and
features included in FeenoX.

  [3]: sds.md

## FAQ

Read the [Frequently Asked Questions][] before asking what has already
been answered.

  [Frequently Asked Questions]: FAQ.md

## History

See the [FeenoX history][].

  [FeenoX history]: history.md

# Programming and contributing

## Asking questions & reporting bugs

-   Use [Github discussions][] to ask for help,
-   Use the [Github issue tracker][] to report bugs.

  [Github discussions]: https://github.com/seamplex/feenox/discussions
  [Github issue tracker]: https://github.com/seamplex/feenox/issues

## Compiling from source

> If the following instructions make no sense to you, read the [FAQs][].

To compile the Git repository, proceed as follows. This procedure does
need `git` and `autoconf` but new versions can be pulled and recompiled
easily.

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
    make -j4
    ```

    If you cannot (or do not want) to use `libgsl-dev` from a package
    repository, call `configure` with `--enable-download-gsl`:

    ``` terminal
    ./configure --enable-download-gsl
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

To stay up to date, pull and then autogen, configure and make (and
optionally install):

``` terminal
git pull
./autogen.sh; ./configure; make -j4
sudo make install
```

-   See the [Compilation guide][] for a full explanation and of the
    steps above.
-   See the [Programming guide][] for more details.

  [FAQs]: FAQ.md
  [Compilation guide]: ./compile.md
  [Programming guide]: ./programming.md

## Contributing guidelines

Any contribution is welcome, especially new types of PDEs and new
formulations of existing PDEs. For elliptic operators feel free to use
the Laplace equation as a template.

1.  Read the [programming guide][]
2.  Browse [Github discussions][]
3.  Fork the [Github repository][]
4.  Create a pull request

It is mandatory to observe the [Code of Conduct][].

  [programming guide]: ./programming.md
  [Github discussions]: https://github.com/seamplex/feenox/discussions
  [Github repository]: https://github.com/seamplex/feenox/
  [Code of Conduct]: CODE_OF_CONDUCT.md

## How FeenoX documentation system works

To be done.
