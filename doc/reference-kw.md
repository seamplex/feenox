#  `ABORT`

Catastrophically abort the execution and quit FeenoX.


::: {.usage}
~~~feenox
ABORT  
~~~
:::



Whenever the instruction `ABORT` is executed, FeenoX quits with a non-zero error leve.
It does not close files nor unlock shared memory objects.
The objective of this instruction is to either debug complex input files
by using only parts of them or to conditionally abort the execution using `IF` clauses.

#  `ALIAS`

Define a scalar alias of an already-defined indentifier.


::: {.usage}
~~~feenox
ALIAS { <new_var_name> IS <existing_object> | <existing_object> AS <new_name> }  
~~~
:::



The existing object can be a variable, a vector element or a matrix element.
In the first case, the name of the variable should be given as the existing object.
In the second case, to alias the second element of vector `v` to the new name `new`, `v(2)` should be given as the existing object.
In the third case, to alias second element (2,3) of matrix `M` to the new name `new`, `M(2,3)` should be given as the existing object.

#  `CLOSE`

Explicitly close a file after input/output.


::: {.usage}
~~~feenox
CLOSE <name>  
~~~
:::



The given `<name>` can be either a fixed-string path or an already-defined `FILE`.

#  `DEFAULT_ARGUMENT_VALUE`

Give a default value for an optional commandline argument.


::: {.usage}
~~~feenox
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

#  `FILE`

Define a file with a particularly formatted name to be used either as input or as output.


::: {.usage}
~~~feenox
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

#  `FIT`

Find parameters to fit an analytical function to a pointwise-defined function.


::: {.usage}
~~~feenox
FIT <function_to_be_fitted>  TO <function_with_data> VIA <var_1> <var_2> ... <var_n>
 [ GRADIENT <expr_1> <expr_2> ... <expr_n> ]
 [ RANGE_MIN <expr_1> <expr_2> ... <expr_j> ]
 [ RANGE_MAX <expr_1> <expr_2> ... <expr_n> ]
 [ TOL_REL <expr> ] [ TOL_ABS <expr> ] [ MAX_ITER <expr> ]
 [ VERBOSE ]  
~~~
:::



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

#  `FUNCTION`

Define a function of one or more variables.


::: {.usage}
~~~feenox
FUNCTION <function_name>(<var_1>[,var2,...,var_n]) { = <expr> | FILE { <file_path> | <file_id> } | VECTORS <vector_1> <vector_2> ... <vector_n> <vector_data> | DATA <num_1> <num_2> ... <num_N> } [ INTERPOLATION { linear | polynomial | spline | spline_periodic | akima | akima_periodic | steffen | nearest | shepard | shepard_kd | bilinear } ] [COLUMNS <expr_1> <expr_2> ... <expr_n> <expr_n+1> ] [ INTERPOLATION_THRESHOLD <expr> ] [ SHEPARD_RADIUS <expr> ] [ SHEPARD_EXPONENT <expr> ]  
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

For $n>1$, if the euclidean distance between the arguments and the definition points is smaller than `INTERPOLATION_THRESHOLD`, the definition point is returned and no interpolation is performed.
Default value is square root of `DEFAULT_MULTIDIM_INTERPOLATION_THRESHOLD`.
The initial radius of points to take into account in `shepard_kd` is given by `SHEPARD_RADIUS`. If no points are found, the radius is double until at least one definition point is found.
The radius is doubled until at least one point is found.
Default is `DEFAULT_SHEPARD_RADIUS`.
The exponent of the `shepard` method is given by `SHEPARD_EXPONENT`.
Default is `DEFAULT_SHEPARD_EXPONENT`.

#  `IF`

Execute a set of instructions if a condition is met.


::: {.usage}
~~~feenox
IF expr 
  <block_of_instructions_if_expr_is_true> 
 [ ELSE  
  <block_of_instructions_if_expr_is_false> ] 
 ENDIF  
~~~
:::




#  `IMPLICIT`

Define whether implicit definition of variables is allowed or not.


::: {.usage}
~~~feenox
IMPLICIT { NONE | ALLOWED }  
~~~
:::



