# Solving the NAFEMS LE10 problem with different codes

This directory contains a script `run.sh` that parametrically solves the [NAFEMS LE 10 problem](https://www.seamplex.com/feenox/examples/#nafems-le10-thick-plate-pressure-benchmark) with a number of different FEA codes over a wide range of mesh refinements. The expected result, namely $\sigma_y$ at point $D$ and the wall time, CPU time and memory are recorded for each run so as to create plots of these results vs. the total number of degrees of freedom being solved from.

This way of executing FEA programs follows the FeenoX design basis of being both cloud and script friendly. The reasons for requiring these friendlinesses are explained in FeenoX documentation, particularly in the SRS and SDS. It might happen that some of the codes tests seem to need to setup and/or read the results in a unnecessarily complex and/or cumbersome way because they were not designed to be either cloud and/or script friendly. It might also happen that the cumbersomeness comes from my lack of expertise about how to properly use the code.

The problem was chosen because 

 * it is a well-established benchmark since its publication in 1990
 * it is simple yet has displacement boundary condition on an edge in addition to faces that makes it challenging
 * the reference solution is a single scalar which is easy to compare among different approaches


Even though there are some particular comments for each of the code used in this comparison, this directory is not about the differences (and eventually pros and cons) each code has for defining and solving a FEA problem. It is about comparing the consumption of computational resources needed to solve the same problem (or almost). The differences about how to set up the problem and considerations about usage, cloud friendliness and scriptability are addressed in a separate directory regarding benchmark NAFEMS LE11.


## Reference solution

The original problem formulation (which can be found in [one of FeenoX' annotated examples](https://www.seamplex.com/feenox/examples/#nafems-le10-thick-plate-pressure-benchmark)) states that the reference solution is -5.38\ MPa. This can be confirmed with FeenoX using the input `le10-ref.fee`. 

```terminal
$ gmsh -3 le10-ref.geo
[...]
Info    : Done meshing order 2 (Wall 0.456586s, CPU 0.438907s)
Info    : 205441 nodes 59892 elements
Info    : Writing 'le10-ref.msh'...
Info    : Done writing 'le10-ref.msh'
Info    : Stopped on Thu Oct 28 12:03:28 2021 (From start: Wall 1.30955s, CPU 1.44333s)
$ time feenox le10-ref.fee
σ_y(D) =  -5.3792 MPa

real    1m34.485s
user    1m30.677s
sys     0m10.449s
$
```

This run can also be used to "calibrate" the timing. Just run the `le10-ref.fee` case and see how long FeenoX needs in your server. Then scale up (or down) the parametric results.


## Scripted parametric execution

The driving script is called `run.sh`.
If one executes the `run.sh` script, one gets

```terminal
$ ./run.sh 
usage: ./run.sh { tet | hex } min_clscale n_steps
$ 
```

To check of the codes are available run with `--check`:

```terminal
$ ./run.sh --check
FeenoX GAMG:  yes
FeenoX MUMPS: yes
Sparselizard: yes
Code Aster:   yes
CalculiX:     yes
$
```

Start with a large `min_clscale` < 1 and a few steps (at least 3, the two ends of the spectrum an the middle point).
Then increase the number of tests. Everything should be cached so only the extra steps will be actually executed.

There are two types of meshes:

 1. locally-refined (around point $D$) unstructured curved tetrahedral grid, and
 2. straight incomplete (i.e. hex20) fully-structured hexahedral mesh.
 
[Gmsh](http://gmsh.info/) is used to create the geometry and the mesh. It uses the `-clscale` command-line parameter to control the elements' size. This parameter\ $c$ is swept over a range of $[1:c_\text{min}]$ using a Sobol Quasi-random number sequence using the FeenoX input `steps.fee`. For example, five steps for $c_\text{min} = 0.1$ gives

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


The execution of `run.sh` with the three proper arguments 

 1. either `tet` or `hex`
 2. min scale factor
 3. number of steps

will give files `*.dat` that will be unsorted on the refinement factor (and number of degrees of freedom) so they are not suitable for plotting with lines. The script `plot.sh` will sort and plot them appropriately. These files have as columns

 1. the parameter $c \in [1:c_\text{min}]
 2. the total number of degrees of freedom
 3. the stress $\sigma_y$ evaluated at point $D$
 4. the wall time in seconds
 5. the kernel-mode CPU time in seconds
 6. the user-mode CPU time in seconds
 7. the maximum memory used by the program, in kB


## Creating the meshes

Both the continuous geometry and the discretized meshes are created with [Gmsh](http://gmsh.info/).
The `run.sh` script will not run if `gmsh` is not a valid command.

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
 
## General comments and caveats

 * The objective of this test is to compare consumption of resources for cloud-based computations. It is therefore suggested to run it on a cloud server and not on a local laptop.
 
 * In order to have the most fair comparison possible, event though the codes can measure CPU and memory consumption, all of them are run through the `time` tool (the actual binary tool at `/usr/bin/time`, not the shell's internal).
 
 * This is a **serial test only** so the variable `OMP_NUM_THREADS` is set to one to avoid spawning OpenMP threads. Parallel tests will come later on.
 
 * The hex mesh is created as a first-order mesh and then either 
   
    1. converted to a (straight) second-order incomplete (i.e. hex20) mesh, or
    2. the first-order mesh is fed to the code and it is asked to use second-order elements.
   
   depending on the capabilities of each code.
   
 
 * The second column of the output is the total number of degrees of freedom. In principle for a simple problem like this one it should be equal to three times the number of nodes. But Code Aster (apparently) sets Dirichlet boundary conditions as Lagrange multipliers, increasing the matrix size. On the contrary, CalculiX removes the degrees of freedom that correspond to nodes with  Dirichlet boundary conditions resulting in a smaller matrix size. Sparselizard needs complete elements so it uses hex27 when reading a hex8 mesh and setting order = 2, resulting in a (much) higher degree-of-freedom count.
 
 * When the mesh is big, chances are that the code runs out of memory being killed by the operating system (if there is no swap partition, which there should be not). Code Aster has a mode in which it can run with less memory than the actual requested, at the expense of larger CPU times (apparently by using a tailor-made disk-swapping procedure). This would explain the discontinuities in the Code Aster's curves.


# Particular comments about the codes

> **Disclaimer**: I am the author of FeenoX so all of my comments are likely to be biased.
If you are reading this and feel like something is not true or way too biased, please contact me and help me to have the fairest comparison possible. There might still be some subjectivity and I apologize in advance for that.


## FeenoX

Not only is [FeenoX](https://www.seamplex.com/feenox) the main code that I want to test, but it also is used to compute the quasi-random sequence of mesh refinements. So it is mandatory to have a working `feenox` command.

The easiest way to set up FeenoX is to download, un-compress and copy a statically-linked binary to a system-wide location:

```
wget https://seamplex.com/feenox/dist/linux/feenox-v0.1.152-g8329396-linux-amd64.tar.gz
tar xvzf feenox-v0.1.152-g8329396-linux-amd64.tar.gz
sudo cp feenox-v0.1.152-g8329396-linux-amd64/bin/feenox /usr/local/bin
```

Instead, it can be compiled from the [Github repository](https://github.com/seamplex/feenox) using stack PETSc from `apt` (which might be considered "old" in some GNU/Linux distributions). This will also enable the MUMPS solver and will add an extra curve named "FeenoX MUMPS" to the results:

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

Custom PETSc versions are architectures are supported as well. The default preconditioner+solver pair GAMG+cg is supported with all PETSc configurations. To make the MUMPS direct solver available, PETSc has to be configured and linked properly (i.e. configure with `--download-mumps` or use the PETSc packages from the operating system's repositories). See the [compilation guide](https://www.seamplex.com/feenox/doc/compilation.html) for further details.

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


The `run.sh` script calls `feenox` with the `le10.fee` input file as the first argument and `${m}-${c}` as the second one, resulting in something like `tet-1` or `hex-0.2`. This argument is expanded where the input file contains a `$1`, namely the mesh file name---which should already contain a second-order mesh. It prints the total number of degrees of freedom and the stress\ $\sigma_y$ at point $D$. It does not write any post-processing file (the `WRITE_MESH` keyword is commented out):

```feenox
# NAFEMS Benchmark LE-10: thick plate pressure
PROBLEM mechanical DIMENSIONS 3
DEFAULT_ARGUMENT_VALUE 1 1
READ_MESH le10_2nd-$1.msh   # FeenoX honors the order of the mesh

BC upper    p=1      # 1 Mpa
BC DCDC     v=0      # Face DCD'C' zero y-displacement
BC ABAB     u=0      # Face ABA'B' zero x-displacement
BC BCBC     u=0 v=0  # Face BCB'C' x and y displ. fixed
BC midplane w=0      #  z displacements fixed along mid-plane

E = 210e3   # Young modulus in MPa (because mesh is in mm)
nu = 0.3    # Poisson's ratio

SOLVE_PROBLEM   # TODO: implicit

PRINT total_dofs %.8f sigmay(2000,0,300)

# write post-processing data for paraview
# WRITE_MESH le10-feenox-${c}.vtk VECTOR u v w sigmax sigmay sigmaz tauxy tauzx tauyz
```



## Sparselizard

In order to test [Sparselizard](http://sparselizard.org/), a sub-directory named `sparselizard` should exist in the directory where `run.sh` is, and an executable named `sparselizard` should exist in that sub-directory. 

This can be achieved by cloning and compiling the [Github repository](https://github.com/halbux/sparselizard/). It should use PETSc/SLEPC obtained by executing the `install_external_libs/install_petsc.sh` script (which pulls latest main from the Gitlab repository). Also the Gmsh API is needed to read `.msh` v4, so run `install_external_libsoptional_install_gmsh_api.sh` as well.
The `main.cpp` is the file `le10.cpp` provided in the `le10-aster-lizard` directory, so a symbolic link has to be added:


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

Thanks Cyprien Rusu for all the help and explanations.

Code Aster is very tricky to compile (and use!). 
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

Thanks Sergio Pluchinsky.

CalculiX is available at Debian/Ubuntu repositories, although the versions are not up to date and they only have the Spooles solver.

There are sources which come with an already-working makefile (i.e. the don't need configuration).
Sadly, the official sources won't compile (and throw millions of warnings) with newer `gcc` (which is known to be more picky than previous versions):

```
$ wget http://www.dhondt.de/ccx_2.18.src.tar.bz2
$ tar xf ccx_2.18.src.tar.bz2 
$ cd CalculiX/ccx_2.18/src/
$ make
[...]
   21 |      &     nodef,idirf,df,cp,r,physcon,numf,set,mi,ider,ttime,time,
      |                                                             1
Warning: Unused dummy argument ‘ttime’ at (1) [-Wunused-dummy-argument]
cross_split.f:101:72:

  101 |      &           *(1.d0-pt2pt1**(1.d0/kdkm1))/r)/dsqrt(Tt1)
      |                                                                        ^
Warning: ‘a’ may be used uninitialized in this function [-Wmaybe-uninitialized]
gfortran -Wall -O2 -c cubic.f
gfortran -Wall -O2 -c cubtri.f
cubtri.f:131:18:

  131 |       CALL CUBRUL(F, VEC, W(1,1), IDATA, RDATA)
      |                  1
Error: Interface mismatch in dummy procedure ‘f’ at (1): 'f' is not a function
cubtri.f:170:20:

  170 |         CALL CUBRUL(F, VEC, W(1,J), IDATA, RDATA)
      |                    1
Error: Interface mismatch in dummy procedure ‘f’ at (1): 'f' is not a function
make: *** [Makefile:11: cubtri.o] Error 1
$  
```

So we are sticking with the `apt` repository.


Regarding the generation of the input, IMHO it is cumbersome to say the least.

https://gitlab.onelab.info/gmsh/gmsh/-/commit/a7fef9f6e8a7c870cf39b8702c57f3e33bfa948d

## Reflex

TO-DO
