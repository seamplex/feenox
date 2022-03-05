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

checkpde mechanical
checkgmsh

gmsh -3 ${dir}/i-beam-hex.geo
gmsh -3 ${dir}/i-beam-tet.geo

answer2 i-beam-euler-bernoulli.fee right tet "+3e-02"
exitifwrong $?

answer2 i-beam-euler-bernoulli.fee right hex "+3e-02"
exitifwrong $?

answer2 i-beam-euler-bernoulli.fee top tet "+4e-02"
exitifwrong $?

answer2 i-beam-euler-bernoulli.fee top hex "+4e-02"
exitifwrong $?
