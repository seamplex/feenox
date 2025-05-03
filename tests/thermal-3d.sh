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

checkpde thermal
checkgmsh

gmsh -v 0 -3 ${dir}/heater-cylinder-inches.geo || exit $?
answerzero heater-cylinder-inches.fee 0.015
exitifwrong $?


for i in struct unstruct; do
 gmsh -v 0 -3 ${dir}/long-bar-${i}.geo || exit $?
 answerzero1 long-bar-thermal.fee ${i}
 exitifwrong $?
done

checkpde mechanical

for i in struct unstruct; do
 for j in struct unstruct; do
  answerzero2 long-bar-mechanical.fee ${i} ${j} 0.002
  exitifwrong $?
 done 
done
