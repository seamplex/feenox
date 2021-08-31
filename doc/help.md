`-h`, `--help`

:    display usage and commmand-line help and exit


`-v`, `--version`

:    display brief version information and exit


`-V`, `--versions`

:    display detailed version information


Instructions will be read from standard input if "-" is passed as inputfile, i.e.

```terminal
$ echo 'PRINT 2+2' | feenox -
4
```

PETSc and SLEPc options can be passed in `[options]` as well, with the difference that
two hyphens have to be used instead of only once. For example, to pass the
PETSc option `-ksp_view` the actual FeenoX invocation should be

```terminal
$ feenox --ksp_view input.fee
```

The optional `[replacement arguments]` part of the command line mean that each
argument after the input file that does not start with an hyphen will be expanded
verbatim in the input file in each occurrence of `$1`, `$2`, etc. For example

```terminal
$ echo 'PRINT $1+$2' | feenox - 3 4
7
```

