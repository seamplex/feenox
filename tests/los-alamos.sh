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

# skip in problematic architectures
arch=$(uname -m)
if [ "x${arch}" = "xalpha" ] || [ "x${arch}" = "xs390x" ] ; then
  exit 77
fi

checkgmsh
checkslepc
checkpde neutron_sn

# one group
gmsh -v 0 -1 ${dir}/la-IN.geo || exit $?
answerzero la-p01-PUa-1-0-IN.fee
exitifwrong $?

gmsh -v 0 -1 ${dir}/la-p02-PUa-1-0-SL.geo || exit $?
answerzero la-p02-PUa-1-0-SL.fee
exitifwrong $?

gmsh -v 0 -1 ${dir}/la-p03-PUa-H2O_1-1-0-SL.geo || exit $?
answerzero la-p03-PUa-H2O_1-1-0-SL.fee
exitifwrong $?

gmsh -v 0 -1 ${dir}/la-p04-PUa-H2O_0.5-1-0-SL.geo || exit $?
answerzero la-p04-PUa-H2O_0.5-1-0-SL.fee
exitifwrong $?

answerzero la-p05-PUb-1-0-IN.fee
exitifwrong $?

gmsh -v 0 -1 ${dir}/la-p06-PUb-1-0-SL.geo || exit $?
answer la-p06-PUb-1-0-SL.fee "0.99 0.97 0.88 0.72 0.49"
exitifwrong $?
        
gmsh -v 0 -2 ${dir}/la-p07-PUb-1-0-CY.geo || exit $?
answer la-p07-PUb-1-0-CY.fee "1.00 0.81 0.30"
exitifwrong $?

gmsh -v 0 -3 ${dir}/la-p08-PUb-1-0-SP.geo || exit $?
answer la-p08-PUb-1-0-SP.fee "1.0 0.9 0.7 0.5 0.2"
exitifwrong $?

gmsh -v 0 -2 ${dir}/la-p09-PUb-H2O_1-1-0-CY.geo || exit $?
answerzero la-p09-PUb-H2O_1-1-0-CY.fee
exitifwrong $?

gmsh -v 0 -2 ${dir}/la-p10-PUb-H2O_10-1-0-CY.geo || exit $?
answerzero la-p10-PUb-H2O_10-1-0-CY.fee
exitifwrong $?


# two groups
answerzero la-p47-U-2-0-IN.fee
exitifwrong $?

gmsh -v 0 -1 ${dir}/la-p48-U-2-0-SL.geo || exit $?
answerzero la-p48-U-2-0-SL.fee
exitifwrong $?

gmsh -v 0 -3 ${dir}/la-p49-U-2-0-SP.geo || exit $?
answerzero la-p49-U-2-0-SP.fee 2e-3
exitifwrong $?

answerzero la-p50-UAl-2-0-IN.fee
exitifwrong $?

gmsh -v 0 -1 ${dir}/la-p51-UAl-2-0-SL.geo || exit $?
answerzero la-p51-UAl-2-0-SL.fee
exitifwrong $?

# this one fails with out-of-memory in github actions
# gmsh -v 0 -3 ${dir}/la-p52-UAl-2-0-SP.geo || exit $?
# answerzero la-p52-UAl-2-0-SP.fee
# exitifwrong $?


answerzero la-p70-URRa-2-1-IN.fee
exitifwrong $?

# this one gives a different solution in github actions
# gmsh -v 0 -1 ${dir}/la-p71-URRa-2-1-SL.geo || exit $?
# answerzero la-p71-URRa-2-1-SL.fee
# exitifwrong $?
