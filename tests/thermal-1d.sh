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

answerzero thermal-slab-uniform-nosource.fee
exitifwrong $?

answerzero thermal-slab-space-nosource.fee
exitifwrong $?

answerzero thermal-slab-temperature-nosource.fee
exitifwrong $?

answerzero thermal-slab-heat-nosource.fee
exitifwrong $?

answerzero thermal-slab-convection-nosource.fee
exitifwrong $?

answerzero thermal-slab-convection-as-heat-nosource.fee
exitifwrong $?

answerzero thermal-slab-transient.fee 1e-4
exitifwrong $?
