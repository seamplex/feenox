#!/bin/bash

if [ ! -e ../src ]; then
  echo "run from dist directory"
  exit 0
fi

. versions.sh

if [ "x${name}" != "xLinux" ]; then
  echo "source tarballs should be created only in GNU/Linux"
  exit 0
fi

# -----------------------------------------------------------
#   source
# -----------------------------------------------------------
cd ${package}
 ./configure PETSC_DIR="" SLEPC_DIR="" PETSC_ARCH=""
 automake --add-missing
 make dist
cd ..

if [ ! -e src ]; then
  echo "creating src directory"
  mkdir src
fi

if [ -e ${package}/${package}-${version}.tar.gz ]; then
 mv ${package}/${package}-${version}.tar.gz src
else
 echo "cannot create source tarball, quitting"
 exit 1
fi
