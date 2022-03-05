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

# t1 and t4 are separated from t2 & t3 because they needs gmsh
checkgmsh

gmsh -2 ${dir}/nafems-t1.geo
answerfloat nafems-t1.fee 50 0.1
exitifwrong $?

gmsh -2 ${dir}/nafems-t4.geo
answer nafems-t4.fee 18.3
exitifwrong $?
