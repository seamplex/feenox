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

gmsh -v 0 -1 ${dir}/wilson-1d.geo || exit $?
answerzero wilson-1d.fee
exitifwrong $?

gmsh -v 0 -2 ${dir}/wilson-2d.geo || exit $?
answerzero wilson-2d.fee 2e-2
exitifwrong $?

# check TIME_PATH
echo -n "time_path_petsc.fee ... "
rm -rf wilson-2d*.vtk
${feenox} ${dir}/time_path_petsc.fee
if [ "x$(ls wilson-2d*.vtk | wc -l)" != "x6" ]; then
  echo "failed"
  return 1
fi
echo "ok"
