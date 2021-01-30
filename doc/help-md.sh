#!/bin/bash

kws=$(grep "///op+" ../src/flow/init.c | awk '{print $1}' | awk -F+ '{print $2}' | uniq)

for kw in ${kws}; do

  # option como definition list
  grep "///op+${kw}+option" ../src/flow/init.c | cut -d" " -f2-
  echo  

  # detailed description
  # el cut saca los tags especiales
  # el segundo se es para poder poner links como https:/\/ (sin la barra del medio gcc piensa que es un comentario)
  echo -n ':    '
  grep "///op+${kw}+desc" ../src/flow/init.c | cut -d" " -f2- | sed 's_/\\/_//_'
  echo  
  echo  
  
done


grep "///help+extra+desc" ../src/flow/init.c | cut -d" " -f2- | sed 's/@$//' | sed 's_/\\/_//_'

echo 
