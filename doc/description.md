
FeenoX solves a problem defined in an plain-text input file and writes user-defined outputs to the standard output and/or files, either also plain-text or with a particular format for further post-processing. The syntax of this input file is designed to be as self-describing as possible, using English keywords that explains FeenoX what problem it has to solve in a way is understandable by both humans and computers. Keywords can work either as

 1. Definitions, for instance ”define function\ $f(x)$ and read its data from file `f.dat`”), or as
 2. Instructions, such as “write the stress at point $D$ into the standard output”. 

A person can tell if a keyword is a definition or an instruction because the former are nouns (`FUNCTION`) and the latter verbs (`PRINT`). The equal sign `=` is a special keyword that is neither a verb nor a noun, and its meaning changes depending on what is on the left hand side of the assignment.

 a. If there is a function, then it is a definition: define an algebraic function to be equal to the expression on the right-hand side, e.g.:
 
    ```{.feenox include="examples/fxy.fee"}
    ```
 
 b. If there is a variable, vector or matrix, it is an instruction: evaluate the expression on the right-hand side and assign it to the variable or vector (or matrix) element indicated in the left-hand side. Strictly speaking, if the variable has not already been defined (and implicit declaration is allowed), then the variable is also defined as well, e.g:
 
    ```{.feenox include="examples/equal.fee"}
    ```
    
    There is no need to explicitly define the scalar variable `a` with `VAR` since the first assignment also defines it implicitly (if this is allowed by the keyword `IMPLICIT`).


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
In general transient PDEs involve a mesh, material properties, initial conditions, transient boundary conditions, etc. And they create a lot of data since results mean spatial and temporal distributions of one or more scalar fields:

```{.feenox include="examples/transient_pde.fee"}
```

PETSc’s TS also honors the `min_dt` and `max_dt` variables, but the time step is controlled by the allowed relative error with the special variable `ts_rtol`. Again, see the section of the [Partial Differential Equations subsystem] for more information. 


# Algebraic expressions

To be done.

 * Everything is an expression.

# Initial conditions

# Expansions of command line arguments

