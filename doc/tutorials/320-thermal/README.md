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

In this section we are going to ask FeenoX to compute a temperature distribution $T(\vec{x})$ that satisfies the linear heat conduction equation

$$
- \text{div} \Big[ k(\vec{x}) \cdot \text{grad} \left[ T(\vec{x}) \right] \Big] = q(\vec{x})
$$ {#eq:heat}

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

If in the heat @eq:heat above the thermal conductivity $k$ or the volumetric heat source $q$ depends on the solution $T(\vec{x})$, or the boundary conditions depend non-linearly on $T(\vec{x})$ then the problem is non linear. FeenoX's parser can detect these dependencies so it will use a non-linear solver automatically. That is to say, there is no need for the user to tell the solver which kind of problem it needs to solve---which is reasonable. Why would the user have to tell the solver?



As we all know, solving a non-linear system of equations is far more complex than solving linear problems.
Even more, the most-widely scheme used to solve the non-linear equation $\vec{F}(\vec{u})=0$,namely the Newton-Raphson method which is the basis of [PETSc's SNES framework](https://petsc.org/release/manual/snes/), involves repeatedly solving a linear system starting from an initial guess $\vec{u}_0$:

 1. Solve $J(\vec{u}_k) \cdot \Delta \vec{u}_k = -\vec{F}(\vec{u}_k)$
 2. Update $\vec{u}_{k+1} \leftarrow \vec{u}_{k} + \Delta \vec{u}_{k}$


The matrix $J = \vec{F}^{\prime}$ associated with these linear solves (which changes from iteration to iteration) is called the jacobian matrix. FeenoX builds an appropriate jacobian for each type of non-linearity, ensuring the convergence is as fast as possible. Advanced users might investigate that indeed $J(\vec{u})$ is correct by using the PETSc options `--snes_test_jacobian` and, for smaller problems, `--snes_test_jacobian_view`. Note that these options render the execution far slower, so make sure the mesh is coarse.


## Temperature-dependent heat flux: radiation

One way of introducing a non-linearity is by having a prescribed heat-flux boundary condition to depend on the temperature in a non-linear way.
A radiation boundary condition is exactly this because the heat flux depends on $T^4(\vec{x})$.
To illustrate this concept, let us consider the one-dimensional slab $x \in [0,1]$ with uniform conductivity equal to 50.

 * At $x=0$ (`left`) we set a prescribed heat flux equal to 1200 W/m$^2$.
 * At $x=1$ (`right`) we set a radiation boundary condition with an emissivity $e$ of 0.8 and an absolute reference temperature of 293.15 K. 

This problem, even though it is non-linear, has an analytical solution: a linear interpolation between the temperature at $x=1$ which is

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
> 1. Rewrite the radiation boundary condition as a convection condition. Hint: note that $T^4 - T_\text{ref}^4$ is a difference of squares.
> Look for `radiation-as-convection.fee` in FeenoX's `tests` directory for the answer.
> 2. Explain why the solver converges even though there are no prescribed temperature conditions. Hint: think it as a convection condition.

## Temperature-dependent conductivity

Another general source of non-linearity in engineering problems modeled as PDEs is due to material properties depending on the unknown.
For heat conduction, this is true when the thermal conductivity depends on the temperature as a certain function $k(T)$.
In general, this dependency is given either using 
 
  a. an algebraic expression with a correlation to experimental data, or
  b. a pointwise-defined "table" with the actual experimental data
 
FeenoX can understand both of them.
In this section we use the former, and in the next section we use the latter.
Consider a pellet of uranium dioxide as the ones used inside the fuel elements of nuclear power reactors.
According to ["Thermophysical Properties of Materials For Nuclear Engineering"](https://www.nuclear-power.com/nuclear-engineering/heat-transfer/thermal-conduction/thermal-conductivity/thermal-conductivity-of-uranium-dioxide/), the thermal conductivity of UO$_2$ can be approximated by

$$
k(\tau) [ \text{W} \cdot \text{m}^{-1} \cdot \text{K}^{-1} ] = \frac{100}{7.5408 + 17.692 \cdot \tau + 3.614 \tau^2} + \frac{6400}{t^{5/2}} \cdot \exp \left( \frac{-16.35}{\tau} \right)
$$
where $\tau = T [ \text{K} ] / 1000$.

How do we tell FeenoX to use this correlation?
Easy: define a special function of space `k(x,y,z)` that uses to this correlation with `T(x,y,z)` as the argument.
If we want `T` in Kelvin:

```feenox
VAR T'
tau(T') = T'/1000 
cond(T') = 100/(7.5408 + 17.692*tau(T') + 3.614*tau(T')^2) + 6400/(tau(T')^(5/2))*exp(-16.35/tau(T'))

# k is in W/(m K)
k(x,y,z) = cond(T(x,y,z))
```

If we want `T` in Celsius:

```feenox
# T is in Celsius, T' is in Kelvin
VAR T'
tau(T') = (T'+273.15)/1000 
cond(T') = 100/(7.5408 + 17.692*tau(T') + 3.614*tau(T')^2) + 6400/(tau(T')^(5/2))*exp(-16.35/tau(T'))

# k is in W/(m K)
k(x,y,z) = cond(T(x,y,z))
```

Two points to take into account:

 1. The symbol `T` is already reserved for the solution field, which is a function of space `T(x,y,z)`, at the time the `PROBLEM` keyword is parsed. Therefore, we cannot use `T` as a variable. If we defined `tau(T)`, we would get
 
    ```terminal
    $ feenox pellet-non-linear-k-uniform-q.fee
    error: pellet-non-linear-k-uniform-q.fee: 4: there already exists a function named 'T'
    $ 
    ```

    If we tried to define `tau(T)` before `PROBLEM`, then FeenoX would fail when trying to allocate space for the `thermal` problem solution as there would already be defined a symbol `T` for the argument of `tau`.
   
 2. When giving a non-uniform conductivity as a special function, this function has to be a function of space `k(x,y,z)`. The dependence on temperature is introduced by using the solution `T` evaluated at point `(x,y,z)`. That is why we defined the correlation as a function of a single variable and then defined the conductivity as the correlation evaluated at `T(x,y,z)`. But if we used the `MATERIALS` keyword, we could have directly written the whole expression:
 
    ```feenox
    MATERIAL uo2 k=100/(7.5408 + 17.692*tau(T(x,y,z)) + 3.614*tau(T(x,y,z))^2) + 6400/(tau(T(x,y,z))^(5/2))*exp(-16.35/tau(T(x,y,z)))
    ```

 
 
Other than this, we are ready to solve for the temperature distribution in a UO$_2$ pellet with a uniform power source (we will refine the power source and make it more interesting later on).
The geometry is half a fuel pellet with 

 * symmetry conditions on the base (`symmetry` in the mesh)
 * prescribed temperature on the external surface (`external` in the mesh)
 * convection on the top surface (`gap` in the mesh)
 * a uniform power source
 
All the values for these conditions are uniform and correspond roughly to actual figures found in a power nuclear reactor core.

```{.feenox include="pellet-non-linear-k-uniform-q.fee"}
```

The execution with `--snes_monitor` should give something like this:

```terminal
$ feenox pellet-non-linear-k-uniform-q.fee --snes_monitor
  0 SNES Function norm 8.445939693892e+03 
  1 SNES Function norm 2.730091094770e+00 
  2 SNES Function norm 4.316892050932e-02 
  3 SNES Function norm 1.021064388940e-05 
1094.77
$ 
```

::: {#fig:pellet-non-linear-k-uniform-q}
![Top view](pellet-non-linear-k-uniform-q-top.png){#fig:pellet-non-linear-k-uniform-q-top}

![Bottom view (symmetry)](pellet-non-linear-k-uniform-q-bottom.png){#fig:pellet-non-linear-k-uniform-q-bottom}

Temperature and heat flux distribution for a half UO$_2$ pellet with uniform power source.
:::


If we comment out the line with the initial guess, then FeenoX does converge but it needs one step more:

```terminal
$ feenox pellet-non-linear-k-uniform-q.fee --snes_monitor
  0 SNES Function norm 2.222870199708e+02 
  1 SNES Function norm 6.228090579878e-01 
  2 SNES Function norm 4.109509310386e-02 
  3 SNES Function norm 1.603956028971e-04 
  4 SNES Function norm 2.124156299986e-09 
1094.77
$ 
```

 
## Temperature-dependent sources

The volumetric power generated by fissioning nuclei of U$^{235}$ in the UO$_2$ is not uniform throughout the fuel.
It depends on...

 1. The location of the fuel bundle inside the core: in general, pellets located near the center dissipate more power than those located at the periphery).
 2. The location of the pellet inside the fuel element: the effect depends on the reactor design but for reactors where the moderator is separated from the coolant, this change is very significative.
 3. The spatial location $\vec{x}$ inside the pellet: points near the periphery of the pellet now dissipate more power than those located in the bulk because they "have access" to more moderated neutrons coming from the outside.
 4. The temperature $T(\vec{x})$: hot nuclei are less likely to fission.

along with other nuclear-related stuff such as fuel burn-up, concentration of poisons, control systems, etc.

Anyway, this is a tutorial about FeenoX capabilities. Our goal here is to show what FeenoX can do and how to ask it to to such things.

 that let us model a power source by depending both on the space $\vec{x}$ and 
 
[Le Chatelier's principle](https://en.wikipedia.org/wiki/Le_Chatelier's_principle)


# Transient problems

## From an arbitrary initial condition

UO$_2$

## From a steady state

Pump

