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

 * We trigger non-linearities by
 
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

The simplest heat conduction problem involves a slab geometry with prescribed temperatures at both ends.
If the conductivity $k$ is uniform, then the solution is a linear interpolation of these two temperatures---independently of the actual value of the conductivity.

### Single-material slab

Let us create a unitary slab between $x=0$ and $x=1$ with Gmsh using this `slab.geo`:

```{.c include="slab.geo"}
```

The end at $x=0$ is called `left` and the one at $x=1$ is called `right`.
So we can ask FeenoX to solve a thermal problem with uniform conductivity $k$ and fixed temperatures at both ends by

 1. Defining `PROBLEM` as `thermal` and giving either `1d` or `DIM 1`:
 
    ```feenox
    PROBLEM thermal 1d
    ```
    
    We can check which problems FeenoX can solve by running it with `--pdes`:
    
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
 
 2. Setting the special variable `k` to a constant:
 
    ```feenox
    k = 1
    ```
 
    The fact that the conductivity is givens as a variable means that 
     a. There is a single material
     b. Its conductivity is uniform, i.e. it does not depend on space
     
 3. Giving `T` equal to the desired temperature values after the `BC` definition for both `left` and `right`

    ```feenox
    BC left  T=0
    BC right T=1
    ```

After the instruction `SOLVE_PROBLEM`, the solution $T(x)$ is available as the one-dimensional function `T(x)`.
We can then
 
 i. print its definition values with `PRINT_FUNCTION`.
 ii. evaluate it at any arbitrary location `x`. FeenoX will use the shape functions to interpolate the nodal solutions, and/or
 
Here's a working input file `slab-uniform.fee`:

```{.feenox include="slab-uniform.fee"}
```
 
```terminal
$ feenox slab-uniform.fee 
0       0
1       1
0.1     0.1
0.2     0.2
0.3     0.3
0.4     0.4
0.5     0.5
0.6     0.6
0.7     0.7
0.8     0.8
0.9     0.9
# the temperature at x=2/3 is   0.666667
``` 

> **Homework**
>
> 1. Check that the solution does not depend on $k$.
> 2. Change the values of the boundary conditions and check the result is always a linear interpolation.
 
### Two-material slab

If we have two materials we can give them in two ways:

 1. Using the `MATERIALS` keyword, or
 2. Appending `_materialname` to either a variable or a function of space.


## Heat flux conditions

These problems need at least one fixed-temperature (a.k.a. Dirichlet) condition.

## Convection conditions

## Volumetric heat sources

## Space-dependent properties: manufactured solution


# Non-linear state-state problems

If the material properties or the volumetric heat source depends on the solution $T(\vec{x})$, or the boundary conditions depend non-linearly on $T(\vec{x})$ then the problem is non linear. FeenoX's parser can detect these dependencies so it will use a non-linear solver automatically. That is to say, there is no need for the user to tell the solver which kind of problem it needs to solve---which is reasonable. Why would the user have to tell the solver?


## Temperature-dependent heat flux: radiation

One way of introducing a non-linearity is by having a prescribed heat-flux boundary condition to depend on the temperature in a non-linear way.
A radiation boundary condition is exactly this because the heat flux depends on $T^4(\vec{x})$.
To illustrate this concept, let us consider the one-dimensional slab $x \in [0,1]$ with uniform conductivity equal to 50.
At $x=0$ (`left`) we set a prescribed heat flux equal to 1200 W/m$^2$.
At $x=1$ (`right`) we set a radiation boundary condition with an emissivity $e$ of 0.8 and an absolute reference temperature of 293.15 K. 

This problem, even though it is non-linear, has an analytical solution: a linear interpolation between the temperature at $x=1$ is

$$
T(1) = \left( \frac{1200}{\sigma \cdot e} + T_\text{ref}^4\right)^{\frac{1}{4}}
$$
and the temperature at $x=0$

$$
T(0) = T(1) + \frac{1200}{50}
$$
where $\sigma$ is the Stefan-Boltzmann constant. Just for fun, instead of looking up online its numerical value, we can FeenoX to compute it from the "more fundamental" constants $h$, $c$ and $k_b$.


FeenoX uses PETSc's SNES framework to solve the resulting non-linear equations.
The available solvers---which can be selected either through `PROBLEM SNES` definition or from the command line---are iterative in nature.
The convergence of these algorithms depend on a good initial guess, which by default is a uniform distribution equal to the average of all the temperatures `T` or `Tref` that appear on the temperature and convection boundary conditions.
Since in this case we only have heat fluxes, the initial guess would be zero which might not be appropriate.
We can give an explicit initial guess can be given with the function `T_guess(x)` (or `T_guess(x,y)` or `T_guess(x,y,z)` if the dimensions were two or three).

Putting everything together in a FeenoX input file:


```{.feenox include="radiation-as-heatflux-kelvin.fee"}
```

We can run FeenoX with the PETSc option `--snes_monitor` to check how the residuals converge as the iterative non-linear solver proceeds:

```terminal
$ feenox radiation-as-heatflux-kelvin.fee --snes_monitor
  0 SNES Function norm 1.200000000000e+03 
  1 SNES Function norm 1.013534450309e+03 
  2 SNES Function norm 8.205392002604e+02 
  3 SNES Function norm 1.010983873807e+02 
  4 SNES Function norm 2.318614629013e+00 
  5 SNES Function norm 1.311027509018e-03 
  6 SNES Function norm 3.975272228975e-10 
452.897 452.897
428.897 428.897
$
```

In this case we used SI units with absolute temperatures.
If we wanted to get the temperature in Celsius, we could have done:

```{.feenox include="radiation-as-heatflux-celsius.fee"}
```



> **Homework**
>
> Rewrite the radiation boundary condition as a convection condition. Hint: note that $T^4 - T_\text{ref}^4$ is a difference of squares.
> Look for `radiation-as-convection.fee` in FeenoX's `tests` directory for the answer.

## Temperature-dependent conductivity

UO$_2$

## Temperature-dependent sources

[Le Chatelier's principle](https://en.wikipedia.org/wiki/Le_Chatelier's_principle)


# Transient problems

## From an arbitrary initial condition

UO$_2$

## From a steady state

Pump

