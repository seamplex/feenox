##  =

> Assign an expression to a variable, a vector or a matrix.

~~~{.feenox style=feenox}
<var>[ [<expr_tmin>, <expr_tmax>] | 
<expr_t> ] = <expr> <vector>(<expr_i>)[<expr_i_min, expr_i_max>] [ [<expr_tmin>, <expr_tmax>] | 
<expr_t> ] = <expr> <matrix>(<expr_i>,<expr_j>)[<expr_i_min, expr_i_max; expr_j_min, expr_j_max>] [ [<expr_tmin>, <expr_tmax>] | 
<expr_t> ] = <expr>
~~~



##  _.=

> Add an equation to the DAE system to be solved in the phase space spanned by `PHASE_SPACE`.

~~~{.feenox style=feenox}
{ 0[(i[,j]][<imin:imax[;jmin:jmax]>] | <expr1> } .= <expr2>
~~~



##  ABORT

> Catastrophically abort the execution and quit FeenoX.

~~~{.feenox style=feenox}
ABORT
~~~


Whenever the instruction `ABORT` is executed, FeenoX quits with a non-zero error leve.
It does not close files nor unlock shared memory objects.
The objective of this instruction is to either debug complex input files
by using only parts of them or to conditionally abort the execution using `IF` clauses.

##  ALIAS

> Define a scalar alias of an already-defined indentifier.

~~~{.feenox style=feenox}
ALIAS { <new_var_name> IS <existing_object> | <existing_object> AS <new_name> }
~~~


The existing object can be a variable, a vector element or a matrix element.
In the first case, the name of the variable should be given as the existing object.
In the second case, to alias the second element of vector `v` to the new name `new`, `v(2)` should be given as the existing object.
In the third case, to alias second element (2,3) of matrix `M` to the new name `new`, `M(2,3)` should be given as the existing object.

##  CLOSE

> Explicitly close a file after input/output.

~~~{.feenox style=feenox}
CLOSE <name>
~~~


The given `<name>` can be either a fixed-string path or an already-defined `FILE`.

##  DEFAULT_ARGUMENT_VALUE

> Give a default value for an optional commandline argument.

~~~{.feenox style=feenox}
DEFAULT_ARGUMENT_VALUE <constant> <string>
~~~


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

##  DIFFERENTIAL

> Explicitly mark variables, vectors or matrices as “differential” to compute intial conditions of DAE systems.

~~~{.feenox style=feenox}
DIFFERENTIAL { <var_1> <var_2> ... | <vector_1> <vector_2> ... | <matrix_1> <matrix_2> ... }
~~~



##  FILE

> Define a file with a particularly formatted name to be used either as input or as output.

~~~{.feenox style=feenox}
< FILE | OUTPUT_FILE | INPUT_FILE > <name> PATH <format> expr_1 expr_2 ... expr_n [ INPUT | OUTPUT | MODE <fopen_mode> ]
~~~


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

> Fit a function of one or more arguments to a set of pointwise-defined data.

~~~{.feenox style=feenox}
FIT <function_to_be_fitted> TO <function_with_data> VIA <var_1> <var_2> ... <var_n>
 [ GRADIENT <expr_1> <expr_2> ... <expr_n> ]
 [ RANGE_MIN <expr_1> <expr_2> ... <expr_j> ]
 [ RANGE_MAX <expr_1> <expr_2> ... <expr_n> ]
 [ DELTAEPSREL <expr> ] [ DELTAEPSABS <expr> ] [ MAX_ITER <expr> ]
 [ VERBOSE ] [ RERUN | DO_NOT_RERUN ]

~~~


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

> Define a function of one or more variables.

~~~{.feenox style=feenox}
FUNCTION <function_name>(<var_1>[,var2,...,var_n]) { [ = <expr> | FILE_PATH <file_path> | ROUTINE <name> | | MESH <name> { DATA <new_vector_name> | VECTOR <existing_vector_name> } { NODES | CELLS } | [ VECTOR_DATA <vector_1> <vector_2> ... <vector_n> <vector_n+1> ] } [COLUMNS <expr_1> <expr_2> ... <expr_n> <expr_n+1> ] [ INTERPOLATION { linear | polynomial | spline | spline_periodic | akima | akima_periodic | steffen | nearest | shepard | shepard_kd | bilinear } ] [ INTERPOLATION_THRESHOLD <expr> ] [ SHEPARD_RADIUS <expr> ] [ SHEPARD_EXPONENT <expr> ] [ SIZES <expr_1> <expr_2> ... <expr_n> ] [ X_INCREASES_FIRST <expr> ] [ DATA <num_1> <num_2> ... <num_N> ]
~~~


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

> Record the time history of a variable as a function of time.

~~~{.feenox style=feenox}
HISTORY <variable> <function>
~~~



##  IF

> Execute a set of instructions if a condition is met.

~~~{.feenox style=feenox}
IF expr 
 <block_of_instructions_if_expr_is_true> 
 [ ELSE 
 <block_of_instructions_if_expr_is_false> ] 
 ENDIF
~~~



##  IMPLICIT

> Define whether implicit definition of variables is allowed or not.

~~~{.feenox style=feenox}
IMPLICIT { NONE | ALLOWED }
~~~


By default, FeenoX allows variables (but not vectors nor matrices) to be
implicitly declared. To avoid introducing errors due to typos, explicit
declaration of variables can be forced by giving `IMPLICIT NONE`.
Whether implicit declaration is allowed or explicit declaration is required
depends on the last `IMPLICIT` keyword given, which by default is `ALLOWED`.

##  INCLUDE

> Include another FeenoX input file.

~~~{.feenox style=feenox}
INCLUDE <file_path> [ FROM <num_expr> ] [ TO <num_expr> ]
~~~


Includes the input file located in the string `file_path` at the current location.
The effect is the same as copying and pasting the contents of the included file
at the location of the `INCLUDE` keyword. The path can be relative or absolute.
Note, however, that when including files inside `IF` blocks that instructions are
conditionally-executed but all definitions (such as function definitions) are processed at
parse-time independently from the evaluation of the conditional.
The included file has to be an actual file path (i.e. it cannot be a FeenoX `FILE`)
because it needs to be resolved at parse time. Yet, the name can contain a
commandline replacement argument such as `$1` so `INCLUDE $1.fee` will include the
file specified after the main input file in the command line.
The optional `FROM` and `TO` keywords can be used to include only portions of a file.

##  INITIAL_CONDITIONS

> Define how initial conditions of DAE problems are computed.

~~~{.feenox style=feenox}
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

##  M4

> Call the `m4` macro processor with definitions from feenox variables or expressions.

~~~{.feenox style=feenox}
M4 { INPUT_FILE <file_id> | FILE_PATH <file_path> } { OUTPUT_FILE <file_id> | OUTPUT_FILE_PATH <file_path> } [ EXPAND <name> ] ... } [ MACRO <name> [ <format> ] <definition> ] ... }
~~~



