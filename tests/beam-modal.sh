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

checkslepc
checkpde modal

# the meshes are in the git repository, but just in case this is how they can be re-generated
# for i in beam-cantilever-*.geo; do gmsh -v 0 -3 $i; done


answer1 beam-cantilever-modal.fee tet4 "249.7 341.2 1506.1 1962.4 2639.7 4050.7"
exitifwrong $?

answer1 beam-cantilever-modal.fee tet10 "69.8 170.6 466.4 1060.6 1154.5 1487.1"
exitifwrong $?

answer1 beam-cantilever-modal.fee hex8 "147.9 213.7 937.0 1122.0 1307.2 2623.5"
exitifwrong $?

answer1 beam-cantilever-modal.fee hex20 "69.3 169.6 456.3 1040.7 1101.3 1411.5"
exitifwrong $?

answer1 beam-cantilever-modal.fee hex27 "68.5 169.2 450.2 1037.6 1098.9 1384.2"
exitifwrong $?



# answer1 beam-cantilever-modal-free-free.fee tet4 "1570.3 2248.0 5220.3 5395.0 6244.1 9072.3"
# exitifwrong $?

# answer1 beam-cantilever-modal-free-free.fee tet10 "419.5 1075.9 1265.7 2243.0 2977.0 3151.3"
# exitifwrong $?

# TODO: this one has a nan
# answer1 beam-cantilever-modal-free-free.fee hex8 "1001.3 1805.9 2190.3 2834.6 -nan 4850.2"
# exitifwrong $?

# TODO: this one has a complex eigenvalue
# answer1 beam-cantilever-modal-free-free.fee hex20 "69.3 169.6 456.3 1040.7 1101.3 1411.5"
# exitifwrong $?

# answer1 beam-cantilever-modal-free-free.fee hex27 "373.9 796.8 1076.3 1806.1 2139.9 2458.0"
# exitifwrong $?

checkgmsh

gmsh -v 0 -3 ${dir}/PF.geo || exit $?
answer modal-solidworks.fee "0.507"
