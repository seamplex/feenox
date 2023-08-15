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

if [ ! -e ./Stanford_Bunny.stl -o -e ${dir}/Stanford_Bunny.stl ] ; then
 if [ ! -z "$(which wget)" ]; then
   wget https://upload.wikimedia.org/wikipedia/commons/4/43/Stanford_Bunny.stl || exit $?
   mv Stanford_Bunny.stl ${dir}
 else
   return 77
 fi
fi 

gmsh -3 ${dir}/bunny.geo -o bunny.msh || exit $?

answerzero bunny-thermal.fee 1e-2
exitifwrong $?
