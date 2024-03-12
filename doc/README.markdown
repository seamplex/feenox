# FeenoX documentation index

- [<span class="toc-section-number">1</span> Manual and description][]
  - [<span class="toc-section-number">1.1</span> Design][]
  - [<span class="toc-section-number">1.2</span> Other documents][]
  - [<span class="toc-section-number">1.3</span> Youtube videos][]
  - [<span class="toc-section-number">1.4</span> Markdown sources][]
- [<span class="toc-section-number">2</span> Hands on][]
  - [<span class="toc-section-number">2.1</span> Examples][]
  - [<span class="toc-section-number">2.2</span> Tutorials][]
    - [<span class="toc-section-number">2.2.1</span> General
      tutorials][]
    - [<span class="toc-section-number">2.2.2</span> Detailed
      functionality][]
    - [<span class="toc-section-number">2.2.3</span> Physics
      tutorials][]
  - [<span class="toc-section-number">2.3</span> Tests][]
- [<span class="toc-section-number">3</span> Contributing][]
  - [<span class="toc-section-number">3.1</span> Asking questions &
    reporting bugs][]
  - [<span class="toc-section-number">3.2</span> Contributing
    guidelines][]
  - [<span class="toc-section-number">3.3</span> Adding a new PDE to
    `src/pdes`][]

Every bit of FeenoX documentation is written in [Pandoc][]-flavored
Markdown in the [`doc`][] directory of the [FeenoX repository][]. It is
then converted to HTML, PDF, Unix manpage or Texinfo as needed. FeenoX
documentation is released under the terms of the [GNU Free Documentation
License v1.3][], or any later version.

  [<span class="toc-section-number">1</span> Manual and description]: #manual-and-description
  [<span class="toc-section-number">1.1</span> Design]: #design
  [<span class="toc-section-number">1.2</span> Other documents]: #other-documents
  [<span class="toc-section-number">1.3</span> Youtube videos]: #sec:youtube
  [<span class="toc-section-number">1.4</span> Markdown sources]: #markdown-sources
  [<span class="toc-section-number">2</span> Hands on]: #hands-on
  [<span class="toc-section-number">2.1</span> Examples]: #examples
  [<span class="toc-section-number">2.2</span> Tutorials]: #tutorials
  [<span class="toc-section-number">2.2.1</span> General tutorials]: #general-tutorials
  [<span class="toc-section-number">2.2.2</span> Detailed functionality]:
    #detailed-functionality
  [<span class="toc-section-number">2.2.3</span> Physics tutorials]: #physics-tutorials
  [<span class="toc-section-number">2.3</span> Tests]: #tests
  [<span class="toc-section-number">3</span> Contributing]: #contributing
  [<span class="toc-section-number">3.1</span> Asking questions & reporting bugs]:
    #asking-questions-reporting-bugs
  [<span class="toc-section-number">3.2</span> Contributing guidelines]:
    #contributing-guidelines
  [<span class="toc-section-number">3.3</span> Adding a new PDE to `src/pdes`]:
    #adding-a-new-pde-to-srcpdes
  [Pandoc]: https://pandoc.org/
  [`doc`]: https://github.com/seamplex/feenox/tree/main/doc
  [FeenoX repository]: https://github.com/seamplex/feenox/
  [GNU Free Documentation License v1.3]: https://www.gnu.org/licenses/fdl-1.3.html

# Manual and description

As per the [GNU Coding Standards][], “a manual should serve both as
tutorial and reference.” Due to the formatting restrictions, the
[Texinfo][] version contains only the description and not the full
reference.

- [FeenoX manual][] [\[PDF\]][1]
- [FeenoX description][] [\[PDF\]][2] [\[Texinfo\]][3]

  [GNU Coding Standards]: https://www.gnu.org/prep/standards/standards.html#GNU-Manuals
  [Texinfo]: https://www.gnu.org/software/texinfo/
  [FeenoX manual]: https://www.seamplex.com/feenox/doc/feenox-manual.html
  [1]: https://www.seamplex.com/feenox/doc/feenox-manual.pdf
  [FeenoX description]: https://www.seamplex.com/feenox/doc/feenox-desc.html
  [2]: https://www.seamplex.com/feenox/doc/feenox-desc.pdf
  [3]: https://www.seamplex.com/feenox/doc/feenox-desc.texi

## Design

1.  The FeenoX project starts as an offer to an imaginary “request for
    quotations” that defines [software requirements specifications][]
    for an open source computational tool.

    - [Software Requirements specifications][] (Fictitious RFQ)
      [\[PDF\]][4]

