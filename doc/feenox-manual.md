---
title: FeenoX manual
subtitle: A cloud-first free no-fee no-X uniX-like finite-element(ish) computational engineering tool
author: Jeremy Theler
infoname: feenox
lang: en-US
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

> See <https://www.seamplex.com/feenox/examples> for updated information.


```{.include shift-heading-level-by=1}
../examples/examples.md
```

# Tutorials

> See <https://www.seamplex.com/feenox/doc/tutorials> for updated information.

```{.include shift-heading-level-by=1}
tutorials/setup.md
```
 
```{.include shift-heading-level-by=1}
tutorials/general.md
```

```{.include shift-heading-level-by=1}
tutorials/detailed.md
```

```{.include shift-heading-level-by=1}
tutorials/physics.md
```

# Writing FeenoX input files

```{.include shift-heading-level-by=1}
input-syntax.md
```

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


## Partial Differential Equations subsystem

### PDE keywords

```{.include shift-heading-level-by=3}
reference-pde-kw.md
```

### PDE variables

```{.include shift-heading-level-by=3}
reference-pde-va.md
```


## Laplace’s equation

Set `PROBLEM` to `laplace` to solve Laplace’s equation (or Poisson’s if the source\ $f$ is given)

$$-\nabla^2 \phi = f(\vec{x})$$

If `end_time` is set, then the transient problem is solved

$$
\alpha(\vec{x}) \frac{\partial \phi}{\partial t} - \nabla^2 \phi = f(\vec{x})
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



## The heat conduction equation {#thermal}

