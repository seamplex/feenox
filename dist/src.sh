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
tmp_dir=$(mktemp -d -t ${package}-XXXXXXXXXX)
git clone .. ${tmp_dir}
current_dir=$(pwd)

cd ${tmp_dir}
 ./autogen.sh --doc || exit 1
 ./configure PETSC_DIR="" SLEPC_DIR="" PETSC_ARCH="" || exit 1
 cd doc
  ./make.sh || exit 1
  ./pdf.sh feenox-manual
  make pdf 
 cd .. 
 make distcheck || exit 1
cd ${current_dir}

if [ -e ${tmp_dir}/${package}-${version}.tar.gz ]; then
 mkdir -p src
 mv ${tmp_dir}/${package}-${version}.tar.gz src
 echo "temporary dir ${tmp_dir} not removed"
else
 echo "could not create source tarball, quitting"
 exit 1
fi
