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

checkpde laplace

answer airfoil.fee "2.42 2.49 2.48 2.51 2.59 -0.01"
exitifwrong $?
