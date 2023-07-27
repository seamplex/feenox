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

gmsh -v 0 -2 ${dir}/ray-effect-full.geo || exit $?
answer2 ray-effect.fee full 4 "1.381"
exitifwrong $?

gmsh -v 0 -2 ${dir}/ray-effect-half.geo || exit $?
answer2 ray-effect.fee half 4 "1.381"
exitifwrong $?

gmsh -v 0 -2 ${dir}/ray-effect-quarter.geo || exit $?
answer2 ray-effect.fee quarter 4 "1.381"
exitifwrong $?

gmsh -v 0 -2 ${dir}/ray-effect-eighth.geo || exit $?
answer2 ray-effect.fee eighth 4 "1.385"
exitifwrong $?
