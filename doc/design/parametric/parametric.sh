for temp in $(seq 1 3); do
 for shape in triang quad; do
   feenox parametric.fee ${temp} ${shape}
 done
done
