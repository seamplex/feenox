# Solving the NAFEMS LE10 with different codes

This directory contains a script `run.sh` that parametrically solves the [NAFEMS LE 10 problem](https://www.seamplex.com/feenox/examples/#nafems-le10-thick-plate-pressure-benchmark) with a number of different FEA codes over a wide range of mesh refinements. The expected result, namely $\sigma_y$ at point $D$ and the wall time, CPU time and memory are recorded for each run so as to create plots of these results vs. the total number of degrees of freedom being solved from.

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




 * The objective of this test is to compare consumption of resources for cloud-based computations. It is therefore suggested to run it on a cloud server and not on a local laptop.
 * In order to have the most fair comparison possible, event though the codes can measure CPU and memory consumption, all of them are run through the `time` tool (the actual binary tool, not the shell's internal).
 * This is a serial test so the variable `OMP_NUM_THREADS` is set to one to avoid OpenMP computations.
 * The hex mesh is created as a first-order mesh and then either converted to a (straight) second-order incomplete (i.e. hex20) mesh or the first-order mesh is fed to the code and it is asked to use second-order elements. Sparselizard needs complete elements so it uses hex27, resulting in a higher degree-of-freedom count.
 * When the mesh is big, chances are that the code run out of memory being killed by the operating system (if there is no swap partition, which there should be not). Code Aster has a mode in which it can run with less memory than the actual requested, at the expense of larger CPU times (apparently by using a tailor-made disk-swapping procedure). This would explain the discontinuities in the Code Aster's curves.

 

# Running the tests

## Gmsh

Make sure [Gmsh](http://gmsh.info/) is up to date:

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

## FEA codes

### FeenoX


An statically-compiled binary can be used for the test. Download, un-compress and copy the binary to a system-wide location:

```
wget https://seamplex.com/feenox/dist/linux/feenox-v0.1.152-g8329396-linux-amd64.tar.gz
tar xvzf feenox-v0.1.152-g8329396-linux-amd64.tar.gz
sudo cp feenox-v0.1.152-g8329396-linux-amd64/bin/feenox /usr/local/bin
```

Instead, it can be compiled from the [Github repository](https://github.com/seamplex/feenox) using stack PETSc/SLEPc from Apt (which might be "old"):

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

Either way, check it works globally:

```terminal
$ feenox -V
FeenoX v0.1.152-g8329396-dirty 
a free no-fee no-X uniX-like finite-element(ish) computational engineering tool

Last commit date   : Sat Oct 23 19:06:18 2021 -0300
Build date         : Sun Oct 24 20:00:34 2021 -0300
Build architecture : linux-gnu x86_64
Compiler           : gcc (Debian 10.2.1-6) 10.2.1 20210110
Compiler flags     : -Ofast -DLD_STATIC
Builder            : gtheler@tom
GSL version        : 2.7
SUNDIALS version   : 5.7.0
PETSc version      : Petsc Release Version 3.16.0, Sep 29, 2021 
PETSc arch         : linux-serial-static
PETSc options      : PETSC_DIR=/home/gtheler/codigos/feenox/dist/petsc-3.16.0 PETSC_ARCH=linux-serial-static --with-mpi=0 --with-fc=0 --with-cxx=0 --with-fortran-bindings=0 --with-fc=0 --with-c2html=0 --with-x=0 --with-debugging=0 --with-shared-libraries=0 --download-f2cblaslapack --COPTFLAGS=-Ofast
SLEPc version      : SLEPc Release Version 3.16.0, Sep 30, 2021
$
```



## Sparselizard

Compile it from the [Github repository](https://github.com/halbux/sparselizard/) using PETSc/SLEPC from the `./install_petsc.sh` script (which pulls latest main from Gitlab). Also the Gmsh API is needed to read `.msh` v4.

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
