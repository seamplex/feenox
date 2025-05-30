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

checkpde thermal

answerzero nafems-t2-1d.fee
exitifwrong $?

answerzero nafems-t2-3d.fee
exitifwrong $?

answerfloat nafems-t3-1d.fee 36.6 0.05
exitifwrong $?

answerfloat nafems-t3-3d.fee 36.6 0.05
exitifwrong $?
