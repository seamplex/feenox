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

# solid 3d
for i in quarter eighth sixteenth; do
  gmsh -3 spinning-disk-parallel-solid-half${i}_unstruct.geo
  answer1zero spinning-disk-parallel-solid-half.fee ${i}_unstruct
  exitifwrong $?
done

# plane stress
for i in quarter eighth sixteenth; do
  gmsh -2 spinning-disk-parallel-plane-${i}_unstruct.geo
  answer1zero spinning-disk-parallel-plane.fee ${i}_unstruct
  exitifwrong $?
done