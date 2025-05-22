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

checkpde mechanical
checkmumps
checkgmsh

# run fem parametric cases
for shape in ust10 uct10 sst10 sct10 ssh20 sch20 ssh27 sch27; do
  gmsh -v 0 -3 ${dir}/pipe-${shape}-2-2.geo || exit $?
  answerzero1 pipe.fee ${shape} 1e-2
  exitifwrong $?
done

