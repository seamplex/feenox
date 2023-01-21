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

checkgmsh

gmsh -v 0 -3 -clmax 0.25 ${dir}/map-cube.geo -o map-cube-4.msh  || exit $?
gmsh -v 0 -3 -clmax 0.10 ${dir}/map-cube.geo -o map-cube-10.msh || exit $?

${feenox} ${dir}/map-create.fee 4 || exit $?
${feenox} ${dir}/map-create.fee 10 || exit $?

answerzero2 map-interpolate.fee 4 10 1e-2
exitifwrong $?

answerzero2 map-interpolate.fee 10 4 
exitifwrong $?


