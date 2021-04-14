#!/bin/bash

for i in grep awk sort uniq gcc; do
 if [ -z "$(which $i)" ]; then
  echo "error: ${i} not installed"
  exit 1
 fi
done


src=${1}
tag=${2}

if [ -z "${tag}" ]; then
  echo "usage: $0 src tag";
  exit
fi

# get the defines in feenox.h to get the defaults
dir=$(dirname ${src})
if [ -e "${dir}/feenox.h" ]; then
  grep '#define' ${dir}/feenox.h > defs.h
elif [ -e "../${dir}/feenox.h" ]; then
  grep '#define' ../${dir}/feenox.h > defs.h
elif [ -e "../../${dir}/feenox.h" ]; then
  grep '#define' ../../${dir}/feenox.h > defs.h
else
  touch defs.h
fi

kws=$(grep "///${tag}+" ${src} | awk '{print $1}' | awk -F+ '{print $2}' | sort | uniq)

for kw in ${kws}; do

  escapedkw=$(echo ${kw} | sed 's/_/\\_/g')

#   echo $kw > /dev/stderr

  # keyword
  echo "##  ${kw}"
  echo
#   echo -n '> '
  grep "///${tag}+${kw}+desc" ${src} | cut -d" " -f2-
  echo  
  echo  

  # usage
  usage=$(grep "///${tag}+${kw}+usage" ${src} | cut -d" " -f2-)
  if [ -n "${usage}" ]; then
    echo -n "\`${usage}\`"
#     echo "::: {.usage}"
#     echo "~~~{.feenox style=feenox}"
#     grep "///${tag}+${kw}+usage" ${src} | cut -d" " -f2- | xargs | tr @ \\n
#     echo "~~~"
#     echo ":::"
#     echo
  fi

  # math+figure
  math=$(grep "///${tag}+${kw}+math" ${src} | cut -d" " -f2-)
  if [ -n "${math}" ]; then
    echo " \$= \displaystyle ${math}\$"

#     echo
#     echo "::: {.math}"
#     echo "\$\$"
#     echo ${math}
#     echo "\$\$"
#     echo ":::"
#     echo
  fi
  echo

  range=$(grep "///${tag}+${kw}+plotx" ${src} | cut -d" " -f2-)
  if [[ -n "${range}" &&  -n "$(which pyxplot)" && -n "$(which feenox)" && -n "$(which pdf2svg)" ]]; then
    if [ ! -e "figures/${kw}.svg" ]; then
      mkdir -p figures
      cd figures
      min=$(echo ${range}       | awk '{printf $1}')
      max=$(echo ${range}       | awk '{printf $2}')
      step=$(echo ${range}      | awk '{printf $3}')
      minxtics=$(echo ${range}  | awk '{printf $4}')
      maxxtics=$(echo ${range}  | awk '{printf $5}')
      stepxtics=$(echo ${range} | awk '{printf $6}')
      minytics=$(echo ${range}  | awk '{printf $7}')
      maxytics=$(echo ${range}  | awk '{printf $8}')
      stepytics=$(echo ${range} | awk '{printf $9}')
      mxtics=$(echo ${range}    | awk '{printf $10}')
      mytics=$(echo ${range}    | awk '{printf $11}')
      cat << EOF > ${kw}.fee
f(x) := ${kw}(x)
PRINT_FUNCTION f MIN ${min} MAX ${max} STEP ${step}
EOF
      feenox ${kw}.fee > ${kw}.dat
      cat << EOF > ${kw}.ppl
