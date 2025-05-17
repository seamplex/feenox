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

checkida

answer lag.fee "0.16"
exitifwrong $?

answer lag_compact.fee "0.16"
exitifwrong $?

# check TIME_PATH
echo -n "time_path_raw.fee ... "

if [ "x$(${feenox} ${dir}/time_path_raw.fee | wc -l)" != "x5" ]; then
  echo "failed"
  return 1
fi
echo "ok"
