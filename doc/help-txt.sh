#!/bin/bash

grep "///help+usage+desc" ../src/flow/init.c | cut -d" " -f2- | sed 's/@$//' | sed 's_/\\/_//_' | pandoc -t plain

echo

kws=$(grep "///op+" ../src/flow/init.c | awk '{print $1}' | awk -F+ '{print $2}' | uniq)

for kw in ${kws}; do

  # one singe line
   left=$(grep "///op+${kw}+option" ../src/flow/init.c | cut -d" " -f2- | pandoc -t plain)
  right=$(grep "///op+${kw}+desc" ../src/flow/init.c | cut -d" " -f2- | sed 's_/\\/_//_' | pandoc -t plain)
  len_left=$(echo ${left} | wc -c)
  n_spaces=$((20 - ${len_left}))
  echo -n "  "${left}
  for i in $(seq 1 ${n_spaces}); do
    echo -n " "
  done
  echo ${right}
  
done

echo

grep "///help+extra+desc" ../src/flow/init.c | cut -d" " -f2- | sed 's/@$//' | sed 's_/\\/_//_' | pandoc -t plain

echo 
