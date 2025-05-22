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

checkpde mechanical
checkmumps

# the meshes are in the git repository, but just in case this is how they can be re-generated
# for i in beam-cantilever-*.geo; do gmsh -v 0 -3 $i; done

answer2 beam-ldef.fee --linear     tet4 "-0.016 -0.016"
exitifwrong $?

answer2 beam-ldef.fee --non-linear tet4 "-0.016 -0.016"
exitifwrong $?

answer2 beam-ldef.fee --linear     hex8 "-0.065 -0.065"
exitifwrong $?

answer2 beam-ldef.fee --non-linear hex8 "-0.064 -0.063"
exitifwrong $?

answer2 beam-ldef.fee --linear     tet10 "-0.291 -0.291"
exitifwrong $?

answer2 beam-ldef.fee --non-linear tet10 "-0.218 -0.213"
exitifwrong $?

answer2 beam-ldef.fee --linear     hex20 "-0.295 -0.295"
exitifwrong $?

answer2 beam-ldef.fee --non-linear hex20 "-0.218 -0.213"
exitifwrong $?

answer2 beam-ldef.fee --linear     hex27 "-0.301 -0.301"
exitifwrong $?

answer2 beam-ldef.fee --non-linear hex27 "-0.220 -0.215"
exitifwrong $?

