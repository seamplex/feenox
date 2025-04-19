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

# the meshes are in the git repository, but just in case this is how they can be re-generated
# for i in beam-cantilever-*.geo; do gmsh -v 0 -3 $i; done

answer1 beam-orthotropic.fee tet4 "12.5 2.4 2.3 -2.4 -2.6"
exitifwrong $?

answer1 beam-orthotropic.fee tet10 "139.0 21.8 23.0 -2.1 -0.2"
exitifwrong $?

answer1 beam-orthotropic.fee hex8 "38.0 10.9 9.4 -1.2 -34.1"
exitifwrong $?

answer1 beam-orthotropic.fee hex20 "151.3 43.4 37.5 -6.9 3.2"
exitifwrong $?

answer1 beam-orthotropic.fee hex27 "158.9 45.6 39.4 -8.2 3.1"
exitifwrong $?

