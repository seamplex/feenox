---
title: Compilation instructions
lang: en-US
titleblock: |
 Compilation instructions
 ========================
...


# Quickstart

Note that the quickest way to get started is to get an already-compiled statically-linked binary executable. Follow these instructions if that option is not suitable for your workflow.

On a GNU/Linux box (preferably Debian-based), follow these quick steps. See next section for detailed explanations.

```{.include}
design/git.md
```

# Detailed configuration and compilation

The main target and development environment is Debian\ GNU/Linux, although it should be possible to compile FeenoX in any free GNU/Linux variant (and even the in non-free  MacOS and Windows). As per the [SRS](SRS.md), all dependencies have to be available on mainstream GNU/Linux distributions. But they can also be compiled from source in case the package repositories are not available or customized compilation flags are needed (i.e. optimization or debugging settings).

All the dependencies are free and open source software. PETSc/SLEPc also depend on other mathematical libraries to perform particular operations such as linear algebra. These extra dependencies can be either free (such as LAPACK) or non-free (such as MKL), but there is always at least one combination of a working setup that involves only free and open source software which is compatible with FeenoX licensing terms (GPLv3+). See the documentation of each package for licensing details.

## Mandatory dependencies

FeenoX has one mandatory dependency for run-time execution and the standard build toolchain for compilation. It is written in C99 so only a C compiler is needed, although `make` is also required. Free and open source compilers are favored. The usual C compiler is `gcc` but `clang` can also be used. Nevertheless, the non-free `icc` has also been tested.

Note that there is no need to have a Fortran nor a C++ compiler to build FeenoX. They might be needed to build other dependencies (such as PETSc), but not to compile FeenoX with all the dependencies installed from package repositories. In case the build toolchain is not already installed, do so with

```terminal
sudo apt-get install gcc make
```

If the source is to be fetched from the Git repository then of course `git` is needed but also `autoconf` and `automake` since the `configure` script is not stored in the Git repository but the `autogen.sh` script that bootstraps the tree and creates it. So if instead of compiling a source tarball one wants to clone from GitHub, these packages are also mandatory:

```terminal
sudo apt-get install git automake autoconf
```

Again, chances are that any existing GNU/Linux box has all these tools already installed. 

### The GNU Scientific Library

The only run-time dependency is [GNU GSL](https://www.gnu.org/software/gsl/) (not to be confused with [Microsoft GSL](https://github.com/microsoft/GSL)). It can be installed with

```terminal
sudo apt-get install libgsl-dev
```

In case this package is not available or you do not have enough permissions to install system-wide packages, there are two options.

 1. Pass the option `--enable-download-gsl` to the `configure` script below.
 2. Manually download, compile and install [GNU GSL](https://www.gnu.org/software/gsl/) 

If the `configure` script cannot find both the headers and the actual library, it will refuse to proceed.
Note that the FeenoX binaries already contain a static version of the GSL so it is not needed to have it installed in order to run the statically-linked binaries.


## Optional dependencies

FeenoX has three optional run-time dependencies. It can be compiled without any of these but functionality will be reduced:

 * [SUNDIALS](https://computing.llnl.gov/projects/sundials) provides support for solving systems of ordinary differential equations (ODEs) or differential-algebraic equations (DAEs). This dependency is needed when running inputs with the `PHASE_SPACE` keyword.
 
 * [PETSc](https://petsc.org/) provides support for solving partial differential equations (PDEs). This dependency is needed when running inputs with the `PROBLEM` keyword.
 
 * [SLEPc](https://slepc.upv.es/) provides support for solving eigen-value problems in partial differential equations (PDEs). This dependency is needed for inputs with `PROBLEM` types with eigen-value formulations such as `modal` and `neutron_transport`. 

In absence of all these, FeenoX can still be used to 

 * solve general mathematical problems such as the ones to compute the Fibonacci sequence,
 * operate on functions, either algebraically or point-wise interpolated,
 * read, operate over and write meshes,
 * etc.
 
These optional dependencies have to be installed separately. There is no option to have `configure` to download them as with `--enable-download-gsl`.
 
### SUNDIALS

[SUNDIALS](https://computing.llnl.gov/projects/sundials) is a SUite of Nonlinear and DIfferential/ALgebraic equation Solvers. It is used by FeenoX to solve dynamical systems casted as DAEs with the keyword `PHASE_SPACE, like the Lorenz system.

Install either by doing

```terminal
sudo apt-get install lib-sundials-dev
```

\noindent 
or by following the instructions in the documentation.


### PETSc

[PETSc](https://petsc.org/), the Portable, Extensible Toolkit for Scientific Computation, pronounced PET-see (/ˈpɛt-siː/), is a suite of data structures and routines for the scalable (parallel) solution of scientific applications modeled by partial differential equations. It is used by FeenoX to solve PDEs with the keyword `PROBLEM`, like thermal conduction on a slab.


Install either by doing

```terminal
sudo apt-get install petsc-dev
```

\noindent 
or by following the instructions in the documentation.

Note that

 * Configuring and compiling PETSc from scratch might be difficult the first time. It has a lot of dependencies and options. Read the official [documentation](https://petsc.org/release/install/) for a detailed explanation.
 * There is a huge difference in efficiency between using PETSc compiled with debugging symbols and with optimization flags. Make sure to configure `--with-debugging=0` for FeenoX production runs and leave the debugging symbols (which is the default) for development only.
 * FeenoX needs PETSc to be configured with real double-precision scalars. It will compile but will complain at run-time when using complex and/or single or quad-precision scalars.
 * FeenoX honors the `PETSC_DIR` and `PETSC_ARCH` environment variables when executing `configure`. If these two do not exist or are empty, it will try to use the default system-wide locations (i.e. the `petsc-dev` package).

### SLEPc

[SLEPc](https://slepc.upv.es/), the Scalable Library for Eigenvalue Problem Computations, is a software library for the solution of large scale sparse eigenvalue problems on parallel computers. It is used by FeenoX to solve PDEs with the keyword `PROBLEM` that need eigen-value computations, such as modal analysis of a cantilevered beam.

Install either by doing

```terminal
sudo apt-get install slepc-dev
```

\noindent 
or by following the instructions in the documentation.

Note that

 * SLEPc is an extension of PETSc so the latter has to be already installed and configured.
 * FeenoX honors the `SLEPC_DIR` environment variable when executing `configure`. If it does not exist or is empty it will try to use the default system-wide locations (i.e. the `slepc-dev` package). 
 * If PETSc was configured with `--download-slepc` then the `SLEPC_DIR` variable has to be set to the directory inside `PETSC_DIR` where SLEPc was cloned and compiled.

## FeenoX source code

There are two ways of getting FeenoX' source code:

 1. Cloning the GitHub repository at <https://github.com/seamplex/feenox>
 2. Downloading a source tarball from <https://seamplex.com/feenox/dist/src/>
 
### Git repository

The main Git repository is hosted on GitHub at <https://github.com/seamplex/feenox. It is public so it can be cloned either through HTTPS or SSH without needing any particular credentials. It can also be forked freely. See the  [Programming Guide](programming.md) for details about pull requests and/or write access to the main repository.



### Source tarballs

 

## Configuration and compilation

## Run the test suite

## Install

# Advanced settings

```{.include}
compile-debug.md
compile-other-compiler.md
```
