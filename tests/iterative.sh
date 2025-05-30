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

answerdiff fibo_iterative.fee
exitifwrong $?

answerdiff iterative.fee
exitifwrong $?

answer geometric_series.fee "2"
exitifwrong $?
