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

checkgmsh

for order in 1 2; do
 rm -f circle_perimeter${order}.dat
 for scale in 0.7 0.6 0.5 0.4 0.35 0.3; do
  gmsh -v 0 -2 ${dir}/circle.geo -order ${order} -clscale ${scale} -o circle-${order}-${scale}.msh
  ${feenox} ${dir}/circle_perimeter.fee ${order}-${scale} >> circle_perimeter${order}.dat
 done
done

answerzero1 circle_fit.fee 1 0.15
exitifwrong $?

answerzero1 circle_fit.fee 2 0.25
exitifwrong $?
