#!/bin/bash

srcs="../src/flow/init.c ../src/pdes/init.c"
kws=$(grep "///op+" ${srcs} | awk '{print $1}' | awk -F+ '{print $2}' | uniq)

for kw in ${kws}; do

  # one singe line
  left=$(grep "///op+${kw}+option" ${srcs} | cut -d" " -f2- | pandoc -t plain)
  right=$(grep "///op+${kw}+desc" ${srcs} | cut -d" " -f2- | sed 's_/\\/_//_' | pandoc -t plain)
  len_left=$(echo ${left} | wc -c)
  n_spaces=$((20 - ${len_left}))
  echo -n "  "${left}
  for i in $(seq 1 ${n_spaces}); do
    echo -n " "
  done
  echo ${right}
  
done
