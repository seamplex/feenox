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

Unlike other similar FEA tools, FeenoX provides...

 * a ready-to-run executable (which uses Autotools and friends to compile) that reads the problem to be solved at run time (i.e. it is a **program** not a _library*) designed an implemented following the Unix programming philosophy:

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


 * a parser for a [syntactically-sugared](https://seamplex.com/feenox/doc/sds.html#syntactic-sugar-highlighting)) self-explanatory ASCII file (passed as the first non-option argument to the `feenox` executable)  with keywords that completely define the problem without requiring further human actions, allowing a [cloud-first](https://seamplex.com/feenox/doc/sds.html#cloud-first) workflow
 
 * continuous integration (using Github actions), an issue tracker (using Github issues) and a discussion page (using Github discussinos)
 
 * a Git repository with GPL sources (and FDL documentation) where contributions are welcome. In particular, each partial differential equation that FeenoX can solve correspondence to one of the subdirectories of `src/pdes`. The `autogen.sh` step (prior to `./configure` and `make`) detects the directory structure and includes all the subdirectories it finds as available [problem types](https://www.seamplex.com/feenox/doc/feenox-manual.html#problem). They can be queried at runtime with the `--pdes` option:
 
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
 
 * a mechanism to expand command-line arguments as literal strings in the input file so as to allow parametric (and/or optimization) loops. For instance, if an input file `print.fee` looks like
 
   ```fee
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
   
 * can come from `stdin` by passing `-` as the input file path:
 
   ```terminal
   $ for i in $(seq 1 5); do echo "PRINT 2*\${1}" | feenox - $i; done
   2
   4
   6
   8
   10
   $ 
   ``` 
 
 * flexibility to handle many workflows, including [web-based interfaces](https://www.caeplex.com)
 
The input file...

 - has one-to-one correspondence between computer input file and human description of the problem
 - is Git-traceable (the mesh is defined in a separate file which may or may not be tracked)
 - allows the user to enter algebraic expressions whenever a numerical value is needed (everything is an expression)
 - understands definitions (nouns) and instructions (verbs). FeenoX has an actual instruction pointer that loops over the instruction set (there might even be conditional blocks).
 - is simple for simple files (but might get more complicated for mor complex problems). Remember Alan Kay's quote: "simple things should be simple and complex things should be possible."
 
 
Following the Unix rule of silence, the output is 100% user-defined output: if there are not explicit output instructions, FeenoX will not write anything. And probably nothing will be computed (because in general FeenoX will not compute things that are not used).

# How

cloud first
unix

explain aws, docker, MPI


## What

SRS + SDS
