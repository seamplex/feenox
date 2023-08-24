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

checkpde neutron_diffusion
checkpde neutron_sn
checkgmsh

gmsh -v 0 -2 ${dir}/square-struct.geo || exit $?

answerzero diffusion-square-fully-mirrored.fee
exitifwrong $?

answerzero sn-square-fully-mirrored.fee
exitifwrong $?
