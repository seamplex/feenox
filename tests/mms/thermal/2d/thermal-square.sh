#!/bin/bash

bcs="dirichlet neumann"
elems="tri3 tri6 quad4 quad8 quad9"
algos="struct frontal delaunay"
# cs="8 16 24 28 32 36 48 64 72 96 112 128 160 192 256"
cs="8 16 24 28 32 36 40 48 64 72 96 104 112"
vtk=0

declare -A pt
pt['tri3']=4
pt['tri6']=18
pt['quad4']=2
pt['quad8']=16
pt['quad9']=17

declare -A lw
lw['tri3']=1
lw['tri6']=1
lw['quad4']=1
lw['quad8']=1
lw['quad9']=1

declare -A lt
lt['delaunay']=4
lt['frontal']=3
lt['struct']=2

declare -A co
co['dirichletstruct']="plum"
co['dirichletfrontal']="rhodamine"
co['dirichletdelaunay']="salmon"
co['neumannstruct']="blue"
co['neumannfrontal']="cyan"
co['neumanndelaunay']="seagreen"


for i in feenox gmsh maxima meshio-convert tr; do
 if [ -z "$(which ${i})" ]; then
  echo "error: ${i} not installed"
  exit 1
 fi
done

# compute the appropriate heat source 
## first read temperature and conductivity from FeenoX input
T=$(grep "T_mms(x,y) :=" thermal-square.fee | sed 's/T_mms(x,y)//' | sed 's/:=//')
T_mms=$(grep "T_mms(x,y) :=" thermal-square.fee)
k_mms=$(grep "k_mms(x,y) :=" thermal-square.fee)

## call maxima to compute q_mms(x)
maxima --very-quiet << EOF > /dev/null
${T_mms};
${k_mms};
q_mms(x,y) := -(diff(k_mms(x,y) * diff(T_mms(x,y), x), x) + diff(k_mms(x,y) * diff(T_mms(x,y), y), y));
stringout("thermal-square-q.txt", q_mms(x,y));
stringout("thermal-square-qx.txt", diff(k_mms(x,y) * T_mms(x,y),x));
stringout("thermal-square-qy.txt", diff(k_mms(x,y) * T_mms(x,y),y));
EOF

## read back the string with q_mms(x) and store it in a FeenoX input
q=$(cat thermal-square-q.txt | tr -d ';\n')
qx=$(cat thermal-square-qx.txt | tr -d ';\n')
qy=$(cat thermal-square-qy.txt | tr -d ';\n')

cat << EOF > thermal-square-q.fee
q_mms(x,y) := ${q}
qx_mms(x,y) := ${qx}
qy_mms(x,y) := ${qy}
EOF

rm -f thermal-square-fits.ppl
echo "plot \\" > thermal-square-einf.ppl
echo "plot \\" > thermal-square-e2.ppl

for bc in ${bcs}; do
 for elem in ${elems}; do
  for algo in ${algos}; do
  
   # quads with non-struct lead to mixed meshes that have problems with meshio
   # (and that's not what we want to test)
   if [[ "x${elem}" == "xtri3" ]] || [[ "x${elem}" == "xtri6" ]] || [[ "x${algo}" == "xstruct" ]]; then
  
    dat="thermal-square_${bc}-${elem}-${algo}"
    rm -f ${dat}.dat
    echo ${dat}
    echo "------------------------------"
     
    for c in ${cs}; do
  
     name="thermal-square_${bc}-${elem}-${algo}-${c}"
   
     # prepare mesh
     if [ ! -e square-${elem}-${algo}-${c}.msh ]; then
       lc=$(echo "PRINT 1/${c}" | feenox -)
       gmsh -v 0 -2 square.geo ${elem}.geo ${algo}.geo -clscale ${lc} -o square-${elem}-${algo}-${c}.msh
     fi
   
     if [ ${vtk} = 0 ]; then
      feenox thermal-square.fee ${bc} ${elem} ${algo} ${c} 0 | tee -a ${dat}.dat
     else
      if [ ! -e ${name}.vtk ]; then
       feenox thermal-square.fee ${bc} ${elem} ${algo} ${c} 1
      fi
      meshio-convert ${name}.vtk ${name}.msh 2> /dev/null
      feenox error2d.fee ${name} "${T}" ${c} | tee -a ${dat}.dat
     fi  
      
    done
  
    feenox fit.fee ${dat} | sed 's/-/_/g' >> thermal-square-fits.ppl
  
    cat << EOF >> thermal-square-einf.ppl
     "${dat}.dat"                              u 1:2 w lp pt ${pt[${elem}]} lw 1 lt 2 color ${co[${bc}${algo}]}  ti "${bc}-${elem}-${algo} = " + e_inf_thermal_square_${bc}_${elem}_${algo}_title,\\
e_inf_thermal_square_${bc}_${elem}_${algo}(x)        w l                    lw 2 lt 1 color ${co[${bc}${algo}]}  ti "",\\
EOF
    cat << EOF >> thermal-square-e2.ppl
     "${dat}.dat"                              u 1:3 w lp pt ${pt[${elem}]} lw 1 lt 2 color ${co[${bc}${algo}]}  ti "${bc}-${elem}-${algo} = " + e_inf_thermal_square_${bc}_${elem}_${algo}_title,\\
  e_2_thermal_square_${bc}_${elem}_${algo}(x)        w l                    lw 2 lt 1 color ${co[${bc}${algo}]}  ti "",\\
EOF

   fi   # tri & struct
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
