---
title: FeenoX annotated examples
titleblock: |
 FeenoX annotated examples
 ==========================
lang: en-US
toc: true 
...

# Examples by problem type

The FeenoX examples are divided by the type of problem they solve:

 #. [Basic mathematics](basic.md)
 #. [Systems of ODEs/DAEs](daes.md)
 #. [Laplace’s equation](laplace.md)
 #. [Heat conduction](thermal.md)
 #. [Linear elasticity](mechanical.md)
 #. [Modal analysis](modal.md)
 #. [Neutron diffusion](neutron_diffusion.md)
 #. [Neutron $S_N$](neutron_sn.md)

> Each type of partial differential equation (i.e. from Laplace downward) is implemented in a subdirectory within [`src/pde`](https://github.com/seamplex/feenox/tree/main/src/pdes) of the source tree.
> Feel free to...
>
>  * ask questions in the [Github discussion page](https://github.com/seamplex/feenox/discussions)
>  * propose features, corrections, improvements, etc.
>  * create a pull request for other PDEs. Candidates would be
>    - fluid mechanics
>    - thermal hydraulics
>    - electromagnetism
>    - non-linear elasticity
>    - cell-level neutronics (i.e. method of characteristics, collision probabilities, etc)
 
All the files needed to run theses examples are available in the [examples](https://github.com/seamplex/feenox/tree/main/examples) directory of the [Git repository](https://github.com/seamplex/feenox) and any of the [tarballs](https://www.seamplex.com/feenox/download.html), both [source](https://www.seamplex.com/feenox/dist/src) and [binary](https://www.seamplex.com/feenox/dist/linux).

For a presentation of these examples, see the "FeenoX Overview Presentation" (August 2021).

 * [Recording (audio in Spanish, slides in English)](https://youtu.be/-RJ5qn7E9uE)
 * [Slides in PDF](https://www.seamplex.com/feenox/doc/2021-feenox.pdf)
 * [Markdown examples sources](https://github.com/gtheler/2021-presentation)

# Test suite
 
The `tests` [directory in the Github repository](https://github.com/seamplex/feenox/tree/main/tests) has dozens of test cases which can be used as examples for reference and for mathematical verification of the results obtained with FeenoX.
These are run when doing `make check` after [compiling the source code](../doc/compile.md). 

The test cases usually return a single number (which should be near zero) comparing the numerical result with the expected one.
Feel free to propose benchmark problems to add to the suite.

# Detailed list of examples

```{.include shift-heading-level-by=1}
examples.md
```

