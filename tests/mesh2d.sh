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

answer1 mesh2d.fee 22 "12"
exitifwrong $?

answer1 mesh2d.fee 40 "12"
exitifwrong $?

answer1 mesh2d.fee 41 "12"
exitifwrong $?
