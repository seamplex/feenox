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

# skip in big-endian architectures
arch=$(uname -m)
if [ "x${arch}" = "xppc64" ] || [ "x${arch}" = "xs390x" ] ; then
  exit 77
fi

checkpde thermal

answer thermal-two-squares-material-explicit-uniform.fee "0.750"
exitifwrong $?

answer thermal-two-squares-material-implicit-uniform.fee "0.750"
exitifwrong $?

answer thermal-two-squares-material-explicit-space.fee "0.718"
exitifwrong $?

answer thermal-two-squares-material-implicit-space.fee "0.718"
exitifwrong $?

answer thermal-two-squares-material-explicit-temperature.fee "0.600"
exitifwrong $?

answer thermal-two-squares-material-implicit-temperature.fee "0.600"
exitifwrong $?

answer thermal-two-squares-material-implicit-temperature.fee "0.600"
exitifwrong $?

answerzero thermal-square.fee
checkmumps

i=thermal-two-squares-material-explicit-uniform
echo -n "${i} (MUMPS)... "
answer=$(${feenox} ${dir}/${i}.fee --mumps --ksp_view | grep INFO | wc -l)
if [ ${answer} -gt 0 ]; then
  echo "ok"
  level=0
else
  echo "wrong, MUMPS was not used"
  level=1
fi
exitifwrong ${level}





