#!/bin/bash -ex

# check for needed tools
for i in lsb_release apt-cache xargs md5sum dpkg-deb tar gzip; do
  if [ -z "$(which ${i})" ]; then
    echo "error: ${i} not installed"
    exit 1
  fi
done


if [ ! -e ../tests ]; then
  echo "error: run from dist directory"
  exit 1
fi

. versions.sh

if [ "x${name}" != "xLinux" ]; then
  echo "error: debian packages should be created only in GNU/Linux"
  exit 1
fi


# -----------------------------------------------------------
#   debian package
# -----------------------------------------------------------
codename=$(lsb_release -a | grep Codename | cut -d: -f2 | xargs)
deb_version=1
dir=${package}-${version}_${deb_version}_amd64
rm -rf ${dir}

# DEBIAN/control
mkdir -p ${dir}/DEBIAN

# check the names of the dependencies
libgsl=$(apt-cache search libgsl | grep "library package" | grep -v blas | cut -d" " -f1)
libsundials=$(apt-cache search libsundials-ida | grep -v idas | cut -d" " -f1)
libpetsc=$(apt-cache search libpetsc-real3 | grep -v 64 | grep -v dbg | grep -v dev | cut -d" " -f1)
libslepc=$(apt-cache search libslepc-real3 | grep -v 64 | grep -v dbg | grep -v dev | cut -d" " -f1)

cat << EOF >> ${dir}/DEBIAN/control
Package: ${package}
Version: ${version}-${deb_version}
Architecture: amd64
Maintainer: Jeremy Theler <jeremy@seamplex.com>
Depends: libc6, ${libgsl}, ${libsundials}, ${libpetsc}, ${libslepc}
Recommends: gmsh, paraview
Section: science
Priority: optional
Homepage: https://www.seamplex.com/feenox
Description: cloud-first free no-X uniX-like finite-element(ish) tool
 FeenoX can be seen either as
  * a syntactically-sweetened way of asking the computer to solve
    engineering-related mathematical problems, and/or
  * a finite-element(ish) tool with a particular design basis.
 Note that some of the problems solved with FeenoX might not actually rely
 on the finite element method, but on general mathematical models and even on
 the finite volumes method. That is why we say it is a finite-element(ish) tool.
EOF

# -----------------------------------------------------------
#   doc
# -----------------------------------------------------------
if [ ! -e src/${package}-${version}.tar.gz ]; then
  # try to download source
  for i in wget; do
    if [ -z "$(which ${i})" ]; then
      echo "error: ${i} not installed"
      exit 1
    fi
  done
  
  mkdir -p src
  cd src
    wget https://seamplex.com/feenox/dist/src/feenox-${version}.tar.gz 
  cd ..  
fi
if [ ! -e src/${package}-${version}.tar.gz ]; then
  echo "error: cannot find src/${package}-${version}.tar.gz, run ./src.sh first"
  exit 1
fi



rm -rf ${package}-${version} 
tar xvzf src/${package}-${version}.tar.gz

mkdir -p ${dir}/usr/share/doc/feenox
cp ${package}-${version}/README            ${dir}/usr/share/doc/feenox
cp ${package}-${version}/ChangeLog         ${dir}/usr/share/doc/feenox/changelog
cp ${package}-${version}/doc/README-doc    ${dir}/usr/share/doc/feenox
cp ${package}-${version}/doc/FAQ           ${dir}/usr/share/doc/feenox
cp ${package}-${version}/doc/feenox-manual ${dir}/usr/share/doc/feenox
cp ${package}-${version}/doc/sds           ${dir}/usr/share/doc/feenox
cp ${package}-${version}/doc/srs           ${dir}/usr/share/doc/feenox
cp ${package}-${version}/doc/srs           ${dir}/usr/share/doc/feenox
cp ${package}-${version}/doc/feenox.xml    ${dir}/usr/share/doc/feenox
cp ${package}-${version}/doc/fee.vim       ${dir}/usr/share/doc/feenox
cp README.Debian                           ${dir}/usr/share/doc/feenox
cp changelog.Debian                        ${dir}/usr/share/doc/feenox
cp copyright                               ${dir}/usr/share/doc/feenox

gzip -9n ${dir}/usr/share/doc/feenox/changelog.Debian
gzip -9n ${dir}/usr/share/doc/feenox/changelog
gzip -9n ${dir}/usr/share/doc/feenox/feenox-manual
gzip -9n ${dir}/usr/share/doc/feenox/sds
gzip -9n ${dir}/usr/share/doc/feenox/srs


mkdir -p ${dir}/usr/share/man/man1
cp ${package}-${version}/doc/feenox.1      ${dir}/usr/share/man/man1
gzip -9n ${dir}/usr/share/man/man1/feenox.1




# -----------------------------------------------------------
#   compile binary
# -----------------------------------------------------------
mkdir -p ${dir}/usr/bin

PETSC_DIR=/usr/lib/petsc
SLEPC_DIR=/usr/lib/slepc
if [ ! -d ${PETSC_DIR} ]; then
  echo "error: ${PETSC_DIR} does not exist, install libpetsc-real-dev"
  exit 1
fi

if [ ! -d ${SLEPC_DIR} ]; then
  echo "error: ${SLEPC_DIR} does not exist, install libslepc-real-dev"
  exit 1
fi

cd ${package}-${version}
  # this command will fail if the dependencies are not there
  # debian policy requires PIE so we have to remove the default -no-pie flag
  ./configure --with-sundials --with-petsc --with-slepc PETSC_DIR=${PETSC_DIR} SLEPC_DIR=${SLEPC_DIR} PETSC_ARCH="" CFLAGS="-O3 -flto=auto" || exit 1
  make
cd ..

cp ${package}-${version}/src/feenox ${dir}/usr/bin
# strip the binary as required by the debian policy
strip --strip-unneeded --remove-section=.comment --remove-section=.note ${dir}/usr/bin/feenox

# -----------------------------------------------------------
#   md5 sums
# -----------------------------------------------------------
cd ${dir}
 find usr -type f | xargs md5sum > DEBIAN/md5sums
cd ..

# -----------------------------------------------------------
#   deb package
# -----------------------------------------------------------
# TODO: use dpkg-buildpackage
dpkg-deb --root-owner-group --build ${dir}
mkdir -p deb/${codename}
mv *.deb deb/${codename}
