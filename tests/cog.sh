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

gmsh -3 ${dir}/cube.geo || exit $?

answerzero cube-cog.fee
exitifwrong $?

