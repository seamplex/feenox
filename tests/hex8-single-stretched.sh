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
answer1  hex8-single-stretched.fee svk "-0.0498"
exitifwrong $?

# TODO: should linear elasticity with ldef be svk or hencky? think!
answer2  hex8-single-stretched.fee linear --non-linear "-0.0498"
exitifwrong $?

# reflex neo  -0.041081556232
answer1  hex8-single-stretched.fee neohookean "-0.0411"
exitifwrong $?

# TODO
# reflex hencky -0.03850440789

for i in svk neohookean; do
  echo -n "jacobian ${i} ... "
  ${feenox} ${dir}/hex8-ldef.fee --snes_test_jacobian | grep "||J - Jfd||_F/||J||_F =" | tr -d , | awk '{d+=$5}END{exit d>1e-5}' 
  exitifwrong $?
  echo "ok"
done

