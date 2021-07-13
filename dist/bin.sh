#!/bin/bash

if [ ! -e ../src ]; then
  echo "run from dist directory"
  exit 0
fi

. versions.sh

if [ ! -e petsc-${petsc_ver} ]; then
  echo "PETSc directory petsc-${petsc_ver} does not exist"
  exit 1
fi
if [ ! -e petsc-${petsc_ver}/${PETSC_ARCH} ]; then
  echo "PETSc architecture petsc-${petsc_ver}/${PETSC_ARCH} does not exist"
  exit 1
fi
if [ ! -e petsc-${petsc_ver} ]; then
  echo "SLEPc directory slepc-${slepc_ver} does not exist"
  exit 1
fi

export PETSC_DIR=$(pwd)/petsc-${petsc_ver}
export SLEPC_DIR=$(pwd)/slepc-${slepc_ver}
cd ${package}
 ./configure PETSC_DIR=${PETSC_DIR} SLEPC_DIR=${SLEPC_DIR} PETSC_ARCH=${PETSC_ARCH} \
             --enable-download-gsl CFLAGS="-Ofast -DLD_STATIC" LDFLAGS="-static"
            
 cd doc
  make pdf
  make info
 cd ..
 make
cd ..

mkdir -p ${package}-${version}-${target}
cd ${package}-${version}-${target}

  mkdir -p bin
  mkdir -p share/doc/examples
  mkdir -p share/doc/tests
  mkdir -p share/man/man1

  if [ "x${target}" = "xlinux-amd64" ]; then
  
    cp ../${package}/${package} bin

    cp ../${package}/AUTHORS   share/doc
    cp ../${package}/ChangeLog share/doc
    cp ../${package}/COPYING   share/doc
    cp ../${package}/README    share/doc
    cp ../${package}/TODO      share/doc
    
  elif [ "x${target}" = "xwindows64" ]; then   
  
    cp ../${package}/${package}.exe bin
    cp /bin/cygwin1.dll bin
    
    unix2dos -n ../${package}/AUTHORS   AUTHORS.txt
    unix2dos -n ../${package}/ChangeLog ChangeLog.txt
    unix2dos -n ../${package}/COPYING   COPYING.txt
    unix2dos -n ../${package}/README    README.txt
    unix2dos -n ../${package}/TODO      TODO.txt
    
  fi

  # cp ../${package}/doc/${package}.svg      share/doc
  cp ../${package}/doc/${package}.xml      share/doc
  # cp ../${package}/doc/reference.txt share/doc
  cp ../${package}/doc/${package}.pdf      share/doc
  cp ../${package}/doc/${package}.texi     share/doc
  cp ../${package}/doc/${package}.info     share/doc

  cp -r ../${package}/examples/*       share/doc/examples
  cp -r ../${package}/tests/*          share/doc/tests
  
  cp ../${package}/doc/${package}.1 share/man/man1
  gzip -f share/man/man1/${package}.1
cd ..

if [ ! -e ${target} ]; then
 mkdir ${target}
fi

if [ "x${target}" = "xlinux-amd64" ]; then
  tar -zcf ${package}-${version}-${target}.tar.gz ${package}-${version}-${target}
  mv ${package}-${version}-${target}.tar.gz ${target}
elif [ "x${target}" = "xwindows64" ]; then   
  zip -r   ${package}-${version}-${target}.zip    ${package}-${version}-${target}
  mv ${package}-${version}-${target}.zip    ${target}
fi

