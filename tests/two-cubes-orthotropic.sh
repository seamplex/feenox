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

gmsh -3 ${dir}/two-cubes.geo || exit $?

answer two-cubes-orthotropic-variables.fee "9.9"
exitifwrong $?

answer two-cubes-orthotropic-functions.fee "9.9"
exitifwrong $?

answer two-cubes-orthotropic-materials.fee "9.9"
exitifwrong $?

