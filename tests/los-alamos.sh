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
checkslepc
checkpde neutron_transport

gmsh -1 ${dir}/la-IN.geo || exit $?
answerzero la-p1-PUa-1-0-IN.fee
exitifwrong $?

gmsh -1 ${dir}/la-p2-PUa-1-0-SL.geo || exit $?
answerzero la-p2-PUa-1-0-SL.fee
exitifwrong $?

gmsh -1 ${dir}/la-p3-PUa-H2O_1-1-0-SL.geo || exit $?
answerzero la-p3-PUa-H2O_1-1-0-SL.fee
exitifwrong $?

gmsh -1 ${dir}/la-p4-PUa-H2O_0.5-1-0-SL.geo || exit $?
answerzero la-p4-PUa-H2O_0.5-1-0-SL.fee
exitifwrong $?

answerzero la-p5-PUb-1-0-IN.fee
exitifwrong $?

gmsh -1 ${dir}/la-p6-PUb-1-0-SL.geo || exit $?
answerzero la-p6-PUb-1-0-SL.fee
exitifwrong $?
        
gmsh -2 ${dir}/la-p7-PUb-1-0-CY.geo || exit $?
answerzero la-p7-PUb-1-0-CY.fee
exitifwrong $?

gmsh -3 ${dir}/la-p8-PUb-1-0-SP.geo || exit $?
answerzero la-p8-PUb-1-0-SP.fee
exitifwrong $?

