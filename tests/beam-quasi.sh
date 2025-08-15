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

checkpde mechanical

# sdef
answerzero beam-sdef-static.fee
exitifwrong $?

answerzero beam-sdef-quasi.fee
exitifwrong $?

answerzero beam-sdef-dumb.fee
exitifwrong $?

# ldef
answerzero beam-ldef-quasi-single-step.fee
exitifwrong $?

answerzero beam-ldef-quasi-two-steps.fee
exitifwrong $?

answerzero beam-ldef-quasi-many-steps.fee
exitifwrong $?

answerzero beam-ldef-quasi-pseudo.fee
exitifwrong $?
