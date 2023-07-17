# General things to do

 * implicit call to `SOLVE_PROBLEM`?
 * use `MatPreallocator` to efficiently pre-allocate the matrices?
    - check v3.19 and see if it works well
    - add an option to choose to allocate or not
 * use `DMPlex`?
    - allocate matrices?
    - write vtus?
    - dump states?
    - choose to use it or not
 * use `spack`? sow?
 * https://joss.theoj.org/
 * FIT to mesh-based functions
 * FIT taking into account uncertainties
 * extended integration (as in reduced, full, extended)
 * make GSL optional
   - rewrite BLAS using ad-hoc routines
   - wrap all GSL calls inside `#ifdef`
   - use a large chunk of contiguous memory in the heap to store shape functions, gradients, etc
 * MPI
 
## Tests

 * move all `doc/examples` to `tests`
 * write a `README` and an explanation of each test
 * make an index of the keywords used in each test and link them from the doc as examples
 * cell-based mesh writes and integrations
 * `FIT` ranges
 * MPI

## Optimizations

 * if the elements are straight all the jacobians are the same and there is no need to sweep over gauss points
 * make two lists of elements, one for bulk and one for BCs and loop over those
 * rewrite `fem.c` to store per-gauss point data in a cache-friendly way
 * remove branches
 * use ad-hoc matrices instead of `gsl_matrix`?
   - have a contiguous array of memory that stores all the per-element matrices in a row-major order
   - access them using macros `dhdx(row,col)`
   - mind SSE!
   - https://github.com/niswegmann/small-matrix-inverse
 * have compile-time macros that will optimize for
   - speed
   - memory
   - something else
 * create FeenoX flavors with compile-time 
   - problem type (so we can avoid function pointers)
   - problem dimension (so we can hardcode sizes)
   - full or axi-symmetry
   - scalar size (float or double)
   - all elements are of the same type
   
   
## Wasora keywords

 * `SEMAPHORE` + `SHARED_MEMORY`
 * `READ` / `WRITE`
 * `M4` (MUSTACHE? MACRO?)
 * `SHELL`
 * `HISTORY`
 * `PRINT_FUNCTION PHYSICAL_ENTITY` -> for dumping and reading back BCs
 * `CALL` user routines (check hash of feenox.h)
 
## Wasora-like stuff

 * vector & matrix assignements & DAEs
 * debug mode, line-by-line
   - run `feenox` with an option like `-d` or `--debug` and then present a gdb-like interface with print, and step by step advances
 * trap and handle signit & sigterm

## API

 * Python?
 * Julia?
 * come up with an example in C

## Nice to have

 * add a keyword and command-line argument to set `OMP_NUM_THREADS`
 * logaritmic ranges for `PRINT_FUNCTION`
 * default separator after `TEXT` should be space, after numerical should be tab
 * `PRINTF` instruction
 * `PRINT_FUNCTION` with `%f` in between arguments (like `PRINT_VECTOR`)
 * `BLAS` 
   - `BLAS a * b = s   # scalar dot`
   - `BLAS a x b = c   # vector cross`
   - `BLAS A * b = c   # matrix-vector`
   - `BLAS a + b = c   # vector`
   - `BLAS A + B = C   # matrix`
   - `BLAS a - b = c`
   - `BLAS A - B = C`
   
 
## Command-line arguments

 * list symbols with `-l`
 
## Error handling 
 
 * GSL error handling
 * `feenox_nan_error()`
 * `feenox_gsl_handler()`

## Postprocessing output

 * formats
   - vtu?
   - xdmf? (needs DMView() and VecView())
   - frd? (to be able to use ccx's post processor... is it worth it?)
   - binary msh and vtk
   - msh v4.1
   - cgns?
   - med?
 
 * full tensors
 
 
# PDEs

 * check when the matrices do not need to be rebuilt -- use constant flag
 * allow different material model for each volume!
   - so far we have a global material model, i.e. all the materials have to be orthotropic
   - this is wrong!
 * remove the need of needing an explicit `SOLVE_PROBLEM`
   - detect which variables/functions are PDE outputs and call `solve_problem()` the first time they are needed
 * benchmark MPI (read `.msh` with partitioning info first)
 * openmp to build matrices and compute stresses? not sure, should be mpi on a partitioned mesh
 * glue (mortar)
 * investigate `-mpi_linear_solver_server`
 * direct elimination for multi-freedom BCs
 
## Laplace/Poisson/Helmholtz

 * poisson f = 1, eta = 0
 * helmholtz f = 1, eta = 1
 * see 2D example in Gmsh list
 
## Heat

 * parallel runs
 * convection with a separate matrix? think!
 * explain that radiation is better off as an expression
 * same for total heat sources/bcs
 * orthotropic conductivity
 
## Mechanical

 * non-linear
 * stresses: count negative jacobians and worst jacobians
 * strain energy density

## Modal

 * partially-unconstrained DOFs in arbitrary directions
 
## Neutron

 * add currents to the outputs? J1x J1y J1x?
 * current in BCs
 * FVM?
 * benchmarks/tests
 * Why is it generally a bad idea to use EPS_SMALLEST_MAGNITUDE? where did I get this?
 
# V & V

 * MMS

# PDEs

 - CFD?
 - LBM?
 - Electromagnetism?
 - FVM

