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
answerzero la-p01-PUa-1-0-IN.fee
exitifwrong $?

gmsh -1 ${dir}/la-p02-PUa-1-0-SL.geo || exit $?
answerzero la-p02-PUa-1-0-SL.fee
exitifwrong $?

gmsh -1 ${dir}/la-p03-PUa-H2O_1-1-0-SL.geo || exit $?
answerzero la-p03-PUa-H2O_1-1-0-SL.fee
exitifwrong $?

gmsh -1 ${dir}/la-p04-PUa-H2O_0.5-1-0-SL.geo || exit $?
answerzero la-p04-PUa-H2O_0.5-1-0-SL.fee
exitifwrong $?

answerzero la-p05-PUb-1-0-IN.fee
exitifwrong $?

gmsh -1 ${dir}/la-p06-PUb-1-0-SL.geo || exit $?
answer la-p06-PUb-1-0-SL.fee "0.995 0.969 0.876 0.719 0.492"
exitifwrong $?
        
gmsh -2 ${dir}/la-p07-PUb-1-0-CY.geo || exit $?
answer la-p07-PUb-1-0-CY.fee "0.996 0.808 0.310"
exitifwrong $?

gmsh -3 ${dir}/la-p08-PUb-1-0-SP.geo || exit $?
answer la-p08-PUb-1-0-SP.fee "1.004 0.909 0.742 0.495 0.191"
exitifwrong $?

gmsh -2 ${dir}/la-p09-PUb-H2O_1-1-0-CY.geo || exit $?
answerzero la-p09-PUb-H2O_1-1-0-CY.fee
exitifwrong $?

gmsh -2 ${dir}/la-p10-PUb-H2O_10-1-0-CY.geo || exit $?
answerzero la-p10-PUb-H2O_10-1-0-CY.fee
exitifwrong $?
