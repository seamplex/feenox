#!/bin/bash

for i in gmsh feenox /usr/bin/time; do
 if [ -z "$(which $i)" ]; then
  echo "error: ${i} needed but not installed"
  exit 1
 fi
done

if [ ! -e le10-ref.msh ]; then
  gmsh -3 le10-ref.geo
fi

/usr/bin/time feenox le10-ref.fee 2>&1 | tee le10-ref.txt


