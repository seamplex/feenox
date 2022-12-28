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

checkpde mechanical
has_gmsh=$(which gmsh)

# # for i in tri3 tri6s tri6c quad4 quad8s quad8c quad9s quad9c; do
# for i in $(seq 1 8); do
#   if [ ! -e nafems-le1-${i}.msh ]; then
#     if [ ! -z "${has_gmsh}" ]; then
#       gmsh -2 -setnumber Mesh.NodeType ${i} nafems-le1.geo -o nafems-le1-${i}.msh || exit $?
#     else
#       exit 77
#     fi
#   fi
# done

answer1 nafems-le1.fee 1 46.8
exitifwrong $?

answer1 nafems-le1.fee 2 78.6
exitifwrong $?

answer1 nafems-le1.fee 3 83.4
exitifwrong $?

answer1 nafems-le1.fee 4 81.3
exitifwrong $?

answer1 nafems-le1.fee 5 99.6
exitifwrong $?

answer1 nafems-le1.fee 6 86.8
exitifwrong $?

answer1 nafems-le1.fee 7 107.7
exitifwrong $?

answer1 nafems-le1.fee 8 92.0
exitifwrong $?
