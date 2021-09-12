---
title: FeenoX manual
subtitle: A free no-fee no-X uniX-like finite-element(ish) computational engineering tool
desc: a completely free-as-in-freedom finite-element thermo-mechancial solver desinged and implemented following the UNIX principles
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

FeenoX solves a problem defined in an plain-text input file and writes user-defined outputs to the standard output and/or files, either also plain-text or with a particular format for further post-processing. The syntax of this input file is designed to be as self-describing as possible, using English keywords that explains FeenoX what problem it has to solve in a way is understandable by both humans and computers. Keywords can work either as

 1. Definitions, for instance ”define function\ $f(x)$ and read its data from file `f.dat`”), or as
 2. Instructions, such as “write the stress at point $D$ into the standard output”. 

A person can tell if a keyword is a definition or an instruction because the former are nouns (`FUNCTION`) and the latter verbs (`PRINT`). The equal sign `=` is a special keyword that is neither a verb nor a noun, and its meaning changes depending on what is on the left hand side of the assignment.

 a. If there is a function, then it is a definition: define an algebraic function to be equal to the expression on the right-hand side, e.g.:
 
    ```{.feenox include="examples/fxy.fee"}
    ```
 
 b. If there is a variable, vector or matrix, it is an instruction: evaluate the expression on the right-hand side and assign it to the varible or vector (or matrix) element indicated in the left-hand side. Strictly speaking, if the variable has not already been defined (and implicit declaration is allowed), then the variable is also defined as well, e.g:
 
    ```{.feenox include="examples/equal.fee"}
    ```
    
    There is no need to explicitly define the scalar variable `a` with `VAR` since the first assigment also defines it implicitly (if this is allowed by the keyword `IMPLICIT`).


An input file can define its own variables as needed, such as `my_var` or `flag`. But there are some reserved names that are special in the sense that they either

 1. can be set to modify the behavior of FeenoX, such as `max_dt` or `dae_tol`
 2. can be read to get the internal status or results back from FeenoX, such as `nodes` or `keff`
 3. can be either set or read, such as `dt` or `done`

The problem being solved can be static or transient, depending on whether the special variable `end_time` is zero (default) or not. If it is zero and `static_steps` is equal to one (default), the instructions in the input file are executed once and then FeenoX quits. For example

```{.feenox include="examples/static_onestep.fee"}
```

If `static_steps` is larger than one, the special variable `step_static` is increased and they are repeated the number of time indicated by `static_steps`:

```{.feenox include="examples/static_manysteps.fee"}
```

If the special variable `end_time` is set to a non-zero value, after computing the static part a transient problem is solved. There are three kinds of transient problems:

 1. Plain “standalone” transients
 2. Differential-Algebraic equations (DAE) transients
 3. Partial Differential equations (PDE) transients

In the first case, after all the instruction in the input file were executed, the special variable `t` is increased by the value of `dt` and then the instructions are executed all over again, until `t` reaches `end_time`:

```{.feenox include="examples/transient_standalone.fee"}
```

In the second case, the keyword `PHASE_SPACE` sets up DAE system. Then, one initial condition and one differential-algebraic equation has to be given for each element in the phase space. The instructions before the  DAE block executed, then the DAE timestep is advanced and finally the instructions after DAE block are executed (there cannot be any instruction between the first and the last DAE):

```{.feenox include="examples/transient_dae.fee"}
```

The timestep is chosen by the SUNDIALS library in order to keep an estimate of the residual error below `dae_tol` (default is $10^{-6}$), although `min_dt` and `max_dt` can be used to control it. See the section of the [Differential-Algebraic Equations subsystem] for more information.


In the third cae, the type of PDE being solved is given by the keyword `PROBLEM`. Some types of PDEs do support transient problems (such as `thermal`) but some others do not (such as `modal`). See the detailed explanation of each problem type for details. Now the transient problem is handled by the TS framework of the PETSc library.
In general transient PDEs involve a mesh, material properties, inital conditions, transient boundary conditions, etc. And they create a lot of data since results mean spatial and temporal distributions of one or more scalar fields:

```{.feenox include="examples/transient_pde.fee"}
```

PETSc’s TS also honors the `min_dt` and `max_dt` variables, but the time step is controled by the allowed relative error with the special variable `ts_rtol`. Again, see the section of the [Partial Differential Equations subsytem] for more information. 


## Algebraic expressions

To be done.

 * Everything is an expression.

## Initial conditions

## Expansions of command line arguments

# Reference

This chapter contains a detailed reference of keywords, variables, functions and functionals available in FeenoX. These are used essentially to define the problem that FeenoX needs to solve and to define what the output should be. It should be noted that this chapter is to be used, indeed, as a _reference_ and not as a tutorial. 

## Differential-Algebraic Equations subsystem


### Keywords

```{.include shift-heading-level-by=3}
reference-dae-kw.md
```

### Variables

```{.include shift-heading-level-by=3}
reference-dae-va.md
```


## Partial Differential Equations subsytem

### Keywords

```{.include shift-heading-level-by=3}
reference-pde-kw.md
```

### Variables

```{.include shift-heading-level-by=3}
reference-pde-va.md
```


## Laplace’s equation

Set `PROBLEM` to `laplace` to solve Laplace’s equation

$$\nabla^2 \phi = 0$$

If `end_time` is set, then the transient problem is solved

$$\alpha\vec{x, \phi} \frac{\partial \phi}{\partial t} + \nabla^2 \phi = 0$$



### Results

```{.include shift-heading-level-by=3}
reference-laplace-re.md
```

### Properties

```{.include shift-heading-level-by=3}
reference-laplace-pr.md
```


### Boundary Conditions

```{.include shift-heading-level-by=3}
reference-laplace-bc.md
```

### Keywords

```{.include shift-heading-level-by=3}
reference-laplace-kw.md
```

### Variables

```{.include shift-heading-level-by=3}
reference-laplace-va.md
```



## The heat conduction equation

Set `PROBLEM` to `thermal` (or `heat`) to solve thermal conduction:

$$\rho \c_p \frac{\partial T}{\partial t} + \text{div} \left[ k(\vec{x, T} \cdot \text{grad}{T} \right] = q'''(\vec{x}, T)$$

If `end_time` is zero, only the steady-state problem is solved.
If $k$, $q'''$ or any Neumann boundary condition depends on\ $T$, the problem is set to non-linear automatically.

### Results

```{.include shift-heading-level-by=3}
reference-thermal-re.md
```

### Properties

```{.include shift-heading-level-by=3}
reference-thermal-pr.md
```


### Boundary Conditions

```{.include shift-heading-level-by=3}
reference-thermal-bc.md
```

### Keywords

```{.include shift-heading-level-by=3}
reference-thermal-kw.md
```

### Variables

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

