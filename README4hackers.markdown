# FeenoX for Hackers

- [<span class="toc-section-number">1</span> Why][]
- [<span class="toc-section-number">2</span> How][]
- [<span class="toc-section-number">3</span> What][]

  [<span class="toc-section-number">1</span> Why]: #why
  [<span class="toc-section-number">2</span> How]: #how
  [<span class="toc-section-number">3</span> What]: #what

# Why

Why is [FeenoX][] different from other “similar” tools?

To better illustrate FeenoX’s unfair advantage (in the entrepreneurial
sense), let us first consider what the options are when we need to write
a technical report, paper or document:

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
| Flexibility           |    ❌/😐     |     ❌/😐     |     ✅     |    ✅     |
| Scalability           |      ❌      |      😐       |     ✅     |    ✅     |
| Traceability          |      ❌      |      😐       |     ✅     |    ✅     |
| Cloud-friendliness    |      ❌      |      ✅       |     ✅     |    ✅     |
| Collaborativeness     |      ❌      |      ✅       |     ✅     |    😐     |
| Licensing/openness    |   ✅/😐/❌   |      ❌       |     ✅     |    ✅     |
| Non-nerd friendliness |      ✅      |      ✅       |     😐     |    ❌     |

</div>

Therefore, FeenoX is—in a certain sense—to desktop FEA programs like

- [Code_Aster][] with [Salome-Meca][], or
- [CalculiX][] with [PrePoMax][])

and to libraries like

- [MoFEM][] or
- [Sparselizard][]

what [Markdown][1] is to Word and [(La)TeX][], respectively and
*deliberately*.

Unlike these other FEA tools, FeenoX provides…

- a ready-to-run executable (which uses Autotools and friends to
  compile) that reads the problem to be solved at run time (i.e. it is a
  **program** not a *library*) designed an implemented following the
  Unix programming philosophy:

  ``` terminal
  $ feenox
  FeenoX v0.3.317-g893dcd9
  a cloud-first free no-fee no-X uniX-like finite-element(ish) computational engineering tool

  usage: feenox [options] inputfile [replacement arguments] [petsc options]

    -h, --help         display options and detailed explanations of commmand-line usage
    -v, --version      display brief version information and exit
    -V, --versions     display detailed version information
    --pdes             list the types of PROBLEMs that FeenoX can solve, one per line
    --elements_info    output a document with information about the supported element types
    --linear           force FeenoX to solve the PDE problem as linear
    --non-linear       force FeenoX to solve the PDE problem as non-linear

  Run with --help for further explanations.
  $ 
  ```

- a parser for a [syntactically-sugared][] self-explanatory ASCII file
  (passed as the first non-optional argument to the `feenox` executable)
  with keywords that completely define the problem without requiring
  further human actions, allowing a [cloud-first][] workflow using
  containerized images (because there is no need to recompile the binary
  for each problem).

- a Git repository with GPL sources (and FDL documentation) where
  contributions are welcome. In particular, each partial differential
  equation that FeenoX can solve correspondence to one of the
  subdirectories of `src/pdes`. The `autogen.sh` step (prior to
  `./configure` and `make`) detects the directory structure and includes
  all the subdirectories it finds as available [problem types][]. They
  can be queried at runtime with the `--pdes` option:

  ``` terminal
  $ feenox --pdes
  laplace
  mechanical
  modal
  neutron_diffusion
  neutron_sn
  thermal
  $ 
  ```

- continuous integration (using Github actions), an issue tracker (using
  Github issues) and a discussion page (using Github discussions)

- a mechanism to expand command-line arguments as literal strings in the
  input file so as to allow parametric (and/or optimization) loops. For
  instance, if an input file `print.fee` looks like

  ``` feenox
  PRINT 2*${1}
  ```

  then

  ``` terminal
  $ for i in $(seq 1 5); do feenox print.fee $i; done
  2
  4
  6
  8
  10
  $ 
  ```

- the possibility to provide the input from `stdin` (so as to use it as
  a Unix pipe) by passing `-` as the input file path:

  ``` terminal
  $ for i in $(seq 1 5); do echo "PRINT 2*\${1}" | feenox - $i; done
  2
  4
  6
  8
  10
  $ 
  ```

- flexibility to handle many workflows, including [web-based
  interfaces][] and thin command-line clients.

The input file…

- has one-to-one correspondence with the human description of the
  problem
- is Git-traceable (the mesh is defined in a separate file created by
  Gmsh, which may or may not be tracked)
- allows the user to enter algebraic expressions whenever a numerical
  value is needed (everything is an expression)
