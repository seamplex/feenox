#  `done`

Flag that indicates whether the overall calculation is over.





#  `done_outer`

Flag that indicates whether the parametric, optimization of fit calculation is over or not.




It is set to true (i.e. $\neq 0$) by feenox whenever the outer calculation is considered to be finished,
which can be that the parametric calculation swept the desired parameter space or that the
optimization algorithm reached the desired convergence criteria.
If the user sets it to true, the current step is marked as the last outer step and
the transient calculation ends after finishing the step.

#  `done_static`

Flag that indicates whether the static calculation is over or not.




It is set to true (i.e. $\neq 0$) by feenox if `step_static` $\ge$ `static_steps`.
If the user sets it to true, the current step is marked as the last static step and
the static calculation ends after finishing the step.

#  `done_transient`

Flag that indicates whether the transient calculation is over or not.




It is set to true (i.e. $\neq 0$) by feenox if `t` $\ge$ `end_time`.
If the user sets it to true, the current step is marked as the last transient step and
the transient calculation ends after finishing the step.

#  `dt`

Actual value of the time step for transient calculations.




 When solving DAE systems,
this variable is set by feenox. It can be written by the user for example by importing it from another
transient code by means of shared-memory objects. Care should be taken when
solving DAE systems and overwriting `t`. Default value is DEFAULT_DT, which is
a power of two and roundoff errors are thus reduced.

#  `end_time`

Final time of the transient calculation, to be set by the user. 




The default value is zero, meaning no transient calculation.

#  `i`

Dummy index, used mainly in vector and matrix row subindex expressions.





#  `infinite`

A very big positive number.




It can be used as `end_time = infinite` or
to define improper integrals with infinite limits.
 Default is $2^{50} \approx 1 \times 10^{15}$.

#  `in_outer_initial`

Flag that indicates if the current step is the initial step of an optimization of fit run.





#  `in_static`

Flag that indicates if feenox is solving the iterative static calculation.





#  `in_static_first`

Flag that indicates if feenox is in the first step of the iterative static calculation.





#  `in_static_last`

Flag that indicates if feenox is in the last step of the iterative static calculation.





#  `in_transient`

Flag that indicates if feenox is solving transient calculation.





#  `in_transient_first`

Flag that indicates if feenox is in the first step of the transient calculation.





#  `in_transient_last`

Flag that indicates if feenox is in the last step of the transient calculation.





#  `j`

Dummy index, used mainly in matrix column subindex expressions.





#  `max_dt`

Maximum bound for the time step that feenox should take when solving DAE systems.





#  `min_dt`

Minimum bound for the time step that feenox should take when solving DAE systems.





#  `ncores`

The number of online available cores, as returned by `sysconf(_SC_NPROCESSORS_ONLN)`.




This value can be used in the `MAX_DAUGHTERS` expression of the `PARAMETRIC` keyword
(i.e `ncores/2`).

#  `on_gsl_error`

This should be set to a mask that indicates how to proceed if an error ir raised in any
routine of the GNU Scientific Library. 





#  `on_ida_error`

This should be set to a mask that indicates how to proceed if an error ir raised in any
routine of the SUNDIALS Library. 





#  `on_nan`

This should be set to a mask that indicates how to proceed if Not-A-Number signal (such as a division by zero)
is generated when evaluating any expression within feenox.





#  `pi`

A double-precision floating point representaion of the number $\pi$




It is equal to the `M_PI` constant in `math.h` .

#  `pid`

The UNIX process id of the FeenoX instance.





#  `realtime_scale`

If this variable is not zero, then the transient problem is run trying to syncrhonize the
problem time with realtime, up to a scale given.




 For example, if the scale is set to one, then
FeenoX will advance the problem time at the same pace that the real wall time advances. If set to
two, FeenoX time wil advance twice as fast as real time, and so on. If the calculation time is
slower than real time modified by the scale, this variable has no effect on the overall behavior
and execution will proceed as quick as possible with no delays.

#  `rel_error`

Maximum allowed relative error for the solution of DAE systems.




Default value is
is $1 \times 10^{-6}$. If a fine per-variable error control is needed, special vector
`abs_error` should be used.

#  `static_steps`

Number of steps that ought to be taken during the static calculation, to be set by the user. 




The default value is one, meaning only one static step.

#  `step_outer`

Indicates the current step number of the iterative outer calculation (parametric, optimization or fit).





#  `step_static`

Indicates the current step number of the iterative static calculation.





#  `step_transient`

Indicates the current step number of the transient static calculation.





#  `t`

Actual value of the time for transient calculations.




This variable is set by FeenoX, but can be written by
the user for example by importing it from another
transient code by means of shared-memory objects.
Care should be taken when solving DAE systems and overwriting `t`.

#  `zero`

A very small positive number.




It is taken to avoid roundoff
errors when comparing floating point numbers such as replacing $a \leq a_\text{max}$
with $a < a_\text{max} +$ `zero`.
 Default is $(1/2)^{-50} \approx 9\times 10^{-16}$ .


