# Current (under development)

 * Neo-hookean material model
 * Saint Venant-Kirchoff material model for ldef
 * large deformation formulation for `mechanical`
 * ...

# v1.1 (Apr 2025)

 * misc fixes for Debian packaging
 * steady-state writer in VTU `.vtu` format
 * skip some tests in big-endian architectures
 * `READ_MESH` can read CalculiX's `.frd` output files
 * `utils/fee2ccx` converter from FeenoX's `.fee` to CalculiX's `.fee`
 * links to examples and tests in HTML manual
 * cleanups to create `debian` directory in Salsa (see <https://salsa.debian.org/jtheler/feenox>)
 * distribution binaries are dynamic instead of static
 * `BC` type `adiabatic` in `thermal`
 * fixed `TIME_PATH` with PETSc and SUNDIALS
 * improved vector initialization syntax
 * remove compilation date to allow reproducible builds
 * BCs as arrays instead of linked lists
 * option `--ast`
 * `BC GROUPS` to select many labels at once
 * cleanups to create Debian `dsc`
 * use `config.h` instead of `version.h`
 * compute perimeters using second-order precision
 * bump GSL to v2.8
 * `INTEGRATE` expressions of `nx`, `ny` and `nz`
 * create `_cog` variables for points as well
 * fix for evaluation of derivatives at arbitrary locations
 * paper in FOSS: <https://doi.org/10.21105/joss.05846>
 
# v1.0 (Mar 2024)

 * option `--check`
 * `README` for engineers, hackers and academics
 * thermal tutorial
 * thermal radiation tests
 * two-dimensional quasi-random number sequences
 * non-zero Neumann BCs in neutron diffusion
 * `LABEL` in now an alias for `PHYSICAL_GROUP` in `MATERIAL`
 * explicit fission spectrum
 * `EIGEN_DIRICHLET_ZERO` keyword
 * special variable `mumps_ictnl_14` to control MUMPS' over-relaxation factor
 * improved MPI execution
 * verification with MMS 
 * `PRINTF` and `PRINTF_ALL` instructions
 * improved memory handling when building elemental objects
 * `PREALLOCATE` and `ALLOW_NEW_NONZEROS` options
 * choose either to fail or allow missed `BC`s
 * updated internal naming of FEM matrices to match the doc
 * dump element drawings, node ordering and shape functions with `--elements_info`
 * cleaned up source distributions
 * CI using Github actions
 * Los Alamos criticallity benchmarks in tests
 * support for reading VTK v5 files
 * `WRITE_RESULTS` keyword
 * split examples by problem type
 * YAML blocks in input file for metadata are allowed (so far they are ignored by FeenoX)
 * `$0` (or `${0}` or `$(1)`) expands to the base name of the main input, i.e. without the `.fee` extension
 * bracketed arguments so ${1}1$(1) is a valid string
 * multi-group neutron transport with discrete ordinates ($S_N$ method)
 * mimicked-nodes BCs
 * read mesh fields from VTK files

# v0.3 (Feb 2023)

 * unofficial Debian/Ubuntu packages
 * added jacobian for temperature-dependent conductivity
 * updated LE10 benchmark
 * read time-dependent functions in `.msh` files
 * detect/handle hanging nodes
 * muti-dof BCs using expressions for mechanical 0=expression of space & displacements
 * volumetric forces in mechanical
 * stress linearization according to ASME VIII Div\ 2 Sec\ 5.
 * tangential and radial symmetry as multi-dof BCs using the penalty method
 * solve (small) systems of non-linear equations given as expressions
 * functions over meshes using vectors
 * argument `--pdes` lists the available PDEs FeenoX can solve
 * auto-detect PDE directories: if one adds or deletes one directory from `src/pdes` FeenoX will compile anyway.
 * faster direct inversion of 3x3 matrices using code from PETSc
 * separated initial conditions from initial guess
 

# v0.2 (Jan 2022)

 * orthotropic elasticity
 * orthotropic thermal expansion
 * compute reactions
 * virtual methods (function pointers) for material models
 * plane stress and strain
 * neutron diffusion problem
 * free-free modal analysis
 * multi-dimensional interpolation using Shepard's method for point-wise functions
 * traction BC in mechanical
 * 3D thermal MMS with space-dependent conductivity
 
# v0.1 (Aug 2021)

 * laplace problem
 * compute gradients of PDE solutions only if they are used
 * use brackets for vectors and parenthesis for functions
 * find extrema of mesh-based functions at gauss points
 * 2d & 3d thermal MMS tests
 * thermal expansion in mechanical
 * binary `.msh` v4.x reader
 * function fitting
 * per-problem virtual methods
 * modal problem
 * mechanical problem
 * cleaner pointwise-defined functions
 * jacobian for non-linear thermal
 * check consistency between PETSc/SLEPc headers and libraries
 * cell-based output
 * check dependency of expressions and decide linearity automatically
 * implicit materials
 * thermal problem
 * skeleton for PDEs
 * ODEs
 * renamed branch `master` to `main`
 * scripts to build binaries for distribution
 * support for `clang`
 * regressions tests under target `make check`
 
 
# v0.0 (Dec 2020)

 * initial development version to move stuff from [wasora](https://github.com/seamplex/wasora)/[Fino](https://github.com/seamplex/fino)/[milonga](https://github.com/seamplex/milonga) to the new [FeenoX repository](https://github.com/seamplex/feenox^).
 
