#!/bin/bash -ex

if [ ! -e ../tests ]; then
  echo "error: run from dist directory"
  exit 1
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
              --enable-fee2ccx --enable-download-gsl CFLAGS="-O3 -flto" LDFLAGS="-flto"

  if [ "x${target}" = "xlinux-amd64" ]; then
    cd doc
      ./make.sh
    cd ..
  fi
  make

  cd utils
  ln -sf ../gsl-2.8
  cd fee2ccx
  make
  cd ../..

cd ..

mkdir -p ${package}-${version}-${target}
cd ${package}-${version}-${target}

  mkdir -p bin
  mkdir -p share/doc/${package}/examples
  mkdir -p share/doc/${package}/tests

  if [ "x${target}" = "xlinux-amd64" ]; then
  
    mkdir -p share/man/man1
    cp ../${package}/doc/${package}.1 share/man/man1
    gzip -f share/man/man1/${package}.1
    
    cp ../${package}/${package} bin
    cp ../${package}/utils/fee2ccx/fee2ccx bin

    cp ../${package}/AUTHORS   share/doc/${package}
    cp ../${package}/ChangeLog share/doc/${package}
    cp ../${package}/COPYING   share/doc/${package}
    cp ../${package}/README    share/doc/${package}
    cp ../${package}/TODO      share/doc/${package}

    cp ../${package}/doc/${package}-manual       share/doc/${package}
    cp ../${package}/doc/programming             share/doc/${package}
    cp ../${package}/doc/compilation             share/doc/${package}
    cp ../${package}/doc/FAQ                     share/doc/${package}
    cp ../${package}/doc/CODE_OF_CONDUCT         share/doc/${package}

  elif [ "x${target}" = "xwindows64" ]; then   

    cp ../${package}/${package}.exe bin
    cp /bin/cygwin1.dll bin

    # -n means "new file"
    unix2dos -n ../${package}/AUTHORS   AUTHORS.txt
    unix2dos -n ../${package}/ChangeLog ChangeLog.txt
    unix2dos -n ../${package}/COPYING   COPYING.txt
    unix2dos -n ../${package}/README    README.txt
    unix2dos -n ../${package}/TODO      TODO.txt
    unix2dos -n ../${package}/TODO      TODO.txt
    unix2dos -n ../${package}/doc/double-click.txt why-nothing-happens-when-I-double-click-on-feenox.exe.txt
  fi

  cp ../${package}/doc/${package}.xml        share/doc/${package}
  cp ../${package}/doc/fee.vim               share/doc/${package}
  cp ../${package}/doc/${package}-desc.texi  share/doc/${package}
  echo "See https://www.seamplex.com/feenox/examples" > share/doc/${package}/examples/README
  echo "See https://github.com/seamplex/feenox/tree/main/tests" > share/doc/${package}/tests/README
  
cd ..

if [ ! -e ${target} ]; then
  mkdir ${target}
fi

# TODO: mac
if [ "x${target}" = "xlinux-amd64" ]; then
  tar -zcf ${package}-${version}-${target}.tar.gz ${package}-${version}-${target}
  cp ${package}-${version}-${target}.tar.gz ${package}-${target}.tar.gz
  mv ${package}-${version}-${target}.tar.gz ${target}
elif [ "x${target}" = "xwindows64" ]; then   
  zip -r   ${package}-${version}-${target}.zip    ${package}-${version}-${target}
  mv ${package}-${version}-${target}.zip    ${target}
fi

