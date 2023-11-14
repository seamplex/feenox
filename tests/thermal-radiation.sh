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

checkpde thermal

answerzero radiation-as-heatflux-celsius.fee 1e-2
exitifwrong $?

answerzero radiation-as-heatflux-kelvin.fee 1e-2
exitifwrong $?

answerzero radiation-as-convection-celsius.fee 1e-2
exitifwrong $?

answerzero radiation-as-convection-kelvin.fee 1e-2
exitifwrong $?
