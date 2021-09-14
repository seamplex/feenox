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
if [ ! -e ${package}-src ]; then
  git clone .. ${package}-src
else
  cd ${package}-src; git pull || exit 1; cd ..
fi

cd ${package}-src
 ./autogen.sh || exit 1
 ./configure PETSC_DIR="" SLEPC_DIR="" PETSC_ARCH="" || exit 1
 automake --add-missing --force-missing || exit 1
 make distcheck || exit 1
cd ..

if [ ! -e src ]; then
  echo "creating src directory"
  mkdir src
fi

if [ -e ${package}-src/${package}-${version}.tar.gz ]; then
 mv ${package}-src/${package}-${version}.tar.gz src
else
 echo "cannot create source tarball, quitting"
 exit 1
fi
