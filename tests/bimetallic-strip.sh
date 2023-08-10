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
checkmumps

checkgmsh
gmsh -v 0 -3 ${dir}/bimetallic-strip.geo || exit $?

answerzero1 bimetallic-strip.fee --mumps
exitifwrong $?
