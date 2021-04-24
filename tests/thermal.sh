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

answer thermal-two-squares-material-explicit-uniform.fee "0.75"
exitifwrong $?

answer thermal-two-squares-material-implicit-uniform.fee "0.75"
exitifwrong $?

answer thermal-two-squares-material-explicit-space.fee "0.71773"
exitifwrong $?

answer thermal-two-squares-material-implicit-space.fee "0.71773"
exitifwrong $?

answer thermal-two-squares-material-explicit-temperature.fee "0.599703"
exitifwrong $?

answer thermal-two-squares-material-implicit-temperature.fee "0.599703"
exitifwrong $?



