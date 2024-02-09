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

# TODO: neither mpi nor mumps work in static
./configure \
  --with-cxx=0 \
  --with-fortran-bindings=0 \
  --with-c2html=0 \
  --with-bison=0 \
  --with-x=0 \
  --with-debugging=0 \
  --with-shared-libraries=0 \
  --download-openblas \
  --download-mpich \
  --COPTFLAGS="-O3" --FOPTFLAGS="-O3"
           
#   --with-mpi=0 \
#   --download-mpich \

           
# export PETSC_ARCH=arch-linux2-serial-mumps-static
# ./configure --with-mpi=0 --with-cxx=0 --with-fortran-bindings=0 --with-fc=0 --with-c2html=0 --with-x=0 --with-debugging=0 --with-shared-libraries=0 --with-mumps-serial=1 --download-mumps --download-openblas --COPTFLAGS="-O" --FOPTFLAGS="-O3"
# OJO! hay que sacar a mano el -lgcc_s de ${PETSC_DIR}/${PETSC_ARCH}/lib/petsc/conf/petscvariables
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
