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

checkgmsh
checkslepc
checkpde neutron_diffusion

gmsh -v 0 -2 ${dir}/2dpwr-quarter.geo || exit $?
answer1 2dpwr.fee quarter "1.0298"
exitifwrong $?

gmsh -v 0 -2 ${dir}/2dpwr-eighth.geo || exit $?
answer1 2dpwr.fee eighth  "1.0297"
exitifwrong $?
