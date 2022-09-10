#!/bin/bash -ex

if [ ! -e ../src ]; then
  echo "run from dist directory"
  exit 0
fi

. versions.sh

if [ ! -e petsc-${petsc_ver} ]; then
  echo "PETSc directory petsc-${petsc_ver} does not exist"
  exit 1
fi
if [ ! -e petsc-${petsc_ver}/${PETSC_ARCH} ]; then
  echo "PETSc architecture petsc-${petsc_ver}/${PETSC_ARCH} does not exist"
  exit 1
fi
if [ ! -e petsc-${petsc_ver} ]; then
  echo "SLEPc directory slepc-${slepc_ver} does not exist"
  exit 1
fi

export PETSC_DIR=$(pwd)/petsc-${petsc_ver}
export SLEPC_DIR=$(pwd)/slepc-${slepc_ver}
cd ${package}
  ./configure PETSC_DIR=${PETSC_DIR} SLEPC_DIR=${SLEPC_DIR} PETSC_ARCH=${PETSC_ARCH} \
              --enable-download-gsl CFLAGS="-O3 -flto -DLD_STATIC" LDFLAGS="-flto=auto -static" || exit 1

  if [ "x${target}" = "xlinux-amd64" ]; then
    cd doc
      ./make.sh || exit 1
      make info || exit 1
      make pdf  || exit 1
    cd .. 
  fi 
  make || exit 1
cd ..

mkdir -p ${package}-${version}-${target} || exit 1
cd ${package}-${version}-${target} || exit 1

  mkdir -p bin || exit 1
  mkdir -p share/doc/examples || exit 1
  mkdir -p share/doc/tests || exit 1

  if [ "x${target}" = "xlinux-amd64" ]; then
  
    mkdir -p share/man/man1 || exit 1
    cp ../${package}/doc/${package}.1 share/man/man1 || exit 1
    gzip -f share/man/man1/${package}.1 || exit 1
    
    cp ../${package}/${package} bin || exit 1

    cp ../${package}/AUTHORS   share/doc || exit 1
    cp ../${package}/ChangeLog share/doc || exit 1
    cp ../${package}/COPYING   share/doc || exit 1
    cp ../${package}/README    share/doc || exit 1
    cp ../${package}/TODO      share/doc || exit 1

    cp ../${package}/README.pdf                  share/doc || exit 1
    cp ../${package}/doc/${package}-manual.pdf   share/doc || exit 1
    cp ../${package}/doc/${package}-desc.pdf     share/doc || exit 1
    cp ../${package}/doc/${package}-desc.info    share/doc || exit 1
    cp ../${package}/doc/programming.pdf         share/doc || exit 1
    cp ../${package}/doc/compilation.pdf         share/doc || exit 1
    cp ../${package}/doc/FAQ.pdf                 share/doc || exit 1
    cp ../${package}/doc/CODE_OF_CONDUCT.pdf     share/doc || exit 1
    
  elif [ "x${target}" = "xwindows64" ]; then   
  
    cp ../${package}/${package}.exe bin || exit 1
    cp /bin/cygwin1.dll bin || exit 1
    
    # -n means "new file"
    unix2dos -n ../${package}/AUTHORS   AUTHORS.txt || exit 1
    unix2dos -n ../${package}/ChangeLog ChangeLog.txt || exit 1
    unix2dos -n ../${package}/COPYING   COPYING.txt || exit 1
    unix2dos -n ../${package}/README    README.txt || exit 1
    unix2dos -n ../${package}/TODO      TODO.txt || exit 1
    unix2dos -n ../${package}/TODO      TODO.txt || exit 1
    
    unix2dos -n ../${package}/doc/double-click.txt why-nothing-happens-when-I-double-click-on-feenox.exe.txt
    
  fi

  cp ../${package}/doc/${package}.xml        share/doc || exit 1
  cp ../${package}/doc/${package}-desc.texi  share/doc || exit 1
  cp ../${package}/doc/syntax-kate.sh        share/doc || exit 1
  cp ../${package}/doc/syntax-tex.sh         share/doc || exit 1
  
  cp -rL ../${package}/examples/             share/doc || exit 1
  echo "See https://www.seamplex.com/feenox/examples" > share/doc/examples/README || exit 1
  cp -rL ../${package}/tests/                share/doc || exit 1
  echo "See https://github.com/seamplex/feenox/tree/main/tests" > share/doc/tests/README || exit 1
  
cd ..

if [ ! -e ${target} ]; then
  mkdir ${target}
fi

# TODO: mac
if [ "x${target}" = "xlinux-amd64" ]; then
  tar -zcf ${package}-${version}-${target}.tar.gz ${package}-${version}-${target}
  mv ${package}-${version}-${target}.tar.gz ${target}
elif [ "x${target}" = "xwindows64" ]; then   
  zip -r   ${package}-${version}-${target}.zip    ${package}-${version}-${target}
  mv ${package}-${version}-${target}.zip    ${target}
fi

