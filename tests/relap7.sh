#!/bin/sh
for i in . tests; do
  if [ -e ${i}/functions.sh ]; then
    . ${i}/functions.sh 
  fi
done
if [ -z "${functions_found}" ]; then
  echo "could not find functions.sh"
   exit 1
fi

checkpde thermal_hydraulics
checkgmsh

gmsh -v 0 -1 ${dir}/relap7-pipe.geo || exit $?
answerzero relap7-pipe.fee 1e-4
exitifwrong $?
