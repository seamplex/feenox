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

gmsh -v 0 -2 ${dir}/azmy-structured.geo || exit $?

answer1zero azmy-structured.fee 8 0.6
exitifwrong $?

answer1zero azmy-structured.fee 12 0.6
exitifwrong $?
