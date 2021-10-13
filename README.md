---
title: FeenoX
subtitle: A free no-fee no-X uniX-like finite-element(ish) computational engineering tool
titleblock: |
 FeenoX: a free no-fee no-X uniX-like finite-element(ish) computational engineering tool
 =======================================================================================
lang: en-US
number-sections: true
toc: true
...


::: {.only-in-format .html .markdown}
> * [FeenoX annotated examples](https://www.seamplex.com/feenox/examples)
> * FeenoX Overview Presentation, August 2021
>   - [Recording (audio in Spanish, slides in English)](https://youtu.be/-RJ5qn7E9uE)
>   - [Slides in PDF](https://www.seamplex.com/feenox/doc/2021-feenox.pdf)
>   - [Markdown examples sources](https://github.com/gtheler/2021-presentation)
:::


# Why FeenoX?

The world is already full of finite-element programs and every day a grad student creates a new one from scratch.
So why adding FeenoX to the already-crowded space of FEA tools?

To better illustrate the point, let us first consider what the options are when an engineer needs to to write a technical report, paper or document:

 Feature                | Microsoft Word |  Google Docs  |  Markdown[^1]    |  (La)TeX
:-----------------------|:--------------:|:-------------:|:------------:|:----------------:
 Aesthetics             |       ❌        |       ❌       |      ✅      |  ✅
 Convertibility (to other formats)         |       😐       |       😐      |      ✅       |  😐
 Traceability           |       ❌        |       😐      |      ✅      |  ✅
 Mobile-friendliness    |       ❌        |       ✅       |      ✅      |  ❌
 Collaborativeness      |       ❌        |       ✅       |      ✅      |  😐
 Licensing/openness     |       ❌        |       ❌       |      ✅      |  ✅
 Non-nerd friendliness  |       ✅        |       ✅      |      😐      |  ❌

[^1]: Here "[Markdown](https://en.wikipedia.org/wiki/Markdown)" means ([Pandoc](https://pandoc.org/) + [Git](https://git-scm.com/) + [Github](https://github.com/) / [Gitlab](https://about.gitlab.com/) / [Gitea](https://gitea.com/}{Gitea}))

 
After analyzing the pros and cons of each alternative, at some point it should be evident that Markdown (plus friends) gives the best trade off. We can then perform a similar analysis for the options available in order to solve an engineering problem casted as a PDE, say by using a finite-element formulation:

 Feature                | Desktop GUIs  |   Web frontends  |  FeenoX[^2]  |  Libraries
:-----------------------|:-------------:|:----------------:|:---------:|:------------:
 Flexibility            |      😐       |         ❌        |     ✅    |      ✅
 Scalability            |      ❌        |         😐       |     ✅    |      ✅
 Traceability           |      ❌        |         😐       |     ✅    |      ✅
 Cloud-friendliness     |      ❌        |         ✅        |     ✅    |      ✅
 Collaborativeness      |      ❌        |         ✅        |     ✅    |      ❌
 Licensing/openness     |   ✅/😐/❌      |         ❌        |     ✅    |      ✅
 Non-nerd friendliness  |      ✅        |         ✅        |     😐   |      ❌
 
[^2]: Here "FeenoX" means ([FeenoX](https://seamplex.com/feenox) + [Gmsh](http://gmsh.info) + [Paraview](https://www.paraview.org/) + [Git](https://git-scm.com/) + [Github](https://github.com/) / [Gitlab](https://about.gitlab.com/) / [Gitea](https://gitea.com/}{Gitea}))


Therefore, on the one hand, FeenoX is---in a certain sense---to desktop FEA programs (like Code Aster with Salome or CalculiX with PrePoMax) and libraries (like MoFEM or Sparselizard) what Markdown is to Word and (La)TeX, respectively and deliberately.


On the other hand, FeenoX meets a fictitious-yet-plausible [Software Requirement Specifications](https://www.seamplex.com/feenox/doc/srs.html) that no other single tool (that I am aware of) meets completely. The FeenoX [Software Design Specifications](https://www.seamplex.com/feenox/doc/sds.html) address each requirement of the SRS.
The first and more important requirement is that FeenoX is both free (as in freedom) and open source. See [Licensing].


If by "Why FeenoX?" you mean "Why is FeenoX named that way?," read the [FAQs](doc/FAQs.md).


# What is FeenoX?

```{.include}
doc/introduction.md
```


# Download

```include
doc/download.md
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

FeenoX is copyright ©2009-2021 [Seamplex](https://www.seamplex.com)  
FeenoX is licensed under [GNU GPL version 3](http://www.gnu.org/copyleft/gpl.html) or (at your option) any later version.  
FeenoX is free software: you are free to change and redistribute it.  
There is **NO WARRANTY**, to the extent permitted by law.  
See the [copying conditions](COPYING).  