2.  Then a fictitious “offer” to the above tender is given in a
    [software design
    specifications][software requirements specifications] document that
    explains the design decisions and features included in FeenoX.

    - [Software Design specifications][] (Imaginary FeenoX’s offer)
      [\[PDF\]][5]

  [software requirements specifications]: https://seamplex.com/feenox/doc/srs.html
  [4]: https://www.seamplex.com/feenox/doc/srs.pdf
  [Software Design specifications]: https://seamplex.com/feenox/doc/sds.html
  [5]: https://www.seamplex.com/feenox/doc/sds.pdf

## Other documents

- [Unix manpage][] (accessible with `man feenox` after (global)
  installation with `sudo make install`)
- [Frequently Asked Questions][] [PDF][]
- [Compilation Guide][] [PDF][6]
- [Programming Guide][] [PDF][7]
- [History][] [PDF][8]

  [Unix manpage]: https://www.seamplex.com/feenox/doc/feenox.1.html
  [Frequently Asked Questions]: FAQs.markdown
  [PDF]: https://www.seamplex.com/feenox/doc/FAQs.pdf
  [Compilation Guide]: compilation.markdown
  [6]: https://www.seamplex.com/feenox/doc/compilation.pdf
  [Programming Guide]: programming.markdown
  [7]: https://www.seamplex.com/feenox/doc/programming.pdf
  [History]: history.markdown
  [8]: https://www.seamplex.com/feenox/doc/history.pdf

## Youtube videos

- [FeenoX hands-on tutorial][]
- [FeenoX, a cloud-first free and open source finite-element(ish)
  tool][]
- [Why FeenoX works the way it works][]
- [Verification of PDE solvers using the Method of Manufactured
  Solutions][]
- [FeenoX: a free and open source computational tool for solving
  (nuclear-related) DAEs in the cloud][]
- [CAEplex: finite elements in the cloud][]
- [Thermal finite elements in the cloud in 1 minute][]
- [Add CAE to your Onshape CAD with CAEplex][]
- [First 10 natural modes of a cylinder in less than 1 min. with
  CAEplex][]
- [Democratizing CAE][]

  [FeenoX hands-on tutorial]: https://www.youtube.com/watch?v=b3K2QewI8jE
  [FeenoX, a cloud-first free and open source finite-element(ish) tool]:
    https://www.youtube.com/watch?v=EZSjFzJAhbw
  [Why FeenoX works the way it works]: https://www.youtube.com/watch?v=66WvYTb4pSg
  [Verification of PDE solvers using the Method of Manufactured Solutions]:
    https://www.youtube.com/watch?v=mGNwL8TGijg
  [FeenoX: a free and open source computational tool for solving (nuclear-related) DAEs in the cloud]:
    https://www.youtube.com/watch?v=e8kFmFOsbPk
  [CAEplex: finite elements in the cloud]: https://www.youtube.com/watch?v=DOnoXo_MCZg
  [Thermal finite elements in the cloud in 1 minute]: https://www.youtube.com/watch?v=mOxQeIk2WJA
  [Add CAE to your Onshape CAD with CAEplex]: https://www.youtube.com/watch?v=ylXAUAsfb5E
  [First 10 natural modes of a cylinder in less than 1 min. with CAEplex]:
    https://www.youtube.com/watch?v=kD3tQdq17ZE
  [Democratizing CAE]: https://www.youtube.com/watch?v=7KqiMbrSLDc

## Markdown sources

The Markdown sources are in the [`doc`][]:

- [`feenox-manual`][]
- [`feenox-desc`][`feenox-manual`]
- [`srs`][]
- [`sds`][]
- [`FAQ`][]
- [`feenox.1`][]
- [`history`][]
- [`compilation`][]
- [`programming`][]

They are compiled by running the [`make.sh`][] script.

  [`doc`]: https://github.com/seamplex/feenox/tree/main/doc
  [`feenox-manual`]: https/github.com/seamplex/feenox/blob/main/doc/feenox-desc.markdown
  [`srs`]: https/github.com/seamplex/feenox/blob/main/doc/srs.markdown
  [`sds`]: https/github.com/seamplex/feenox/blob/main/doc/sds.markdown
  [`FAQ`]: https/github.com/seamplex/feenox/blob/main/doc/FAQ.markdown
  [`feenox.1`]: https/github.com/seamplex/feenox/blob/main/doc/feenox.1.markdown
  [`history`]: https/github.com/seamplex/feenox/blob/main/doc/history.markdown
  [`compilation`]: https/github.com/seamplex/feenox/blob/main/doc/compilation.markdown
  [`programming`]: https/github.com/seamplex/feenox/blob/main/doc/programming.markdown
  [`make.sh`]: https://github.com/seamplex/feenox/blob/main/doc/make.sh

