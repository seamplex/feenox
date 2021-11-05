#!/bin/bash

declare -A color
color["feenox"]="black"
color["sparselizard"]="blue"
color["aster"]="dark-green"
color["calculix"]="red"
color["reflex"]="grey"

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
lt["default"]="7"

echo -n "plot " > plot.gp

for i in $(ls *.dat | grep -v le10); do
  program=$(echo $(basename ${i} .dat) | cut -d"_" -f 1)
  solver=$(echo $(basename ${i} .dat) | cut -d"_" -f 2)
  shape=$(echo $(basename ${i} .dat) | cut -d"_" -f 3)
 
  echo ${program} ${solver}
  echo -n "\"${i}s\" u cx:cy  w lp  lw ${lw[$program]}  dashtype ${lt[$solver]}  lc \"${color[$program]}\"  ti \"${program} ${solver}\", " >> plot.gp
  sort -r -g ${i} | grep -v nan | grep -v exited > ${i}s
done

cat plot.gp | tr -d '\n' > plot-sigmay.gp
echo "-5.38 w l lw 1 lc \"tan1\" ti \"reference\"" >> plot-sigmay.gp


if [ -z "$(which gnuplot)" ]; then
  echo "gnuplot not installed, not creating the plots, just the data files *.dats"
fi

gnuplot figures.gp
