# FeenoX for Engineers

- [<span class="toc-section-number">0.1</span> How FeenoX works][]
- [<span class="toc-section-number">0.2</span> What][]
- [<span class="toc-section-number">0.3</span> Why][]

  [<span class="toc-section-number">0.1</span> How FeenoX works]: #how-feenox-works
  [<span class="toc-section-number">0.2</span> What]: #what
  [<span class="toc-section-number">0.3</span> Why]: #sec:why

## How FeenoX works

> **Heads up** First things first:
>
> 1.  [FeenoX][] is not a traditional point-and-click finite-element
>     solver. It **does not include** a graphical interface.
>
> 2.  FeenoX is designed as a cloud-first [back end][] for generic
>     computational workflows. One of these many workflows can be
>     point-and-click graphical interface such as [CAEplex][] which
>     allows to solve thermo-mechanical problems directly from the
>     browser.
>
> 3.  Since it is a cloud-first tool, FeenoX targets at GNU/Linux. If
>     you are using Windows, FeenoX may not be for you (but [CAEplex][]
>     may). See below

In plain engineering terms, FeenoX works like a transfer function
between one or more input files and zero or more output files:

                                 +------------+
     mesh (*.msh)  }             |            |             { terminal
     data (*.dat)  } input ----> |   FeenoX   |----> output { data files
     input (*.fee) }             |            |             { post (vtk/msh)
                                 +------------+

When solving problems using the finite element method, FeenoX sits in
the middle of pre and post-processing tools. The former should aid the
engineer to prepare the mesh and, eventually, the input file. The latter
should show the numerical results. See the tutorials.

Terminal & cloud

  [FeenoX]: https://www.seamplex.com/feenox
  [back end]: https://en.wikipedia.org/wiki/Frontend_and_backend
  [CAEplex]: https://www.caeplex.com

## What

Examples

Tutorials

Tests

## Why

There are two “why” questions we have to answer.

1.  Why is FeenoX different from other “similar” tools?

    LE10

    - ready-to-run executable (no need to compile)
    - self-explanatory plain-text near-English input file
      - one-to-one correspondence between computer input file and human
        description of the problem
      - Git-traceable (mesh is not part of the input file)
      - everything is an expression
    - 100% user-defined output
    - simple problems need simple inputs
    - similar problems need similar inputs
    - parametric and optimization runs through command-line arguments
    - flexibility to handle many workflows

2.  Why FeenoX works the way it does?

    Because it is cloud first and its objective is to be flexible enough
    to power web-based interfaces like [CAEplex][] and many other
    workflows. More information in the details for [hackers][] and
    [professors][].

    Depending on the complexity of the case, [CAEplex][] might be enough
    or not. If the latter, one has to see what’s sitting under the hood.
    Again, peek into the [hackers][] and [professors][] details for more
    information.

    > <div class="container text-center my-5 ratio ratio-16x9">
    >
    > <iframe class="embed-responsive-item" src="https://www.youtube.com/embed/kD3tQdq17ZE" allowfullscreen>
    > </iframe>
    >
    > </div>

    > <div class="container text-center my-5 ratio ratio-16x9">
    >
    > <iframe class="embed-responsive-item" src="https://www.youtube.com/embed/ylXAUAsfb5E" allowfullscreen>
    > </iframe>
    >
    > </div>

  [CAEplex]: https://www.caeplex.com
  [hackers]: README4engineers.md
  [professors]: README4academics.md
