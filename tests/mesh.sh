#!/bin/sh
for i in . tests; do
  if [ -e ${i}/functions.sh ]; then
    . ${i}/functions.sh 
  fi
done
if [ -z "${functions_found}" ]; then
  echo "could not find functions.sh"
  exit 1;
fi

# skip in big-endian architectures
arch=$(uname -m)
if [ "x${arch}" = "xppc64" ] || [ "x${arch}" = "xs390x" ] ; then
  exit 77
fi

answer1 read_mesh2d.fee 22.msh "12 22 14"
exitifwrong $?

answer1 read_mesh2d.fee 40.msh "12 22 14"
exitifwrong $?

answer1 read_mesh2d.fee 41.msh "12 22 14"
exitifwrong $?

answer1 read_mesh2d.fee 41bin.msh "12 22 14"
exitifwrong $?

answer1 read_mesh2d.fee .vtk "12 22 14"
exitifwrong $?

for i in msh vtk; do
  answer1 write_mesh2d.fee ${i} "0.531905"
  exitifwrong $?

  answer1 integrate2d.fee ${i} "0.5309834 0.5309834 0.3130105 0.3101734"
  exitifwrong $? 

  answer1diff find_extrema2d.fee ${i}
  exitifwrong $? 
done

answer mesh3d.fee "50"
exitifwrong $?

answerzero read_vtk5.fee
exitifwrong $?

answer readmsh_writevtk_readbackvtk.fee ""
exitifwrong $?
