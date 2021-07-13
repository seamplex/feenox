#!/bin/bash

if [ ! -e ../src ]; then
  echo "run from dist directory"
  exit 0
fi

package=feenox

. clone.sh
. versions.sh

# -----------------------------------------------------------
#   source
# -----------------------------------------------------------
cd ${package}
 ./autogen.sh
 ./configure PETSC_DIR="" SLEPC_DIR="" PETSC_ARCH=""
 automake --add-missing
 echo ${version}

 # cd doc
 #  make pdf
 # cd ..
 #  make

 # rm -f petscslepc.mak
 # touch petscslepc.mak

 make dist
cd ..

if [ ! -e src ]; then
  echo "creating src directory"
  mkdir src
fi

echo ${package}/${package}-${version}.tar.gz

if [ -e ${package}/${package}-${version}.tar.gz ]; then
 mv ${package}/${package}-${version}.tar.gz src
else
 echo "cannot create source tarball, quitting"
 exit 1
fi

exit 0
