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

gmsh -v 0 -3 ${dir}/pellet.geo || exit $?

# run with --ksp_view to see the difference between these two
answerzero pellet-linear.fee 5e-2
exitifwrong $?

answerzero pellet-linear-guess.fee 5e-2
exitifwrong $?

answerzero pellet-nonuniform-q.fee 12
exitifwrong $?


# run with --snes_monitor
answerzero pellet-nonlinear.fee 12
exitifwrong $?

answerzero pellet-nonlinear-guess.fee 12
exitifwrong $?

answerzero pellet-nonlinear-q.fee 12
exitifwrong $?


# transients
answerzero pellet-linear-transient-from-ss.fee 15
exitifwrong $?

answerzero pellet-linear-transient-from-initial.fee 15
exitifwrong $?

answerzero pellet-nonlinear-transient-from-ss.fee 15
exitifwrong $?

answerzero pellet-nonlinear-transient-from-initial.fee 15
exitifwrong $?
