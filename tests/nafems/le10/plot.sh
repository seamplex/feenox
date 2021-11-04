#!/bin/bash

declare -A color
color["feenox"]="grey30"
color["sparselizard"]="blue"
color["aster"]="green"
color["calculix"]="red"
color["reflex"]="salmon"

declare -A lw
lw["feenox"]="5"
lw["sparselizard"]="4"
lw["aster"]="3"
lw["calculix"]="2"
lw["reflex"]="1"

declare -A lt
lt["gamg"]="1"
lt["mumps"]="2"
lt["spooles"]="3"
lt["hypre"]="4"
lt["diagonal"]="5"
lt["cholesky"]="6"

echo -n "plot " > plot.gp

for i in $(ls *.dat | grep -v le10); do
 program=$(echo $(basename ${i} .dat) | cut -d"_" -f 1)
 solver=$(echo $(basename ${i} .dat) | cut -d"_" -f 2)
 shape=$(echo $(basename ${i} .dat) | cut -d"_" -f 3)
 steps=$(cat ${i} | wc -l)
 
 echo -n "\"${i}s\"   u 2:c  w l lw ${lw[$program]} dashtype ${lt[$solver]}  lc \"${color[$program]}\"    ti \"\", " >> plot.gp
 echo -n "\"${i}s\"   u 2:c  every $(($RANDOM % ${steps} + 1)) w p           lc \"${color[$program]}\"    ti \"${program} ${solver}\", " >> plot.gp

 sort -r -g ${i} | grep -v nan | grep -v exited > ${i}s
done

if [ -z "$(which gnuplot)" ]; then
  echo "gnuplot not installed, not creating the plots"
fi

gnuplot figures.gp
