#!/bin/bash

rm -f README.md
for category in $(cat categories); do
 echo -n "# " >> README.md
 head -n1 ${category} >> README.md
 echo >> README.md

 for i in $(sed '1d' ${category}); do
  echo ${i}
  grep '#\.' ${i} | sed 's/#.//' | sed 's/^[ \t]*//' >> README.md
  echo >> README.md
  
  grep '#\$' ${i} | sed 's/#$//' | sed 's/^[ \t]*//' >> ${i}-post.sh
  bash ${i}-post.sh
#   rm -f ${i}-post.sh

  echo >> README.md
  echo '```{.feenox style=feenox}' >> README.md
  cat ${i} | grep -v '#\.' | grep -v '#\$' | grep -v '#>' >> README.md
  echo '```' >> README.md
  echo >> README.md

  grep '#>' ${i} | sed 's/#>//' | sed 's/^[ \t]*//' >> README.md
  echo >> README.md

 done
done
