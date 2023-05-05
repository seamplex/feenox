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

gmsh -v 0 -1 ${dir}/reed.geo || exit $?

answer1zero reed.fee 2 0.08
exitifwrong $?

answer1zero reed.fee 4 0.02
exitifwrong $?

answer1zero reed.fee 6 0.015
exitifwrong $?

answer1zero reed.fee 8 0.015
exitifwrong $?
