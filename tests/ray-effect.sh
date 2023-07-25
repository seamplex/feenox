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
checkpde neutron_sn

gmsh -2 ${dir}/ray-effect.geo || exit $?
answer ray-effect.fee 1.37095
exitifwrong $?

gmsh -2 ${dir}/ray-effect-full.geo || exit $?
answer ray-effect-full.fee 1.37098
exitifwrong $?
