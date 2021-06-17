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
