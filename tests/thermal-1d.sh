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

answerzero thermal-slab-uniform-nosource.fee
exitifwrong $?

answerzero thermal-slab-space-nosource.fee
exitifwrong $?

answerzero thermal-slab-temperature-nosource.fee
exitifwrong $?

answerzero thermal-slab-heat-nosource.fee
exitifwrong $?

answerzero thermal-slab-convection-nosource.fee
exitifwrong $?

answerzero thermal-slab-convection-as-heat-nosource.fee
exitifwrong $?

answerzero thermal-slab-transient.fee 1e-4
exitifwrong $?

answerzero thermal-slab-uniform-source.fee
exitifwrong $?

answer thermal-slab-uniform-source-dirichlet-neumann.fee "0.100"
exitifwrong $?



if [ ! -z "$(which octave)" ]; then
  base="thermal-slab-uniform-source-octave"
  echo "K_bc; b_bc; K_bc\\\\b_bc" | octave > ${base}.last
  difference=$(diff -w ${dir}/${base}.ref ${dir}/${base}.last)
  if [ ! -z "${difference}" ]; then
    echo "Octave check on DUMP failed"
    exitifwrong 1
  fi
fi

# clean up if we are not in tests
if [ ! -e ./functions.sh ]; then
  rm -f b_bc.m  b.m  K_bc.m  K.m ${base}.last
fi
