#!/bin/bash -e

bcs="dirichlet"
elems="tet4 tet10 hex8 hex20 hex27"
algos="struct hxt"
cs="2 3 4 6 8"

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
/* Method of Manufactured Solutions for Large Deformation Elasticity */
/* Computes b0 = -Divergence(F*S) in reference configuration */

/* Step 1: Define symbolic variables */
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

/* Deformation gradient F = I + grad(u) */
F: matrix(
  [1 + diff(u, x),      diff(u, y),      diff(u, z)],
  [diff(v, x),      1 + diff(v, y),      diff(v, z)],
  [diff(w, x),          diff(w, y),  1 + diff(w, z)]
)$

/* Green-Lagrange strain E = 0.5*(F^T.F - I) */
E: 0.5 * (transpose(F).F - ident(3))$

/* Second Piola-Kirchhoff stress S = lambda*tr(E)*I + 2*mu*E */
trE: E[1,1] + E[2,2] + E[3,3]$
S: lambda * trE * ident(3) + 2 * mu * E$

/* First Piola-Kirchhoff stress P = F.S */
P: F.S$

/* Divergence of P to get body force b0 = -Div(P) */
b0_x: -diff(P[1,1], x) - diff(P[1,2], y) - diff(P[1,3], z)$  
b0_y: -diff(P[2,1], x) - diff(P[2,2], y) - diff(P[2,3], z)$  
b0_z: -diff(P[3,1], x) - diff(P[3,2], y) - diff(P[3,3], z)$


stringout("mechanical-ldefcube-bx.txt", b0_x);
stringout("mechanical-ldefcube-by.txt", b0_y);
stringout("mechanical-ldefcube-bz.txt", b0_z);
EOF

## read back the strings and store them in a FeenoX input
bx=$(sed 's/%pi/pi/g' mechanical-ldefcube-bx.txt | tr -d ';\n')
by=$(sed 's/%pi/pi/g' mechanical-ldefcube-by.txt | tr -d ';\n')
bz=$(sed 's/%pi/pi/g' mechanical-ldefcube-bz.txt | tr -d ';\n')

cat << EOF > mechanical-ldefcube-b.fee
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

rm -f mechanical-ldefcube-fits.ppl
echo "plot \\" > mechanical-ldefcube-e2.ppl

for bc in ${bcs}; do
 for elem in ${elems}; do
  for algo in ${algos}; do
   dat="mechanical_ldefcube_${bc}_${elem}_${algo}"
   rm -f ${dat}.dat
   echo ${dat}
   echo "--------------------------------------------------------"

   if [ ${algo} = "hxt" ] && [ ${elem:0:3} = "hex" ]; then
     echo skiping
   else
    for c in ${cs}; do
     name="mechanical_ldefcube_${bc}-${elem}-${algo}-${c}"
   
     # prepare mesh
     if [ ! -e ldefcube-${elem}-${algo}-${c}.msh ]; then
       lc=$(echo "PRINT 1/${c}" | feenox -)
       gmsh -v 0 -3 cube.geo ${elem}.geo ${algo}.geo -clscale ${lc} -o cube-${elem}-${algo}-${c}.msh
     fi
     
     # run feenox
     feenox mechanical-ldefcube.fee ${bc} ${elem} ${algo} ${c} ${vtk} | tee -a ${dat}.dat
    done
  
    feenox fit.fee ${dat} >> mechanical-ldefcube-fits.ppl
  
    cat << EOF >> mechanical-ldefcube-e2.ppl
     "${dat}.dat"                              u (exp(\$1)):(exp(\$2)) w lp pt ${pt[${elem}]} lw 1 lt 2 color ${co[${bc}${algo}]}  ti "${bc}-${elem}-${algo} = " + e_2_mechanical_ldefcube_${bc}_${elem}_${algo}_title,\\
EOF
   fi
  done
 done
done

cat << EOF >> mechanical-ldefcube-e2.ppl
 x**2    w l lt 2 lw 4 color gray ti "\$h^2\$",\\
 x**3    w l lt 3 lw 4 color gray ti "\$h^3\$"
EOF

cat << EOF > mechanical-ldefcube-results.md

 boundary condition | element type | algorithm | order of convergence
--------------------|--------------|-----------|:----------------------:
EOF
grep ^# mechanical-ldefcube-fits.ppl | tr -d \# | sed s/mechanical_ldefcube_// | sed 's/_/ | /g' >> mechanical-ldefcube-results.md

if [ ! -z "$(which pyxplot)" ]; then
  pyxplot mechanical-ldefcube.ppl
  if [ ! -z "$(which pdf2svg)" ]; then
    pdf2svg mechanical-ldefcube-e2.pdf mechanical-ldefcube-e2.svg
    cat << EOF >> mechanical-ldefcube-results.md
    
![\$L-2\$ error for the large-deformation non-linear mechanical problem in FeenoX](mechanical-ldefcube-results.svg)

EOF
  fi
fi