Set [`PROBLEM`](#problem) to `thermal` to solve the heat conduction equation:

$$
\rho(\vec{x},T) c_p(\vec{x},T) \cdot \frac{\partial T}{\partial t} - \text{div} \left[ k(\vec{x}, T) \cdot \text{grad}{T} \right] = q'''(\vec{x}, T)
$$

 * If [`end_time`](#end_time) is zero, only the steady-state problem is solved.
 * If [`end_time`](#end_time) $>$ zero, then a transient problem is solved.
 * If either
 
    a. [`k`](#k), and/or
    b. [`q'''](#q-1)$, and/or
    c. any Neumann boundary condition
    
   depends on [`T`](#t), the problem is set to non-linear automatically.

> Check out the [heat conduction tutorial](https://www.seamplex.com/feenox/doc/tutorials/320-thermal/) as well.


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

## Elasticity {#mechanical}

Set [`PROBLEM`](#problem) to `mechanical` to solve the elasticity problem.

> Check out the [tensile test tutorial](https://www.seamplex.com/feenox/doc/tutorials/110-tensile-test/) as well.


### Elasticity results

```{.include shift-heading-level-by=3}
reference-mechanical-re.md
```

### Elasticity properties

```{.include shift-heading-level-by=3}
reference-mechanical-pr.md
```


### Elasticity boundary conditions

```{.include shift-heading-level-by=3}
reference-mechanical-bc.md
```

### Elasticity keywords

```{.include shift-heading-level-by=3}
reference-mechanical-kw.md
```

### Elasticity variables

```{.include shift-heading-level-by=3}
reference-mechanical-va.md
```



## Modal analysis {#modal}

Set [`PROBLEM`](#problem) to `modal` to compute the natural frequencies and oscillation modes of a solid,
i.e. the generalized eigenvalue problem $K \vec{\phi} = \omega^2 M \vec{\phi}$.
The number of modes is given by `MODES` in [`PROBLEM`](#problem).

### Modal boundary conditions

```{.include shift-heading-level-by=3}
reference-modal-bc.md
```

### Modal variables

```{.include shift-heading-level-by=3}
reference-modal-va.md
```


## Neutron diffusion {#neutron_diffusion}

Set [`PROBLEM`](#problem) to `neutron_diffusion` to solve the multi-group core-level neutron diffusion equation
with a finite-element formulation.

### Neutron diffusion boundary conditions

```{.include shift-heading-level-by=3}
reference-neutron_diffusion-bc.md
```

### Neutron diffusion results

```{.include shift-heading-level-by=3}
reference-neutron_diffusion-re.md
```

### Neutron diffusion variables

```{.include shift-heading-level-by=3}
reference-neutron_diffusion-va.md
```



## Neutron transport with discrete ordinates

### Neutron transport results

```{.include shift-heading-level-by=3}
reference-neutron_sn-re.md
```

### Neutron transport properties

```{.include shift-heading-level-by=3}
reference-neutron_sn-pr.md
```


### Neutron transport boundary conditions

```{.include shift-heading-level-by=3}
reference-neutron_sn-bc.md
```

### Neutron transport keywords

```{.include shift-heading-level-by=3}
reference-neutron_sn-kw.md
```

### Neutron transport variables

```{.include shift-heading-level-by=3}
reference-neutron_sn-va.md
```



## Thermal hydraulics {#thermal_hydraulics}

Set [`PROBLEM`](#problem) to `thermal_hydraulics` to solve one-dimensional single-phase liquid-water flow
in networks of pipes, optionally coupled to fuel-rod and wall heat structures.
The problem does not need any extra keyword in [`PROBLEM`](#problem).

 * Each mesh node is a control volume with pressure\ $p$ and temperature\ $T$ as primary unknowns,
   and each line element is a flow path with mass flow\ $\dot{m}$ (staggered finite volumes, as in RELAP).
 * The fluid is single-phase liquid water only: IAPWS-IF97 region\ 1 between 273.15\ K and 623.15\ K,
   with IAPWS-2008 viscosity and IAPWS-2011 thermal conductivity.
   A warning is issued if a temperature leaves this range or if the pressure falls below saturation.
   Boiling and two-phase flow are not modeled.
 * Only first-order line elements are supported, and the problem can only be solved in serial.
 * Pipes are always topologically one-dimensional but they live in a space of one, two or three dimensions.
   The default is three, so spatial functions such as initial conditions and the solution take\ $(x,y,z)$ as
   arguments for any network, including straight pipes along the\ $x$ axis.
   Use `DIMENSIONS 1` for functions of\ $x$ only. The nodes may span fewer coordinates than the given dimension, but not more.
 * Problem-specific keywords such as [`JUNCTION`](#junction), [`PUMP`](#pump) and [`HEAT_STRUCTURE`](#heat_structure)
   have to come after `PROBLEM`.
 * If [`end_time`](#end_time) is zero, only the steady-state problem is solved. Otherwise, a transient is solved
   using PETSc's TS with backward Euler by default (which damps the acoustic waves) and iteration-based adaptive time steps:
   the step is multiplied by [`dt_factor`](#dt_factor) after a step whose Newton solve took at most
   [`dt_grow_iterations`](#dt_grow_iterations) iterations, divided by it after one that took
   [`dt_shrink_iterations`](#dt_shrink_iterations) or more, and kept within `min_dt` and `max_dt`.
   A step whose nonlinear solve fails is retried with the step divided by `dt_factor`.
   Assigning `dt_0` sets the first step only, while assigning `dt` (e.g. `dt = 0.1`) dictates a fixed step.
   Other schemes can be chosen with `TRANSIENT_SOLVER` (e.g. `bdf`) and local-error control with `TIME_ADAPTATION basic`.
   Each control volume carries its mass and internal energy as unknowns, so any linear multistep scheme conserves them discretely.

> Check out the [thermal-hydraulics examples](https://seamplex.com/feenox/examples/thermal_hydraulics.html) as well.

### Thermal-hydraulics results

```{.include shift-heading-level-by=3}
reference-thermal_hydraulics-re.md
```

### Thermal-hydraulics properties

```{.include shift-heading-level-by=3}
reference-thermal_hydraulics-pr.md
```


### Thermal-hydraulics boundary conditions

```{.include shift-heading-level-by=3}
reference-thermal_hydraulics-bc.md
```

### Thermal-hydraulics keywords

```{.include shift-heading-level-by=3}
reference-thermal_hydraulics-kw.md
```

### Thermal-hydraulics variables

```{.include shift-heading-level-by=3}
reference-thermal_hydraulics-va.md
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
reference-fv.md
```


\appendix

# FeenoX & the Unix Philospohy

```{.include shift-heading-level-by=1}
unix.md
```


# History

```{.include shift-heading-level-by=1}
history.md
```

