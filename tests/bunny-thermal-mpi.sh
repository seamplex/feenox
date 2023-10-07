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
checkmpirun

if [ ! -e ${dir}/bunny.msh ]; then
  exit 77
fi

if [ ! -e ${dir}/bunny4.msh ]; then
  gmsh - ${dir}/bunny.msh -part 4 -o ${dir}/bunny4.msh || exit $?
fi
  
# this input file reads bunny4.msh that has 4 partitions
for n in $(seq 1 4); do
  answerzerompi ${n} bunny-thermal-mpi.fee 1e-2
  exitifwrong $?
done

# this input file reads bunny.msh that doest not have partitions
for n in $(seq 1 4); do
  answerzerompi ${n} bunny-thermal.fee 1e-2
  exitifwrong $?
done
