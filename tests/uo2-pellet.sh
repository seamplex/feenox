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

# run with --ksp_view to see the difference between these two
answerzero pellet-linear.fee 5e-2
exitifwrong $?

answerzero pellet-linear-guess.fee 5e-2
exitifwrong $?


# run with --snes_view
answer pellet-nonlinear.fee "1115"
exitifwrong $?

answer pellet-nonlinear-guess.fee "1115"
exitifwrong $?


# transients
answer pellet-linear-transient-from-ss.fee "1453"
exitifwrong $?

answer pellet-linear-transient-from-initial.fee "1272"
exitifwrong $?

answer pellet-nonlinear-transient-from-ss.fee "1438"
exitifwrong $?

answer pellet-nonlinear-transient-from-initial.fee "1115"
exitifwrong $?
