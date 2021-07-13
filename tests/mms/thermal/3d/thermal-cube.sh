#!/bin/bash

bcs="dirichlet neumann"
elems="tet4 tet10 hex8 hex20 hex27"
algos="struct delaunay hxt"
cs="4 8 12 16 24 32 36 42 48"
vtk=0

declare -A pt
pt['tet4']=4
pt['tet10']=18
pt['hex8']=2
pt['hex20']=16
pt['hex27']=17

declare -A lw
lw['tet4']=1
lw['tet10']=1
lw['hex8']=1
lw['hex20']=1
lw['hex27']=1

declare -A lt
lt['hxt']=4
lt['delaunay']=3
lt['struct']=2

declare -A co
co['dirichletstruct']="plum"
co['dirichletdelaunay']="rhodamine"
co['dirichlethxt']="salmon"
co['neumannstruct']="blue"
co['neumanndelaunay']="cyan"
co['neumannhxt']="seagreen"


for i in feenox gmsh maxima meshio-convert tr; do
 if [ -z "$(which ${i})" ]; then
  echo "error: ${i} not installed"
  exit 1
 fi
done

# compute the appropriate heat source 
## first read temperature and conductivity from FeenoX input
T=$(grep "T_mms(x,y,z) :=" thermal-cube.fee | sed 's/T_mms(x,y,z)//' | sed 's/:=//')
T_mms=$(grep "T_mms(x,y,z) :=" thermal-cube.fee)
k_mms=$(grep "k_mms(x,y,z) :=" thermal-cube.fee)

## call maxima to compute q_mms(x)
maxima --very-quiet << EOF > /dev/null
${T_mms};
${k_mms};
q_mms(x,y,z) := -(diff(k_mms(x,y,z) * diff(T_mms(x,y,z), x), x) + diff(k_mms(x,y,z) * diff(T_mms(x,y,z), y), y) + diff(k_mms(x,y,z) * diff(T_mms(x,y,z), z), z));
stringout("thermal-cube-q.txt", q_mms(x,y,z));
stringout("thermal-cube-qx.txt", diff(k_mms(x,y,z) * T_mms(x,y,z),x));
stringout("thermal-cube-qy.txt", diff(k_mms(x,y,z) * T_mms(x,y,z),y));
stringout("thermal-cube-qz.txt", diff(k_mms(x,y,z) * T_mms(x,y,z),z));
EOF

## read back the string with q_mms(x) and store it in a FeenoX input
q=$(cat thermal-cube-q.txt | tr -d ';\n')
qx=$(cat thermal-cube-qx.txt | tr -d ';\n')
qy=$(cat thermal-cube-qy.txt | tr -d ';\n')
qz=$(cat thermal-cube-qz.txt | tr -d ';\n')

cat << EOF > thermal-cube-q.fee
q_mms(x,y,z) := ${q}
qx_mms(x,y,z) := ${qx}
qy_mms(x,y,z) := ${qy}
qz_mms(x,y,z) := ${qz}
EOF

rm -f thermal-cube-fits.ppl
echo "plot \\" > thermal-cube-einf.ppl
echo "plot \\" > thermal-cube-e2.ppl

for bc in ${bcs}; do
 for elem in ${elems}; do
  for algo in ${algos}; do
  
   # quads with non-struct lead to mixed meshes that have problems with meshio
   # (and that's not what we want to test)
   if [[ "x${elem}" == "xtet4" ]] || [[ "x${elem}" == "xtet10" ]] || [[ "x${algo}" == "xstruct" ]]; then
  
    dat="thermal-cube_${bc}-${elem}-${algo}"
    rm -f ${dat}.dat
    echo ${dat}
    echo "------------------------------"
     
    for c in ${cs}; do
  
     name="thermal-cube_${bc}-${elem}-${algo}-${c}"
   
     # prepare mesh
     if [ ! -e cube-${elem}-${algo}-${c}.msh ]; then
       lc=$(echo "PRINT 1/${c}" | feenox -)
       gmsh -v 0 -3 cube.geo ${elem}.geo ${algo}.geo -clscale ${lc} -o cube-${elem}-${algo}-${c}.msh
     fi
  
     if [ ${vtk} = 0 ]; then
      feenox thermal-cube.fee ${bc} ${elem} ${algo} ${c} 0 | tee -a ${dat}.dat
     else
      if [ ! -e ${name}.vtk ]; then
       feenox thermal-cube.fee ${bc} ${elem} ${algo} ${c} 1
      fi
       meshio-convert ${name}.vtk ${name}.msh 2> /dev/null
       feenox error3d.fee ${name} "${T}" ${c} | tee -a ${dat}.dat
     fi  
    done
  
    feenox fit.fee ${dat} | sed 's/-/_/g' >> thermal-cube-fits.ppl
  
    cat << EOF >> thermal-cube-einf.ppl
     "${dat}.dat"                              u 1:2 w lp pt ${pt[${elem}]} lw 1 lt 2 color ${co[${bc}${algo}]}  ti "${bc}-${elem}-${algo} = " + e_inf_thermal_cube_${bc}_${elem}_${algo}_title,\\
e_inf_thermal_cube_${bc}_${elem}_${algo}(x)          w l                    lw 2 lt 1 color ${co[${bc}${algo}]}  ti "",\\
EOF
    cat << EOF >> thermal-cube-e2.ppl
     "${dat}.dat"                              u 1:3 w lp pt ${pt[${elem}]} lw 1 lt 2 color ${co[${bc}${algo}]}  ti "${bc}-${elem}-${algo} = " + e_inf_thermal_cube_${bc}_${elem}_${algo}_title,\\
  e_2_thermal_cube_${bc}_${elem}_${algo}(x)          w l                    lw 2 lt 1 color ${co[${bc}${algo}]}  ti "",\\
EOF

   fi   # tri & struct
  done
 done
done

cat << EOF >> thermal-cube-einf.ppl
 x**2    w l lt 2 lw 4 color gray ti "\$h^2\$",\\
 x**3    w l lt 3 lw 4 color gray ti "\$h^3\$"
EOF

cat << EOF >> thermal-cube-e2.ppl
 x**2    w l lt 2 lw 4 color gray ti "\$h^2\$",\\
 x**3    w l lt 3 lw 4 color gray ti "\$h^3\$"
EOF
