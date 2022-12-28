#!/bin/sh -e
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
checkpde modal

gmsh -3 ${dir}/annulus.geo || exit $?
answerdiff annulus-modal.fee
exitifwrong $?
