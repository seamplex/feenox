#!/bin/bash -e

if [ -z "${1}" ]; then
  echo "usage: ${0} source.c"
  exit 1
fi

kws=$(grep "///op+" ${1} | awk '{print $1}' | awk -F+ '{print $2}' | uniq)

for kw in ${kws}; do

  # one single line
  left=$(grep "///op+${kw}+option" ${1} | cut -d" " -f2- | pandoc -t plain)
  right=$(grep "///op+${kw}+desc" ${1} | cut -d" " -f2- | sed 's_/\\/_//_' | pandoc -t plain)
  len_left=$(echo ${left} | wc -c)
  n_spaces=$((20 - ${len_left}))
  echo -n "  "${left}
  for i in $(seq 1 ${n_spaces}); do
    echo -n " "
  done
  echo ${right}
  
done
