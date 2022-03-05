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

answer cylinder-force.fee "0.287"
exitifwrong $?

answer cylinder-traction.fee "0.287"
exitifwrong $?
