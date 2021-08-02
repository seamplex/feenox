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

# checks if feenox is compiled with ida and skips the test if necessary
checkida() {
 if [ $(${feenox} --versions | grep 'SUNDIAL' | grep -v 'N/A'| wc -l) = 0 ]; then
  echo "FeenoX was not compiled with IDA, skipping test"
  exit 77
 fi
}

# checks if feenox is compiled with ida and skips the test if necessary
checkpetsc() {
 if [ $(${feenox} --versions | grep 'PETSc' | grep -v 'N/A'| wc -l) = 0 ]; then
  echo "FeenoX was not compiled with PETSc, skipping test"
  exit 77
 fi
}

# checks if feenox is compiled with ida and skips the test if necessary
checkslepc() {
 if [ $(${feenox} --versions | grep 'SLEPc' | grep -v 'N/A'| wc -l) = 0 ]; then
  echo "FeenoX was not compiled with SLEPc, skipping test"
  exit 77
 fi
}


# checks if gmsh executable is available in the path
checkgmsh() {
 if [ -z "$(which gmsh)" ]; then
  echo "Gmsh not found, skipping test"
  exit 77
 fi
}

answer() {
  echo -n "${1} ... "
  answer=$(${feenox} ${dir}/${1})
  error=$?
  
  if [ ${error} != 0 ]; then
    echo "failed"
    return 2
  fi
  
  if [ "${answer}" = "${2}" ]; then
    echo "ok"
    level=0
  else
    echo "wrong, expected '${2}' and got '${answer}'"
    level=1
  fi

  return ${level}
}

answerdiff() {
  echo -n "${1} ... "
  base=$(basename ${1} .fee)
  ${feenox} ${dir}/${1} > ${dir}/${base}.last
  error=$?
  
  difference=$(diff -w ${dir}/${base}.ref ${dir}/${base}.last)
  if [ -z "${difference}" ]; then
    echo "ok"
    level=0
  else
    echo "wrong, see ${base}.ref and ${base}.last"
    level=1
  fi
  
  return ${level}
}

answer1diff() {
  echo -n "${1} ... "
  base=$(basename ${1} .fee)
  ${feenox} ${dir}/${1} ${2} > ${dir}/${base}.last
  error=$?
  
  difference=$(diff -w ${dir}/${base}.ref ${dir}/${base}.last)
  if [ -z "${difference}" ]; then
    echo "ok"
    level=0
  else
    echo "wrong, see ${base}.ref and ${base}.last"
    level=1
  fi
  
  return ${level}
}


answerfloat() {
  echo -n "${1} ... "
  answer=$(${feenox} ${dir}/${1})

  if [ "$(${feenox} ${dir}/cmp-float.fee "(${answer})" "${2}" "${3}")" = "1" ]; then
    echo "ok"
    level=0
  else
    echo "wrong, expected '${2}' and got '${answer}'"
    level=1
  fi

  return ${level}
}

answerzero() {
  echo -n "${1} ... "
  answer=$(${feenox} ${dir}/${1})
  error=$?
  
  if [ ${error} != 0 ]; then
    return 2
  fi

  if [ -z "${2}" ]; then
    result=$(${feenox} ${dir}/cmp-zero.fee "(${answer})")
  else  
    result=$(${feenox} ${dir}/cmp-zero.fee "(${answer})" "${2}")
  fi  
  if [ "${result}" = "1" ]; then
    echo "ok"
    level=0
  else
    echo "wrong, expected zero and got '${answer}'"
    level=1
  fi

  return ${level}
}


answer1() {
  echo -n "${1} ${2} ... "
  answer=$(${feenox} ${dir}/${1} ${2})
  error=$?
  
  if [ ${error} != 0 ]; then
    return 2
  fi
  
  if [ "${answer}" = "${3}" ]; then
    echo "ok"
    level=0
  else
    echo "wrong, expected '${3}' and got '${answer}'"
    level=1
  fi

  return ${level}
}


answerfloat() {
  echo -n "${1} ... "
  answer=$(${feenox} ${dir}/${1})

  if [ "$(${feenox} ${dir}/cmp-float.fee "(${answer})" "${2}" "${3}")" = "1" ]; then
    echo "ok"
    level=0
  else
    echo "wrong, expected '${2}' and got '${answer}'"
    level=1
  fi

  return ${level}
}


answer1float() {
  echo -n "${1} ${2} ... "
  answer=$(${feenox} ${dir}/${1} ${2})
  
  if [ -z "${4}" ]; then
    result=$(${feenox} ${dir}/cmp-float.fee "(${answer})" "${3}")
  else  
    result=$(${feenox} ${dir}/cmp-float.fee "(${answer})" "${3}" "${4}")
  fi  
  
  if [ "${result}" = "1" ]; then
    echo "ok"
    level=0
  else
    echo "wrong, expected '${3}' and got '${answer}'"
    level=1
  fi

  return ${level}
}

answer1zero() {
  echo -n "${1} ${2} ... "
  answer=$(${feenox} ${dir}/${1} ${2})

  if [ -z "${3}" ]; then
    result=$(${feenox} ${dir}/cmp-zero.fee "(${answer})")
  else  
    result=$(${feenox} ${dir}/cmp-zero.fee "(${answer})" "${3}")
  fi  
  if [ "${result}" = "1" ]; then
    echo "ok"
    level=0
  else
    echo "wrong, expected zero and got '${answer}'"
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
