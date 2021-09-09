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

answer ud20-1-0-sl-src.fee "0.474 0.495"
exitifwrong $?

answer reflected-src.fee "0.6688 0.7116"
exitifwrong $?
