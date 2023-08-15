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

if [ ! -e ${dir}/bunny.msh ]; then
  if [ ! -e ${dir}/Stanford_Bunny.stl ] ; then
   if [ ! -z "$(which wget)" ]; then
     wget https://upload.wikimedia.org/wikipedia/commons/4/43/Stanford_Bunny.stl -O ${dir}/Stanford_Bunny.stl || exit $?
   else
     return 77
   fi
  fi 
  gmsh -3 ${dir}/bunny.geo || exit $?
fi

# gmsh -v 0 -3 ${dir}/bunny.geo -order 2 -o bunny-2nd.msh || exit $?

answer1 bunny-modal.fee fixed "1 1 2 2 2 2"
exitifwrong $?

answer1 bunny-modal.fee free  "2 2 2 2 3 3"
exitifwrong $?

answer1 bunny-modal.fee rest  "2 2 2 2 3 3"
exitifwrong $?