set preamble "\usepackage{amsmath} \usepackage{amssymb}"
set width 12*unit(cm)
set size ratio 0.375
set axis x arrow nomirrored
set axis y arrow nomirrored
set grid
set nokey
EOF
      if [ ! -z "${minxtics}" ]; then
        echo "set xtics ${minxtics},${stepxtics},${maxxtics}" >> ${kw}.ppl
      fi
      if [ ! -z "${maxytics}" ]; then
        echo "set ytics ${minytics},${stepytics},${maxytics}" >> ${kw}.ppl
      fi
      if [ ! -z "${mxtics}" ]; then
        echo "set mxtics ${minxtics},${mxtics},${maxxtics}" >> ${kw}.ppl
      fi
      if [ ! -z "${mytics}" ]; then
        echo "set mytics ${minytics},${mytics},${maxytics}" >> ${kw}.ppl
      fi

      cat << EOF >> ${kw}.ppl
set xrange [${min}:${max}]
set xlabel "\$x\$"
set ylabel "\\texttt{${escapedkw}}\$(x)\$"
set terminal pdf
set output "${kw}.pdf"
plot "${kw}.dat" w l lw 3 color blue
set output "${kw}.png"
set terminal png
replot
EOF
      pyxplot ${kw}.ppl
      pdf2svg ${kw}.pdf ${kw}.svg
      rm -f ${kw}.fee
      rm -f ${kw}.ppl
      rm -f ${kw}.dat
      cd ..
    fi  

    echo
    echo "![${kw}](figures/${kw}.svg){width=90%}\ "
    echo 
  fi

  # detailed description
  echo
#   echo ${kw} 1>&2
  # el cut saca los tags especiales, el gcc permite usar los defines para documentar los defaults,
  # el primer sed transforma una arroba seguida de un newline en un newline
  # el segundo se es para poder poner links como https:/\/ (sin la barra del medio gcc piensa que es un comentario)
  grep "///${tag}+${kw}+detail" ${src} | cut -d" " -f2- | gcc -E -P -include defs.h - | sed 's/@$//' | sed 's_/\\/_//_'
  echo  

  # examples
  exs=$(grep ///${tag}+${kw}+example ${src} | cut -d" " -f2-)
#  exs=""
  i=0
  for ex in ${exs}; do
  
    if [ ! -e "examples/${ex}" ]; then
      echo "example ${ex} does not exist" > /dev/stderr
      exit 1
    fi
  
    i=$((${i} + 1))
    
    echo "### Example #${i}, ${ex}"
    echo
    echo "~~~{.feenox style=feenox}"
    cat examples/${ex}
    echo "~~~"

    # terminal
    cd examples
    echo
    echo "~~~{.terminal style=terminal}"
    sh="$(basename ${ex} .fee).sh"
    if [ -e "${sh}" ]; then
      k=1
      rm -f ${ex}.term
      n=$(wc -l < ${sh})
      while [ ${k} -le ${n} ]; do
       echo -n "\$ " >> ${ex}.term
       head -n ${k} ${sh} | tail -n1 > tmp
       chmod +x tmp
       cat tmp >> ${ex}.term
       script -aq -c ./tmp ${ex}.term 2>&1 | grep error: > errors
       if [ ! -z "$(cat errors)" ]; then
         echo "error: something happened on the way to heaven"
         cat ./tmp > /dev/error
         cat errors > /dev/error
         exit 1
       fi
       k=$((${k} + 1))
      done
      
      cat ${ex}.term | grep -v Script | sed s/^\\.\$/\$/ | sed /^$/d
      echo \$ 
      
      # figure
      ppl="$(basename ${ex} .fee).ppl"
      if [[ -e "${ppl}" &&  -n "$(which pyxplot)" && -n "$(which feenox)" && -n "$(which pdf2svg)" ]]; then
        pyxplot ${ppl}
        pdf2svg $(basename ${ex} .fee).pdf $(basename ${ex} .fee).svg 
      fi
      
    else
      echo "$ feenox ${ex}"
      feenox ${ex}
      echo "$"
    fi
    echo "~~~"
    echo
    cd ..
    

    # example figure
    svg="examples/$(basename ${ex} .fee).svg"
    if [ -e ${svg} ]; then
      echo "![${ex}](${svg}){width=90%}\ "
    fi


    echo
    echo
  done
done

echo 


rm -f examples/errors examples/tmp
