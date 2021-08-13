#!/bin/bash

bcs="dirichlet neumann"
elems="tet4 tet10 hex8 hex20 hex27"
algos="struct delaunay hxt"
cs="12 16 20 24 28 32 36 38 40"

# if this flag is true then the error is computed by first writing
# T(x,y,z) into a vtk, converting it to .msh with meshio and
# then reading it back in FeenoX
# if it is false the error is computed directly in a single call to FeenoX
vtk=0

for i in feenox gmsh maxima meshio-convert tr; do
 if [ -z "$(which ${i})" ]; then
  echo "error: ${i} not installed"
  exit 1
 fi
done


if [ ! -z "$1" ]; then
 n=$1
else
 n=1
fi

. styles.sh


# compute the appropriate heat source 
## first read temperature and conductivity from FeenoX input
T=$(grep "T_mms(x,y,z) :=" thermal-cube${n}.fee | sed 's/T_mms(x,y,z)//' | sed 's/:=//')
T_mms=$(grep "T_mms(x,y,z) :=" thermal-cube${n}.fee)
k_mms=$(grep "k_mms(x,y,z) :=" thermal-cube${n}.fee)

## call maxima to compute q_mms(x)
maxima --very-quiet << EOF > /dev/null
${T_mms};
${k_mms};
q_mms(x,y,z) := -(diff(k_mms(x,y,z) * diff(T_mms(x,y,z), x), x) + diff(k_mms(x,y,z) * diff(T_mms(x,y,z), y), y) + diff(k_mms(x,y,z) * diff(T_mms(x,y,z), z), z));
stringout("thermal-cube${n}-q.txt", q_mms(x,y,z));
stringout("thermal-cube${n}-qx.txt", k_mms(x,y,z) * diff(T_mms(x,y,z), x));
stringout("thermal-cube${n}-qy.txt", k_mms(x,y,z) * diff(T_mms(x,y,z), y));
stringout("thermal-cube${n}-qz.txt", k_mms(x,y,z) * diff(T_mms(x,y,z), z));
EOF

## read back the string with q_mms(x) and store it in a FeenoX input
q=$(cat thermal-cube${n}-q.txt | tr -d ';\n')
qx=$(cat thermal-cube${n}-qx.txt | tr -d ';\n')
qy=$(cat thermal-cube${n}-qy.txt | tr -d ';\n')
qz=$(cat thermal-cube${n}-qz.txt | tr -d ';\n')

cat << EOF > thermal-cube${n}-q.fee
q_mms(x,y,z) := ${q}
qx_mms(x,y,z) := ${qx}
qy_mms(x,y,z) := ${qy}
qz_mms(x,y,z) := ${qz}
EOF

rm -f thermal-cube${n}-fits.ppl
echo "plot \\" > thermal-cube${n}-einf.ppl
echo "plot \\" > thermal-cube${n}-e2.ppl

for bc in ${bcs}; do
 for elem in ${elems}; do
  for algo in ${algos}; do
  
   # quads with non-struct lead to mixed meshes that have problems with meshio
   # (and that's not what we want to test)
   if [[ "x${elem}" == "xtet4" ]] || [[ "x${elem}" == "xtet10" ]] || [[ "x${algo}" == "xstruct" ]]; then
  
    dat="thermal-cube${n}_${bc}-${elem}-${algo}"
    rm -f ${dat}.dat
    echo ${dat}
    echo "------------------------------"
     
    for c in ${cs}; do
  
     name="thermal-cube${n}_${bc}-${elem}-${algo}-${c}"
   
     # prepare mesh
     if [ ! -e cube-${elem}-${algo}-${c}.msh ]; then
       lc=$(echo "PRINT 1/${c}" | feenox -)
       gmsh -v 0 -3 cube.geo ${elem}.geo ${algo}.geo -clscale ${lc} -o cube-${elem}-${algo}-${c}.msh
     fi
  
     if [ ${vtk} = 0 ]; then
      feenox thermal-cube${n}.fee ${bc} ${elem} ${algo} ${c} 0 | tee -a ${dat}.dat
     else
      if [ ! -e ${name}.vtk ]; then
       feenox thermal-cube${n}.fee ${bc} ${elem} ${algo} ${c} 1
      fi
       meshio-convert ${name}.vtk ${name}.msh 2> /dev/null
       feenox error3d.fee ${name} "${T}" ${c} | tee -a ${dat}.dat
     fi  
    done
  
    feenox fit.fee ${dat} | sed 's/-/_/g' >> thermal-cube${n}-fits.ppl
  
    cat << EOF >> thermal-cube${n}-einf.ppl
     "${dat}.dat"                              u 1:2 w lp pt ${pt[${elem}]} lw 1 lt 2 color ${co[${bc}${algo}]}  ti "${bc}-${elem}-${algo} = " + e_inf_thermal_cube${n}_${bc}_${elem}_${algo}_title,\\
EOF
    cat << EOF >> thermal-cube${n}-e2.ppl
     "${dat}.dat"                              u 1:3 w lp pt ${pt[${elem}]} lw 1 lt 2 color ${co[${bc}${algo}]}  ti "${bc}-${elem}-${algo} = " + e_inf_thermal_cube${n}_${bc}_${elem}_${algo}_title,\\
EOF

   fi   # tri & struct
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
