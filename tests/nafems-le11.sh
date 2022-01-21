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
checkgmsh

for i in ${dir}/nafems-le11-*.geo; do
 if [ -e ${i} ]; then
   gmsh -3 ${i}
 else
   exit 77
 fi
done

answer1 nafems-le11.fee tet4 "-108.2"
exitifwrong $?

answer1 nafems-le11.fee tet10 "-105.9"
exitifwrong $?

answer1 nafems-le11.fee hex8 "-99.3"
exitifwrong $?

answer1 nafems-le11.fee hex20 "-102.4"
exitifwrong $?

answer1 nafems-le11.fee hex27 "-104.8"
exitifwrong $?
