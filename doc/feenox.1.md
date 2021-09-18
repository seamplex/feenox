---
title: FEENOX(1) FeenoX | FeenoX User Manual
author: Jeremy Theler <jeremy@seamplex.com>
---
  
# NAME

FeenoX - a free no-fee no-X uniX-like finite-element(ish) computational engineering tool

# SYNOPSIS

The basic usage is to execute the **feenox** binary passing a path to an input file that defines the problem, along with other options and command-line replacement arguments which are [explained below](#options):

**feenox** [_options_ ...] _input-file_ [_optional_commandline_replacement_arguments_ ...]

For large problems that do not fit in a single computer, a parallel run using **mpirun**`(1)` will be needed:

**mpirun** **-n** _number_of_threads_ **feenox** [_options_ ...] _input-file_ [_optional_commandline_replacement_arguments_ ...]


Besides executing the **feenox** binary, there is an API that allows to use FeenoX from Python or Julia.


# DESCRIPTION

```include
overview.md
```

# OPTIONS

```include
help.md
```


# EXAMPLES


# EXIT STATUS


# ENVIRONMENT


# FILES


# CONFORMING TO


# INPUT-FILE KEYWORDS

```{.include shift-heading-level-by=1}
reference-kw.md
```

# SPECIAL VARIABLES

```{.include shift-heading-level-by=1}
reference-va.md
```

# MATERIAL PROPERTIES

TBD.

# BOUNDARY CONDITIONS

TBD.

# RESULTING DISTRIBUTIONS

TBD.

# BUILT-IN FUNCTIONS

```{.include shift-heading-level-by=1}
reference-fn.md
```

# BUILT-IN FUNCTIONALS

TBD.

# BUILT-IN VECTOR FUNCTIONS

TBD.





# NOTES

TBD.

# BUGS

Report on GitHub <https://github.com/seamplex/feenox> or at <jeremy@seamplex.com>

# SEE ALSO

**gmsh**`(1)`, **mpirun**`(1)`, **paraview**`(1)`

The FeenoX web page contains links to the full source code, binary versions, updates, examples, verification & validation cases and full documentation:
<https://www.seamplex.com/feenox>.
