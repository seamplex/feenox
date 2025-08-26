\appendix
 
# Appendix: Downloading and compiling FeenoX

## Debian & Ubuntu packages

To be explained

## Binary executables

```include
binary.md
```

## Source tarballs

```include
source.md
```

## Git repository

```include
git.md
```



# Appendix: Rules of Unix philosophy {#sec:unix}

```{.include shift-heading-level-by=1}
unix.md
```

# Appendix: FeenoX history {#sec:history}

```{.include shift-heading-level-by=1}
history.md
```



# Appendix: Downloading & compiling {#sec:download}

```{.include shift-heading-level-by=1}
../download.md
```

```{.include shift-heading-level-by=1}
compilation.md
```
# Appendix: Inputs for solving LE10 with other FEA programs {#sec:le10-other}

This appendix illustrates the differences in the input file formats used by FeenoX and the ones used by other open source finite-element solvers. The problem being solved is the [NAFEMS\ LE10 benchmark](https://www.seamplex.com/feenox/examples/#nafems-le10-thick-plate-pressure-benchmark), first discussed in @sec:scope:

```{.feenox include="nafems-le10.fee"}
```

See the following URL and its links for further details about solving this problem with the other codes: <https://cofea.readthedocs.io/en/latest/benchmarks/004-eliptic-membrane/tested-codes.html>

## CalculiX

```{include="le10-calculix.inp"}
```

## Code Aster

```{include="le10-aster.comm"}
```

## Elmer

```{include="le10-elmer.elm"}
```

