To better illustrate FeenoX’s unfair advantage (in the entrepreneurial sense), let us first consider what the options are when we need to write a technical report, paper or document:

::: {.not-in-format .latex}
 Feature                               | Microsoft Word |  Google Docs  |  Markdown[^1]    |  (La)TeX
:--------------------------------------|:--------------:|:-------------:|:------------:|:----------------:
 Aesthetics                            |       ❌        |       ❌       |      ✅      |  ✅
 Convertibility (to other formats)     |       😐       |       😐      |      ✅       |  😐
 Traceability                          |       ❌        |       😐      |      ✅      |  ✅
 Mobile-friendliness                   |       ❌        |       ✅       |      ✅      |  ❌
 Collaborativeness                     |       ❌        |       ✅       |      ✅      |  😐
 Licensing/openness                    |       ❌        |       ❌       |      ✅      |  ✅
 Non-nerd friendliness                 |       ✅        |       ✅      |      😐      |  ❌
:::

::: {.only-in-format .latex}
 Feature                               |     Microsoft Word    |       Google Docs    |      Markdown[^1]  |     (La)TeX
:--------------------------------------|:---------------------:|:--------------------:|:------------------:|:----------------:
 Aesthetics                            |       $\times$        |       $\times$       |      $\checkmark$      |  $\checkmark$
 Convertibility (to other formats)     |       $\sim$          |       $\sim$         |      $\checkmark$      |  $\sim$
 Traceability                          |       $\times$        |       $\sim$         |      $\checkmark$      |  $\checkmark$
 Mobile-friendliness                   |       $\times$        |       $\checkmark$       |      $\checkmark$      |  $\times$
 Collaborativeness                     |       $\times$        |       $\checkmark$       |      $\checkmark$      |  $\sim$
 Licensing/openness                    |       $\times$        |       $\times$       |      $\checkmark$      |  $\checkmark$
 Non-nerd friendliness                 |       $\checkmark$        |       $\checkmark$       |      $\sim$        |  $\times$
:::


[^1]: Here “[Markdown](https://en.wikipedia.org/wiki/Markdown)” means ([Pandoc](https://pandoc.org/) + [Git](https://git-scm.com/) + [Github](https://github.com/) / [Gitlab](https://about.gitlab.com/) / [Gitea](https://gitea.com/}{Gitea}))

 
After analyzing the pros and cons of each alternative, at some point it should be evident that [Markdown](https://en.wikipedia.org/wiki/Markdown) (plus friends) gives the best trade off. We can then perform a similar analysis for the options available in order to solve an engineering problem casted as a partial differential equation, say by using a finite-element formulation:

::: {.not-in-format .latex}
 Feature                               | Desktop GUIs  |   Web frontends  |  FeenoX[^2]  |  Libraries
:--------------------------------------|:--------------:|:-------------:|:------------:|:----------------:
 Flexibility                           |     ❌/😐      |        ❌/😐      |     ✅    |      ✅
 Scalability                           |      ❌        |         😐       |     ✅    |      ✅
 Traceability                          |      ❌        |         😐       |     ✅    |      ✅
 Cloud-friendliness                    |      ❌        |         ✅        |     ✅    |      ✅
 Collaborativeness                     |      ❌        |         ✅        |     ✅    |      😐
 Licensing/openness                    |   ✅/😐/❌      |         ❌        |     ✅    |      ✅
 Non-nerd friendliness                 |      ✅        |         ✅        |     😐   |      ❌
:::

::: {.only-in-format .latex}
 Feature                           | Desktop GUIs  |   Web frontends  |  FeenoX[^2]  |  Libraries
:--------------------------------------|:------------------------:|:-----------------------:|:------------------:|:----------------:
 Flexibility                           |      $\sim$              |         $\times$        |     $\checkmark$       |      $\checkmark$
 Scalability                           |      $\times$            |         $\sim$          |     $\checkmark$       |      $\checkmark$
 Traceability                          |      $\times$            |         $\sim$          |     $\checkmark$       |      $\checkmark$
 Cloud-friendliness                    |      $\times$            |         $\checkmark$        |     $\checkmark$       |      $\checkmark$
 Collaborativeness                     |      $\times$            |         $\checkmark$        |     $\checkmark$       |      $\sim$
 Licensing/openness                    | $\checkmark$/$\sim$/$\times$ |         $\times$        |     $\checkmark$       |      $\checkmark$
 Non-nerd friendliness                 |      $\checkmark$            |         $\checkmark$        |     $\sim$         |      $\times$
:::



[^2]: Here “FeenoX” means ([FeenoX](https://seamplex.com/feenox) + [Gmsh](http://gmsh.info) + [Paraview](https://www.paraview.org/) + [Git](https://git-scm.com/) + [Github](https://github.com/) / [Gitlab](https://about.gitlab.com/) / [Gitea](https://gitea.com/}{Gitea}))


Therefore, FeenoX is---in a certain sense---to desktop FEA programs like

 * [Code_Aster](https://www.code-aster.org/spip.php?rubrique2) with [Salome-Meca](https://www.code-aster.org/V2/spip.php?article303), or
 * [CalculiX](http://www.calculix.de/) with [PrePoMax](https://prepomax.fs.um.si/))
 
and to libraries like

 * [MoFEM](http://mofem.eng.gla.ac.uk/mofem/html/) or
 * [Sparselizard](http://sparselizard.org/)

what [Markdown](https://commonmark.org/) is to Word and [(La)TeX](https://en.wikipedia.org/wiki/LaTeX), respectively and _deliberately_.