# Hands on

Go directly to the point and see how to solve problems with FeenoX.
Everything needed (input files, geometry files, meshes, data, scripts,
etc.) is included in the [FeenoX repository][].

  [FeenoX repository]: https://github.com/seamplex/feenox/

## Examples

[Annotated examples][] can be found in the [examples][] directory of the
Github repository.

- [Online annotated examples][Annotated examples]
  - [Basic mathematics][]
  - [Systems of ODEs/DAEs][]
  - [Laplace’s equation][]
  - [Heat conduction][]
  - [Linear elasticity][]
  - [Modal analysis][]
  - [Neutron diffusion][]
  - [Neutron S<sub>N</sub>][]
- [Github examples directory][examples]

  [Annotated examples]: https://www.seamplex.com/feenox/examples
  [examples]: https://github.com/seamplex/feenox/tree/main/examples
  [Basic mathematics]: https://seamplex.com/feenox/examples/basic.html
  [Systems of ODEs/DAEs]: https://seamplex.com/feenox/examples/daes.html
  [Laplace’s equation]: https://seamplex.com/feenox/examples/laplace.html
  [Heat conduction]: https://seamplex.com/feenox/examples/thermal.html
  [Linear elasticity]: https://seamplex.com/feenox/examples/mechanical.html
  [Modal analysis]: https://seamplex.com/feenox/examples/modal.html
  [Neutron diffusion]: https://seamplex.com/feenox/examples/neutron_diffusion.html
  [Neutron S<sub>N</sub>]: https://seamplex.com/feenox/examples/neutron_sn.html

## Tutorials

1.  [Setting up your workspace][]

  [Setting up your workspace]: https://www.seamplex.com/feenox/doc/tutorials/000-setup

### General tutorials

1.  [Overview: the tensile test case][]
2.  [Fun & games: solving mazes with PDES instead of AI][]

  [Overview: the tensile test case]: https://www.seamplex.com/feenox/doc/tutorials/110-tensile-test
  [Fun & games: solving mazes with PDES instead of AI]: https://www.seamplex.com/feenox/doc/tutorials/120-mazes

### Detailed functionality

1.  Input files, expressions and command-line arguments
2.  Static & transient cases
3.  Functions & functionals
4.  Vectors & matrices
5.  Differential-algebraic equations
6.  Meshes & distributions

### Physics tutorials

1.  The Laplace equation
2.  [Heat conduction][9]
3.  Linear elasticity
4.  Modal analysis
5.  Thermo-mechanical analysis
6.  Neutron diffusion
7.  Neutron transport

  [9]: https://www.seamplex.com/feenox/doc/tutorials/320-thermal

## Tests

The [`tests`][] directory in the repository has hundreds of

- `grep`-able examples,
- unit tests,
- regression tests, and/or
- (crude) mathematical verification tests.

  [`tests`]: https://github.com/seamplex/feenox/tree/main/tests

# Contributing

## Asking questions & reporting bugs

- Use [Github discussions][] to ask for help,
- Use the [Github issue tracker][] to report bugs.

  [Github discussions]: https://github.com/seamplex/feenox/discussions
  [Github issue tracker]: https://github.com/seamplex/feenox/issues

## Contributing guidelines

Contributions from [hackers][] and/or [academics][] are welcome,
especially new types of PDEs and new formulations of existing PDEs. For
elliptic operators feel free to use the Laplace equation at
[`src/pdes/laplace`][] as a template.

1.  Read the [Programming Guide][10].
2.  Browse [Github discussions][] and open a new thread explaining what
    you want to do and/or asking for help.
3.  Fork the [Git repository][] under your Github account
4.  Create a pull request, including
    - code,
    - documentation, and
    - tests.
5.  Follow up the review procedure.

Note that

- It is mandatory to observe the [Code of Conduct][].
- The contributed code has to be compatible with the [GPLv3+ license][].
- Each author keeps the copyright of the contribution.
- You can [ask][Github discussions]!

  [hackers]: README4hackers.html
  [academics]: README4academics.html
  [`src/pdes/laplace`]: https://github.com/seamplex/feenox/tree/main/src/pdes/laplace
  [10]: https://seamplex.com/feenox/doc/programming.html
  [Github discussions]: https://github.com/seamplex/feenox/discussions
  [Git repository]: https://github.com/seamplex/feenox/
  [Code of Conduct]: https://seamplex.com/feenox/doc/CODE_OF_CONDUCT.html
  [GPLv3+ license]: https://www.seamplex.com/feenox/#sec:licensing

## Adding a new PDE to `src/pdes`

To be explained.
