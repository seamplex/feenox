---
title: FeenoX documentation index
titleblock: |
 FeenoX documentation index
 ==========================
lang: en-US
...

> FeenoX documentation is released under the terms of the [GNU Free Documentation License v1.3](https://www.gnu.org/licenses/fdl-1.3.html), or any later version.

Every bit of FeenoX documentation is written in [Pandoc](https://pandoc.org/)-flavored Markdown in the [`doc`](https://github.com/seamplex/feenox/tree/main/doc) directory of the [FeenoX repository](https://github.com/seamplex/feenox/).
It is then converted to HTML, PDF, Unix manpage or Texinfo as needed.


# Manual and description


As per the [GNU Coding Standards](https://www.gnu.org/prep/standards/standards.html#GNU-Manuals), "a manual should serve both as tutorial and reference." Due to the formatting restrictions, the [Texinfo](https://www.gnu.org/software/texinfo/) version contains only the description and not the full reference.

 * [FeenoX manual](https://www.seamplex.com/feenox/doc/feenox-manual.html) [PDF](https://www.seamplex.com/feenox/doc/feenox-manual.pdf)
 * [FeenoX description](https://www.seamplex.com/feenox/doc/feenox-desc.html) [PDF](https://www.seamplex.com/feenox/doc/feenox-desc.pdf) [Texinfo](https://www.seamplex.com/feenox/doc/feenox-desc.texi)


## Design

 1. The FeenoX project starts as an offer to an imaginary “request for quotations” that defines [software requirements specifications](https://seamplex.com/feenox/doc/srs.html) for an open source computational tool.

     * [Software Requirements specifications](https://seamplex.com/feenox/doc/srs.html) (Fictitious RFQ) [[PDF]](https://www.seamplex.com/feenox/doc/srs.pdf)

 2. Then a fictitious “offer” to the above tender is given in a [software design specifications](https://seamplex.com/feenox/doc/srs.html) document that explains the design decisions and features included in FeenoX.

     * [Software Design specifications](https://seamplex.com/feenox/doc/sds.html) (Imaginary FeenoX's offer) [[PDF]](https://www.seamplex.com/feenox/doc/sds.pdf)

 
::::::::: {.only-in-format .html .markdown}
> **Why FeenoX works the way it works** (i.e. why it does not run in Windows) 
>
> ::::: {.container .text-center .my-5 .ratio .ratio-16x9}
> <iframe class="embed-responsive-item" src="https://www.youtube.com/embed/66WvYTb4pSg?rel=0" allowfullscreen></iframe>
> :::::
::::::::: 
 



    
## Other documents
 
 * [Unix manpage](https://www.seamplex.com/feenox/doc/feenox.1.html) (accessible with `man feenox` after (global) installation with `sudo make install`)
 * [Frequently Asked Questions](https://www.seamplex.com/feenox/doc/FAQs.html) [PDF](https://www.seamplex.com/feenox/doc/FAQs.pdf)
 * [Compilation Guide](https://www.seamplex.com/feenox/doc/compilation.html) [PDF](https://www.seamplex.com/feenox/doc/compilation.pdf)
 * [Programming Guide](https://www.seamplex.com/feenox/doc/programming.html) [PDF](https://www.seamplex.com/feenox/doc/programming.pdf)
 * [History](https://www.seamplex.com/feenox/doc/history.html) [PDF](https://www.seamplex.com/feenox/doc/history.pdf)

## Youtube videos {#sec:youtube}

 * [FeenoX hands-on tutorial](https://www.youtube.com/watch?v=b3K2QewI8jE) 
 * [FeenoX, a cloud-first free and open source finite-element(ish) tool](https://www.youtube.com/watch?v=EZSjFzJAhbw) 
 * [Why FeenoX works the way it works](https://www.youtube.com/watch?v=66WvYTb4pSg)
 * [Verification of PDE solvers using the Method of Manufactured Solutions](https://www.youtube.com/watch?v=mGNwL8TGijg)
 * [FeenoX: a free and open source computational tool for solving (nuclear-related) DAEs in the cloud](https://www.youtube.com/watch?v=e8kFmFOsbPk)
 * [CAEplex: finite elements in the cloud](https://www.youtube.com/watch?v=DOnoXo_MCZg)
 * [Thermal finite elements in the cloud in 1 minute](https://www.youtube.com/watch?v=mOxQeIk2WJA)
 * [Add CAE to your Onshape CAD with CAEplex](https://www.youtube.com/watch?v=ylXAUAsfb5E)
 * [First 10 natural modes of a cylinder in less than 1 min. with CAEplex](https://www.youtube.com/watch?v=kD3tQdq17ZE)
 * [Democratizing CAE](https://www.youtube.com/watch?v=7KqiMbrSLDc)
    




## Markdown sources

The Markdown sources are in the [`doc`](https://github.com/seamplex/feenox/tree/main/doc):

 * [`feenox-manual`](https://github.com/seamplex/feenox/blob/main/doc/feenox-desc.md)
 * [`feenox-desc`](https://github.com/seamplex/feenox/blob/main/doc/feenox-desc.md)
 * [`srs`](https://github.com/seamplex/feenox/blob/main/doc/srs.md)
 * [`sds`](https://github.com/seamplex/feenox/blob/main/doc/sds.md)
 * [`FAQ`](https://github.com/seamplex/feenox/blob/main/doc/FAQ.md)
 * [`feenox.1`](https://github.com/seamplex/feenox/blob/main/doc/feenox.1.md)
 * [`history`](https://github.com/seamplex/feenox/blob/main/doc/history.md)
 * [`compilation`](https://github.com/seamplex/feenox/blob/main/doc/compilation.md)
 * [`programming`](https://github.com/seamplex/feenox/blob/main/doc/programming.md) 
 
They are compiled by running the [`make.sh`](https://github.com/seamplex/feenox/blob/main/doc/make.sh) script.
 
 
# Hands on

Go directly to the point and see how to solve problems with FeenoX.
Everything needed (input files, geometry files, meshes, data, scripts, etc.) is included in the [FeenoX repository](https://github.com/seamplex/feenox/).

## Examples

[Annotated examples](https://www.seamplex.com/feenox/examples) can be found in the [examples](https://github.com/seamplex/feenox/tree/main/examples) directory of the Github repository.

 * [Online annotated examples](https://www.seamplex.com/feenox/examples)
     - [Basic mathematics](https://seamplex.com/feenox/examples/basic.html)
     - [Systems of ODEs/DAEs](https://seamplex.com/feenox/examples/daes.html)
     - [Laplace’s equation](https://seamplex.com/feenox/examples/laplace.html)
     - [Heat conduction](https://seamplex.com/feenox/examples/thermal.html)
     - [Linear elasticity](https://seamplex.com/feenox/examples/mechanical.html)
     - [Modal analysis](https://seamplex.com/feenox/examples/modal.html)
     - [Neutron diffusion](https://seamplex.com/feenox/examples/neutron_diffusion.html)
     - [Neutron S<sub>N</sub>](https://seamplex.com/feenox/examples/neutron_sn.html)
 * [Github examples directory](https://github.com/seamplex/feenox/tree/main/examples)

 
## Tutorials


```{.include shift-heading-level-by=2}
tutorials/setup.md
```
 
 
```{.include shift-heading-level-by=2}
tutorials/general.md
```

```{.include shift-heading-level-by=2}
tutorials/detailed.md
```

```{.include shift-heading-level-by=2}
tutorials/physics.md
```

## Tests
 
The [`tests`](https://github.com/seamplex/feenox/tree/main/tests) directory in the repository has hundreds of
 
 - `grep`-able examples,
 - unit tests,
 - regression tests, and/or
 - (crude) mathematical verification tests.
 

# Contributing

## Asking questions & reporting bugs

 * Use [Github discussions](https://github.com/seamplex/feenox/discussions) to ask for help,
 * Use the [Github issue tracker](https://github.com/seamplex/feenox/issues) to report bugs.

  
## Contributing guidelines

```include
contributing-guidelines.md
```

## Adding a new PDE to `src/pdes`

To be explained.
