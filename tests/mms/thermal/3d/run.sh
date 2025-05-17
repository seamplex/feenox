#!/bin/bash

bcs="dirichlet neumann"
elems="tet4 tet10 hex8 hex20 hex27"
algos="struct hxt"
cs="4 8 12 16 20 24 28 32"

# set this flag to 1 if you want to create one VTK for each run
vtk=0


for i in feenox gmsh maxima tr m4; do
 if [ -z "$(which ${i})" ]; then
  echo "error: ${i} not installed"
  exit 1
 fi
done

# n = 1 is steady-state uniform conductivity
# n = 2 is steady-state space-dependent conductivity
# n = 3 is steady-state temperature-dependent
# n = 4 is steady-state space & temperature-dependent
if [ ! -z "$1" ]; then
 ns=$1
else
 ns="1 2 3 4"
fi

. styles.sh
rm -rf thermal-cube-results.md

for n in ${ns}; do
 # for transient we need to change this line
 m4 -Dn=${n} thermal-cube-static.fee > thermal-cube${n}.fee
 
 # compute the appropriate heat source 
 ## first read temperature and conductivity from FeenoX input
 T=$(grep "T_mms(x,y,z) =" thermal-cube${n}-T.fee | sed 's/T_mms(x,y,z)//' | sed 's/=//')
 T_mms=$(grep "T_mms(x,y,z) =" thermal-cube${n}-T.fee | sed 's/=/:=/')
 k_mms=$(grep "k_mms(x,y,z) =" thermal-cube${n}-T.fee | sed 's/=/:=/')
 
 ## call maxima to compute q_mms(x)
 ## The T(x,y,z) without the _mms is needed if k depends on T because maxima needs to know the T in the derivative of k wrt T
 maxima --very-quiet << EOF > /dev/null
 ${T_mms};
 ${k_mms};
 T(x,y,z) := ${T}; 
 q_mms(x,y,z) := -(diff(k_mms(x,y,z) * diff(T_mms(x,y,z), x), x) + diff(k_mms(x,y,z) * diff(T_mms(x,y,z), y), y) + diff(k_mms(x,y,z) * diff(T_mms(x,y,z), z), z));
 stringout("thermal-cube${n}-q.txt", q_mms(x,y,z));
 stringout("thermal-cube${n}-qx.txt", -k_mms(x,y,z) * diff(T_mms(x,y,z), x));
 stringout("thermal-cube${n}-qy.txt", -k_mms(x,y,z) * diff(T_mms(x,y,z), y));
 stringout("thermal-cube${n}-qz.txt", -k_mms(x,y,z) * diff(T_mms(x,y,z), z));
EOF

 ## read back the string with q_mms(x) and store it in a FeenoX input
 q=$(cat thermal-cube${n}-q.txt | tr -d ';\n')
 qx=$(cat thermal-cube${n}-qx.txt | tr -d ';\n')
 qy=$(cat thermal-cube${n}-qy.txt | tr -d ';\n')
 qz=$(cat thermal-cube${n}-qz.txt | tr -d ';\n')
 
 cat << EOF > thermal-cube${n}-q.fee
 q_mms(x,y,z) = ${q}
 qx_mms(x,y,z) = ${qx}
 qy_mms(x,y,z) = ${qy}
 qz_mms(x,y,z) = ${qz}
EOF


 # report what we found
 cat << EOF
 # manufactured solution
 ${T_mms}
 ${k_mms}
 # source terms
 q_mms(x,y,z) = ${q}
 qx_mms(x,y,z) = ${qx}
 qy_mms(x,y,z) = ${qy}
 qz_mms(x,y,z) = ${qz}
EOF


 rm -f thermal-cube${n}-fits.ppl
 echo "plot \\" > thermal-cube${n}-einf.ppl
 echo "plot \\" > thermal-cube${n}-e2.ppl

 for bc in ${bcs}; do
  for elem in ${elems}; do
   for algo in ${algos}; do

    dat="thermal_cube${n}_${bc}_${elem}_${algo}"
    rm -f ${dat}.dat
    echo ${dat}
    echo "--------------------------------------------------------"
    
    if [ ${algo} = "hxt" ] && [ ${elem:0:3} = "hex" ]; then
     echo skiping
    else
   
     for c in ${cs}; do
  
      name="thermal_cube${n}_${bc}-${elem}-${algo}-${c}"
   
      # prepare mesh
      if [ ! -e cube-${elem}-${algo}-${c}.msh ]; then
       lc=$(echo "PRINT 1/${c}" | feenox -)
       gmsh -v 0 -3 cube.geo ${elem}.geo ${algo}.geo -clscale ${lc} -o cube-${elem}-${algo}-${c}.msh
      fi
     
      # run feenox
      feenox thermal-cube${n}.fee ${bc} ${elem} ${algo} ${c} ${vtk} | tee -a ${dat}.dat

     done
  
     feenox fit.fee ${dat} >> thermal-cube${n}-fits.ppl
  
     cat << EOF >> thermal-cube${n}-einf.ppl
      "${dat}.dat"                              u (exp(\$1)):(exp(\$2)) w lp pt ${pt[${elem}]} lw 1 lt 2 color ${co[${bc}${algo}]}  ti "${bc}-${elem}-${algo} = " + e_inf_thermal_cube${n}_${bc}_${elem}_${algo}_title,\\
EOF
     cat << EOF >> thermal-cube${n}-e2.ppl
      "${dat}.dat"                              u (exp(\$1)):(exp(\$3)) w lp pt ${pt[${elem}]} lw 1 lt 2 color ${co[${bc}${algo}]}  ti "${bc}-${elem}-${algo} = " + e_inf_thermal_cube${n}_${bc}_${elem}_${algo}_title,\\
EOF
    fi
   done
  done
 done

 cat << EOF >> thermal-cube${n}-einf.ppl
  x**2    w l lt 2 lw 4 color gray ti "\$h^2\$",\\
  x**3    w l lt 3 lw 4 color gray ti "\$h^3\$"
EOF

 cat << EOF >> thermal-cube${n}-e2.ppl
  x**2    w l lt 2 lw 4 color gray ti "\$h^2\$",\\
  x**3    w l lt 3 lw 4 color gray ti "\$h^3\$"
EOF
 m4 -Dn=${n} thermal-cube.ppl.m4 > thermal-cube${n}.ppl

 cat << EOF >> thermal-cube-results.md

 boundary condition | element type | algorithm | order of convergence
--------------------|--------------|-----------|:----------------------:
EOF
 grep ^# thermal-cube${n}-fits.ppl | tr -d \# | sed s/thermal_cube${n}_// | sed 's/_/ | /g' >> thermal-cube${n}-results.md
 
 if [ ! -z "$(which pyxplot)" ]; then
  pyxplot thermal-cube${n}.ppl
  if [ ! -z "$(which pdf2svg)" ]; then
   pdf2svg thermal-cube${n}-e2.pdf thermal-cube${n}-e2.svg
   cat << EOF >> thermal-cube-results.md
    
![\$L-2\$ error for the 2d heat conduction problem in FeenoX](thermal-cube${n}-results.svg)

EOF
  fi
 fi
 
done
