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

answer function_file1d.fee "6.5"
exitifwrong $?

answer function_file1d_columns.fee "6.5"
exitifwrong $?
