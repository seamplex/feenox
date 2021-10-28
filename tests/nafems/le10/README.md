# Solving the NAFEMS LE10 problem with different codes

This directory contains a script `run.sh` that parametrically solves the [NAFEMS LE 10 problem](https://www.seamplex.com/feenox/examples/#nafems-le10-thick-plate-pressure-benchmark) with a number of different FEA codes over a wide range of mesh refinements. The expected result, namely $\sigma_y$ at point $D$ and the wall time, CPU time and memory are recorded for each run so as to create plots of these results vs. the total number of degrees of freedom being solved from.

This way of executing FEA programs follows the FeenoX design basis of being both cloud and script friendly. The reasons for requiring these friendlinesses are explained in FeenoX documentation, particularly in the SRS and SDS. It might happen that some of the codes tests seem to need to setup and/or read the results in a unnecessarily complex and/or cumbersome way because they were not designed to be either cloud and/or script friendly. It might also happen that the cumbersomeness comes from my lack of expertise about how to properly use the code.

The problem was chosen because 

 * it is a well-established benchmark since its publication in 1990
 * it is simple yet has displacement boundary condition on an edge in addition to faces that makes it challenging
 * the reference solution is a single scalar which is easy to compare among different approaches


Even though there are some particular comments for each of the code used in this comparison, this directory is not about the differences (and eventually pros and cons) each code has for defining and solving a FEA problem. It is about comparing the consumption of computational resources needed to solve the same problem (or almost). The differences about how to set up the problem and considerations about usage, cloud friendliness and scriptability are addressed in a separate directory regarding benchmark NAFEMS LE11.


## Reference solution



## Scripted parametric execution

There are two types of meshes, one locally-refined (around point $D$) unstructured curved tetrahedral grid and one straight incomplete (i.e. hex20) fully-structured hexahedral mesh. Gmsh is used to create the geometry and the mesh. It uses the `-clscale` command-line parameter to control the elements' size. This parameter $c$ is swept over a range of $[1:c_\text{min}]$ using a Sobol Quasi-random number sequence using the FeenoX input `steps.fee`. For example, five steps for $c_\text{min} = 0.1$ gives

```terminal
$ feenox steps.fee 0.1 5 
1
0.1
0.55
0.775
0.325
```

That is to say, the first and second steps are the coarsest and finest meshes respectively. The third one is the middle-range mesh, and the following steps start to "fill" in the blanks. Since the `run.sh` script caches the results it gets, further steps can be performed by reusing the existing data. So if we now want to run ten steps,

```terminal
$ feenox steps.fee 0.1 10
1
0.1
0.55
0.775
0.325
0.4375
0.8875
0.6625
0.2125
0.26875
```

and the first five steps will use cached data instead of re-running all the codes.
The execution of `run.sh` will give files `*.dat` that will be unsorted on the number of degrees of freedom so they are not suitable for plotting with lines. The script `plot.sh` will sort and plot them appropriately. These files have as columns

 1. the parameter $c \in [1:c_\text{min}]
 2. the total number of degrees of freedom
 3. the stress $\sigma_y$ evaluated at point $D$
 4. the wall time in seconds
 5. the user-mode CPU time in seconds
 6. the kernel-mode CPU time in seconds
 7. the maximum memory used by the program, in kB


## Caveats and comments

 * The objective of this test is to compare consumption of resources for cloud-based computations. It is therefore suggested to run it on a cloud server and not on a local laptop.
 * In order to have the most fair comparison possible, event though the codes can measure CPU and memory consumption, all of them are run through the `time` tool (the actual binary tool, not the shell's internal).
 * This is a **serial test only** so the variable `OMP_NUM_THREADS` is set to one to avoid OpenMP computations.
 * The hex mesh is created as a first-order mesh and then either converted to a (straight) second-order incomplete (i.e. hex20) mesh or the first-order mesh is fed to the code and it is asked to use second-order elements. Sparselizard needs complete elements so it uses hex27, resulting in a higher degree-of-freedom count.
 * When the mesh is big, chances are that the code run out of memory being killed by the operating system (if there is no swap partition, which there should be not). Code Aster has a mode in which it can run with less memory than the actual requested, at the expense of larger CPU times (apparently by using a tailor-made disk-swapping procedure). This would explain the discontinuities in the Code Aster's curves.


 
 

# Running the tests

The driving script is called `run.sh`.
Check which of the codes are available with `--check`:

```terminal
$ ./run.sh --check
FeenoX GAMG:  yes
FeenoX MUMPS: yes
Sparselizard: yes
Code Aster:   yes
CalculiX:     yes
$
```

Gmsh and FeenoX are mandatory.


## Gmsh

The meshes are created with [Gmsh](http://gmsh.info/).
It is better to use latest versions instead of the one distributed in the operating system's package repositories.
Either the official binaries or a compiled-from-scratch version will do.

To compile from source:

```terminal
sudo apt-get install libocct-data-exchange-dev libocct-foundation-dev libocct-modeling-data-dev
git clone https://gitlab.onelab.info/gmsh/gmsh.git
cd gmsh
mkdir build && cd build
make
sudo make install
```

To download the no-X binary version:

```terminal
wget http://gmsh.info/bin/Linux/gmsh-nox-git-Linux64-sdk.tgz
tar xvzf gmsh-nox-git-Linux64-sdk.tgz
sudo cp gmsh-nox-git-Linux64-sdk/bin/gmsh /usr/local/bin
sudo cp -P gmsh-nox-git-Linux64-sdk/lib/* /usr/local/lib
```

Check it works globally:

```terminal
$ gmsh -info
Version       : 4.9.0-nox-git-701db57af
License       : GNU General Public License
Build OS      : Linux64-sdk
Build date    : 20211019
Build host    : gmsh.info
Build options : 64Bit ALGLIB ANN Bamg Blas[petsc] Blossom Cgns DIntegration Dlopen DomHex Eigen Gmm Hxt Kbipack Lapack[petsc] LinuxJoystick MathEx Med Mesh Metis Mmg Netgen ONELAB ONELABMetamodel OpenCASCADE OpenMP OptHom PETSc Parser Plugins Post QuadMeshingTools QuadTri Solver TetGen/BR Voro++ WinslowUntangler Zlib
PETSc version : 3.14.4 (real arithmtic)
OCC version   : 7.6.0
MED version   : 4.1.0
Packaged by   : geuzaine
Web site      : https://gmsh.info
Issue tracker : https://gitlab.onelab.info/gmsh/gmsh/issues
$
```


## FeenoX


An statically-compiled binary can be used for the test. Download, un-compress and copy the binary to a system-wide location:

```
wget https://seamplex.com/feenox/dist/linux/feenox-v0.1.152-g8329396-linux-amd64.tar.gz
tar xvzf feenox-v0.1.152-g8329396-linux-amd64.tar.gz
sudo cp feenox-v0.1.152-g8329396-linux-amd64/bin/feenox /usr/local/bin
```

Instead, it can be compiled from the [Github repository](https://github.com/seamplex/feenox) using stack PETSc from `apt` (which might be considered "old"):

```terminal
sudo apt-get install gcc make git automake autoconf libgsl-dev petsc-dev slepc-dev
git clone https://github.com/seamplex/feenox
cd feenox
./autogen.sh
./configure
make
make check
sudo make install
```

Custom PETSc versions are architectures are supported as well. The default preconditioner+solver pair GAMG+cg is supported with all PETSc configurations. To make the MUMPS direct solver available, PETSc has to be configured and linked properly (i.e. configure with `--download-mumps` or use the PETSc packages from the operating system's repositories).

Either way, check it works globally:

```terminal
$ feenox -V
FeenoX v0.1.159-gab7abd8-dirty 
a free no-fee no-X uniX-like finite-element(ish) computational engineering tool

Last commit date   : Thu Oct 28 10:43:38 2021 -0300
Build date         : Thu Oct 28 13:52:11 2021 +0000
Build architecture : linux-gnu x86_64
Compiler           : gcc (Ubuntu 9.3.0-17ubuntu1~20.04) 9.3.0
Compiler flags     : -O3
Builder            : ubuntu@ip-172-31-44-208
GSL version        : 2.5
SUNDIALS version   : 3.1.2
PETSc version      : Petsc Release Version 3.12.4, Feb, 04, 2020 
PETSc arch         : 
PETSc options      : --build=x86_64-linux-gnu --prefix=/usr --includedir=${prefix}/include --mandir=${prefix}/share/man --infodir=${prefix}/share/info --sysconfdir=/etc --localstatedir=/var --with-silent-rules=0 --libdir=${prefix}/lib/x86_64-linux-gnu --runstatedir=/run --with-maintainer-mode=0 --with-dependency-tracking=0 --with-debugging=0 --shared-library-extension=_real --with-shared-libraries --with-pic=1 --with-cc=mpicc --with-cxx=mpicxx --with-fc=mpif90 --with-cxx-dialect=C++11 --with-opencl=1 --with-blas-lib=-lblas --with-lapack-lib=-llapack --with-scalapack=1 --with-scalapack-lib=-lscalapack-openmpi --with-mumps=1 --with-mumps-include="[]" --with-mumps-lib="-ldmumps -lzmumps -lsmumps -lcmumps -lmumps_common -lpord" --with-suitesparse=1 --with-suitesparse-include=/usr/include/suitesparse --with-suitesparse-lib="-lumfpack -lamd -lcholmod -lklu" --with-ptscotch=1 --with-ptscotch-include=/usr/include/scotch --with-ptscotch-lib="-lptesmumps -lptscotch -lptscotcherr" --with-fftw=1 --with-fftw-include="[]" --with-fftw-lib="-lfftw3 -lfftw3_mpi" --with-superlu=1 --with-superlu-include=/usr/include/superlu --with-superlu-lib=-lsuperlu --with-superlu_dist=1 --with-superlu_dist-include=/usr/include/superlu-dist --with-superlu_dist-lib=-lsuperlu_dist --with-hdf5-include=/usr/include/hdf5/openmpi --with-hdf5-lib="-L/usr/lib/x86_64-linux-gnu/hdf5/openmpi -L/usr/lib/openmpi/lib -lhdf5 -lmpi" --CXX_LINKER_FLAGS=-Wl,--no-as-needed --with-hypre=1 --with-hypre-include=/usr/include/hypre --with-hypre-lib=-lHYPRE_core --prefix=/usr/lib/petscdir/petsc3.12/x86_64-linux-gnu-real --PETSC_ARCH=x86_64-linux-gnu-real CFLAGS="-g -O2 -fstack-protector-strong -Wformat -Werror=format-security -fPIC" CXXFLAGS="-g -O2 -fstack-protector-strong -Wformat -Werror=format-security -fPIC" FCFLAGS="-g -O2 -fstack-protector-strong -fPIC -ffree-line-length-0" FFLAGS="-g -O2 -fstack-protector-strong -fPIC -ffree-line-length-0" CPPFLAGS="-Wdate-time -D_FORTIFY_SOURCE=2" LDFLAGS="-Wl,-Bsymbolic-functions -Wl,-z,relro -fPIC" MAKEFLAGS=w
SLEPc version      : SLEPc Release Version 3.12.2, Jan 13, 2020
$
```

The `PETSc options` line will tell if MUMPS is available or not, so grepping will tell:

```
$ feenox -V | grep -i mumps | wc -l
1
$
```

## Sparselizard

Compile it from the [Github repository](https://github.com/halbux/sparselizard/) using PETSc/SLEPC from the `./install_petsc.sh` script (which pulls latest main from the Gitlab repository). Also the Gmsh API is needed to read `.msh` v4, so run `optional_install_gmsh_api.sh` as well.

The `main.cpp` is the file `le10.cpp` provided in the `le10-aster-lizard` directory.
The tree `sparselizard` should live in `le10-aster-lizard`:


```terminal
cd feenox/tests/nafems/le10-aster-lizard
git clone https://github.com/halbux/sparselizard/
cd sparselizard/
ln -s ../le10.cpp main.cpp
cd install_external_libs/
./install_petsc.sh
./optional_install_gmsh_api.sh
cd ..
make
cd ..
```

If there is no binary named `sparselizard` in a directory named `sparselizard`, the `run.sh` script will not try to solve the case with Sparselizard.

## Code Aster

Code Aster is very tricky to compile (and use!). Thanks Cyprien Rusu for all the help and explanations.
This works in Ubuntu 20.04:

```terminal
wget https://www.code-aster.org/FICHIERS/aster-full-src-14.6.0-1.noarch.tar.gz
tar xvzf aster-full-src-14.6.0-1.noarch.tar.gz 
cd aster-full-src-14.6.0
mkdir -p $HOME/aster
python3 setup.py --aster_root=$HOME/aster
cd 
source aster/etc/codeaster/profile.sh 
```

Check it works globally:

```
$ as_run --getversion
<INFO> Version exploitation 14.6.0 - 11/06/2020 - rev. b3490fa3b76c
$
```

## CalculiX

TO-DO. Thanks Sergio Pluchinsky.

## Reflex

TO-DO
