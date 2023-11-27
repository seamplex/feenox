# FeenoX tutorials

- [<span class="toc-section-number">1</span> Introduction][]
  - [<span class="toc-section-number">1.1</span> General tutorials][]
  - [<span class="toc-section-number">1.2</span> Detailed
    functionality][]

  [<span class="toc-section-number">1</span> Introduction]: #introduction
  [<span class="toc-section-number">1.1</span> General tutorials]: #general-tutorials
  [<span class="toc-section-number">1.2</span> Detailed functionality]: #detailed-functionality

# Introduction

[FeenoX][] is a cloud-first engineering tool. This idea is thoroughly
discussed in the [documentation][], particularly in the

- [Software Requirements Specification][], and
- [Software Design Specification][]

As such, it is a tool that in principle should be used from a
higher-level interface (e.g. a web-based UI such as \<www.caeplex.com\>)
or called from a set of automated scripts following some kind of
parametric or optimization workflow.

In any case, if you want to learn how to create your web-based interface
or how to implement an optimization workflow on your own, you will need
to start running simple cases manually and then increasing the
complexity until reaching the solve state-of-the-art capabilities.

Recall by reading again the [project’s main README][], that FeenoX is—in
a certain sense—to desktop FEA programs (like [Code_Aster][] with
[Salome-Meca][] or [CalculiX][] with [PrePoMax][]) and libraries (like
[MoFEM][] or [Sparselizard][]) what [Markdown][] is to Word and
[(La)TeX][], respectively and *deliberately*.

Indeed, it is this the main goal of the tutorials, namely to be able to
run relatively small problems in FeenoX by creating the appropriate
input files and, eventually meshes or other needed data files. This way,
by the end of each tutorial you will better understsand how FeenoX works
and thus, how to make it work the way you need and/or want. And if not,
you always have the *freedom* to hire someone to explain it to you and
even to modify the code to make it work the way you need and/or want
since it is [free and open source software][].

1.  [Setting up your workspace][]

  [FeenoX]: https://www.seamplex.com/feenox
  [documentation]: https://www.seamplex.com/feenox/doc
  [Software Requirements Specification]: https://www.seamplex.com/feenox/doc/srs.html
  [Software Design Specification]: https://www.seamplex.com/feenox/doc/sds.html
  [project’s main README]: https://github.com/seamplex/feenox
  [Code_Aster]: https://www.code-aster.org/spip.php?rubrique2
  [Salome-Meca]: https://www.code-aster.org/V2/spip.php?article303
  [CalculiX]: http://www.calculix.de/
  [PrePoMax]: https://prepomax.fs.um.si/
  [MoFEM]: http://mofem.eng.gla.ac.uk/mofem/html/
  [Sparselizard]: http://sparselizard.org/
  [Markdown]: https://commonmark.org/
  [(La)TeX]: https://en.wikipedia.org/wiki/LaTeX
  [free and open source software]: https://www.seamplex.com/feenox/#licensing
  [Setting up your workspace]: https://www.seamplex.com/feenox/doc/tutorials/000-setup

## General tutorials

1.  [Overview: the tensile test case][]
2.  [Fun & games: solving mazes with PDES instead of AI][]

  [Overview: the tensile test case]: https://www.seamplex.com/feenox/doc/tutorials/110-tensile-test
  [Fun & games: solving mazes with PDES instead of AI]: https://www.seamplex.com/feenox/doc/tutorials/120-mazes

## Detailed functionality

1.  Input files, expressions and command-line arguments
2.  Static & transient cases
3.  Functions & functionals
4.  Vectors & matrices
5.  Differential-algebraic equations
6.  Meshes & distributions
