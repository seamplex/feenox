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
