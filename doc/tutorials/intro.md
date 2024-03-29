[FeenoX](https://www.seamplex.com/feenox) is a cloud-first engineering tool. This idea is thoroughly discussed in the [documentation](https://www.seamplex.com/feenox/doc), particularly in the

 * [Software Requirements Specification](https://www.seamplex.com/feenox/doc/srs.html), and
 * [Software Design Specification](https://www.seamplex.com/feenox/doc/sds.html)
 
As such, it is a tool that in principle should be used from a higher-level interface (e.g. a web-based UI such as <www.caeplex.com>) or called from a set of automated scripts following some kind of parametric or optimization workflow.


In any case, if you want to learn how to create your web-based interface or how to implement an optimization workflow on your own, you will need to start running simple cases manually and then increasing the complexity until reaching the solve state-of-the-art capabilities.

Recall by reading again the [project's main README](https://github.com/seamplex/feenox), that FeenoX is---in a certain sense---to desktop FEA programs (like [Code_Aster](https://www.code-aster.org/spip.php?rubrique2) with [Salome-Meca](https://www.code-aster.org/V2/spip.php?article303) or [CalculiX](http://www.calculix.de/) with [PrePoMax](https://prepomax.fs.um.si/)) and libraries (like [MoFEM](http://mofem.eng.gla.ac.uk/mofem/html/) or [Sparselizard](http://sparselizard.org/)) what [Markdown](https://commonmark.org/) is to Word and [(La)TeX](https://en.wikipedia.org/wiki/LaTeX), respectively and _deliberately_.


Indeed, it is this the main goal of the tutorials, namely to be able to run relatively small problems in FeenoX by creating the appropriate input files and, eventually meshes or other needed data files. This way, by the end of each tutorial you will better understsand how FeenoX works and thus, how to make it work the way you need and/or want. And if not, you always have the _freedom_ to hire someone to explain it to you and even to modify the code to make it work the way you need and/or want since it is [free and open source software](https://www.seamplex.com/feenox/#licensing).