- understands definitions (nouns) and instructions (verbs). FeenoX has
  an actual instruction pointer that loops over the instruction set
  (there might even be conditional blocks).
- is simple for simple files (but might get more complicated for mor
  complex problems). Remember Alan Kay’s quote: “simple things should be
  simple and complex things should be possible.”

Following the Unix rule of silence, the output is 100% user-defined
output: if there are not explicit output instructions, FeenoX will not
write anything. And probably nothing will be computed (because FeenoX is
smart and will not compute things that are not actually needed).

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
  [syntactically-sugared]: https://seamplex.com/feenox/doc/sds.html#syntactic-sugar-highlighting
  [cloud-first]: https://seamplex.com/feenox/doc/sds.html#cloud-first
  [problem types]: https://www.seamplex.com/feenox/doc/feenox-manual.html#problem
  [web-based interfaces]: https://www.caeplex.com
  [Pandoc]: https://pandoc.org/
  [Git]: https://git-scm.com/
  [Github]: https://github.com/
  [Gitlab]: https://about.gitlab.com/
  [Gitea]: https://gitea.com/%7D%7BGitea%7D
  [2]: https://seamplex.com/feenox
  [Gmsh]: http://gmsh.info
  [Paraview]: https://www.paraview.org/

# How

Feenox is a computational too designed to be run on a Unix server as a
part of a [cloud-first][] workflow, optionally involving MPI
communication among different servers to hande arbitrarily-large
problems. It has been designed and implemented in C following the Unix
programming philosophy.

Following the rule of composition, when solving PDEs FeenoX works very
much as a “glue layer” between a mesher (such as Gmsh) and a
post-processing tool (such as Paraview):

                                 +------------+
     mesh (*.msh)  }             |            |             { terminal
     data (*.dat)  } input ----> |   FeenoX   |----> output { data files
     input (*.fee) }             |            |             { post (vtk/msh)
                                 +------------+

FeenoX consists of a binary executable which is compiled using GNU
Autotools (i.e. `./autogen.sh && ./configure && make`) and uses three
well-established and open source libraries:

1.  The GNU Scientific Library for basic numerical computations
2.  SUNDIALS IDA for solving systems of ODEs/DAEs
3.  PETSc and SLEPc for solving PDEs

The stock packages provided in most GNU/Linux distributions work
perfectly well, but custom configured and compiled versions (e.g. with
particular optimization flags or linked with non-standard MPI
implementations) can be used as well.

An empty Debian-based GNU/Linux server (either `amd64` or `arm`) can be
provisioned with a working FeenoX binary at `/usr/local/bin` ready to
solve arbitrary problems by doing

``` terminal
sudo apt-get install -y libgsl-dev libsundials-dev petsc-dev slepc-dev
git clone https://github.com/seamplex/feenox
cd feenox
./autogen.sh
./configure
make
make install
```

> **Heads up!** If we want to be sure everything went smooth, we should
> take some time to install Gmsh and run the test suite:
>
> ``` terminal
> sudo apt-get install gmsh
> make check
> ```

These steps are flexible enough so as to be integrated into
containerization technologies (e.g. Docker files), continuous
integration schemes (e.g. Github actions) or to suit any other
particular needs (e.g. servers with custom PETSc installations or
clusters multi-node MPI communication schemes). For instance, it is also
possible to generate custom `.deb` (or `.rpm`) packages and make the
server’s `apt` manager to fetch and install them without needing to
compile the source code at all.

Following the Unix rule of diversity, different compilers, both for the
C code part of FeenoX as for the code in the dependencies (and their
dependencies) can be used. So far we tested

- GCC
- Clang
- Intel OneAPI

Also, different MPI implementations have been tested:

- OpenMPI (not to confuse with OpenMP)
- MPICH
- Intel MPI

Feel free to raise any concerns you might have in our discussions page.

  [cloud-first]: https://seamplex.com/feenox/doc/sds.html#cloud-first

# What

- FeenoX follows a fictitious (yet plausible) Software Design
  Requirements document.

- The explanation of how FeenoX addresses the requirements can be found
  in the Software Design Specifications.

- FeenoX’ performance can be profiled and analyzed with the Google
  Benchmark library using this repository

- A rough comparison of FeenoX’s performance (and differences with
  respect to problem set up and execution) with respect to other similar
  tools can be found in this link:

Check out [FeenoX for Engineersr][] and [FeenoX for Academics][] for
complementary information.

  [FeenoX for Engineersr]: README4engineers.md
  [FeenoX for Academics]: README4academics.md
