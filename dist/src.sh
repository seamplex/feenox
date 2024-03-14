#!/bin/bash -ex

# check for needed tools
for i in pandoc pandoc-crossref feenox; do
  if [ -z "$(which ${i})" ]; then
    echo "error: ${i} not installed"
    exit 1
  fi
done

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
  echo "error: source tarballs should be created only in GNU/Linux"
  exit 0
fi

# -----------------------------------------------------------
#   source
# -----------------------------------------------------------
export PETSC_DIR=""
export PETSC_ARCH=""
export SLEPC_DIR=""

current_dir=$(pwd)
tmp_dir=$(mktemp -d -t ${package}-XXXXXXXXXX)
echo "temporary working directory: ${tmp_dir}"

git clone .. ${tmp_dir}
cd ${tmp_dir} 
 git checkout ${branch}
 ./autogen.sh --doc
#  ./autogen.sh
 ./configure --without-sundials --without-petsc
 cd doc
  ./make.sh --no-pdf
 cd .. 
 make distcheck
cd ${current_dir}

if [ -e ${tmp_dir}/${package}-${version}.tar.gz ]; then
 mkdir -p src
 mv ${tmp_dir}/${package}-${version}.tar.gz src
 echo "success: temporary dir ${tmp_dir} not removed"
else
 echo "error: could not create source tarball ${tmp_dir}/${package}-${version}.tar.gz, quitting..."
 exit 1
fi
