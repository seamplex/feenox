#!/bin/bash
rm -rf *.png *.dat
wasora plane.fee > plane.dat

n=0
for i in $(seq 1 10 1580); do
  echo $i
  zn=$(printf %04d ${n})
  wasora constant-dt.fee $(echo ${i}/100 | bc -l) 1e-2 > anim-${zn}.dat
  tail -n1 anim-${zn}.dat > current-${zn}.dat
  n=$((n + 1))
cat << EOF | gnuplot -
set terminal png
set output "anim-${zn}.png"
set xrange [0:9]
set yrange [0:9]
unset key
plot "anim-${zn}.dat" u 2:3 w l lt 1, "current-${zn}.dat" u 2:3 w p pt 59 ps 2 lt 1, "anim-${zn}.dat" u 4:5 w l lt 2, "current-${zn}.dat" u 4:5 w p pt 52 ps 1.5 lt 2
EOF
done

avconv -y -f image2 -framerate 20 -i anim-%04d.png anim.gif
