#!/bin/bash
for temp in $(seq 1 3); do
 for shape in triang quad; do
   feenox two-squares-thermal.fee ${temp} ${shape}
 done
done
