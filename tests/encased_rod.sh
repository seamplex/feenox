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

gmsh -3 ${dir}/encased_rod.geo
answerzero pellet-linear.fee 1e-5
exitifwrong $?
