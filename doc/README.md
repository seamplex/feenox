---
title: FeenoX documentation index
titleblock: |
 FeenoX documentation index
 ==========================
lang: en-US
...

> FeenoX documentation is released under the terms of the [GNU Free Documentation License v1.3](https://www.gnu.org/licenses/fdl-1.3.html), or any later version.

Every bit of FeenoX documentation is written in [Pandoc](https://pandoc.org/)-flavored Markdown.
It is then converted to HTML, PDF, Unix manpage or Texinfo as needed.


# FeenoX manual and description


As per the [GNU Coding Standards](https://www.gnu.org/prep/standards/standards.html#GNU-Manuals), "a manual should serve both as tutorial and reference." Due to the formatting restrictions, the [Texinfo](https://www.gnu.org/software/texinfo/) version contains only the description and not the full reference.

 * [HTML manual](https://www.seamplex.com/feenox/doc/feenox-manual.html)
 * [PDF manual](https://www.seamplex.com/feenox/doc/feenox-manual.pdf)
 * [Texinfo description](https://www.seamplex.com/feenox/doc/feenox-desc.texi)
 * [HTML description](https://www.seamplex.com/feenox/doc/feenox-desc.html)
 * [PDF description](https://www.seamplex.com/feenox/doc/feenox-desc.pdf)

The sources are in the [doc](https://github.com/seamplex/feenox/tree/main/doc) directory of the Git repository.


## Design

 1. The FeenoX project starts as an offer to an imaginary “request for quotations” that defines [software requirements specifications](./srs.md) for an open source computational tool.
 
 2. Then a fictitious “offer” to the above tender is given in a [software design specifications](./sds.md) document that explains the design decisions and features included in FeenoX.


::::::::: {.only-in-format .html .markdown}
> **Why FeenoX works the way it works** (i.e. why it does not run in Windows) 
>
> ::::: {.container .text-center .my-5 .ratio .ratio-16x9}
> <iframe class="embed-responsive-item" src="https://www.youtube.com/embed/66WvYTb4pSg?rel=0" allowfullscreen></iframe>
> :::::
::::::::: 

 
 * [Software Requirements specifications](srs.md) [PDF](https://www.seamplex.com/feenox/doc/srs.pdf) (Fictitious RFQ)
 * [Software Design specifications](sds.md) [PDF](https://www.seamplex.com/feenox/doc/sds.pdf) (Imaginary FeenoX' offer)
 * [Mid-term PhD review presentation slides with the SRS/SDS explanation in PDF](https://www.seamplex.com/feenox/doc/2021-feenox.pdf) (August 2021).
   - [Video recording of the presentation](https://youtu.be/-RJ5qn7E9uE) (Slides are in English but audio is in Spanish).
   - Browse the repository with the sources of the examples in the presentation [here](https://github.com/gtheler/2021-presentation)
 * [A free and open source computational tool for solving (nuclear-related) differential equations in the cloud](https://www.seamplex.com/feenox/doc/2021-brasil.pdf) (INAC/ENFIR Congress, December 2021).
   - [Video recording of the presentation](https://youtu.be/e8kFmFOsbPk) (Turn on the CCs, I am not a native English speaker).
   - Browse the repository with the sources of the examples in the presentation [here](https://github.com/gtheler/2021-brasil)
   
## Frequently Asked Questions

Check out FeenoX’ [Frequently Asked Questions](./FAQ.md)
 * Ask yours on [GitHub Discussions](https://github.com/seamplex/feenox/discussions/)
 * Check also the [GitHub Issues](https://github.com/seamplex/feenox/issues)
 
## UNIX manpage

See [UNIX manpage converted to HTML](https://www.seamplex.com/feenox/doc/feenox.1.html).
It should be accessible with `man feenox` after (global) installation and its sources are available in the Git repository.

 
 
# Hands on

Go directly to the point and see how to solve problems with FeenoX. Everything (except the case files) is included in the Git repository.

## Quick examples

[Annotated examples](https://www.seamplex.com/feenox/examples) can be found in the [examples](https://github.com/seamplex/feenox/tree/main/examples) directory of the Github repository.

 * [Online annotated examples](https://www.seamplex.com/feenox/examples)
 * [Github examples directory](https://github.com/seamplex/feenox/tree/main/examples)

These are simple and quick (but varied) cases. They are based on the August 2021 presentation:

 * [Recording (audio in Spanish, slides in English)](https://youtu.be/-RJ5qn7E9uE)
 * [Slides in PDF](https://www.seamplex.com/feenox/doc/2021-feenox.pdf)
 * [Markdown examples sources](https://github.com/gtheler/2021-presentation)

The regression tests can also be used as quick examples:

 * [Regression tests](https://github.com/seamplex/feenox/tree/main/tests)
 
## Tutorials

Step-by-step instructions and explanations to solve increasingly-complex problems are given in the [tutorials](tutorials) directory.


### Introduction

 1. [Setting up your workspace](tutorials/000-setup)
 
### General tutorials
 
 1. [Overview: the tensile test case](tutorials/110-tensile-test)
 2. Fun & games: solving a maze without AI



# Background and generalities

## Software requirements specifications

The FeenoX project starts as an offer to an imaginary “request for quotations” that defines [software requirements specifications](./srs.md) for a computational tool.

## Software design specifications

The “quotation” to the above tender is given in a [software design specifications](./sds.md) document that explains the design decisions and features included in FeenoX.

## History

See the [FeenoX history](history.md).


# Programming and contributing

## Asking questions & reporting bugs

 * Use [Github discussions](https://github.com/seamplex/feenox/discussions) to ask for help,
 * Use the [Github issue tracker](https://github.com/seamplex/feenox/issues) to report bugs.

## Compiling from source

 * See the [Compilation guide](./compilation.md) for a full explanation and of the steps above.
 * See the [Programming guide](./programming.md) for more details.

    
## Contributing guidelines

Any contribution is welcome, especially new types of PDEs and new formulations of existing PDEs.
For elliptic operators feel free to use the Laplace equation as a template.

 1. Read the [programming guide](./programming.md)
 2. Browse [Github discussions](https://github.com/seamplex/feenox/discussions)
 3. Fork the [Github repository](https://github.com/seamplex/feenox/)
 4. Create a pull request
 
It is mandatory to observe the [Code of Conduct](CODE_OF_CONDUCT.md).



## How FeenoX documentation system works

**TO BE DONE**

