~~~terminal
feenox [options] inputfile [replacement arguments] [petsc options]  
~~~

`-h`, `--help`

:    display options and detailed explanations of commmand-line usage


`-v`, `--version`

:    display brief version information and exit


`-V`, `--versions`

:    display detailed version information


`--pdes`

:    list the types of `PROBLEM`s that FeenoX can solve, one per line


`--elements_info`

:    output a document with information about the supported element types


`--progress`

:    print ASCII progress bars when solving PDEs


`--mumps`

:    ask PETSc to use the direct linear solver MUMPS


`--linear`

:    force FeenoX to solve the PDE problem as linear


`--non-linear`

:    force FeenoX to solve the PDE problem as non-linear


Instructions will be read from standard input if "-" is passed as `inputfile`, i.e.

```terminal
$ echo 'PRINT 2+2' | feenox -
4
```

The optional `[replacement arguments]` part of the command line mean that each
argument after the input file that does not start with an hyphen will be expanded
verbatim in the input file in each occurrence of `$1`, `$2`, etc. For example

```terminal
$ echo 'PRINT $1+$2' | feenox - 3 4
7
```

PETSc and SLEPc options can be passed in `[petsc options]` as well, with the
difference that two hyphens have to be used instead of only once. For example,
to pass the PETSc option `-ksp_view` the actual FeenoX invocation should be

```terminal
$ feenox input.fee --ksp_view
```

For PETSc options that take values, en equal sign has to be used:

```terminal
$ feenox input.fee --mg_levels_pc_type=sor
```

See <https://www.seamplex.com/feenox/examples> for annotated examples.

