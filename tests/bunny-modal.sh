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

checkpde modal
checkslepc
checkgmsh

if [ ! -e Stanford_Bunny.stl ]; then
 if [ ! -z "$(which wget)" ]; then
   wget https://upload.wikimedia.org/wikipedia/commons/4/43/Stanford_Bunny.stl || exit $?
 else
   return 77
 fi
fi 

gmsh -v 0 -3 ${dir}/bunny.geo -order 2 -o bunny-2nd.msh || exit $?

answer1 bunny-modal.fee fixed "0.75 0.85 1.09 1.14 1.44 1.59"
exitifwrong $?

answer1 bunny-modal.fee free  "0.96 1.09 1.44 1.59 2.55 2.67"
exitifwrong $?

answer1 bunny-modal.fee rest  "0.96 1.09 1.44 1.59 2.58 2.71"
exitifwrong $?
