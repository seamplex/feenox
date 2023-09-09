#!/bin/sh

thetas="0 5 15 30 45"
sns="4 6 8 12"
scales="4 3 2 1"

for theta in $thetas; do
 echo "angle = ${theta};" > azmy-angle-${theta}.geo
 for scale in $scales; do
  gmsh -v 0 -2 azmy-angle-${theta}.geo azmy-full.geo -clscale ${scale} -o azmy-full-${theta}.msh
  for sn in $sns; do
   echo $theta $scale $sn
   feenox azmy-full.fee ${theta} ${sn} ${scale} --progress
  done
 done
done

# fixed theta & sn, variable scale
rm -f azmy-full-fixed-theta-fixed-sn.ppl
for theta in $thetas; do
 for sn in $sn; do
 
  echo "set title \"Fixed \$\\\theta=${theta} \\\textdegree\$, \$N=${sn}\$\"" >> azmy-full-fixed-theta-fixed-sn.ppl
  echo "set output \"azmy-full-${theta}-${sn}.pdf\"" >> azmy-full-fixed-theta-fixed-sn.ppl 
  echo -n "plot " >> azmy-full-fixed-theta-fixed-sn.ppl
  for scale in $scales; do
   echo -n " \"azmy-full-${theta}-${sn}-${scale}.dat\" w l lw 2*$scale ti \"\$c=${scale}\$\"," >> azmy-full-fixed-theta-fixed-sn.ppl
  done
  echo "0" >> azmy-full-fixed-theta-fixed-sn.ppl
  echo  >> azmy-full-fixed-theta-fixed-sn.ppl
 done
done


