#!/bin/sh

for angle in 0 5 15 30 45; do
 echo "angle = ${angle};" > azmy-angle-${angle}.geo
 for scale in 3 2 1.5 1; do
  gmsh -v 0 -2 azmy-angle-${angle}.geo azmy-full.geo -clscale ${scale} -o azmy-full-${angle}.msh
  for sn in 2 4 6 8; do
   echo $angle $scale $sn
   feenox azmy-full.fee ${angle} ${sn} ${scale} --progress
  done
 done
done
