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


# t1 and t4 are separated from t2 & t3 because they needs gmsh
checkgmsh

checkpde mechanical
gmsh -v 0 -2 ${dir}/nafems-t1.geo || exit $?
answerfloat nafems-t1.fee 50 0.1
exitifwrong $?

checkpde thermal
gmsh -v 0 -2 ${dir}/nafems-t4.geo || exit $?
answer nafems-t4.fee 18.3
exitifwrong $?
