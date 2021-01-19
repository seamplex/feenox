#!/bin/false

# marker
functions_found=yes

# see how to invoke FeenoX
for i in  ../feenox ../src/feenox ./feenox ./src/feenox; do
  if [ -f ${i} ]; then
    feenox=${i}
  fi
done
if [ -z "${feenox}" ]; then
  echo "could not find feenox executable"
  exit 1
fi

# see where the inputs are
for i in  . tests; do
  if [ -f ${i}/functions.sh ]; then
    dir=${i}
  fi  
done


#######

exitifwrong() {
  if [ $1 != 0 ]; then
    exit $1
  fi  
}  


answer() {
  echo -n "${1}... "
  answer=$(${feenox} ${dir}/${1})
  
  if [ "${answer}" = "${2}" ]; then
    echo "ok"
    level=0
  else
    echo "wrong, expected '${2}' and got '${answer}'"
    level=1
  fi

  return ${level}
}


answerfloat() {
  echo -n "${1}... "
  answer=$(${feenox} ${dir}/${1})
  
  if [ "$(${feenox} ${dir}/cmp.fee "${answer}" "${2}")" = "0.000000" ]; then
    echo "ok"
    level=0
  else
    echo "wrong, expected '${2}' and got '${answer}'"
    level=1
  fi

  return ${level}
}

answer1() {
  echo -n "${1} ${2}... "
  answer=$(${feenox} ${dir}/${1} ${2})
  
  if [ "${answer}" = "${3}" ]; then
    echo "ok"
    level=0
  else
    echo "wrong, expected '${3}' and got '${answer}'"
    level=1
  fi

  return ${level}
}

answer1float() {
  echo -n "${1} ${2}... "
  answer=$(${feenox} ${dir}/${1} ${2})
  
  if [ "$(${feenox} ${dir}/cmp.fee "${answer}" "${3}")" = "0.000000" ]; then
    echo "ok"
    level=0
  else
    echo "wrong, expected '${3}' and got '${answer}'"
    level=1
  fi

  return ${level}
}


answer2sorthead1() {
  answer=$(${feenox} ${dir}/$1 $2 $3 | sort -rg | head -n1)
  
  if [ "${answer}" = "$4" ]; then
    echo "ok"
    level=0
  else
    echo "wrong"
    level=1
  fi

  return ${level}
}
