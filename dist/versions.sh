#!/bin/false

# check for needed tools
for i in git autoconf m4 make; do
  if [ -z "$(which $i)" ]; then
    echo "error: $i not installed"
    exit 1
  fi
done

package=feenox
petsc_ver=3.16.3
slepc_ver=3.16.1

if [ -z "$(which git)" ]; then
  echo "git is not installed"
fi

if [ -z "${branch}" ]; then
  branch="main"
fi

if [ ! -e ${package} ]; then
  git clone .. ${package}
fi

cd ${package}
  git checkout ${branch}
  git pull
cd ..

cd ${package}
  ./autogen.sh
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

