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

# reflex linear  -0.044117647059
answer1  hex8-single-stretched.fee linear "-0.0441"
exitifwrong $?

# reflex svk -0.049812041887
# this is the same as linear with ldef which is svk
answer2  hex8-single-stretched.fee linear --non-linear "-0.0498"
exitifwrong $?

# reflex neo  -0.041081556232
answer1  hex8-single-stretched.fee neohookean "-0.0411"
exitifwrong $?

# reflex hencky -0.03850440789
