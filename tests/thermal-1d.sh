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

checkpetsc

answerfloat thermal-slab-uniform-nosource.fee "0"
exitifwrong $?

answerfloat thermal-slab-space-nosource.fee "0"
exitifwrong $?

answerfloat thermal-slab-temperature-nosource.fee "0"
exitifwrong $?

answerfloat thermal-slab-heat-nosource.fee "0"
exitifwrong $?

answerfloat thermal-slab-convection-nosource.fee "0"
exitifwrong $?

answerfloat thermal-slab-convection-as-heat-nosource.fee "0"
exitifwrong $?
