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

answer1 read_mesh2d.fee 22 "12 22 14"
exitifwrong $?

answer1 read_mesh2d.fee 40 "12 22 14"
exitifwrong $?

answer1 read_mesh2d.fee 41 "12 22 14"
exitifwrong $?

answer1 read_mesh2d.fee 41bin "12 22 14"
exitifwrong $?

answer1 write_mesh2d.fee msh "0.531905"
exitifwrong $?

answer1 integrate2d.fee msh "0.530983 0.530983 0.313011 0.310173"
exitifwrong $? 

# TODO
# answer1 write_mesh2d.fee vtk "0.531905"
# exitifwrong $?

answer mesh3d.fee "50"
exitifwrong $?
