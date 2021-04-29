# General things to TODO

## Binaries

 * automate the generation of binary tarballs
    - --with-fc=0 --with-cxx=0 --with-c2html=0

## Wasora keywords

 * SEMAPHORE
 * READ / WRITE
 * PRINT_VECTOR
 * SOLVE
 * M4
 * SHELL
 * HISTORY
 * PARAMETRIC
 * FIT
 * MINIMIZE
 
## Wasora stuff

 * non-topology pointwise functions
   - derivative of a column with respect to another one as a UNIX filter
 * vector & matrix assignements & DAEs
 * debug mode, line-by-line

## API

 * Python?
 * Julia?
 

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
 
## Extensions
 
 * mustache
 * gmsh as a library?

# PDEs

 * check when the matrices do not need to be rebuilt
 * keep K_original and K_with_bc or whatever?
 
## Heat

 * convection (with a separate matrix?)
 * reactions (i.e. total power through faces)
 * heat flux as secondary field
 * orthotropic conductivity
 * total power sources and BCs (can't we get away with the volumes/areas as variables?)
 * transient
 * non-linear jacobians
 
## Mechanical

 * everything
 * stresses

## Modal

 * everything
 * deflate null space without BCs
 
## Neutron

 * diffusion
 * transport
