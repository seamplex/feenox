#!/bin/bash

if [ ! -e ../src ]; then
  echo "run from dist directory"
  exit 0
fi

package=feenox

. clone.sh
. versions.sh

if [ ! -e petsc-${petsc_ver} ]; then
  echo "PETSc directory petsc-${petsc_ver} does not exist"
  exit 1
fi
if [ ! -e petsc-${petsc_ver} ]; then
  echo "SLEPc directory slepc-${slepc_ver} does not exist"
  exit 1
fi

cd ${package}
 ./autogen.sh
 ./configure PETSC_DIR=$(pwd)/../petsc-${petsc_ver} \
             SLEPC_DIR=$(pwd)/../slepc-${slepc_ver} \
             CFLAGS="-Ofast -DLD_STATIC" \
             LDFLAGS="-static" \
             --enable-download-gsl
 cd doc
  make pdf
  make info
 cd ..
 make
cd ..

mkdir -p ${package}-${version}-${target}
cd ${package}-${version}-${target}


if [ "x${target}" = "xlinux-amd64" ]; then
# the static binary
  mkdir -p bin
  cp ../${package}/${package} bin
  
  mkdir -p share/doc
  cp ../${package}/AUTHORS   share/doc
  cp ../${package}/ChangeLog share/doc
  cp ../${package}/COPYING   share/doc
  cp ../${package}/README    share/doc
  cp ../${package}/TODO      share/doc

  # cp ../${package}/doc/${package}.svg      share/doc
  cp ../${package}/doc/${package}.xml      share/doc
  # cp ../${package}/doc/reference.txt share/doc
  cp ../${package}/doc/${package}.pdf      share/doc
  cp ../${package}/doc/${package}.texi     share/doc
  cp ../${package}/doc/${package}.info     share/doc

  mkdir -p share/doc/examples
  cp ../${package}/examples/*       share/doc/examples

  mkdir -p share/man/man1
  cp ../${package}/doc/${package}.1 share/man/man1
  gzip -f share/man/man1/${package}.1
fi
cd ..
tar -zcf ${package}-${version}-${target}.tar.gz ${package}-${version}-${target}

if [ ! -e ${target} ]; then
 mkdir ${target}
fi
mv ${package}-${version}-${target}.tar.gz ${target}

exit 0


# # -----------------------------------------------------------
# #   binary: windows-amd64
# # -----------------------------------------------------------
# 
# if [ ! -e ${package}-${version}.exe ]; then
#   echo "${package}-${version}.exe does not exist, not creating windows binary"
# else
# 
#   target=windows-amd64
#   
#   mkdir -p ${package}-${version}-${target}
#   cd ${package}-${version}-${target}
#   
#   # the static binary and the cygwin dll
#   cp ../${package}-${version}.exe  ./${package}.exe
#   cp ../cygwin1.dll .
#   
#   unix2dos -n ../${package}/AUTHORS   AUTHORS.txt
#   unix2dos -n ../${package}/ChangeLog ChangeLog.txt
#   unix2dos -n ../${package}/COPYING   COPYING.txt
#   unix2dos -n ../${package}/README    README.txt
#   unix2dos -n ../${package}/TODO      TODO.txt
#   
#   unix2dos -n ../${package}/doc/reference.txt reference.txt
#   cp ../${package}/doc/fino.svg      .
#   cp ../${package}/doc/fino.xml      .
#   cp ../${package}/doc/fino.pdf      .
#   
#   mkdir -p examples
#   cp ../${package}/examples/*       examples
#   
#   cd ..
#   zip -r ${package}-${version}-${target}.zip ${package}-${version}-${target}
#   
# fi
# 
# 
# 
# # -----------------------------------------------------------
# #   binary: darwin-amd64
# # -----------------------------------------------------------
# 
# if [ ! -e ${package}-${version}.darwin ]; then
#   echo "${package}-${version}.darwin does not exist, not creating darwin binary"
# else
# 
#   target=darwin-amd64
#   
#   mkdir -p ${package}-${version}-${target}
#   cd ${package}-${version}-${target}
#   
#   # the binary
#   cp ../${package}-${version}.darwin ./${package}
#   
#   mkdir -p share/doc
#   cp ../${package}/AUTHORS   share/doc
#   cp ../${package}/ChangeLog share/doc
#   cp ../${package}/COPYING   share/doc
#   cp ../${package}/README    share/doc
#   cp ../${package}/TODO      share/doc
#   
#   cp ../${package}/doc/fino.svg      share/doc
#   cp ../${package}/doc/fino.xml      share/doc
#   cp ../${package}/doc/reference.txt share/doc
#   cp ../${package}/doc/fino.pdf      share/doc
#   cp ../${package}/doc/fino.texi     share/doc
#   cp ../${package}/doc/fino.info     share/doc
#   
#   mkdir -p share/doc/examples
#   cp ../${package}/examples/*       share/doc/examples
#   
#   mkdir -p share/man/man1
#   cp ../${package}/doc/${package}.1 share/man/man1
#   
#   cd ..
#   tar -zcf ${package}-${version}-${target}.tgz ${package}-${version}-${target}
# fi
