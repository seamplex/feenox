for i in triang quad; do
 for j in $(seq 1 3); do
   feenox parametric.fee $i $j
 done
done
