##  ABORT

Catastrophically abort the execution and quit FeenoX.


::: {.usage}
~~~{.feenox style=feenox}
ABORT
~~~
:::



Whenever the instruction `ABORT` is executed, FeenoX quits with a non-zero error leve.
It does not close files nor unlock shared memory objects.
The objective of this instruction is to either debug complex input files
by using only parts of them or to conditionally abort the execution using `IF` clauses.

##  ALIAS

Define a scalar alias of an already-defined indentifier.


::: {.usage}
~~~{.feenox style=feenox}
ALIAS { <new_var_name> IS <existing_object> | <existing_object> AS <new_name> }
~~~
:::



The existing object can be a variable, a vector element or a matrix element.
In the first case, the name of the variable should be given as the existing object.
In the second case, to alias the second element of vector `v` to the new name `new`, `v(2)` should be given as the existing object.
In the third case, to alias second element (2,3) of matrix `M` to the new name `new`, `M(2,3)` should be given as the existing object.

##  BC

Define a boundary condition to be applied to faces, edges and/or vertices.


::: {.usage}
~~~{.feenox style=feenox}
BC <name> [ MESH <name> ] [ PHYSICAL_GROUP <name_1> PHYSICAL_GROUP <name_2> ... ] [ <bc_data1> <bc_data2> ... ]
~~~
:::



If the name of the boundary condition matches a physical group in the mesh, it is automatically linked to that physical group.
If there are many meshes, the mesh this keyword refers to has to be given with `MESH`.
If the boundary condition applies to more than one physical group in the mesh,
they can be added using as many `PHYSICAL_GROUP` keywords as needed.
Each `<bc_data>` argument is a single string whose meaning depends on the type
of problem being solved. For instance `T=150*sin(x/pi)` prescribes the
temperature to depend on space as the provided expression in a
thermal problem and `fixed` fixes the displacements in all the directions
in a mechanical or modal problem.
See the particular section on boundary conditions for further details.

##  CLOSE

Explicitly close a file after input/output.


::: {.usage}
~~~{.feenox style=feenox}
CLOSE <name>
~~~
:::



The given `<name>` can be either a fixed-string path or an already-defined `FILE`.

##  DEFAULT_ARGUMENT_VALUE

Give a default value for an optional commandline argument.


::: {.usage}
~~~{.feenox style=feenox}
DEFAULT_ARGUMENT_VALUE <constant> <string>
~~~
:::



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

##  DUMP

Dump raw PETSc objects used to solve PDEs into files.


