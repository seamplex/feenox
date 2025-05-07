#!/bin/bash -e
# 
bcs="dirichlet neumann"
elems="tri3 tri6 quad4 quad8 quad9"
algos="struct frontal delaunay"
cs="4 6 8 10 12 16 20 24 30 36 48"

# set this flag to 1 if you want to create one VTK for each run
vtk=0

. styles.sh


for i in feenox gmsh maxima tr; do
 if [ -z "$(which ${i})" ]; then
  echo "error: ${i} not installed"
  exit 1
 fi
done

# compute the appropriate heat source 
## first read temperature and conductivity from FeenoX input
T=$(grep "T_mms(x,y) =" thermal-square.fee | sed 's/T_mms(x,y)//' | sed 's/=//')
T_mms=$(grep "T_mms(x,y) =" thermal-square.fee | sed 's/=/:=/')
k_mms=$(grep "k_mms(x,y) =" thermal-square.fee | sed 's/=/:=/')

## then ask maxima to compute q_mms(x)
maxima --very-quiet << EOF > /dev/null
${T_mms};
${k_mms};
q_mms(x,y) := -(diff(k_mms(x,y) * diff(T_mms(x,y), x), x) + diff(k_mms(x,y) * diff(T_mms(x,y), y), y));
stringout("thermal-square-q.txt", q_mms(x,y));
stringout("thermal-square-qx.txt", -k_mms(x,y) * diff(T_mms(x,y),x));
stringout("thermal-square-qy.txt", -k_mms(x,y) * diff(T_mms(x,y),y));
EOF

## read back the string with q_mms(x) and store it in a FeenoX input
q=$(cat thermal-square-q.txt | tr -d ';\n')
qx=$(cat thermal-square-qx.txt | tr -d ';\n')
qy=$(cat thermal-square-qy.txt | tr -d ';\n')

cat << EOF > thermal-square-q.fee
q_mms(x,y) = ${q}
qx_mms(x,y) = ${qx}
qy_mms(x,y) = ${qy}
EOF

# report what we found
cat << EOF
# manufactured solution
${T_mms}
${k_mms}
# source terms
q_mms(x,y) = ${q}
qx_mms(x,y) = ${qx}
qy_mms(x,y) = ${qy}

EOF


rm -f thermal-square-fits.ppl
echo "plot \\" > thermal-square-einf.ppl
echo "plot \\" > thermal-square-e2.ppl

for bc in ${bcs}; do
 for elem in ${elems}; do
  for algo in ${algos}; do
  
    dat="thermal_square_${bc}_${elem}_${algo}"
    rm -f ${dat}.dat
    echo ${dat}
    echo "--------------------------------------------------------"
     
    for c in ${cs}; do
  
     name="thermal_square_${bc}-${elem}-${algo}-${c}"
   
     # prepare mesh
     if [ ! -e square-${elem}-${algo}-${c}.msh ]; then
       lc=$(echo "PRINT 1/${c}" | feenox -)
       gmsh -v 0 -2 square.geo ${elem}.geo ${algo}.geo -clscale ${lc} -o square-${elem}-${algo}-${c}.msh
     fi
   
     # run feenox
     feenox thermal-square.fee ${bc} ${elem} ${algo} ${c} ${vtk} | tee -a ${dat}.dat 
      
    done
 
    feenox fit.fee ${dat}  >> thermal-square-fits.ppl
  
    cat << EOF >> thermal-square-einf.ppl
     "${dat}.dat"                              u (exp(\$1)):(exp(\$2)) w lp pt ${pt[${elem}]} lw 1 lt 2 color ${co[${bc}${algo}]}  ti "${bc}-${elem}-${algo} = " + e_inf_thermal_square_${bc}_${elem}_${algo}_title,\\
e_inf_thermal_square_${bc}_${elem}_${algo}(x)        w l                    lw 2 lt 1 color ${co[${bc}${algo}]}  ti "",\\
EOF
    cat << EOF >> thermal-square-e2.ppl
     "${dat}.dat"                              u (exp(\$1)):(exp(\$3)) w lp pt ${pt[${elem}]} lw 1 lt 2 color ${co[${bc}${algo}]}  ti "${bc}-${elem}-${algo} = " + e_inf_thermal_square_${bc}_${elem}_${algo}_title,\\
  e_2_thermal_square_${bc}_${elem}_${algo}(x)        w l                    lw 2 lt 1 color ${co[${bc}${algo}]}  ti "",\\
EOF

  done
 done
done

cat << EOF >> thermal-square-einf.ppl
 x**2    w l lt 2 lw 4 color gray ti "\$h^2\$",\\
 x**3    w l lt 3 lw 4 color gray ti "\$h^3\$"
EOF

cat << EOF >> thermal-square-e2.ppl
 x**2    w l lt 2 lw 4 color gray ti "\$h^2\$",\\
 x**3    w l lt 3 lw 4 color gray ti "\$h^3\$"
EOF

cat << EOF > thermal-square-results.md

 Boundary condition | Element type | Algorithm | Order of convergence
--------------------|--------------|-----------|:----------------------:
EOF
grep ^# thermal-square-fits.ppl | tr -d \# | sed s/thermal_square_// | sed 's/_/ | /g' >> thermal-square-results.md

if [ ! -z "$(which pyxplot)" ]; then
  pyxplot thermal-square.ppl
  if [ ! -z "$(which pdf2svg)" ]; then
    pdf2svg thermal-square-e2.pdf thermal-square-e2.svg
    cat << EOF >> thermal-square-results.md
    
![\$L-2\$ error for the 2d heat conduction problem in FeenoX](thermal-square-results.svg)

EOF
  fi
fi
