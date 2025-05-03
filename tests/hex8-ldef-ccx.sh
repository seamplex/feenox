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

if [ -z "$(which ccx)" ]; then
  exit 77
fi

ccx -i hex8-ldef > hex8-ldef.txt
${feenox} ${dir}/hex8-ldef.fee

answerzero hex8-ldef-diff.fee
exitifwrong $?
