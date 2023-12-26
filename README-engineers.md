---
title: FeenoX for Engineers
titleblock: |
 FeenoX for Engineers
 ====================
lang: en-US
number-sections: true
toc: true
...

## How FeenoX works

> **Heads up** First things first:
>
> 1. FeenoX is not a traditional point-and-click finite-element solver. It **does not include** a graphical interface.
> 
> 2. FeenoX is designed as a cloud-first [back end](https://en.wikipedia.org/wiki/Frontend_and_backend) for generic computational workflows. One of these many workflows can be point-and-click graphical interface such as [CAEplex](https://www.caeplex.com) which allows to solve thermo-mechanical problems directly from the browser.
>
> 3. Since it is a cloud-first tool, FeenoX targets at GNU/Linux. If you are using Windows, FeenoX may not be for you (but [CAEplex](https://www.caeplex.com) may). Watch this video for a quick explanation about _why_ FeenoX works the way it works: https://www.youtube.com/watch?v=66WvYTb4pSg


In plain engineering terms, FeenoX works like a transfer function between one or more input files and zero or more output files:

```include
transfer.md
```

When solving problems using the finite element method, FeenoX sits in the middle of pre and post-processing tools.
The former should aid the engineer to prepare the mesh and, eventually, the input file.
The latter should show the numerical results. See the tutorials.


## What

Examples

Tutorials

Tests

## Why

Why it works the way it does?
Because it is cloud first and its objective is to power web-based interfaces like CAEplex.
Depending on the complexity of the case, CAEplex might be enough or not. In the latter, one has to see what's sitting under the hood.

> ::::: {.container .text-center .my-5 .ratio .ratio-16x9}
> <iframe class="embed-responsive-item" src="https://www.youtube.com/embed/kD3tQdq17ZE" allowfullscreen></iframe>
> :::::


> ::::: {.container .text-center .my-5 .ratio .ratio-16x9}
> <iframe class="embed-responsive-item" src="https://www.youtube.com/embed/ylXAUAsfb5E" allowfullscreen></iframe>
> :::::





