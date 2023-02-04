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




::: {.only-in-format .html .markdown}
> **News**
>
> * [Debian/Ubuntu `.deb` packages available](download.html#debian) for FeenoX version\ 0.3.
>
> * A cloud-first approach for solving core-level neutron transport. 8a. Reunión Anual GArCyAR 2022, December 2022.
>   - [Recordings](https://www.youtube.com/watch?v=rdZtyMf5m0k)
>   - [Slides in PDF](8a. Reunión Anual GArCyAR 2022)
>   - [Markdown examples sources](https://github.com/gtheler/2022-garcar)
>
>
> * FeenoX, a cloud-first free and open source finite-element(ish) tool. Science Circle, on-line course. November 2022.
>   - [Recordings](https://youtu.be/EZSjFzJAhbw)
>   - [Markdown examples sources](https://github.com/gtheler/2022-feenox-sci-circle)
>
>   ::::: {.container .text-center .my-5 .ratio .ratio-16x9}
>   <iframe class="embed-responsive-item" src="https://www.youtube.com/embed/EZSjFzJAhbw?rel=0" allowfullscreen></iframe>
>   :::::
>
> * FeenoX hands-on tutorial. Science Circle, on-line course. October 2022 <https://www.youtube.com/watch?v=b3K2QewI8jE>
>
>   ::::: {.container .text-center .my-5 .ratio .ratio-16x9}
>   <iframe class="embed-responsive-item" src="https://www.youtube.com/embed/b3K2QewI8jE?rel=0" allowfullscreen></iframe>
>   :::::
> 
> * Why FeenoX works the way it works (i.e. does not run in Windows):
>
>   ::::: {.container .text-center .my-5 .ratio .ratio-16x9}
>   <iframe class="embed-responsive-item" src="https://www.youtube.com/embed/66WvYTb4pSg?rel=0" allowfullscreen></iframe>
>   :::::
>
> * [FeenoX tutorials](doc/tutorials):
>   - [Setting up your workspace](doc/tutorials/000-setup)
>   - [Tensile test](doc/tutorials/110-tensile-test)
>   - [Solving mazes with PDES instead of AI](doc/tutorials/120-mazes)
>
> * FeenoX for nuclear-related problems Presentation, December 2021
>   - [Recording with CCs](https://youtu.be/e8kFmFOsbPk)
>   - [Slides in PDF](https://www.seamplex.com/feenox/doc/2021-brasil.pdf)
>   - [Markdown examples sources](https://github.com/gtheler/2021-brasil)
>
> * FeenoX Overview Presentation, August 2021
>   - [Recording (audio in Spanish, slides in English)](https://youtu.be/-RJ5qn7E9uE)
>   - [Slides in PDF](https://www.seamplex.com/feenox/doc/2021-feenox.pdf)
>   - [Markdown examples sources](https://github.com/gtheler/2021-presentation)
:::


# Why FeenoX?

The world is already full of finite-element programs and every day a grad student creates a new one from scratch.
So why adding FeenoX to the already-crowded space of FEA tools?

```{.include}
doc/why.md
```


On the other hand, FeenoX meets a fictitious-yet-plausible [Software Requirement Specifications](https://www.seamplex.com/feenox/doc/srs.html) that no other single tool (that I am aware of) meets completely. The FeenoX [Software Design Specifications](https://www.seamplex.com/feenox/doc/sds.html) address each requirement of the SRS.
The two more important design-basis features are that FeenoX is...

 1. a cloud-first computational tool (not just cloud _friendly,_ but cloud **first**).
 2. both free (as in freedom) and open source. See [Licensing].


If by "Why FeenoX?" you mean "Why is FeenoX named that way?," read the [FAQs](doc/FAQ.md).


# What is FeenoX?

```{.include}
doc/introduction.md
```


# Download

```include
doc/downloads.md
```

## Git repository

```include
doc/git.md
```

See the [detailed compilation instructions](doc/compilation.md) for further details.

# Licensing

FeenoX is distributed under the terms of the [GNU General Public License](http://www.gnu.org/copyleft/gpl.html) version 3 or (at your option) any later version. The following text was borrowed from the [Gmsh documentation](http://gmsh.info/doc/texinfo/gmsh.html#Copying-conditions). Replacing “Gmsh” with “FeenoX” gives:

> ```include
> doc/freesw.md
> ```

```include
doc/licensing.md
```


# Further information

Home page: <https://www.seamplex.com/feenox>  
Repository: <https://github.com/seamplex/feenox>  
Bug reporting: <https://github.com/seamplex/feenox/issues>  
Discussions: <https://github.com/seamplex/feenox/discussions>  
Follow us: [YouTube](https://www.youtube.com/channel/UCC6SzVLxO8h6j5rLlfCQPhA)
           [LinkedIn](https://www.linkedin.com/company/seamplex/)
           [Github](https://github.com/seamplex)

---------------------------

FeenoX is copyright ©2009-2022 [Seamplex](https://www.seamplex.com)  
FeenoX is licensed under [GNU GPL version 3](http://www.gnu.org/copyleft/gpl.html) or (at your option) any later version.  
FeenoX is free software: you are free to change and redistribute it.  
There is **NO WARRANTY**, to the extent permitted by law.  
See the [copying conditions](COPYING).  
