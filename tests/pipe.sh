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

# run fem parametric cases
# for shape in ust10 uct10 sst10 sct10 ssh20 sch20 ssh27 sch27; do
for shape in uct10 sst10 sct10 ssh20 sch20 ssh27 sch27; do
  gmsh -v 0 -3 pipe-${shape}-2-2.geo || exit 1
  feenox pipe.fee ${shape} 2 2
done

