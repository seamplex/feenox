#!/bin/sh

for angle in 0 5 10 15 20 25 30 35 40 45; do
  echo "angle = ${angle};" > azmy-angle-${angle}.geo
  gmsh -v 0 -2 azmy-angle-${angle}.geo azmy-full.geo -o azmy-full-${angle}.msh
  feenox azmy-full.fee ${angle} 6
done
