# FeenoX tutorials

- [<span class="toc-section-number">1</span> Introduction][]
- [<span class="toc-section-number">2</span> General tutorials][]
- [<span class="toc-section-number">3</span> Detailed functionality][]
- [<span class="toc-section-number">4</span> Physics tutorials][]

  [<span class="toc-section-number">1</span> Introduction]: #introduction
  [<span class="toc-section-number">2</span> General tutorials]: #general-tutorials
  [<span class="toc-section-number">3</span> Detailed functionality]: #detailed-functionality
  [<span class="toc-section-number">4</span> Physics tutorials]: #physics-tutorials

# Introduction

[FeenoX][] is a cloud-first engineering tool. This idea is thoroughly
discussed in the [documentation][], particularly in the

- [Software Requirements Specification][], and
- [Software Design Specification][]

As such, it is a tool that in principle should be used from a
higher-level interface (e.g. a web-based UI such as www.caeplex.com) or
called from a set of automated scripts following some kind of parametric
or optimization workflow. In any case, if you want to learn how to
create your web-based interface or how to implement an optimization
workflow on your own, you will need to start running simple cases
manually and then increasing the complexity until reaching the solve
state-of-the-art capabilities. Indeed, it is this the main goal of the
tutorials, namely to be able to run relatively small problems in FeenoX
by creating the appropriate input files and, eventually meshes or other
needed data files.

1.  [Setting up your workspace][]

  [FeenoX]: https://www.seamplex.com/feenox
  [documentation]: https://www.seamplex.com/feenox/doc
  [Software Requirements Specification]: https://www.seamplex.com/feenox/doc/srs.html
  [Software Design Specification]: https://www.seamplex.com/feenox/doc/sds.html
  [Setting up your workspace]: 000-setup

# General tutorials

1.  [Overview: the tensile test case][]
2.  Fun & games: solving a maze without AI

  [Overview: the tensile test case]: 110-tensile-test

# Detailed functionality

1.  Input files, expressions and command-line arguments
2.  Static & transient cases
3.  Functions & functionals
4.  Vectors & matrices
5.  Differential-algebraic equations
6.  Meshes & distributions

# Physics tutorials

1.  The Laplace equation
2.  Heat conduction
3.  Linear elasticity
4.  Modal analysis
5.  Thermo-mechanical analysis
6.  Neutron diffusion
7.  Neutron transport
