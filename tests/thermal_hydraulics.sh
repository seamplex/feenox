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

checkpde thermal_hydraulics
checkgmsh

gmsh -v 0 -1 ${dir}/thermal-hydraulics-tee.geo || exit $?
gmsh -v 0 -1 ${dir}/relap7-pipe.geo || exit $?
answerzero thermal-hydraulics-read-mesh.fee 1e-4
exitifwrong $?

answerzero thermal-hydraulics-tee.fee 1e-6
exitifwrong $?

for flux in 10000 -10000 0; do
  answerzero1 thermal-hydraulics-wall-flux.fee ${flux} 1e-7
  exitifwrong $?
done
for perimeter in 0 2; do
  answerzero2 thermal-hydraulics-wall-flux.fee 10000 ${perimeter} 1e-7
  exitifwrong $?
done
for temperature in 565 550; do
  answerzero1 thermal-hydraulics-wall-temperature.fee ${temperature} 1e-7
  exitifwrong $?
done
answerzero2 thermal-hydraulics-wall-temperature.fee 565 0 1e-7
exitifwrong $?
answerzero thermal-hydraulics-wall-transient.fee 1e-7
exitifwrong $?
answerzero thermal-hydraulics-wall-groups.fee 1e-7
exitifwrong $?
answerzero thermal-hydraulics-wall-distributions.fee 1e-7
exitifwrong $?
for flux in 10000 -10000; do
  answerzero1 thermal-hydraulics-wall-reverse.fee ${flux} 1e-7
  exitifwrong $?
done

for properties in "T_wall=565 q_wall=0" "h_wall=1000" "T_wall=565 h_wall=-1" "q_wall=1000 P_h=-1" "T_wall=700"; do
  echo "Rejecting invalid wall properties: ${properties}"
  if ${feenox} ${dir}/thermal-hydraulics-wall-invalid.fee "${properties}"; then
    echo "expected an error"
    exit 1
  fi
done
