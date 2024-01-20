---
title: FeenoX for Engineers
titleblock: |
 FeenoX for Engineers
 ====================
lang: en-US
number-sections: true
toc: true
...

# How FeenoX works

> **Heads up!** First things first:
>
> 1. [FeenoX](https://www.seamplex.com/feenox) is not a traditional point-and-click finite-element solver. It **does not include** a graphical interface.
> 
> 2. FeenoX is designed as a [cloud-first](https://seamplex.com/feenox/doc/sds.html#cloud-first) [back end](https://en.wikipedia.org/wiki/Frontend_and_backend) for generic computational workflows to solve engineering-related problems. One of these many workflows can be point-and-click graphical interface such as [CAEplex](https://www.caeplex.com) which allows to solve thermo-mechanical problems directly from the browser.
>
> 3. Since it is a cloud-first tool, FeenoX targets at GNU/Linux. If you are using Windows, FeenoX may not be for you (but [CAEplex](https://www.caeplex.com) may). See below to find out [why](#why) FeenoX works the way it does.


In plain engineering terms, FeenoX works like a transfer function between one or more [input files](https://seamplex.com/feenox/doc/sds.html#sec:input) and zero or more [output files](https://seamplex.com/feenox/doc/sds.html#sec:output):

```include
doc/transfer.md
```

When solving problems using the finite element method, FeenoX sits in the middle of pre and post-processing tools.
The former should aid the engineer to prepare the mesh and, eventually, the input file.
The latter should show the numerical results. See the [tensile test tutorial](https://www.seamplex.com/feenox/doc/tutorials/110-tensile-test/) for an in-depth step-by-step explanation.

To fix ideas, let us consider the [NAFEMS LE10 "Thick plate pressure" benchmark](https://www.seamplex.com/feenox/examples/mechanical.html#nafems-le10-thick-plate-pressure-benchmark). @Fig:nafems-le10 shows that there is a one-to-one correspondence between the human-friendly problem formulation and the input file FeenoX reads.
There is no need to give extra settings if the problem does not ask for them.
Note that since the problem has only one volume, `E` means "the" Young modulus.
No need to deal with a map between materials and mesh entitites (in this case the mapping is not needed but in [multi-material problems](https://seamplex.com/feenox/examples/mechanical.html#two-cubes-compressing-each-other) the mapping is needed indeed). Nothing more, nothing less.

![The NAFEMS LE10 problem statement and the corresponding FeenoX input](nafems-le10-problem-input.svg){#fig:nafems-le10 width=100% }

Say we already have a [`nafems-le10.geo`](https://github.com/seamplex/feenox/blob/main/examples/nafems-le10.geo) file which tells [Gmsh](http://gmsh.info/) how to create a mesh `nafems-le10.msh` (check out the [tensile test tutorial](https://www.seamplex.com/feenox/doc/tutorials/110-tensile-test/) for details).
Then, we can create an input file for FeenoX (using [editors with syntax highlighting](https://seamplex.com/feenox/doc/sds.html#sec:syntactic) for example) as follows:

```{.feenox include="doc/nafems-le10.fee"}
```


> **Heads up!** The `.msh` file from Gmsh can be either
> 
>  a. version 2.2, or
>  b. version 4.0, or
>  c. version 4.1.
>
> and can be partitioned or not.

Once we put these two files, `nafems-le10.geo` and `nafems-le10.fee` in the same directory, say in the [`examples`](https://github.com/seamplex/feenox/tree/main/examples) directory of the repository, then we call first Gmsh and then FeenoX from the terminal to solve the benchmark problem:

:::::: {.only-in-format .html }
```{=html}
<div id="cast-le10"></div>
<script>AsciinemaPlayer.create('doc/le10.cast', document.getElementById('cast-le10'), {cols:133,rows:32, poster: 'npt:0:3'});</script>
```
::::::

Check out the section about [invocation](https://www.seamplex.com/feenox/doc/feenox-manual.html#running-feenox) in the [FeenoX manual](https://www.seamplex.com/feenox/doc/feenox-manual.html).

The trick is that this workflow is susceptible of being automated and customized to run [in the cloud](https://www.seamplex.com/feenox/doc/sds.html#cloud-first), possibly [in parallel throughout several servers using the MPI standard](https://seamplex.com/feenox/doc/sds.html#sec:scalability).
Check out this 1-minute video that covers a similar case from the [tensile-test tutorial](https://www.seamplex.com/feenox/doc/tutorials/110-tensile-test/):

:::: {.not-in-format .latex}
![](https://seamplex.com/feenox/doc/tutorials/110-tensile-test/quick.mp4){width=100%}\ 
::::

There is a sound explanation about [why FeenoX works this way](#why) and not like other FEA tools you might have encountered in the past. If you are feeling curious, take a look at what FeenoX has to offer to [hackers](README4hackers.md) and [academics](README4academics.md).


# What Feenox can solve

FeenoX can solve the following types of problems:

```include
doc/examples-list.md
```

Take the tutorials to learn how to solve those types of problems:
 
```include
doc/tutorials-list.md
```
 
Browse through [the documentation](https://seamplex.com/feenox/doc/) to dive deeper into the details.
 
# Why FeenoX works the way it does

There are two "why" questions we have to answer.

 1. Why is FeenoX different from other "similar" tools?
 
    Consider again the NAFEMS LE10 case from @fig:nafems-le10 above.
    Take some time to think (or investigate) how other FEA tools handle this case.
    Note the following features FeenoX provides:
    
     * [ready-to-run executable](https://www.seamplex.com/feenox/doc/sds.html#sec:execution) that reads the problem at runtime (no need to compile anything to solve a particular problem)
     * self-explanatory plain-text near-English input file
       - one-to-one correspondence between computer input file and human description of the problem
       - Git-traceable (mesh is not part of the input file)
       - everything is an expression
         - material properties can be given as functions of space and/or time and/or temperature (or other intermediate variables)
         - these functions can be given as algebraic expresions or come from interpolateed point-wise defined data
     * [100% user-defined output](https://seamplex.com/feenox/doc/sds.html#sec:output)
     * simple problems need simple inputs
     * similar problems need similar inputs
     * parametric and optimization runs through command-line arguments
     * flexibility to handle many workflows, including [web-based interfaces](https://www.caeplex.com)
     
    > ::::: {.container .text-center .my-5 .ratio .ratio-16x9}
    > <iframe class="embed-responsive-item" src="https://www.youtube.com/embed/kD3tQdq17ZE" allowfullscreen></iframe>
    > :::::
     
    > ::::: {.container .text-center .my-5 .ratio .ratio-16x9}
    > <iframe class="embed-responsive-item" src="https://www.youtube.com/embed/ylXAUAsfb5E" allowfullscreen></iframe>
    > :::::

 2. Why FeenoX works the way it does?
 
    Because it is [cloud first](https://seamplex.com/feenox/doc/sds.html#cloud-first) and its objective is to be flexible enough to power web-based interfaces like [CAEplex](https://www.caeplex.com) and many other workflows.
    More information in the details for [Unix experts](README4hackers.md) and [academic professors](README4academics.md).
    
    Depending on the complexity of the case, [CAEplex](https://www.caeplex.com) might be enough or not.
    If the latter, one has to see what's sitting under the hood.
    Peek into [the documentation](https://seamplex.com/feenox/doc/) and [the repository](https://github.com/seamplex/feenox/) for further details.

    > ::::::::: {.only-in-format .html .markdown}
    > Here is a 5-min explanation of why it works this way, and why it is not expected to run in Windows (although it _can_ run in Windows if you try hard enough, but it is not worth it):
    > 
    > ::::: {.container .text-center .my-5 .ratio .ratio-16x9}
    > <iframe class="embed-responsive-item" src="https://www.youtube.com/embed/66WvYTb4pSg?rel=0" allowfullscreen></iframe>
    > :::::
    > ::::::::: 


::::: {.only-in-format .html }
```{=html}
<a class="btn btn-lg btn-outline-secondary" href="./README4hackers.md"   role="button">FeenoX for Unix Hackers</a>
<a class="btn btn-lg btn-outline-info"      href="./README4academics.md" role="button">FeenoX for Academic Professors</a>
```
:::::

