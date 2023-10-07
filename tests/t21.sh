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
checkmpirun

for n in $(seq 1 8); do
  answermpi ${n} t21.fee "2 2 2 2 2"
  exitifwrong $?
done
