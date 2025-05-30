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

checkpde laplace
checkgmsh

gmsh -v 0 -2 ${dir}/square-centered.geo || exit $?

answer laplace-square.fee "-1 -2.68735 -1"
exitifwrong $?

answer poisson-square.fee "0.2949 0.2044 -0.2044"
exitifwrong $?

gmsh -v 0 -2 ${dir}/maze.geo || exit $?
answerzero maze.fee
exitifwrong $?