##  MATRIX

> Define a matrix.

~~~{.feenox style=feenox}
MATRIX <name> ROWS <expr> COLS <expr> [ DATA <expr_1> <expr_2> ... <expr_n> |
~~~


A new matrix of the prescribed size is defined. The number of rows and columns can be an expression which will be
evaluated the very first time the matrix is used and then kept at those constant values.
All elements will be initialized to zero unless `DATA` is given (which should be the last keyword of the line),
in which case the expressions will be evaluated the very first time the matrix is used
and row-major-assigned to each of the elements.
If there are less elements than the matrix size, the remaining values will be zero.
If there are more elements than the matrix size, the values will be ignored.

##  MINIMIZE

> Find the combination of arguments that give a (relative) minimum of a function.

~~~{.feenox style=feenox}
MINIMIZE <function>
 [ METHOD { nmsimplex2 | nmsimplex | nmsimplex2rand | conjugate_fr | conjugate_pr | vector_bfgs2 | vector_bfgs | steepest_descent}
 [ GRADIENT <expr_1> <expr_2> ... <expr_n> ]
 [ GUESS <expr_1> <expr_2> ... <expr_n> ]
 [ MIN <expr_1> <expr_2> ... <expr_n> ]
 [ MAX <expr_1> <expr_2> ... <expr_n> ]
 [ STEP <expr_1> <expr_2> ... <expr_n> ]
 [ MAX_ITER <expr> ] [ TOL <expr> ] [ GRADTOL <expr> ]
 [ VERBOSE ] [ NORERUN ]

~~~



##  OPEN

> Explicitly open a file for input/output.

~~~{.feenox style=feenox}
OPEN <name> [ MODE <fopen_mode> ]
~~~


The given `<name>` can be either a fixed-string path or an already-defined `FILE`.
The mode is only taken into account if the file is not already defined.
Default is write `w`.

##  PARAMETRIC

> Systematically sweep a zone of the parameter space, i.e. perform a parametric run.

~~~{.feenox style=feenox}
PARAMETRIC <var_1> [ ... <var_n> ] [ TYPE { linear | logarithmic | random | gaussianrandom | sobol | niederreiter | halton | reversehalton } ] [ MIN <num_expr_1> ... <num_expr_n> ] [ MAX <num_expr_1> ... <num_expr_n> ] [ STEP <num_expr_1> ... <num_expr_n> ] [ NSTEPS <num_expr_1> ... <num_expr_n> ] [ OUTER_STEPS <num_expr> ] [ MAX_DAUGHTERS <num_expr> ] [ OFFSET <num_expr> ] [ ADIABATIC ]
~~~



##  PHASE_SPACE

> Define which variables, vectors and/or matrices belong to the phase space of the DAE system to be solved.

~~~{.feenox style=feenox}
PHASE_SPACE { <vars> | <vectors> | <matrices> }
~~~



##  PRINT

> Write plain-text and/or formatted data to the standard output or into an output file.

~~~{.feenox style=feenox}
PRINT [ <object_1> <object_2> ... <object_n> ] [ TEXT <string_1> ... TEXT <string_n> ] 
 [ FILE < <file_path> | <file_id> > ] [ HEADER ] [ NONEWLINE ] [ SEP <string> ] 
 [ SKIP_STEP <expr> ] [ SKIP_STATIC_STEP <expr> ] [ SKIP_TIME <expr> ] [ SKIP_HEADER_STEP <expr> ] 

~~~


Each argument `object` which is not a keyword of the `PRINT` instruction will be part of the output.
Objects can be either a matrix, a vector or any valid scalar algebraic expression.
If the given object cannot be solved into a valid matrix, vector or expression, it is treated as a string literal
if `IMPLICIT` is `ALLOWED`, otherwise a parser error is raised.
To explicitly interpret an object as a literal string even if it resolves to a valid numerical expression,
it should be prefixed with the `TEXT` keyword such as `PRINT TEXT 1+1` that would print `1+1` instead of `2`.
Objects and string literals can be mixed and given in any order.
Hashes `#` appearing literal in text strings have to be quoted to prevent the parser to treat them as comments
within the FeenoX input file and thus ignoring the rest of the line, like `PRINT "\# this is a printed comment"`.
Whenever an argument starts with a porcentage sign `%`, it is treated as a C `printf`-compatible format
specifier and all the objects that follow it are printed using the given format until a new format definition is found.
The objects are treated as double-precision floating point numbers, so only floating point formats should be given.
See the `printf(3)` man page for further details. The default format is `DEFAULT_PRINT_FORMAT`.
Matrices, vectors, scalar expressions, format modifiers and string literals can be given in any desired order,
and are processed from left to right.
Vectors are printed element-by-element in a single row. See `PRINT_VECTOR` to print vectors column-wise.
Matrices are printed element-by-element in a single line using row-major ordering if mixed
with other objects but in the natural row and column fashion if it is the only given object in the `PRINT` instruction.
If the `FILE` keyword is not provided, default is to write to `stdout`.
If the `HEADER` keyword is given, a single line containing the literal text
given for each object is printed at the very first time the `PRINT` instruction is
processed, starting with a hash `#` character.
If the `NONEWLINE` keyword is not provided, default is to write a newline `\n` character after
all the objects are processed.
Otherwise, if the last token to be printed is a numerical value, a separator string will be printed but not the newline `\n` character.
If the last token is a string, neither the separator nor the newline will be printed.
The `SEP` keyword expects a string used to separate printed objects.
To print objects without any separation in between give an empty string like `SEP ""`.
The default is a tabulator character 'DEFAULT_PRINT_SEPARATOR' character.
To print an empty line write `PRINT` without arguments.
By default the `PRINT` instruction is evaluated every step.
If the `SKIP_STEP` (`SKIP_STATIC_STEP`) keyword is given, the instruction is processed
only every the number of transient (static) steps that results in evaluating the expression,
which may not be constant.
The `SKIP_HEADER_STEP` keyword works similarly for the optional `HEADER` but
by default it is only printed once. The `SKIP_TIME` keyword use time advancements
to choose how to skip printing and may be useful for non-constant time-step problems.

##  PRINT_FUNCTION

> Print one or more functions as a table of values of dependent and independent variables.

~~~{.feenox style=feenox}
PRINT_FUNCTION <function_1> [ { function_2 | expr_1 } ... { function_n | expr_n-1 } ] [ FILE <file_id> | FILE_PATH <file_path> ] [ HEADER ] [ MIN <expr_1> <expr_2> ... <expr_m> ] [ MAX <expr_1> <expr_2> ... <expr_m> ] [ STEP <expr_1> <expr_2> ... <expr_m> ] [ NSTEPs <expr_1> <expr_2> ... <expr_m> ] [ FORMAT <print_format> ] [ PHYSICAL_ENTITY <name> ]
~~~



##  PRINT_VECTOR

> Print the elements of one or more vectors.

~~~{.feenox style=feenox}
PRINT_VECTOR [ FILE <file_id> ] FILE_PATH <file_path> ] [ { VERTICAL | HORIZONTAL } ] [ ELEMS_PER_LINE <expr> ] [ FORMAT <print_format> ] <vector_1> [ vector_2 ... vector_n ]
~~~



##  READ

> Read data (variables, vectors o matrices) from files or shared-memory segments.

~~~{.feenox style=feenox}
[ READ | WRITE ] [ SHM <name> ] [ { ASCII_FILE_PATH | BINARY_FILE_PATH } <file_path> ] [ { ASCII_FILE | BINARY_FILE } <identifier> ] [ IGNORE_NULL ] [ object_1 object_2 ... object_n ]
~~~



##  SEMAPHORE

> Perform either a wait or a post operation on a named shared semaphore.

~~~{.feenox style=feenox}
[ SEMAPHORE | SEM ] <name> { WAIT | POST }
~~~



##  SHELL

> Execute a shell command.

~~~{.feenox style=feenox}
SHELL <print_format> [ expr_1 expr_2 ... expr_n ]
~~~



##  SOLVE

> Solve a non-linear system of\ $n$ equations with\ $n$ unknowns.

~~~{.feenox style=feenox}
SOLVE <n> UNKNOWNS <var_1> <var_2> ... <var_n> RESIDUALS <expr_1> <expr_2> ... <expr_n> ] GUESS <expr_1> <expr_2> ... <expr_n> ] [ METHOD { dnewton | hybrid | hybrids | broyden } ] [ EPSABS <expr> ] [ EPSREL <expr> ] [ MAX_ITER <expr> ] [ VERBOSE ]
~~~



