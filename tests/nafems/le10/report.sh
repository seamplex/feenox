#!/bin/bash

if [ -z "${1}" ]; then 
  echo "usage: $0 { tet | hex }"
  exit 0  
fi

declare -A color
color["feenox"]="black"
color["sparselizard"]="blue"
color["aster"]="dark-green"
color["calculix"]="orange"
color["reflex"]="purple"

declare -A lw
lw["feenox"]="4"
lw["sparselizard"]="3"
lw["aster"]="2"
lw["calculix"]="1"
lw["reflex"]="1"

declare -A lt
lt["gamg"]="1"
lt["mumps"]="2"
lt["spooles"]="3"
lt["default"]="4"
lt["diagonal"]="5"
lt["cholesky"]="6"
lt["hypre"]="7"

if [ "x$(ls *${1}*.dat | wc -l)" != "x0" ]; then

  cat << EOF > report-${1}.md
---
title: Parametric NAFEMS LE10 benchmark with ${1} elements
subtitle: Comparison of resource consumption for different FEA programs
lang: en-US
geometry:
- left=2cm
- right=2cm
- bottom=2.5cm
- foot=1cm
- top=2.5cm
- head=1cm
toc: true
...
EOF

  # table with architecture
  if [ -e arch-${1}.md ]; then
    cat << EOF >> report-${1}.md
    
# Architecture
    
\\rowcolors{1}{black!0}{black!10}    

EOF
    cat arch-${1}.md >> report-${1}.md
  fi

    cat << EOF >> report-${1}.md
    
# Codes & versions
    
EOF

  for i in version_*.txt; do
    program=$(echo $(basename ${i} .txt) | cut -d"_" -f 2)
    cat << EOF >> report-${1}.md
    
## ${program}

\`\`\`terminal
$(cat ${i})
\`\`\`
    
EOF
  done

  
  # terminal with reference
  if [ -e le10-ref.txt ]; then
  cat << EOF >> report-${1}.md
  
# Reference solution
  
\`\`\`terminal
$ time feenox le10-ref.fee
$(cat le10-ref.txt)
$
\`\`\`

EOF
  fi
  
cat << EOF >> report-${1}.md
# Initial mesh

![Coarser ${1} mesh for \$c=1\$](le10-${1}-mesh.png){width=90%}

EOF

  # figures
  cat << EOF  >> report-${1}.md
\\clearpage
\\newpage
# Figures

EOF
  echo -n "plot " > plot-${1}.gp

  for i in feenox_*.dat sparselizard_*.dat aster_*.dat calculix_*.dat reflex_*.dat; do
    if [ -e ${i} ]; then
      program=$(echo $(basename ${i} .dat) | cut -d"_" -f 1)
      solver=$(echo $(basename ${i} .dat) | cut -d"_" -f 2)
      shape=$(echo $(basename ${i} .dat) | cut -d"_" -f 3)
  
      if [ "x${shape}" = "x${1}" ]; then
        echo ${program} ${solver} ${shape}
        echo -n "\"${i}s\" u cx:cy  w lp lw ${lw[$program]} pt ${lt[$solver]} dashtype ${lt[$solver]}  lc \"${color[$program]}\"  ti \"${program} ${solver}\", " >> plot-${1}.gp
        sort -r -g ${i} | grep -v nan | grep -v exited | grep -v -e "[[:blank:]]0[[:blank:]]" > ${i}s
      fi
    fi
  done

  cat plot-${1}.gp | tr -d '\n' > plot-${1}-sigmay.gp
  echo "-5.38 w l lw 1 lc \"red\" ti \"reference\"" >> plot-${1}-sigmay.gp

  if [ ! -z "$(which gnuplot)" ]; then
    sed s/xxx/${1}/ figures.gp | gnuplot -  
  else
    echo "gnuplot not installed, not creating the plots, just the data files *.dats"
  fi

  for i in sigmay*${1}.svg wall*${1}.svg memory*${1}.svg user*${1}.svg kernel*${1}.svg; do
    echo "![](${i})\\ " >> report-${1}.md
    echo >> report-${1}.md
  done
  echo >> report-${1}.md

  
  # tables with data
    cat << EOF  >> report-${1}.md
\\clearpage
\\newpage
# Tables

EOF
  
  for c in $(cat *.dat | awk '{print $1}' | sort -rg | uniq); do

    rm -f tmp-table.md
    for i in feenox_*.dat sparselizard_*.dat aster_*.dat calculix_*.dat reflex_*.dat; do
      if [ -e ${i} ]; then
        program=$(echo $(basename ${i} .dat) | cut -d"_" -f 1)
        solver=$(echo $(basename ${i} .dat) | cut -d"_" -f 2)
        shape=$(echo $(basename ${i} .dat) | cut -d"_" -f 3)
      
        if [ "x${shape}" = "x${1}" ]; then
          grep -w ^${c} ${i} | gawk -vprogram=${program} -vsolver=${solver} '{printf("%s | %s | %\047d | %.3f | %.1f | %.1f | %.1f  | %.2f\n", program, solver, $2, $3, $4, $5, $6, $7/(1024*1024));}'  >> tmp-table.md
        fi
      fi
    done
  
    cat << EOF  >> report-${1}.md
## \$c=${c}\$
    
\\rowcolors{1}{black!0}{black!10}    
    
Program | Solver | DOFs | $\\sigma_y$ | Wall [s] | Kernel [s] | User [s] | Memory [Gb]
--------|--------|-----:|------------:|---------:|-----------:|---------:|------------:
EOF
  
    sort -g -k 9 tmp-table.md >> report-${1}.md
    cat << EOF  >> report-${1}.md
 
Table: \$c=${c}\$ sorted by wall time


EOF


    cat << EOF  >> report-${1}.md
\\rowcolors{1}{black!0}{black!10}    
    
Program | Solver | DOFs | $\\sigma_y$ | Wall [s] | Kernel [s] | User [s] | Memory [Gb]
--------|--------|-----:|------------:|---------:|-----------:|---------:|------------:
EOF
  
    sort -g -k 15 tmp-table.md >> report-${1}.md
    cat << EOF  >> report-${1}.md
 
Table: \$c=${c}\$ sorted by memory


EOF

  done
  
  
else
  echo "nothing to plot for ${1}"
  exit 0
fi
