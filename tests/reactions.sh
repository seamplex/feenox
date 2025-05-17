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

checkpde mechanical

answerdiff reaction-displ.fee
exitifwrong $?

answerdiff reaction-force.fee
exitifwrong $?
