# FeenoX for Hackers

- [<span class="toc-section-number">1</span> Why][]
- [<span class="toc-section-number">2</span> How][]

  [<span class="toc-section-number">1</span> Why]: #why
  [<span class="toc-section-number">2</span> How]: #how

# Why

Why is [FeenoX][] different from other “similar” tools?

To better illustrate FeenoX’s unfair advantage (in the entrepreneurial
sense), let us first consider what the options are when an engineer
needs to to write a technical report, paper or document:

<div class="not-in-format latex">

| Feature                           | Microsoft Word | Google Docs | Markdown[^1] | (La)TeX |
|:----------------------------------|:--------------:|:-----------:|:------------:|:-------:|
| Aesthetics                        |       ❌       |     ❌      |      ✅      |   ✅    |
| Convertibility (to other formats) |       😐       |     😐      |      ✅      |   😐    |
| Traceability                      |       ❌       |     😐      |      ✅      |   ✅    |
| Mobile-friendliness               |       ❌       |     ✅      |      ✅      |   ❌    |
| Collaborativeness                 |       ❌       |     ✅      |      ✅      |   😐    |
| Licensing/openness                |       ❌       |     ❌      |      ✅      |   ✅    |
| Non-nerd friendliness             |       ✅       |     ✅      |      😐      |   ❌    |

</div>

After analyzing the pros and cons of each alternative, at some point it
should be evident that [Markdown][] (plus friends) gives the best trade
off. We can then perform a similar analysis for the options available in
order to solve an engineering problem casted as a partial differential
equation, say by using a finite-element formulation:

<div class="not-in-format latex">

| Feature               | Desktop GUIs | Web frontends | FeenoX[^2] | Libraries |
|:----------------------|:------------:|:-------------:|:----------:|:---------:|
| Flexibility           |      😐      |      ❌       |     ✅     |    ✅     |
| Scalability           |      ❌      |      😐       |     ✅     |    ✅     |
| Traceability          |      ❌      |      😐       |     ✅     |    ✅     |
| Cloud-friendliness    |      ❌      |      ✅       |     ✅     |    ✅     |
| Collaborativeness     |      ❌      |      ✅       |     ✅     |    ❌     |
| Licensing/openness    |   ✅/😐/❌   |      ❌       |     ✅     |    ✅     |
| Non-nerd friendliness |      ✅      |      ✅       |     😐     |    ❌     |

</div>

Therefore, on the one hand, FeenoX is—in a certain sense—to desktop FEA
programs (like [Code_Aster][] with [Salome-Meca][] or [CalculiX][] with
[PrePoMax][]) and libraries (like [MoFEM][] or [Sparselizard][]) what
[Markdown][1] is to Word and [(La)TeX][], respectively and
*deliberately*.

[^1]: Here “[Markdown][]” means ([Pandoc][] + [Git][] + [Github][] /
    [Gitlab][] / [Gitea][])

[^2]: Here “FeenoX” means ([FeenoX][2] + [Gmsh][] + [Paraview][] +
    [Git][] + [Github][] / [Gitlab][] / [Gitea][])

  [FeenoX]: https://www.seamplex.com/feenox
  [Markdown]: https://en.wikipedia.org/wiki/Markdown
  [Code_Aster]: https://www.code-aster.org/spip.php?rubrique2
  [Salome-Meca]: https://www.code-aster.org/V2/spip.php?article303
  [CalculiX]: http://www.calculix.de/
  [PrePoMax]: https://prepomax.fs.um.si/
  [MoFEM]: http://mofem.eng.gla.ac.uk/mofem/html/
  [Sparselizard]: http://sparselizard.org/
  [1]: https://commonmark.org/
  [(La)TeX]: https://en.wikipedia.org/wiki/LaTeX
  [Pandoc]: https://pandoc.org/
  [Git]: https://git-scm.com/
  [Github]: https://github.com/
  [Gitlab]: https://about.gitlab.com/
  [Gitea]: https://gitea.com/%7D%7BGitea%7D
  [2]: https://seamplex.com/feenox
  [Gmsh]: http://gmsh.info
  [Paraview]: https://www.paraview.org/

# How

cloud first unix

explain aws, docker, MPI

\##What

SRS + SDS
