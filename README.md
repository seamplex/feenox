---
title: FeenoX
subtitle: A cloud-first free no-fee no-X uniX-like finite-element(ish) computational engineering tool
titleblock: |
 FeenoX: A cloud-first free no-fee no-X uniX-like finite-element(ish) computational engineering tool
 =======================================================================================
lang: en-US
number-sections: true
toc: true
...

# About FeenoX

FeenoX is a cloud-first free and open source tool to solve engineering-related problems using a computer (or many computers in parallel) with a particular design basis.

 * See the [examples](https://www.seamplex.com/feenox/examples/) for an overview of the type of problems FeenoX can solve:

    #. [Basic mathematics](https://seamplex.com/feenox/examples/basic.html)
    #. [Systems of ODEs/DAEs](https://seamplex.com/feenox/examples/daes.html)
    #. [Laplace’s equation](https://seamplex.com/feenox/examples/laplace.html)
    #. [Heat conduction](https://seamplex.com/feenox/examples/thermal.html)
    #. [Linear elasticity](https://seamplex.com/feenox/examples/mechanical.html)
    #. [Modal analysis](https://seamplex.com/feenox/examples/modal.html)
    #. [Neutron diffusion](https://seamplex.com/feenox/examples/neutron_diffusion.html)
    #. [Neutron $S_N$](https://seamplex.com/feenox/examples/neutron_sn.html)

 * Take the [tutorials](https://www.seamplex.com/feenox/doc/tutorials/) to learn how to solve them yourself.
 
 * Choose your background for further details about the what, how and whys:

   - [Industry Engineer](README-engineers.md)
   - [Unix Hacker](README-hackers.md)
   - [Academic Professor](README-academics.md)

 * Make sure you also read the [FAQs](doc/FAQ.md), including the question about what the name means.

## As seen on  "The Science Circle"
 
> ::::: {.container .text-center .my-5 .ratio .ratio-16x9}
> <iframe class="embed-responsive-item" src="https://www.youtube.com/embed/EZSjFzJAhbw?rel=0" allowfullscreen></iframe>
> :::::

 
# Quickstart

## Download

```include
doc/downloads.md
```

## Git repository

```include
doc/git.md
```

See the [download page](https://seamplex.com/feenox/download.html) for more detailed information.


# Examples

 * The [examples page](https://seamplex.com/feenox/examples) contains dozens of curated and annotated cases.

 * All the files needed to run theses examples are available in the [examples](https://github.com/seamplex/feenox/tree/main/examples) directory of the [Git repository](https://github.com/seamplex/feenox).

 * Each type of partial differential equation (i.e. from Laplace downward) is implemented in a subdirectory within [`src/pde`](https://github.com/seamplex/feenox/tree/main/src/pdes) of the source tree.
See the [contributing](#sec:contributing) section below for further instructions.

 * Feel free to ask questions in the [Github discussion page](https://github.com/seamplex/feenox/discussions).

## Tutorials (work in progress)


> ::::: {.container .text-center .my-5 .ratio .ratio-16x9}
> <iframe class="embed-responsive-item" src="https://www.youtube.com/embed/b3K2QewI8jE?rel=0" allowfullscreen></iframe>
> :::::

All the files needed are available in the [examples](https://github.com/seamplex/feenox/tree/main/examples) directory of the [Git repository](https://github.com/seamplex/feenox)

```{.include shift-heading-level-by=1}
setup.md
```
 
 
```{.include shift-heading-level-by=1}
general.md
```

```{.include shift-heading-level-by=1}
detailed.md
```

```{.include shift-heading-level-by=1}
physics.md
```

 

## Test suite
 
 * The `tests` [directory in the Github repository](https://github.com/seamplex/feenox/tree/main/tests) has hundreds of test cases which are used as
   - examples,
   - unit tests,
   - regression tests, and/or
   - (crude) mathematical verification tests.

 * These tests are run when doing `make check` after [compiling the source code](doc/compile.md). 

 * The test cases usually return a single number (which should be near zero) comparing the numerical result with the expected one.
 
 * Feel free to propose benchmark problems to add to the suite in the [Github discussion page](https://github.com/seamplex/feenox/discussions).

 * See [`tests/mms`](https://github.com/seamplex/feenox/tree/main/tests/mms) for verification cases using the Method of Manufactured Solutions.


# Licensing {#sec:licensing}

FeenoX is distributed under the terms of the [GNU General Public License](http://www.gnu.org/copyleft/gpl.html) version 3 or (at your option) any later version. The following text was borrowed from the [Gmsh documentation](http://gmsh.info/doc/texinfo/gmsh.html#Copying-conditions). Replacing “Gmsh” with “FeenoX” gives:

> ```include
> doc/freesw.md
> ```

```include
doc/licensing.md
```

## Contributing {#sec:contributing}

Contributions from hackers and/or academics are welcome, especially new types of PDEs and new formulations of existing PDEs.
For elliptic operators feel free to use the Laplace equation at `src/pdes/laplace` as a template.

 1. Read [programming guide](doc/programming.md).
 2. Browse [Github discussions](https://github.com/seamplex/feenox/discussions) and open a new thread explaining what you want to do and/or asking for help.
 3. Fork the [Git repository](https://github.com/seamplex/feenox/) under your Github account
 4. Create a pull request, including
    - code,
    - documentation, and
    - tests.
 5. Follow up the review procedure.

Note that

 * It is mandatory to observe the [Code of Conduct](CODE_OF_CONDUCT.md).
 * The contributed code has to be compatible with the [GPLv3+ license](#sec:licensing).
 * Each author keeps the copyright of the contribution.


# Further information

Home page: <https://www.seamplex.com/feenox>  
Repository: <https://github.com/seamplex/feenox>  
Bug reporting: <https://github.com/seamplex/feenox/issues>  
Discussions: <https://github.com/seamplex/feenox/discussions>  
Follow us: [YouTube](https://www.youtube.com/channel/UCC6SzVLxO8h6j5rLlfCQPhA)
           [LinkedIn](https://www.linkedin.com/company/seamplex/)
           [Github](https://github.com/seamplex)

---------------------------

FeenoX is copyright ©2009-2023 [Seamplex](https://www.seamplex.com)  
FeenoX is licensed under [GNU GPL version 3](http://www.gnu.org/copyleft/gpl.html) or (at your option) any later version.  
FeenoX is free software: you are free to change and redistribute it.  
There is **NO WARRANTY**, to the extent permitted by law.  
