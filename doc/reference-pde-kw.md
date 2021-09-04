#  `BC`

Define a boundary condition to be applied to faces, edges and/or vertices.


~~~feenox
BC <name> [ MESH <name> ] [ PHYSICAL_GROUP <name_1>  PHYSICAL_GROUP <name_2> ... ] [ <bc_data1> <bc_data2> ... ]  
~~~



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

#  `DUMP`

Dump raw PETSc objects used to solve PDEs into files.


~~~feenox
DUMP [ FORMAT { binary | ascii | octave } ] [ K |   K_bc |   b |   b_bc |   M |   M_bc |  
~~~




#  `FIND_EXTREMA`

Find and/or compute the absolute extrema of a function or expression over a mesh (or a subset of it).


~~~feenox
FIND_EXTREMA { <expression> | <function> } [ OVER <physical_group> ] [ MESH <mesh_identifier> ] [ NODES | CELLS | GAUSS ]
 [ MIN <variable> ] [ MAX <variable> ] [ X_MIN <variable> ] [ X_MAX <variable> ] [ Y_MIN <variable> ] [ Y_MAX <variable> ] [ Z_MIN <variable> ] [ Z_MAX <variable> ] [ I_MIN <variable> ] [ I_MAX <variable> ]  
~~~



Either an expression or a function of space $x$, $y$ and/or $z$ should be given.
By default the search is performed over the highest-dimensional elements of the mesh,
i.e. over the whole volume, area or length for three, two and one-dimensional meshes, respectively.
If the search is to be carried out over just a physical group, it has to be given in `OVER`.
If there are more than one mesh defined, an explicit one has to be given with `MESH`.
If neither `NODES`, `CELLS` or `GAUSS` is given then the search is
performed over the three of them.
With `NODES` only the function or expression is evalauted at the mesh nodes.
With `CELLS` only the function or expression is evalauted at the element centers.
With `GAUSS` only the function or expression is evalauted at the Gauss points.
The value of the absolute minimum (maximum) is stored in the variable indicated by `MIN` (`MAX`).
If the variable does not exist, it is created.
The value of the $x$-$y$-$z$\ coordinate of the absolute minimum (maximum)
is stored in the variable indicated by `X_MIN`-`Y_MIN`-`Z_MIN` (`X_MAX`-`Y_MAX`-`Z_MAX`).
If the variable does not exist, it is created.
The index (either node or cell) where the absolute minimum (maximum) is found
is stored in the variable indicated by `I_MIN` (`I_MAX`).

#  `INTEGRATE`

Spatially integrate a function or expression over a mesh (or a subset of it).


~~~feenox
INTEGRATE { <expression> | <function> } [ OVER <physical_group> ] [ MESH <mesh_identifier> ] [ NODES | CELLS ]
 RESULT <variable>
  
~~~



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

#  `MATERIAL`

Define a material its and properties to be used in volumes.


~~~feenox
MATERIAL <name> [ MESH <name> ] [ PHYSICAL_GROUP <name_1>  [ PHYSICAL_GROUP <name_2> [ ... ] ] ] [ <property_name_1>=<expr_1> [ <property_name_2>=<expr_2> [ ... ] ] ]  
~~~



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

#  `PHASE_SPACE`



~~~feenox
PHASE_SPACE <vars> ... <vectors> ... <matrices> ...   
~~~




#  `PHYSICAL_GROUP`

Explicitly defines a physical group of elements on a mesh.


~~~feenox
PHYSICAL_GROUP <name> [ MESH <name> ] [ DIMENSION <expr> ] [ ID <expr> ]
 [ MATERIAL <name> | | BC <name> [ BC ... ] ]
  
~~~



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

#  `PROBLEM`

Ask FeenoX to solve a partial-differential equation problem.


~~~feenox
PROBLEM [ mechanical | thermal | modal | neutron_diffusion ]
 
 [ 1D |   2D |   3D |   DIMENSIONS <expr> ] [ MESH <identifier> ] 
 
[ AXISYMMETRIC { x | y } | [ PROGRESS ]
 [ TRANSIENT | QUASISTATIC]
 [ LINEAR | NON_LINEAR ] [ MODES <expr> ] 
 [ PRECONDITIONER { gamg | mumps | lu | hypre | sor | bjacobi | cholesky | ... } ]
 [ LINEAR_SOLVER { gmres | mumps | bcgs | bicg | richardson | chebyshev | ... } ]
 [ NONLINEAR_SOLVER { newtonls | newtontr | nrichardson | ngmres | qn | ngs | ... } ]
 [ TRANSIENT_SOLVER { bdf | beuler | arkimex | rosw | glee | ... } ]
 [ TIME_ADAPTATION { basic | none | dsp | cfl | glee | ... } ]
 [ EIGEN_SOLVER { krylovschur | lanczos | arnoldi | power | gd | ... } ]
 [ SPECTRAL_TRANSFORMATION { shift | sinvert | cayley | ... } ]
 [ EIGEN_FORMULATION { omega | lambda } ]
  
~~~




 * `laplace` (or `poisson`) solves the Laplace (or Poisson) equation.
 * `mechanical` (or `elastic`) solves the mechanical elastic problem.
If the mesh is two-dimensional, and not `AXISYMMETRIC`, either
`plane_stress` or `plane_strain` has to be set instead.
 * `thermal` (or `heat` ) solves the heat conduction problem.
 * `modal` computes the natural mechanical frequencies and oscillation modes.
 * `neutron_diffusion` multi-group core-level neutron diffusion with a FEM formulation

If you are a programmer and want to contribute with another problem type, please do so!
Check out [FeenoX repository](https:
for licensing information, programming guides and code of conduct.

The number of spatial dimensions of the problem needs to be given either
as `1d`, `2d`, `3d` or with the keyword `DIMENSIONS`.
Alternatively, one can define a `MESH` with an explicit `DIMENSIONS` keyword before `PROBLEM`.
If there are more than one `MESH`es define, the one over which the problem is to be solved
can be defined by giving the explicit mesh name with `MESH`. By default, the first mesh to be
defined in the input file is the one over which the problem is solved.
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
The number of modes to be computed when solving eigenvalue problems is given by `MODES`.
The default value is problem dependent.
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

#  `READ_MESH`

Read an unstructured mesh and (optionally) functions of space-time from a file.


~~~feenox
READ_MESH { <file_path> | <file_id> } [ DIMENSIONS <num_expr> ]
 [ SCALE <expr> ] [ OFFSET <expr_x> <expr_y> <expr_z> ]
 [ INTEGRATION { full | reduced } ]
 [ MAIN ] [ UPDATE_EACH_STEP ]
 [ READ_FIELD <name_in_mesh> AS <function_name> ] [ READ_FIELD ... ] 
 [ READ_FUNCTION <function_name> ] [READ_FUNCTION ...] 
  
~~~



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

#  `SOLVE_PROBLEM`

Explicitly solve the PDE problem.


~~~feenox
SOLVE_PROBLEM  
~~~



Whenever the instruction `SOLVE_PROBLEM` is executed,
FeenoX solves the PDE problem.
For static problems, that means solving the equations
and filling in the result functions.
For transient or quasisstatic problems, that means
advancing one time step.

#  `WRITE_MESH`

Write a mesh and functions of space-time to a file for post-processing.


~~~feenox
WRITE_MESH { <file_path> | <file_id> } [ MESH <mesh_identifier> ] [ NO_MESH ] [ FILE_FORMAT { gmsh | vtk } ] [ NO_PHYSICAL_NAMES ]  [ NODE | [ SCALAR_FORMAT <printf_specification>] [ VECTOR <field_x> <field_y> <field_z> ] [...] [ <field_1> ] [ <field_2> ] ...  
~~~



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


