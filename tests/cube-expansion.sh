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

checkpetsc
checkgmsh

gmsh -3 ${dir}/cube.geo

answerzero cube-free-expansion-uniform-isotropic.fee
exitifwrong $?

answerzero cube-restrained-expansion-uniform-isotropic.fee
exitifwrong $?
