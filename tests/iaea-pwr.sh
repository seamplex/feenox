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
checkslepc

gmsh -v 0 -2 ${dir}/2dpwr-quarter.geo
answer1 2dpwr.fee quarter "1.02986"
exitifwrong $?

gmsh -v 0 -2 ${dir}/2dpwr-eighth.geo
answer1 2dpwr.fee eighth  "1.02975"
exitifwrong $?
