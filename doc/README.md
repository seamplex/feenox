---
title: FeenoX documentation index
lang: en-US
titleblock: |
 FeenoX documentation index
 ==========================
...


 * There is a brief introduction on the main [README](..).
 * See [this presentation](https://www.seamplex.com/feenox/doc/2021-feenox.pdf) (August 2021).
 * There's a [recording of the presentation](https://youtu.be/-RJ5qn7E9uE) (Slides are in English but audio is in Spanish).

# Hands on

Go directly to the point and see how to solve problems with FeenoX. Everything (except the case files) is included in the Git repository.

## Quick examples

Annotated examples can be found in the [examples](../examples) directory. These are simple and quick (but varied) cases.

## Tutorials

Step-by-step instructions and explanations to solve increasingly-complex problems are given in the [tutorials](../tutorials) directory.

## Case files

Detailed solutions of benchmarks, V&V cases and/or problems with industrial interest. These cases include parametric studies for mesh convergence, comparisons with analytical or other programs, efficiency measurements,  etc. Since these cases involve other tools, figures, data files, etc. they have a separate repository.

# Manual and reference

## UNIX manpage

See [the Markdown source](./feenox.1.md) of the actual [UNIX manpage](./feenox.1.html). It should be accessible with `man feenox` after installation.

## FeenoX manual

The full FeenoX manual is available as a Texinfo PDF, compiled from an M4-preprocessed [Markdown source](./feenox.md) converted to `texi` by Pandoc.

## Reference sheet

 * [Keywords](reference-kw.md)
 * Variables
 * Distributions
 * Boundary conditions
 * Functions
 

# Background and generalities

## Software requirements specifications

The FeenoX project starts as an offer to an imaginary “request for quotations” that defines [software requirements specifications](srs.md) for a computational tool.

## Software design specifications

The “quotation” to the above tender is given in a [software design specifications](sds.md) document that explains the design decisions and features included in FeenoX.

## FAQ

Read the [Frequently Asked Questions](FAQ.md) before asking what has already been answered.

## History

To be done.


# Programming and contributing

## Asking questions & reporting bugs

These are two most basic ways of contributing to an open source project, so do not hesitate to ask questions in our [mailing list](https://www.seamplex.com/lists.html) and to report bugs in our [Github issue tracker](https://github.com/seamplex/feenox/issues). 

## Compiling from source

> If these instructions make no sense to you, read the [FAQs](FAQ.md).


 1. Install mandatory dependencies

    ```terminal
    sudo apt-get install git gcc make automake autoconf libgsl-dev
    ```

    If you cannot install `libgsl-dev` but still have `git` and the build toolchain, you can have the `configure` script to download and compile it for you. See below.
    
 2. Install optional dependencies (of course these are _optional_ but recommended)
 
    ```terminal
    sudo apt-get install lib-sundials-dev petsc-dev slepc-dev libreadline-dev
    ```

 3. Clone Github repository
 
    ```terminal
    git clone https://github.com/seamplex/feenox
    ```

 4. Boostrap, configure, compile & make
 
    ```terminal
    cd feenox
    ./autogen.sh
    ./configure
    make
    ```
    
    If you cannot (or do not want) to use `libgsl-dev` from a package repository, call `configure` with `--enable-download-gsl`:
    
    ```terminal
    configure --enable-download-gsl
    ```
    
    If you do not have Internet access, get the tarball manually, store it in the same directory as `configure` and run again.

 5. Run test suite (optional)
 
    ```terminal
    make check
    ```

 6. Install the binary system wide (optional)
 
    ```terminal
    sudo make install
    ```

See the [programming guide](./programming.md) for more details about

 * custom flags,
 * different compilers,
 * compiling with debugging information,
 * configuring and compiling dependencies from sources,
 * etc.
    
## Contributing guidelines

Any contribution is welcome, especially new types of PDEs and new formulations of existing PDEs.
For elliptic operators feel free to use the Laplace equation as a template.

 1. Read the [programming guide](./programming.md)
 2. Join the [mailing list](https://www.seamplex.com/lists.html)
 3. Fork the [Github repository](https://github.com/seamplex/feenox/)
 4. Create a pull request
 
It is mandatory to observe the following code of conduct:

> Be kind to others. Do not insult or put down others. Behave professionally. Remember that harassment and sexist, racist, or exclusionary jokes are not appropriate for FeenoX.
> 
> All communication should be appropriate for a professional audience including people of many different backgrounds. Sexual language and imagery is not appropriate.
> 
> FeenoX is dedicated to providing a harassment-free community for everyone, regardless of gender, sexual orientation, gender identity and expression, disability, physical appearance, body size, race, or religion. We do not tolerate harassment of community members in any form.
>
> Help to make this a welcoming, friendly community for all.



## How FeenoX documentation system works

To be done.

