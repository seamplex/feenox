for i in *.dat; do
 sort -r -g ${i} > ${i}s
done
pyxplot plot.ppl
