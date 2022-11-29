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
checkgmsh

for i in struct unstruct; do
 gmsh -3 ${dir}/long-bar-${i}.geo
 answerzero1 long-bar-thermal.fee ${i}
 exitifwrong $?
done

checkpde mechanical

for i in struct unstruct; do
 for j in struct unstruct; do
  answerzero2 long-bar-thermal.fee ${i} ${j}
  exitifwrong $?
 done 
done
