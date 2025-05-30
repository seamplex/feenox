#!/bin/bash

bcs="dirichlet"
elems="tet4 tet10 hex8 hex20 hex27"
algos="struct hxt"
cs="4 8 12 14 16 18"

vtk=0

for i in feenox gmsh maxima tr m4; do
 if [ -z "$(which ${i})" ]; then
  echo "error: ${i} not installed"
  exit 1
 fi
done

. styles.sh

u=$(grep "u_mms(x,y,z) =" mechanical-cube-mms.fee | sed 's/u_mms(x,y,z)//' | sed 's/=//' | sed 's/pi/%pi/')
v=$(grep "v_mms(x,y,z) =" mechanical-cube-mms.fee | sed 's/v_mms(x,y,z)//' | sed 's/=//' | sed 's/pi/%pi/')
w=$(grep "w_mms(x,y,z) =" mechanical-cube-mms.fee | sed 's/w_mms(x,y,z)//' | sed 's/=//' | sed 's/pi/%pi/')

maxima --very-quiet << EOF > /dev/null
/* Method of Manufactured Solutions - Small Deformation Linear Elasticity */
load("vect")$  
ratprint: false$

/* Material properties */
lambda: \\lambda$  
mu: \\mu$

/* Reference coordinates */
x: x$  
y: y$  
z: z$

/* Manufactured displacement field u(x,y,z) */
u: ${u}$  
v: ${v}$  
w: ${w}$

/* Small strain tensor epsilon = 0.5*(grad(u) + grad(u)^T) */
epsilon: matrix(
  [                   diff(u, x),       0.5*(diff(u, y) + diff(v, x)),       0.5*(diff(u, z) + diff(w, x))],
  [0.5*(diff(v, x) + diff(u, y)),                          diff(v, y),       0.5*(diff(v, z) + diff(w, y))],
  [0.5*(diff(w, x) + diff(u, z)),       0.5*(diff(w, y) + diff(v, z)),       diff(w, z)]
)$

/* Cauchy stress sigma = lambda*tr(epsilon)*I + 2*mu*epsilon */
tr_epsilon: epsilon[1,1] + epsilon[2,2] + epsilon[3,3]$
sigma: lambda * tr_epsilon * ident(3) + 2 * mu * epsilon$

/* Body force b = -Div(sigma) */
bx: -diff(sigma[1,1], x) - diff(sigma[1,2], y) - diff(sigma[1,3], z)$  
by: -diff(sigma[2,1], x) - diff(sigma[2,2], y) - diff(sigma[2,3], z)$  
bz: -diff(sigma[3,1], x) - diff(sigma[3,2], y) - diff(sigma[3,3], z)$

stringout("mechanical-sdefcube-bx.txt", bx);
stringout("mechanical-sdefcube-by.txt", by);
stringout("mechanical-sdefcube-bz.txt", bz);
EOF

## read back the strings and store them in a FeenoX input
bx=$(sed 's/%pi/pi/g' mechanical-sdefcube-bx.txt | tr -d ';\n')
by=$(sed 's/%pi/pi/g' mechanical-sdefcube-by.txt | tr -d ';\n')
bz=$(sed 's/%pi/pi/g' mechanical-sdefcube-bz.txt | tr -d ';\n')

cat << EOF > mechanical-sdefcube-b.fee
bx(x,y,z) = ${bx}
by(x,y,z) = ${by}
bz(x,y,z) = ${bz}
EOF


# report what we found
cat << EOF
# manufactured solution
u(x,y,z) = ${u}
v(x,y,z) = ${v}
w(x,y,z) = ${w}
# source terms
bx(x,y,z) = ${bx}
by(x,y,z) = ${by}
bz(x,y,z) = ${bz}

EOF

rm -f mechanical-sdefcube-fits.ppl
echo "plot \\" > mechanical-sdefcube-e2.ppl

for bc in ${bcs}; do
 for elem in ${elems}; do
  for algo in ${algos}; do
   dat="mechanical_sdefcube_${bc}_${elem}_${algo}"
   rm -f ${dat}.dat
   echo ${dat}
   echo "--------------------------------------------------------"

   if [ ${algo} = "hxt" ] && [ ${elem:0:3} = "hex" ]; then
     echo skiping
   else
    for c in ${cs}; do
     name="mechanical_sdefcube_${bc}-${elem}-${algo}-${c}"
   
     # prepare mesh
     if [ ! -e sdefcube-${elem}-${algo}-${c}.msh ]; then
       lc=$(echo "PRINT 1/${c}" | feenox -)
       gmsh -v 0 -3 cube.geo ${elem}.geo ${algo}.geo -clscale ${lc} -o cube-${elem}-${algo}-${c}.msh
     fi
     
     # run feenox
     feenox mechanical-sdefcube.fee ${bc} ${elem} ${algo} ${c} ${vtk} | tee -a ${dat}.dat
    done
  
    feenox fit.fee ${dat} >> mechanical-sdefcube-fits.ppl
  
    cat << EOF >> mechanical-sdefcube-e2.ppl
     "${dat}.dat"                              u (exp(\$1)):(exp(\$2)) w lp pt ${pt[${elem}]} lw 1 lt 2 color ${co[${bc}${algo}]}  ti "${bc}-${elem}-${algo} = " + e_2_mechanical_sdefcube_${bc}_${elem}_${algo}_title,\\
EOF
   fi
  done
 done
done

cat << EOF >> mechanical-sdefcube-e2.ppl
 x**2    w l lt 2 lw 4 color gray ti "\$h^2\$",\\
 x**3    w l lt 3 lw 4 color gray ti "\$h^3\$"
EOF

cat << EOF > mechanical-sdefcube-results.md

 boundary condition | element type | algorithm | order of convergence
--------------------|--------------|-----------|:----------------------:
EOF
grep ^# mechanical-sdefcube-fits.ppl | tr -d \# | sed s/mechanical_sdefcube_// | sed 's/_/ | /g' >> mechanical-sdefcube-results.md

if [ ! -z "$(which pyxplot)" ]; then
  pyxplot mechanical-sdefcube.ppl
  if [ ! -z "$(which pdf2svg)" ]; then
    pdf2svg mechanical-sdefcube-e2.pdf mechanical-sdefcube-e2.svg
    cat << EOF >> mechanical-sdefcube-results.md
    
![\$L-2\$ error for the small-deformation linear mechanical problem in FeenoX](mechanical-sdefcube-results.svg)

EOF
  fi
fi
