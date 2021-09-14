---
title: FeenoX manual
subtitle: A free no-fee no-X uniX-like finite-element(ish) computational engineering tool
author: Jeremy Theler
infoname: feenox
lang: en-US
template: manual.tex
documentclass: book
classoption: oneside
...

# Overview

```{.include}
overview.md
```

# Introduction

```{.include}
introduction.md
```


# Running `feenox`

## Invocation

The format for running the `feenox` program is:

```
feenox [options] inputfile [optional_extra_arguments] ...
```

The `feenox` executable supports the following options:

```{.include}
help.md
```

## Compilation

```{.include shift-heading-level-by=2}
compilation.md
```

# Examples

To be done.

# Tutorial

To be done.

# Description

# Description

```{.include shift-heading-level-by=1}
description.md
```

# Reference

This chapter contains a detailed reference of keywords, variables, functions and functionals available in FeenoX. These are used essentially to define the problem that FeenoX needs to solve and to define what the output should be. It should be noted that this chapter is to be used, indeed, as a _reference_ and not as a tutorial. 

## Differential-Algebraic Equations subsystem


### DAE keywords

```{.include shift-heading-level-by=3}
reference-dae-kw.md
```

### DAE variables

```{.include shift-heading-level-by=3}
reference-dae-va.md
```


## Partial Differential Equations subsytem

### PDE keywords

```{.include shift-heading-level-by=3}
reference-pde-kw.md
```

### PDE variables

```{.include shift-heading-level-by=3}
reference-pde-va.md
```


## Laplace’s equation

Set `PROBLEM` to `laplace` to solve Laplace’s equation

$$\nabla^2 \phi = 0$$

If `end_time` is set, then the transient problem is solved

$$
\alpha(\vec{x}) \frac{\partial \phi}{\partial t} + \nabla^2 \phi = 0
$$



### Laplace results

```{.include shift-heading-level-by=3}
reference-laplace-re.md
```

### Laplace properties

```{.include shift-heading-level-by=3}
reference-laplace-pr.md
```


### Laplace boundary conditions

```{.include shift-heading-level-by=3}
reference-laplace-bc.md
```

### Laplace keywords

```{.include shift-heading-level-by=3}
reference-laplace-kw.md
```

### Laplace variables

```{.include shift-heading-level-by=3}
reference-laplace-va.md
```



## The heat conduction equation

Set `PROBLEM` to `thermal` (or `heat`) to solve thermal conduction:

$$\rho \c_p \frac{\partial T}{\partial t} + \text{div} \left[ k(\vec{x, T} \cdot \text{grad}{T} \right] = q'''(\vec{x}, T)$$

If `end_time` is zero, only the steady-state problem is solved.
If $k$, $q'''$ or any Neumann boundary condition depends on\ $T$, the problem is set to non-linear automatically.

### Thermal results

```{.include shift-heading-level-by=3}
reference-thermal-re.md
```

### Thermal properties

```{.include shift-heading-level-by=3}
reference-thermal-pr.md
```


### Thermal boundary conditions

```{.include shift-heading-level-by=3}
reference-thermal-bc.md
```

### Thermal keywords

```{.include shift-heading-level-by=3}
reference-thermal-kw.md
```

### Thermal variables

```{.include shift-heading-level-by=3}
reference-thermal-va.md
```




## General & “standalone” mathematics

### Keywords

```{.include shift-heading-level-by=3}
reference-kw.md
```

### Variables

```{.include shift-heading-level-by=3}
reference-va.md
```



## Functions

```{.include shift-heading-level-by=2}
reference-fn.md
```

## Functionals

```{.include shift-heading-level-by=2}
reference-fu.md
```

## Vector functions

```{.include shift-heading-level-by=2}
reference-fu.md
```


\appendix

# FeenoX & the UNIX Philospohy

```{.include shift-heading-level-by=1}
unix.md
```


# History

```{.include shift-heading-level-by=1}
history.md
```

