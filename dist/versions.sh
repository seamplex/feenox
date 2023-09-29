#!/bin/false

petsc_ver=3.20.0
slepc_ver=3.20.0
package=feenox

# check for needed tools
for i in git autoconf m4 make wget; do
  if [ -z "$(which $i)" ]; then
    echo "error: ${i} not installed"
    exit 1
  fi
done

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
  # the sed is just in case version is heads/main
  version=$(echo ${package}version | m4 version.m4 - | sed s_/_-_g)
cd ..

name=$(uname)
if [ "x${name}" == "xLinux" ]; then
  PETSC_ARCH=linux-serial-static
  target=linux-amd64
elif [ "x${name}" == "xCYGWIN_NT-10.0" ]; then
  PETSC_ARCH=cygwin-serial-static
  target=windows64
else
  echo "error: unknown uname '${name}'"
  exit 1
fi