By default, FeenoX allows variables (but not vectors nor matrices) to be
implicitly declared. To avoid introducing errors due to typos, explicit
declaration of variables can be forced by giving `IMPLICIT NONE`.
Whether implicit declaration is allowed or explicit declaration is required
depends on the last `IMPLICIT` keyword given, which by default is `ALLOWED`.

#  `INCLUDE`

Include another FeenoX input file.


::: {.usage}
~~~feenox
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

#  `MATRIX`

Define a matrix.


::: {.usage}
~~~feenox
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

#  `OPEN`

Explicitly open a file for input/output.


::: {.usage}
~~~feenox
OPEN <name> [ MODE <fopen_mode> ]  
~~~
:::



The given `<name>` can be either a fixed-string path or an already-defined `FILE`.
The mode is only taken into account if the file is not already defined.
Default is write `w`.

#  `PHYSICAL_GROUP_pde`



::: {.usage}
~~~feenox
<name> [ MESH <name> ] [ DIMENSION <expr> ] [ ID <expr> ]
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

#  `PRINT`

Write plain-text and/or formatted data to the standard output or into an output file.


::: {.usage}
~~~feenox
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

#  `PRINT_FUNCTION`

Print one or more functions as a table of values of dependent and independent variables.


::: {.usage}
~~~feenox
PRINT_FUNCTION <function_1> [ { function | expr } ... { function | expr } ] 
 [ FILE { <file_path> | <file_id> } ] [ HEADER ] 
 [ MIN <expr_1> <expr_2> ... <expr_k> ] [ MAX <expr_1> <expr_2> ... <expr_k> ] 
 [ STEP <expr_1> <expr_2> ... <expr_k> ] [ NSTEPs <expr_1> <expr_2> ... <expr_k> ] 
 [ FORMAT <print_format> ] <vector_1> [ { vector | expr } ... { vector | expr } ]  
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

#  `PRINT_VECTOR`

Print the elements of one or more vectors, one element per line.


::: {.usage}
~~~feenox
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

#  `READ_MESH_pde`



::: {.usage}
~~~feenox
[ DIMENSIONS <num_expr> ]
 [ SCALE <expr> ] [ OFFSET <expr_x> <expr_y> <expr_z> ]
 [ INTEGRATION { full | reduced } ]
 [ MAIN ] [ UPDATE_EACH_STEP ]
 [ READ_FIELD <name_in_mesh> AS <function_name> ] [ READ_FIELD ... ] 
 [ READ_FUNCTION <function_name> ] [READ_FUNCTION ...] 
  
~~~
:::



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

#  `SORT_VECTOR`

Sort the elements of a vector, optionally making the same rearrangement in another vector.


::: {.usage}
~~~feenox
SORT_VECTOR <vector> [ ASCENDING | DESCENDING ] [ <other_vector> ]  
~~~
:::



This instruction sorts the elements of `<vector>` into either ascending or descending numerical order.
If `<other_vector>` is given, the same rearrangement is made on it.
Default is ascending order.

#  `VAR`

Explicitly define one or more scalar variables.


::: {.usage}
~~~feenox
VAR <name_1> [ <name_2> ] ... [ <name_n> ]  
~~~
:::



When implicit definition is allowed (see [`IMPLICIT`]), scalar variables
need not to be defined before being used if from the context FeenoX can tell
that an scalar variable is needed. For instance, when defining a function like
`f(x) = x^2` it is not needed to declare `x` explictly as a scalar variable.
But if one wants to define a function like `g(x) = integral(f(x'), x', 0, x)` then
the variable `x'` needs to be explicitly defined as `VAR x'` before the integral.

#  `VECTOR`

Define a vector.


::: {.usage}
~~~feenox
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

#  `WRITE_MESH`



::: {.usage}
~~~feenox
[ FILE_FORMAT { gmsh | vtk } ] CELL ]  
~~~
:::




#  `WRITE_MESH_pde`



::: {.usage}
~~~feenox
{ <file_path> | <file_id> } [ MESH <mesh_identifier> ] [ NO_MESH ] [ NO_PHYSICAL_NAMES ]  [ NODE | [ SCALAR_FORMAT <printf_specification>] [ VECTOR <field_x> <field_y> <field_z> ] [...] [ <field_1> ] [ <field_2> ] ...  
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


