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

# skip in problematic architectures
arch=$(uname -m)
if [ "x${arch}" = "xppc64" ] || [ "x${arch}" = "xs390x" ] ; then
  exit 77
fi

checkpetsc
checkmpirun

for n in 1 2 3 4; do
  echo -n "hello_mpi with ${n} ranks ... "
  if [ $(mpirun -n ${n} --map-by :OVERSUBSCRIBE ${feenox} ${dir}/hello_mpi.fee | wc -l) -ne ${n} ]; then
    echo "failed"
    exit 1
  else
    echo "ok"
  fi
done
  
exit 0