##  SORT_VECTOR

> Sort the elements of a vector, optionally making the same rearrangement in another vector.

~~~{.feenox style=feenox}
SORT_VECTOR <vector> [ ASCENDING | DESCENDING ] [ <other_vector> ]
~~~


This instruction sorts the elements of `<vector>` into either ascending or descending numerical order.
If `<other_vector>` is given, the same rearrangement is made on it.
Default is ascending order.

##  TIME_PATH

> Force time-dependent problems to pass through specific instants of time.

~~~{.feenox style=feenox}
TIME_PATH <expr_1> [ <expr_2> [ ... <expr_n> ] ]
~~~


The time step `dt` will be reduced whenever the distance between
the current time `t` and the next expression in the list is greater
than `dt` so as to force `t` to coincide with the expressions given.
The list of expresssions should evaluate to a sorted list of values for all times.

##  VAR

> Explicitly define one or more scalar variables.

~~~{.feenox style=feenox}
VAR <name_1> [ <name_2> ] ... [ <name_n> ]
~~~


When implicit definition is allowed (see [`IMPLICIT`]), scalar variables
need not to be defined before being used if from the context FeenoX can tell
that an scalar variable is needed. For instance, when defining a function like
`f(x) = x^2` it is not needed to declare `x` explictly as a scalar variable.
But if one wants to define a function like `g(x) = integral(f(x'), x', 0, x)` then
the variable `x'` needs to be explicitly defined as `VAR x'` before the integral.

##  VECTOR

> Define a vector.

~~~{.feenox style=feenox}
VECTOR <name> SIZE <expr> [ FUNCTION_DATA <function> ] [ DATA <expr_1> <expr_2> ... <expr_n> |
~~~


A new vector of the prescribed size is defined. The size can be an expression which will be
evaluated the very first time the vector is used and then kept at that constant value.
If the keyword `FUNCTION_DATA` is given, the elements of the vector will be synchronized
with the inpedendent values of the function, which should be point-wise defined.
The sizes of both the function and the vector should match.
All elements will be initialized to zero unless `DATA` is given (which should be the last keyword of the line),
in which case the expressions will be evaluated the very first time the vector is used
and assigned to each of the elements.
If there are less elements than the vector size, the remaining values will be zero.
If there are more elements than the vector size, the values will be ignored.

##  WRITE

> Write data (variables, vectors o matrices) to files or shared-memory segments.
See the `READ` keyword for usage details.




