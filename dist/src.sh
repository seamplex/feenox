#!/bin/bash -ex

if [ ! -e ../src ]; then
  echo "run from dist directory"
  exit 0
fi

if [ ! -z "${1}" ]; then
  branch="${1}"
else
  branch="main"
fi

. versions.sh

if [ "x${name}" != "xLinux" ]; then
  echo "source tarballs should be created only in GNU/Linux"
  exit 0
fi

# -----------------------------------------------------------
#   source
# -----------------------------------------------------------
current_dir=$(pwd)
tmp_dir=$(mktemp -d -t ${package}-XXXXXXXXXX)
# echo "temporary working directory: ${tmp_dir}"

git clone .. ${tmp_dir}
cd ${tmp_dir} 
 git checkout ${branch}
 ./autogen.sh --doc
 ./configure PETSC_DIR="" SLEPC_DIR="" PETSC_ARCH=""
 cd doc
  ./make.sh
#   make info
#   make pdf
  ./pdf.sh feenox-manual
  ./pdf.sh srs
  ./pdf.sh sds
 cd .. 
 make distcheck
cd ${current_dir}

if [ -e ${tmp_dir}/${package}-${version}.tar.gz ]; then
 mkdir -p src
 mv ${tmp_dir}/${package}-${version}.tar.gz src
 echo "temporary dir ${tmp_dir} not removed"
else
 echo "could not create source tarball ${tmp_dir}/${package}-${version}.tar.gz, quitting..."
 exit 1
fi
