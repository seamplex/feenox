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

answer airfoil.fee "2.4 2.5 2.5 2.5 2.6 -0.0"
exitifwrong $?
