# FeenoX documentation index

- [<span class="toc-section-number">1</span> FeenoX manual and
  description][]
  - [<span class="toc-section-number">1.1</span> Design][]
  - [<span class="toc-section-number">1.2</span> Frequently Asked
    Questions][]
  - [<span class="toc-section-number">1.3</span> UNIX manpage][]
- [<span class="toc-section-number">2</span> Hands on][]
  - [<span class="toc-section-number">2.1</span> Quick examples][]
  - [<span class="toc-section-number">2.2</span> Tutorials][]
  - [<span class="toc-section-number">2.3</span> Case files][]
- [<span class="toc-section-number">3</span> Background and
  generalities][]
  - [<span class="toc-section-number">3.1</span> Software requirements
    specifications][]
  - [<span class="toc-section-number">3.2</span> Software design
    specifications][]
  - [<span class="toc-section-number">3.3</span> History][]
- [<span class="toc-section-number">4</span> Programming and
  contributing][]
  - [<span class="toc-section-number">4.1</span> Asking questions &
    reporting bugs][]
  - [<span class="toc-section-number">4.2</span> Compiling from
    source][]
  - [<span class="toc-section-number">4.3</span> Contributing
    guidelines][]
  - [<span class="toc-section-number">4.4</span> How FeenoX
    documentation system works][]

> FeenoX documentation is released under the terms of the [GNU Free
> Documentation License v1.3][], or any later version.

  [<span class="toc-section-number">1</span> FeenoX manual and description]:
    #feenox-manual-and-description
  [<span class="toc-section-number">1.1</span> Design]: #design
  [<span class="toc-section-number">1.2</span> Frequently Asked Questions]:
    #frequently-asked-questions
  [<span class="toc-section-number">1.3</span> UNIX manpage]: #unix-manpage
  [<span class="toc-section-number">2</span> Hands on]: #hands-on
  [<span class="toc-section-number">2.1</span> Quick examples]: #quick-examples
  [<span class="toc-section-number">2.2</span> Tutorials]: #tutorials
  [<span class="toc-section-number">2.3</span> Case files]: #case-files
  [<span class="toc-section-number">3</span> Background and generalities]:
    #background-and-generalities
  [<span class="toc-section-number">3.1</span> Software requirements specifications]:
    #software-requirements-specifications
  [<span class="toc-section-number">3.2</span> Software design specifications]:
    #software-design-specifications
  [<span class="toc-section-number">3.3</span> History]: #history
  [<span class="toc-section-number">4</span> Programming and contributing]:
    #programming-and-contributing
  [<span class="toc-section-number">4.1</span> Asking questions & reporting bugs]:
    #asking-questions-reporting-bugs
  [<span class="toc-section-number">4.2</span> Compiling from source]: #compiling-from-source
  [<span class="toc-section-number">4.3</span> Contributing guidelines]:
    #contributing-guidelines
  [<span class="toc-section-number">4.4</span> How FeenoX documentation system works]:
    #how-feenox-documentation-system-works
  [GNU Free Documentation License v1.3]: https://www.gnu.org/licenses/fdl-1.3.html

# FeenoX manual and description

As per the [GNU Coding Standards][], “a manual should serve both as
tutorial and reference.” Due to the formatting restrictions, the
[Texinfo][] version contains only the description and not the full
reference.

- [HTML manual][]
- [PDF manual][]
- [Texinfo description][]
- [HTML description][]
- [PDF description][]

The sources are in the [doc][] directory of the Git repository. They are
in [Pandoc][]-flavored Markdown.

  [GNU Coding Standards]: https://www.gnu.org/prep/standards/standards.html#GNU-Manuals
  [Texinfo]: https://www.gnu.org/software/texinfo/
  [HTML manual]: https://www.seamplex.com/feenox/doc/feenox-manual.html
  [PDF manual]: https://www.seamplex.com/feenox/doc/feenox-manual.pdf
  [Texinfo description]: https://www.seamplex.com/feenox/doc/feenox-desc.texi
  [HTML description]: https://www.seamplex.com/feenox/doc/feenox-desc.html
  [PDF description]: https://www.seamplex.com/feenox/doc/feenox-desc.pdf
  [doc]: https://github.com/seamplex/feenox/tree/main/doc
  [Pandoc]: https://pandoc.org/

## Design

The FeenoX project starts as an offer to an imaginary “request for
quotations” that defines [software requirements specifications][] for an
open source computational tool. Then a fictitious “offer” to the above
tender is given in a [software design specifications][] document that
explains the design decisions and features included in FeenoX.

- [Software Requirements specifications][1] [PDF][] (Fictitious RFQ)
- [Software Design specifications][2] [PDF][3] (Imaginary FeenoX’ offer)
- [Mid-term PhD review presentation slides with the SRS/SDS explanation
  in PDF][] (August 2021).
  - [Video recording of the presentation][] (Slides are in English but
    audio is in Spanish).
  - Browse the repository with the sources of the examples in the
    presentation [here][]
