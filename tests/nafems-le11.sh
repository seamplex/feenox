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

for i in ${dir}/nafems-le11-*.geo; do
 if [ -e ${i} ]; then
   gmsh -v 0 -3 ${i} || exit $?
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



answer1 warp.fee nafems-le11-tet4  "6.332e+01 1.000e+00 6.232e-04"
exitifwrong $?

answer1 warp.fee nafems-le11-tet10 "6.556e+01 1.000e+00 6.456e-04"
exitifwrong $?

answer1 warp.fee nafems-le11-hex8  "6.509e+01 1.000e+00 6.409e-04"
exitifwrong $?

answer1 warp.fee nafems-le11-hex20 "6.563e+01 1.000e+00 6.463e-04"
exitifwrong $?

answer1 warp.fee nafems-le11-hex27 "6.569e+01 1.000e+00 6.469e-04"
exitifwrong $?



answer nafems-le11-alpha-of-x.fee "-104.8"
exitifwrong $?

answer nafems-le11-alpha-of-T.fee "-104.8"
exitifwrong $?
