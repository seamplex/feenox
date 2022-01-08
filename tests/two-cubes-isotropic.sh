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

gmsh -3 ${dir}/two-cubes.geo

answer two-cubes-isotropic-variables.fee "10.2"
exitifwrong $?

answer two-cubes-isotropic-functions.fee "10.2"
exitifwrong $?

answer two-cubes-isotropic-materials.fee "10.2"
exitifwrong $?

