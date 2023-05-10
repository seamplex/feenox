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

answerdiff expressions_variables.fee
exitifwrong $?

answer expressions_functions.fee "1.5"
exitifwrong $?

answerdiff expressions_cumbersome.fee
exitifwrong $?