::: {.usage}
~~~{.feenox style=feenox}
DUMP [ FORMAT { binary | ascii | octave } ] [ K | K_bc | b | b_bc | M | M_bc |
~~~
:::




##  FILE

Define a file with a particularly formatted name to be used either as input or as output.


::: {.usage}
~~~{.feenox style=feenox}
< FILE | OUTPUT_FILE | INPUT_FILE > <name> PATH <format> expr_1 expr_2 ... expr_n [ INPUT | OUTPUT | MODE <fopen_mode> ]
~~~
:::



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

##  FIND_EXTREMA

Find and/or compute the absolute extrema of a function or expression over a mesh (or a subset of it).


::: {.usage}
~~~{.feenox style=feenox}
FIND_EXTREMA { <expression> | <function> } [ OVER <physical_group> ] [ MESH <mesh_identifier> ] [ NODES | CELLS ]
 [ MIN <variable> ] [ MAX <variable> ] [ X_MIN <variable> ] [ X_MAX <variable> ] [ Y_MIN <variable> ] [ Y_MAX <variable> ] [ Z_MIN <variable> ] [ Z_MAX <variable> ] [ I_MIN <variable> ] [ I_MAX <variable> ]
~~~
:::



Either an expression or a function of space $x$, $y$ and/or $z$ should be given.
By default the search is performed over the highest-dimensional elements of the mesh,
i.e. over the whole volume, area or length for three, two and one-dimensional meshes, respectively.
If the search is to be carried out over just a physical group, it has to be given in `OVER`.
If there are more than one mesh defined, an explicit one has to be given with `MESH`.
Either `NODES` or `CELLS` define how the integration is to be performed.
With `NODES` the integration is performed using the Gauss points
and weights associated to each element type.
With `CELLS` the integral is computed as the sum of the product of the
integrand at the center of each cell (element) and the cell’s volume.
Do expect differences in the results and efficiency between these two approaches
depending on the nature of the integrand.
The value of the absolute minimum (maximum) is stored in the variable indicated by `MIN` (`MAX`).
If the variable does not exist, it is created.
The value of the $x$-$y$-$z$\ coordinate of the absolute minimum (maximum)
is stored in the variable indicated by `X_MIN`-`Y_MIN`-`Z_MIN` (`X_MAX`-`Y_MAX`-`Z_MAX`).
If the variable does not exist, it is created.
The index (either node or cell) where the absolute minimum (maximum) is found
is stored in the variable indicated by `I_MIN` (`I_MAX`).

##  FIT

Find parameters to fit an analytical function to a pointwise-defined function.


::: {.usage}
~~~{.feenox style=feenox}
FIT <function_to_be_fitted> TO <function_with_data> VIA <var_1> <var_2> ... <var_n>
 [ GRADIENT <expr_1> <expr_2> ... <expr_n> ]
 [ RANGE_MIN <expr_1> <expr_2> ... <expr_j> ]
 [ RANGE_MAX <expr_1> <expr_2> ... <expr_n> ]
 [ TOL_REL <expr> ] [ TOL_ABS <expr> ] [ MAX_ITER <expr> ]
 [ VERBOSE ]
~~~
:::



The combination of arguments that minimize the function are computed and stored in the variables.
So if `f(x,y)` is to be minimized, after a `MINIMIZE f` both `x` and `y` would have the appropriate values.
The details of the method used can be found in [GSL’s documentation](https:\//www.gnu.org/software/gsl/doc/html/multimin.html).
Some of them use derivatives and some of them do not.
Default method is `DEFAULT_MINIMIZER_METHOD`, which does not need derivatives.
The function with the data has to be point-wise defined
(i.e. a `FUNCTION` read from a file, with inline `DATA` or defined over a mesh).
The function to be fitted has to be parametrized with at least one of
the variables provided after the `USING` keyword.
For example to fit\ $f(x,y)=a x^2 + b sqrt(y)$ to a pointwise-defined
function\ $g(x,y)$ one gives `FIT f TO g VIA a b`.
Only the names of the functions have to be given, not the arguments.
Both functions have to have the same number of arguments.
The initial guess of the solution is given by the initial value of the variables after the `VIA` keyword.
Analytical expressions for the gradient of the function to be fitted with respect
to the parameters to be fitted can be optionally given with the `GRADIENT` keyword.
If none is provided, the gradient will be computed numerically using finite differences.
A range over which the residuals are to be minimized can be given with `RANGE_MIN` and `RANGE_MAX`.
The expressions give the range of the arguments of the functions, not of the parameters.
For multidimensional fits, the range is an hypercube.
If no range is given, all the definition points of the function with the data are used for the fit.
Convergence can be controlled by giving the relative and absolute tolreances with
`TOL_REL` (default `DEFAULT_NLIN_FIT_EPSREL`) and `TOL_ABS` (default `DEFAULT_NLIN_FIT_EPSABS`),
and with the maximum number of iterations `MAX_ITER` (default DEFAULT_NLIN_FIT_MAX_ITER).
If the optional keyword `VERBOSE` is given, some data of the intermediate steps is written in the standard output.

##  FUNCTION

Define a function of one or more variables.


::: {.usage}
~~~{.feenox style=feenox}
FUNCTION <function_name>(<var_1>[,var2,...,var_n]) { = <expr> | FILE { <file_path> | <file_id> } | VECTORS <vector_1> <vector_2> ... <vector_n> <vector_data> | DATA <num_1> <num_2> ... <num_N> } [ INTERPOLATION { linear | polynomial | spline | spline_periodic | akima | akima_periodic | steffen | nearest | shepard | shepard_kd | bilinear } ] ROUTINE <name> | | MESH <name> { DATA <new_vector_name> | VECTOR <existing_vector_name> } { NODES | CELLS } | [ VECTOR_DATA <vector_1> <vector_2> ... <vector_n> <vector_n+1> ] } [COLUMNS <expr_1> <expr_2> ... <expr_n> <expr_n+1> ] [ INTERPOLATION_THRESHOLD <expr> ] [ SHEPARD_RADIUS <expr> ] [ SHEPARD_EXPONENT <expr> ] [ SIZES <expr_1> <expr_2> ... <expr_n> ] [ X_INCREASES_FIRST <expr> ]
~~~
:::



The number of variables $n$ is given by the number of arguments given between parenthesis after the function name.
The arguments are defined as new variables if they had not been already defined explictly as scalar variables.
If the function is given as an algebraic expression, the short-hand operator `:=` can be used.
That is to say, `FUNCTION f(x) = x^2` is equivalent to `f(x) := x^2`.
If a `FILE` is given, an ASCII file containing at least $n+1$ columns is expected.
By default, the first $n$ columns are the values of the arguments and the last column
is the value of the function at those points.
The order of the columns can be changed with the keyword `COLUMNS`,
which expects $n+1$ expressions corresponding to the column numbers.
If `VECTORS` is given, a set of $n+1$ vectors of the same size is expected.
The first $n$ correspond to the arguments and the last one to the function values.
The function can be pointwise-defined inline in the input using `DATA`.
This should be the last keyword of the line, followed by $N=k \cdot (n+1)$ expresions
giving\ $k$ definition points: $n$ arguments and the value of the function.
Multiline continuation using brackets `{` and `}` can be used for a clean data organization.
Interpolation schemes can be given for either one or multi-dimensional functions with `INTERPOLATION`.
Available schemes for $n=1$ are:

 * linear
 * polynomial, the grade is equal to the number of data minus one
 * spline, cubic (needs at least 3 points)
 * spline_periodic
 * akima (needs at least 5 points)
 * akima_periodic (needs at least 5 points)
 * steffen, always-monotonic splines-like interpolator

Default interpolation scheme for one-dimensional functions is `DEFAULT_INTERPOLATION`.

Available schemes for $n>1$ are:

 * nearest, $f(\vec{x})$ is equal to the value of the closest definition point
 * shepard, [inverse distance weighted average definition points](https://en.wikipedia.org/wiki/Inverse_distance_weighting) (might lead to inefficient evaluation)
 * shepard_kd, [average of definition points within a kd-tree](https://en.wikipedia.org/wiki/Inverse_distance_weighting#Modified_Shepard&#39;s_method) (more efficient evaluation provided `SHEPARD_RADIUS` is set to a proper value)
 * bilinear, only available if the definition points configure an structured hypercube-like grid. If $n>3$, `SIZES` should be given.

A function of type `ROUTINE` calls an already-defined user-provided routine using the `CALL` keyword and passes the values of the variables in each required evaluation as a `double *` argument.
If `MESH` is given, the definition points are the nodes or the cells of the mesh.
The function arguments should be $(x)$, $(x,y)$ or $(x,y,z)$ matching the dimension the mesh.
If the keyword `DATA` is used, a new empty vector of the appropriate size is defined.
The elements of this new vector can be assigned to the values of the function at the $i$-th node or cell.
If the keyword `VECTOR` is used, the values of the dependent variable are taken to be the values of the already-existing vector.
Note that this vector should have the size of the number of nodes or cells the mesh has, depending on whether `NODES` or `CELLS` is given.
If `VECTOR_DATA` is given, a set of $n+1$ vectors of the same size is expected.
The first $n+1$ correspond to the arguments and the last one is the function value.
For $n>1$, if the euclidean distance between the arguments and the definition points is smaller than `INTERPOLATION_THRESHOLD`, the definition point is returned and no interpolation is performed.
Default value is square root of `DEFAULT_MULTIDIM_INTERPOLATION_THRESHOLD`.
The initial radius of points to take into account in `shepard_kd` is given by `SHEPARD_RADIUS`. If no points are found, the radius is double until at least one definition point is found.
The radius is doubled until at least one point is found.
Default is `DEFAULT_SHEPARD_RADIUS`.
The exponent of the `shepard` method is given by `SHEPARD_EXPONENT`.
Default is `DEFAULT_SHEPARD_EXPONENT`.
When requesting `bilinear` interpolation for $n>3$, the number of definition points for each argument variable has to be given with `SIZES`,
and wether the definition data is sorted with the first argument changing first (`X_INCREASES_FIRST` evaluating to non-zero) or with the last argument changing first (zero).

##  HISTORY

Record the time history of a variable as a function of time.


::: {.usage}
~~~{.feenox style=feenox}
HISTORY <variable> <function>
~~~
:::




##  IF

Execute a set of instructions if a condition is met.


::: {.usage}
~~~{.feenox style=feenox}
IF expr 
 <block_of_instructions_if_expr_is_true> 
 [ ELSE 
 <block_of_instructions_if_expr_is_false> ] 
 ENDIF
~~~
:::




##  IMPLICIT

Define whether implicit definition of variables is allowed or not.


::: {.usage}
~~~{.feenox style=feenox}
IMPLICIT { NONE | ALLOWED }
~~~
:::



By default, FeenoX allows variables (but not vectors nor matrices) to be
implicitly declared. To avoid introducing errors due to typos, explicit
declaration of variables can be forced by giving `IMPLICIT NONE`.
Whether implicit declaration is allowed or explicit declaration is required
depends on the last `IMPLICIT` keyword given, which by default is `ALLOWED`.

##  INCLUDE

Include another FeenoX input file.


::: {.usage}
~~~{.feenox style=feenox}
INCLUDE <file_path> [ FROM <num_expr> ] [ TO <num_expr> ]
~~~
:::



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

Define how initial conditions of DAE problems are computed.


::: {.usage}
~~~{.feenox style=feenox}
INITIAL_CONDITIONS { AS_PROVIDED | FROM_VARIABLES | FROM_DERIVATIVES }
~~~
:::



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

##  INTEGRATE

Spatially integrate a function or expression over a mesh (or a subset of it).


::: {.usage}
~~~{.feenox style=feenox}
INTEGRATE { <expression> | <function> } [ OVER <physical_group> ] [ MESH <mesh_identifier> ] [ NODES | CELLS ]
 RESULT <variable>

~~~
:::



Either an expression or a function of space $x$, $y$ and/or $z$ should be given.
If the integrand is a function, do not include the arguments, i.e. instead of `f(x,y,z)` just write `f`.
The results should be the same but efficiency will be different (faster for pure functions).
By default the integration is performed over the highest-dimensional elements of the mesh,
i.e. over the whole volume, area or length for three, two and one-dimensional meshes, respectively.
If the integration is to be carried out over just a physical group, it has to be given in `OVER`.
If there are more than one mesh defined, an explicit one has to be given with `MESH`.
Either `NODES` or `CELLS` define how the integration is to be performed.
With `NODES` the integration is performed using the Gauss points
and weights associated to each element type.
With `CELLS` the integral is computed as the sum of the product of the
integrand at the center of each cell (element) and the cell’s volume.
Do expect differences in the results and efficiency between these two approaches
depending on the nature of the integrand.
The scalar result of the integration is stored in the variable given by
the mandatory keyword `RESULT`.
If the variable does not exist, it is created.

##  M4

Call the `m4` macro processor with definitions from feenox variables or expressions.


::: {.usage}
~~~{.feenox style=feenox}
M4 { INPUT_FILE <file_id> | FILE_PATH <file_path> } { OUTPUT_FILE <file_id> | OUTPUT_FILE_PATH <file_path> } [ EXPAND <name> ] ... } [ MACRO <name> [ <format> ] <definition> ] ... }
~~~
:::




##  MATERIAL

Define a material its and properties to be used in volumes.


::: {.usage}
~~~{.feenox style=feenox}
MATERIAL <name> [ MESH <name> ] [ PHYSICAL_GROUP <name_1> [ PHYSICAL_GROUP <name_2> [ ... ] ] ] [ <property_name_1>=<expr_1> [ <property_name_2>=<expr_2> [ ... ] ] ]
~~~
:::



If the name of the material matches a physical group in the mesh, it is automatically linked to that physical group.
If there are many meshes, the mesh this keyword refers to has to be given with `MESH`.
If the material applies to more than one physical group in the mesh, they can be
added using as many `PHYSICAL_GROUP` keywords as needed.
The names of the properties in principle can be arbitrary, but each problem type
needs a minimum set of properties defined with particular names.
For example, steady-state thermal problems need at least the conductivity which
should be named\ `k`. If the problem is transient, it will also need
heat capacity\ `rhocp` or diffusivity\ `alpha`.
Mechanical problems need Young modulus\ `E` and Poisson’s ratio\ `nu`.
Modal also needs density\ `rho`. Check the particular documentation for each problem type.
Besides these mandatory properties, any other one can be defined.
For instance, if one mandatory property dependend on the concentration of boron in the material,
a new per-material property can be added named `boron` and then the function `boron(x,y,z)` can
be used in the expression that defines the mandatory property.

##  MATRIX

Define a matrix.


::: {.usage}
~~~{.feenox style=feenox}
MATRIX <name> ROWS <expr> COLS <expr> [ DATA <expr_1> <expr_2> ... <expr_n> |
~~~
:::



A new matrix of the prescribed size is defined. The number of rows and columns can be an expression which will be
evaluated the very first time the matrix is used and then kept at those constant values.
All elements will be initialized to zero unless `DATA` is given (which should be the last keyword of the line),
in which case the expressions will be evaluated the very first time the matrix is used
and row-major-assigned to each of the elements.
If there are less elements than the matrix size, the remaining values will be zero.
If there are more elements than the matrix size, the values will be ignored.

##  MESH_WRITE



::: {.usage}
~~~{.feenox style=feenox}
{ <file_path> | <file_id> } [ MESH <mesh_identifier> ] [ NO_MESH ] [ FILE_FORMAT { gmsh | vtk } ] [ NO_PHYSICAL_NAMES ] [ NODE | CELL ] [ SCALAR_FORMAT <printf_specification>] [ VECTOR <field_x> <field_y> <field_z> ] [...] [ <field_1> ] [ <field_2> ] ...
~~~
:::



Either a file identifier (defined previously with a `FILE` keyword) or a file path should be given.
The format is automatically detected from the extension. Otherwise, the keyword `FILE_FORMAT` can
be use to give the format explicitly.
If there are several meshes defined then which one should be used has to be
given explicitly with `MESH`.
If the `NO_MESH` keyword is given, only the results are written into the output file.
Depending on the output format, this can be used to avoid repeating data and/or
creating partial output files which can the be latter assembled by post-processing scripts.
When targetting the `.msh` output format, if `NO_PHYSICAL_NAMES` is given then the
section that sets the actual names of the physical entities is not written.
This can be needed to avoid name clashes when reading multiple `.msh` files.
The output is node-based by default. This can be controlled with both the
`NODE` and `CELL` keywords. All fields that come after a `NODE` (`CELL`) keyword
will be written at the node (cells). These keywords can be used several times
and mixed with fields. For example `CELL k(x,y,z) NODE T sqrt(x^2+y^2) CELL 1+z` will
write the conductivity and the expression $1+z$ as cell-based and the temperature
$T(x,y,z)$ and the expression $\sqrt{x^2+y^2}$ as a node-based fields.
Also, the `SCALAR_FORMAT` keyword can be used to define the precision of the ASCII
representation of the fields that follow. Default is `%g`.
The data to be written has to be given as a list of fields,
i.e. distributions (such as `k` or `E`), functions of space (such as `T`)
and/or expressions (such as `x^2+y^2+z^2`).
Each field is written as a scalar, unless the keyword `VECTOR` is given.
In this case, there should be exactly three fields following `VECTOR`.

##  MINIMIZE

Find the combination of arguments that give a (relative) minimum of a function.


::: {.usage}
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
:::




##  OPEN

Explicitly open a file for input/output.


::: {.usage}
~~~{.feenox style=feenox}
OPEN <name> [ MODE <fopen_mode> ]
~~~
:::



The given `<name>` can be either a fixed-string path or an already-defined `FILE`.
The mode is only taken into account if the file is not already defined.
Default is write `w`.

##  PARAMETRIC

Systematically sweep a zone of the parameter space, i.e. perform a parametric run.


::: {.usage}
~~~{.feenox style=feenox}
PARAMETRIC <var_1> [ ... <var_n> ] [ TYPE { linear | logarithmic | random | gaussianrandom | sobol | niederreiter | halton | reversehalton } ] [ MIN <num_expr_1> ... <num_expr_n> ] [ MAX <num_expr_1> ... <num_expr_n> ] [ STEP <num_expr_1> ... <num_expr_n> ] [ NSTEPS <num_expr_1> ... <num_expr_n> ] [ OUTER_STEPS <num_expr> ] [ MAX_DAUGHTERS <num_expr> ] [ OFFSET <num_expr> ] [ ADIABATIC ]
~~~
:::




##  PHASE_SPACE

Define the variables, vectors and/or matrices that span the phase space of the DAE system of equations.


::: {.usage}
~~~{.feenox style=feenox}
PHASE_SPACE PHASE_SPACE { <vars> ... | <vectors> ... | <matrices> ... }
~~~
:::




##  PHYSICAL_GROUP

Explicitly defines a physical group of elements on a mesh.


::: {.usage}
~~~{.feenox style=feenox}
PHYSICAL_GROUP <name> [ MESH <name> ] [ DIMENSION <expr> ] [ ID <expr> ]
 [ MATERIAL <name> | | BC <name> [ BC ... ] ]

~~~
:::



This keyword should seldom be needed. Most of the times,
 a combination of `MATERIAL` and `BC` ought to be enough for most purposes.
The name of the `PHYSICAL_GROUP` keyword should match the name of the physical group defined within the input file.
If there is no physical group with the provided name in the mesh, this instruction has no effect.
If there are many meshes, an explicit mesh can be given with `MESH`.
Otherwise, the physical group is defined on the main mesh.
An explicit dimension of the physical group can be provided with `DIMENSION`.
An explicit id can be given with `ID`.
Both dimension and id should match the values in the mesh.
For volumetric elements, physical groups can be linked to materials using `MATERIAL`.
Note that if a material is created with the same name as a physical group in the mesh,
they will be linked automatically, so there is no need to use `PHYSCAL_GROUP` for this.
The `MATERIAL` keyword in `PHYSICAL_GROUP` is used to link a physical group
in a mesh file and a material in the feenox input file with different names.
Likewise, for non-volumetric elements, physical groups can be linked to boundary using `BC`.
As in the preceeding case, if a boundary condition is created with the same name as a physical group in the mesh,
they will be linked automatically, so there is no need to use `PHYSCAL_GROUP` for this.
The `BC` keyword in `PHYSICAL_GROUP` is used to link a physical group
in a mesh file and a boundary condition in the feenox input file with different names.
Note that while there can be only one `MATERIAL` associated to a physical group,
there can be many `BC`s associated to a physical group.

##  PRINT

Write plain-text and/or formatted data to the standard output or into an output file.


::: {.usage}
~~~{.feenox style=feenox}
PRINT [ <object_1> <object_2> ... <object_n> ] [ TEXT <string_1> ... TEXT <string_n> ] 
 [ FILE { <file_path> | <file_id> } ] [ HEADER ] [ NONEWLINE ] [ SEP <string> ] 
 [ SKIP_STEP <expr> ] [ SKIP_STATIC_STEP <expr> ] [ SKIP_TIME <expr> ] [ SKIP_HEADER_STEP <expr> ] 

~~~
:::



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
Vectors are printed element-by-element in a single row.
See `PRINT_VECTOR` to print one or more vectors with one element per line (i.e. vertically).
Matrices are printed element-by-element in a single line using row-major ordering if mixed
with other objects but in the natural row and column fashion
if it is the only given object in the `PRINT` instruction.
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

Print one or more functions as a table of values of dependent and independent variables.


::: {.usage}
~~~{.feenox style=feenox}
PRINT_FUNCTION <function_1> [ { function | expr } ... { function | expr } ] 
 [ FILE { <file_path> | <file_id> } ] [ HEADER ] 
 [ MIN <expr_1> <expr_2> ... <expr_k> ] [ MAX <expr_1> <expr_2> ... <expr_k> ] 
 [ STEP <expr_1> <expr_2> ... <expr_k> ] [ NSTEPs <expr_1> <expr_2> ... <expr_k> ] 
 [ FORMAT <print_format> ] [ PHYSICAL_ENTITY <name> ] <vector_1> [ { vector | expr } ... { vector | expr } ]
~~~
:::



Each argument should be either a function or an expression.
The output of this instruction consists of\ $n+k$ columns,
where\ $n$ is the number of arguments of the first function of the list
and\ $k$ is the number of functions and expressions given.
The first\ $n$ columns are the arguments (independent variables) and
the last\ $k$ one has the evaluated functions and expressions.
The columns are separated by a tabulator, which is the format that most
plotting tools understand.
Only function names without arguments are expected.
All functions should have the same number of arguments.
Expressions can involve the arguments of the first function.
If the `FILE` keyword is not provided, default is to write to `stdout`.
If `HEADER` is given, the output is prepended with a single line containing the
names of the arguments and the names of the functions, separated by tabs.
The header starts with a hash\ `#` that usually acts as a comment and is ignored
by most plotting tools.
If there is no explicit range where to evaluate the functions and the first function
is point-wise defined, they are evalauted at the points of definition of the first one.
The range can be explicitly given as a product of\ $n$ ranges\ $[x_{i,\min},x_{i,\max}]$
for $i=1,\dots,n$.
The values $x_{i,\min}$ and $x_{i,\max}$ are given with the `MIN` _and_ `MAX` keywords.
The discretization steps of the ranges are given by either `STEP` that gives\ $\delta x$ _or_
`NSTEPS` that gives the number of steps.
If the first function is not point-wise defined, the ranges are mandatory.

##  PRINT_VECTOR

Print the elements of one or more vectors, one element per line.


::: {.usage}
~~~{.feenox style=feenox}
PRINT_VECTOR 
 [ FILE { <file_path> | <file_id> } ] [ HEADER ] 
 [ FORMAT <print_format> ]
~~~
:::



Each argument should be either a vector or an expression of the integer\ `i`.
If the `FILE` keyword is not provided, default is to write to `stdout`.
If `HEADER` is given, the output is prepended with a single line containing the
names of the arguments and the names of the functions, separated by tabs.
The header starts with a hash\ `#` that usually acts as a comment and is ignored
by most plotting tools.

##  PROBLEM

Sets the problem type that FeenoX has to solve.      


::: {.usage}
~~~{.feenox style=feenox}
PROBLEM [ mechanical | thermal | modal ]
 [ DIMENSIONS <expr> ] 
 [ AXISYMMETRIC { x | y } | [ PROGRESS ]
 [ TRANSIENT | QUASISTATIC]
 [ LINEAR | NON_LINEAR ] [ MESH <identifier> ] 
 [ MODES <expr> ] 
 [ PRECONDITIONER { gamg | mumps | lu | hypre | sor | bjacobi | cholesky | ... } ]
 [ LINEAR_SOLVER { gmres | mumps | bcgs | bicg | richardson | chebyshev | ... } ]
 [ NONLINEAR_SOLVER { newtonls | newtontr | nrichardson | ngmres | qn | ngs | ... } ]
 [ TRANSIENT_SOLVER { bdf | beuler | arkimex | rosw | glee | ... } ]
 [ TIME_ADAPTATION { basic | none | dsp | cfl | glee | ... } ]
 [ EIGEN_SOLVER { krylovschur | lanczos | arnoldi | power | gd | ... } ]
 [ SPECTRAL_TRANSFORMATION { shift | sinvert | cayley | ... } ]
 [ EIGEN_FORMULATION { omega | lambda } ]

~~~
:::




 * `mechanical` (or `elastic`) solves the mechanical elastic problem.
If the mesh is two-dimensional and not `AXISYMMETRIC`, either
`plane_stress` or `plane_strain` has to be set instead.
 * `thermal` (or `heat` ) solves the heat conduction problem.
 * `modal` computes the natural mechanical frequencies and oscillation modes.
 * `modal` computes the natural mechanical frequencies and oscillation modes.

If you want to contribute with anothey problem type, you are welcome!
Check out FeenoX's repository for licensing, programming guides and code of conduct.
The number of spatial dimensions of the problem needs to be given either with the keyword `DIMENSIONS`
or by defining a `MESH` (with an explicit `DIMENSIONS` keyword) before `PROBLEM`.
If the `AXISYMMETRIC` keyword is given, the mesh is expected
to be two-dimensional in the $x$-$y$ plane and the problem
is assumed to be axi-symmetric around the given axis.
If the keyword `PROGRESS` is given, three ASCII lines will show in the terminal the
progress of the ensamble of the stiffness matrix (or matrices),
the solution of the system of equations
and the computation of gradients (stresses).
If the special variable `end_time` is zero, FeenoX solves a static
 problem---although the variable `static_steps` is still honored.
If `end_time` is non-zero, FeenoX solves a transient or quasistatic problem.
This can be controlled by `TRANSIENT` or `QUASISTATIC`.
By default FeenoX tries to detect wheter the computation should be linear or non-linear.
An explicit mode can be set with either `LINEAR` on `NON_LINEAR`.
If there are more than one `MESH`es define, the one over which the problem is to be solved
can be defined by giving the explicit mesh name with `MESH`. By default, the first mesh to be
defined in the input file is the one over which the problem is solved.
The number of modes to be computed when solving eigenvalue problems is given by `MODES`.
The default is DEFAULT_NMODES.
The preconditioner (`PC`), linear (`KSP`), non-linear (`SNES`) and time-stepper (`TS`)
solver types be any of those available in PETSc (first option is the default):

 * List of `PRECONDITIONER`s <http://www.mcs.anl.gov/petsc/petsc-current/docs/manualpages/PC/PCType.html>.
 * List of `LINEAR_SOLVER`s <http://www.mcs.anl.gov/petsc/petsc-current/docs/manualpages/KSP/KSPType.html>.
 * List of `NONLINEAR_SOLVER`s <http://www.mcs.anl.gov/petsc/petsc-current/docs/manualpages/SNES/SNESType.html>.

 * List of `TRANSIENT_SOLVER`s <http://www.mcs.anl.gov/petsc/petsc-current/docs/manualpages/TS/TSType.html>.
 * List of `TIME_ADAPTATION`s <http://www.mcs.anl.gov/petsc/petsc-current/docs/manualpages/TS/TSAdaptType.html>.
 * List of `EIGEN_SOLVER`s <https:
 * List of `SPECTRAL_TRANSFORMATION`s <https:
If the `EIGEN_FORMULATION` is `omega` then $K \phi = \omega^2 M \phi$,
and $M \phi = \lambda K \phi$$ if it is `lambda`.

##  READ

Read data (variables, vectors o matrices) from files or shared-memory segments.


::: {.usage}
~~~{.feenox style=feenox}
[ READ | WRITE ] [ SHM <name> ] [ { ASCII_FILE_PATH | BINARY_FILE_PATH } <file_path> ] [ { ASCII_FILE | BINARY_FILE } <identifier> ] [ IGNORE_NULL ] [ object_1 object_2 ... object_n ]
~~~
:::




##  READ_MESH

Read an unstructured mesh and (optionally) functions of space-time from a file.


::: {.usage}
~~~{.feenox style=feenox}
READ_MESH { <file_path> | <file_id> } [ DIMENSIONS <num_expr> ]
 [ SCALE <expr> ] [ OFFSET <expr_x> <expr_y> <expr_z> ]
 [ INTEGRATION { full | reduced } ]
 [ MAIN ] [ UPDATE_EACH_STEP ]
 [ READ_FIELD <name_in_mesh> AS <function_name> ] [ READ_FIELD ... ] 
 [ READ_FUNCTION <function_name> ] [READ_FUNCTION ...] 

~~~
:::



Either a file identifier (defined previously with a `FILE` keyword) or a file path should be given.
The format is read from the extension, which should be either

 * `.msh`, `.msh2` or `.msh4` [Gmsh ASCII format](http:\//gmsh.info/doc/texinfo/gmsh.html#MSH-file-format), versions 2.2, 4.0 or 4.1
 * `.vtk` [ASCII legacy VTK](https:\//lorensen.github.io/VTKExamples/site/VTKFileFormats/)
 * `.frd` [CalculiX’s FRD ASCII output](https:\//web.mit.edu/calculix_v2.7/CalculiX/cgx_2.7/doc/cgx/node4.html))

Note than only MSH is suitable for defining PDE domains, as it is the only one that provides
physical groups (a.k.a labels) which are needed in order to define materials and boundary conditions.
The other formats are primarily supported to read function data contained in the file
and eventually, to operate over these functions (i.e. take differences with other functions contained
in other files to compare results).
The file path or file id can be used to refer to a particular mesh when reading more than one,
for instance in a `WRITE_MESH` or `INTEGRATE` keyword.
If a file path is given such as `cool_mesh.msh`, it can be later referred to as either
`cool_mesh.msh` or just `cool_mesh`.
The spatial dimensions cab be given with `DIMENSION`.
If material properties are uniform and given with variables,
the number of dimensions are not needed and will be read from the file at runtime.
But if either properties are given by spatial functions or if functions
are to be read from the mesh with `READ_DATA` or `READ_FUNCTION`, then
the number of dimensions ought to be given explicitly because FeenoX needs to know
how many arguments these functions take.
If either `OFFSET` and/or `SCALE` are given, the node locations are first shifted and then scaled by the provided values.
When defining several meshes and solving a PDE problem, the mesh used
as the PDE domain is the one marked with `MAIN`.
If none of the meshes is explicitly marked as main, the first one is used.
If `UPDATE_EACH_STEP` is given, then the mesh data is re-read from the file at
each time step. Default is to read the mesh once, except if the file path changes with time.
For each `READ_FIELD` keyword, a point-wise defined function of space named `<function_name>`
is defined and filled with the scalar data named `<name_in_mesh>` contained in the mesh file.
The `READ_FUNCTION` keyword is a shortcut when the scalar name and the to-be-defined function are the same.
If no mesh is marked as `MAIN`, the first one is the main one.

##  SEMAPHORE

Perform either a wait or a post operation on a named shared semaphore.


::: {.usage}
~~~{.feenox style=feenox}
[ SEMAPHORE | SEM ] <name> { WAIT | POST }
~~~
:::




##  SHELL

Execute a shell command.


::: {.usage}
~~~{.feenox style=feenox}
SHELL <print_format> [ expr_1 expr_2 ... expr_n ]
~~~
:::




##  SOLVE

Solve a non-linear system of\ $n$ equations with\ $n$ unknowns.


::: {.usage}
~~~{.feenox style=feenox}
SOLVE <n> UNKNOWNS <var_1> <var_2> ... <var_n> RESIDUALS <expr_1> <expr_2> ... <expr_n> ] GUESS <expr_1> <expr_2> ... <expr_n> ] [ METHOD { dnewton | hybrid | hybrids | broyden } ] [ EPSABS <expr> ] [ EPSREL <expr> ] [ MAX_ITER <expr> ] [ VERBOSE ]
~~~
:::




##  SOLVE_PROBLEM

Explicitly solve the PDE problem.


::: {.usage}
~~~{.feenox style=feenox}
SOLVE_PROBLEM
~~~
:::



Whenever the instruction `SOLVE_PROBLEM` is executed,
FeenoX solves the PDE problem.
For static problems, that means solving the equations
and filling in the result functions.
For transient or quasisstatic problems, that means
advancing one time step.

##  SORT_VECTOR

Sort the elements of a vector, optionally making the same rearrangement in another vector.


::: {.usage}
~~~{.feenox style=feenox}
SORT_VECTOR <vector> [ ASCENDING | DESCENDING ] [ <other_vector> ]
~~~
:::



This instruction sorts the elements of `<vector>` into either ascending or descending numerical order.
If `<other_vector>` is given, the same rearrangement is made on it.
Default is ascending order.

##  TIME_PATH

Force time-dependent problems to pass through specific instants of time.


::: {.usage}
~~~{.feenox style=feenox}
TIME_PATH <expr_1> [ <expr_2> [ ... <expr_n> ] ]
~~~
:::



The time step `dt` will be reduced whenever the distance between
the current time `t` and the next expression in the list is greater
than `dt` so as to force `t` to coincide with the expressions given.
The list of expresssions should evaluate to a sorted list of values for all times.

##  VAR

Explicitly define one or more scalar variables.


::: {.usage}
~~~{.feenox style=feenox}
VAR <name_1> [ <name_2> ] ... [ <name_n> ]
~~~
:::



When implicit definition is allowed (see [`IMPLICIT`]), scalar variables
need not to be defined before being used if from the context FeenoX can tell
that an scalar variable is needed. For instance, when defining a function like
`f(x) = x^2` it is not needed to declare `x` explictly as a scalar variable.
But if one wants to define a function like `g(x) = integral(f(x'), x', 0, x)` then
the variable `x'` needs to be explicitly defined as `VAR x'` before the integral.

##  VECTOR

Define a vector.


::: {.usage}
~~~{.feenox style=feenox}
VECTOR <name> SIZE <expr> [ FUNCTION_DATA <function> ] [ DATA <expr_1> <expr_2> ... <expr_n> |
~~~
:::



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

Write data (variables, vectors o matrices) to files or shared-memory segments.
See the `READ` keyword for usage details.





##  WRITE_MESH

Write a mesh and functions of space-time to a file for post-processing.


::: {.usage}
~~~{.feenox style=feenox}
WRITE_MESH
~~~
:::





