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

answer1 expr.fee "sin(pi/2)" 1
exitifwrong $?

answer1 expr.fee "sin(1)^2+cos(1)^2" 1
exitifwrong $?

answer1float expr.fee "cos(30*pi/180)-sqrt(3)/2" 0
exitifwrong $?

answer1float expr.fee "atan2(1,-1)-pi/2" "pi/4"
exitifwrong $?
