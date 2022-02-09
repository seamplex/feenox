#!/bin/bash -e

# create a markdown version of the usage and command-line options

srcs="../src/flow/init.c ../src/pdes/init.c"
kws=$(grep "///op+" ${srcs} | awk '{print $1}' | awk -F+ '{print $2}' | uniq)

echo "~~~terminal"
echo -n "feenox "
grep "///help+usage+desc" ../src/flow/init.c | cut -d" " -f2- | sed 's/@$//' | sed 's_/\\/_//_'
echo "~~~"
echo


for kw in ${kws}; do

  # option as definition list
  grep "///op+${kw}+option" ${srcs} | cut -d" " -f2-
  echo  

  # detailed description
  # el cut saca los tags especiales
  # el segundo se es para poder poner links como https:/\/ (sin la barra del medio gcc piensa que es un comentario)
  echo -n ':    '
  grep "///op+${kw}+desc" ${srcs} | cut -d" " -f2- | sed 's_/\\/_//_'
  echo
  echo
  
done


grep "///help+extra+desc" ${srcs} | cut -d" " -f2- | sed 's/@$//' | sed 's_/\\/_//_'

echo 
