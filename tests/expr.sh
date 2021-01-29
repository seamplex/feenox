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

answer1 expr.fee 1+1 "2"
exitifwrong $?

answer1 expr.fee 1+3 "4"
exitifwrong $?

answer1 expr.fee '1+2*3' "7"
exitifwrong $?

answer1 expr.fee '(1+2)*3' "9"
exitifwrong $?

answer1 expr.fee '(-2^3)' "-8"
exitifwrong $?

answer1 expr.fee '(-1-(-1))' "0"
exitifwrong $?

answer1 expr.fee '(-1+(-1))' "-2"
exitifwrong $?

answer1 expr.fee '(-1+(+1))' "0"
exitifwrong $?

answer1 expr.fee '(-1-(+1))' "-2"
exitifwrong $?

answer1 expr.fee '1/3' "0.333333"
exitifwrong $?

answer1 expr.fee '(1+1/1e6)^1e6' "2.71828"
exitifwrong $?

answer1 expr.fee '1/((1+1/1e6)^1e6)' "0.36788" 
exitifwrong $?

answer1 expr.fee '((1+1/1e6)^1e6)^(-1)' "0.36788"
exitifwrong $?

answer1 expr.fee '2^0.5' "1.41421"
exitifwrong $?

answer1 expr.fee '2^(1/2)' "1.41421"
exitifwrong $?

answer1 expr.fee 'sqrt(5)+(-2*(-2))*(-1)' "-1.76393"
exitifwrong $?


answer1 expr.fee 'pi' "3.14159"
exitifwrong $?

answer1 expr.fee 'pi/2' "1.5708"
exitifwrong $?

answer1 expr.fee '2*pi' "6.28319"
exitifwrong $?

answer1 expr.fee 'pi^2' "9.8696"
exitifwrong $?

answer1 expr.fee 'pi^0.5' "1.77245"
exitifwrong $?



answer1 expr.fee 'sqrt(2)' "1.41421"
exitifwrong $?

answer1 expr.fee 'sqrt(3)' "1.73205"
exitifwrong $?

answer1 expr.fee 'sqrt(pi)' "1.77245"
exitifwrong $?

answer1 expr.fee 'sqrt(pi^2)' "3.14159"
exitifwrong $?

answer1 expr.fee 'sqrt(pi)^2' "3.14159"
exitifwrong $?


answer1 expr.fee 'atan2(exp(-1/2),log(sqrt(2)))' "1.05167"
exitifwrong $?

answer1 expr.fee 'mod(sinh(1+1^2),pi)' "0.485268"
exitifwrong $?

answer1 expr.fee 'max(1,sqrt(2),cos(acos(2/3)),floor(1.9))' "1.41421"
exitifwrong $?

answer1 expr.fee 'if(sqrt(10)>pi,10,pi^2)' "10"
exitifwrong $?



answer1 expr.fee 'sin(1)' "0.841471"
exitifwrong $?

answer1 expr.fee 'sqrt(1-cos(1)^2)' "0.841471"
exitifwrong $?


answer1 expr.fee 'sin(1)^2+cos(1)^2' "1"
exitifwrong $?


answer1 expr.fee '1/0' "inf"
exitifwrong $?

answer1 expr.fee '(-1)^0.5' "-nan"
exitifwrong $?

answer1 expr.fee '1+' "1+"
exitifwrong $?

answer1 expr.fee '1+*1' "1+*1"
exitifwrong $?

answer1 expr.fee '(1+1' "(1+1"
exitifwrong $?
