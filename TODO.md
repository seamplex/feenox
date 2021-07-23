# General things to TODO

 * https://joss.theoj.org/
 * FIT to mesh-based functions
 * FIT taking into account uncertainties
 * extended integration (as in reduced, full, extended)
 * check typos in BC names

 
## Tests
 
 * cell-based mesh writes and integrations
 * FIT ranges

## Wasora keywords

 * SEMAPHORE
 * READ / WRITE
 * PRINT_VECTOR
 * SOLVE
 * M4 (MUSTACHE? MACRO?)
 * SHELL
 * HISTORY
 * PARAMETRIC? - "do one thing well and move these guys to an outer-level shell"
 * MINIMIZE?
 * FIT? maybe we can keep this for fitting DAEs
 
 * explain that 
   - nouns -> definitions
   - verbs -> instructions
 
## Wasora stuff

 * non-topology pointwise functions
   - derivative of a column with respect to another one as a UNIX filter
 * vector & matrix assignements & DAEs
 * debug mode, line-by-line
 * trap signit & sigterm

## API

 * Python?
 * Julia?
 * come up with an example in C

## Nice to have

 * logaritmic ranges for PRINT_FUNCTION
 * default separator after TEXT is space, after numerical is tab
 * BLAS
   - BLAS a * b = s   # scalar dot
   - BLAS a x b = c   # vector cross
   - BLAS A * b = c   # matrix-vector
   - BLAS a + b = c   # vector
   - BLAS A + B = C   # matrix
   - BLAS a - b = c
   - BLAS A - B = C
   
 
## Command-line arguments

 * list symbols with "-l" (is there any clash with petsc?)
 * $1 = ${1}
 
## Error handling 
 
 * gsl error handling
 * feenox_nan_error()
 * feenox_gsl_handler()

## Postprocessing output

 * symmetric & full tensors
 * vtu format
 * frd? (to be able to use ccx's post processor... is it worth it?)
 * binary msh and vtk
 * support CGNS formats
 * problem-aware WRITE_POST 
   - FULL | DISPLACEMENTS | STRESS | STRAIN | VON_MISES | TRESCA | PRINCIPAL
   - FULL | TEMPERATURE | HEAT_FLUX
   - FULL | MODES
   - FULL | FLUX(ES) | CURRENT(S)
 
## Extensions
 
 * mustache instead of m4
 * gmsh as a library? maybe not, just import both gmsh and feenox in Python and use them together

 
# PDEs

 * check when the matrices do not need to be rebuilt
 * Laplace/Poisson/Helmholtz as a template (fem or fem+fvm?)
 
 
## Heat

 * parallel runs
 * convection with a separate matrix? think!
 * jacobians for non-linear problems
 * explain that radiation is better off as an expression
 * same for total heat sources/bcs
 * reactions (i.e. total power through faces)
 * heat flux as secondary field
 * orthotropic conductivity
 * total power sources and BCs (can't we get away with the volumes/areas as variables?)
 
## Mechanical

 * orthotropic linear
 * non-linear
 * stresses: count negative jacobians and worst jacobians
 * stress linearization (do we need a per-problem parser?)

## Modal

 * partially-unconstrained DOFs
 
## Neutron

 * diffusion/transport
 * FEM/FVM

