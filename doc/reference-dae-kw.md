#  `INITIAL_CONDITIONS`

Define how initial conditions of DAE problems are computed.


~~~feenox
INITIAL_CONDITIONS { AS_PROVIDED | FROM_VARIABLES | FROM_DERIVATIVES }  
~~~



In DAE problems, initial conditions may be either:

 * equal to the provided expressions (`AS_PROVIDED`)
 * the derivatives computed from the provided phase-space variables (`FROM_VARIABLES`)
 * the phase-space variables computed from the provided derivatives (`FROM_DERIVATIVES`)

In the first case, it is up to the user to fulfill the DAE system at\ $t = 0$.
If the residuals are not small enough, a convergence error will occur.
The `FROM_VARIABLES` option means calling IDA’s `IDACalcIC` routine with the parameter `IDA_YA_YDP_INIT`.
The `FROM_DERIVATIVES` option means calling IDA’s `IDACalcIC` routine with the parameter IDA_Y_INIT.
Wasora should be able to automatically detect which variables in phase-space are differential and
which are purely algebraic. However, the [`DIFFERENTIAL`] keyword may be used to explicitly define them.
See the (SUNDIALS documentation)[https://computation.llnl.gov/casc/sundials/documentation/ida_guide.pdf] for further information.

#  `PHASE_SPACE`

Asks FeenoX to solve a set of algebraic-differntial equations and define the variables, vectors and/or matrices that span the phase space.


~~~feenox
PHASE_SPACE  
~~~




#  `TIME_PATH`

Force time-dependent problems to pass through specific instants of time.


~~~feenox
TIME_PATH <expr_1> [ <expr_2>  [ ... <expr_n> ] ]  
~~~



The time step `dt` will be reduced whenever the distance between
the current time `t` and the next expression in the list is greater
than `dt` so as to force `t` to coincide with the expressions given.
The list of expresssions should evaluate to a sorted list of values for all times.


