To better illustrate FeenoX’ unfair advantage (in the entrepreneurial sense), let us first consider what the options are when an engineer needs to to write a technical report, paper or document:

 Feature                | Microsoft Word |  Google Docs  |  Markdown[^1]    |  (La)TeX
:-----------------------|:--------------:|:-------------:|:------------:|:----------------:
 Aesthetics             |       ❌        |       ❌       |      ✅      |  ✅
 Convertibility (to other formats)         |       😐       |       😐      |      ✅       |  😐
 Traceability           |       ❌        |       😐      |      ✅      |  ✅
 Mobile-friendliness    |       ❌        |       ✅       |      ✅      |  ❌
 Collaborativeness      |       ❌        |       ✅       |      ✅      |  😐
 Licensing/openness     |       ❌        |       ❌       |      ✅      |  ✅
 Non-nerd friendliness  |       ✅        |       ✅      |      😐      |  ❌

[^1]: Here “[Markdown](https://en.wikipedia.org/wiki/Markdown)” means ([Pandoc](https://pandoc.org/) + [Git](https://git-scm.com/) + [Github](https://github.com/) / [Gitlab](https://about.gitlab.com/) / [Gitea](https://gitea.com/}{Gitea}))

 
After analyzing the pros and cons of each alternative, at some point it should be evident that [Markdown](https://en.wikipedia.org/wiki/Markdown) (plus friends) gives the best trade off. We can then perform a similar analysis for the options available in order to solve an engineering problem casted as a partial differential equation, say by using a finite-element formulation:

 Feature                | Desktop GUIs  |   Web frontends  |  FeenoX[^2]  |  Libraries
:-----------------------|:-------------:|:----------------:|:---------:|:------------:
 Flexibility            |      😐       |         ❌        |     ✅    |      ✅
 Scalability            |      ❌        |         😐       |     ✅    |      ✅
 Traceability           |      ❌        |         😐       |     ✅    |      ✅
 Cloud-friendliness     |      ❌        |         ✅        |     ✅    |      ✅
 Collaborativeness      |      ❌        |         ✅        |     ✅    |      ❌
 Licensing/openness     |   ✅/😐/❌      |         ❌        |     ✅    |      ✅
 Non-nerd friendliness  |      ✅        |         ✅        |     😐   |      ❌
 
[^2]: Here “FeenoX” means ([FeenoX](https://seamplex.com/feenox) + [Gmsh](http://gmsh.info) + [Paraview](https://www.paraview.org/) + [Git](https://git-scm.com/) + [Github](https://github.com/) / [Gitlab](https://about.gitlab.com/) / [Gitea](https://gitea.com/}{Gitea}))


Therefore, on the one hand, FeenoX is---in a certain sense---to desktop FEA programs (like [Code_Aster](https://www.code-aster.org/spip.php?rubrique2) with [Salome-Meca](https://www.code-aster.org/V2/spip.php?article303) or [CalculiX](http://www.calculix.de/) with [PrePoMax](https://prepomax.fs.um.si/)) and libraries (like [MoFEM](http://mofem.eng.gla.ac.uk/mofem/html/) or [Sparselizard](http://sparselizard.org/)) what [Markdown]() is to Word and [(La)TeX](https://en.wikipedia.org/wiki/LaTeX), respectively and _deliberately_.
