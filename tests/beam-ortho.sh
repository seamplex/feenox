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

checkpetsc

# the meshes are in the git repository, but just in case this is how they can be re-generated
# for i in beam-cantilever-*.geo; do gmsh -3 $i; done


answer1 orthotropic-beam.fee tet4 "12.54 2.44 2.30 -2.44 -2.56"
exitifwrong $?

answer1 orthotropic-beam.fee tet10 "139.04 21.84 23.02 -2.13 -0.24"
exitifwrong $?

answer1 orthotropic-beam.fee hex8 "38.03 10.91 9.42 -1.16 -34.13"
exitifwrong $?

answer1 orthotropic-beam.fee hex20 "151.29 43.41 37.49 -6.92 3.21"
exitifwrong $?

answer1 orthotropic-beam.fee hex27 "158.92 45.60 39.39 -8.16 3.13"
exitifwrong $?

