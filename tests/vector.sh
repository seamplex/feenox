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

answer vector.fee "196"
exitifwrong $?

answer vector_init.fee "1 2 3 4"
exitifwrong $?

answerdiff fibo_vector.fee
exitifwrong $?

answer stats.fee "0.5500 1.4694 1.2122"
exitifwrong $?
