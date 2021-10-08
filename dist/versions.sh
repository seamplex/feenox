#!/bin/false

package=feenox
petsc_ver=3.16.0
slepc_ver=3.16.0

if [ -z "$(which git)" ]; then
  echo "git is not installed"
fi

if [ ! -e ${package} ]; then
  git clone .. ${package} || exit 1
else 
  cd ${package}
    git pull || exit 1
  cd ..
fi

cd ${package}
  ./autogen.sh || exit 1
  if [ ! -e version.m4 ]; then
    echo "error: version.m4 does not exist"
    exit 1
  fi
  version=$(echo ${package}version | m4 version.m4 -)
cd ..

name=$(uname)
if [ "x${name}" == "xLinux" ]; then
  PETSC_ARCH=linux-serial-static
  target=linux-amd64
elif [ "x${name}" == "xCYGWIN_NT-10.0" ]; then
  PETSC_ARCH=cygwin-serial-static
  target=windows64
else
  echo "unknown uname '${name}'"
  exit 1
fi

