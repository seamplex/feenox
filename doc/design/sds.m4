---
title: FeenoX Software Design Specification
lang: en-US
abstract: This design specification addresses the (hypothetical) Software Requirement Specification for developing a piece of free and open source engineering software with certain specifications, defined in an imaginary tender.
number-sections: true
---

dnl --------------------------------------------------------------------------------
esyscmd(awk -f quote.awk 010-introduction.md)


Besides noting that software being _free_ (regarding freedom, not price) does not imply the same as being _open source_, the requirement is clear in that the tool has to be both _free_ and _open source_, a combination which is usually called FOSS. This condition leaves all of the well-known non-free finite-element solvers in the market out of the tender.

FeenoX is licensed under the terms of the GNU General Public License version\ 3 or, at the user convenience, any later version. This means that users get the four essential freedoms:

 0. The freedom to _run_ the program as they wish, for _any_ purpose.
 1. The freedom to _study_ how the program works, and _change_ it so it does their computing as they wish.
 2. The freedom to _redistribute_ copies so they can help others.
 3. The freedom to _distribute_ copies of their _modified_ versions to others.

There are some examples of pieces of computational software which are described as “open source” in which even the first of the four freedoms is denied. In the nuclear industry, it is the case of a Monte Carlo particle-transport program that requests users to sign an agreement about the objective of its usage before allowing its execution. The software itself might be open source because the source code is provided after signing the agreement, but it is not free at all.
 
Licensing FeenoX as GPLv3+ also implies that the source code and all the scripts and makefiles needed to compile and run it are available for anyone that requires it. Anyone wanting to modify the program either to fix bugs, improve it or add new features is free to do so. And if they do not know how to program, the have the freedom to hire a programmer to do it without needing to ask permission to the original authors. 

Nevertheless, since the original authors are the copyright holders, they still can use it to either enforce or prevent further actions the users that receive FeenoX under the GPLv3+. In particular, the license allows re-distribution of modified versions only if they are clearly marked as different from the original and only under the same terms of the GPLv3+. It is forbidden to include either totally or partially FeenoX in another piece of software whose license is incompatible with the GPLv3+.


dnl --------------------------------------------------------------------------------
esyscmd(awk -f quote.awk 020-objective.md)

FeenoX arranca con un subset de eso más neutrónica.
En algún lugar hay que poner un límite para arrancar. Release plans.

FeenoX está diseñado desde cero para correr en GNU/Linux, que constituye la arquitectura ampliamente mayoritaria de "la nube."
Usa MPI que es un estándar para cosas en paralelo sore muchos hosts.
También se puede ejecutar en una PC o laptop, aunque la performance y escalabilidad están limitadas por el tipo y número de CPUs, la memoria RAM disponible y los métodos de refrigeración.


dnl --------------------------------------------------------------------------------
esyscmd(awk -f quote.awk 030-scope.md)


Transfer function

![The tool working as a transfer function between input and output files](transfer.svg){#fig:transfer width=50%}


UNIX rules as appendix

rule of separation

separate mesher - unix 

dnl --------------------------------------------------------------------------------
esyscmd(awk -f quote.awk 040-architecture.md)

C, quote petsc, flat memory address space -> that's what virtual servers have!

posix

dnl --------------------------------------------------------------------------------
esyscmd(awk -f quote.awk 050-deployment.md)

autoconf vs. cmake, rule of diversity 

gcc, clang

blas, lapack, atlas

binaries freely available at seamplex.com

Linux, mac, windows 

dnl --------------------------------------------------------------------------------
esyscmd(awk -f quote.awk 060-execution.md)

command line args for paramteric runs from a shell script

script friendly

dnl --------------------------------------------------------------------------------
esyscmd(awk -f quote.awk 070-efficienciy.md)

cloud, rent don't buy
benchmark and comparisons


dnl --------------------------------------------------------------------------------
esyscmd(awk -f quote.awk 080-scalability.md)

PETSc, MPI
error handling, rule of repair
check all malloc() calls


dnl --------------------------------------------------------------------------------
esyscmd(awk -f quote.awk 090-flexibility.md)

FeenoX comes from nuclear + experience (what to do and what not to do)


dnl --------------------------------------------------------------------------------
esyscmd(awk -f quote.awk 100-extensibility.md)

user-provided routines
skel for pdes and annotated models


dnl --------------------------------------------------------------------------------
esyscmd(awk -f quote.awk 110-interoperability.md)

UNIX
POSIX


dnl --------------------------------------------------------------------------------
esyscmd(awk -f quote.awk 120-interfaces.md)

dnl --------------------------------------------------------------------------------
esyscmd(awk -f quote.awk 130-input.md)

dar ejemplos
comparar con <https://cofea.readthedocs.io/en/latest/benchmarks/004-eliptic-membrane/tested-codes.html>

macro-friendly inputs, rule of generation

dnl --------------------------------------------------------------------------------
esyscmd(awk -f quote.awk 140-output.md)

100% user-defined output with PRINT, rule of silence
rule of economy, i.e. no RELAP
yaml/json friendly outputs
vtk (vtu), gmsh


dnl --------------------------------------------------------------------------------
esyscmd(awk -f quote.awk 150-qa.md)



dnl --------------------------------------------------------------------------------
esyscmd(awk -f quote.awk 160-reproducibility.md)



dnl --------------------------------------------------------------------------------
esyscmd(awk -f quote.awk 170-testing.md)


dnl --------------------------------------------------------------------------------
esyscmd(awk -f quote.awk 180-bugs.md)


dnl --------------------------------------------------------------------------------
esyscmd(awk -f quote.awk 190-verification.md)


open source (really, not like CCX -> mostrar ejemplo)
GPLv3+ free
Git + gitlab, github, bitbucket


dnl --------------------------------------------------------------------------------
esyscmd(awk -f quote.awk 200-validation.md)


dnl --------------------------------------------------------------------------------
esyscmd(awk -f quote.awk 210-documentation.md)

it's not compact, but almost!
Compactness is the property that a design can fit inside a human being's head. A good practical test for compactness is this: Does an experienced user normally need a manual? If not, then the design (or at least the subset of it that covers normal use) is compact.
unix man page
markdown + pandoc = html, pdf, texinfo
 
