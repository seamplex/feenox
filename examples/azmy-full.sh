#!/bin/bash

thetas="0 15 30 45"
cs="4 3 2 1.5 1"
sns="4 6 8 10 12"

# for theta in ${thetas}; do
#  echo "angle = ${theta};" > azmy-angle-${theta}.geo
#  for c in ${cs}; do
#   gmsh -v 0 -2 azmy-angle-${theta}.geo azmy-full.geo -clscale ${c} -o azmy-full-${theta}.msh
#   for sn in ${sns}; do
#    if [ ! -e azmy-full-${theta}-${sn}-${c}.dat ]; then
#      echo ${theta} ${c} ${sn}
#      feenox azmy-full.fee ${theta} ${sn} ${c} --progress
#    fi
#   done
#  done
# done

color=(grey blue green tan cyan)


# fixed theta & sn, variable c
rm -f azmy-full-fixed-theta-fixed-sn.ppl
for theta in ${thetas}; do
 for sn in ${sns}; do
 
  echo "set title \"Fixed \$\\\theta=${theta} \\\textdegree\$, \$N=${sn}\$\"" >> azmy-full-fixed-theta-fixed-sn.ppl
  echo "set output \"azmy-full-theta-${theta}-sn-${sn}.pdf\"" >> azmy-full-fixed-theta-fixed-sn.ppl 
  echo -n "plot " >> azmy-full-fixed-theta-fixed-sn.ppl
  i=0
  for c in ${cs}; do
#    if [[ "${sn}" != "10" || "${c}" != "1" ]] && [[ "${sn}" != "12" || "${c}" != "1.5" ]] && [[ "${sn}" != "12" || "${c}" != "1" ]]; then
   if [ -e azmy-full-${theta}-${sn}-${c}.dat ]; then
   
    if [ $i != 0 ]; then
     echo -n ", " >> azmy-full-fixed-theta-fixed-sn.ppl
    fi
   
    echo -n " \"azmy-full-${theta}-${sn}-${c}.dat\" w l lw 2*${c} color ${color[${i}]} ti \"\$c=${c}\$\"" >> azmy-full-fixed-theta-fixed-sn.ppl
    let i++
    
   fi
  done
  echo  >> azmy-full-fixed-theta-fixed-sn.ppl
 done
done


# fixed theta & c, variable sn
rm -f azmy-full-fixed-theta-fixed-c.ppl
for theta in ${thetas}; do
 for c in ${cs}; do
 
  echo "set title \"Fixed \$\\\theta=${theta} \\\textdegree\$, \$c=${c}\$\"" >> azmy-full-fixed-theta-fixed-c.ppl
  echo "set output \"azmy-full-theta-${theta}-c-${c}.pdf\"" >> azmy-full-fixed-theta-fixed-c.ppl 
  echo -n "plot " >> azmy-full-fixed-theta-fixed-c.ppl
  i=0
  for sn in ${sns}; do
#    if [[ "${sn}" != "10" || "${c}" != "1" ]] && [[ "${sn}" != "12" || "${c}" != "1.5" ]] && [[ "${sn}" != "12" || "${c}" != "1" ]]; then
   if [ -e azmy-full-${theta}-${sn}-${c}.dat ]; then
   
    if [ $i != 0 ]; then
     echo -n ", " >> azmy-full-fixed-theta-fixed-c.ppl
    fi
    
    echo -n " \"azmy-full-${theta}-${sn}-${c}.dat\" w l lw 0.5*(16-${sn}) color ${color[${i}]} ti \"\$N=${sn}\$\"" >> azmy-full-fixed-theta-fixed-c.ppl
    let i++
    
   fi
  done
  echo  >> azmy-full-fixed-theta-fixed-c.ppl
 done
done


# fixed sn & c, variable theta
rm -f azmy-full-fixed-sn-fixed-c.ppl
for sn in ${sns}; do
 for c in ${cs}; do

  if [ -e azmy-full-${theta}-${sn}-${c}.dat ]; then
#   if [[ "${sn}" != "10" || "${c}" != "1" ]] && [[ "${sn}" != "12" || "${c}" != "1.5" ]] && [[ "${sn}" != "12" || "${c}" != "1" ]]; then
 
   echo "set title \"Fixed \$N=${sn}\$, \$c=${c}\$\"" >> azmy-full-fixed-sn-fixed-c.ppl
   echo "set output \"azmy-full-sn-${sn}-c-${c}.pdf\"" >> azmy-full-fixed-sn-fixed-c.ppl 
   echo -n "plot " >> azmy-full-fixed-sn-fixed-c.ppl
   i=0
   for theta in ${thetas}; do
   
    if [ $i != 0 ]; then
      echo -n ", " >> azmy-full-fixed-sn-fixed-c.ppl
    fi
    
    echo -n " \"azmy-full-${theta}-${sn}-${c}.dat\" w l lw 2+0.25*(${theta}-45) color ${color[${i}]} ti \"\$\\\theta=${theta}\$\"" >> azmy-full-fixed-sn-fixed-c.ppl
    let i++
    
   done
   echo  >> azmy-full-fixed-sn-fixed-c.ppl
   
  fi
 done
done
