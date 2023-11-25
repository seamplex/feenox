# FeenoX Tutorial \#3: Heat conduction

- [<span class="toc-section-number">1</span> Foreword][]
  - [<span class="toc-section-number">1.1</span> Summary][]
- [<span class="toc-section-number">2</span> Linear steady-state
  problems][]
  - [<span class="toc-section-number">2.1</span> Temperature
    conditions][]
    - [<span class="toc-section-number">2.1.1</span> Single-material
      slab][]
    - [<span class="toc-section-number">2.1.2</span> Two-material
      slab][]
  - [<span class="toc-section-number">2.2</span> Heat flux conditions][]
  - [<span class="toc-section-number">2.3</span> Convection
    conditions][]
  - [<span class="toc-section-number">2.4</span> Volumetric heat
    sources][]
  - [<span class="toc-section-number">2.5</span> Space-dependent
    properties: manufactured solution][]
- [<span class="toc-section-number">3</span> Non-linear state-state
  problems][]
  - [<span class="toc-section-number">3.1</span> Temperature-dependent
    heat flux: radiation][]
  - [<span class="toc-section-number">3.2</span> Temperature-dependent
    conductivity][]
  - [<span class="toc-section-number">3.3</span> Temperature-dependent
    sources][]
- [<span class="toc-section-number">4</span> Transient problems][]
  - [<span class="toc-section-number">4.1</span> From an arbitrary
    initial condition][]
  - [<span class="toc-section-number">4.2</span> From a steady state][]

  [<span class="toc-section-number">1</span> Foreword]: #sec:foreword
  [<span class="toc-section-number">1.1</span> Summary]: #summary
  [<span class="toc-section-number">2</span> Linear steady-state problems]:
    #linear-steady-state-problems
  [<span class="toc-section-number">2.1</span> Temperature conditions]: #temperature-conditions
  [<span class="toc-section-number">2.1.1</span> Single-material slab]: #single-material-slab
  [<span class="toc-section-number">2.1.2</span> Two-material slab]: #two-material-slab
  [<span class="toc-section-number">2.2</span> Heat flux conditions]: #heat-flux-conditions
  [<span class="toc-section-number">2.3</span> Convection conditions]: #convection-conditions
  [<span class="toc-section-number">2.4</span> Volumetric heat sources]:
    #volumetric-heat-sources
  [<span class="toc-section-number">2.5</span> Space-dependent properties: manufactured solution]:
    #space-dependent-properties-manufactured-solution
  [<span class="toc-section-number">3</span> Non-linear state-state problems]:
    #non-linear-state-state-problems
  [<span class="toc-section-number">3.1</span> Temperature-dependent heat flux: radiation]:
    #temperature-dependent-heat-flux-radiation
  [<span class="toc-section-number">3.2</span> Temperature-dependent conductivity]:
    #temperature-dependent-conductivity
  [<span class="toc-section-number">3.3</span> Temperature-dependent sources]:
    #temperature-dependent-sources
  [<span class="toc-section-number">4</span> Transient problems]: #transient-problems
  [<span class="toc-section-number">4.1</span> From an arbitrary initial condition]:
    #from-an-arbitrary-initial-condition
  [<span class="toc-section-number">4.2</span> From a steady state]: #from-a-steady-state

# Foreword

Welcome to **FeenoX’s tutorial number three**. Here you will learn how
to solve the heat conduction equation with FeenoX in all of its flavors:
linear and non-linear, static and transient.

## Summary

- We start solving steady-state problems. As long as neither of the

  1.  material properties, nor
  2.  sources

  depend on the temperature $T(\vec{x})$ and

  3.  the boundary conditions do not depend or depend linearly
      on $T(\vec{x})$

  then problem is linear. If these three guys depend on space $\vec{x}$
  (but not on $T(\vec{x})$, the problem is still linear:

  ``` feenox
  ```

- If any of a. b. or c. do depend on the temperature $T(\vec{x})$, then
  the problem is non-linear. FeenoX is able to detect this and will
  switch to a non-linear solver automatically. You can check this by
  passing `--snes_view` in the command line. Here, SNES means “Scalable
  Non-linear Equation Solvers” in PETSc’s jargon. The `--snes_view`
  option will show some details about the solver. In linear problems,
  SNES is not used but the KSP (Krylov SubSpace solvers) framework is
  used instead. Therefore, `--snes_view` is empty for linear problems.

- We show how to trigger non-linearities by

  - adding a boundary conditions that depends on $T(\vec{x})$, such as
    radiation
  - having a conductivity that depends on temperature, which is the case
    for most materials anyway
  - using heat sources that are temperature-dependent, where
    increasing $T$ decreases the source

- Finally we show how to solve transient problems, either

  1.  starting from an arbitrary initial temperature distribution using
      constant boundary conditions
  2.  starting from a steady-state solution and changing the boundary
      conditions over time
  3.  both

# Linear steady-state problems

In this section we are going to ask FeenoX to compute a temperature
distribution $T(\vec{x})$ that satisfies

$$
- \text{div} \Big[ k(\vec{x}) \cdot \text{grad} T(\vec{x}) \Big] = q(\vec{x})
$$

## Temperature conditions

### Single-material slab

### Two-material slab

## Heat flux conditions

These problems need at least one fixed-temperature (a.k.a. Dirichlet)
condition.

## Convection conditions

## Volumetric heat sources

## Space-dependent properties: manufactured solution

# Non-linear state-state problems

The initial guess is given with `T_guess(x,y,z)` (or `T_guess(x,y)` or
`T_guess(x)`).

## Temperature-dependent heat flux: radiation

Mind the units! Kelvin:

Celsius:

## Temperature-dependent conductivity

UO$_2$

## Temperature-dependent sources

[Le Chatelier’s principle][]

  [Le Chatelier’s principle]: https://en.wikipedia.org/wiki/Le_Chatelier's_principle

# Transient problems

## From an arbitrary initial condition

UO$_2$

## From a steady state

Pump
