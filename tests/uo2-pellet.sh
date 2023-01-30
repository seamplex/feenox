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

gmsh -3 ${dir}/pellet.geo || exit $?

# run with --ksp_view to see the difference between these two
answerzero pellet-linear.fee 5e-2
exitifwrong $?

answerzero pellet-linear-guess.fee 5e-2
exitifwrong $?


# run with --snes_view
answerzero pellet-nonlinear.fee 5
exitifwrong $?

answerzero pellet-nonlinear-guess.fee 5
exitifwrong $?


# transients
answerzero pellet-linear-transient-from-ss.fee 5
exitifwrong $?

answerzero pellet-linear-transient-from-initial.fee 5
exitifwrong $?

answerzero pellet-nonlinear-transient-from-ss.fee 5
exitifwrong $?

answerzero pellet-nonlinear-transient-from-initial.fee 5
exitifwrong $?
