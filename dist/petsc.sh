#!/bin/bash -ex

if [ ! -e ../tests ]; then
  echo "error: run from dist directory"
  exit 0
fi

. versions.sh

# --- PETSc -------------------------------
if [ ! -e petsc-${petsc_ver} ]; then
 if [ ! -e petsc-${petsc_ver}.tar.gz ]; then
#   wget https://ftp.mcs.anl.gov/pub/petsc/release-snapshots/petsc-${petsc_ver}.tar.gz
  wget -c https://web.cels.anl.gov/projects/petsc/download/release-snapshots/petsc-${petsc_ver}.tar.gz
  if [ ! -e petsc-${petsc_ver}.tar.gz ]; then
   echo "error: cannot download PETSc"
   exit 1
  fi
 fi
 tar xzf petsc-${petsc_ver}.tar.gz
fi 

cd petsc-${petsc_ver}
export PETSC_DIR=$(pwd)
export PETSC_ARCH=${PETSC_ARCH}

# openblas is fast but needs libgfortran, we stay with f2cblaslapack
# same for mumps

# this is doable but the resulting binary is huge
#   --download-mpich \

./configure \
  --download-f2cblaslapack --download-f2cblaslapack-shared=0 \
  --with-shared-libraries=0 \
  --with-debugging=0 \
  --with-mpi=0 \
  --with-cxx=0 \
  --with-fc=0 \
  --with-fortran-bindings=0 \
  --with-c2html=0 \
  --with-bison=0 \
  --with-x=0 \
  --COPTFLAGS="-O3" #CC=$CC FC=$FC F77=$F77 CXX=$CXX

make
make check
cd ..

# --- SLEPc -------------------------------
if [ ! -e slepc-${slepc_ver} ]; then
 if [ ! -e slepc-${slepc_ver}.tar.gz ]; then
  wget https://slepc.upv.es/download/distrib/slepc-${slepc_ver}.tar.gz
  if [ ! -e slepc-${slepc_ver}.tar.gz ]; then
   echo "error: cannot download SLEPc"
   exit 1
  fi
 fi
 tar xzf slepc-${slepc_ver}.tar.gz
fi

cd slepc-${slepc_ver}
export SLEPC_DIR=$(pwd)
./configure
make 
make check
cd ..
