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
> 2. FeenoX is designed as a [cloud-first](https://seamplex.com/feenox/doc/sds.html#cloud-first) [back end](https://en.wikipedia.org/wiki/Frontend_and_backend) for generic computational workflows. One of these many workflows can be point-and-click graphical interface such as [CAEplex](https://www.caeplex.com) which allows to solve thermo-mechanical problems directly from the browser.
>
> 3. Since it is a cloud-first tool, FeenoX targets at GNU/Linux. If you are using Windows, FeenoX may not be for you (but [CAEplex](https://www.caeplex.com) may). See below to find out [why](#why) FeenoX works the way it does.


In plain engineering terms, FeenoX works like a transfer function between one or more input files and zero or more output files:

```include
doc/transfer.md
```

When solving problems using the finite element method, FeenoX sits in the middle of pre and post-processing tools.
The former should aid the engineer to prepare the mesh and, eventually, the input file.
The latter should show the numerical results. See the [tensile test tutorial](https://www.seamplex.com/feenox/doc/tutorials/110-tensile-test/) for an in-depth step-by-step explanation.

To fix ideas, let us consider the [NAFEMS LE10 "Thick plate pressure" benchmark](https://www.seamplex.com/feenox/examples/mechanical.html#nafems-le10-thick-plate-pressure-benchmark). @Fig:nafems-le10 shows that there is a one-to-one correspondence between the human-friendly problem formulation and the input file FeenoX reads.

![The NAFEMS LE10 problem statement and the corresponding FeenoX input](nafems-le10-problem-input.svg){#fig:nafems-le10 width=100% }

Say we already have a [`nafems-le10.geo`](https://github.com/seamplex/feenox/blob/main/examples/nafems-le10.geo) file which tells [Gmsh](http://gmsh.info/) how to create a mesh `nafems-le10.msh` (check out the [tensile test tutorial](https://www.seamplex.com/feenox/doc/tutorials/110-tensile-test/) for details).
Then, we can create an input file for FeenoX (using [editors with syntax highlighting](https://seamplex.com/feenox/doc/sds.html#syntactic-sugar-highlighting) for example) as follows:

```feenox
PROBLEM mechanical 3D
READ_MESH nafems-le10.msh   # mesh in millimeters

# LOADING: uniform normal pressure on the upper surface
BC upper    p=1      # 1 Mpa

# BOUNDARY CONDITIONS:
BC DCD'C'   v=0      # Face DCD'C' zero y-displacement
BC ABA'B'   u=0      # Face ABA'B' zero x-displacement
BC BCB'C'   u=0 v=0  # Face BCB'C' x and y displ. fixed
BC midplane w=0      #  z displacements fixed along mid-plane

# MATERIAL PROPERTIES: isotropic single-material properties
E = 210e3   # Young modulus in MPa
nu = 0.3    # Poisson's ratio

SOLVE_PROBLEM   # solve!

# print the direct stress y at D (and nothing more)
PRINT "sigma_y @ D = " sigmay(2000,0,300) "MPa"
```

Once we put these two files, `nafems-le10.geo` and `nafems-le10.fee` in the same directory, we call first Gmsh and then FeenoX from the terminal to solve the benchmark problem:

```{=html}
<div id="cast-le10"></div>
<script>AsciinemaPlayer.create('doc/le10.cast', document.getElementById('cast-le10'), {cols:133,rows:32, poster: 'npt:0:3'});</script>
```

The trick is that this workflow is susceptible of being automated and customized to run in the cloud, possibly in parallel throughout several servers using the MPI standard.
Check out this 1-minute video that covers a similar case from the [tensile-test tutorial](https://www.seamplex.com/feenox/doc/tutorials/110-tensile-test/):

:::: {.not-in-format .latex}
![](https://seamplex.com/feenox/doc/tutorials/110-tensile-test/quick.mp4){width=100%}\ 
::::

There is a sound explanation about [why FeenoX works this way](#why) and not like other FEA tools you might have encountered in the past. If you are feeling curious, take a look at what FeenoX has to offer to [hackers](README4hackers.md) and [academics](README4academics.md).


# What can FeenoX solve

FeenoX can solve the following types of problems:

```include
doc/examples-list.md
```

Take the tutorials to learn how to solve those types of problems:
 
```include
doc/tutorials-list.md
```
 
Browse through [the documentation](https://seamplex.com/feenox/doc/) to dive deeper into the details.
 
# Why

There are two "why" questions we have to answer.

 1. Why is FeenoX different from other "similar" tools?
 
    Consider again the NAFEMS LE10 case from @fig:nafems-le10 above.
    Take some time and think (or investigate) how other FEA tools handle this case.
    Note the following features FeenoX provides:
    
     * ready-to-run executable (no need to compile)
     * self-explanatory plain-text near-English input file
       - one-to-one correspondence between computer input file and human description of the problem
       - Git-traceable (mesh is not part of the input file)
       - everything is an expression
         - material properties can be given as functions of space and/or time and/or temperature (or other intermediate variables)
         - these functions can be given as algebraic expresions or come from interpolateed point-wise defined data
     * 100% user-defined output
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
    More information in the details for [Unix experts](README4hackers.md) and [professors](README4academics.md).
    
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



