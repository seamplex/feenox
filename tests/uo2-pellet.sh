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

gmsh -3 ${dir}/pellet.geo

# run with --ksp_view to see the difference
answerzero pellet-linear.fee 1.5
exitifwrong $?

answerzero pellet-linear-guess.fee 1.5
exitifwrong $?


# run with --snes_view to see the difference
answerzero pellet-nonlinear.fee 1117 2
exitifwrong $?

answerzero pellet-nonlinear-guess.fee 1117 2
exitifwrong $?


# transients
answer pellet-linear-transient-from-ss.fee "30 1757"
exitifwrong $?

answer pellet-linear-transient-from-initial.fee "30 1277"
exitifwrong $?
