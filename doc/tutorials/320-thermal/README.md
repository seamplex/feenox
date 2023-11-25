---
title: Heat conduction
subtitle: FeenoX Tutorial \#3
titleblock: |
 FeenoX Tutorial \#3: Heat conduction
 ====================================
lang: en-US
number-sections: true
toc: true
prev_link: ../120-mazes
prev_title: \#2 Solving mazes
...

# Summary

Welcome to **FeenoX’s tutorial number three**.
Here you will learn how to solve the heat conduction equation with FeenoX in all of its flavors: linear and non-linear, static and transient.


 * We start solving steady-state problems. As long as neither of the
 
    a. material properties, nor
    b. sources
    
   depend on the temperature $T(\vec{x})$ and 
   
    c. the boundary conditions do not depend or depend linearly on $T(\vec{x})$
    
   then problem is linear.
   If these three guys depend on space $\vec{x}$ (but not on $T(\vec{x})$), the problem is still linear no matter how complex it looks like:

   ```{.feenox include="manufactured.fee"}
   ```

 * If any of a. b. or c. do depend on the temperature $T(\vec{x})$, then the problem is non-linear.
   FeenoX is able to detect this and will switch to a non-linear solver automatically.
   You can check this by passing `--snes_view` in the command line. Here, SNES means "Scalable Non-linear Equation Solvers" in PETSc's jargon. The `--snes_view` option will show some details about the solver. In linear problems, SNES is not used but the KSP (Krylov SubSpace solvers) framework is used instead. Therefore, `--snes_view` is empty for linear problems. 

 * We show how to trigger non-linearities by
 
    - adding a boundary conditions that depends on $T(\vec{x})$, such as radiation
    - having a conductivity that depends on temperature, which is the case for most materials anyway
    - using heat sources that are temperature-dependent, where increasing $T$ decreases the source
   
 * Finally we show how to solve transient problems, either
 
    i. starting from an arbitrary initial temperature distribution using constant boundary conditions
    ii. starting from a steady-state solution and changing the boundary conditions over time
    iii. both
 
# Linear steady-state problems

In this section we are going to ask FeenoX to compute a temperature distribution $T(\vec{x})$ that satisfies 

$$
- \text{div} \Big[ k(\vec{x}) \cdot \text{grad} \left[ T(\vec{x}) \right] \Big] = q(\vec{x})
$$

along with proper boundary conditions.


## Temperature conditions

### Single-material slab

### Two-material slab

## Heat flux conditions

These problems need at least one fixed-temperature (a.k.a. Dirichlet) condition.

## Convection conditions

## Volumetric heat sources

## Space-dependent properties: manufactured solution


# Non-linear state-state problems

The initial guess is given with `T_guess(x,y,z)` (or `T_guess(x,y)` or `T_guess(x)`).

## Temperature-dependent heat flux: radiation

Mind the units!
Kelvin:

Celsius:

## Temperature-dependent conductivity

UO$_2$

## Temperature-dependent sources

[Le Chatelier's principle](https://en.wikipedia.org/wiki/Le_Chatelier's_principle)


# Transient problems

## From an arbitrary initial condition

UO$_2$

## From a steady state

Pump

