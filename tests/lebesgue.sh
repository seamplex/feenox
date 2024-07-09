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

answer1 single-arc.fee 1   "-1.6e-01"
exitifwrong $?

answer1 single-arc.fee 2   "-8.7e-03"
exitifwrong $?

