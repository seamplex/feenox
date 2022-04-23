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

checkpde mechanical

answer cube-tangential.fee "0.234568 0.123457 -0.345679"
exitifwrong $?

answer cube-radial.fee "0.123457 0.000000 0.123457"
exitifwrong $?

