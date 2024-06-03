#!/bin/bash

for i in grep sed sort mawk; do
 if [ -z "$(which $i)" ]; then
  echo "error: $i not installed"
  exit 1
 fi
done

src="../src"
parsers="../src/parser/parser.c ../src/pdes/neutron_diffusion/parser.c ../src/pdes/neutron_sn/parser.c "
inits="../src/flow/init.c ../src/pdes/init.c ../src/pdes/thermal/init.c ../src/pdes/mechanical/init.c ../src/pdes/neutron_diffusion/init.c ../src/pdes/neutron_sn/init.c"
functions="../src/math/builtin_functions.c ../src/math/builtin_functionals.c ../src/math/builtin_vectorfunctions.c"

UPPER1=$(grep strcasecmp ${parsers}   | sed -r 's/[^"]*("[^"]*")?/ \1/g;s/" +"/\n/g;s/ *"//g' | sort | mawk '$1 in p{next}{p[$1];print}' | mawk '/^[A-Z]/')
UPPER2=$(grep keywords   ${parsers}   | sed -r 's/[^"]*("[^"]*")?/ \1/g;s/" +"/\n/g;s/ *"//g' | sort | mawk '$1 in p{next}{p[$1];print}' | mawk '/^[A-Z]/')
LOWER=$(grep strcasecmp  ${parsers}   | sed -r 's/[^"]*("[^"]*")?/ \1/g;s/" +"/\n/g;s/ *"//g' | sort | mawk '$1 in p{next}{p[$1];print}' | mawk '/^[a-z]/')
# VARS=$(grep variable     ${inits}     | grep -v "computing" | sed -r 's/[^"]*("[^"]*")?/ \1/g;s/" +"/\n/g;s/ *"//g'| sort | mawk '$1 in p{next}{p[$1];print}')
VARS=$(grep -e variable -e distribution_define -e distribution_init     ${inits}     | grep -v "computing" | sed -r 's/[^"]*("[^"]*")?/ \1/g;s/" +"/\n/g;s/ *"//g'| sort | mawk '$1 in p{next}{p[$1];print}')
FUNCS=$(grep builtin_    ${functions} | grep '}' | sed -r 's/[^"]*("[^"]*")?/ \1/g;s/" +"/\n/g;s/ *"//g' | sort | mawk '$1 in p{next}{p[$1];print}')
UPPER="${UPPER1} ${UPPER2}"

# echo
# echo "upper1"
# echo ${UPPER1}
# 
# echo
# echo "upper2"
# echo ${UPPER2}
# 
# echo
# echo "LOWER"
# echo ${LOWER}
# 
# echo
# echo "vars"
# echo ${VARS}
# 
# echo
# echo "FUNCS"
# echo ${FUNCS}
