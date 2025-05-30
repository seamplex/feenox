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

${feenox} ${dir}/file.fee
diff two-is-2.txt ${dir}/two-is-2.ref
exitifwrong $?

${feenox} ${dir}/file-append.fee
diff two-is-2.txt ${dir}/two-is-3.ref
exitifwrong $?

