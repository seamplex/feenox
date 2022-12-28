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

checkpde thermal
checkgmsh

gmsh -1 ${dir}/wilson-1d.geo || exit $?
answerzero wilson-1d.fee
exitifwrong $?

gmsh -2 ${dir}/wilson-2d.geo || exit $?
answerzero wilson-2d.fee 2e-2
exitifwrong $?
