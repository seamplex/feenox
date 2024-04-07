---
title: FeenoX for Hackers
titleblock: |
 FeenoX for Hackers
 ====================
lang: en-US
number-sections: true
toc: true
...

# Why

Why is [FeenoX](https://www.seamplex.com/feenox) different from other "similar" tools?

```{.include}
doc/word-md-tex.md
```

Unlike these other FEA tools, FeenoX provides...

 * a [ready-to-run executable](https://www.seamplex.com/feenox/doc/sds.html#sec:execution) (which uses Autotools and friends to compile) that [reads the problem to be solved from an input file](https://www.seamplex.com/feenox/doc/sds.html#sec:input) at run time (i.e. it is a program not a library) designed an implemented following the [Unix programming philosophy](https://www.seamplex.com/feenox/doc/sds.html#sec:unix):

   ```terminal
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


 * a parser for a [syntactically-sugared](https://seamplex.com/feenox/doc/sds.html#sec:syntactic) [self-explanatory ASCII file](https://seamplex.com/feenox/doc/sds.html#sec:input) (passed as the first non-optional argument to the `feenox` executable) with keywords that completely define the problem without requiring further human actions.
 Since the there is no need to recompile the binary for each problem, this allows efficient [cloud-first](https://seamplex.com/feenox/doc/sds.html#cloud-first) workflows using containerized images or even provisioning by downloading  binary tarballs or `.deb` packages.
 
 * a few supported [`PROBLEM`](https://www.seamplex.com/feenox/doc/feenox-manual.html#problem) types and a mechanism to allow hacker and [academics](./README4academics.md) to add new PDEs (as explained in the next bullet). This bullet is about the fact that a [regular user](./README4engineers.md) wanting to solve heat conduction (even with [multi-material non-uniform conductivities](https://www.seamplex.com/feenox/doc/tutorials/320-thermal/)) just needs to do
 
   ```feenox
   PROBLEM thermal
   ```
   
   and does not need to know nor write the weak form of the Poisson equation in the input file, since the vast majority of [users](README4engineers.md) will not know what a weak form is (even though other "similar" tools ask their users for that).
 
 * a [Git repository](https://github.com/seamplex/feenox) with [GPL sources](https://github.com/seamplex/feenox/tree/main/src) (and [FDL documentation](https://github.com/seamplex/feenox/tree/main/doc)) where [contributions are welcome](https://www.seamplex.com/feenox/doc/#contributing).
 In particular, each partial differential equation that FeenoX can solve correspondens to one of the subdirectories of `src/pdes` that provide [C entry points that the main mathematical framework calls as function pointer to build the elemental objects](https://seamplex.com/feenox/doc/sds.html#sec:extensibility). The `autogen.sh` step (prior to `./configure` and `make`) detects the directory structure and includes all the subdirectories it finds as available [problem types](https://www.seamplex.com/feenox/doc/feenox-manual.html#problem). They can be queried at runtime with the `--pdes` option:
 
    ```terminal
    $ feenox --pdes
    laplace
    mechanical
    modal
    neutron_diffusion
    neutron_sn
    thermal
    $ 
    ```
    
   The decision of extensibility through compiled code is, as the choice of making FeenoX a program and not a library, a thoughtful one. See [FeenoX for academics](./README4academics.md) for more details about how the extensibility mechanism works.
    
 * continuous integration (using [Github actions](https://github.com/seamplex/feenox/actions)), an issue tracker (using [Github issues](https://github.com/seamplex/feenox/issues) and a discussion page (using [Github discussions](https://github.com/seamplex/feenox/discussions))
  
 * a mechanism to [expand command-line arguments as literal strings in the input file](https://www.seamplex.com/feenox/doc/sds.html#sec:run-time-arguments) so as to allow [parametric](https://www.seamplex.com/feenox/doc/sds.html#sec:parametric) (and/or [optimization](https://www.seamplex.com/feenox/doc/sds.html#sec:optimization)) loops. For instance, if an input file `print.fee` looks like
 
   ```feenox
   PRINT 2*${1}
   ```
   then
   
   ```terminal
   $ for i in $(seq 1 5); do feenox print.fee $i; done
   2
   4
   6
   8
   10
   $ 
   ``` 
   
 * the possibility to provide the input from `stdin` (so as to use it as a Unix pipe) by passing `-` as the input file path:
 
   ```terminal
   $ for i in $(seq 1 5); do echo "PRINT 2*\${1}" | feenox - $i; done
   2
   4
   6
   8
   10
   $ 
   ``` 
 
 * flexibility to handle many workflows, including [web-based interfaces](https://www.caeplex.com) and [thin command-line clients](https://www.seamplex.com/feenox/doc/sds.html#cloud-first).
 
The [input file](https://seamplex.com/feenox/doc/sds.html#sec:input)...

 - has a [one-to-one correspondence with the human description of the problem](https://seamplex.com/feenox/doc/sds.html#sec:matching-formulations)
 - is Git-traceable ([the mesh is defined in a separate file](https://seamplex.com/feenox/doc/sds.html#sec:input) created by [Gmsh](http://gmsh.info/), which may or may not be tracked)
 - allows the user to enter [algebraic expressions whenever a numerical value is needed](https://seamplex.com/feenox/doc/sds.html#sec:expression) (everything is an expression)
 - understands [definitions (nouns) and instructions (verbs)](https://seamplex.com/feenox/doc/sds.html#sec:nouns_verbs). FeenoX has an actual instruction pointer that loops over the instruction set (there might even be [conditional blocks](https://www.seamplex.com/feenox/doc/feenox-manual.html#if)).
 - is [simple for simple files](https://seamplex.com/feenox/doc/sds.html#sec:simple) (but might get [more complicated for mor complex problems](https://seamplex.com/feenox/doc/sds.html#sec:complex)). Remember [Alan Kay](https://en.wikipedia.org/wiki/Alan_Kay)'s quote: ["simple things should be simple and complex things should be possible."](https://www.quora.com/What-is-the-story-behind-Alan-Kay-s-adage-Simple-things-should-be-simple-complex-things-should-be-possible)
 
 
Following the Unix rule of silence, [the output is 100% user-defined](https://seamplex.com/feenox/doc/sds.html#sec:output): if there are not explicit output instructions, FeenoX will not write anything. And probably nothing will be computed (because FeenoX is smart and will not compute things that are not actually needed).


# How

Feenox is a computational tool designed to be run on Unix servers as a part of a [cloud-first](https://seamplex.com/feenox/doc/sds.html#cloud-first) workflow, optionally [involving MPI communication among different servers](https://seamplex.com/feenox/doc/sds.html#sec:scalability) to hande arbitrarily-large problems:

:::::: {.only-in-format .html }
```{=html}
<div id="cast-le10"></div>
<script>AsciinemaPlayer.create('doc/le10.cast', document.getElementById('cast-le10'), {cols:133,rows:32, poster: 'npt:0:3'});</script>
```
::::::

Check out the section about [invocation](https://www.seamplex.com/feenox/doc/feenox-manual.html#running-feenox) in the [FeenoX manual](https://www.seamplex.com/feenox/doc/feenox-manual.html).

It has been [written in C](https://seamplex.com/feenox/doc/programming.html#languages) and designed under the Unix programming philosophy as quoted by Eric Raymond.
Following the rule of composition, when solving PDEs FeenoX works very much as a Unix pipe between a mesher (such as Gmsh) and a post-processing tool (such as Paraview):

```include
doc/transfer.md
```

FeenoX consists of a binary executable which is compiled using GNU Autotools (i.e. `./autogen.sh && ./configure && make`) and uses three well-established and open source libraries:

 a. The [GNU Scientific Library](https://www.gnu.org/software/gsl/) for basic numerical computations
 b. [SUNDIALS IDA](https://computing.llnl.gov/projects/sundials/ida) for solving systems of ODEs/DAEs
 c. [PETSc](https://petsc.org/) and [SLEPc](http://slepc.upv.es/) for solving PDEs
 
So even more, considering the [NAFEMS LE10 Benchmark problem](https://seamplex.com/feenox/examples/mechanical.html#nafems-le10-thick-plate-pressure-benchmark), it works as two ["glue layers,"](https://www.linuxtopia.org/online_books/programming_books/art_of_unix_programming/ch04s03_1.html)

 1. between the mesher [Gmsh](http://gmsh.info/) and the [PETSc library](https://petsc.org/release/)
 2. between the [PETSc library](https://petsc.org/release/) and the post-processor [Paraview](https://www.paraview.org/)
 
![](doc/transfer-le10-zoom.svg)\  
 

The stock packages provided in most GNU/Linux distributions work perfectly well, but custom configured and compiled versions (e.g. with particular optimization flags or linked with non-standard MPI implementations) can be used as well.
 
An empty Debian-based GNU/Linux server (either `amd64` or `arm`) can be provisioned with a working FeenoX binary at `/usr/local/bin` ready to solve arbitrary problems by doing

```terminal
sudo apt-get install -y libgsl-dev libsundials-dev petsc-dev slepc-dev
git clone https://github.com/seamplex/feenox
cd feenox
./autogen.sh
./configure
make
make install
```

> **Heads up!** If we wanted to be sure everything went smooth, we would need to take some time to install Gmsh and run the test suite:
>
> ```terminal
> sudo apt-get install gmsh
> make check
> ```

These steps are flexible enough so as to be integrated into containerization technologies (e.g. Docker files), continuous integration schemes (e.g. Github actions) or to suit any other particular needs (e.g. servers with custom PETSc installations or clusters multi-node MPI communication schemes).
For instance, it is also possible to generate custom `.deb` (or `.rpm`) packages and make the server's `apt` manager to fetch and install them without needing to compile the source code at all.

Following the Unix rule of diversity, different compilers, both for the C code part of FeenoX as for the code in the dependencies (and their dependencies) can be used. So far there were tested
 
 - GCC (free)
 - Clang (free)
 - Intel OneAPI (privative)

Also, different MPI implementations have been tested:

 - OpenMPI (free, not to confuse with OpenMP)
 - MPICH (free)
 - Intel MPI (privative)
 
Feel free to raise any concerns you might have in our [discussions forum](https://github.com/seamplex/feenox/discussions).


# What

FeenoX is a [cloud-first](https://seamplex.com/feenox/doc/sds.html#cloud-first) [back end](https://en.wikipedia.org/wiki/Frontend_and_backend) for generic computational workflows to solve [engineering-related problems](./README4engineers.md):

```include
doc/examples-list.md
```

## Design

 * FeenoX follows a [fictitious (yet plausible) Software Design Requirements](https://www.seamplex.com/feenox/doc/srs.html).
 * The explanation of how FeenoX addresses the requirements can be found in the [Software Design Specification](https://www.seamplex.com/feenox/doc/sds.html). 

## Performance

 * FeenoX's performance can be profiled and analyzed with the Google Benchmark library using [this repository](https://github.com/seamplex/feenox-benchmark).
 * A rough comparison of FeenoX's performance (and differences with respect to problem set up and execution) with respect to other similar tools can be found in this link: <https://seamplex.com/feenox/tests/nafems/le10/>

Check out [FeenoX for Engineers](README4engineers.md) and [FeenoX for Academics](README4academics.md) for complementary information.

::::: {.only-in-format .html }
```{=html}
<a class="btn btn-lg btn-outline-primary"   href="./README4engineers.md" role="button">FeenoX for Industry Engineers</a>
<a class="btn btn-lg btn-outline-info"      href="./README4academics.md" role="button">FeenoX for Academic Professors</a>
```
:::::
