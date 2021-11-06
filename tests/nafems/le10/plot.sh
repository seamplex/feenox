#!/bin/bash

if [ -z "${1}" ]; then 
  echo "usage: $0 { tet | hex }"
  exit 0  
fi

declare -A color
color["feenox"]="black"
color["sparselizard"]="blue"
color["aster"]="dark-green"
color["calculix"]="orange"
color["reflex"]="purple"

declare -A lw
lw["feenox"]="2"
lw["sparselizard"]="2"
lw["aster"]="2"
lw["calculix"]="2"
lw["reflex"]="2"

declare -A lt
lt["gamg"]="1"
lt["mumps"]="2"
lt["spooles"]="3"
lt["default"]="4"
lt["diagonal"]="5"
lt["cholesky"]="6"
lt["hypre"]="7"

if [ "x$(ls *${1}*.dat | wc -l)" != "x0" ]; then

  cat << EOF > report-${1}.md
---
title: Report for resource consumption in the NAFEMS LE10 problem
subtitle: ${1} case
lang: en-US
author: Jeremy Theler
date: ${date}
...

|         |        
|---------|------------------------------------------------------------------------------------------------
| Host    |  $(uname -a) 
| CPU     |  $(cat /proc/cpuinfo | grep "model name" | head -n1 | cut -d: -f2) 
| Memory  |  $(cat /proc/meminfo  | head -n1 | cut -d: -f2)
| Date    |  $(stat *${1}.dat | grep Change | sort | tail -n1 | cut -d" " -f2,3,4)
  

![Reference ${1} mesh for \$c=1\$](le10-${1}-mesh.png)

EOF

  echo -n "plot " > plot-${1}.gp

  for i in feenox_*.dat sparselizard_*.dat aster_*.dat calculix_*.dat reflex_*.dat; do
    program=$(echo $(basename ${i} .dat) | cut -d"_" -f 1)
    solver=$(echo $(basename ${i} .dat) | cut -d"_" -f 2)
    shape=$(echo $(basename ${i} .dat) | cut -d"_" -f 3)
  
    if [ "x${shape}" = "x${1}" ]; then
      echo ${program} ${solver} ${shape}
      echo -n "\"${i}s\" u cx:cy  w lp lw ${lw[$program]} pt ${lt[$solver]} dashtype ${lt[$solver]}  lc \"${color[$program]}\"  ti \"${program} ${solver}\", " >> plot-${1}.gp
      sort -r -g ${i} | grep -v nan | grep -v exited | grep -v -w 0 > ${i}s
    fi  
  done

  cat plot-${1}.gp | tr -d '\n' > plot-${1}-sigmay.gp
  echo "-5.38 w l lw 1 lc \"plum\" ti \"reference\"" >> plot-${1}-sigmay.gp

  if [ ! -z "$(which gnuplot)" ]; then
    sed s/xxx/${1}/ figures.gp | gnuplot -  
  else
    echo "gnuplot not installed, not creating the plots, just the data files *.dats"
  fi

  for i in sigmay*${1}.svg wall*${1}.svg memory*${1}.svg user*${1}.svg kernel*${1}.svg; do
    echo "![](${i})\\ " >> report-${1}.md
    echo >> report-${1}.md
  done
  echo >> report-${1}.md
  
else

  echo "nothing to plot for ${1}"
  exit 0
fi
