# FeenoX for Engineers

- [<span class="toc-section-number">1</span> How FeenoX works][]
- [<span class="toc-section-number">2</span> What Feenox can solve][]
- [<span class="toc-section-number">3</span> Why FeenoX works the way it
  does][]

  [<span class="toc-section-number">1</span> How FeenoX works]: #how-feenox-works
  [<span class="toc-section-number">2</span> What Feenox can solve]: #what-feenox-can-solve
  [<span class="toc-section-number">3</span> Why FeenoX works the way it does]:
    #why-feenox-works-the-way-it-does

# How FeenoX works

> **Heads up!** First things first:
>
> 1.  [FeenoX][] is not a traditional point-and-click finite-element
>     solver. It **does not include** a graphical interface.
>
> 2.  FeenoX is designed as a [cloud-first][] [back end][] for generic
>     computational workflows to solve engineering-related problems. One
>     of these many workflows can be point-and-click graphical interface
>     such as [CAEplex][] which allows to solve thermo-mechanical
>     problems directly from the browser.
>
> 3.  Since it is a cloud-first tool, FeenoX targets at GNU/Linux. If
>     you are using Windows, FeenoX may not be for you (but [CAEplex][]
>     may). See below to find out [why][] FeenoX works the way it does.

In plain engineering terms, FeenoX works like a transfer function
between one or more [input files][] and zero or more [output files][]:

                                 +------------+
     mesh (*.msh)  }             |            |             { terminal
     data (*.dat)  } input ----> |   FeenoX   |----> output { data files
     input (*.fee) }             |            |             { post (vtk/msh)
                                 +------------+

When solving problems using the finite element method, FeenoX sits in
the middle of pre and post-processing tools. The former should aid the
engineer to prepare the mesh and, eventually, the input file. The latter
should show the numerical results. See the [tensile test tutorial][] for
an in-depth step-by-step explanation.

To fix ideas, let us consider the [NAFEMS LE10 “Thick plate pressure”
benchmark][]. Fig. 1 shows that there is a one-to-one correspondence
between the human-friendly problem formulation and the input file FeenoX
reads. There is no need to give extra settings if the problem does not
ask for them. Note that since the problem has only one volume, `E` means
“the” Young modulus. No need to deal with a map between materials and
mesh entitites (in this case the mapping is not needed but in
[multi-material problems][] the mapping is needed indeed). Nothing more,
nothing less.

<figure id="fig:nafems-le10">
<img src="nafems-le10-problem-input.svg" style="width:100.0%"
alt="The NAFEMS LE10 problem statement and the corresponding FeenoX input" />
<figcaption>Figure 1: The NAFEMS LE10 problem statement and the
corresponding FeenoX input</figcaption>
</figure>

Say we already have a [`nafems-le10.geo`][] file which tells [Gmsh][]
how to create a mesh `nafems-le10.msh` (check out the [tensile test
tutorial][] for details). Then, we can create an input file for FeenoX
(using [editors with syntax highlighting][] for example) as follows:

``` feenox
```

> **Heads up!** The `.msh` file from Gmsh can be either
>
> 1.  version 2.2, or
> 2.  version 4.0, or
> 3.  version 4.1.
>
> and can be partitioned or not.

Once we put these two files, `nafems-le10.geo` and `nafems-le10.fee` in
the same directory, say in the [`examples`][] directory of the
repository, then we call first Gmsh and then FeenoX from the terminal to
solve the benchmark problem:

Check out the section about [invocation][] in the [FeenoX manual][].

The trick is that this workflow is susceptible of being automated and
customized to run [in the cloud][], possibly [in parallel throughout
several servers using the MPI standard][]. Check out this 1-minute video
that covers a similar case from the [tensile-test
tutorial][tensile test tutorial]:

<div class="not-in-format latex">

<video
src="https://seamplex.com/feenox/doc/tutorials/110-tensile-test/quick.mp4"
style="width:100.0%" controls=""><a
href="https://seamplex.com/feenox/doc/tutorials/110-tensile-test/quick.mp4">Video</a></video> 

</div>

There is a sound explanation about [why FeenoX works this way][why] and
not like other FEA tools you might have encountered in the past. If you
are feeling curious, take a look at what FeenoX has to offer to
[hackers][] and [academics][].

  [FeenoX]: https://www.seamplex.com/feenox
  [cloud-first]: https://seamplex.com/feenox/doc/sds.html#cloud-first
  [back end]: https://en.wikipedia.org/wiki/Frontend_and_backend
  [CAEplex]: https://www.caeplex.com
  [why]: #why
  [input files]: https://seamplex.com/feenox/doc/sds.html#sec:input
  [output files]: https://seamplex.com/feenox/doc/sds.html#sec:output
  [tensile test tutorial]: https://www.seamplex.com/feenox/doc/tutorials/110-tensile-test/
  [NAFEMS LE10 “Thick plate pressure” benchmark]: https://www.seamplex.com/feenox/examples/mechanical.html#nafems-le10-thick-plate-pressure-benchmark
  [multi-material problems]: https://seamplex.com/feenox/examples/mechanical.html#two-cubes-compressing-each-other
  [`nafems-le10.geo`]: https://github.com/seamplex/feenox/blob/main/examples/nafems-le10.geo
  [Gmsh]: http://gmsh.info/
  [editors with syntax highlighting]: https://seamplex.com/feenox/doc/sds.html#sec:syntactic
  [`examples`]: https://github.com/seamplex/feenox/tree/main/examples
  [invocation]: https://www.seamplex.com/feenox/doc/feenox-manual.html#running-feenox
  [FeenoX manual]: https://www.seamplex.com/feenox/doc/feenox-manual.html
  [in the cloud]: https://www.seamplex.com/feenox/doc/sds.html#cloud-first
  [in parallel throughout several servers using the MPI standard]: https://seamplex.com/feenox/doc/sds.html#sec:scalability
  [hackers]: README4hackers.md
  [academics]: README4academics.md

# What Feenox can solve

FeenoX can solve the following types of problems:

- [Basic mathematics][]
- [Systems of ODEs/DAEs][]
- [Laplace’s equation][]
- [Heat conduction][]
- [Linear elasticity][]
- [Modal analysis][]
- [Neutron diffusion][]
- [Neutron S<sub>N</sub>][]

Take the tutorials to learn how to solve those types of problems:

0.  [Setting up your workspace][]
1.  [Overview: the tensile test case][]
2.  [Fun & games: solving mazes with PDES instead of AI][]
3.  [Heat conduction][1]

Browse through [the documentation][] to dive deeper into the details.

  [Basic mathematics]: https://seamplex.com/feenox/examples/examples/basic.html
  [Systems of ODEs/DAEs]: https://seamplex.com/feenox/examples/daes.html
  [Laplace’s equation]: https://seamplex.com/feenox/examples/laplace.html
  [Heat conduction]: https://seamplex.com/feenox/examples/thermal.html
  [Linear elasticity]: https://seamplex.com/feenox/examples/mechanical.html
  [Modal analysis]: https://seamplex.com/feenox/examples/modal.html
  [Neutron diffusion]: https://seamplex.com/feenox/examples/neutron_diffusion.html
  [Neutron S<sub>N</sub>]: https://seamplex.com/feenox/examples/neutron_sn.html
  [Setting up your workspace]: https://www.seamplex.com/feenox/doc/tutorials/000-setup
  [Overview: the tensile test case]: https://www.seamplex.com/feenox/doc/tutorials/110-tensile-test
  [Fun & games: solving mazes with PDES instead of AI]: https://www.seamplex.com/feenox/doc/tutorials/120-mazes
  [1]: https://www.seamplex.com/feenox/doc/tutorials/320-thermal
  [the documentation]: https://seamplex.com/feenox/doc/

# Why FeenoX works the way it does

There are two “why” questions we have to answer.

1.  Why is FeenoX different from other “similar” tools?

    Consider again the NAFEMS LE10 case from fig. 1 above. Take some
    time to think (or investigate) how other FEA tools handle this case.
    Note the following features FeenoX provides:

    - [ready-to-run executable][] that reads the problem at runtime (no
      need to compile anything to solve a particular problem)
    - self-explanatory plain-text near-English input file
      - one-to-one correspondence between computer input file and human
        description of the problem
      - Git-traceable (mesh is not part of the input file)
      - everything is an expression
        - material properties can be given as functions of space and/or
          time and/or temperature (or other intermediate variables)
        - these functions can be given as algebraic expresions or come
          from interpolateed point-wise defined data
    - [100% user-defined output][]
    - simple problems need simple inputs
    - similar problems need similar inputs
    - parametric and optimization runs through command-line arguments
    - flexibility to handle many workflows, including [web-based
      interfaces][]

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

2.  Why FeenoX works the way it does?

    Because it is [cloud first][] and its objective is to be flexible
    enough to power web-based interfaces like
    [CAEplex][web-based interfaces] and many other workflows. More
    information in the details for [Unix experts][] and [academic
    professors][].

    Depending on the complexity of the case,
    [CAEplex][web-based interfaces] might be enough or not. If the
    latter, one has to see what’s sitting under the hood. Peek into [the
    documentation][] and [the repository][] for further details.

  [ready-to-run executable]: https://www.seamplex.com/feenox/doc/sds.html#sec:execution
  [100% user-defined output]: https://seamplex.com/feenox/doc/sds.html#sec:output
  [web-based interfaces]: https://www.caeplex.com
  [cloud first]: https://seamplex.com/feenox/doc/sds.html#cloud-first
  [Unix experts]: README4hackers.md
  [academic professors]: README4academics.md
  [the documentation]: https://seamplex.com/feenox/doc/
  [the repository]: https://github.com/seamplex/feenox/
