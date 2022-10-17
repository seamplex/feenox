#!/bin/bash -ex

for i in convert potrace inkscape pstoedit make; do
 if [ -z "$(which $i)" ]; then
  echo "error: ${i} not installed"
  exit 1
 fi
done


convert maze.png -negate maze_negated.pnm
potrace maze_negated.pnm --alphamax 0  --opttolerance 0 -b svg -o maze.svg
inkscape maze.svg --export-eps=maze.eps
pstoedit -dt -f 'dxf:-polyaslines -mm' maze.eps > maze.dxf
make dxf2geo
./dxf2geo maze.dxf 0.1

