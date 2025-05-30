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

echo -n "jacobian ... "
${feenox} ${dir}/hex8-ldef.fee --snes_test_jacobian | grep "||J - Jfd||_F/||J||_F =" | tr -d , | awk '{d+=$5}END{exit d>1e-5}' 
exitifwrong $?
echo "ok"
