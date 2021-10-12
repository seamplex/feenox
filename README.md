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

# What is FeenoX?

```{.include}
doc/introduction.md
```


## Why?

The world is already full of finite-element programs and every day a grad student creates a new one from scratch.
Let us first think what the options to write a technical report, paper or document are:

 Feature                | Microsoft Word |  Google Docs  |  Markdown[^1]    |  (La)TeX
:-----------------------|:--------------:|:-------------:|:------------:|:----------------:
 Aesthetics             |       ❌        |       ❌       |      ✅      |  ✅
 Convertibility (to other formats)         |       --       |       --      |      ✅       |  --
 Traceability           |       ❌        |       --      |      ✅      |  ✅
 Mobile-friendliness    |       ❌        |       ✅       |      ✅      |  ❌
 Collaborativeness      |       ❌        |       ✅       |      ✅      |  --
 Licensing/openness     |       ❌        |       ❌       |      ✅      |  ✅
 Non-nerd friendliness  |       ✅        |       ✅      |      --      |  ❌

[^1]: Here [Markdown](https://en.wikipedia.org/wiki/Markdown) means ([Pandoc](https://pandoc.org/) + [Git](https://git-scm.com/) + [Github](https://github.com/) / [Gitlab](https://about.gitlab.com/) / [Gitea](https://gitea.com/}{Gitea}))

 
After analyzing the pros and cons of each alternative, we can then perform a similar analysis for the options available in order to solve an engineering problem, say a finite-element analysis:

 Feature                | Desktop GUIs  |   Web frontends  |  FeenoX[^2]  |  Libraries
:-----------------------|:-------------:|:----------------:|:---------:|:------------:
 Flexibility            |      --       |         ❌        |     ✅    |      ✅
 Scalability            |      ❌        |         --       |     ✅    |      ✅
 Traceability           |      ❌        |         --       |     ✅    |      ✅
 Cloud-friendliness     |      ❌        |         ✅        |     ✅    |      ✅
 Collaborativeness      |      ❌        |         ✅        |     --   |      ❌
 Licensing/openness     |   ✅/--/❌      |         ❌        |     ✅    |      ✅
 Non-nerd friendliness  |      ✅        |         ✅        |     --   |      ❌
 
[^2]: Here FeenoX means ([FeenoX](https://seamplex.com/feenox) + [Gmsh](http://gmsh.info) + [Paraview](https://www.paraview.org/) + [Git](https://git-scm.com/) + [Github](https://github.com/) / [Gitlab](https://about.gitlab.com/) / [Gitea](https://gitea.com/}{Gitea}))


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
