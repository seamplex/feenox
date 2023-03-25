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
   gmsh -3 ${i} || exit $?
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



answer1 warp.fee nafems-le11-tet4 "63.31560 1.00000 0.00062"
exitifwrong $?

answer1 warp.fee nafems-le11-tet10 "65.56050 1.00000 0.00065"
exitifwrong $?

answer1 warp.fee nafems-le11-hex8 "65.08550 1.00000 0.00064"
exitifwrong $?

answer1 warp.fee nafems-le11-hex20 "65.62840 1.00000 0.00065"
exitifwrong $?

answer1 warp.fee nafems-le11-hex27 "65.68890 1.00000 0.00065"
exitifwrong $?



answer nafems-le11-alpha-of-x.fee "-104.8"
exitifwrong $?

answer nafems-le11-alpha-of-T.fee "-104.8"
exitifwrong $?
