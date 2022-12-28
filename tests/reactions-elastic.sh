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

checkpde mechanical
# checkgmsh
# gmsh -3 ${dir}/cube-hex.geo || exit $?

answerdiff reaction-elastic-lr.fee
exitifwrong $?

answerdiff reaction-elastic-lt.fee
exitifwrong $?
