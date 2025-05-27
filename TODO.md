# ldef

 * Put `J`, `J23`, `E`, `C`, `B` in the material's context
   - `mechanical.C` = stress-strain tensor
   - `material.ctx->C` = right cauchy-green tensor
 * virtual methods
 * stresses in output
   - strain energy
 * many volumes
   - same models
   - different models
 * compare with other solvers
   - mofem
   - fenicsx
   - ccx
   - code aster
 * check $\det(\mathbf{F}) \neq 0$
 * use TS with quasi-static
 * examples

# General things to do

 * windows build (against my will)
 * `PROGRESS` `ascii_art` `percentage` `norm` 
   - build, KSP, SNES
 * implicit call to `SOLVE_PROBLEM`?
 * for/while loops?
 * use `DMPlex`?
    - to read non-gmsh meshes?
    - to partition meshes on the fly?
    - allocate matrices?
    - dump states?
    - choose to use it or not
 * `FIT` to mesh-based functions
 * `FIT` taking into account uncertainties
 * extended integration (as in reduced, full, extended)
 * make GSL optional
   - rewrite BLAS using ad-hoc routines
     - profile GSL BLAS vs. manual implementation vs. OpenBLAS
     - eigen in C++ (optional)
   - define `fee_matrix` as `gsl_matrix` but allow to change the implementation
   - wrap all GSL calls inside `#ifdef`
   - use a large chunk of contiguous memory in the heap to store shape functions, gradients, etc
 * optimize MPI usage
   - variables `mpi_rank`, `mpi_size`,  `memory_total` y `memory_local(rank)`
 * V&V
   - fully document tests
   - section for MMS
 
## Tests

 * use the same documentation strategy from `doc/examples` in `tests`
 * write a `README` and an explanation of each test
 * cell-based mesh writes and integrations
 * `FIT` ranges
 * more MPI tests

## Optimizations

 * if the elements are straight all the jacobians are the same and there is no need to sweep over gauss points
 * make two lists of elements, one for bulk and one for BCs and loop over those
 * re-think `fem.c` to store per-gauss point data in a cache-friendly way
 * remove branches (and replace with what?)
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
 * use `MatSetValuesBlocked()`
   
## Wasora keywords

 * `SEMAPHORE` + `SHARED_MEMORY`
 * `READ` / `WRITE`
 * `M4` (MUSTACHE? MACRO?)
 * `SHELL`
 * `HISTORY`
 * `PRINT_FUNCTION PHYSICAL_ENTITY` -> for dumping and reading back BCs
 * `CALL` user routines (check hash of feenox.h)
 
## Wasora-like stuff

 * vector & matrix assignments & DAEs
 * debug mode, line-by-line
   - run `feenox` with an option like `-d` or `--debug` and then present a gdb-like interface with print, and step by step advances
 * trap and handle signit & sigterm

## API

 * Python?
 * Julia?
 * come up with an example in C

## Nice to have

 * ask how to set `OMP_NUM_THREADS=1`
 * logarithmic ranges for `PRINT_FUNCTION`
 * default separator after `TEXT` should be space, after numerical should be tab
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
   - xdmf? (needs DMView() and VecView())
   - frd? (to be able to use ccx's post processor... is it worth it?)
   - binary msh and vtu
   - msh v4.1
   - cgns?
   - med?
 
 * full tensors
 
# Meshes

 * expose the vectors with the node coordinates
 
# PDEs

 * check when the matrices do not need to be rebuilt -- use constant flag
 * remove the need of needing an explicit `SOLVE_PROBLEM`
   - detect which variables/functions are PDE outputs and call `solve_problem()` the first time they are needed
 * benchmark MPI (read `.msh` with partitioning info first)
 * glue (mortar)
 * investigate `-mpi_linear_solver_server`
 * direct elimination for multi-freedom BCs
 * third-medium contact?

 
## Laplace/Poisson/Helmholtz

 * poisson f = 1, eta = 0
 * helmholtz f = 1, eta = 1
 * investigate `dsyrk()` instead of `dgemmv()` https://stackoverflow.com/questions/47013581/blas-matrix-by-matrix-transpose-multiply
 
## Heat

 * improve parallel runs
 * convection with a separate matrix? think!
 * explain that radiation is better off as an expression
 * same for total heat sources/bcs
 * orthotropic conductivity
 
## Mechanical

 * be able to use $\lambda$ & $\mu$ for elasticity
 * plasticity
 * stresses: count negative jacobians and worst jacobians
 * strain energy density
 * compare non-linear solutions against other solvers
 * quasi-static: use PETSc's TS
   - if a simple SNES does not converge, convert to TS
   - if $\det{F} \leq 0$, convert to TS
   - `PROBLEM` `AUTO_LOAD_STEPS`
 *   
   

## Modal

 * partially-unconstrained DOFs in arbitrary directions
 
## Neutron

 * current in BCs
 * FVM?
 * allow per-material fission spectrum (it has to be uniform and normalized)


 
# V & V

 * MMS

# PDEs

 - CFD?
 - LBM?
 - Electromagnetism?
 - FVM

