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

checkpde mechanical
checkgmsh

gmsh -3 ${dir}/cube.geo || exit $?

answerzero cube-free-expansion-uniform-isotropic.fee
exitifwrong $?

answerzero cube-restrained-expansion-uniform-isotropic.fee
exitifwrong $?

answerzero cube-free-expansion-uniform-orthotropic.fee
exitifwrong $?

answerzero cube-restrained-expansion-uniform-orthotropic.fee
exitifwrong $?

answerzero cube-free-expansion-alpha-of-space-orthotropic.fee
exitifwrong $?

answer cube-free-expansion-alpha-of-temperature-orthotropic.fee "3.356e-03 3.353e-03 3.340e-03"
exitifwrong $?
