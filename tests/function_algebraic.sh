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

answer function_algebraic.fee "4	37"
exitifwrong $?

answerdiff fibo_formula.fee
exitifwrong $?
