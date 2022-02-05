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

gmsh -v 0 -3 ${dir}/parallelepiped.geo -order 2
answerzero parallelepiped.fee
exitifwrong $?

gmsh -v 0 -3 ${dir}/parallelepiped.geo -order 1 -clscale 2 -o parallelepiped-coarse.msh
answerzero parallelepiped-thermal.fee
exitifwrong $?

answerzero parallelepiped-from-msh.fee
exitifwrong $?
