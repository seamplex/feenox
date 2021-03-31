##  ABORT

Catastrophically abort the execution and quit FeenoX.


`ABORT`

Whenever the instruction `ABORT` is executed, FeenoX quits with a non-zero error leve.
It does not close files nor unlock shared memory objects.
The objective of this instruction is to either debug complex input files
by using only parts of them or to conditionally abort the execution using `IF` clauses.

##  ALIAS

Define a scalar alias of an already-defined indentifier.


`ALIAS
{ <new_var_name> IS <existing_object> | <existing_object> AS <new_name> }`

The existing object can be a variable, a vector element or a matrix element.
In the first case, the name of the variable should be given as the existing object.
In the second case, to alias the second element of vector `v` to the new name `new`, `v(2)` should be given as the existing object.
In the third case, to alias second element (2,3) of matrix `M` to the new name `new`, `M(2,3)` should be given as the existing object.

##  BC

Define a boundary condition to be applied to faces, edges and/or vertices.


`BC
<name>
[ MESH <name> ]
[ PHYSICAL_GROUP <name_1>  [ PHYSICAL_GROUP <name_2> [ ... ] ] ]`

If the name of the boundary condition matches a physical group in the mesh, it is automatically linked to that physical group.
If there are many meshes, the mesh this keyword refers to has to be given with `MESH`.
If the boundary condition applies to more than one physical group in the mesh, they can be
added using as many `PHYSICAL_GROUP` keywords as needed.

##  CLOSE

Explicitly close a file after input/output.


`CLOSE
<name>`

The given `<name>` can be either a fixed-string path or an already-defined `FILE`.

##  DEFAULT_ARGUMENT_VALUE

Give a default value for an optional commandline argument.


`DEFAULT_ARGUMENT_VALUE
<constant>
<string>`

If a `$n` construction is found in the input file but the
commandline argument was not given, the default behavior is to
fail complaining that an extra argument has to be given in the
commandline. With this keyword, a default value can be assigned if
no argument is given, thus avoiding the failure and making the argument
optional.
The `<constant>` should be 1, 2, 3, etc. and `<string>` will be expanded
character-by-character where the `$n` construction is.
Whether the resulting expression is to be interpreted as a string or as a
numerical expression will depend on the context.

##  FILE

Define a file with a particularly formatted name to be used either as input or as output.


`< FILE | OUTPUT_FILE | INPUT_FILE >
<name>
PATH <format>
expr_1 expr_2 ... expr_n
[ INPUT | OUTPUT | MODE <fopen_mode> ]`

For reading or writing into files with a fixed path, this instruction is usually not needed as
the `FILE` keyword of other instructions (such as `PRINT` or `MESH`) can take a fixed-string path as an argument.
However, if the file name changes as the execution progresses (say because one file for each step is needed),
then an explicit `FILE` needs to be defined with this keyword and later referenced by the given name.
The path should be given as a `printf`-like format string followed by the expressions which shuold be
evaluated in order to obtain the actual file path. The expressions will always be floating-point expressions,
but the particular integer specifier `%d` is allowed and internally transformed to `%.0f`.
The file can be explicitly defined and `INPUT`, `OUTPUT` or a certain `fopen()` mode can be given (i.e. "a").
If not explicitly given, the nature of the file will be taken from context, i.e. `FILE`s in `PRINT`
will be `OUTPUT` and `FILE`s in `FUNCTION` will be `INPUT`.
This keyword justs defines the `FILE`, it does not open it.
The file will be actually openened (and eventually closed) automatically.
In the rare case where the automated opening and closing does not fit the expected workflow,
the file can be explicitly opened or closed with the instructions `FILE_OPEN` and `FILE_CLOSE`.

##  FIT

Fit a function of one or more arguments to a set of pointwise-defined data.


