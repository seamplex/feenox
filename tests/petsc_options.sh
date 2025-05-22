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

i=petsc_options
echo -n "${i}... "
answer=$(${feenox} ${dir}/${i}.fee | grep ilu | wc -l)
if [ ${answer} -gt 0 ]; then
  echo "ok"
  level=0
else
  echo "wrong, no ILU found"
  level=1
fi
exitifwrong ${level}





