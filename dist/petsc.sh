#!/bin/bash

. versions.sh

# --- PETSc -------------------------------
if [ ! -e petsc-${petsc_ver} ]; then
 if [ ! -e petsc-${petsc_ver}.tar.gz ]; then
  wget https://ftp.mcs.anl.gov/pub/petsc/release-snapshots/petsc-${petsc_ver}.tar.gz
  if [ ! -e petsc-${petsc_ver}.tar.gz ]; then
   echo "cannot download PETSc"
   exit 1
  fi
 fi
 tar xvzf petsc-${petsc_ver}.tar.gz
fi 

cd petsc-${petsc_ver}
export PETSC_DIR=$(pwd)
./configure PETSC_DIR=${PETSC_DIR} \
	    PETSC_ARCH=${PETSC_ARCH} \
	    --with-mpi=0 \
            --with-fc=0 \
            --with-cxx=0 \
            --with-fortran-bindings=0 \
            --with-fc=0 \
            --with-c2html=0 \
            --with-x=0 \
            --with-debugging=0 \
            --with-shared-libraries=0 \
            --download-f2cblaslapack \
            --COPTFLAGS="-Ofast"

# export PETSC_ARCH=arch-linux2-serial-mumps-static
# ./configure --with-mpi=0 --with-cxx=0 --with-fortran-bindings=0 --with-fc=0 --with-c2html=0 --with-x=0 --with-debugging=0 --with-shared-libraries=0 --with-mumps-serial=1 --download-mumps --download-openblas --COPTFLAGS="-Ofast" --FOPTFLAGS="-Ofast"
# OJO! hay que sacar a mano el -lgcc_s de ${PETSC_DIR}/${PETSC_ARCH}/lib/petsc/conf/petscvariables
make
make check
cd ..

# --- SLEPc -------------------------------
if [ ! -e slepc-${slepc_ver} ]; then
 if [ ! -e slepc-${slepc_ver}.tar.gz ]; then
  wget https://slepc.upv.es/download/distrib/slepc-${slepc_ver}.tar.gz
  if [ ! -e slepc-${slepc_ver}.tar.gz ]; then
   echo "cannot download SLEPc"
   exit 1
  fi
 fi
 tar xvzf slepc-${slepc_ver}.tar.gz
fi

cd slepc-${slepc_ver}
export SLEPC_DIR=$(pwd)
export PETSC_DIR=${PETSC_DIR}
export PETSC_ARCH=${PETSC_ARCH}
./configure 
make 
make check
cd ..