- [A free and open source computational tool for solving
  (nuclear-related) differential equations in the cloud][] (INAC/ENFIR
  Congress, December 2021).
  - [Video recording of the presentation][4] (Turn on the CCs, I am not
    a native English speaker).
  - Browse the repository with the sources of the examples in the
    presentation [here][5]

  [software requirements specifications]: ./srs.md
  [software design specifications]: ./sds.md
  [1]: srs.md
  [PDF]: https://www.seamplex.com/feenox/doc/srs.pdf
  [2]: sds.md
  [3]: https://www.seamplex.com/feenox/doc/sds.pdf
  [Mid-term PhD review presentation slides with the SRS/SDS explanation in PDF]:
    https://www.seamplex.com/feenox/doc/2021-feenox.pdf
  [Video recording of the presentation]: https://youtu.be/-RJ5qn7E9uE
  [here]: https://github.com/gtheler/2021-presentation
  [A free and open source computational tool for solving (nuclear-related) differential equations in the cloud]:
    https://www.seamplex.com/feenox/doc/2021-brasil.pdf
  [4]: https://youtu.be/e8kFmFOsbPk
  [5]: https://github.com/gtheler/2021-brasil

## Frequently Asked Questions

- [Frequently Asked Questions][]
- Ask yours on [GitHub Discussions][]
- Check also the [GitHub Issues][]

  [Frequently Asked Questions]: ./FAQ.md
  [GitHub Discussions]: https://github.com/seamplex/feenox/discussions/
  [GitHub Issues]: https://github.com/seamplex/feenox/issues

## UNIX manpage

See [UNIX manpage converted to HTML][]. It should be accessible with
`man feenox` after (global) installation and its sources are available
in the Git repository.

  [UNIX manpage converted to HTML]: https://www.seamplex.com/feenox/doc/feenox.1.html

# Hands on

Go directly to the point and see how to solve problems with FeenoX.
Everything (except the case files) is included in the Git repository.

## Quick examples

Annotated examples can be found in the [examples][] directory. These are
simple and quick (but varied) cases. They are based on the August 2021
presentation:

- [Recording (audio in Spanish, slides in English)][]
- [Slides in PDF][]
- [Markdown examples sources][]

Make sure you also browse the [tests][] directory of the Git repository
and the [Software Design specifications][] document.

  [examples]: ../examples
  [Recording (audio in Spanish, slides in English)]: https://youtu.be/-RJ5qn7E9uE
  [Slides in PDF]: https://www.seamplex.com/feenox/doc/2021-feenox.pdf
  [Markdown examples sources]: https://github.com/gtheler/2021-presentation
  [tests]: https://github.com/seamplex/feenox/tree/main/tests
  [Software Design specifications]: ./sds.md

## Tutorials

Step-by-step instructions and explanations to solve increasingly-complex
problems are given in the [tutorials][] directory.

**TO BE DONE**

  [tutorials]: ../tutorials

## Case files

Detailed solutions of benchmarks, V&V cases and/or problems with
industrial interest. These cases include parametric studies for mesh
convergence, comparisons with analytical or other programs, efficiency
measurements, etc. Since these cases involve other tools, figures, data
files, etc. they have a separate repository.

**TO BE DONE**

# Background and generalities

## Software requirements specifications

The FeenoX project starts as an offer to an imaginary “request for
quotations” that defines [software requirements specifications][] for a
computational tool.

  [software requirements specifications]: ./srs.md

## Software design specifications

The “quotation” to the above tender is given in a [software design
specifications][] document that explains the design decisions and
features included in FeenoX.

  [software design specifications]: ./sds.md

## History

See the [FeenoX history][].

  [FeenoX history]: history.md

# Programming and contributing

## Asking questions & reporting bugs

- Use [Github discussions][6] to ask for help,
- Use the [Github issue tracker][] to report bugs.

  [6]: https://github.com/seamplex/feenox/discussions
  [Github issue tracker]: https://github.com/seamplex/feenox/issues

## Compiling from source

- See the [Compilation guide][] for a full explanation and of the steps
  above.
- See the [Programming guide][] for more details.

  [Compilation guide]: ./compilation.md
  [Programming guide]: ./programming.md

## Contributing guidelines

Any contribution is welcome, especially new types of PDEs and new
formulations of existing PDEs. For elliptic operators feel free to use
the Laplace equation as a template.

1.  Read the [programming guide][]
2.  Browse [Github discussions][Github discussions6]
3.  Fork the [Github repository][]
4.  Create a pull request

It is mandatory to observe the [Code of Conduct][].

  [programming guide]: ./programming.md
  [Github discussions6]: https://github.com/seamplex/feenox/discussions
  [Github repository]: https://github.com/seamplex/feenox/
  [Code of Conduct]: CODE_OF_CONDUCT.md

## How FeenoX documentation system works

**TO BE DONE**