`FIT
<function_to_be_fitted> 
TO <function_with_data>
VIA
<var_1> <var_2> ... <var_n>@
[ GRADIENT <expr_1> <expr_2> ... <expr_n> ]@
[ RANGE_MIN <expr_1> <expr_2> ... <expr_j> ]@
[ RANGE_MAX <expr_1> <expr_2> ... <expr_n> ]@
[ DELTAEPSREL <expr> ]
[ DELTAEPSABS <expr> ]
[ MAX_ITER <expr> ]@
[ VERBOSE ]
[ RERUN | DO_NOT_RERUN ]@`

The function with the data has to be point-wise defined
(i.e. a `FUNCTION` read from a file with inline `DATA`).
The function to be fitted has to be parametrized with at least one of
the variables provided after the `VIA` keyword.
Only the names of the functions have to be given, not the arguments.
Both functions have to have the same number of arguments.
The initial guess of the solution is given by the initial value of the variables listed in the `VIA` keyword.
Analytical expressions for the gradient of the function to be fitted with respect
to the parameters to be fitted can be optionally given with the `GRADIENT` keyword.
If none is provided, the gradient will be computed numerically using finite differences.
A range over which the residuals are to be minimized can be given with `RANGE_MIN` and `RANGE_MAX`.
The expressions give the range of the arguments of the functions, not of the parameters.
For multidimensional fits, the range is an hypercube.
If no range is given, all the definition points of the function with the data are used for the fit.
Convergence can be controlled by giving the relative and absolute tolreances with
`DELTAEPSREL` (default `DEFAULT_NLIN_FIT_EPSREL`) and `DELTAEPSABS` (default `DEFAULT_NLIN_FIT_EPSABS`),
and with the maximum number of iterations `MAX_ITER` (default DEFAULT_NLIN_FIT_MAX_ITER).
If the optional keyword `VERBOSE` is given, some data of the intermediate steps is written in the standard output.
The combination of arguments that minimize the function are computed and stored in the variables.
So if `f(x,y)` is to be minimized, after a `MINIMIZE f` both `x` and `y` would have the appropriate values.
The details of the method used can be found in [GSL’s documentation](https:\//www.gnu.org/software/gsl/doc/html/multimin.html).
Some of them use derivatives and some of them do not.
Default method is `DEFAULT_MINIMIZER_METHOD`, which does not need derivatives.

##  FUNCTION

Define a function of one or more variables.


`FUNCTION
<function_name>(<var_1>[,var2,...,var_n])
{
[ = <expr> |
FILE_PATH <file_path> |
ROUTINE <name> |
| MESH <name> { DATA <new_vector_name> | VECTOR <existing_vector_name> } { NODES | CELLS } |
[ VECTOR_DATA <vector_1> <vector_2> ... <vector_n> <vector_n+1> ]
}
[COLUMNS <expr_1> <expr_2> ... <expr_n> <expr_n+1> ]
[ INTERPOLATION
{
linear |
polynomial |
spline |
spline_periodic |
akima |
akima_periodic |
steffen |
nearest |
shepard |
shepard_kd |
bilinear
} ]
[ INTERPOLATION_THRESHOLD <expr> ]
[ SHEPARD_RADIUS <expr> ]
[ SHEPARD_EXPONENT <expr> ]
[ SIZES <expr_1> <expr_2> ... <expr_n> ]
[ X_INCREASES_FIRST <expr> ]
[ DATA <num_1> <num_2> ... <num_N> ]`

The number of variables $n$ is given by the number of arguments given between parenthesis after the function name.
The arguments are defined as new variables if they had not been already defined explictly as scalar variables.
If the function is given as an algebraic expression, the short-hand operator `:=` can be used.
That is to say, `FUNCTION f(x) = x^2` is equivalent to `f(x) := x^2`.
If a `FILE_PATH` is given, an ASCII file containing at least $n+1$ columns is expected.
By default, the first $n$ columns are the values of the arguments and the last column is the value of the function at those points.
The order of the columns can be changed with the keyword `COLUMNS`, which expects $n+1$ expressions corresponding to the column numbers.
A function of type `ROUTINE` calls an already-defined user-provided routine using the `CALL` keyword and passes the values of the variables in each required evaluation as a `double *` argument.
If `MESH` is given, the definition points are the nodes or the cells of the mesh.
The function arguments should be $(x)$, $(x,y)$ or $(x,y,z)$ matching the dimension the mesh.
If the keyword `DATA` is used, a new empty vector of the appropriate size is defined.
The elements of this new vector can be assigned to the values of the function at the $i$-th node or cell.
If the keyword `VECTOR` is used, the values of the dependent variable are taken to be the values of the already-existing vector.
Note that this vector should have the size of the number of nodes or cells the mesh has, depending on whether `NODES` or `CELLS` is given.
If `VECTOR_DATA` is given, a set of $n+1$ vectors of the same size is expected.
The first $n+1$ correspond to the arguments and the last one is the function value.
Interpolation schemes can be given for either one or multi-dimensional functions with `INTERPOLATION`.
Available schemes for $n=1$ are:

 * linear
 * polynomial, the grade is equal to the number of data minus one
 * spline, cubic (needs at least 3 points)
 * spline_periodic
 * akima (needs at least 5 points)
 * akima_periodic (needs at least 5 points)
 * steffen, always-monotonic splines-like (available only with GSL >= 2.0)

Default interpolation scheme for one-dimensional functions is `DEFAULT_INTERPOLATION`.

Available schemes for $n>1$ are:

 * nearest, $f(\vec{x})$ is equal to the value of the closest definition point
 * shepard, [inverse distance weighted average definition points](https://en.wikipedia.org/wiki/Inverse_distance_weighting) (might lead to inefficient evaluation)
 * shepard_kd, [average of definition points within a kd-tree](https://en.wikipedia.org/wiki/Inverse_distance_weighting#Modified_Shepard&#39;s_method) (more efficient evaluation provided `SHEPARD_RADIUS` is set to a proper value)
 * bilinear, only available if the definition points configure an structured hypercube-like grid. If $n>3$, `SIZES` should be given.

For $n>1$, if the euclidean distance between the arguments and the definition points is smaller than `INTERPOLATION_THRESHOLD`, the definition point is returned and no interpolation is performed.
Default value is square root of `DEFAULT_MULTIDIM_INTERPOLATION_THRESHOLD`.
The initial radius of points to take into account in `shepard_kd` is given by `SHEPARD_RADIUS`. If no points are found, the radius is double until at least one definition point is found.
The radius is doubled until at least one point is found.
Default is `DEFAULT_SHEPARD_RADIUS`.
The exponent of the `shepard` method is given by `SHEPARD_EXPONENT`.
Default is `DEFAULT_SHEPARD_EXPONENT`.
When requesting `bilinear` interpolation for $n>3$, the number of definition points for each argument variable has to be given with `SIZES`,
and wether the definition data is sorted with the first argument changing first (`X_INCREASES_FIRST` evaluating to non-zero) or with the last argument changing first (zero).
The function can be pointwise-defined inline in the input using `DATA`. This should be the last keyword of the line, followed by $N=k\cdot (n+1)$ expresions giving $k$ definition points: $n$ arguments and the value of the function.
Multiline continuation using brackets `{` and `}` can be used for a clean data organization. See the examples.

##  HISTORY

Record the time history of a variable as a function of time.


`HISTORY
<variable>
<function>`


##  IF

Execute a set of instructions if a condition is met.


`IF expr @
 <block_of_instructions_if_expr_is_true> @
[ ELSE  @
 <block_of_instructions_if_expr_is_false> ] @
ENDIF`


##  IMPLICIT

Define whether implicit definition of variables is allowed or not.


`IMPLICIT
{ NONE | ALLOWED }`

By default, FeenoX allows variables (but not vectors nor matrices) to be
implicitly declared. To avoid introducing errors due to typos, explicit
declaration of variables can be forced by giving `IMPLICIT NONE`.
Whether implicit declaration is allowed or explicit declaration is required
depends on the last `IMPLICIT` keyword given, which by default is `ALLOWED`.

